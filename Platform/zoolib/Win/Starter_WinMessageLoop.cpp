// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Starter_WinMessageLoop.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/Callable_PMF.h"

#include "zoolib/ZWinWND.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Starter_WinMessageLoop

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
