// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

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

	typedef std::pair<ZP<Starter>,ZP<Startable>> Job;

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

bool sCancel(const ZP<Starter>& iStarter, const ZP<Startable>& iCallable);

void sNextStartAt(double iSystemTime,
	const ZP<Starter>& iStarter, const ZP<Startable>& iCallable);

void sNextStartIn(double iInterval,
	const ZP<Starter>& iStarter, const ZP<Startable>& iCallable);

} // namespace ZooLib

#endif // __ZooLib_StartScheduler_h__
