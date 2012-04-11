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

#ifndef __ZNetscape_Host_Win_h__
#define __ZNetscape_Host_Win_h__ 1
#include "zconfig.h"

#include "zoolib/netscape/ZNetscape_Host_Std.h"

#if defined(XP_WIN)

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - Host_Win

class Host_Win
:	public Host_Std
	{
public:
	Host_Win(ZRef<GuestFactory> iGuestFactory, HWND iHWND);
	virtual ~Host_Win();

// From Host_Std
	virtual NPError Host_GetValue(NPP npp, NPNVariable variable, void* ret_value);
	virtual NPError Host_SetValue(NPP npp, NPPVariable variable, void* value);
	virtual void Host_InvalidateRect(NPP npp, NPRect* rect);

// From Host_Std
	virtual void PostCreateAndLoad();

// Our protocol
	virtual void PaintBackground(HDC iHDC, const PAINTSTRUCT& iPS);

protected:
	ZQ<LRESULT> pWindowProc(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM);

	void pPaint(HWND iHWND, WPARAM iWPARAM, LPARAM iLPARAM);
	void pStuffNPWindow(int iWidth, int iHeight);

	HWND fHWND;
	bool fIsWindowed;
	bool fIsTransparent;
	NPWindow fNPWindow;
	UINT fTimerID;
	HWND fInnerWND;
	};

} // namespace ZNetscape
} // namespace ZooLib

#endif // defined(XP_WIN)

#endif // __ZNetscape_Host_Win_h__
