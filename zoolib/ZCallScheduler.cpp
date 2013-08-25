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

#include "zoolib/ZCallScheduler.h"
#include "zoolib/ZUtil_STL_set.h"

namespace ZooLib {

using std::set;

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - ZCallScheduler

ZCallScheduler::ZCallScheduler()
:	fThreadRunning(false)
	{}

bool ZCallScheduler::Cancel(const Job& iJob)
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

void ZCallScheduler::NextCallAt(ZTime iSystemTime, const Job& iJob)
	{ this->pNextCallAt(iSystemTime, iJob); }

void ZCallScheduler::NextCallIn(double iInterval, const Job& iJob)
	{ this->pNextCallAt(ZTime::sSystem() + iInterval, iJob); }

bool ZCallScheduler::IsAwake(const Job& iJob)
	{
	ZAcqMtx acq(fMtx);

	set<JobTime>::iterator iterJT = fJobTimes.lower_bound(JobTime(iJob, 0.0));
	if (iterJT != fJobTimes.end() && iterJT->first == iJob)
		return ZTime::sSystem() >= iterJT->second;

	return false;
	}

bool ZCallScheduler::Cancel(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable_Void>& iCallable)
	{ return this->Cancel(Job(iCaller, iCallable)); }

void ZCallScheduler::NextCallAt(ZTime iSystemTime,
	const ZRef<ZCaller>& iCaller, const ZRef<ZCallable_Void>& iCallable)
	{ this->pNextCallAt(iSystemTime, Job(iCaller, iCallable)); }

void ZCallScheduler::NextCallIn(double iInterval,
	const ZRef<ZCaller>& iCaller, const ZRef<ZCallable_Void>& iCallable)
	{ this->pNextCallAt(ZTime::sSystem() + iInterval, Job(iCaller, iCallable)); }

bool ZCallScheduler::IsAwake(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable_Void>& iCallable)
	{ return this->IsAwake(Job(iCaller, iCallable)); }

void ZCallScheduler::pNextCallAt(ZTime iSystemTime, const Job& iJob)
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
			ZThread::sCreate_T<ZCallScheduler*>(spRun, this);
			}
		fCnd.Broadcast();
		}
	}

void ZCallScheduler::pRun()
	{
	ZGuardMtx guard(fMtx);
	for (;;)
		{
		if (fTimeJobs.empty())
			{
			// Nothing pending, wait 100ms in case something else comes along.
			fCnd.WaitFor(fMtx, 0.1);
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
				ZRef<ZCaller> theCaller = begin->second.first;
				ZRef<ZCallable_Void> theCallable = begin->second.second;

				sEraseMust(fJobTimes, JobTime(begin->second, begin->first));
				fTimeJobs.erase(begin);

				guard.Release();

				try { theCaller->Enqueue(theCallable); }
				catch (...) {}

				guard.Acquire();
				}
			}
		}
	}

void ZCallScheduler::spRun(ZCallScheduler* iCallScheduler)
	{
	ZThread::sSetName("ZCallScheduler");

	iCallScheduler->pRun();
	}

} // namespace ZooLib
