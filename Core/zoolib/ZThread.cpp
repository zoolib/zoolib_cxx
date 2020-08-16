// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZThread.h"

#include "zoolib/ZDebug.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ZThread

namespace ZThread {

static ZAtomic_t spThreadCount;

void sStarted()
	{ sAtomic_Inc(&spThreadCount); }

void sFinished()
	{ sAtomic_Dec(&spThreadCount); }

static bool spDontTearDown;

void sDontTearDownTillAllThreadsExit()
	{ spDontTearDown = true; }

void sWaitTillAllThreadsExit()
	{
	for (;;)
		{
		int count = sAtomic_Get(&spThreadCount);
		// This sleep serves two purposes. First it means we're polling at
		// intervals for the value of sThreadCount, rather than busy-waiting.
		// Second, at least .1s will elapse between the thread count hitting
		// zero and this code exiting, so the last thread will have
		// had a chance to fully terminate.
		sSleep(.1);
		if (0 == count)
			break;
		}
	}

static int spInitCount;

InitHelper::InitHelper()
	{ ++spInitCount; }

InitHelper::~InitHelper()
	{
	if (0 == --spInitCount && spDontTearDown)
		sWaitTillAllThreadsExit();
	}

} // namespace ZThread

// =================================================================================================
#pragma mark - ZTSS

namespace ZTSS {

Key sKey(std::atomic<Key>& ioStorage)
	{
	if (not ioStorage)
		sAtomic_CAS(&ioStorage, Key(0), ZTSS::sCreate());
	return ioStorage;
	}

} // namespace ZTSS

} // namespace ZooLib
