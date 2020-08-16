// Copyright (c) 2013 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZWinWND_Dialog.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZUtil_Win.h"

namespace ZooLib {
namespace ZWinWND {

// =================================================================================================
#pragma mark - Callable <--> Dialog

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
		return ::CreateDialogIndirectParamW(
			theHMODULE,
			(LPCDLGTEMPLATE)::LockResource(theHGLOBAL),
			hWndParent,
			spDialogProcW,
			(LPARAM)iCallable.Get());
		}

	return ::CreateDialogParamW(
		theHMODULE,
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
