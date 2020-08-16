// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZWinWND_h__
#define __ZWinWND_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#include "zoolib/ZCompat_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

namespace ZooLib {
namespace ZWinWND {

// =================================================================================================
#pragma mark - ZWinWND::ClassRegistration

class ClassRegistration
	{
public:
	ClassRegistration(WNDPROC iWNDPROC, const WCHAR* iClassName, size_t iWndExtra);
	ClassRegistration(WNDPROC iWNDPROC, const WCHAR* iClassName);
	~ClassRegistration();

	const WCHAR* GetClassName() const;

private:
	void pInit(WNDPROC iWNDPROC, const WCHAR* iClassName, size_t iWndExtra);

	const WCHAR* fClassName;
	ATOM fATOM;
	};

// =================================================================================================
#pragma mark - sCreate_DefWindowProc

HWND sCreate_DefWindowProc(HWND iParent, DWORD iStyle, void* iCreateParam);
HWND sCreate_DefWindowProc(HWND iParent, DWORD iStyle, DWORD iExStyle, void* iCreateParam);

// =================================================================================================
#pragma mark - sCreate_DefDlgProc

HWND sCreate_DefDlgProc(HWND iParent, DWORD iStyle, void* iCreateParam);
HWND sCreate_DefDlgProc(HWND iParent, DWORD iStyle, DWORD iExStyle, void* iCreateParam);

// =================================================================================================
#pragma mark - ZWinWND, Callable <--> Regular window

typedef Callable<ZQ<LRESULT>(HWND,UINT,WPARAM,LPARAM)> Callable_t;

HWND sCreate(
	DWORD dwExStyle,
	LPCWSTR lpWindowName,
	DWORD dwStyle,
	int X,
	int Y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU hMenu,
	WNDPROC iWNDPROC,
	ZRef<Callable_t> iCallable);

HWND sCreate(HWND iParent, ZRef<Callable_t> iCallable);

bool sAttach(HWND iHWND, ZRef<Callable_t> iCallable);

ZRef<Callable_t> sGetCallable(HWND iHWND);

// =================================================================================================
#pragma mark - Message pump

void sSetActiveHWND(HWND iHWND);
HWND sGetActiveHWND();

bool sDoOneMessage();

bool sDoOneMessageForDialogs();

bool sDoOneMessageForDialog(HWND iHWND);

} // namespace ZWinWND
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinWND_h__
