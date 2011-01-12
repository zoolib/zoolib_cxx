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

#include "zoolib/ZWorkerRunner_CFRunLoop.h"

#if ZCONFIG_API_Enabled(WorkerRunner_CFRunLoop)

#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZVal_CFType.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace { // anonymous

CFAbsoluteTime spSystemAsAbsoluteTime(ZTime iSystemTime)
	{
	return iSystemTime.GetVal()
		+ (ZTime::sNow() - ZTime::sSystem())
		+ ZTime::kEpochDelta_1970_To_2001;
	}

CFAbsoluteTime spDelayAsAbsoluteTime(double iDelay)
	{ return ::CFAbsoluteTimeGetCurrent() + iDelay; }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_CFRunLoop

/**
\class ZWorkerRunner_CFRunLoop
\ingroup Worker
\sa Worker
*/

ZWorkerRunner_CFRunLoop::ZWorkerRunner_CFRunLoop(ZRef<CFRunLoopRef> iRunLoop)
:	fRunLoop(iRunLoop)
	{
	CFRunLoopTimerContext theContext = { 0, this, 0, 0, 0 };
	fRunLoopTimer = Adopt& ::CFRunLoopTimerCreate(
		nullptr, // allocator
		0, // fireDate
		1000000, // interval, needs to be non-zero
		0, // flags
		0, // order
		spCallback,
		&theContext);

	::CFRunLoopAddTimer(fRunLoop, fRunLoopTimer, kCFRunLoopCommonModes);
	}

ZWorkerRunner_CFRunLoop::~ZWorkerRunner_CFRunLoop()
	{}

void ZWorkerRunner_CFRunLoop::Finalize()
	{
	::CFRunLoopTimerInvalidate(fRunLoopTimer);
	ZWorkerRunner::Finalize();
	}

void ZWorkerRunner_CFRunLoop::Wake(ZRef<ZWorker> iWorker)
	{ this->pWake(iWorker, 0); }

void ZWorkerRunner_CFRunLoop::WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime)
	{ this->pWake(iWorker, spSystemAsAbsoluteTime(iSystemTime)); }

void ZWorkerRunner_CFRunLoop::WakeIn(ZRef<ZWorker> iWorker, double iInterval)
	{ this->pWake(iWorker, spDelayAsAbsoluteTime(iInterval)); }

bool ZWorkerRunner_CFRunLoop::IsAwake(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	return ZUtil_STL::sContains(fWorkersMap, iWorker)
		&& fWorkersMap[iWorker] <= ::CFAbsoluteTimeGetCurrent();
	}

bool ZWorkerRunner_CFRunLoop::IsAttached(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	return ZUtil_STL::sContains(fWorkersMap, iWorker);
	}

void ZWorkerRunner_CFRunLoop::Attach(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	if (ZWorkerRunner::pAttachWorker(iWorker))
		{
		fWorkersSet.Insert(iWorker);
		ZUtil_STL::sInsertMustNotContain(1,
			fWorkersMap, iWorker, ::CFAbsoluteTimeGetCurrent() + ZTime::kYear);
		}
	}

// Assume that main thread will become the thread used by that returned
// by CFRunLoopGetMain, which is only available on 10.5+.
static ZRef<ZWorkerRunner_CFRunLoop> spWR = new ZWorkerRunner_CFRunLoop(::CFRunLoopGetCurrent());

ZRef<ZWorkerRunner_CFRunLoop> ZWorkerRunner_CFRunLoop::sMain()
	{ return spWR; }

void ZWorkerRunner_CFRunLoop::pWake(ZRef<ZWorker> iWorker, CFAbsoluteTime iAbsoluteTime)
	{
	ZAcqMtxR acq(fMtx);
	if (ZUtil_STL::sContains(fWorkersMap, iWorker))
		{
		fWorkersMap[iWorker] = iAbsoluteTime;
		this->pTrigger(iAbsoluteTime);
		}	
	}

void ZWorkerRunner_CFRunLoop::pTrigger(CFAbsoluteTime iAbsoluteTime)
	{
	CFAbsoluteTime nextTime = ::CFRunLoopTimerGetNextFireDate(fRunLoopTimer);
	if (iAbsoluteTime <= nextTime || iAbsoluteTime < ::CFAbsoluteTimeGetCurrent())
		::CFRunLoopTimerSetNextFireDate(fRunLoopTimer, iAbsoluteTime);
	}

void ZWorkerRunner_CFRunLoop::pCallback()
	{
	const CFAbsoluteTime now = ::CFAbsoluteTimeGetCurrent();
	// Reset our next fire date now, so any call to pTrigger has a
	// sensible distant value against which to be compared.
	::CFRunLoopTimerSetNextFireDate(fRunLoopTimer, now + 30 * 1e6);

	bool gotEarliestLater = false;
	CFAbsoluteTime earliestLater;

	for (ZSafeSetIter<ZRef<ZWorker> > iter = fWorkersSet;;)
		{
		if (ZQ<ZRef<ZWorker>,false> theNQ = iter.QReadInc())
			{
			break;
			}
		else
			{
			ZRef<ZWorker> theWorker = theNQ.Get();
			ZGuardRMtxR guard(fMtx);
			const CFAbsoluteTime theTime = ZUtil_STL::sGetMustContain(1, fWorkersMap, theWorker);
			guard.Release();

			if (theTime <= now)
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
			else if (!gotEarliestLater || earliestLater > theTime)
				{
				gotEarliestLater = true;
				earliestLater = theTime;
				}
			}
		}

	ZGuardRMtxR guard(fMtx);

	if (gotEarliestLater)
		this->pTrigger(earliestLater);

	if (fWorkersSet.Empty())
		{
		// No workers registered, if this object were a per-thread singleton,
		// this is where we could release it.
		}
	}

void ZWorkerRunner_CFRunLoop::spCallback(CFRunLoopTimerRef iTimer, void* iRefcon)
	{
	if (ZRef<ZWorkerRunner_CFRunLoop> theRunner = static_cast<ZWorkerRunner_CFRunLoop*>(iRefcon))
		theRunner->pCallback();
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(WorkerRunner_CFRunLoop)
