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
#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zooLib/dataspace/RelsWatcher_Relater.h"

namespace ZooLib {
namespace Dataspace {

using namespace ZUtil_STL;

using std::vector;

// =================================================================================================
// MARK: - Registration

class RelsWatcher_Relater::Registration
:	public ZCounted
	{
public:
	Registration(
		const ZRef<RelsWatcher_Relater>& iRWR,
		const ZRef<RelsWatcher::Callable_Changed>& iCallable,
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
		if (ZRef<RelsWatcher_Relater> theRWR = fWeakRef_RWR)
			theRWR->pFinalize(this);
		else
			ZCounted::Finalize();
		}

	int64 fRefcon;

	const ZWeakRef<RelsWatcher_Relater> fWeakRef_RWR;
	const ZRef<RelsWatcher::Callable_Changed> fCallable;
	const ZRef<Expr_Rel> fRel;

	bool fHadResultPrior;

	ZRef<Event> fEvent;
	ZRef<QueryEngine::Result> fResult;
	};

// =================================================================================================
// MARK: - RelsWatcher_Relater

RelsWatcher_Relater::RelsWatcher_Relater(const ZRef<Relater>& iRelater,
	const ZRef<Callable_NeedsUpdate>& iCallable_NeedsUpdate)
:	fRelater(iRelater)
,	fCallable_NeedsUpdate(iCallable_NeedsUpdate)
	{}

void RelsWatcher_Relater::Initialize()
	{
	inherited::Initialize();

	fRelater->SetCallable_RelaterResultsAvailable(
		sCallable(sWeakRef(this), &RelsWatcher_Relater::pCallback_Relater));
	}

ZQ<ZRef<ZCounted> > RelsWatcher_Relater::QCall(
	const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
	const ZRef<Expr_Rel>& iRel)
	{
	ZRef<Registration> theR = new Registration(this, iCallable_Changed, iRel);

	ZGuardMtxR guard(fMtxR);

	sInsertMust(fToAdd, theR.Get());

	if (not fCalled_NeedsUpdate())
		{
		guard.Release();
		sCall(fCallable_NeedsUpdate);
		}

	return ZRef<ZCounted>(theR);
	}

void RelsWatcher_Relater::Update()
	{
	ZGuardMtxR guard(fMtxR);
	fCalled_NeedsUpdate.Reset();

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
			sSet(fMap_RefconToRegistrationX, theRefcon, rr);
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
			Map_RefconToRegistrationX::iterator iterRegistration =
				fMap_RefconToRegistrationX.find(iterQueryResults->GetRefcon());

			if (fMap_RefconToRegistrationX.end() == iterRegistration)
				continue;

			ZRef<Registration> theRegistration = iterRegistration->second;
			theRegistration->fHadResultPrior = bool(theRegistration->fResult);
			theRegistration->fResult = iterQueryResults->GetResult();
			theRegistration->fEvent = iterQueryResults->GetEvent();
			if (not theRegistration->fEvent)
				ZLOGTRACE(eDebug);
			changes.push_back(theRegistration);
			}
		guard.Release();

		foreacha (rr, changes)
			{ sCall(rr->fCallable, rr, rr->fEvent, rr->fResult, not rr->fHadResultPrior); }
		}
	}

void RelsWatcher_Relater::pCallback_Relater(ZRef<Relater> iRelater)
	{
	ZGuardMtxR guard(fMtxR);
	if (not fCalled_NeedsUpdate())
		{
		guard.Release();
		sCall(fCallable_NeedsUpdate);
		}
	}

void RelsWatcher_Relater::pFinalize(Registration* iRegistration)
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

		sEraseMust(fMap_RefconToRegistrationX, theRefcon);

		sInsertMust(fToRemove, theRefcon);

		if (not fCalled_NeedsUpdate())
			{
			guard.Release();
			sCall(fCallable_NeedsUpdate);
			}
		}
	}

} // namespace Dataspace
} // namespace ZooLib
