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

#ifndef __ZWorkerRunner_Thread__
#define __ZWorkerRunner_Thread__ 1
#include "zconfig.h"

#include "zoolib/ZThread.h"
#include "zoolib/ZWorker.h"

#include "zoolib/ZCompat_MSVCStaticLib.h"
ZMACRO_MSVCStaticLib_Reference(WorkerRunner_Thread)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_Thread

class ZWorkerRunner_Thread : public ZWorkerRunner
	{
public:
	ZWorkerRunner_Thread(ZRef<ZWorker> iWorker);

// From ZWorkerRunner
	virtual void Wake(ZRef<ZWorker> iWorker);
	virtual void WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime);
	virtual void WakeIn(ZRef<ZWorker> iWorker, double iInterval);
	virtual bool IsAwake(ZRef<ZWorker> iWorker);
	virtual bool IsAttached(ZRef<ZWorker> iWorker);

// Our protocol
	void Start();

private:
	void pWakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime);

	void pRun();
	static void spRun(ZRef<ZWorkerRunner_Thread> iParam);

	ZMtx fMtx;
	ZCnd fCnd;
	ZRef<ZWorker> fWorker;
	ZTime fNextWake;
	};

} // namespace ZooLib

#endif // __ZWorkerRunner_Thread__
