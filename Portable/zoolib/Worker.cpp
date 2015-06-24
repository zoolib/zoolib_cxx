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

#include "zoolib/StartScheduler.h"
#include "zoolib/Worker.h"

namespace ZooLib {

/**
\defgroup Worker

\brief Worker provides a disciplined lifecycle for long-lived repetitive jobs.

*/

// =================================================================================================
#pragma mark -
#pragma mark Worker

/**
\class Worker
*/

static const double kDistantFuture = 1000 * Time::kYear;

typedef StartScheduler::Job Job;

Worker::Worker(
	const ZRef<Callable_Attached>& iCallable_Attached,
	const ZRef<Callable_Work>& iCallable_Work,
	const ZRef<Callable_Detached>& iCallable_Detached)
:	fWorking(0)
,	fCallable_Attached(iCallable_Attached)
,	fCallable_Work(iCallable_Work)
,	fCallable_Detached(iCallable_Detached)
	{
	ZAssert(fCallable_Work);
	}

Worker::Worker(
	const ZRef<Callable_Attached>& iCallable_Attached,
	const ZRef<Callable_Work>& iCallable_Work)
:	fWorking(0)
,	fCallable_Attached(iCallable_Attached)
,	fCallable_Work(iCallable_Work)
	{
	ZAssert(fCallable_Work);
	}

Worker::Worker(
	const ZRef<Callable_Work>& iCallable_Work,
	const ZRef<Callable_Detached>& iCallable_Detached)
:	fWorking(0)
,	fCallable_Work(iCallable_Work)
,	fCallable_Detached(iCallable_Detached)
	{
	ZAssert(fCallable_Work);
	}

Worker::Worker(const ZRef<Callable_Work>& iCallable_Work)
:	fWorking(0)
,	fCallable_Work(iCallable_Work)
	{
	ZAssert(fCallable_Work);
	}

Worker::Worker()
:	fWorking(0)
	{}

ZQ<void> Worker::QCall()
	{
	ZGuardMtx guard(fMtx);

	for (;;)
		{
		fWorking = ZThread::sID();
		fNextWake = kDistantFuture;
		guard.Release();

		ZQ<bool> result;

		try { result = fCallable_Work->QCall(this); }
		catch (...) {}

		guard.Acquire();
		fWorking = 0;

		if (result && *result)
			{
			if (fNextWake < kDistantFuture)
				{
				if (fNextWake <= Time::sSystem())
					continue;
				sNextStartAt(fNextWake, Job(fStarter, this));
				}
			return notnull;
			}

		fStarter.Clear();
		fCnd.Broadcast();

		guard.Release();

		try { sCall(fCallable_Detached, this); }
		catch (...) {}

		return null;
		}
	}

void Worker::Wake()
	{ this->pWakeAt(Time::sSystem()); }

void Worker::WakeIn(double iInterval)
	{ this->pWakeAt(Time::sSystem() + iInterval); }

void Worker::WakeAt(double iSystemTime)
	{ this->pWakeAt(iSystemTime); }

bool Worker::IsWorking()
	{ return ZThread::sID() == fWorking; }

bool Worker::Attach(ZRef<Starter> iStarter)
	{
	ZGuardMtx guard(fMtx);
	if (not fStarter)
		{
		fStarter = iStarter;

		guard.Release();
		try
			{
			sCall(fCallable_Attached, this);
			return true;
			}
		catch (...) {}
		guard.Acquire();

		fStarter.Clear();
		fCnd.Broadcast();

		guard.Release();
		try { sCall(fCallable_Detached, this); }
		catch (...) {}
		guard.Acquire();
		}
	return false;
	}

bool Worker::IsAttached()
	{
	ZAcqMtx acq(fMtx);
	return fStarter;
	}

void Worker::pWakeAt(double iSystemTime)
	{
	ZAcqMtx acq(fMtx);
	if (fStarter)
		{
		if (fWorking)
			{
			if (fNextWake > iSystemTime)
				fNextWake = iSystemTime;
			}
		else
			{
			sNextStartAt(iSystemTime, Job(fStarter, this));
			}
		}
	}

} // namespace ZooLib
