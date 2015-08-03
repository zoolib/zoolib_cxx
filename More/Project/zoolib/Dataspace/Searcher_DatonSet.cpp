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
#include "zoolib/Compare.h"
#include "zoolib/Log.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_STL.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_JSON.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/Dataspace/Daton_Val.h"
#include "zoolib/Dataspace/Searcher_DatonSet.h"

#include "zoolib/DatonSet/Util_Strim_IntervalTreeClock.h"

#include "zoolib/Expr/Util_Expr_Bool_CNF.h"

#include "zoolib/QueryEngine/ResultFromWalker.h"
#include "zoolib/QueryEngine/Walker_Project.h"
#include "zoolib/QueryEngine/Walker_Result.h"
#include "zoolib/QueryEngine/Walker_Restrict.h"

#include "zoolib/RelationalAlgebra/RelHead.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/ValPred/ValPred_Any.h"
#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Do_GetNames.h"

namespace ZooLib {
namespace Dataspace {

using DatonSet::Daton;
using DatonSet::Deltas;
using DatonSet::Vector_Event_Delta_t;

using namespace Util_STL;

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

namespace QE = QueryEngine;
namespace RA = RelationalAlgebra;

/*
Minimally, keep an index of property names in contents -- that way we can efficiently
identify those entities that satisfy a RelHead.
*/

// =================================================================================================
#pragma mark -
#pragma mark Index

typedef vector<ColName> IndexSpec;

class Searcher_DatonSet::Index
	{
public:
	static const size_t kMaxCols = 4;

	// -----

	struct Key
		{
		const Val_Any* fTarget;
		const Val_Any* fValues[kMaxCols];
		};

	// -----

	struct Comparer
		{
		Comparer(size_t iCount)
		:	fCount(iCount)
			{}

		bool operator()(const Key& iLeft, const Key& iRight) const
			{
			for (size_t xx = 0; xx < fCount; ++xx)
				{
				if (int compare = sCompare_T(*iLeft.fValues[xx], *iRight.fValues[xx]))
					return compare < 0;
				}

			// Tie-break on the *pointer*, just so keys are distinct.
			return iLeft.fTarget < iRight.fTarget;
			}
		const size_t fCount;
		};

	// -----

	Index(const IndexSpec& iIndexSpec)
	:	fCount(iIndexSpec.size())
	,	fSet(Comparer(fCount))
		{
		ZAssert(fCount <= kMaxCols);
		std::copy_n(iIndexSpec.begin(), fCount, fColNames);
		}

	void Insert(const Val_Any* iVal)
		{
		Key theKey;
		if (this->pAsKey(iVal, theKey))
			sInsertMust(fSet, theKey);
		}

	void Erase(const Val_Any* iVal)
		{
		Key theKey;
		if (this->pAsKey(iVal, theKey))
			sEraseMust(fSet, theKey);
		}

	bool pAsKey(const Val_Any* iValPtr, Key& oKey)
		{
		const Map_Any* asMap = iValPtr->PGet<Map_Any>();
		if (not asMap)
			{
			// iValPtr is not a map, can't index.
			return false;
			}

		const Val_Any* firstVal = asMap->PGet(fColNames[0]);
		if (not firstVal)
			{
			// The map does not have our first property, so there's no point
			// in storing it -- no search we can do will help find it.
			return false;
			}

		const Val_Any* emptyValPtr = &sDefault<Val_Any>();

		oKey.fValues[0] = firstVal;
		for (size_t xx = 1; xx < fCount; ++xx)
			{
			if (const Val_Any* theVal = asMap->PGet(fColNames[xx]))
				oKey.fValues[xx] = theVal;
			else
				oKey.fValues[xx] = emptyValPtr;
			}

		oKey.fTarget = iValPtr;
		return true;
		}

	ColName fColNames[kMaxCols];
	const size_t fCount;

	typedef std::set<Key,Comparer> Set;
	Set fSet;
	};

// =================================================================================================
#pragma mark -
#pragma mark Searcher_DatonSet::Walker_Map

class Searcher_DatonSet::Walker_Map
:	public QE::Walker
	{
public:
	Walker_Map(ZRef<Searcher_DatonSet> iSearcher, const ConcreteHead& iConcreteHead)
	:	fSearcher(iSearcher)
	,	fConcreteHead(iConcreteHead)
		{}

	virtual ~Walker_Map()
		{}

// From QE::Walker
	virtual void Rewind()
		{ fSearcher->pRewind(this); }

	virtual ZRef<QE::Walker> Prime(const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{
		fSearcher->pPrime(this, iOffsets, oOffsets, ioBaseOffset);
		return this;
		}

	virtual bool QReadInc(Val_Any* ioResults)
		{ return fSearcher->pReadInc(this, ioResults); }

	const ZRef<Searcher_DatonSet> fSearcher;
	const ConcreteHead fConcreteHead;
	size_t fBaseOffset;
	Map_Assert::const_iterator fCurrent;
	std::set<std::vector<Val_Any> > fPriors;
	};

// =================================================================================================
#pragma mark -
#pragma mark Searcher_DatonSet::Walker_Index

class Searcher_DatonSet::Walker_Index
:	public QE::Walker
	{
public:
	Walker_Index(ZRef<Searcher_DatonSet> iSearcher,
		Index::Set::const_iterator iBegin, Index::Set::const_iterator iEnd)
	:	fSearcher(iSearcher)
	,	fBegin(iBegin)
	,	fEnd(iEnd)
		{}

	virtual ~Walker_Index()
		{}

// From QE::Walker
	virtual void Rewind()
		{ fSearcher->pRewind(this); }

	virtual ZRef<QE::Walker> Prime(const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{
		fSearcher->pPrime(this, iOffsets, oOffsets, ioBaseOffset);
		return this;
		}

	virtual bool QReadInc(Val_Any* ioResults)
		{ return fSearcher->pReadInc(this, ioResults); }

	const ZRef<Searcher_DatonSet> fSearcher;
	const Index::Set::const_iterator fBegin;
	const Index::Set::const_iterator fEnd;

	Index::Set::const_iterator fCurrent;
	};

// =================================================================================================
#pragma mark -
#pragma mark Searcher_DatonSet::ClientSearch

class Searcher_DatonSet::DLink_ClientSearch_InPSearch
:	public DListLink<ClientSearch, DLink_ClientSearch_InPSearch, kDebug>
	{};

class Searcher_DatonSet::DLink_ClientSearch_NeedsWork
:	public DListLink<ClientSearch, DLink_ClientSearch_NeedsWork, kDebug>
	{};

class Searcher_DatonSet::ClientSearch
:	public DLink_ClientSearch_InPSearch
,	public DLink_ClientSearch_NeedsWork
	{
public:
	ClientSearch(int64 iRefcon, PSearch* iPSearch)
	:	fRefcon(iRefcon)
	,	fPSearch(iPSearch)
		{}

	int64 const fRefcon;
	PSearch* const fPSearch;
	};

// =================================================================================================
#pragma mark -
#pragma mark Searcher_DatonSet::PSearch

class Searcher_DatonSet::DLink_PSearch_NeedsWork
:	public DListLink<PSearch, DLink_PSearch_NeedsWork, kDebug>
	{};

class Searcher_DatonSet::PSearch
:	public DLink_PSearch_NeedsWork
	{
public:
	PSearch(const SearchSpec& iSearchSpec)
	:	fSearchSpec(iSearchSpec)
		{}

	const SearchSpec fSearchSpec;

	DListHead<DLink_ClientSearch_InPSearch> fClientSearch_InPSearch;

	ZRef<QE::Result> fResult;
	};

// =================================================================================================
#pragma mark -
#pragma mark Searcher_DatonSet

Searcher_DatonSet::Searcher_DatonSet(const vector<IndexSpec>& iIndexSpecs)
:	fEvent(Event::sZero())
	{
	foreachi (ii, iIndexSpecs)
		fIndexes.push_back(new Index(*ii));
	}

Searcher_DatonSet::~Searcher_DatonSet()
	{
	for (DListEraser<PSearch,DLink_PSearch_NeedsWork> eraser = fPSearch_NeedsWork;
		eraser; eraser.Advance())
		{}

	for (DListEraser<ClientSearch,DLink_ClientSearch_NeedsWork> eraser = fClientSearch_NeedsWork;
		eraser; eraser.Advance())
		{}

	sDeleteAll(fIndexes.begin(), fIndexes.end());
	}

bool Searcher_DatonSet::Intersects(const RelHead& iRelHead)
	{ return true; }

struct Searcher_DatonSet::Thing
	{
	Index* fIndex;
	vector<Val_Any> fVals_EQ;
	ZQ<Val_Any> fVal_Lo;
	bool fLoIsLT;
	ZQ<Val_Any> fVal_Hi;
	bool fHiIsGT;
	Util_Expr_Bool::CNF fCNFRemainder;

	Thing()
	:	fIndex(nullptr)
		{}
	};

typedef ValComparator_Simple::EComparator EComparator;

#if 1
static EComparator spFlipped(EComparator iEComparator)
	{
	switch (iEComparator)
		{
		case ValComparator_Simple::eLT: return ValComparator_Simple::eGT;
		case ValComparator_Simple::eLE: return ValComparator_Simple::eGE;
		case ValComparator_Simple::eEQ: return ValComparator_Simple::eEQ;
		case ValComparator_Simple::eNE: return ValComparator_Simple::eNE;
		case ValComparator_Simple::eGE: return ValComparator_Simple::eLE;
		case ValComparator_Simple::eGT: return ValComparator_Simple::eLT;
		}
	ZUnimplemented();
	}

static void spMungeIt(const string8& iName, EComparator iEComparator, const Val_Any& iVal,
	Searcher_DatonSet::Thing& ioThing)
	{
	}

void Searcher_DatonSet::pSetupPSearch(PSearch& ioPSearch)
	{
	using namespace Util_Expr_Bool;

	const CNF theCNF = sAsCNF(ioPSearch.fSearchSpec.GetRestriction());

	foreachv (Index* curIndex, fIndexes)
		{
		CNF curDClauses = theCNF;

		vector<Val_Any> equalityVals;
		ZQ<Val_Any> tailLowerQ;
		ZQ<Val_Any> tailUpperQ;

		for (size_t xxColName = 0; xxColName < curIndex->fCount; ++xxColName)
			{
			const ColName& curColName = curIndex->fColNames[xxColName];

			EComparator curEComparator;
			Val_Any curComparand;
			bool gotCurComparison = false;

			// Go through each DClause -- intersect each clause's effect into curEComparator/curComparand.
			// If the clause is completely representable as such, remove it from curDClauses.

			for (set<DClause>::iterator iterDClauses = curDClauses.begin();
				iterDClauses != curDClauses.end();
				/*no inc*/)
				{
				bool gotClauseComparison = false;
				EComparator clauseEComparator;
				Val_Any clauseComparand;

				// Every term in this DClause must refer to curColName in a usable fashion. We could
				// consolidate them into a single comparator. For the moment I'm just going to take
				// DClauses that have a single term. Fixes needed at ***

				bool everyTermIsRelevant = iterDClauses->size() == 1; // ***

				for (set<Term>::iterator iterTerms = iterDClauses->begin();
					everyTermIsRelevant && iterTerms != iterDClauses->end();
					/*no inc*/)
					{
					bool termIsRelevant = false;
					if (ZRef<Expr_Bool_ValPred> theExpr = iterTerms->Get().DynamicCast<Expr_Bool_ValPred>())
						{
						const ValPred& theValPred = theExpr->GetValPred();

						if (ZRef<ValComparator_Simple> theValComparator =
							theValPred.GetComparator().DynamicCast<ValComparator_Simple>())
							{
							EComparator theEComparator = theValComparator->GetEComparator();

							ZRef<ValComparand_Const_Any> theComparand_Const =
								theValPred.GetRHS().DynamicCast<ValComparand_Const_Any>();

							ZRef<ValComparand_Name> theComparand_Name =
								theValPred.GetLHS().DynamicCast<ValComparand_Name>();

							if (not theComparand_Const || not theComparand_Name)
								{
								theComparand_Const = theValPred.GetLHS().DynamicCast<ValComparand_Const_Any>();
								theComparand_Name = theValPred.GetRHS().DynamicCast<ValComparand_Name>();
								theEComparator = spFlipped(theEComparator);
								}

							if (theComparand_Const
								&& theComparand_Name && theComparand_Name->GetName() == curColName)
								{
								if (not gotClauseComparison)
									{
									gotClauseComparison = true;
									clauseEComparator = theEComparator;
									clauseComparand = theComparand_Const->GetVal();
									}
								else
									{
									// Don't have the code to union yet.***
									ZUnimplemented();
									}
								termIsRelevant = true;
								++iterTerms;
								}
							}
						}
					everyTermIsRelevant = everyTermIsRelevant && termIsRelevant;
					} // iterTerms

				if (not everyTermIsRelevant)
					{
					++iterDClauses;
					}
				else
					{
					// Remove this DClause from further consideration -- its constraints will be
					// represented in curComparison.
					iterDClauses = sEraseInc(curDClauses, iterDClauses);

					if (ZLOGF(w, eDebug))
						{
						w << curColName << " " << clauseEComparator << " ";
						Yad_JSON::sToChan(sYadR(clauseComparand), w);
						}

					// And intersect clauseComparison with curComparison
					if (not gotCurComparison)
						{
						gotCurComparison = true;
						curEComparator = clauseEComparator;
						curComparand = clauseComparand;
						}
					else
						{
						// Don't have the code to intersect yet.
						ZUnimplemented();
						}
					}
				} // iterDClauses

			if (not gotCurComparison)
				{
				break;
				}
			else if (curEComparator == ValComparator_Simple::eEQ)
				{
				equalityVals.push_back(curComparand);
				}
			else
				{
				gotLastComparison = true;
				lastEComparator = curEComparator;
				lastComparand = curComparand;
				break;
				}
			} // xxColName

		// We've got equalityVals filled in with stuff we're doing an equality search on, and
		// may have a comparison in lastComparison.
		ZLOGTRACE(eDebug);
		}

//	this->pChooseIndex(theCNF, theIndex, theVals_EQ, theVal_Lo, loIsLT, theVal_Hi, hiIsGT,
//#		theCNFRemainder);

	}

#endif

void Searcher_DatonSet::ModifyRegistrations(
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardMtxR guard(fMtxR);

	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		const SearchSpec& theSearchSpec = iAdded->GetSearchSpec();

		const pair<Map_SearchSpec_PSearch::iterator,bool>
			iterPSearchPair = fMap_SearchSpec_PSearch.insert(
				Map_SearchSpec_PSearch::value_type(theSearchSpec, theSearchSpec));

		PSearch* thePSearch = &iterPSearchPair.first->second;

		if (iterPSearchPair.second)
			{
			if (ZLOGPF(w, eDebug))
				{
				w << "\n" << theSearchSpec.GetConcreteHead();
				w << "\n";
				Visitor_Expr_Bool_ValPred_Any_ToStrim().ToStrim(sDefault(), w, theSearchSpec.GetRestriction());

				foreacha (anIndex, fIndexes)
					{
					w << "\n";
					for (size_t xx = 0; xx < anIndex->fCount; ++xx)
						w << anIndex->fColNames[xx] << " ";
					foreachi (iterSet, anIndex->fSet)
						{
						for (size_t xx = 0; xx < anIndex->fCount; ++xx)
							{
							Yad_JSON::sToChan(sYadR(*(iterSet->fValues[xx])), w);
							w << " ";
							}
						w << "--> ";
						Yad_JSON::sToChan(sYadR(*(iterSet->fTarget)), w);
						w << "\n";
						}
					w << "\n";
					}
				}

			// It's a new PSearch, so we'll need to work on it
			sInsertBackMust(fPSearch_NeedsWork, thePSearch);

			// and get it hooked up.
			this->pSetupPSearch(*thePSearch);
			}

		const int64 theRefcon = iAdded->GetRefcon();

		const pair<map<int64,ClientSearch>::iterator,bool>
			iterClientSearchPair = fMap_Refcon_ClientSearch.insert(
				make_pair(theRefcon, ClientSearch(theRefcon, thePSearch)));
		ZAssert(iterClientSearchPair.second);

		ClientSearch* theClientSearch = &iterClientSearchPair.first->second;
		sInsertBackMust(thePSearch->fClientSearch_InPSearch, theClientSearch);

		sInsertBackMust(fClientSearch_NeedsWork, theClientSearch);
		}

	while (iRemovedCount--)
		{
		const int64 theRefcon = *iRemoved++;

		map<int64, ClientSearch>::iterator iterClientSearch =
			fMap_Refcon_ClientSearch.find(theRefcon);

		ZAssertStop(kDebug, iterClientSearch != fMap_Refcon_ClientSearch.end());

		ClientSearch* theClientSearch = &iterClientSearch->second;

		PSearch* thePSearch = theClientSearch->fPSearch;
		sEraseMust(thePSearch->fClientSearch_InPSearch, theClientSearch);
		if (sIsEmpty(thePSearch->fClientSearch_InPSearch))
			{
			sQErase(fPSearch_NeedsWork, thePSearch);
			sEraseMust(kDebug, fMap_SearchSpec_PSearch, thePSearch->fSearchSpec);
			}

		sQErase(fClientSearch_NeedsWork, theClientSearch);
		fMap_Refcon_ClientSearch.erase(iterClientSearch);
		}

	if (sNotEmpty(fClientSearch_NeedsWork) || sNotEmpty(fPSearch_NeedsWork))
		{
		guard.Release();
		Searcher::pTriggerSearcherResultsAvailable();
		}
	}

void Searcher_DatonSet::CollectResults(vector<SearchResult>& oChanged)
	{
	Searcher::pCollectResultsCalled();

	this->pPull();

	ZAcqMtxR acq(fMtxR);

	oChanged.clear();

	// Go through the PScans that need work, and generate any result needed.

//	for (DListEraser<PScan,DLink_PScan_NeedsWork> eraser = fPScan_NeedsWork;
//		eraser; eraser.Advance())
//		{
//		PScan* thePScan = eraser.Current();
//		if (not thePScan->fResult)
//			{
//			ZRef<QE::Walker> theWalker = new Walker(this, thePScan->fConcreteHead);
//			thePScan->fResult = QE::sResultFromWalker(theWalker);
//
//			for (DListIterator<PSearch, DLink_PSearch_InPScan>
//				iter = thePScan->fPSearch_InPScan; iter; iter.Advance())
//				{
//				PSearch* thePSearch = iter.Current();
//				thePSearch->fResult.Clear();
//				sQInsertBack(fPSearch_NeedsWork, thePSearch);
//				}
//			}
//		}

	for (DListEraser<PSearch,DLink_PSearch_NeedsWork> eraser = fPSearch_NeedsWork;
		eraser; eraser.Advance())
		{
		PSearch* thePSearch = eraser.Current();

		if (not thePSearch->fResult)
			{
#if 0
			PScan* thePScan = thePSearch->fPScan;

			const SearchSpec& theSearchSpec = thePSearch->fSearchSpec;

			const ZRef<Expr_Bool>& theRestriction = theSearchSpec.GetRestriction();

			ZRef<QE::Walker> theWalker;
			if (theRestriction && theRestriction != sTrue())
				{
				theWalker = new QE::Walker_Result(thePScan->fResult);
				theWalker = new QE::Walker_Restrict(theWalker, theRestriction);
				}

			const RelHead theRH_Wanted = RA::sRelHead(theSearchSpec.GetConcreteHead());
			if (theRH_Wanted != RA::sRelHead(thePScan->fConcreteHead))
				{
				if (not theWalker)
					theWalker = new QE::Walker_Result(thePScan->fResult);
				theWalker = new QE::Walker_Project(theWalker, theRH_Wanted);
				}

			if (not theWalker)
				thePSearch->fResult = thePScan->fResult;
			else
				thePSearch->fResult = QE::sResultFromWalker(theWalker);

			for (DListIterator<ClientSearch, DLink_ClientSearch_InPSearch>
				iter = thePSearch->fClientSearch_InPSearch; iter; iter.Advance())
				{ sQInsertBack(fClientSearch_NeedsWork, iter.Current()); }
#endif
			}
		}

	for (DListEraser<ClientSearch,DLink_ClientSearch_NeedsWork> eraser = fClientSearch_NeedsWork;
		eraser; eraser.Advance())
		{
		ClientSearch* theClientSearch = eraser.Current();
		PSearch* thePSearch = theClientSearch->fPSearch;
		oChanged.push_back(SearchResult(theClientSearch->fRefcon, thePSearch->fResult));
		}
	}

ZRef<DatonSet::Callable_PullSuggested> Searcher_DatonSet::GetCallable_PullSuggested()
	{
	ZAcqMtxR acq(fMtxR);
	if (not fCallable_PullSuggested_Self)
		fCallable_PullSuggested_Self = sCallable(sWeakRef(this), &Searcher_DatonSet::pPullSuggested);
	return fCallable_PullSuggested_Self;
	}

void Searcher_DatonSet::pPullSuggested(const ZRef<DatonSet::Callable_PullFrom>& iCallable_PullFrom)
	{
	ZGuardMtxR guard(fMtxR);
	sInsert(fCallables_PullFrom, iCallable_PullFrom);
	guard.Release();
	Searcher::pTriggerSearcherResultsAvailable();
	}

void Searcher_DatonSet::pPull()
	{
	ZGuardMtxR guard(fMtxR);

	set<ZRef<DatonSet::Callable_PullFrom> > theCallables_PullFrom;
	swap(theCallables_PullFrom, fCallables_PullFrom);

	foreachv (ZRef<DatonSet::Callable_PullFrom> theCallable, theCallables_PullFrom)
		{
		guard.Release();

		ZRef<DatonSet::Deltas> theDeltas = theCallable->Call(fEvent);

		guard.Acquire();

		foreachi (iter, theDeltas->GetVector())
			fEvent = fEvent->Joined(iter->first);

		const Vector_Event_Delta_t& theVector = theDeltas->GetVector();

		foreachi (iterVector, theVector)
			{
			const ZRef<Event>& theEvent = iterVector->first;
			const map<Daton, bool>& theStatements = iterVector->second->GetStatements();

			foreachi (iterStmts, theStatements)
				{
				const Daton& theDaton = iterStmts->first;

				map<Daton, pair<ZRef<Event>, Val_Any> >::iterator lbAssert =
					fMap_Assert.lower_bound(theDaton);

				map<Daton, ZRef<Event> >::iterator lbRetract =
					fMap_Retract.lower_bound(theDaton);

				if (iterStmts->second)
					{
					// It's an assert
					if (lbAssert != fMap_Assert.end() and lbAssert->first == theDaton)
						{
						// It's in fMap_Assert.
						ZAssert(lbRetract == fMap_Retract.end() || lbRetract->first != theDaton);
						if (sIsBefore(lbAssert->second.first, theEvent))
							{
							// It's more recent.
							lbAssert->second.first = theEvent;
							}
						}
					else if (lbRetract != fMap_Retract.end() and lbRetract->first == theDaton)
						{
						// It's in fMap_Retract.
						if (sIsBefore(lbRetract->second, theEvent))
							{
							// It's more recent.
							fMap_Retract.erase(lbRetract);

							Map_Assert::const_iterator iter = fMap_Assert.insert(lbAssert,
								make_pair(theDaton, make_pair(theEvent, sAsVal(theDaton))));

							this->pIndexInsert(&iter->second.second);
							}
						}
					else
						{
						// It's not previously known.
						Map_Assert::const_iterator iter = fMap_Assert.insert(lbAssert,
							make_pair(theDaton, make_pair(theEvent, sAsVal(theDaton))));

						this->pIndexInsert(&iter->second.second);
						}
					}
				else
					{
					// It's a retract.
					if (lbAssert != fMap_Assert.end() and lbAssert->first == theDaton)
						{
						// It's in fMap_Assert.
						ZAssert(lbRetract == fMap_Retract.end() || lbRetract->first != theDaton);
						if (sIsBefore(lbAssert->second.first, theEvent))
							{
							// It's more recent.
							this->pIndexErase(&lbAssert->second.second);
							fMap_Assert.erase(lbAssert);
							fMap_Retract.insert(lbRetract, make_pair(theDaton, theEvent));
							}
						}
					else if (lbRetract != fMap_Retract.end() and lbRetract->first == theDaton)
						{
						// It's in fMap_Retract.
						if (sIsBefore(lbRetract->second, theEvent))
							{
							// It's more recent.
							lbRetract->second = theEvent;
							}
						}
					else
						{
						// It's not previously known.
						fMap_Retract.insert(lbRetract, make_pair(theDaton, theEvent));
						}
					}
				}
			}
		}
	}

//void Searcher_DatonSet::pChanged(const Val_Any& iVal)
//	{
//	const Map_Any theMap = iVal.Get<Map_Any>();
//	RelHead theRH;
//	for (Map_Any::Index_t i = theMap.Begin(); i != theMap.End(); ++i)
//		theRH |= RA::ColName(theMap.NameOf(i));
//
//	// The Daton itself has changed, so include the daton's pseudo-name in theRH.
//	theRH.insert(string8());
//
//	if (ZLOGPF(w, eDebug + 1))
//		w << "theRH: " << theRH;
//
//	// This is overkill -- we don't necessarily have to rework the whole PScan.
//	for (Map_PScan::iterator iterPScan = fMap_PScan.begin();
//		iterPScan != fMap_PScan.end(); ++iterPScan)
//		{
//		PScan* thePScan = &iterPScan->second;
//		if (sIncludes(theRH, RA::sRelHead_Required(thePScan->fConcreteHead)))
//			{
//			if (ZLOGPF(w,eDebug + 1))
//				w << "Invalidating PScan: " << thePScan->fConcreteHead;
//			thePScan->fResult.Clear();
//			sQInsertBack(fPScan_NeedsWork, thePScan);
//			}
//		else
//			{
//			if (ZLOGPF(w,eDebug + 1))
//				w << "Not invalidating PScan: " << thePScan->fConcreteHead;
//			}
//		}
//	}

void Searcher_DatonSet::pIndexInsert(const Val_Any* iVal)
	{
	foreacha (anIndex, fIndexes)
		anIndex->Insert(iVal);
	}

void Searcher_DatonSet::pIndexErase(const Val_Any* iVal)
	{
	foreacha (anIndex, fIndexes)
		anIndex->Erase(iVal);
	}

void Searcher_DatonSet::pRewind(ZRef<Walker_Map> iWalker_Map)
	{
	iWalker_Map->fCurrent = fMap_Assert.begin();
	}

void Searcher_DatonSet::pPrime(ZRef<Walker_Map> iWalker_Map,
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	iWalker_Map->fCurrent = fMap_Assert.begin();
	iWalker_Map->fBaseOffset = ioBaseOffset;
	foreachi (ii, iWalker_Map->fConcreteHead)
		oOffsets[ii->first] = ioBaseOffset++;
	}

bool Searcher_DatonSet::pReadInc(ZRef<Walker_Map> iWalker_Map, Val_Any* ioResults)
	{
	const ConcreteHead& theConcreteHead = iWalker_Map->fConcreteHead;

	while (iWalker_Map->fCurrent != fMap_Assert.end())
		{
		if (const Map_Any* theMap = iWalker_Map->fCurrent->second.second.PGet<Map_Any>())
			{
			bool gotAll = true;
			vector<Val_Any> subset;
			subset.reserve(theConcreteHead.size());
			size_t offset = iWalker_Map->fBaseOffset;
			for (ConcreteHead::const_iterator
				ii = theConcreteHead.begin(), end = theConcreteHead.end();
				ii != end; ++ii, ++offset)
				{
				const string8& theName = ii->first;
				if (theName.empty())
					{
					// Empty name indicates that we want the Daton itself.
					const Val_Any& theVal = iWalker_Map->fCurrent->first;
					ioResults[offset] = theVal;
					subset.push_back(theVal);
					}
				else if (const Val_Any* theVal = sPGet(*theMap, theName))
					{
					ioResults[offset] = *theVal;
					subset.push_back(*theVal);
					}
				else if (not ii->second)
					{
					ioResults[offset] = AbsentOptional_t();
					subset.push_back(AbsentOptional_t());
					}
				else
					{
					gotAll = false;
					break;
					}
				}

			if (gotAll && sQInsert(iWalker_Map->fPriors, subset))
				{
				++iWalker_Map->fCurrent;
				return true;
				}
			}
		++iWalker_Map->fCurrent;
		}

	return false;
	}

void Searcher_DatonSet::pRewind(ZRef<Walker_Index> iWalker_Index)
	{
	iWalker_Index->fCurrent = iWalker_Index->fBegin;
	}

void Searcher_DatonSet::pPrime(ZRef<Walker_Index> iWalker_Index,
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	ZUnimplemented();
//	iWalker_Map->fCurrent = fMap_Assert.begin();
//	iWalker_Map->fBaseOffset = ioBaseOffset;
//	foreachi (ii, iWalker_Map->fConcreteHead)
//		oOffsets[ii->first] = ioBaseOffset++;
	}

bool Searcher_DatonSet::pReadInc(ZRef<Walker_Index> iWalker_Index, Val_Any* ioResults)
	{
	ZUnimplemented();
	}

} // namespace Dataspace
} // namespace ZooLib
