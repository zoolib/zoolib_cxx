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

#ifndef __ZCallScheduler_h__
#define __ZCallScheduler_h__ 1
#include "zconfig.h"

#include "zoolib/ZCaller.h"
#include "zoolib/ZCompat_noncopyable.h"
#include "zoolib/ZThread.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallScheduler

class ZCallScheduler
:	ZooLib::NonCopyable
	{
public:
	static ZCallScheduler* sGet();

	void Cancel(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable_Void>& iCallable);

	void NextCallAt(ZTime iSystemTime,
		const ZRef<ZCaller>& iCaller, const ZRef<ZCallable_Void>& iCallable);

	void NextCallIn(double iInterval,
		const ZRef<ZCaller>& iCaller, const ZRef<ZCallable_Void>& iCallable);

	bool IsAwake(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable_Void>& iCallable);

private:
	ZCallScheduler();

	typedef std::pair<ZRef<ZCaller>,ZRef<ZCallable_Void> > Job;

	void pNextCallAt(ZTime iSystemTime, const Job& iJob);

	void pRun();
	static void spRun(ZCallScheduler*);

	ZMtx fMtx;
	ZCnd fCnd;

	bool fThreadRunning;

	typedef std::pair<ZTime,Job> TimeJob;
	typedef std::pair<Job,ZTime> JobTime;
	std::set<TimeJob> fTimeJobs;
	std::set<JobTime> fJobTimes;
	};

} // namespace ZooLib

#endif // __ZCallScheduler_h__
