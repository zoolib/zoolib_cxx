/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZWorkerRunner.h"

/*
If you get duplicate symbol errors when linking, check that you're not including both
this file and ZWorkerRunner.cpp. The cpp file is what you should use in most circumstances,
but if you're building for the older ObjC runtime (i.e Mac 32 bit, Cocotron) then it can
be a good idea to use this file *instead* -- it guards against the unruly propogation of
unhandled ObjC exceptions which bypass the C++ dtor chain.
*/

#if !defined(__OBJC2__)

#include <Foundation/NSException.h>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner, for Objective C older runtime

bool ZWorkerRunner::pAttachWorker(ZRef<ZWorker> iWorker)
	{
	ZAssert(iWorker);
	ZAssert(not iWorker->fRunner.Get());

	iWorker->fRunner = MakeRef(this);

	NS_DURING
	try
		{
		if (ZRef<ZWorker::Callable_Attached_t> theCallable = iWorker->fCallable_Attached)
			theCallable->Call(iWorker);

		NS_DURING
		try
			{
			iWorker->RunnerAttached();
			return true;
			}
		catch (...)
			{}
		NS_HANDLER
		NS_ENDHANDLER

		NS_DURING
		try
			{
			if (ZRef<ZWorker::Callable_Detached_t> theCallable = iWorker->fCallable_Detached)
				theCallable->Call(iWorker);
			}
		catch (...)
			{}
		NS_HANDLER
		NS_ENDHANDLER
		}
	catch (...)
		{}
	NS_HANDLER
	NS_ENDHANDLER

	iWorker->fRunner.Clear();

	return false;
	}

void ZWorkerRunner::pDetachWorker(ZRef<ZWorker> iWorker)
	{
	ZAssert(iWorker);
	ZAssert(iWorker->fRunner.Get() == this);

	iWorker->fRunner.Clear();

	NS_DURING
	try { iWorker->RunnerDetached(); }
	catch (...)
		{}
	NS_HANDLER
	NS_ENDHANDLER

	NS_DURING
	try
		{
		if (ZRef<ZWorker::Callable_Detached_t> theCallable = iWorker->fCallable_Detached)
			theCallable->Call(iWorker);
		}
	catch (...)
		{}
	NS_HANDLER
	NS_ENDHANDLER
	}

bool ZWorkerRunner::pInvokeWork(ZRef<ZWorker> iWorker)
	{
	NS_DURING
	try { return iWorker->Work(); }
	catch (...) {}
	NS_HANDLER
	NS_ENDHANDLER
	return false;
	}

} // namespace ZooLib

#endif // !defined(__OBJC2__)
