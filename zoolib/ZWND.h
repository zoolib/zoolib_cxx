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

#ifndef __ZWND__
#define __ZWND__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZWNDW

class ZWNDW
	{
public:
	static HWND sCreateDefault(HWND iParent, DWORD iStyle, void* iCreateParam);

	ZWNDW(WNDPROC iWNDPROC);
	virtual ~ZWNDW();

	void Create(HWND iParent, DWORD iStyle);

	HWND GetHWND();

	virtual void HWNDDestroyed(HWND iHWND);

	virtual LRESULT WindowProc(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM);

	LRESULT CallBase(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM);

	static LRESULT CALLBACK sWindowProcW(
		HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM);

protected:
	static ZWNDW* sFromHWNDNilOkayW(HWND iHWND);

	HWND fHWND;
	WNDPROC fWNDPROC;	
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWNDSubClassW

class ZWNDSubClassW
	{
public:
	ZWNDSubClassW();
	virtual ~ZWNDSubClassW();

	void Attach(HWND iHWND);
	void Detach();

	HWND GetHWND();

	virtual void HWNDDestroyed();

	virtual LRESULT WindowProc(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM);

	LRESULT CallBase(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM);

	static LRESULT CALLBACK sWindowProcW(
		HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM);

protected:
	static ZWNDSubClassW* sFromHWNDNilOkayW(HWND iHWND);

	HWND fHWND;
	WNDPROC fWNDPROC;	
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWND__
