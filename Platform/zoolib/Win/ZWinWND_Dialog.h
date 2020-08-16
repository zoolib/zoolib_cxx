// Copyright (c) 2013 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZWinWND_Dialog_h__
#define __ZWinWND_Dialog_h__ 1
#include "zconfig.h"

#include "zoolib/ZWinWND.h"

#if ZCONFIG_SPI_Enabled(Win)

namespace ZooLib {
namespace ZWinWND {

// =================================================================================================
#pragma mark - ZWinWND, Callable <--> Dialog

typedef Callable<INT_PTR(HWND,UINT,WPARAM,LPARAM)> Callable_Dialog;

HWND sCreateDialog(LPCWSTR lpTemplate, LCID iLCID, HWND hWndParent, ZRef<Callable_Dialog> iCallable);
HWND sCreateDialog(LPCWSTR lpTemplate, HWND hWndParent, ZRef<Callable_Dialog> iCallable);

} // namespace ZWinWND
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinWND_h__
