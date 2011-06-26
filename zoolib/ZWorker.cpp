/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZWorker.h"

namespace ZooLib {

/**
\defgroup Worker

\brief ZWorker provides a disciplined lifecycle for long-lived repetitive jobs.

A ZWorker derivative overrides the ZWorker::Work method, and once attached to a ZWorkerRunner
Work will be called whenever the ZWorker has been woken, until Work returns false or allows
an exception to propogate out.
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker

/**
\class ZWorker
\ingroup Worker
\sa Worker
*/

void ZWorker::RunnerAttached()
	{
	if (ZRef<Callable_Attached_t> theCallable = fCallable_Attached)
		theCallable->Call(this);
	}

void ZWorker::RunnerDetached()
	{
	if (ZRef<Callable_Detached_t> theCallable = fCallable_Detached)
		theCallable->Call(this);
	}

void ZWorker::Kill()
	{}

void ZWorker::Wake()
	{
	if (ZRef<ZWorkerRunner> theRunner = fRunner)
		theRunner->Wake(this);
	}

void ZWorker::WakeIn(double iInterval)
	{
	if (ZRef<ZWorkerRunner> theRunner = fRunner)
		theRunner->WakeIn(this, iInterval);
	}

void ZWorker::WakeAt(ZTime iSystemTime)
	{
	if (ZRef<ZWorkerRunner> theRunner = fRunner)
		theRunner->WakeAt(this, iSystemTime);
	}

bool ZWorker::IsAwake()
	{
	if (ZRef<ZWorkerRunner> theRunner = fRunner)
		return theRunner->IsAwake(this);
	return false;
	}

bool ZWorker::IsAttached()
	{
	if (ZRef<ZWorkerRunner> theRunner = fRunner)
		return theRunner->IsAttached(this);
	return false;
	}

ZRef<ZWorker::Callable_Attached_t>
ZWorker::GetSetCallable_Attached(ZRef<Callable_Attached_t> iCallable)
	{ return fCallable_Attached.GetSet(iCallable); }

ZRef<ZWorker::Callable_Detached_t>
ZWorker::GetSetCallable_Detached(ZRef<Callable_Detached_t> iCallable)
	{ return fCallable_Detached.GetSet(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner

/**
\class ZWorkerRunner
\ingroup Worker
\sa Worker
*/

bool ZWorkerRunner::pAttachWorker(ZRef<ZWorker> iWorker)
	{
	ZAssert(iWorker);
	ZAssert(not iWorker->fRunner.Get());

	iWorker->fRunner = MakeRef(this);

	try
		{
		iWorker->RunnerAttached();
		return true;
		}
	catch (...)
		{
		iWorker->fRunner.Clear();
		}

	return false;
	}

void ZWorkerRunner::pDetachWorker(ZRef<ZWorker> iWorker)
	{
	ZAssert(iWorker);
	ZAssert(iWorker->fRunner.Get() == this);

	iWorker->fRunner.Clear();

	try { iWorker->RunnerDetached(); }
	catch (...) {}
	}

bool ZWorkerRunner::pInvokeWork(ZRef<ZWorker> iWorker)
	{
	try { return iWorker->Work(); }
	catch (...) {}
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

void sStartWorkerRunner(ZRef<ZWorker> iWorker)
	{
	bool result = ZFunctionChain_T<ZRef<ZWorkerRunner>, ZRef<ZWorker> >::sInvoke(iWorker);
	ZAssert(result);
	iWorker->Wake();
	}

} // namespace ZooLib
