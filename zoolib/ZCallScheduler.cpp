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

// =================================================================================================
#pragma mark -
#pragma mark * ZCallScheduler

static ZCallScheduler* spScheduler;

ZCallScheduler* ZCallScheduler::sGet()
	{
	if (!spScheduler)
		{
		ZCallScheduler* theScheduler = new ZCallScheduler;
		if (ZAtomic_CompareAndSwapPtr(&spScheduler, nullptr, theScheduler))
			ZThread::sCreate_T<ZCallScheduler*>(spRun, theScheduler);
		else
			delete theScheduler;
		}
	return spScheduler;
	}

void ZCallScheduler::Cancel(ZRef<ZCaller> iCaller, ZRef<ZCallable_Void> iCallable)
	{
	using namespace ZUtil_STL;

	ZAcqMtxR acq(fMtxR);

	const Job theJob(iCaller, iCallable);

	set<JobTime>::iterator iterJT = fJobTimes.lower_bound(make_pair(theJob, 0.0));
	if (iterJT != fJobTimes.end() && iterJT->first == theJob)
		{
		sEraseMustContain(1, fTimeJobs, make_pair(iterJT->second, theJob));
		fJobTimes.erase(iterJT);
		}
	}

void ZCallScheduler::NextCallAt
	(ZTime iSystemTime, ZRef<ZCaller> iCaller, ZRef<ZCallable_Void> iCallable)
	{ this->pNextCallAt(iSystemTime, Job(iCaller, iCallable)); }

void ZCallScheduler::NextCallIn
	(double iInterval, ZRef<ZCaller> iCaller, ZRef<ZCallable_Void> iCallable)
	{ this->pNextCallAt(ZTime::sSystem() + iInterval, Job(iCaller, iCallable)); }

bool ZCallScheduler::IsAwake(ZRef<ZCaller> iCaller, ZRef<ZCallable_Void> iCallable)
	{
	ZAcqMtxR acq(fMtxR);

	const Job theJob(iCaller, iCallable);

	set<JobTime>::iterator iterJT = fJobTimes.lower_bound(make_pair(theJob, 0.0));
	if (iterJT != fJobTimes.end() && iterJT->first == theJob)
		return ZTime::sSystem() <= iterJT->second;

	return false;
	}

void ZCallScheduler::pNextCallAt(ZTime iSystemTime, const Job& iJob)
	{
	using namespace ZUtil_STL;

	ZAcqMtxR acq(fMtxR);

	set<JobTime>::iterator iterJT = fJobTimes.lower_bound(make_pair(iJob, 0.0));
	if (iterJT != fJobTimes.end() && iterJT->first == iJob)
		{
		if (iSystemTime < iterJT->second)
			{
			sEraseMustContain(1, fTimeJobs, make_pair(iterJT->second, iJob));
			fJobTimes.erase(iterJT);

			sInsertMustNotContain(1, fTimeJobs, make_pair(iSystemTime, iJob));
			sInsertMustNotContain(1, fJobTimes, make_pair(iJob, iSystemTime));
			fCnd.Broadcast();
			}
		}
	else
		{
		sInsertMustNotContain(1, fJobTimes, make_pair(iJob, iSystemTime));
		sInsertMustNotContain(1, fTimeJobs, make_pair(iSystemTime, iJob));
		fCnd.Broadcast();
		}
	}

void ZCallScheduler::pRun()
	{
	using namespace ZUtil_STL;

	ZGuardRMtxR guard(fMtxR);
	for (;;)
		{
		if (fTimeJobs.empty())
			{
			fCnd.Wait(fMtxR);
			}
		else
			{
			set<TimeJob>::iterator front = fTimeJobs.begin();
			const double delta = front->first - ZTime::sSystem();
			if (delta > 0)
				{
				fCnd.WaitFor(fMtxR, delta);
				}
			else
				{
				ZRef<ZCaller> theCaller = front->second.first;
				ZRef<ZCallable_Void> theCallable = front->second.second;

				ZUtil_STL::sEraseMustContain(1, fJobTimes, make_pair(front->second, front->first));
				fTimeJobs.erase(front);

				guard.Release();

				try { sCall(theCaller, theCallable); }
				catch (...) {}

				guard.Acquire();
				}
			}
		}
	}

void ZCallScheduler::spRun(ZCallScheduler* iCallScheduler)
	{ iCallScheduler->pRun(); }

} // namespace ZooLib
