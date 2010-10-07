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

#ifndef __ZWorkerRunner_CFRunLoop__
#define __ZWorkerRunner_CFRunLoop__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZCONFIG_API.h"

#include "zoolib/ZSafeSet.h"
#include "zoolib/ZWorker.h"

#include <map>

// CFRunLoop is not available in CW's headers
#ifndef ZCONFIG_API_Avail__WorkerRunner_CFRunLoop
#	if ZCONFIG_SPI_Enabled(CoreFoundation) && !ZCONFIG(Compiler,CodeWarrior)
#		define ZCONFIG_API_Avail__WorkerRunner_CFRunLoop 1
#	endif
#endif

#ifndef ZCONFIG_API_Avail__WorkerRunner_CFRunLoop
#	define ZCONFIG_API_Avail__WorkerRunner_CFRunLoop 0
#endif

#ifndef ZCONFIG_API_Desired__WorkerRunner_CFRunLoop
#	define ZCONFIG_API_Desired__WorkerRunner_CFRunLoop 1
#endif

#if ZCONFIG_API_Enabled(WorkerRunner_CFRunLoop)

#include ZMACINCLUDE2(CoreFoundation,CFRunLoop.h)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_CFRunLoop

class ZWorkerRunner_CFRunLoop
:	public ZWorkerRunner
	{
public:
	ZWorkerRunner_CFRunLoop(ZRef<CFRunLoopRef> iRunLoop);
	virtual ~ZWorkerRunner_CFRunLoop();

// From ZCounted via ZWorkerRunner
	virtual void Finalize();

// From ZWorkerRunner
	virtual void Wake(ZRef<ZWorker> iWorker);
	virtual void WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime);
	virtual void WakeIn(ZRef<ZWorker> iWorker, double iInterval);
	virtual bool IsAwake(ZRef<ZWorker> iWorker);
	virtual bool IsAttached(ZRef<ZWorker> iWorker);

// Our protocol
	void Add(ZRef<ZWorker> iWorker);

	static ZRef<ZWorkerRunner_CFRunLoop> sMain();

private:
	void pWake(ZRef<ZWorker> iWorker, CFAbsoluteTime iAbsoluteTime);
	void pTrigger(CFAbsoluteTime iAbsoluteTime);

	void pRunLoopTimerCallBack();
	static void spRunLoopTimerCallBack(CFRunLoopTimerRef iTimer, void* iRefcon);

	ZRef<CFRunLoopRef> fRunLoop;
	ZRef<CFRunLoopTimerRef> fRunLoopTimer;
	ZMtxR fMtx;
	ZCnd fCnd;
	std::map<ZRef<ZWorker>, CFAbsoluteTime> fWorkersMap;
	ZSafeSet<ZRef<ZWorker> > fWorkersSet;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(WorkerRunner_CFRunLoop)
#endif // __ZWorkerRunner_CFRunLoop__
