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
#include "zoolib/ZCaller_EventLoop.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZThread_pthread_more.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zoolib/dataspace/Stew.h"
#include "zoolib/dataspace/Relater_Searcher.h"

namespace ZooLib {
namespace Dataspace {

using namespace Dataspace;
using namespace ZUtil_STL;

using std::vector;

// =================================================================================================
// MARK: - Stew::Caller_EventLoop_Callable

class Stew::Caller_EventLoop_Callable : public ZCaller_EventLoop
	{
public:
	typedef ZCallable<bool()> Callable_Trigger;

	Caller_EventLoop_Callable(const ZRef<Callable_Trigger>& iCallable_Trigger)
	:	fCallable_Trigger(iCallable_Trigger)
		{}

// From ZCaller_EventLoop
	virtual bool pTrigger()
		{ return sDCall(false, fCallable_Trigger); }

// Our protocol
	void DoCalls()
		{ ZCaller_EventLoop::pCall(); }

private:
	ZRef<Callable_Trigger> fCallable_Trigger;
	};

// =================================================================================================
// MARK: - Registration_Real

class Stew::Registration_Real : public Stew::Registration
	{
public:
	Registration_Real(
		const ZRef<Stew>& iStew,
		const ZRef<Callable_RegistrationChanged>& iCallable,
		const ZRef<Expr_Rel>& iRel)
	:	fRefcon(0)
	,	fWeakRef_Stew(iStew)
	,	fCallable(iCallable)
	,	fRel(iRel)
	,	fHadResultPrior(false)
		{}

	~Registration_Real()
		{}

	void Finalize()
		{
		if (ZRef<Stew> theStew = fWeakRef_Stew)
			theStew->pFinalize(this);
		else
			Registration::Finalize();
		}

	int64 fRefcon;

	const ZWeakRef<Stew> fWeakRef_Stew;
	const ZRef<Callable_RegistrationChanged> fCallable;
	const ZRef<Expr_Rel> fRel;

	bool fHadResultPrior;

	ZRef<Event> fEvent;
	ZRef<QueryEngine::Result> fResult;
	};

// =================================================================================================
// MARK: - Stew

Stew::Stew(ZRef<WrappedDatonSet> iWDS_Parent)
:	fWDS_Parent(iWDS_Parent)
,	fKeepWorking(false)
,	fNextRefcon(1)
	{}

void Stew::Initialize()
	{
	ZCounted::Initialize();

	const ZWeakRef<Stew> weak_this = sWeakRef(this);

	fCaller = new Caller_EventLoop_Callable(sCallable(weak_this, &Stew::pTriggerEventLoop));

	fWDS = fWDS_Parent->Spawn(sCallable(weak_this, &Stew::pUpdateNeeded));
	fWDS_Parent.Clear();

	ZRef<Searcher_DatonSet> theSDS = new Searcher_DatonSet(fWDS->GetDatonSet_Committed());
	fRelater = new Relater_Searcher(theSDS);

	fRelater->SetCallable_ResultsAvailable(sCallable(weak_this, &Stew::pCallback_Relater));

	fWorker = new ZWorker(
		sCallable(weak_this, &Stew::pWork),
		sCallable(weak_this, &Stew::pDetached));
	}

void Stew::Finalize()
	{
	fRelater->SetCallable_ResultsAvailable(null);
	ZCounted::Finalize();
	}

void Stew::Start(ZRef<ZCaller> iCaller)
	{
	ZAcqMtxR acq(fMtxR);
	ZAssert(not fWorker->IsAttached());
	ZAssert(not fKeepWorking);
	fKeepWorking = true;
	fWorker->Attach(iCaller);
	}

void Stew::Stop()
	{
	ZAcqMtxR acq(fMtxR);
	fKeepWorking = false;
	fCnd.Broadcast();
	fWorker->Wake();
	}

void Stew::StopWait()
	{
	ZAcqMtxR acq(fMtxR);
	fKeepWorking = false;
	fCnd.Broadcast();
	fWorker->Wake();
	while (fWorker->IsAttached())
		fCnd.Wait(fMtxR);
	}

ZRef<Stew::Registration> Stew::Register(
	const ZRef<Callable_RegistrationChanged>& iCallable, const ZRef<Expr_Rel>& iRel)
	{
	ZRef<Registration_Real> theR = new Registration_Real(this, iCallable, iRel);

	ZAcqMtxR acq(fMtxR);
	sInsertMust(fToAdd, theR.Get());
	fWorker->Wake();
	return theR;
	}

ZRef<WrappedDatonSet> Stew::GetWrappedDatonSet()
	{ return fWDS; }

ZRef<ZCaller> Stew::GetCaller()
	{ return fCaller; }

ZRef<ZWorker> Stew::GetWorker()
	{ return fWorker; }

void Stew::pDetached(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtxR);
	fCnd.Broadcast();
	}

bool Stew::pWork(ZRef<ZWorker> iWorker)
	{
	const std::string description = sObjPtrStr(this) + ", with " + sObjPtrStr(fWDS.Get());

	ZThread::SaveSetRestoreName ssrn("Stew::pWork, " + description);

	ZGuardMtxR guard(fMtxR);
	if (sNotEmpty(fToAdd) || sNotEmpty(fToRemove))
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

		guard.Acquire();
		}

	// Do any work
	fCaller->DoCalls();

	// Update our WrappedDatonset (its DatonSet_Committed will then include changes to its
	// DatonSet_Active, as well as changes pulled from other WDS).
	fWDS->Update();

	// Pick up any results
	guard.Release();
	vector<QueryResult> theQueryResults;
	fRelater->CollectResults(theQueryResults);

	if (sNotEmpty(theQueryResults))
		{
		vector<ZRef<Registration_Real> > changes;

		guard.Acquire();

		foreachi (iterQueryResults, theQueryResults)
			{
			Map_RefconToRegistration::iterator iterRegistration =
				fMap_RefconToRegistration.find(iterQueryResults->GetRefcon());

			if (fMap_RefconToRegistration.end() == iterRegistration)
				continue;

			ZRef<Registration_Real> theRegistration = iterRegistration->second;
			theRegistration->fHadResultPrior = bool(theRegistration->fResult);
			theRegistration->fResult = iterQueryResults->GetResult();
			theRegistration->fEvent = iterQueryResults->GetEvent();
			changes.push_back(theRegistration);
			}
		guard.Release();

		foreacha (rr, changes)
			{ sCall(rr->fCallable, rr, rr->fResult, not rr->fHadResultPrior); }
		}

	guard.Acquire();

	// We're not done until Stop has been called, and we have no pending
	// changes in registration. If we're destroyed with registrations active, then
	// our underlying relater will probably complain, but preventing detach until
	// requested changes have been processed will allow the requests of well-behaved
	// clients to have the correct effect.
	return fKeepWorking || sNotEmpty(fToAdd) || sNotEmpty(fToRemove);
	}

void Stew::pUpdateNeeded()
	{ fWorker->Wake(); }

bool Stew::pTriggerEventLoop()
	{
	ZAcqMtxR acq(fMtxR);
	if (ZRef<ZWorker> theWorker = fWorker)
		{
		theWorker->Wake();
		return true;
		}
	return false;
	}

void Stew::pCallback_Relater(ZRef<Relater> iRelater)
	{ fWorker->Wake(); }

void Stew::pFinalize(Registration_Real* iRegistration)
	{
	ZGuardMtxR guard(fMtxR);

	if (not iRegistration->FinishFinalize())
		return;

	if (not sQErase(fToAdd, iRegistration))
		{
		int64 const theRefcon = iRegistration->fRefcon;

		sEraseMust(fMap_RefconToRegistration, theRefcon);

		sInsertMust(fToRemove, theRefcon);

		fWorker->Wake();
		}

	delete iRegistration;
	}

} // namespace Dataspace
} // namespace ZooLib
