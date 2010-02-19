/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZLog.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZWorker.h"


NAMESPACE_ZOOLIB_BEGIN

/**
\defgroup Worker

ZWorker provides a disciplined lifecycle for long-lived repetitive tasks.

*/

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker

/**
\class ZWorker
\ingroup Worker
\sa Worker
*/

void ZWorker::RunnerAttached()
	{}

void ZWorker::RunnerDetached()
	{}

void ZWorker::Wake()
	{
	if (ZRef<ZWorkerRunner> theRunner = fRunner)
		theRunner->Wake(this);
	}

void ZWorker::WakeIn(double iInterval)
	{
	if (ZRef<ZWorkerRunner> theRunner = fRunner)
		theRunner->WakeIn(this, iInterval);
	}

void ZWorker::WakeAt(ZTime iSystemTime)
	{
	if (ZRef<ZWorkerRunner> theRunner = fRunner)
		theRunner->WakeAt(this, iSystemTime);
	}

void ZWorker::pRunnerAttached()
	{ this->RunnerAttached(); }

void ZWorker::pRunnerDetached()
	{ this->RunnerDetached(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner

void ZWorkerRunner::pAttachWorker(ZRef<ZWorker> iWorker)
	{
	ZAssert(iWorker);
	ZAssert(! ZRef<ZWorkerRunner>(iWorker->fRunner));

	iWorker->fRunner = ZRef<ZWorkerRunner>(this);

	iWorker->pRunnerAttached();
	}

void ZWorkerRunner::pDetachWorker(ZRef<ZWorker> iWorker)
	{
	ZAssert(iWorker);
	ZAssert(this == ZRef<ZWorkerRunner>(iWorker->fRunner));

	iWorker->fRunner.Clear();

	iWorker->pRunnerDetached();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_Threaded

class ZWorkerRunner_Threaded : public ZWorkerRunner
	{
public:
	ZWorkerRunner_Threaded(ZRef<ZWorker> iWorker);

// From ZWorkerRunner
	virtual void Wake(ZRef<ZWorker> iWorker);
	virtual void WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime);
	virtual void WakeIn(ZRef<ZWorker> iWorker, double iInterval);

// Our protocol
	void Start();

private:
	void pRun();
	static void spRun(ZRef<ZWorkerRunner_Threaded> iParam);

	ZMtx fMtx;
	ZCnd fCnd;
	ZRef<ZWorker> fWorker;
	ZTime fNextWake;
	};

ZWorkerRunner_Threaded::ZWorkerRunner_Threaded(ZRef<ZWorker> iWorker)
:	fWorker(iWorker)
,	fNextWake(ZTime::sSystem())
	{}

void ZWorkerRunner_Threaded::Wake(ZRef<ZWorker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	ZAssert(iWorker == fWorker);
	fNextWake = 0;
	fCnd.Broadcast();
	}

void ZWorkerRunner_Threaded::WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime)
	{
	ZAcqMtx acq(fMtx);
	ZAssert(iWorker == fWorker);
	if (fNextWake > iSystemTime)
		{
		fNextWake = iSystemTime;
		fCnd.Broadcast();
		}
	}

void ZWorkerRunner_Threaded::WakeIn(ZRef<ZWorker> iWorker, double iInterval)
	{
	ZAcqMtx acq(fMtx);
	ZAssert(iWorker == fWorker);
	ZTime newWake = ZTime::sSystem() + iInterval;
	if (fNextWake > newWake)
		{
		fNextWake = newWake;
		fCnd.Broadcast();
		}
	}
void ZWorkerRunner_Threaded::Start()
	{
	ZWorkerRunner::pAttachWorker(fWorker);
	ZThread::sCreate_T<ZRef<ZWorkerRunner_Threaded> >(spRun, this);
	}

void ZWorkerRunner_Threaded::pRun()
	{
	ZLOGFUNCTION(eDebug);

	ZAcqMtx acq(fMtx);
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
		try
			{
			if (!fWorker->Work())
				break;
			}
		catch (...)
			{
			break;
			}
		}

	ZWorkerRunner::pDetachWorker(fWorker);
	fWorker.Clear();
	fCnd.Broadcast();
	}

void ZWorkerRunner_Threaded::spRun(ZRef<ZWorkerRunner_Threaded> iParam)
	{ iParam->pRun(); }

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

void sStartWorkerRunner(ZRef<ZWorker> iWorker)
	{
	ZRef<ZWorkerRunner_Threaded> theRunner = new ZWorkerRunner_Threaded(iWorker);
	theRunner->Start();
	}

NAMESPACE_ZOOLIB_END
