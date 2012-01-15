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

#include "zoolib/ZCallScheduler.h"
#include "zoolib/ZWorker.h"

namespace ZooLib {

/**
\defgroup Worker

\brief ZWorker provides a disciplined lifecycle for long-lived repetitive jobs.

*/

// =================================================================================================
// MARK: - ZWorker

/**
\class ZWorker
\ingroup Worker
\sa Worker
*/

static const ZTime kDistantFuture = 1000 * ZTime::kYear;

ZWorker::ZWorker
	(const ZRef<Callable_Attached>& iCallable_Attached,
	const ZRef<Callable_Work>& iCallable_Work,
	const ZRef<Callable_Detached>& iCallable_Detached)
:	fWorking(0)
,	fCallable_Attached(iCallable_Attached)
,	fCallable_Work(iCallable_Work)
,	fCallable_Detached(iCallable_Detached)
	{
	ZAssert(fCallable_Work);
	}

ZWorker::ZWorker
	(const ZRef<Callable_Attached>& iCallable_Attached,
	const ZRef<Callable_Work>& iCallable_Work)
:	fWorking(0)
,	fCallable_Attached(iCallable_Attached)
,	fCallable_Work(iCallable_Work)
	{
	ZAssert(fCallable_Work);
	}

ZWorker::ZWorker
	(const ZRef<Callable_Work>& iCallable_Work,
	const ZRef<Callable_Detached>& iCallable_Detached)
:	fWorking(0)
,	fCallable_Work(iCallable_Work)
,	fCallable_Detached(iCallable_Detached)
	{
	ZAssert(fCallable_Work);
	}

ZWorker::ZWorker(const ZRef<Callable_Work>& iCallable_Work)
:	fWorking(0)
,	fCallable_Work(iCallable_Work)
	{
	ZAssert(fCallable_Work);
	}

ZWorker::ZWorker()
:	fWorking(0)
	{}

ZQ<void> ZWorker::QCall()
	{
	ZGuardRMtx guard(fMtx);

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

		if (result && result.Get())
			{
			if (fNextWake < kDistantFuture)
				{
				if (ZTime::sSystem() >= fNextWake)
					continue;
				ZCallScheduler::sGet()->NextCallAt(fNextWake, fCaller, this);
				}
			return notnull;
			}

		fCaller.Clear();

		if (ZRef<Callable_Detached> theCallable = fCallable_Detached)
			{
			guard.Release();

			try { theCallable->Call(this); }
			catch (...) {}

			guard.Acquire();
			}

		return null;
		}
	}

void ZWorker::Wake()
	{ this->pWakeAt(ZTime::sSystem()); }

void ZWorker::WakeIn(double iInterval)
	{ this->pWakeAt(ZTime::sSystem() + iInterval); }

void ZWorker::WakeAt(ZTime iSystemTime)
	{ this->pWakeAt(iSystemTime); }

bool ZWorker::IsWorking()
	{
	ZAcqMtx acq(fMtx);
	return ZThread::sID() == fWorking;
	}

bool ZWorker::IsAwake()
	{
	ZAcqMtx acq(fMtx);
	if (fCaller)
		{
		if (fWorking)
			return fNextWake <= ZTime::sSystem();
		else
			return ZCallScheduler::sGet()->IsAwake(fCaller, this);
		}
	return false;
	}

bool ZWorker::Attach(ZRef<ZCaller> iCaller)
	{
	ZGuardRMtx guard(fMtx);
	if (!fCaller)
		{
		fCaller = iCaller;

		if (ZRef<Callable_Attached,false> theCallable = fCallable_Attached)
			{ return true; }
		else
			{
			guard.Release();
			try
				{
				theCallable->Call(this);
				return true;
				}
			catch (...) {}
			guard.Acquire();
			}

		fCaller.Clear();

		if (ZRef<Callable_Detached> theCallable = fCallable_Detached)
			{
			guard.Release();
			try { theCallable->Call(this); }
			catch (...) {}
			guard.Acquire();
			}
		}
	return false;
	}

bool ZWorker::IsAttached()
	{
	ZAcqMtx acq(fMtx);
	return fCaller;
	}

void ZWorker::pWakeAt(ZTime iSystemTime)
	{
	ZAcqMtx acq(fMtx);
	if (fCaller)
		{
		if (fWorking)
			{
			if (fNextWake > iSystemTime)
				fNextWake = iSystemTime;
			}
		else
			{
			ZCallScheduler::sGet()->NextCallAt(iSystemTime, fCaller, this);
			}
		}
	}

} // namespace ZooLib
