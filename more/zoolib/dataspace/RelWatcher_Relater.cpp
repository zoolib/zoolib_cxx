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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zooLib/dataspace/RelWatcher_Relater.h"

namespace ZooLib {
namespace Dataspace {

using namespace ZUtil_STL;

using std::vector;

// =================================================================================================
// MARK: - Registration

class RelWatcher_Relater::Registration
:	public ZCounted
	{
public:
	Registration(
		const ZRef<RelWatcher_Relater>& iRWR,
		const ZRef<RelWatcher::Callable_Changed>& iCallable,
		const ZRef<Expr_Rel>& iRel)
	:	fRefcon(0)
	,	fWeakRef_RWR(iRWR)
	,	fCallable(iCallable)
	,	fRel(iRel)
	,	fHadResultPrior(false)
		{}

	~Registration()
		{}

	void Finalize()
		{
		if (ZRef<RelWatcher_Relater> theRWR = fWeakRef_RWR)
			theRWR->pFinalize(this);
		else
			ZCounted::Finalize();
		}

	int64 fRefcon;

	const ZWeakRef<RelWatcher_Relater> fWeakRef_RWR;
	const ZRef<RelWatcher::Callable_Changed> fCallable;
	const ZRef<Expr_Rel> fRel;

	bool fHadResultPrior;

	ZRef<Event> fEvent;
	ZRef<QueryEngine::Result> fResult;
	};

// =================================================================================================
// MARK: - RelWatcher_Relater

RelWatcher_Relater::RelWatcher_Relater(const ZRef<Relater>& iRelater)
:	fRelater(iRelater)
,	fCalled_NeedsUpdate(false)
	{}

ZQ<ZRef<ZCounted> > RelWatcher_Relater::QCall(
	const ZRef<RelWatcher::Callable_Changed>& iCallable_Changed,
	const ZRef<Expr_Rel>& iRel)
	{
	ZRef<Registration> theR = new Registration(this, iCallable_Changed, iRel);

	ZGuardMtxR guard(fMtxR);

	sInsertMust(fToAdd, theR.Get());

	if (not sGetSet(fCalled_NeedsUpdate, true))
		{
		guard.Release();
		sCall(fCallable_NeedsUpdate);
		}

	return ZRef<ZCounted>(theR);
	}

void RelWatcher_Relater::SetCallable_NeedsUpdate(
	const ZRef<Callable_NeedsUpdate>& iCallable_NeedsUpdate)
	{
	ZGuardMtxR guard(fMtxR);
	fCallable_NeedsUpdate = iCallable_NeedsUpdate;
	fRelater->SetCallable_ResultsAvailable(sCallable(sWeakRef(this), &RelWatcher_Relater::pCallback_Relater));
	}

void RelWatcher_Relater::Update()
	{
	ZGuardMtxR guard(fMtxR);
	fCalled_NeedsUpdate = false;

	if (sIsEmpty(fToAdd) && sIsEmpty(fToRemove))
		{
		guard.Release();
		}
	else
		{
		vector<AddedQuery> added;
		foreacha (rr, fToAdd)
			{
			const int64 theRefcon = fNextRefcon++;
			rr->fRefcon = theRefcon;
			sSet(fMap_RefconToRegistration, theRefcon, rr);
			added.push_back(AddedQuery(theRefcon, rr->fRel));
			}
		fToAdd.clear();

		vector<int64> removed(fToRemove.begin(), fToRemove.end());
		fToRemove.clear();
		guard.Release();

		fRelater->ModifyRegistrations(
			ZUtil_STL::sFirstOrNil(added), added.size(),
			ZUtil_STL::sFirstOrNil(removed), removed.size());

		}

	// Pick up any results
	vector<QueryResult> theQueryResults;
	fRelater->CollectResults(theQueryResults);

	if (sNotEmpty(theQueryResults))
		{
		vector<ZRef<Registration> > changes;

		guard.Acquire();

		foreachi (iterQueryResults, theQueryResults)
			{
			Map_RefconToRegistration::iterator iterRegistration =
				fMap_RefconToRegistration.find(iterQueryResults->GetRefcon());

			if (fMap_RefconToRegistration.end() == iterRegistration)
				continue;

			ZRef<Registration> theRegistration = iterRegistration->second;
			theRegistration->fHadResultPrior = bool(theRegistration->fResult);
			theRegistration->fResult = iterQueryResults->GetResult();
			theRegistration->fEvent = iterQueryResults->GetEvent();
			changes.push_back(theRegistration);
			}
		guard.Release();

		foreacha (rr, changes)
			{ sCall(rr->fCallable, rr, rr->fEvent, rr->fResult, not rr->fHadResultPrior); }
		}
	}

void RelWatcher_Relater::pCallback_Relater(ZRef<Relater> iRelater)
	{
	ZGuardMtxR guard(fMtxR);
	if (not sGetSet(fCalled_NeedsUpdate, true))
		{
		guard.Release();
		sCall(fCallable_NeedsUpdate);
		}
	}

void RelWatcher_Relater::pFinalize(Registration* iRegistration)
	{
	ZGuardMtxR guard(fMtxR);

	if (not iRegistration->FinishFinalize())
		return;

	if (sQErase(fToAdd, iRegistration))
		{
		delete iRegistration;
		}
	else
		{
		int64 const theRefcon = iRegistration->fRefcon;

		sEraseMust(fMap_RefconToRegistration, theRefcon);

		sInsertMust(fToRemove, theRefcon);

		if (not sGetSet(fCalled_NeedsUpdate, true))
			{
			guard.Release();
			sCall(fCallable_NeedsUpdate);
			}
		}
	}

} // namespace Dataspace
} // namespace ZooLib
