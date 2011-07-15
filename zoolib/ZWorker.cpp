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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZCallScheduler.h"
#include "zoolib/ZGetSet.h"
#include "zoolib/ZWorker.h"

namespace ZooLib {

/**
\defgroup Worker

\brief ZWorker provides a disciplined lifecycle for long-lived repetitive jobs.

*/

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker

/**
\class ZWorker
\ingroup Worker
\sa Worker
*/

static const ZTime kDistantFuture = 1000 * ZTime::kYear;

ZWorker::ZWorker(ZRef<Callable_Attached> iCallable_Attached,
	ZRef<Callable_Work> iCallable_Work,
	ZRef<Callable_Detached> iCallable_Detached)
:	fWorking(0)
,	fCallable_Attached(iCallable_Attached)
,	fCallable_Work(iCallable_Work)
,	fCallable_Detached(iCallable_Detached)
	{}

ZWorker::ZWorker(ZRef<Callable_Attached> iCallable_Attached, ZRef<Callable_Work> iCallable_Work)
:	fWorking(0)
,	fCallable_Attached(iCallable_Attached)
,	fCallable_Work(iCallable_Work)
	{}

ZWorker::ZWorker(ZRef<Callable_Work> iCallable_Work, ZRef<Callable_Detached> iCallable_Detached)
:	fWorking(0)
,	fCallable_Work(iCallable_Work)
,	fCallable_Detached(iCallable_Detached)
	{}

ZWorker::ZWorker(ZRef<Callable_Work> iCallable_Work)
:	fWorking(0)
,	fCallable_Work(iCallable_Work)
	{}

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

		ZQ<bool> result;

		if (ZRef<Callable_Work> theCallable = fCallable_Work)
			{
			guard.Release();

			try { result = theCallable->QCall(this); }
			catch (...) {}

			guard.Acquire();
			}

		fWorking = 0;

		if (result && result.Get())
			{
			if (fNextWake < kDistantFuture)
				{
				if (ZTime::sSystem() >= fNextWake)
					{
					// If we're awake, just go around again.
					puts("sdfsdfs\n");
					continue;
					}
				ZCallScheduler::sGet()->NextCallAt(fNextWake, fCaller, this);
				}
			return true;
			}
		else
			{
			if (ZRef<Callable_Detached> theCallable = fCallable_Detached)
				{
				guard.Release();

				try { theCallable->Call(this); }
				catch (...) {}

				guard.Acquire();
				}

			fCaller.Clear();
			}
		}
	return null;
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
			{
			return true;
			}
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

		guard.Release();

		if (ZRef<Callable_Detached> theCallable = fCallable_Detached)
			{
			guard.Release();
			try { theCallable->Call(this); }
			catch (...) {}
			guard.Acquire();
			}

		fCaller.Clear();
		}
	return false;
	}

bool ZWorker::IsAttached()
	{
	ZAcqMtx acq(fMtx);
	return fCaller;
	}

ZRef<ZWorker::Callable_Attached> ZWorker::GetSetCallable_Attached(ZRef<Callable_Attached> iCallable)
	{
	ZAcqMtx acq(fMtx);
	return sGetSet(fCallable_Attached, iCallable);
	}

ZRef<ZWorker::Callable_Work> ZWorker::GetSetCallable_Work(ZRef<Callable_Work> iCallable)
	{
	ZAcqMtx acq(fMtx);
	return sGetSet(fCallable_Work, iCallable);
	}

ZRef<ZWorker::Callable_Detached> ZWorker::GetSetCallable_Detached(ZRef<Callable_Detached> iCallable)
	{
	ZAcqMtx acq(fMtx);
	return sGetSet(fCallable_Detached, iCallable);
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
