/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZWorkerRunner_EventLoop_Carbon.h"

#if ZCONFIG_SPI_Enabled(Carbon64)

#include "zoolib/ZSafeRef.h"
#include "zoolib/ZUtil_CarbonEvents.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace { // anonymous

ZSafeRef<ZWorkerRunner_EventLoop_Carbon> spRunner(new ZWorkerRunner_EventLoop_Carbon);

ZRef<ZWorkerRunner_EventLoop_Carbon> spGetRunner()
	{ return spRunner; }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_EventLoop_Carbon

/**
\class ZWorkerRunner_EventLoop_Carbon
\ingroup Worker
\sa Worker
*/

ZWorkerRunner_EventLoop_Carbon::ZWorkerRunner_EventLoop_Carbon()
	{}

ZWorkerRunner_EventLoop_Carbon::~ZWorkerRunner_EventLoop_Carbon()
	{}

void ZWorkerRunner_EventLoop_Carbon::pQueueCallback()
	{ ZUtil_CarbonEvents::sInvokeOnMainThread(spCallback, this); }

void ZWorkerRunner_EventLoop_Carbon::sStartWorker(ZRef<ZWorker> iWorker)
	{
	if (ZRef<ZWorkerRunner_EventLoop_Carbon> theRunner = spGetRunner())
		theRunner->pStartWorker(iWorker);
	}

void ZWorkerRunner_EventLoop_Carbon::spCallback(void* iRefcon)
	{
	if (ZRef<ZWorkerRunner_EventLoop_Carbon> theRunner =
		static_cast<ZWorkerRunner_EventLoop_Carbon*>(iRefcon))
		{
		theRunner->pCallback();
		}
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Carbon64)
