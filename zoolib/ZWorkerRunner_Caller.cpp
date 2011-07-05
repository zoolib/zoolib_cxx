/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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
#include "zoolib/ZWorkerRunner_Caller.h"
#include "zoolib/ZUtil_STL_map.h"

namespace ZooLib {

using std::map;

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_Caller::Worker_Waker

class ZWorkerRunner_Caller::Worker_Waker : public ZWorker
	{
public:
	Worker_Waker(ZRef<ZWorkerRunner_Caller> iRunner)
	:	fRunner(iRunner)
		{}

	virtual bool Work()
		{
		if (ZRef<ZWorkerRunner_Caller> theRunner = fRunner)
			{
			theRunner->pTrigger();
			return true;
			}
		return false;
		}

	ZWeakRef<ZWorkerRunner_Caller> fRunner;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_Caller

/**
\class ZWorkerRunner_Caller
\ingroup Worker
\sa Worker
*/

ZWorkerRunner_Caller::ZWorkerRunner_Caller(ZRef<ZCaller> iCaller)
:	fCaller(iCaller)
,	fTriggered(false)
	{}

ZWorkerRunner_Caller::~ZWorkerRunner_Caller()
	{}

void ZWorkerRunner_Caller::Initialize()
	{
	ZWorkerRunner::Initialize();

	fCallable_Callback = MakeCallable(MakeWeakRef(this), &ZWorkerRunner_Caller::pCallback);
	}

void ZWorkerRunner_Caller::Finalize()
	{
	{
	ZAcqMtxR acq(fMtx);
	if (ZRef<ZWorker> theWorker = fWorker_Waker)
		{
		fWorker_Waker->fRunner.Clear();
		fWorker_Waker.Clear();
		theWorker->Wake();
		}
	}

	ZWorkerRunner::Finalize();
	}

void ZWorkerRunner_Caller::Wake(ZRef<ZWorker> iWorker)
	{ this->pWakeAt(iWorker, 0); }

void ZWorkerRunner_Caller::WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime)
	{ this->pWakeAt(iWorker, iSystemTime); }

void ZWorkerRunner_Caller::WakeIn(ZRef<ZWorker> iWorker, double iInterval)
	{ this->pWakeAt(iWorker, ZTime::sSystem() + iInterval); }

bool ZWorkerRunner_Caller::IsAwake(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	if (ZQ<ZTime> theQ = ZUtil_STL::sQGet(fWorkersMap, iWorker))
		return theQ.Get() <= ZTime::sSystem();
	return false;
	}

bool ZWorkerRunner_Caller::IsAttached(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	return ZUtil_STL::sContains(fWorkersMap, iWorker);
	}

void ZWorkerRunner_Caller::Attach(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	if (ZWorkerRunner::pAttachWorker(iWorker))
		{
		fWorkersSet.Insert(iWorker);
		ZUtil_STL::sInsertMustNotContain(1,
			fWorkersMap, iWorker, ZTime::sSystem() + ZTime::kYear);
		}
	}

ZRef<ZCaller> ZWorkerRunner_Caller::GetCaller()
	{ return fCaller; }

void ZWorkerRunner_Caller::pWakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime)
	{
	ZAcqMtxR acq(fMtx);
	if (ZUtil_STL::sSetIfContains(fWorkersMap, iWorker, iSystemTime))
		{
		if (iSystemTime <= ZTime::sSystem())
			{
			this->pTrigger();
			}
		else
			{
			if (!fWorker_Waker)
				{
				fWorker_Waker = new Worker_Waker(this);
				sStartWorkerRunner(fWorker_Waker);
				}
			fWorker_Waker->WakeAt(iSystemTime);
			}
		}
	}

void ZWorkerRunner_Caller::pCallback()
	{
	// Not all subclasses invoke this through a ZRef, so ensure our reference remains valid.
	ZRef<ZWorkerRunner_Caller> self = this;

	ZGuardRMtxR guard(fMtx);
	fTriggered = false;
	guard.Release();

	for (ZSafeSetIter<ZRef<ZWorker> > iter = fWorkersSet;;)
		{
		if (ZQ<ZRef<ZWorker>,false> theNQ = iter.QReadInc())
			{ break; }
		else
			{
			ZRef<ZWorker> theWorker = theNQ.Get();
			guard.Acquire();
			const ZTime theTime = ZUtil_STL::sGetMustContain(1, fWorkersMap, theWorker);
			guard.Release();

			if (theTime <= ZTime::sSystem())
				{
				if (!this->pInvokeWork(theWorker))
					{
					guard.Acquire();
					fWorkersSet.Erase(theWorker);
					ZUtil_STL::sEraseMustContain(1, fWorkersMap, theWorker);
					guard.Release();
					ZWorkerRunner::pDetachWorker(theWorker);
					}
				}
			}
		}

	if (fWorkersSet.Empty())
		{
		if (ZRef<ZWorker> theWorker = fWorker_Waker)
			{
			fWorker_Waker->fRunner.Clear();
			fWorker_Waker.Clear();
			theWorker->Wake();
			}
		}
	}

void ZWorkerRunner_Caller::pTrigger()
	{
	ZAcqMtxR acq(fMtx);
	if (not fTriggered++)
		fCaller->Call(fCallable_Callback);
	}

} // namespace ZooLib
