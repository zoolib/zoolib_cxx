// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/StartScheduler.h"

#include "zoolib/Singleton.h"
#include "zoolib/Util_STL_set.h"

namespace ZooLib {

using std::set;

using namespace Util_STL;

// =================================================================================================
#pragma mark - StartScheduler

StartScheduler::StartScheduler()
:	fThreadRunning(false)
	{}

bool StartScheduler::Cancel(const Job& iJob)
	{
	ZAcqMtx acq(fMtx);

	set<JobTime>::iterator iterJT = fJobTimes.lower_bound(JobTime(iJob, 0.0));
	if (iterJT != fJobTimes.end() && iterJT->first == iJob)
		{
		sEraseMust(fTimeJobs, TimeJob(iterJT->second, iJob));
		fJobTimes.erase(iterJT);
		return true;
		}
	return false;
	}

void StartScheduler::NextStartAt(double iSystemTime, const Job& iJob)
	{ this->pNextStartAt(iSystemTime, iJob); }

void StartScheduler::NextStartIn(double iInterval, const Job& iJob)
	{ this->pNextStartAt(Time::sSystem() + iInterval, iJob); }

void StartScheduler::pNextStartAt(double iSystemTime, const Job& iJob)
	{
	ZAssert(iJob.first);

	ZAcqMtx acq(fMtx);

	set<JobTime>::iterator iterJT = fJobTimes.lower_bound(JobTime(iJob, 0.0));
	if (iterJT != fJobTimes.end() && iterJT->first == iJob)
		{
		if (iSystemTime < iterJT->second)
			{
			sEraseMust(fTimeJobs, TimeJob(iterJT->second, iJob));
			fJobTimes.erase(iterJT);

			sInsertMust(fTimeJobs, TimeJob(iSystemTime, iJob));
			sInsertMust(fJobTimes, JobTime(iJob, iSystemTime));
			fCnd.Broadcast();
			}
		}
	else
		{
		sInsertMust(fJobTimes, JobTime(iJob, iSystemTime));
		sInsertMust(fTimeJobs, TimeJob(iSystemTime, iJob));
		if (not fThreadRunning)
			{
			fThreadRunning = true;
			ZThread::sStart_T<StartScheduler*>(spRun, this);
			}
		fCnd.Broadcast();
		}
	}

void StartScheduler::pRun()
	{
	ZAcqMtx acq(fMtx);
	for (;;)
		{
		if (fTimeJobs.empty())
			{
			// Nothing pending, wait 100ms in case something else comes along.
			fCnd.WaitFor(fMtx, 100e-3);
			if (fTimeJobs.empty())
				{
				// Still nothing pending, exit thread.
				fThreadRunning = false;
				break;
				}
			}
		else
			{
			const set<TimeJob>::iterator& begin = fTimeJobs.begin();
			const double delta = begin->first - Time::sSystem();
			if (delta > 0)
				{
				fCnd.WaitFor(fMtx, delta);
				}
			else
				{
				ZP<Starter> theStarter = begin->second.first;
				ZP<Startable> theCallable = begin->second.second;

				sEraseMust(fJobTimes, JobTime(begin->second, begin->first));
				fTimeJobs.erase(begin);

				ZRelMtx rel(fMtx);

				try { theStarter->QStart(theCallable); }
				catch (...) {}
				}
			}
		}
	}

void StartScheduler::spRun(StartScheduler* iStartScheduler)
	{
	ZThread::sSetName("StartScheduler");

	iStartScheduler->pRun();
	}

// =================================================================================================
#pragma mark - StartScheduler function interface

bool sCancel(const StartScheduler::Job& iJob)
	{ return sSingleton<StartScheduler>().Cancel(iJob); }

void sNextStartAt(double iSystemTime, const StartScheduler::Job& iJob)
	{ sSingleton<StartScheduler>().NextStartAt(iSystemTime, iJob); }

void sNextStartIn(double iInterval, const StartScheduler::Job& iJob)
	{ sSingleton<StartScheduler>().NextStartIn(iInterval, iJob); }

// -----

bool sCancel(const ZP<Starter>& iStarter, const ZP<Startable>& iCallable)
	{ return sCancel(StartScheduler::Job(iStarter, iCallable)); }

void sNextStartAt(double iSystemTime,
	const ZP<Starter>& iStarter, const ZP<Startable>& iCallable)
	{ sNextStartAt(iSystemTime, StartScheduler::Job(iStarter, iCallable)); }

void sNextStartIn(double iInterval,
	const ZP<Starter>& iStarter, const ZP<Startable>& iCallable)
	{ sNextStartIn(iInterval, StartScheduler::Job(iStarter, iCallable)); }

} // namespace ZooLib
