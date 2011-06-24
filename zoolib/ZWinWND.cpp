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

#include "zoolib/ZWinWND.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZDebug.h"
//#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Win.h"

namespace ZooLib {
namespace ZWinWND {

// =================================================================================================
#pragma mark -
#pragma mark * Helpers (anonymous)

namespace { // anonymous

ZRef<Callable_WNDPROC> spGetCallable(HWND iHWND)
	{
	DWORD windowProcessID;
	::GetWindowThreadProcessId(iHWND, &windowProcessID);

	if (windowProcessID == ::GetCurrentProcessId())
		return (Callable_WNDPROC*)::GetPropW(iHWND, L"WinWND Callable");

	return null;
	}

WNDPROC spGetBasePROC(HWND iHWND)
	{ return (WNDPROC)::GetPropW(iHWND, L"ZWinWND BasePROC"); }

void spAttach(HWND iHWND, WNDPROC basePROC, ZRef<Callable_WNDPROC> iCallable)
	{
	::SetPropW(iHWND, L"ZWinWND BasePROC", basePROC);

	iCallable->Retain();
	::SetPropW(iHWND, L"WinWND Callable", iCallable.Get());
	}

struct CreateStruct
	{
	WNDPROC fWNDPROC;
	ZRef<Callable_WNDPROC> fCallable;
	};

LRESULT CALLBACK spWindowProcW(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	if (WNDPROC baseProc = spGetBasePROC(iHWND))
		{
		if (ZRef<Callable_WNDPROC> theCallable = spGetCallable(iHWND))
			{
			if (iMessage == WM_NCDESTROY)
				{
				// Undo the Retain we did when we attached.
				theCallable->Release();
				}
			return theCallable->Call(baseProc, iHWND, iMessage, iWPARAM, iLPARAM);
			}
		else
			{
			return ::CallWindowProcW(baseProc, iHWND, iMessage, iWPARAM, iLPARAM);
			}
		}
	else
		{
		// We're in the process of being created.
		if (iMessage == WM_GETMINMAXINFO)
			{
			// The *very* first message sent to a window is (go figure) WM_GETMINMAXINFO.
			// We don't have access to our CREATESTRUCTW, so we have to handle it specially.
			return ::DefWindowProcW(iHWND, iMessage, iWPARAM, iLPARAM);
			}
		else if (iMessage == WM_NCCREATE)
			{
			// This is the second message sent to a window, and is where we're
			// able to attach to our Callable.
			CreateStruct* theCS = (CreateStruct*)((CREATESTRUCTW*)iLPARAM)->lpCreateParams;
			spAttach(iHWND, theCS->fWNDPROC, theCS->fCallable);
			return theCS->fCallable->Call(theCS->fWNDPROC, iHWND, iMessage, iWPARAM, iLPARAM);
			}
		else
			{
			// Can't get here
			ZUnimplemented();
			return 0;
			}
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZWinWND::ClassRegistration

ClassRegistration::ClassRegistration(WNDPROC iWNDPROC, const WCHAR* iClassName)
:	fClassName(iClassName)
	{
	WNDCLASSW windowClass;
	windowClass.style = 0;
	windowClass.lpfnWndProc = iWNDPROC;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = ZUtil_Win::sGetModuleHandle();
	windowClass.hIcon = nullptr;
	windowClass.hCursor = ::LoadCursorW(nullptr, MAKEINTRESOURCEW(IDC_ARROW));
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = iClassName;
	fATOM = ::RegisterClassW(&windowClass);
	ZAssertStop(0, fATOM != 0);
	}

ClassRegistration::~ClassRegistration()
	{
	bool result = ::UnregisterClassW(fClassName, ZUtil_Win::sGetModuleHandle());
	// Result will be false if there are still windows using this class.
	// ZAssert(result);
	}

const WCHAR* ClassRegistration::GetClassName() const
	{ return fClassName; }

// =================================================================================================
#pragma mark -
#pragma mark * ZWinWND

static ClassRegistration spClassRegistration(spWindowProcW, L"ZWinWND ClassRegistration");

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
	ZRef<Callable_WNDPROC> iCallable)
	{
	CreateStruct theCS = { iWNDPROC, iCallable };
	
	return ::CreateWindowExW(
		dwExStyle,
		spClassRegistration.GetClassName(),
		lpWindowName,
		dwStyle,
		X,
		Y,
		nWidth,
		nHeight,
		hWndParent,
		hMenu,
		ZUtil_Win::sGetModuleHandle(),
		&theCS); // creation parameters
	}

HWND sCreate(HWND iParent, ZRef<Callable_WNDPROC> iCallable)
	{
	return sCreate(
		0, // Extended attributes
		nullptr, // window caption
		0, // window style
		0, // initial x position
		0, // initial y position
		10, // initial x size
		10, // initial y size
		iParent, // Parent window
		nullptr, // menu
		DefWindowProcW,
		iCallable);
	}

bool sAttach(HWND iHWND, ZRef<Callable_WNDPROC> iCallable)
	{
	if (iCallable)
		{
		DWORD windowProcessID;
		::GetWindowThreadProcessId(iHWND, &windowProcessID);

		if (windowProcessID == ::GetCurrentProcessId())
			{
			WNDPROC basePROC = (WNDPROC)(LPARAM)
				::SetWindowLongPtrW(iHWND, GWLP_WNDPROC, (LPARAM)spWindowProcW);

			spAttach(iHWND, basePROC, iCallable);

			return true;
			}
		}
	return false;
	}

} // namespace ZWinWND
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
