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

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZWorkerRunner_Thread.h"

#include <typeinfo>

ZMACRO_MSVCStaticLib_cpp(WorkerRunner_Thread)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

namespace { // anonymous

class Maker
:	public ZFunctionChain_T<ZRef<ZWorkerRunner>, ZRef<ZWorker> >
	{
public:
	Maker() : Base_t(false) {}

	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		ZRef<ZWorkerRunner_Thread> theWR = new ZWorkerRunner_Thread(iParam);
		theWR->Start();
		oResult = theWR;
		return true;
		}
	} sMaker0;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_Thread

/**
\class ZWorkerRunner_Thread
\ingroup Worker
\sa Worker
*/

ZWorkerRunner_Thread::ZWorkerRunner_Thread(ZRef<ZWorker> iWorker)
:	fWorker(iWorker)
,	fNextWake(ZTime::sSystem() + ZTime::kYear)
	{}

void ZWorkerRunner_Thread::Wake(ZRef<ZWorker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	if (fWorker)
		{
		fNextWake = 0;
		fCnd.Broadcast();
		}
	}

void ZWorkerRunner_Thread::WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime)
	{
	ZAcqMtx acq(fMtx);
	if (fWorker)
		{
		if (fNextWake > iSystemTime)
			{
			fNextWake = iSystemTime;
			fCnd.Broadcast();
			}
		}
	}

void ZWorkerRunner_Thread::WakeIn(ZRef<ZWorker> iWorker, double iInterval)
	{
	ZAcqMtx acq(fMtx);
	if (fWorker)
		{
		ZTime newWake = ZTime::sSystem() + iInterval;
		if (fNextWake > newWake)
			{
			fNextWake = newWake;
			fCnd.Broadcast();
			}
		}
	}

bool ZWorkerRunner_Thread::IsAwake(ZRef<ZWorker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	return fWorker && fNextWake <= ZTime::sSystem();
	}

bool ZWorkerRunner_Thread::IsAttached(ZRef<ZWorker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	return fWorker == iWorker;
	}

void ZWorkerRunner_Thread::Start()
	{
	ZRef<ZWorkerRunner_Thread> self = this;

	if (ZWorkerRunner::pAttachWorker(fWorker))
		{
		try
			{
			ZThread::sCreate_T<ZRef<ZWorkerRunner_Thread> >(spRun, this);
			}
		catch (...)
			{
			ZRef<ZWorker> theWorker = fWorker;
			fWorker.Clear();
			ZWorkerRunner::pDetachWorker(theWorker);
			throw;
			}
		}
	}

void ZWorkerRunner_Thread::pRun()
	{
	ZLOGFUNCTION(eDebug + 1);

	if (ZLOGF(s, eDebug + 1))
		s << typeid(*fWorker.Get()).name();

	ZGuardRMtx guard(fMtx);
	for (;;)
		{
		for (;;)
			{
			const ZTime now = ZTime::sSystem();
			if (fNextWake <= now)
				{
				fNextWake = now + 3600;
				break;
				}

			fCnd.WaitFor(fMtx, fNextWake - now);
			}

		ZRelMtx rel(fMtx);
		if (!this->pInvokeWork(fWorker))
			break;
		}

	ZRef<ZWorker> theWorker = fWorker;
	fWorker.Clear();
	guard.Release();

	ZWorkerRunner::pDetachWorker(theWorker);
	}

void ZWorkerRunner_Thread::spRun(ZRef<ZWorkerRunner_Thread> iParam)
	{ iParam->pRun(); }

} // namespace ZooLib
