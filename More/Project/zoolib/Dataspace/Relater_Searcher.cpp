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

#include "zoolib/Callable_PMF.h"
#include "zoolib/Log.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zooLib/Dataspace/Relater_Searcher.h"

#include "zoolib/QueryEngine/ResultFromWalker.h"
#include "zoolib/QueryEngine/Transform_Search.h"
#include "zoolib/QueryEngine/Visitor_DoMakeWalker.h"
//#include "zoolib/QueryEngine/Walker_Project.h"
#include "zoolib/QueryEngine/Walker_Result.h"
#include "zoolib/QueryEngine/Walker_Rename.h"
//#include "zoolib/QueryEngine/Walker_Restrict.h"
#include "zoolib/QueryEngine/Util_Strim_Result.h"

#include "zoolib/RelationalAlgebra/GetRelHead.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/RelationalAlgebra/Transform_ConsolidateRenames.h"
#include "zoolib/RelationalAlgebra/Transform_DecomposeRestricts.h"
#include "zoolib/RelationalAlgebra/Transform_PushDownRestricts.h"

#include "zoolib/ValPred/Util_Strim_Expr_Bool_ValPred.h"
#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Any_ToStrim.h"
#include "zoolib/ValPred/ValPred_Any.h" // For ValComparand_Const_Any

namespace ZooLib {
namespace Dataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

namespace QE = QueryEngine;
namespace RA = RelationalAlgebra;

using namespace Util_STL;

// =================================================================================================
#pragma mark -
#pragma mark Relater_Searcher::ClientQuery

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
#pragma mark -
#pragma mark Relater_Searcher::PQuery

class Relater_Searcher::DLink_PQuery_NeedsWork
:	public DListLink<PQuery, DLink_PQuery_NeedsWork, kDebug>
	{};

class Relater_Searcher::PQuery
:	public DLink_PQuery_NeedsWork
	{
public:
	PQuery(ZRef<RA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	const ZRef<RA::Expr_Rel> fRel;
	DListHead<DLink_ClientQuery_InPQuery> fClientQuery_InPQuery;
	set<PRegSearch*> fPRegSearch_Used;
	ZRef<QE::Result> fResult;
	};

// =================================================================================================
#pragma mark -
#pragma mark Relater_Searcher::PRegSearch

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
	ZRef<QE::Result> fResult;
	};

// =================================================================================================
#pragma mark -
#pragma mark DoReplaceBoundNames (anonymous)

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

	ZRef<ValComparand> pUpdated(const ZRef<ValComparand>& iVC)
		{
		if (ZRef<ValComparand_Name> asName = iVC.DynamicCast<ValComparand_Name>())
			{
			// We could have a different kind of ValComparand, which holds the name's offset
			// and for which we sub a value without needing to do the lookup every time.
			if (ZQ<size_t> offset = sQFindSorted(fBoundNames, asName->GetName()))
				return new ValComparand_Const_Any(fVals[fBoundOffsets[*offset]]);
			}
		return iVC;
		}

	virtual void Visit_Expr_Bool_ValPred(const ZRef<Expr_Bool_ValPred>& iExpr)
		{
		const ValPred& theValPred = iExpr->GetValPred();

		ZRef<Expr_Bool> theResult = new Expr_Bool_ValPred(
			ValPred(
				pUpdated(theValPred.GetLHS()),
				theValPred.GetComparator(),
				pUpdated(theValPred.GetRHS())));

		this->pSetResult(theResult);
		}

private:
	const vector<ColName>& fBoundNames;
	const size_t* fBoundOffsets;
	const Val_Any* fVals;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Relater_Searcher::Walker_Bingo

class Relater_Searcher::Walker_Bingo
:	public QE::Walker
	{
public:
	Walker_Bingo(ZRef<Relater_Searcher> iRelater,
		PQuery* iPQuery,
		const RelHead& iBoundNames,
		const ConcreteHead& iConcreteHead,
		ZRef<Expr_Bool> iRestriction)
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
		{ fRelater->pRewind(this); }

	virtual ZRef<QE::Walker> Prime(const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{ return fRelater->pPrime(this, iOffsets, oOffsets, ioBaseOffset); }

	virtual bool QReadInc(Val_Any* ioResults)
		{ return fRelater->pQReadInc(this, ioResults); }

	const ZRef<Relater_Searcher> fRelater;
	PQuery* fPQuery;
	PRegSearch* fPRegSearch;
	size_t fNextRow;

	const vector<ColName> fBoundNames;
	ConcreteHead fConcreteHead;
	ZRef<Expr_Bool> fRestriction;
	size_t fBaseOffset;
	vector<size_t> fBoundOffsets;
	};

// =================================================================================================
#pragma mark -
#pragma mark Relater_Searcher::Visitor_DoMakeWalker

class Relater_Searcher::Visitor_DoMakeWalker
:	public virtual QE::Visitor_DoMakeWalker
,	public virtual RA::Visitor_Expr_Rel_Concrete
,	public virtual QE::Visitor_Expr_Rel_Search
	{
	typedef QE::Visitor_DoMakeWalker inherited;
public:
	Visitor_DoMakeWalker(ZRef<Relater_Searcher> iSearcher, PQuery* iPQuery)
	:	fSearcher(iSearcher)
	,	fPQuery(iPQuery)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<RA::Expr_Rel_Concrete>& iExpr)
		{
		// Do we need to do something with embed here as well? To gather the bound names
		// that will be passed off?
		ZRef<QueryEngine::Walker> theWalker = fSearcher->pMakeWalker(fPQuery,
			RelHead(), SearchSpec(iExpr->GetConcreteHead(), null));

		this->pSetResult(theWalker);
		}

	virtual void Visit_Expr_Rel_Search(const ZRef<QE::Expr_Rel_Search>& iExpr)
		{
		if (ZLOGF(w, eDebug+1))
			w << "Try handling:\n" << ZRef<Expr_Rel>(iExpr);

		// Get rename and optional into a ConcreteHead, and if needed a stack of Renames.
		vector<pair<string8,string8> > finalRename;
		ConcreteHead theConcreteHead;
		foreachi (iter, iExpr->GetRename())
			{
			const string8& source = iter->first;
			if (source != iter->second)
				sPushBack(finalRename, *iter);

			theConcreteHead[source] = not sContains(iExpr->GetRelHead_Optional(), source);
			}

		ZRef<QueryEngine::Walker> theWalker = fSearcher->pMakeWalker(fPQuery,
			iExpr->GetRelHead_Bound(),
			SearchSpec(theConcreteHead, iExpr->GetExpr_Bool()));

		foreachi (iter, finalRename)
			theWalker = new QueryEngine::Walker_Rename(theWalker, iter->second, iter->first);

		this->pSetResult(theWalker);
		}

	ZRef<Relater_Searcher> const fSearcher;
	PQuery* const fPQuery;
	};

// =================================================================================================
#pragma mark -
#pragma mark Relater_Searcher

Relater_Searcher::Relater_Searcher(ZRef<Searcher> iSearcher)
:	fSearcher(iSearcher)
,	fNextRefcon(1)
	{}

Relater_Searcher::~Relater_Searcher()
	{
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
	ZGuardMtxR guard(fMtxR);

	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		ZRef<RA::Expr_Rel> theRel = iAdded->GetRel();

//		theRel = RelationalAlgebra::Transform_DecomposeRestricts().Do(theRel);
//
//		// This next one needs some work doing with Embeds
//		theRel = RelationalAlgebra::Transform_PushDownRestricts().Do(theRel);
//
		// As does this one:
		theRel = QueryEngine::sTransform_Search(theRel);

		if (ZLOGF(w, eDebug + 0))
			{
			w << "\nBefore:\n" << iAdded->GetRel();
			w << "\nAfter:\n" << theRel;
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
			foreachi (iterPRegSearch, thePQuery->fPRegSearch_Used)
				{
				PRegSearch* thePRegSearch = *iterPRegSearch;
				sEraseMust(kDebug, thePRegSearch->fPQuery_Using, thePQuery);
				if (sIsEmpty(thePRegSearch->fPQuery_Using))
					sQInsertBack(fPRegSearch_NeedsWork, thePRegSearch);
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
		guard.Release();
		Relater::pTrigger_RelaterResultsAvailable();
		}
	}

void Relater_Searcher::CollectResults(vector<QueryResult>& oChanged)
	{
	Relater::pCalled_RelaterCollectResults();

	this->pCollectResultsFromSearcher();

	ZGuardMtxR guard(fMtxR);

	for (DListEraser<PQuery,DLink_PQuery_NeedsWork> eraser = fPQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		PQuery* thePQuery = eraser.Current();

		guard.Release();

		ZAssert(not thePQuery->fResult);

		ZRef<QE::Walker> theWalker = Visitor_DoMakeWalker(this, thePQuery).Do(thePQuery->fRel);

		const double start = Time::sSystem();
		thePQuery->fResult = QE::sResultFromWalker(theWalker);
		const double elapsed = Time::sSystem() - start;

//##		if (elapsed > 0.01)
			{
			if (ZLOGF(w, eDebug))
				{
				w << "Query, " << elapsed * 1e3 << "ms\n";
				w << thePQuery->fRel << "\n";
				sToStrim(thePQuery->fResult, w);
				}
			}

		guard.Acquire();

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
		oChanged.push_back(QueryResult(theClientQuery->fRefcon, thePQuery->fResult));
		}

	// Remove any unused PRegSearches
	vector<int64> toRemove;
	for (DListEraser<PRegSearch,DLink_PRegSearch_NeedsWork> eraser = fPRegSearch_NeedsWork;
		eraser; eraser.Advance())
		{
		PRegSearch* thePRegSearch = eraser.Current();
		if (sIsEmpty(thePRegSearch->fPQuery_Using))
			{
			toRemove.push_back(thePRegSearch->fRefconInSearcher);
			sEraseMust(kDebug, fMap_SearchSpec_PRegSearchStar, thePRegSearch->fSearchSpec);
			sEraseMust(kDebug, fMap_Refcon_PRegSearch, thePRegSearch->fRefconInSearcher);
			}
		}
	guard.Release();

	if (sNotEmpty(toRemove))
		fSearcher->ModifyRegistrations(nullptr, 0, &toRemove[0], toRemove.size());
	}

void Relater_Searcher::ForceUpdate()
	{ Relater::pTrigger_RelaterResultsAvailable(); }

bool Relater_Searcher::pCollectResultsFromSearcher()
	{
	vector<SearchResult> theSearchResults;
	fSearcher->CollectResults(theSearchResults);

	ZAcqMtxR acq(fMtxR);
	for (vector<SearchResult>::const_iterator ii = theSearchResults.begin();
		ii != theSearchResults.end(); ++ii)
		{
		if (PRegSearch* thePRS = sPMut(fMap_Refcon_PRegSearch, ii->GetRefcon()))
			{
			thePRS->fResult = ii->GetResult();
			foreacha (thePQuery, thePRS->fPQuery_Using)
				{
				sQInsertBack(fPQuery_NeedsWork, thePQuery);
				thePQuery->fResult.Clear();
				}
			}
		}
	fCnd.Broadcast();
	return sNotEmpty(theSearchResults);
	}

void Relater_Searcher::pSearcherResultsAvailable(ZRef<Searcher>)
	{
	// *1* We must call pCollectResultsFromSearcher because we're possibly
	// waiting on an fResult field at *2* below.
	if (this->pCollectResultsFromSearcher())
		Relater::pTrigger_RelaterResultsAvailable();
	}

void Relater_Searcher::pFinalize(Walker_Bingo* iWalker_Bingo)
	{
	if (iWalker_Bingo->FinishFinalize())
		delete iWalker_Bingo;
	}

void Relater_Searcher::pRewind(ZRef<Walker_Bingo> iWalker_Bingo)
	{
	iWalker_Bingo->fPRegSearch = nullptr;
	iWalker_Bingo->fNextRow = 0;
	}

ZRef<QE::Walker> Relater_Searcher::pPrime(ZRef<Walker_Bingo> iWalker_Bingo,
	const std::map<string8,size_t>& iOffsets,
	std::map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	// Remember the offsets from which we're fetching bound values.
	foreachi (ii, iWalker_Bingo->fBoundNames)
		sPushBack(iWalker_Bingo->fBoundOffsets, sGetMust(iOffsets, *ii));

	// Where we're going to start putting our output.
	iWalker_Bingo->fBaseOffset = ioBaseOffset;
	foreachi (ii, iWalker_Bingo->fConcreteHead)
		{
		// Hmm, we should watch for *ii being in fBoundNames.
		oOffsets[ii->first] = ioBaseOffset++;
		}
	return iWalker_Bingo;
	}

bool Relater_Searcher::pQReadInc(ZRef<Walker_Bingo> iWalker_Bingo, Val_Any* ioResults)
	{
	ZGuardMtxR guard(fMtxR);
	if (not iWalker_Bingo->fPRegSearch)
		{
		ZRef<Expr_Bool> theRestriction = iWalker_Bingo->fRestriction;

		if (sNotEmpty(iWalker_Bingo->fBoundNames))
			{
			// fBoundNames ends up as a sorted vector.
			theRestriction = DoReplaceBoundNames(
				iWalker_Bingo->fBoundNames,
				&iWalker_Bingo->fBoundOffsets[0],
				ioResults).Do(theRestriction);

			if (ZLOGF(w, eDebug+1))
				{
				w << "\n";
				Visitor_Expr_Bool_ValPred_Any_ToStrim().ToStrim(sDefault(), w, iWalker_Bingo->fRestriction);
				w << "\n";
				Visitor_Expr_Bool_ValPred_Any_ToStrim().ToStrim(sDefault(), w, theRestriction);
				}
			}

		const SearchSpec theSearchSpec(iWalker_Bingo->fConcreteHead, theRestriction);

		PRegSearch* thePRegSearch = nullptr;
		if (PRegSearch** thePRegSearchP = sPMut(fMap_SearchSpec_PRegSearchStar, theSearchSpec))
			{ thePRegSearch = *thePRegSearchP; }
		else
			{
			pair<Map_Refcon_PRegSearch::iterator,bool> iterPair =
				fMap_Refcon_PRegSearch.insert(make_pair(fNextRefcon++, PRegSearch()));
			ZAssert(iterPair.second);

			thePRegSearch = &iterPair.first->second;

			sInsertMust(fMap_SearchSpec_PRegSearchStar, theSearchSpec, thePRegSearch);

			thePRegSearch->fSearchSpec = theSearchSpec;
			thePRegSearch->fRefconInSearcher = iterPair.first->first;

			const AddedSearch theAS(thePRegSearch->fRefconInSearcher, theSearchSpec);

			guard.Release();

			fSearcher->ModifyRegistrations(&theAS, 1, nullptr, 0);

			while (not thePRegSearch->fResult)
				fCnd.Wait(fMtxR); // *2* see above at *1*
			}

		// We may end up using the same PRegSearch to support a single PQuery (e.g. a self-join),
		// so we sQInsert rather than sInsertMust.
		PQuery* thePQuery = iWalker_Bingo->fPQuery;
		sQInsert(thePRegSearch->fPQuery_Using, thePQuery);
		sQInsert(thePQuery->fPRegSearch_Used, thePRegSearch);
		iWalker_Bingo->fPRegSearch = thePRegSearch;

		ZAssert(thePRegSearch->fResult);
		}

	ZRef<Result> theResult = iWalker_Bingo->fPRegSearch->fResult;

	if (iWalker_Bingo->fNextRow >= theResult->Count())
		return false;

	const Val_Any* theRows = theResult->GetValsAt(iWalker_Bingo->fNextRow++);

	for (size_t xx = 0, colCount = iWalker_Bingo->fConcreteHead.size(); xx < colCount; ++xx)
		ioResults[iWalker_Bingo->fBaseOffset + xx] = theRows[xx];

	return true;
	}

ZRef<QueryEngine::Walker> Relater_Searcher::pMakeWalker(PQuery* iPQuery,
	const RelHead& iRelHead_Bound,
	const SearchSpec& iSearchSpec)
	{
	ZGuardMtxR guard(fMtxR);
	ZRef<Walker_Bingo> theWalker = new Walker_Bingo(this,
		iPQuery, iRelHead_Bound, iSearchSpec.GetConcreteHead(), iSearchSpec.GetRestriction());
	return theWalker;
	}

} // namespace Dataspace
} // namespace ZooLib
