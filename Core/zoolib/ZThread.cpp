/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
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
