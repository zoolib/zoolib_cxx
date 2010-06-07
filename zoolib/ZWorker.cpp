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
#include "zoolib/ZWorker.h"

namespace ZooLib {

/**
\defgroup Worker

ZWorker provides a disciplined lifecycle for long-lived repetitive tasks.

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
	{}

void ZWorker::RunnerDetached()
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
	{ return ZRef<ZWorkerRunner>(fRunner); }

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner

/**
\class ZWorkerRunner
\ingroup Worker
\sa Worker
*/

void ZWorkerRunner::pAttachWorker(ZRef<ZWorker> iWorker)
	{
	ZAssert(iWorker);
	ZAssert(! ZRef<ZWorkerRunner>(iWorker->fRunner));

	iWorker->fRunner = ZRef<ZWorkerRunner>(this);

	try
		{
		iWorker->RunnerAttached();
		}
	catch (...)
		{}
	}

void ZWorkerRunner::pDetachWorker(ZRef<ZWorker> iWorker)
	{
	ZAssert(iWorker);
	ZAssert(this == ZRef<ZWorkerRunner>(iWorker->fRunner));

	iWorker->fRunner.Clear();

	try
		{
		iWorker->RunnerDetached();
		}
	catch (...)
		{}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

void sStartWorkerRunner(ZRef<ZWorker> iWorker)
	{
	bool result = ZFunctionChain_T<ZRef<ZWorkerRunner>, ZRef<ZWorker> >::sInvoke(iWorker);
	ZAssert(result);
	}

} // namespace ZooLib
