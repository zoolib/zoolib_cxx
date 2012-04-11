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

#include "zoolib/ZCallable_Bind.h"
#include "zoolib/ZCallable_Function.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZThreadVal.h"
#include "zoolib/ZUtil_Win.h"

namespace ZooLib {
namespace ZWinWND {

// =================================================================================================
// MARK: - Helpers (anonymous)

namespace { // anonymous

class Callable_Fallback
:	public Callable
	{
public:
	Callable_Fallback(const ZRef<Callable>& i0, const ZRef<Callable>& i1)
	:	f0(i0)
	,	f1(i1)
		{}

	virtual ZQ<ZQ<LRESULT> > QCall(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
		{
		if (ZQ<LRESULT> theQ = f0->Call(iHWND, iMessage, iWPARAM, iLPARAM))
			return theQ;
		return f1->Call(iHWND, iMessage, iWPARAM, iLPARAM);
		}

	const ZRef<Callable> f0;
	const ZRef<Callable> f1;
	};

class Callable_WithWNDPROC
:	public Callable
	{
public:
	Callable_WithWNDPROC(const ZRef<Callable>& iCallable, WNDPROC iProc)
	:	fCallable(iCallable)
	,	fProc(iProc)
		{}

	virtual ZQ<ZQ<LRESULT> > QCall(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
		{
		if (ZQ<LRESULT> theQ = fCallable->Call(iHWND, iMessage, iWPARAM, iLPARAM))
			return theQ;
		return ::CallWindowProcW(fProc, iHWND, iMessage, iWPARAM, iLPARAM);
		}

	const ZRef<Callable> fCallable;
	const WNDPROC fProc;
	};

LRESULT CALLBACK spWindowProcW(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	ZRef<Callable> theCallable = (Callable*)::GetPropW(iHWND, L"ZWinWND Callable");

	if (not theCallable)
		{
		// The very first message sent to a window is WM_GETMINMAXINFO.
		ZAssert(iMessage == WM_GETMINMAXINFO);
		theCallable =  ZThreadVal<ZRef<Callable> >::sGet();
		theCallable->Retain();
		::SetPropW(iHWND, L"ZWinWND Callable", theCallable.Get());
		}

	if (iMessage == WM_NCDESTROY)
		{
		// Undo the Retain we did when we attached.
		theCallable->Release();
		}

	if (ZQ<LRESULT> theQ = theCallable->Call(iHWND, iMessage, iWPARAM, iLPARAM))
		return *theQ;

	return 0;
	}

INT_PTR CALLBACK spDialogProcW(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	if (ZRef<Callable_Dialog> theCallable =
		reinterpret_cast<Callable_Dialog*>((long long)::GetWindowLongPtrW(iHWND, GWLP_USERDATA)))
		{
		if (iMessage == WM_NCDESTROY)
			{
			// Undo the Retain we did in WM_INITDIALOG.
			theCallable->Release();
			}
		return theCallable->Call(iHWND, iMessage, iWPARAM, iLPARAM);
		}
	else if (iMessage == WM_INITDIALOG)
		{
		::SetWindowLongPtrW(iHWND, GWLP_USERDATA, iLPARAM);
		theCallable = (Callable_Dialog*)iLPARAM;
		theCallable->Retain();
		return theCallable->Call(iHWND, iMessage, iWPARAM, iLPARAM);
		}
	else
		{
		return false;
		}
	}

} // anonymous namespace

// =================================================================================================
// MARK: - ZWinWND::ClassRegistration

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
	if (not ::UnregisterClassW(fClassName, ZUtil_Win::sGetModuleHandle()))
		{
		if (ZLOGPF(s, eInfo))
			s << "Failed for << " << fClassName;
		}
	}

const WCHAR* ClassRegistration::GetClassName() const
	{ return fClassName; }

// =================================================================================================
// MARK: - sCreateDefWindowProc

HWND ZWinWND::sCreateDefWindowProc(HWND iParent, DWORD iStyle, void* iCreateParam)
	{
	static ClassRegistration spClassRegistration(DefWindowProcW, L"DefWindowProcW");

	return ::CreateWindowExW
		(0, // Extended attributes
		spClassRegistration.GetClassName(),
		nullptr, // window caption
		iStyle, // window style
		0, // initial x position
		0, // initial y position
		10, // initial x size
		10, // initial y size
		iParent, // Parent window
		nullptr,
		ZUtil_Win::sGetModuleHandle(),
		iCreateParam); // creation parameters
	}

// =================================================================================================
// MARK: - ZWinWND, Callable <--> Regular window

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
	ZRef<Callable> iCallable)
	{
	static ClassRegistration spClassRegistration(spWindowProcW, L"ZWinWND ClassRegistration");

	ZThreadVal<ZRef<Callable> > theCallableTV = new Callable_WithWNDPROC(iCallable, iWNDPROC);

	return ::CreateWindowExW
		(dwExStyle,
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
		nullptr); // creation parameters
	}

HWND sCreate(HWND iParent, ZRef<Callable> iCallable)
	{
	return sCreate
		(0, // Extended attributes
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

bool sAttach(HWND iHWND, ZRef<Callable> iCallable)
	{
	if (iCallable)
		{
		DWORD windowProcessID;
		::GetWindowThreadProcessId(iHWND, &windowProcessID);

		if (windowProcessID == ::GetCurrentProcessId())
			{
			WNDPROC priorWNDPROC = (WNDPROC)(LPARAM)
				::SetWindowLongPtrW(iHWND, GWLP_WNDPROC, (LPARAM)spWindowProcW);

			ZRef<Callable> theCallable;
			if (priorWNDPROC != spWindowProcW)
				{
				theCallable = new Callable_WithWNDPROC(iCallable, priorWNDPROC);
				}
			else
				{
				theCallable = (Callable*)::GetPropW(iHWND, L"ZWinWND Callable");
				theCallable->Release();
				theCallable = new Callable_Fallback(iCallable, theCallable);
				}

			theCallable->Retain();
			::SetPropW(iHWND, L"ZWinWND Callable", theCallable.Get());
			}
		}
	return false;
	}

bool sDoOneMessage()
	{
	MSG theMSG;
	if (not ::GetMessageW(&theMSG, nullptr, 0, 0))
		return false;

	::TranslateMessage(&theMSG);
	::DispatchMessageW(&theMSG);

	return true;
	}

// =================================================================================================
// MARK: - ZWinWND, Callable <--> Dialog

HWND sCreateDialog(LPCWSTR lpTemplate, LCID iLCID, HWND hWndParent, ZRef<Callable_Dialog> iCallable)
	{
	HMODULE theHMODULE = ZUtil_Win::sGetModuleHandle();

	if (HRSRC theHRSRC = ::FindResourceExW(theHMODULE, (LPCWSTR)RT_DIALOG, lpTemplate, iLCID))
		{
		HGLOBAL theHGLOBAL = ::LoadResource(theHMODULE, theHRSRC);
		return ::CreateDialogIndirectParamW
			(theHMODULE,
			(LPCDLGTEMPLATE)::LockResource(theHGLOBAL),
			hWndParent,
			spDialogProcW,
			(LPARAM)iCallable.Get());
		}

	return ::CreateDialogParamW
		(theHMODULE,
		lpTemplate,
		hWndParent,
		spDialogProcW,
		(LPARAM)iCallable.Get());
	}

HWND sCreateDialog(LPCWSTR lpTemplate, HWND hWndParent, ZRef<Callable_Dialog> iCallable)
	{ return sCreateDialog(lpTemplate, ::GetThreadLocale(), hWndParent, iCallable); }

bool sDoOneMessageForDialog(HWND iHWND)
	{
	MSG theMSG;
	if (not ::GetMessageW(&theMSG, nullptr, 0, 0))
		return false;

	if (not ::IsDialogMessageW(iHWND, &theMSG))
		{
		::TranslateMessage(&theMSG);
		::DispatchMessageW(&theMSG);
		}

	return true;
	}

} // namespace ZWinWND
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
