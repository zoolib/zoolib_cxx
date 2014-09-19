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

#ifndef __ZooLib_CallScheduler_h__
#define __ZooLib_CallScheduler_h__ 1
#include "zconfig.h"

#include "zoolib/Caller.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThread.h"

#include <set>

namespace ZooLib {

// =================================================================================================
// MARK: - CallScheduler

class CallScheduler
:	NonCopyable
	{
public:
	CallScheduler();

	typedef std::pair<ZRef<Caller>,ZRef<Callable_Void> > Job;

// -----

	bool Cancel(const Job& iJob);

	void NextCallAt(ZTime iSystemTime, const Job& iJob);

	void NextCallIn(double iInterval, const Job& iJob);

	bool WillCall(const Job& iJob);

// -----

	bool Cancel(const ZRef<Caller>& iCaller, const ZRef<Callable_Void>& iCallable);

	void NextCallAt(ZTime iSystemTime,
		const ZRef<Caller>& iCaller, const ZRef<Callable_Void>& iCallable);

	void NextCallIn(double iInterval,
		const ZRef<Caller>& iCaller, const ZRef<Callable_Void>& iCallable);

	bool WillCall(const ZRef<Caller>& iCaller, const ZRef<Callable_Void>& iCallable);

// -----

private:
	void pNextCallAt(ZTime iSystemTime, const Job& iJob);

	void pRun();
	static void spRun(CallScheduler*);

	ZMtx fMtx;
	ZCnd fCnd;

	bool fThreadRunning;

	typedef std::pair<ZTime,Job> TimeJob;
	typedef std::pair<Job,ZTime> JobTime;
	std::set<TimeJob> fTimeJobs;
	std::set<JobTime> fJobTimes;
	};

// =================================================================================================
// MARK: - CallScheduler function interface

bool sCancel(const CallScheduler::Job& iJob);

void sNextCallAt(ZTime iSystemTime, const CallScheduler::Job& iJob);

void sNextCallIn(double iInterval, const CallScheduler::Job& iJob);

bool sWillCall(const CallScheduler::Job& iJob);

} // namespace ZooLib

#endif // __ZooLib_CallScheduler_h__
