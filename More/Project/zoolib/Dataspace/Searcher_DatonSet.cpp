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
#include "zoolib/Util_STL_vector.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/Dataspace/Searcher_DatonSet.h"

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

//namespace QE = QueryEngine;
//namespace RA = RelationalAlgebra;

// =================================================================================================
#pragma mark -
#pragma mark Searcher_DatonSet

Searcher_DatonSet::Searcher_DatonSet(const vector<IndexSpec>& iIndexSpecs)
:	fEvent(Event::sZero())
,	fSearcher_Datons(new Searcher_Datons(iIndexSpecs))
	{}

Searcher_DatonSet::~Searcher_DatonSet()
	{}

void Searcher_DatonSet::Initialize()
	{
	Searcher::Initialize();
	fSearcher_Datons->SetCallable_SearcherResultsAvailable(
		sCallable(sWeakRef(this), &Searcher_DatonSet::pSearcherResultsAvailable));
	}

void Searcher_DatonSet::pSearcherResultsAvailable(ZRef<Searcher>)
	{ Searcher::pTriggerSearcherResultsAvailable(); }

bool Searcher_DatonSet::Intersects(const RelHead& iRelHead)
	{ return true; }

void Searcher_DatonSet::ModifyRegistrations(
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{ fSearcher_Datons->ModifyRegistrations(iAdded, iAddedCount, iRemoved, iRemovedCount); }

void Searcher_DatonSet::CollectResults(vector<SearchResult>& oChanged)
	{
	Searcher::pCollectResultsCalled();

	this->pPull();

	fSearcher_Datons->CollectResults(oChanged);
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

	vector<Daton> asserted, retracted;

	foreachv (ZRef<DatonSet::Callable_PullFrom> theCallable, theCallables_PullFrom)
		{
		guard.Release();

		ZRef<DatonSet::Deltas> theDeltas = theCallable->Call(fEvent);

		guard.Acquire();

		foreachi (iter, theDeltas->GetVector())
			fEvent = fEvent->Joined(iter->first);

		const Vector_Event_Delta_t& theVector = theDeltas->GetVector();

		bool anyChanges = false;

		foreachi (iterVector, theVector)
			{
			const ZRef<Event>& theEvent = iterVector->first;
			const map<Daton, bool>& theStatements = iterVector->second->GetStatements();

			foreachi (iterStmts, theStatements)
				{
				const Daton& theDaton = iterStmts->first;

				Map_Assert::iterator lbAssert = fMap_Assert.lower_bound(theDaton);

				Map_Retract::iterator lbRetract = fMap_Retract.lower_bound(theDaton);

				if (iterStmts->second)
					{
					// It's an assert
					if (lbAssert != fMap_Assert.end() and lbAssert->first == theDaton)
						{
						// It's in fMap_Assert.
						ZAssert(lbRetract == fMap_Retract.end() || lbRetract->first != theDaton);
						if (sIsBefore(lbAssert->second, theEvent))
							{
							// It's more recent.
							lbAssert->second = theEvent;
							}
						}
					else if (lbRetract != fMap_Retract.end() and lbRetract->first == theDaton)
						{
						// It's in fMap_Retract.
						if (sIsBefore(lbRetract->second, theEvent))
							{
							// It's more recent.
							fMap_Retract.erase(lbRetract);

							fMap_Assert.insert(lbAssert, make_pair(theDaton, theEvent));

							sPushBack(asserted, theDaton);
							anyChanges = true;
							}
						}
					else
						{
						// It's not previously known.
						fMap_Assert.insert(lbAssert, make_pair(theDaton, theEvent));

						sPushBack(asserted, theDaton);
						anyChanges = true;
						}
					}
				else
					{
					// It's a retract.
					if (lbAssert != fMap_Assert.end() and lbAssert->first == theDaton)
						{
						// It's in fMap_Assert.
						ZAssert(lbRetract == fMap_Retract.end() || lbRetract->first != theDaton);
						if (sIsBefore(lbAssert->second, theEvent))
							{
							// It's more recent.
							sPushBack(retracted, theDaton);
							anyChanges = true;
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

	fSearcher_Datons->MakeChanges(sFirstOrNil(asserted), asserted.size(),
		sFirstOrNil(retracted), retracted.size());
	}

} // namespace Dataspace
} // namespace ZooLib
