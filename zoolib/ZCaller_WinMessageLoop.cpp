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

#include "zoolib/ZCaller_WinMessageLoop.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZWinWND.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCaller_WinMessageLoop

ZCaller_WinMessageLoop::ZCaller_WinMessageLoop()
:	fHWND(nullptr)
	{}

ZCaller_WinMessageLoop::~ZCaller_WinMessageLoop()
	{}

void ZCaller_WinMessageLoop::Initialize()
	{
	ZCaller_EventLoop::Initialize();

	fHWND = ZWinWND::sCreate(
		HWND_MESSAGE, sCallable(sWeakRef(this), &ZCaller_WinMessageLoop::pWindowProc));

	::SetWindowTextW(fHWND, L"ZCaller_WinMessageLoop::fHWND");
	}

void ZCaller_WinMessageLoop::Finalize()
	{
	if (fHWND)
		{
		::DestroyWindow(fHWND);
		ZAssert(not fHWND);
		}

	ZCaller_EventLoop::Finalize();
	}

void ZCaller_WinMessageLoop::Disable()
	{
	if (fHWND)
		{
		::DestroyWindow(fHWND);
		ZAssert(not fHWND);
		}
	ZCaller_EventLoop::pDiscardPending();
	}

static UINT spGetMSG()
	{
	static UINT spMSG = ::RegisterWindowMessageW(L"ZCaller_WinMessageLoop::Invoke");
	return spMSG;
	}

bool ZCaller_WinMessageLoop::pTrigger()
	{
	if (fHWND)
		{
		::PostMessageW(fHWND, spGetMSG(), 0, 0);
		return true;
		}
	return false;
	}

ZQ<LRESULT> ZCaller_WinMessageLoop::pWindowProc(
	HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	if (iMessage == spGetMSG())
		{
		ZCaller_EventLoop::pCall();
		return 0;
		}
	else if (iMessage == WM_NCDESTROY)
		{
		fHWND = nullptr;
		}
	return null;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
