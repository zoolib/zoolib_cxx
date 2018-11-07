/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/StartOnNewThread.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark sStartOnNewThread

#if ZCONFIG_API_Enabled(Thread_Win)
static __stdcall unsigned spCall(Callable<void()>* iCallable)
	{
	ZRef<Callable<void()>> theCallable = sAdopt& iCallable;
	theCallable->QCall();
	return 0;
	}
#else
static void spCall(Callable<void()>* iCallable)
	{
	ZRef<Callable<void()>> theCallable = sAdopt& iCallable;
	theCallable->QCall();
	}
#endif

void sStartOnNewThread(const ZRef<Callable<void()> >& iCallable)
	{
	if (iCallable)
		ZThread::sStartRaw(0, (ZThread::ProcRaw_t)spCall, iCallable.Copy());
	}

} // namespace ZooLib
