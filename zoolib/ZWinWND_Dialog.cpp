/* -------------------------------------------------------------------------------------------------
Copyright (c) 2013 Andrew Green
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

#include "zoolib/ZWinWND_Dialog.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZUtil_Win.h"

namespace ZooLib {
namespace ZWinWND {

// =================================================================================================
// MARK: - Callable <--> Dialog

static
INT_PTR CALLBACK spDialogProcW(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	if (ZRef<Callable_Dialog> theCallable =
		(Callable_Dialog*)::GetWindowLongPtrW(iHWND, GWLP_USERDATA))
		{
		switch (iMessage)
			{
			case WM_NCDESTROY:
				{
				// Undo the Retain we did in WM_INITDIALOG.
				theCallable->Release();
				break;
				}
			case WM_ACTIVATE:
				{
				if (iWPARAM)
					sSetActiveHWND(iHWND);
				else
					sSetActiveHWND(nullptr);
				break;
				}
			default:
				{
				break;
				}
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

} // namespace ZWinWND
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
