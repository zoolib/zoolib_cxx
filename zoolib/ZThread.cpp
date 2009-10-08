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

#include "zoolib/ZDebug.h"
#include "zoolib/ZThread.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZThread

namespace ZThread {

ZAssertCompile(sizeof(void*) == sizeof(ProcVoid_t));

static ZAtomic_t sThreadCount;

void sStarted()
	{ ZAtomic_Inc(&sThreadCount); }

void sFinished()
	{ ZAtomic_Dec(&sThreadCount); }

static bool sDontTearDown;

void sDontTearDownTillAllThreadsExit()
	{ sDontTearDown = true; }

static ZAtomic_t sInitCount;

InitHelper::InitHelper()
	{ ZAtomic_Inc(&sInitCount); }

InitHelper::~InitHelper()
	{
	if (ZAtomic_DecAndTest(&sInitCount) && sDontTearDown)
		{
		for (;;)
			{
			int count = ZAtomic_Get(&sThreadCount);
			// This sleep serves two purposes. First it means we're polling at
			// intervals for the value of sThreadCount, rather than busy-waiting.
			// Second, at least .1s will elapse between the thread count hitting
			// zero and this code exiting, so the last thread will have
			// had a chance to fully terminate.
			sSleep(.1);
			if (0 == count)
				{
				fputs("ZThread, final exit\n", stderr);
				break;
				}
			}
		}
	}

#if ZCONFIG_API_Enabled(ThreadImp_Win)
	static ProcResult_t __stdcall sEntryVoid(ProcVoid_t iProc)
#else
	static ProcResult_t sEntryVoid(ProcVoid_t iProc)
#endif
	{
	sStarted();
	try
		{
		iProc();
		}
	catch (...)
		{}
	sFinished();
	return 0;
	}

void sCreateVoid(ProcVoid_t iProcVoid)
	{
	union Converter_t
		{
		void* fAsPointer;
		ProcVoid_t fAsProc;
		} theConverter;

	theConverter.fAsProc = iProcVoid;

	sCreateRaw(0, (ProcRaw_t)sEntryVoid, theConverter.fAsPointer);
	}

} // namespace ZThread
	
NAMESPACE_ZOOLIB_END
