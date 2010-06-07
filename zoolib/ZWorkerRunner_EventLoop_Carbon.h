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

#ifndef __ZWorkerRunner_EventLoop_Carbon__
#define __ZWorkerRunner_EventLoop_Carbon__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZSafeSet.h"
#include "zoolib/ZWorker.h"

#include <map>

#if ZCONFIG_SPI_Enabled(Carbon64)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_EventLoop_Carbon

class ZWorkerRunner_EventLoop_Carbon
:	public ZWorkerRunner
	{
public:
	ZWorkerRunner_EventLoop_Carbon();
	virtual ~ZWorkerRunner_EventLoop_Carbon();

// From ZWorkerRunner
	virtual void Wake(ZRef<ZWorker> iWorker);
	virtual void WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime);
	virtual void WakeIn(ZRef<ZWorker> iWorker, double iInterval);
	virtual bool IsAwake(ZRef<ZWorker> iWorker);

// Our protocol
	static void sStartWorker(ZRef<ZWorker> iWorker);

private:
	class Worker_Waker;
	friend class Worker_Waker;

	void pStartWorker(ZRef<ZWorker> iWorker);

	void pInvokeWork();
	static void spInvokeWork(void* iRefcon);

	bool pTriggerInvokeWork();

	ZMtxR fMtx;
	ZCnd fCnd;
	bool fInvokeCallbackTriggered;
	ZRef<Worker_Waker> fWorker_Waker;
	std::map<ZRef<ZWorker>, ZTime> fWorkersMap;
	ZSafeSet<ZRef<ZWorker> > fWorkersSet;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Carbon64)

#endif // __ZWorkerRunner_EventLoop_Carbon__
