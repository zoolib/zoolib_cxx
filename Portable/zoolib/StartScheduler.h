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

#ifndef __ZooLib_StartScheduler_h__
#define __ZooLib_StartScheduler_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_NonCopyable.h"
#include "zoolib/Starter.h"

#include "zoolib/ZThread.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark - StartScheduler

class StartScheduler
:	NonCopyable
	{
public:
	StartScheduler();

	typedef std::pair<ZP<Starter>,ZP<Callable_Void>> Job;

	bool Cancel(const Job& iJob);

	void NextStartAt(double iSystemTime, const Job& iJob);

	void NextStartIn(double iInterval, const Job& iJob);

private:
	void pNextStartAt(double iSystemTime, const Job& iJob);

	void pRun();
	static void spRun(StartScheduler*);

	ZMtx fMtx;
	ZCnd fCnd;

	bool fThreadRunning;

	typedef std::pair<double,Job> TimeJob;
	typedef std::pair<Job,double> JobTime;
	std::set<TimeJob> fTimeJobs;
	std::set<JobTime> fJobTimes;
	};

// =================================================================================================
#pragma mark - StartScheduler function interface

bool sCancel(const StartScheduler::Job& iJob);

void sNextStartAt(double iSystemTime, const StartScheduler::Job& iJob);

void sNextStartIn(double iInterval, const StartScheduler::Job& iJob);

// -----

bool sCancel(const ZP<Starter>& iStarter, const ZP<Callable_Void>& iCallable);

void sNextStartAt(double iSystemTime,
	const ZP<Starter>& iStarter, const ZP<Callable_Void>& iCallable);

void sNextStartIn(double iInterval,
	const ZP<Starter>& iStarter, const ZP<Callable_Void>& iCallable);

} // namespace ZooLib

#endif // __ZooLib_StartScheduler_h__
