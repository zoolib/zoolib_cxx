/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#include "zoolib/Singleton.h"
#include "zoolib/StartScheduler.h"

#include "zoolib/ZUtil_STL_set.h"

namespace ZooLib {

using std::set;

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - StartScheduler

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

void StartScheduler::NextStartAt(ZTime iSystemTime, const Job& iJob)
	{ this->pNextStartAt(iSystemTime, iJob); }

void StartScheduler::NextStartIn(double iInterval, const Job& iJob)
	{ this->pNextStartAt(ZTime::sSystem() + iInterval, iJob); }

void StartScheduler::pNextStartAt(ZTime iSystemTime, const Job& iJob)
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
			ZThread::sCreate_T<StartScheduler*>(spRun, this);
			}
		fCnd.Broadcast();
		}
	}

void StartScheduler::pRun()
	{
	ZGuardMtx guard(fMtx);
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
			const double delta = begin->first - ZTime::sSystem();
			if (delta > 0)
				{
				fCnd.WaitFor(fMtx, delta);
				}
			else
				{
				ZRef<Starter> theStarter = begin->second.first;
				ZRef<Callable_Void> theCallable = begin->second.second;

				sEraseMust(fJobTimes, JobTime(begin->second, begin->first));
				fTimeJobs.erase(begin);

				guard.Release();

				try { theStarter->QStart(theCallable); }
				catch (...) {}

				guard.Acquire();
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
// MARK: - StartScheduler function interface

bool sCancel(const StartScheduler::Job& iJob)
	{ return sSingleton<StartScheduler>().Cancel(iJob); }

void sNextStartAt(ZTime iSystemTime, const StartScheduler::Job& iJob)
	{ sSingleton<StartScheduler>().NextStartAt(iSystemTime, iJob); }

void sNextStartIn(double iInterval, const StartScheduler::Job& iJob)
	{ sSingleton<StartScheduler>().NextStartIn(iInterval, iJob); }

// -----

bool sCancel(const ZRef<Starter>& iStarter, const ZRef<Callable_Void>& iCallable)
	{ return sCancel(StartScheduler::Job(iStarter, iCallable)); }

void sNextStartAt(ZTime iSystemTime,
	const ZRef<Starter>& iStarter, const ZRef<Callable_Void>& iCallable)
	{ sNextStartAt(iSystemTime, StartScheduler::Job(iStarter, iCallable)); }

void sNextStartIn(double iInterval,
	const ZRef<Starter>& iStarter, const ZRef<Callable_Void>& iCallable)
	{ sNextStartIn(iInterval, StartScheduler::Job(iStarter, iCallable)); }

} // namespace ZooLib
