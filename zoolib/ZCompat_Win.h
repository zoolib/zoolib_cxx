/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZCompat_Win_h__
#define __ZCompat_Win_h__
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

// Enable DCOM features
#define _WIN32_DCOM 1

// If not otherwise specified, require Windows 2K.
#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0500
#endif

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#ifndef STRICT
	#define STRICT
#endif

// Recent windows headers conditionalize visibility of GetModuleHandleExW on 
// _WIN32_WINNT being > 0x0500, not >= 0x0500.
// However it's also visible if WINBASE_DECLARE_GET_MODULE_HANDLE_EX is defined.
#define WINBASE_DECLARE_GET_MODULE_HANDLE_EX

#include <windows.h>

// We replace MAKEINTRESOURCE with a definition that exposes the int parameter, and forces
// code trying to use it to instead use MAKEINTRESOURCEW or MAKEINTRESOURCEA, as
// appropriate for the actual runtime.
#undef MAKEINTRESOURCE
#define MAKEINTRESOURCE(a) (a)

// Remove the definitions of all string-related windows api entry points --
// if you're using unicode strings, then do so *deliberately*.

// winbase.h
#undef Yield
#undef GetCurrentTime
#undef GetModuleHandle
#undef GetModuleFileName
#undef OutputDebugString
#undef FindResource
#undef FindResourceEx
#undef EnumResourceTypes
#undef EnumResourceNames
#undef EnumResourceLanguages
#undef BeginUpdateResource
#undef UpdateResource
#undef EndUpdateResource
#undef GlobalAddAtom
#undef GlobalFindAtom
#undef GlobalGetAtomName
#undef AddAtom
#undef FindAtom
#undef GetAtomName

// wingdi.h
#undef CreateDC
#undef CreateFontIndirect
#undef CreateFont
#undef CreateIC
#undef CreateRC
#undef CreateMetaFile
#undef CreateScalableFontResource
#undef EnumFontFamiliesEx
#undef EnumFontFamilies
#undef EnumFonts
#undef FONTENUMPROC
#undef NEWTEXTMETRICEX
#undef GetCharWidth
#undef GetCharWidth32
#undef GetCharWidthFloat
#undef GetCharABCWidths
#undef GetCharABCWidthsFloat
#undef GetGlyphOutline
#undef GetMetaFile
#undef CopyMetaFile
#undef GetOutlineTextMetrics
#undef GetTextExtentPoint
#undef GetTextExtentPoint32
#undef GetTextExtentExPoint
#undef GetCharacterPlacement
#undef GetGlyphIndices
#undef AddFontResource
#undef AddFontResourceEx
#undef RemoveFontResourceEx
#undef CreateFontIndirectEx
#undef ResetDC
#undef RemoveFontResource
#undef CopyEnhMetaFile
#undef CreateEnhMetaFile
#undef GetEnhMetaFile
#undef GetEnhMetaFileDescription
#undef GetTextMetrics
#undef StartDoc
#undef GetObject
#undef TextOut
#undef ExtTextOut
#undef PolyTextOut
#undef GetTextFace
#undef GetKerningPairs
#undef ICMENUMPROC
#undef GetLogColorSpace
#undef CreateColorSpace
#undef GetICMProfile
#undef SetICMProfile
#undef EnumICMProfiles
#undef UpdateICMRegKey
#undef OLDFONTENUMPROC

// winuser.h
#undef DispatchMessage
#undef GetMessage
#undef PeekMessage
#undef PostMessage
#undef SendMessage
#undef RegisterWindowMessage
#undef DefWindowProc
#undef CallWindowProc
#undef RegisterClass
#undef RegisterClassEx
#undef UnregisterClass
#undef GetClassInfo
#undef CreateWindowEx
#undef CreateDialog
#undef DialogBox
#undef DefDlgProc
#undef RegisterClipboardFormat
#undef GetClipboardFormatName
#undef CharToOem
#undef OemToChar
#undef CharToOemBuff
#undef CharUpper
#undef CharUpperBuff
#undef CharLower
#undef CharLowerBuff
#undef CharNext
#undef CharPrev
#undef IsCharAlpha
#undef IsCharAlphaNumeric
#undef IsCharUpper
#undef IsCharLower
#undef GetKeyNameText
#undef VkKeyScan
#undef VkKeyScanEx
#undef MapVirtualKey
#undef MapVirtualKeyEx
#undef LoadAccelerators
#undef CreateAcceleratorTable
#undef CopyAcceleratorTable
#undef TranslateAccelerator
#undef LoadMenu
#undef LoadMenuIndirect
#undef ChangeMenu
#undef GetMenuString
#undef InsertMenu
#undef AppendMenu
#undef ModifyMenu
#undef InsertMenuItem
#undef GetMenuItemInfo
#undef SetMenuItemInfo


#undef DrawText
#undef DrawTextEx
#undef GrayString
#undef DrawState
#undef TabbedTextOut
#undef GetTabbedTextExtent
#undef SetProp
#undef GetProp
#undef RemoveProp
#undef EnumPropsEx
#undef EnumProps
#undef SetWindowText
#undef GetWindowText
#undef GetWindowTextLength
#undef MessageBox
#undef MessageBoxEx
#undef GetWindowLong
#undef SetWindowLong
#undef GetWindowLongPtr
#undef SetWindowLongPtr
#undef GetClassLong
#undef SetClassLong
#undef GetClassLongPtr
#undef SetClassLongPtr
#undef FindWindow
#undef FindWindowEx
#undef GetClassName
#undef SetWindowsHook
#undef SetWindowsHookEx
#undef LoadBitmap
#undef LoadCursor
#undef LoadCursorFromFile
#undef LoadIcon
#undef LoadImage
#undef LoadString
#undef IsDialogMessage
#undef DefFrameProc
#undef DefMDIChildProc
#undef CreateMDIWindow
#undef WinHelp
#undef ChangeDisplaySettings
#undef ChangeDisplaySettingsEx
#undef EnumDisplaySettings
#undef EnumDisplayDevices
#undef SystemParametersInfo
#undef GetWindowModuleFileName
#undef RealGetWindowClass
#undef GetAltTabInfo
#undef wvsprintf
#undef wsprintf
#undef FormatMessage

// commdlg.h
#undef GetOpenFileName
#undef FindText
#undef ReplaceText
#undef ChooseFont
#undef PageSetupDlg
#undef PrintDlg

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZCompat_Win_h__
