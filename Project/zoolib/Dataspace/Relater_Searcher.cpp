/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#include "zoolib/Dataspace/Relater_Searcher.h"

#include "zoolib/Callable_PMF.h"
#include "zoolib/Log.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/QueryEngine/Expr_Rel_Search.h"
#include "zoolib/QueryEngine/ResultFromWalker.h"
#include "zoolib/QueryEngine/Transform_Search.h"
#include "zoolib/QueryEngine/Util_Strim_Result.h"
#include "zoolib/QueryEngine/Util_Strim_Walker.h"
#include "zoolib/QueryEngine/Visitor_DoMakeWalker.h"
#include "zoolib/QueryEngine/Walker_Rename.h"
#include "zoolib/QueryEngine/Walker_Result.h"

#include "zoolib/RelationalAlgebra/Transform_ConsolidateRenames.h"
#include "zoolib/RelationalAlgebra/Transform_DecomposeRestricts.h"
#include "zoolib/RelationalAlgebra/Transform_PushDownRestricts.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"

#include "zoolib/ValPred/Util_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ValPred/ValPred_Any.h" // For ValComparand_Const_Any
#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Do_GetNames.h"

namespace ZooLib {

namespace QE = QueryEngine;
namespace RA = RelationalAlgebra;

namespace Dataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

using namespace Util_STL;

// =================================================================================================
#pragma mark - Relater_Searcher::ClientQuery

class Relater_Searcher::DLink_ClientQuery_InPQuery
:	public DListLink<ClientQuery, DLink_ClientQuery_InPQuery, kDebug>
	{};

class Relater_Searcher::DLink_ClientQuery_NeedsWork
:	public DListLink<ClientQuery, DLink_ClientQuery_NeedsWork, kDebug>
	{};

class Relater_Searcher::ClientQuery
:	public DLink_ClientQuery_InPQuery
,	public DLink_ClientQuery_NeedsWork
	{
public:
	ClientQuery(int64 iRefcon, PQuery* iPQuery)
	:	fRefcon(iRefcon)
	,	fPQuery(iPQuery)
		{}

	int64 const fRefcon;
	PQuery* const fPQuery;
	};

// =================================================================================================
#pragma mark - Relater_Searcher::PQuery

class Relater_Searcher::DLink_PQuery_NeedsWork
:	public DListLink<PQuery, DLink_PQuery_NeedsWork, kDebug>
	{};

class Relater_Searcher::PQuery
:	public DLink_PQuery_NeedsWork
	{
public:
	PQuery(ZP<RA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	const ZP<RA::Expr_Rel> fRel;
	DListHead<DLink_ClientQuery_InPQuery> fClientQuery_InPQuery;
	set<PRegSearch*> fPRegSearch_Used;
	ZP<QE::Result> fResult;
	ZP<ResultDeltas> fResultDeltas;
	};

// =================================================================================================
#pragma mark - Relater_Searcher::PRegSearch

class Relater_Searcher::DLink_PRegSearch_NeedsWork
:	public DListLink<PRegSearch, DLink_PRegSearch_NeedsWork, kDebug>
	{};

class Relater_Searcher::PRegSearch
:	public DLink_PRegSearch_NeedsWork
	{
public:
	PRegSearch() {}

	int64 fRefconInSearcher;
	SearchSpec fSearchSpec;
	set<PQuery*> fPQuery_Using;
	ZP<QE::Result> fResult;
	};

// =================================================================================================
#pragma mark - DoReplaceBoundNames (anonymous)

namespace { // anonymous

class DoReplaceBoundNames
:	public virtual Visitor_Expr_Op_Do_Transform_T<Expr_Bool>
,	public virtual Visitor_Expr_Bool_ValPred
	{
public:
	DoReplaceBoundNames(const vector<ColName>& iBoundNames,
		const size_t* iBoundOffsets,
		const Val_Any* iVals)
	:	fBoundNames(iBoundNames)
	,	fBoundOffsets(iBoundOffsets)
	,	fVals(iVals)
		{}

	ZP<ValComparand> pUpdated(const ZP<ValComparand>& iVC)
		{
		if (ZP<ValComparand_Name> asName = iVC.DynamicCast<ValComparand_Name>())
			{
			// We could have a different kind of ValComparand, which holds the name's offset
			// and for which we sub a value without needing to do the lookup every time.
			if (ZQ<size_t> offset = sQFindSorted(fBoundNames, asName->GetName()))
				return new ValComparand_Const_Any(fVals[fBoundOffsets[*offset]]);
			}
		return null;
		}

	virtual void Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred>& iExpr)
		{
		const ValPred& theVP = iExpr->GetValPred();

		ZP<Expr_Bool> theResult;

		if (ZP<ValComparand> newLHS = pUpdated(theVP.GetLHS()))
			{
			if (ZP<ValComparand> newRHS = pUpdated(theVP.GetRHS()))
				theResult = new Expr_Bool_ValPred(ValPred(newLHS, theVP.GetComparator(), newRHS));
			else
				theResult = new Expr_Bool_ValPred(ValPred(newLHS, theVP.GetComparator(), theVP.GetRHS()));
			}
		else if (ZP<ValComparand> newRHS = pUpdated(theVP.GetRHS()))
			{
			theResult = new Expr_Bool_ValPred(ValPred(theVP.GetLHS(), theVP.GetComparator(), newRHS));
			}
		else
			{
			theResult = iExpr;
			}

		this->pSetResult(theResult);
		}

private:
	const vector<ColName>& fBoundNames;
	const size_t* fBoundOffsets;
	const Val_Any* fVals;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - Relater_Searcher::Walker_Bingo

class Relater_Searcher::Walker_Bingo
:	public QE::Walker
	{
public:
	Walker_Bingo(ZP<Relater_Searcher> iRelater,
		PQuery* iPQuery,
		const RelHead& iBoundNames,
		const ConcreteHead& iConcreteHead,
		ZP<Expr_Bool> iRestriction)
	:	fRelater(iRelater)
	,	fPQuery(iPQuery)
	,	fPRegSearch(nullptr)
	,	fNextRow(0)
	,	fBoundNames(iBoundNames.begin(), iBoundNames.end())
	,	fConcreteHead(iConcreteHead)
	,	fRestriction(iRestriction)
		{}

	virtual ~Walker_Bingo()
		{}

// From ZCounted via QE::Walker
	virtual void Finalize()
		{ fRelater->pFinalize(this); }

// From QE::Walker
	virtual void Rewind()
		{
		this->Called_Rewind();
		fRelater->pRewind(this);
		}

	virtual ZP<QE::Walker> Prime(const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{ return fRelater->pPrime(this, iOffsets, oOffsets, ioBaseOffset); }

	virtual bool QReadInc(Val_Any* ioResults)
		{
		this->Called_QReadInc();
		return fRelater->pQReadInc(this, ioResults);
		}

	const ZP<Relater_Searcher> fRelater;
	PQuery* fPQuery;
	PRegSearch* fPRegSearch;
	size_t fNextRow;

	const vector<ColName> fBoundNames;
	ConcreteHead fConcreteHead;
	ZP<Expr_Bool> fRestriction;
	size_t fBaseOffset;
	vector<size_t> fBoundOffsets;
	};

// =================================================================================================
#pragma mark - Relater_Searcher::Visitor_DoMakeWalker

class Relater_Searcher::Visitor_DoMakeWalker
:	public virtual QE::Visitor_DoMakeWalker
,	public virtual RA::Visitor_Expr_Rel_Concrete
,	public virtual QE::Visitor_Expr_Rel_Search
	{
public:
	Visitor_DoMakeWalker(ZP<Relater_Searcher> iSearcher, PQuery* iPQuery)
	:	fSearcher(iSearcher)
	,	fPQuery(iPQuery)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZP<RA::Expr_Rel_Concrete>& iExpr)
		{
		// Do we need to do something with embed here as well? To gather the bound names
		// that will be passed off?
		ZP<QE::Walker> theWalker = fSearcher->pMakeWalker(fPQuery,
			RelHead(), SearchSpec(iExpr->GetConcreteHead(), null));

		this->pSetResult(theWalker);
		}

	virtual void Visit_Expr_Rel_Search(const ZP<QE::Expr_Rel_Search>& iExpr)
		{
		// Get rename and optional into a ConcreteHead, and if needed a stack of Renames.
		vector<pair<string8,string8> > finalRename;
		ConcreteHead theConcreteHead;
		foreacha (entry, iExpr->GetRename())
			{
			const string8& source = entry.first;
			if (source != entry.second)
				sPushBack(finalRename, entry);

			theConcreteHead[source] = not sContains(iExpr->GetRelHead_Optional(), source);
			}

		ZP<QE::Walker> theWalker = fSearcher->pMakeWalker(fPQuery,
			iExpr->GetRelHead_Bound(),
			SearchSpec(theConcreteHead, iExpr->GetExpr_Bool()));

		foreacha (entry, finalRename)
			theWalker = new QE::Walker_Rename(theWalker, entry.second, entry.first);

		this->pSetResult(theWalker);
		}

	ZP<Relater_Searcher> const fSearcher;
	PQuery* const fPQuery;
	};

// =================================================================================================
#pragma mark - sTransform_PushDownRestricts_IntoSearch

namespace { // anonymous

class Transform_PushDownRestricts_IntoSearch
:	public virtual RA::Transform_PushDownRestricts
,	public virtual QE::Visitor_Expr_Rel_Search
	{
public:
	virtual void Visit_Expr_Rel_Search(const ZP<QE::Expr_Rel_Search>& iExpr)
		{
		const RA::Rename theRename = iExpr->GetRename();
		const RA::Rename theRenameInverted = RA::sInverted(theRename);

		const RelHead generatedFrom = RA::sNamesFrom(theRename);
		const RelHead generatedTo = RA::sNamesTo(iExpr->GetRename());

		const RelHead optionalColumns = iExpr->GetRelHead_Optional();

		const RelHead boundTo = iExpr->GetRelHead_Bound();
		const RelHead boundFrom = RA::sRenamed(theRenameInverted, boundTo);

		// Start with the extant search expression.
		ZP<Expr_Bool> result = iExpr->GetExpr_Bool();

		const RelHead available = fRelHead | boundTo | generatedTo;

		RelHead newBound;

		// Go through each active restriction.
		foreacha (theRestrictPtr, fRestricts)
			{
			Restrict& theRestrict = *theRestrictPtr;

			// Which names does the restriction reference?
			const RelHead exprNames = sGetNames(theRestrict.fExpr_Bool);

			// Of those, which are part of available?
			const RelHead intersection = exprNames & available;

			if (intersection.size())
				{
				++theRestrict.fCountTouching;
				if (intersection.size() == exprNames.size())
					{
					++theRestrict.fCountSubsuming;
					result &= Util_Expr_Bool::sRenamed(theRenameInverted, theRestrict.fExpr_Bool);
					newBound |= exprNames & fRelHead;
					}
				}
			}

		fRelHead |= generatedTo;

		ZP<QE::Expr_Rel_Search> theSearch = new QE::Expr_Rel_Search(
			newBound | boundTo,
			iExpr->GetRename(),
			iExpr->GetRelHead_Optional(),
			result);

		this->pSetResult(theSearch);
		}
	};

} // anonymous namespace

ZP<Expr_Rel> sTransform_PushDownRestricts_IntoSearch(const ZP<Expr_Rel>& iRel)
	{ return Transform_PushDownRestricts_IntoSearch().Do(iRel); }

// =================================================================================================
#pragma mark - Relater_Searcher

Relater_Searcher::Relater_Searcher(ZP<Searcher> iSearcher)
:	fSearcher(iSearcher)
,	fChangeCount(0)
,	fNextRefcon(1)
	{}

Relater_Searcher::~Relater_Searcher()
	{
	for (DListEraser<PRegSearch,DLink_PRegSearch_NeedsWork> eraser = fPRegSearch_NeedsWork;
		eraser; eraser.Advance())
		{}

	for (DListEraser<ClientQuery,DLink_ClientQuery_NeedsWork> eraser = fClientQuery_NeedsWork;
		eraser; eraser.Advance())
		{}
	}

void Relater_Searcher::Initialize()
	{
	Relater::Initialize();
	fSearcher->SetCallable_SearcherResultsAvailable(
		sCallable(sWeakRef(this), &Relater_Searcher::pSearcherResultsAvailable));
	}

bool Relater_Searcher::Intersects(const RelHead& iRelHead)
	{ return true; }

void Relater_Searcher::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZAcqMtx acq(fMtx);

	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		ZP<Expr_Rel> theRel = iAdded->GetRel();

		if (true)
			{
			// This branch is much faster. sTransform_Search doesn't do a great job of pulling
			// restricts into itself.
			theRel = QE::sTransform_Search(theRel);
			theRel = RA::Transform_DecomposeRestricts().Do(theRel);
			theRel = Transform_PushDownRestricts_IntoSearch().Do(theRel);
			}
		else
			{
			theRel = RA::Transform_DecomposeRestricts().Do(theRel);
			theRel = RA::Transform_PushDownRestricts().Do(theRel);
			theRel = QE::sTransform_Search(theRel);
			}

		const pair<Map_Rel_PQuery::iterator,bool> iterPQueryPair =
			fMap_Rel_PQuery.insert(make_pair(theRel, PQuery(theRel)));

		const Map_Rel_PQuery::iterator& iterPQuery = iterPQueryPair.first;
		PQuery* thePQuery = &iterPQuery->second;

		const int64 theRefcon = iAdded->GetRefcon();

		const pair<map<int64,ClientQuery>::iterator,bool> iterClientQueryPair =
			fMap_Refcon_ClientQuery.insert(
				make_pair(theRefcon, ClientQuery(theRefcon, thePQuery)));

		ZAssert(iterClientQueryPair.second);

		ClientQuery* theClientQuery = &iterClientQueryPair.first->second;
		sInsertBackMust(thePQuery->fClientQuery_InPQuery, theClientQuery);

		if (iterPQueryPair.second)
			{
			// It's a new PQuery, so we'll need to work on it.
			sInsertBackMust(fPQuery_NeedsWork, thePQuery);
			}
		else
			{
			// It's an existing PQuery, so the ClientQuery will need to be worked on.
			sInsertBackMust(fClientQuery_NeedsWork, theClientQuery);
			}
		}

	while (iRemovedCount--)
		{
		int64 theRefcon = *iRemoved++;

		map<int64, ClientQuery>::iterator iterClientQuery =
			fMap_Refcon_ClientQuery.find(theRefcon);

		ZAssertStop(kDebug, iterClientQuery != fMap_Refcon_ClientQuery.end());

		ClientQuery* theClientQuery = &iterClientQuery->second;

		PQuery* thePQuery = theClientQuery->fPQuery;
		sEraseMust(thePQuery->fClientQuery_InPQuery, theClientQuery);
		if (sIsEmpty(thePQuery->fClientQuery_InPQuery))
			{
			// Detach from any depended-upon PRegSearch
			foreacha (thePRegSearchStar, thePQuery->fPRegSearch_Used)
				{
				sEraseMust(kDebug, thePRegSearchStar->fPQuery_Using, thePQuery);
				if (sIsEmpty(thePRegSearchStar->fPQuery_Using))
					sQInsertBack(fPRegSearch_NeedsWork, thePRegSearchStar);
				}
			thePQuery->fPRegSearch_Used.clear();

			sQErase(fPQuery_NeedsWork, thePQuery);
			sEraseMust(kDebug, fMap_Rel_PQuery, thePQuery->fRel);
			}

		sQErase(fClientQuery_NeedsWork, theClientQuery);
		fMap_Refcon_ClientQuery.erase(iterClientQuery);
		}

	if (sNotEmpty(fClientQuery_NeedsWork) || sNotEmpty(fPQuery_NeedsWork))
		{
		ZRelMtx rel(fMtx);
		Relater::pTrigger_RelaterResultsAvailable();
		}
	}

// =================================================================================================

void Relater_Searcher::CollectResults(vector<QueryResult>& oChanged, int64& oChangeCount)
	{
	Relater::pCalled_RelaterCollectResults();

	this->pCollectResultsFromSearcher();

	ZAcqMtx acq(fMtx);

	oChangeCount = fChangeCount;

	for (DListEraser<PQuery,DLink_PQuery_NeedsWork> eraser = fPQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		PQuery* thePQuery = eraser.Current();

		{
		ZRelMtx rel(fMtx);

		ZP<QE::Walker> theWalker = Visitor_DoMakeWalker(this, thePQuery).Do(thePQuery->fRel);

		const double start = Time::sSystem();
		ZP<Result> priorResult = thePQuery->fResult;
		thePQuery->fResult = QE::sResultFromWalker(theWalker);
		const double elapsed = Time::sSystem() - start;

		if (elapsed > 0.1)
			{
			if (ZLOGPF(w, eDebug))
				{
				w << "\nSlow Query " << elapsed * 1e3 << "ms: ";
				w << thePQuery->fRel << "\n";
				sToStrim(thePQuery->fResult, w);
				sDumpWalkers(theWalker, w);
				}
			}

		thePQuery->fResultDeltas.Clear();
		if (priorResult)
			{
			ZAssert(priorResult->GetRelHead() == thePQuery->fResult->GetRelHead());
			if (priorResult->Count() == thePQuery->fResult->Count())
				{
				ZP<ResultDeltas> theDeltas = new ResultDeltas;
				const size_t rowCount = priorResult->Count();
				const size_t colCount = priorResult->GetRelHead().size();

				for (size_t rr = 0; rr < rowCount; ++rr)
					{
					const Val_Any* priorVals = priorResult->GetValsAt(rr);
					const Val_Any* currVals = thePQuery->fResult->GetValsAt(rr);
					for (size_t cc = 0; cc < colCount; ++cc)
						{
						if (priorVals[cc] != currVals[cc])
							{
							for (size_t cc = 0; cc < colCount; ++cc)
								theDeltas->fPackedRows.push_back(currVals[cc]);
							theDeltas->fMapping.push_back(rr);
							break;
							}
						}
					}
				thePQuery->fResultDeltas = theDeltas;
				}
			}
		}

		for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
			iter = thePQuery->fClientQuery_InPQuery; iter; iter.Advance())
			{ sQInsertBack(fClientQuery_NeedsWork, iter.Current()); }
		}

	oChanged.clear();

	for (DListEraser<ClientQuery,DLink_ClientQuery_NeedsWork> eraser = fClientQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		ClientQuery* theClientQuery = eraser.Current();
		PQuery* thePQuery = theClientQuery->fPQuery;
		oChanged.push_back(QueryResult(theClientQuery->fRefcon, thePQuery->fResult, thePQuery->fResultDeltas));
		}

	// Remove any unused PRegSearches
	vector<int64> toRemove;
	for (DListEraser<PRegSearch,DLink_PRegSearch_NeedsWork> eraser = fPRegSearch_NeedsWork;
		eraser; eraser.Advance())
		{
		PRegSearch* thePRegSearchStar = eraser.Current();
		if (sIsEmpty(thePRegSearchStar->fPQuery_Using))
			{
			toRemove.push_back(thePRegSearchStar->fRefconInSearcher);
			sEraseMust(kDebug, fMap_SearchSpec_PRegSearchStar, thePRegSearchStar->fSearchSpec);
			sEraseMust(kDebug, fMap_Refcon_PRegSearch, thePRegSearchStar->fRefconInSearcher);
			}
		}

	ZRelMtx rel(fMtx);
	if (sNotEmpty(toRemove))
		fSearcher->ModifyRegistrations(nullptr, 0, &toRemove[0], toRemove.size());
	}

void Relater_Searcher::ForceUpdate()
	{ Relater::pTrigger_RelaterResultsAvailable(); }

bool Relater_Searcher::pCollectResultsFromSearcher()
	{
	vector<SearchResult> theSearchResults;
	int64 theChangeCount;
	fSearcher->CollectResults(theSearchResults, theChangeCount);

	ZAcqMtx acq(fMtx);
	ZAssert(fChangeCount <= theChangeCount);
	fChangeCount = theChangeCount;
	for (vector<SearchResult>::const_iterator ii = theSearchResults.begin();
		ii != theSearchResults.end(); ++ii)
		{
		if (PRegSearch* thePRS = sPMut(fMap_Refcon_PRegSearch, ii->GetRefcon()))
			{
			thePRS->fResult = ii->GetResult();
			foreacha (thePQuery, thePRS->fPQuery_Using)
				{
				sQInsertBack(fPQuery_NeedsWork, thePQuery);
//##				thePQuery->fResult.Clear();
				}
			}
		}
	fCnd.Broadcast();
	return sNotEmpty(theSearchResults);
	}

void Relater_Searcher::pSearcherResultsAvailable(ZP<Searcher>)
	{
	Relater::pTrigger_RelaterResultsAvailable();
	ZAcqMtx acq(fMtx);
	fCnd.Broadcast();
	}

void Relater_Searcher::pFinalize(Walker_Bingo* iWalker_Bingo)
	{
	if (iWalker_Bingo->FinishFinalize())
		delete iWalker_Bingo;
	}

void Relater_Searcher::pRewind(ZP<Walker_Bingo> iWalker_Bingo)
	{
	iWalker_Bingo->fPRegSearch = nullptr;
	iWalker_Bingo->fNextRow = 0;
	}

ZP<QE::Walker> Relater_Searcher::pPrime(ZP<Walker_Bingo> iWalker_Bingo,
	const std::map<string8,size_t>& iOffsets,
	std::map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	// Remember the offsets from which we're fetching bound values.
	foreacha (entry, iWalker_Bingo->fBoundNames)
		sPushBack(iWalker_Bingo->fBoundOffsets, sGetMust(iOffsets, entry));

	// Where we're going to start putting our output.
	iWalker_Bingo->fBaseOffset = ioBaseOffset;
	foreacha (entry, iWalker_Bingo->fConcreteHead)
		{
		// Hmm, we should watch for *ii being in fBoundNames.
		oOffsets[entry.first] = ioBaseOffset++;
		}
	return iWalker_Bingo;
	}

bool Relater_Searcher::pQReadInc(ZP<Walker_Bingo> iWalker_Bingo, Val_Any* ioResults)
	{
	ZAcqMtx acq(fMtx);
	if (not iWalker_Bingo->fPRegSearch)
		{
		ZP<Expr_Bool> theRestriction = iWalker_Bingo->fRestriction;

		if (sNotEmpty(iWalker_Bingo->fBoundNames))
			{
			// fBoundNames ends up as a sorted vector.
			theRestriction = DoReplaceBoundNames(
				iWalker_Bingo->fBoundNames, &iWalker_Bingo->fBoundOffsets[0], ioResults)
				.Do(theRestriction);
			}

		const SearchSpec theSearchSpec(iWalker_Bingo->fConcreteHead, theRestriction);

		PRegSearch* thePRegSearchStar = nullptr;

		Map_SearchSpec_PRegSearchStar::iterator iterMap =
			fMap_SearchSpec_PRegSearchStar.lower_bound(theSearchSpec);

		if (iterMap != fMap_SearchSpec_PRegSearchStar.end() && iterMap->first == theSearchSpec)
			{
			thePRegSearchStar = iterMap->second;
			}
		else
			{
			pair<Map_Refcon_PRegSearch::iterator,bool> iterPair =
				fMap_Refcon_PRegSearch.insert(make_pair(fNextRefcon++, PRegSearch()));

			ZAssert(iterPair.second);

			thePRegSearchStar = &iterPair.first->second;

			fMap_SearchSpec_PRegSearchStar.insert(iterMap, make_pair(theSearchSpec, thePRegSearchStar));

			thePRegSearchStar->fSearchSpec = theSearchSpec;
			thePRegSearchStar->fRefconInSearcher = iterPair.first->first;

			const AddedSearch theAS(thePRegSearchStar->fRefconInSearcher, theSearchSpec);

			ZRelMtx rel(fMtx);

			fSearcher->ModifyRegistrations(&theAS, 1, nullptr, 0);

			for (;;)
				{
				this->pCollectResultsFromSearcher();

				ZAcqMtx acq(fMtx);

				if (thePRegSearchStar->fResult)
					break;

				fCnd.Wait(fMtx);
				}
			}

		// We may end up using the same PRegSearch to support a single PQuery (e.g. a self-join),
		// so we sQInsert rather than sInsertMust.
		PQuery* thePQuery = iWalker_Bingo->fPQuery;
		sQInsert(thePRegSearchStar->fPQuery_Using, thePQuery);
		sQInsert(thePQuery->fPRegSearch_Used, thePRegSearchStar);
		iWalker_Bingo->fPRegSearch = thePRegSearchStar;

		ZAssert(thePRegSearchStar->fResult);
		}

	ZP<Result> theResult = iWalker_Bingo->fPRegSearch->fResult;

	ZAssert(theResult->GetRelHead() == sRelHead(iWalker_Bingo->fConcreteHead));

	if (iWalker_Bingo->fNextRow >= theResult->Count())
		return false;

	const Val_Any* theRows = theResult->GetValsAt(iWalker_Bingo->fNextRow++);

	for (size_t xx = 0,
			colCount = iWalker_Bingo->fConcreteHead.size(),
			theBaseOffset = iWalker_Bingo->fBaseOffset;
		xx < colCount;
		++xx)
		{
		ioResults[theBaseOffset + xx] = theRows[xx];
		}

	return true;
	}

ZP<QE::Walker> Relater_Searcher::pMakeWalker(PQuery* iPQuery,
	const RelHead& iRelHead_Bound,
	const SearchSpec& iSearchSpec)
	{
	ZAcqMtx acq(fMtx);
	ZP<Walker_Bingo> theWalker = new Walker_Bingo(this,
		iPQuery, iRelHead_Bound, iSearchSpec.GetConcreteHead(), iSearchSpec.GetRestriction());
	return theWalker;
	}

} // namespace Dataspace
} // namespace ZooLib
