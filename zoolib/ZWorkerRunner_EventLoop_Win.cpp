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

#include "zoolib/ZWorkerRunner_EventLoop_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZSafeRef.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace { // anonymous

ZSafeRef<ZWorkerRunner_EventLoop_Win> spRunner(new ZWorkerRunner_EventLoop_Win);

ZRef<ZWorkerRunner_EventLoop_Win> spGetRunner()
	{ return spRunner; }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner_EventLoop_Win

/**
\class ZWorkerRunner_EventLoop_Win
\ingroup Worker
\sa Worker
*/

ZWorkerRunner_EventLoop_Win::ZWorkerRunner_EventLoop_Win()
:	ZWNDW(::DefWindowProcW)
	{}

ZWorkerRunner_EventLoop_Win::~ZWorkerRunner_EventLoop_Win()
	{}

void ZWorkerRunner_EventLoop_Win::Initialize()
	{
	ZWorkerRunner_EventLoop::Initialize();
	ZWNDW::Create(0, 0);
	}

static UINT spMSG_Invoke = ::RegisterWindowMessageW(L"ZWorkerRunner_EventLoop_Win::Invoke");

void ZWorkerRunner_EventLoop_Win::pQueueCallback()
	{ ::PostMessageW(this->GetHWND(), spMSG_Invoke, 0, 0); }

void ZWorkerRunner_EventLoop_Win::sAttach(ZRef<ZWorker> iWorker)
	{
	if (ZRef<ZWorkerRunner_EventLoop_Win> theRunner = spGetRunner())
		theRunner->pAttach(iWorker);
	}

LRESULT ZWorkerRunner_EventLoop_Win::WindowProc(
	HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	if (iMessage == spMSG_Invoke)
		{
		ZWorkerRunner_EventLoop::pCallback();
		return 0;
		}
	return ZWNDW::WindowProc(iHWND, iMessage, iWPARAM, iLPARAM);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
