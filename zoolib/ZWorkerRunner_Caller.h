/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZWorkerRunner_Caller__
#define __ZWorkerRunner_Caller__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCaller.h"
#include "zoolib/ZSafeSet.h"
#include "zoolib/ZWorkerRunner.h"

#include <map>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_Caller

class ZWorkerRunner_Caller
:	public ZWorkerRunner
	{
public:
	ZWorkerRunner_Caller(ZRef<ZCaller> iCaller);
	virtual ~ZWorkerRunner_Caller();

// From ZCounted via ZWorkerRunner
	virtual void Initialize();
	virtual void Finalize();

// From ZWorkerRunner
	virtual void Wake(ZRef<ZWorker> iWorker);
	virtual void WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime);
	virtual void WakeIn(ZRef<ZWorker> iWorker, double iInterval);
	virtual bool IsAwake(ZRef<ZWorker> iWorker);
	virtual bool IsAttached(ZRef<ZWorker> iWorker);

// Our protocol
	virtual void Attach(ZRef<ZWorker> iWorker);

	ZRef<ZCaller> GetCaller();

private:
	class Worker_Waker;
	friend class Worker_Waker;

	void pCallback();
	bool pTriggerCallback();

	void pWake(ZRef<ZWorker> iWorker, ZTime iSystemTime);

	ZMtxR fMtx;
	ZCnd fCnd;
	ZRef<ZCaller> fCaller;
	bool fCallbackTriggered;
	ZRef<Worker_Waker> fWorker_Waker;
	ZRef<ZCallable_Void> fCallable_Callback;
	std::map<ZRef<ZWorker>, ZTime> fWorkersMap;
	ZSafeSet<ZRef<ZWorker> > fWorkersSet;
	};

} // namespace ZooLib

#endif // __ZWorkerRunner_Caller__
