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

#ifndef __ZWinWND_h__
#define __ZWinWND_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZCompat_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

namespace ZooLib {
namespace ZWinWND {

// =================================================================================================
// MARK: - ZWinWND::ClassRegistration

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
// MARK: - sCreate_DefWindowProc

HWND sCreate_DefWindowProc(HWND iParent, DWORD iStyle, void* iCreateParam);
HWND sCreate_DefWindowProc(HWND iParent, DWORD iStyle, DWORD iExStyle, void* iCreateParam);

// =================================================================================================
// MARK: - sCreate_DefDlgProc

HWND sCreate_DefDlgProc(HWND iParent, DWORD iStyle, void* iCreateParam);
HWND sCreate_DefDlgProc(HWND iParent, DWORD iStyle, DWORD iExStyle, void* iCreateParam);

// =================================================================================================
// MARK: - ZWinWND, Callable <--> Regular window

typedef ZCallable<ZQ<LRESULT>(HWND,UINT,WPARAM,LPARAM)> Callable;

HWND sCreate
	(DWORD dwExStyle,
	LPCWSTR lpWindowName,
	DWORD dwStyle,
	int X,
	int Y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU hMenu,
	WNDPROC iWNDPROC,
	ZRef<Callable> iCallable);

HWND sCreate(HWND iParent, ZRef<Callable> iCallable);

bool sAttach(HWND iHWND, ZRef<Callable> iCallable);

// =================================================================================================
// MARK: - ZWinWND, Callable <--> Dialog

typedef ZCallable<INT_PTR(HWND,UINT,WPARAM,LPARAM)> Callable_Dialog;

HWND sCreateDialog(LPCWSTR lpTemplate, LCID iLCID, HWND hWndParent, ZRef<Callable_Dialog> iCallable);
HWND sCreateDialog(LPCWSTR lpTemplate, HWND hWndParent, ZRef<Callable_Dialog> iCallable);

// =================================================================================================
// MARK: - Message pump

bool sDoOneMessage();

bool sDoOneMessageForDialogs();

bool sDoOneMessageForDialog(HWND iHWND);

} // namespace ZWinWND
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinWND_h__
