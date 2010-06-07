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

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include "zoolib/ZUtil_STL.h"

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
	fRunLoopTimer = Adopt(::CFRunLoopTimerCreate(
		nullptr, // allocator
		0, // fireDate
		1000000, // interval, needs to be non-zero
		0, // flags
		0, // order
		spRunLoopTimerCallBack,
		&theContext));
	::CFRunLoopAddTimer(fRunLoop, fRunLoopTimer, kCFRunLoopCommonModes);
	}

ZWorkerRunner_CFRunLoop::~ZWorkerRunner_CFRunLoop()
	{
	::CFRunLoopTimerInvalidate(fRunLoopTimer);
	}

void ZWorkerRunner_CFRunLoop::Wake(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	fWorkersMap[iWorker] = 0;
	this->pTrigger(0);
	}

void ZWorkerRunner_CFRunLoop::WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime)
	{
	const CFAbsoluteTime theAT = spSystemAsAbsoluteTime(iSystemTime);
	ZAcqMtxR acq(fMtx);
	fWorkersMap[iWorker] = theAT;
	this->pTrigger(theAT);
	}

void ZWorkerRunner_CFRunLoop::WakeIn(ZRef<ZWorker> iWorker, double iInterval)
	{
	const CFAbsoluteTime theAT = spDelayAsAbsoluteTime(iInterval);
	ZAcqMtxR acq(fMtx);
	fWorkersMap[iWorker] = theAT;
	this->pTrigger(theAT);
	}

bool ZWorkerRunner_CFRunLoop::IsAwake(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	return fWorkersMap[iWorker] <= ::CFAbsoluteTimeGetCurrent();
	}

void ZWorkerRunner_CFRunLoop::Add(ZRef<ZWorker> iWorker)
	{
	ZAcqMtxR acq(fMtx);
	ZWorkerRunner::pAttachWorker(iWorker);

	fWorkersSet.Insert(iWorker);
	ZUtil_STL::sInsertMustNotContain(1, fWorkersMap, iWorker, CFAbsoluteTime(0));
	this->pTrigger(0);
	}

void ZWorkerRunner_CFRunLoop::pTrigger(CFAbsoluteTime iAbsoluteTime)
	{
	CFAbsoluteTime theTime = CFRunLoopTimerGetNextFireDate(fRunLoopTimer);
	if (iAbsoluteTime <= theTime)
		::CFRunLoopTimerSetNextFireDate(fRunLoopTimer, iAbsoluteTime);
	}

void ZWorkerRunner_CFRunLoop::pRunLoopTimerCallBack()
	{
	ZRef<ZWorkerRunner_CFRunLoop> self = this;

	const CFAbsoluteTime now = ::CFAbsoluteTimeGetCurrent();

	bool gotEarliestLater = false;
	CFAbsoluteTime earliestLater;

	for (ZSafeSetIter<ZRef<ZWorker> > iter = fWorkersSet;;)
		{
		if (ZRef<ZWorker> theWorker = iter.ReadInc())
			{
			ZGuardRMtxR guard(fMtx);
			const CFAbsoluteTime theTime = fWorkersMap[theWorker];
			guard.Release();

			if (theTime <= now)
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
			else if (!gotEarliestLater || earliestLater > theTime)
				{
				gotEarliestLater = true;
				earliestLater = theTime;
				}
			}
		else
			{
			break;
			}
		}

	ZGuardRMtxR guard(fMtx);

	if (gotEarliestLater)
		this->pTrigger(earliestLater);

	if (fWorkersSet.Empty())
		{}
	}

void ZWorkerRunner_CFRunLoop::spRunLoopTimerCallBack(CFRunLoopTimerRef iTimer, void* iRefcon)
	{
	if (ZRef<ZWorkerRunner_CFRunLoop> theRunner =
		static_cast<ZWorkerRunner_CFRunLoop*>(iRefcon))
		{
		theRunner->pRunLoopTimerCallBack();
		}
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)
