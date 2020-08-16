// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Server/Worker.h"

#include "zoolib/StartScheduler.h"

namespace ZooLib {

/**
\defgroup Worker

\brief Worker provides a disciplined lifecycle for long-lived repetitive jobs.

*/

// =================================================================================================
#pragma mark - Worker

/**
\class Worker
*/

static const double kDistantFuture = 1000 * Time::kYear;

typedef StartScheduler::Job Job;

Worker::Worker(
	const ZP<Callable_Attached>& iCallable_Attached,
	const ZP<Callable_Work>& iCallable_Work,
	const ZP<Callable_Detached>& iCallable_Detached)
:	fWorking(0)
,	fCallable_Attached(iCallable_Attached)
,	fCallable_Work(iCallable_Work)
,	fCallable_Detached(iCallable_Detached)
	{
	ZAssert(fCallable_Work);
	}

Worker::Worker(
	const ZP<Callable_Attached>& iCallable_Attached,
	const ZP<Callable_Work>& iCallable_Work)
:	fWorking(0)
,	fCallable_Attached(iCallable_Attached)
,	fCallable_Work(iCallable_Work)
	{
	ZAssert(fCallable_Work);
	}

Worker::Worker(
	const ZP<Callable_Work>& iCallable_Work,
	const ZP<Callable_Detached>& iCallable_Detached)
:	fWorking(0)
,	fCallable_Work(iCallable_Work)
,	fCallable_Detached(iCallable_Detached)
	{
	ZAssert(fCallable_Work);
	}

Worker::Worker(const ZP<Callable_Work>& iCallable_Work)
:	fWorking(0)
,	fCallable_Work(iCallable_Work)
	{
	ZAssert(fCallable_Work);
	}

Worker::Worker()
:	fWorking(0)
	{}

bool Worker::QCall()
	{
	ZAcqMtx acq(fMtx);

	for (;;)
		{
		fWorking = ZThread::sID();
		fNextWake = kDistantFuture;
		ZRelMtx rel(fMtx);

		ZQ<bool> result;

		try { result = fCallable_Work->QCall(this); }
		catch (...) {}

		ZAcqMtx acq(fMtx);
		fWorking = 0;

		if (result && *result)
			{
			if (fNextWake < kDistantFuture)
				{
				if (fNextWake <= Time::sSystem())
					continue;
				sNextStartAt(fNextWake, Job(fStarter, this));
				}
			return true;
			}

		fStarter.Clear();
		fCnd.Broadcast();

		ZRelMtx rel2(fMtx);

		try { sCall(fCallable_Detached, sZP(this)); }
		catch (...) {}

		return false;
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

bool Worker::Attach(ZP<Starter> iStarter)
	{
	ZAcqMtx acq(fMtx);
	if (not fStarter)
		{
		fStarter = iStarter;

		try
			{
			ZRelMtx rel(fMtx);
			sCall(fCallable_Attached, this);
			return true;
			}
		catch (...) {}

		fStarter.Clear();
		fCnd.Broadcast();

		ZRelMtx rel(fMtx);
		try { sCall(fCallable_Detached, this); }
		catch (...) {}
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
