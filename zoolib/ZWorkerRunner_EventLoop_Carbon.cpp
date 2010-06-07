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

#include "zoolib/ZWorkerRunner_EventLoop_Carbon.h"

#if ZCONFIG_SPI_Enabled(Carbon64)

#include "zoolib/ZLog.h"
#include "zoolib/ZSafeRef.h"
#include "zoolib/ZUtil_CarbonEvents.h"
#include "zoolib/ZUtil_STL.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_EventLoop_Carbon::Worker_Waker

class ZWorkerRunner_EventLoop_Carbon::Worker_Waker : public ZWorker
	{
public:
	Worker_Waker(ZRef<ZWorkerRunner_EventLoop_Carbon> iRunner);

	virtual bool Work();

	ZWeakRef<ZWorkerRunner_EventLoop_Carbon> fRunner;
	};

ZWorkerRunner_EventLoop_Carbon::Worker_Waker::Worker_Waker(
	ZRef<ZWorkerRunner_EventLoop_Carbon> iRunner)
:	fRunner(iRunner)
	{}

bool ZWorkerRunner_EventLoop_Carbon::Worker_Waker::Work()
	{
	if (ZRef<ZWorkerRunner_EventLoop_Carbon> theRunner = fRunner)
		return theRunner->pTriggerInvokeWork();
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_EventLoop_Carbon

/**
\class ZWorkerRunner_EventLoop_Carbon
\ingroup Worker
\sa Worker
*/

static ZSafeRef<ZWorkerRunner_EventLoop_Carbon> spRunner;

static ZRef<ZWorkerRunner_EventLoop_Carbon> spGetRunner()
	{
	ZRef<ZWorkerRunner_EventLoop_Carbon> theRunner = spRunner;
	if (!theRunner)
		theRunner = new ZWorkerRunner_EventLoop_Carbon;

	return spRunner;
	}

ZWorkerRunner_EventLoop_Carbon::ZWorkerRunner_EventLoop_Carbon()
:	fInvokeCallbackTriggered(false)
	{
	ZAssert(!spRunner);
	spRunner = this;
	fWorker_Waker = new Worker_Waker(this);
	sStartWorkerRunner(fWorker_Waker);
	}

ZWorkerRunner_EventLoop_Carbon::~ZWorkerRunner_EventLoop_Carbon()
	{
	ZAssert(!spRunner);

	ZWeakReferee::pDetachProxy();

	fWorker_Waker->Wake();
	}

void ZWorkerRunner_EventLoop_Carbon::Wake(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	fWorkersMap[iWorker] = 0;
	fWorker_Waker->Wake();
	}

void ZWorkerRunner_EventLoop_Carbon::WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime)
	{
	ZAcqMtxR acq(fMtx);
	fWorkersMap[iWorker] = iSystemTime;
	fWorker_Waker->Wake();
	}

void ZWorkerRunner_EventLoop_Carbon::WakeIn(ZRef<ZWorker> iWorker, double iInterval)
	{
	ZAcqMtxR acq(fMtx);
	fWorkersMap[iWorker] = ZTime::sSystem() + iInterval;
	fWorker_Waker->Wake();
	}

bool ZWorkerRunner_EventLoop_Carbon::IsAwake(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	return fWorkersMap[iWorker] <= ZTime::sSystem();
	}

void ZWorkerRunner_EventLoop_Carbon::sStartWorker(ZRef<ZWorker> iWorker)
	{
	if (ZRef<ZWorkerRunner_EventLoop_Carbon> theRunner = spGetRunner())
		theRunner->pStartWorker(iWorker);
	}

void ZWorkerRunner_EventLoop_Carbon::pStartWorker(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	ZWorkerRunner::pAttachWorker(iWorker);

	fWorkersSet.Insert(iWorker);
	ZUtil_STL::sInsertMustNotContain(1, fWorkersMap, iWorker, ZTime(0));
	this->pTriggerInvokeWork();
	}

void ZWorkerRunner_EventLoop_Carbon::pInvokeWork()
	{
	ZRef<ZWorkerRunner_EventLoop_Carbon> self = this;
	ZGuardRMtxR guard(fMtx);
	fInvokeCallbackTriggered = false;
	guard.Release();

	for (ZSafeSetIter<ZRef<ZWorker> > iter = fWorkersSet;;)
		{
		if (ZRef<ZWorker> theWorker = iter.ReadInc())
			{
			guard.Acquire();
			ZTime theTime = fWorkersMap[theWorker];
			guard.Release();

			if (theTime <= ZTime::sSystem())
				{
				if (! theWorker->Work())
					{
					guard.Acquire();
					fWorkersSet.Erase(theWorker);
					ZUtil_STL::sEraseMustContain(1, fWorkersMap, theWorker);
					ZWorkerRunner::pDetachWorker(theWorker);
					guard.Release();
					}
				}
			}
		else
			{
			break;
			}
		}
	guard.Acquire();
	if (fWorkersSet.Empty())
		spRunner.Clear();
	}

void ZWorkerRunner_EventLoop_Carbon::spInvokeWork(void* iRefcon)
	{
	if (ZRef<ZWorkerRunner_EventLoop_Carbon> theRunner =
		static_cast<ZWorkerRunner_EventLoop_Carbon*>(iRefcon))
		{
		theRunner->pInvokeWork();
		}
	}

bool ZWorkerRunner_EventLoop_Carbon::pTriggerInvokeWork()
	{
	ZAcqMtxR acq(fMtx);
	if (!fInvokeCallbackTriggered)
		{
		fInvokeCallbackTriggered = true;
		ZUtil_CarbonEvents::sInvokeOnMainThread(spInvokeWork, this);
		}
	return true;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Carbon64)
