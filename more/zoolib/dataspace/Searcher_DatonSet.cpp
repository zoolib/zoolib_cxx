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
#include "zoolib/Stringf.h"

#include "zoolib/ZCompare.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_vector.h"
#include "zoolib/ZUtil_Strim_IntervalTreeClock.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Do_GetNames.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

#include "zoolib/dataspace/Daton_Val.h"

#include "zoolib/dataspace/Searcher_DatonSet.h"

#include "zoolib/QueryEngine/ResultFromWalker.h"
#include "zoolib/QueryEngine/Walker_Project.h"
#include "zoolib/QueryEngine/Walker_Result.h"
#include "zoolib/QueryEngine/Walker_Restrict.h"

#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

namespace ZooLib {
namespace Dataspace {

using DatonSet::Daton;
using DatonSet::Deltas;
using DatonSet::Vector_Event_Delta_t;

using namespace ZUtil_STL;

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
// MARK: - Searcher_DatonSet::Walker

class Searcher_DatonSet::Walker
:	public QE::Walker
	{
public:
	Walker(ZRef<Searcher_DatonSet> iSearcher, const ConcreteHead& iConcreteHead)
	:	fSearcher(iSearcher)
	,	fConcreteHead(iConcreteHead)
		{}

	virtual ~Walker()
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

	virtual bool QReadInc(ZVal_Any* ioResults)
		{ return fSearcher->pReadInc(this, ioResults); }

	const ZRef<Searcher_DatonSet> fSearcher;
	const ConcreteHead fConcreteHead;
	size_t fBaseOffset;
	Map_Assert::const_iterator fCurrent;
	std::set<std::vector<ZVal_Any> > fPriors;
	};

// =================================================================================================
// MARK: - Searcher_DatonSet::ClientSearch

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
// MARK: - Searcher_DatonSet::PSearch

class Searcher_DatonSet::DLink_PSearch_InPScan
:	public DListLink<PSearch, DLink_PSearch_InPScan, kDebug>
	{};

class Searcher_DatonSet::DLink_PSearch_NeedsWork
:	public DListLink<PSearch, DLink_PSearch_NeedsWork, kDebug>
	{};

class Searcher_DatonSet::PSearch
:	public DLink_PSearch_InPScan
,	public DLink_PSearch_NeedsWork
	{
public:
	PSearch(const SearchSpec& iSearchSpec)
	:	fSearchSpec(iSearchSpec)
		{}

	const SearchSpec fSearchSpec;

	DListHead<DLink_ClientSearch_InPSearch> fClientSearch_InPSearch;

	PScan* fPScan;

	ZRef<QE::Result> fResult;
	};

// =================================================================================================
// MARK: - Searcher_DatonSet::PScan

class Searcher_DatonSet::DLink_PScan_NeedsWork
:	public DListLink<PScan, DLink_PScan_NeedsWork, kDebug>
	{};

class Searcher_DatonSet::PScan
:	public DLink_PScan_NeedsWork
	{
public:
	PScan(const ConcreteHead& iConcreteHead)
	:	fConcreteHead(iConcreteHead)
		{}

	const ConcreteHead fConcreteHead;
	DListHead<DLink_PSearch_InPScan> fPSearch_InPScan;
	ZRef<QE::Result> fResult;
	};

// =================================================================================================
// MARK: - Searcher_DatonSet

Searcher_DatonSet::Searcher_DatonSet()
:	fEvent(Event::sZero())
	{}

Searcher_DatonSet::~Searcher_DatonSet()
	{
	for (DListEraser<PScan,DLink_PScan_NeedsWork> eraser = fPScan_NeedsWork;
		eraser; eraser.Advance())
		{}

	for (DListEraser<PSearch,DLink_PSearch_NeedsWork> eraser = fPSearch_NeedsWork;
		eraser; eraser.Advance())
		{}

	for (DListEraser<ClientSearch,DLink_ClientSearch_NeedsWork> eraser = fClientSearch_NeedsWork;
		eraser; eraser.Advance())
		{}
	}

bool Searcher_DatonSet::Intersects(const RelHead& iRelHead)
	{ return true; }

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
				make_pair(theSearchSpec, PSearch(theSearchSpec)));

		PSearch* thePSearch = &iterPSearchPair.first->second;

		if (iterPSearchPair.second)
			{
			// It's a new PSearch, so we'll need to work on it
			sInsertBackMust(fPSearch_NeedsWork, thePSearch);

			// and connect it to a PScan
			RelHead theRH_Required, theRH_Optional;
			RA::sRelHeads(theSearchSpec.GetConcreteHead(), theRH_Required, theRH_Optional);

			const RelHead theRH_Restriction = sGetNames(theSearchSpec.GetRestriction());

			const ConcreteHead theCH = RA::sConcreteHead(
				theRH_Required, theRH_Optional | theRH_Restriction);

			const pair<Map_PScan::iterator,bool>
				iterPScanPair = fMap_PScan.insert(make_pair(theCH, PScan(theCH)));

			PScan* thePScan = &iterPScanPair.first->second;
			thePSearch->fPScan = thePScan;
			sInsertBackMust(thePScan->fPSearch_InPScan, thePSearch);

			if (iterPScanPair.second)
				{
				// It's a new PScan, so we'll need to work on it.
				sInsertBackMust(fPScan_NeedsWork, thePScan);
				}
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
		int64 theRefcon = *iRemoved++;

		map<int64, ClientSearch>::iterator iterClientSearch =
			fMap_Refcon_ClientSearch.find(theRefcon);

		ZAssertStop(kDebug, iterClientSearch != fMap_Refcon_ClientSearch.end());

		ClientSearch* theClientSearch = &iterClientSearch->second;

		PSearch* thePSearch = theClientSearch->fPSearch;
		sEraseMust(thePSearch->fClientSearch_InPSearch, theClientSearch);
		if (sIsEmpty(thePSearch->fClientSearch_InPSearch))
			{
			PScan* thePScan = thePSearch->fPScan;
			sEraseMust(thePScan->fPSearch_InPScan, thePSearch);
			if (sIsEmpty(thePScan->fPSearch_InPScan))
				{
				sQErase(fPScan_NeedsWork, thePScan);
				sEraseMust(kDebug, fMap_PScan, thePScan->fConcreteHead);
				}

			sQErase(fPSearch_NeedsWork, thePSearch);
			sEraseMust(kDebug, fMap_SearchSpec_PSearch, thePSearch->fSearchSpec);
			}

		sQErase(fClientSearch_NeedsWork, theClientSearch);
		fMap_Refcon_ClientSearch.erase(iterClientSearch);
		}

	if (sNotEmpty(fClientSearch_NeedsWork)
		|| sNotEmpty(fPSearch_NeedsWork)
		|| sNotEmpty(fPScan_NeedsWork))
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

	for (DListEraser<PScan,DLink_PScan_NeedsWork> eraser = fPScan_NeedsWork;
		eraser; eraser.Advance())
		{
		PScan* thePScan = eraser.Current();
		if (not thePScan->fResult)
			{
			ZRef<QE::Walker> theWalker = new Walker(this, thePScan->fConcreteHead);
			thePScan->fResult = QE::sResultFromWalker(theWalker);

			for (DListIterator<PSearch, DLink_PSearch_InPScan>
				iter = thePScan->fPSearch_InPScan; iter; iter.Advance())
				{
				PSearch* thePSearch = iter.Current();
				thePSearch->fResult.Clear();
				sQInsertBack(fPSearch_NeedsWork, thePSearch);
				}
			}
		}

	for (DListEraser<PSearch,DLink_PSearch_NeedsWork> eraser = fPSearch_NeedsWork;
		eraser; eraser.Advance())
		{
		PSearch* thePSearch = eraser.Current();

		if (not thePSearch->fResult)
			{
			PScan* thePScan = thePSearch->fPScan;

			const SearchSpec& theSearchSpec = thePSearch->fSearchSpec;

			const ZRef<ZExpr_Bool>& theRestriction = theSearchSpec.GetRestriction();

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
			}
		}

	for (DListEraser<ClientSearch,DLink_ClientSearch_NeedsWork> eraser = fClientSearch_NeedsWork;
		eraser; eraser.Advance())
		{
		ClientSearch* theClientSearch = eraser.Current();
		PSearch* thePSearch = theClientSearch->fPSearch;
		oChanged.push_back(SearchResult(theClientSearch->fRefcon, thePSearch->fResult, fEvent));
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

				map<Daton, pair<ZRef<Event>, ZVal_Any> >::iterator lbAssert =
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
							const ZVal_Any theVal = sAsVal(theDaton);
							this->pChanged(theVal);
							lbAssert->second.first = theEvent;
							lbAssert->second.second = theVal;
							}
						}
					else if (lbRetract != fMap_Retract.end() and lbRetract->first == theDaton)
						{
						// It's in fMap_Retract.
						if (sIsBefore(lbRetract->second, theEvent))
							{
							// It's more recent.
							const ZVal_Any theVal = sAsVal(theDaton);
							this->pChanged(theVal);

							fMap_Retract.erase(lbRetract);
							fMap_Assert.insert(lbAssert,
								make_pair(theDaton, make_pair(theEvent, theVal)));
							}
						}
					else
						{
						// It's not previously known.
						const ZVal_Any theVal = sAsVal(theDaton);
						this->pChanged(theVal);

						fMap_Assert.insert(lbAssert,
							make_pair(theDaton, make_pair(theEvent, theVal)));
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
							this->pChanged(lbAssert->second.second);

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

void Searcher_DatonSet::pChanged(const ZVal_Any& iVal)
	{
	const ZMap_Any theMap = iVal.Get<ZMap_Any>();
	RelHead theRH;
	for (ZMap_Any::Index_t i = theMap.Begin(); i != theMap.End(); ++i)
		theRH |= RA::ColName(theMap.NameOf(i));

	// The Daton itself has changed, so include the daton's pseudo-name in theRH.
	theRH.insert(string8());

	if (ZLOGPF(w, eDebug + 1))
		w << "theRH: " << theRH;

	// This is overkill -- we don't necessarily have to rework the whole PScan.
	for (Map_PScan::iterator iterPScan = fMap_PScan.begin();
		iterPScan != fMap_PScan.end(); ++iterPScan)
		{
		PScan* thePScan = &iterPScan->second;
		if (sIncludes(theRH, RA::sRelHead_Required(thePScan->fConcreteHead)))
			{
			if (ZLOGPF(w,eDebug + 1))
				w << "Invalidating PScan: " << thePScan->fConcreteHead;
			thePScan->fResult.Clear();
			sQInsertBack(fPScan_NeedsWork, thePScan);
			}
		else
			{
			if (ZLOGPF(w,eDebug + 1))
				w << "Not invalidating PScan: " << thePScan->fConcreteHead;
			}
		}
	}

void Searcher_DatonSet::pRewind(ZRef<Walker> iWalker)
	{
	iWalker->fCurrent = fMap_Assert.begin();
	}

void Searcher_DatonSet::pPrime(ZRef<Walker> iWalker,
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	iWalker->fCurrent = fMap_Assert.begin();
	iWalker->fBaseOffset = ioBaseOffset;
	const ConcreteHead& theConcreteHead = iWalker->fConcreteHead;
	for (ConcreteHead::const_iterator ii =
		theConcreteHead.begin(), end = theConcreteHead.end();
		ii != end; ++ii)
		{ oOffsets[ii->first] = ioBaseOffset++; }
	}

bool Searcher_DatonSet::pReadInc(ZRef<Walker> iWalker, ZVal_Any* ioResults)
	{
	const ConcreteHead& theConcreteHead = iWalker->fConcreteHead;

	while (iWalker->fCurrent != fMap_Assert.end())
		{
		if (const ZMap_Any* theMap = iWalker->fCurrent->second.second.PGet<ZMap_Any>())
			{
			bool gotAll = true;
			vector<ZVal_Any> subset;
			subset.reserve(theConcreteHead.size());
			size_t offset = iWalker->fBaseOffset;
			for (ConcreteHead::const_iterator
				ii = theConcreteHead.begin(), end = theConcreteHead.end();
				ii != end; ++ii, ++offset)
				{
				const string8& theName = ii->first;
				if (theName.empty())
					{
					// Empty name indicates that we want the Daton itself.
					const ZVal_Any& theVal = iWalker->fCurrent->first;
					ioResults[offset] = theVal;
					subset.push_back(theVal);
					}
				else if (const ZVal_Any* theVal = sPGet(*theMap, theName))
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

			if (gotAll && sQInsert(iWalker->fPriors, subset))
				{
				++iWalker->fCurrent;
				return true;
				}
			}
		++iWalker->fCurrent;
		}

	return false;
	}

} // namespace Dataspace
} // namespace ZooLib
