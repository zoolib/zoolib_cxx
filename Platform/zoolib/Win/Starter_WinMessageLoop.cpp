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

#include "zoolib/Starter_WinMessageLoop.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/Callable_PMF.h"

#include "zoolib/ZWinWND.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Starter_WinMessageLoop

Starter_WinMessageLoop::Starter_WinMessageLoop()
:	fHWND(nullptr)
	{}

Starter_WinMessageLoop::~Starter_WinMessageLoop()
	{}

void Starter_WinMessageLoop::Initialize()
	{
	Starter_EventLoopBase::Initialize();

	fHWND = ZWinWND::sCreate(
		HWND_MESSAGE, sCallable(sWeakRef(this), &Starter_WinMessageLoop::pWindowProc));

	::SetWindowTextW(fHWND, L"Starter_WinMessageLoop::fHWND");
	}

void Starter_WinMessageLoop::Finalize()
	{
	if (fHWND)
		{
		::DestroyWindow(fHWND);
		ZAssert(not fHWND);
		}
	Starter_EventLoopBase::Finalize();
	}

void Starter_WinMessageLoop::Disable()
	{
	if (fHWND)
		{
		::DestroyWindow(fHWND);
		ZAssert(not fHWND);
		}
	Starter_EventLoopBase::pDiscardPending();
	}

static UINT spGetMSG()
	{
	static UINT spMSG = ::RegisterWindowMessageW(L"Starter_WinMessageLoop::Invoke");
	return spMSG;
	}

bool Starter_WinMessageLoop::pTrigger()
	{
	if (fHWND)
		{
		::PostMessageW(fHWND, spGetMSG(), 0, 0);
		return true;
		}
	return false;
	}

ZQ<LRESULT> Starter_WinMessageLoop::pWindowProc(
	HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	if (iMessage == spGetMSG())
		{
		Starter_EventLoopBase::pInvokeClearQueue();
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
