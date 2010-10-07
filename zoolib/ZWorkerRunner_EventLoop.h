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

#ifndef __ZWorkerRunner_EventLoop__
#define __ZWorkerRunner_EventLoop__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZSafeSet.h"
#include "zoolib/ZWorker.h"

#include <map>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_EventLoop

class ZWorkerRunner_EventLoop
:	public ZWorkerRunner
	{
public:
	ZWorkerRunner_EventLoop();
	virtual ~ZWorkerRunner_EventLoop();

// From ZWorkerRunner
	virtual void Wake(ZRef<ZWorker> iWorker);
	virtual void WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime);
	virtual void WakeIn(ZRef<ZWorker> iWorker, double iInterval);
	virtual bool IsAwake(ZRef<ZWorker> iWorker);
	virtual bool IsAttached(ZRef<ZWorker> iWorker);

protected:
// Must be implemented by subclasses
	virtual void pQueueCallback() = 0;

// Called by subclasses
	void pAttach(ZRef<ZWorker> iWorker);
	void pCallback();

private:
	class Worker_Waker;
	friend class Worker_Waker;

	bool pTriggerCallback();

	void pWake(ZRef<ZWorker> iWorker, ZTime iSystemTime);

	ZMtxR fMtx;
	ZCnd fCnd;
	bool fCallbackTriggered;
	ZRef<Worker_Waker> fWorker_Waker;
	std::map<ZRef<ZWorker>, ZTime> fWorkersMap;
	ZSafeSet<ZRef<ZWorker> > fWorkersSet;
	};

} // namespace ZooLib

#endif // __ZWorkerRunner_EventLoop__
