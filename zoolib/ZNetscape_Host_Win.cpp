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

#include "zoolib/ZNetscape_Host_Win.h"

#if defined(XP_WIN)

#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * Host_Win

Host_Win::Host_Win(ZRef<GuestFactory> iGuestFactory)
:	Host_Std(iGuestFactory),
	fIsWindowless(false),
	fIsTransparent(false)
	{
	ZBlockZero(&fNPWindow, sizeof(fNPWindow));
	}

Host_Win::~Host_Win()
	{
	}

NPError Host_Win::Host_GetValue(NPP npp, NPNVariable variable, void* ret_value)
	{
	switch (variable)
		{
		case NPNVnetscapeWindow:
			{
			*static_cast<HWND*>(ret_value) = fInnerWNDA->GetHWND();//fHWND;
			return NPERR_NO_ERROR;
			}
		}

	return Host_Std::Host_GetValue(npp, variable, ret_value);
	}

NPError Host_Win::Host_SetValue(NPP npp, NPPVariable variable, void* value)
	{
	switch (variable)
		{
		case NPPVpluginWindowBool:
			{
			fIsWindowless = (value == 0);
			return NPERR_NO_ERROR;
			}
		case NPPVpluginTransparentBool:
			{
			fIsTransparent = (value != 0);
			return NPERR_NO_ERROR;
			}
		}

	return Host_Std::Host_SetValue(npp, variable, value);
	}

void Host_Win::AttachHWND(HWND iHWND)
	{
	if (ZLOG(s, eDebug, "Host_Win"))
		s << "AttachHWND";

	this->Attach(iHWND);
	fTimerID = ::SetTimer(iHWND, 1, 50, nil);

	fInnerWNDA = new ZWNDA(DefWindowProcA);
	fInnerWNDA->Create(iHWND, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN);
	::SetWindowPos(fInnerWNDA->GetHWND(), nil, 0, 0, 400, 200, 0);

//	fNPWindow.window = nil;
//	this->pStuffNPWindow(0, 0, 400, 200);
	}

void Host_Win::DoActivate(bool iActivate)
	{
	}

void Host_Win::DoFocus(bool iFocused)
	{
	}

void Host_Win::DoIdle()
	{
	}

void Host_Win::pSendSetWindow()
	{
    RECT theCR;
    ::GetClientRect(fHWND, &theCR);
    WNDPROC priorPROC = (WNDPROC)::GetWindowLongA(fHWND, GWL_WNDPROC);
    
	this->pStuffNPWindow(
		theCR.left,
		theCR.top,
		theCR.right - theCR.left,
		theCR.bottom - theCR.top);	

    WNDPROC afterPROC = (WNDPROC)::GetWindowLongA(fHWND, GWL_WNDPROC);
	}

void Host_Win::pStuffNPWindow(int iLeft, int iTop, int iWidth, int iHeight)
	{
	if (fIsWindowless)
		{
		fNPWindow.type = NPWindowTypeDrawable;
		}
	else
		{
		fNPWindow.type = NPWindowTypeWindow;
		fNPWindow.window = fInnerWNDA->GetHWND();//fHWND;
		}
	fNPWindow.x = 0;
	fNPWindow.y = 0;
	fNPWindow.width = iWidth;
	fNPWindow.height = iHeight;
	fNPWindow.clipRect.left = 0;
	fNPWindow.clipRect.top = 0;
	fNPWindow.clipRect.right = iWidth;
	fNPWindow.clipRect.bottom = iHeight;
	this->Guest_SetWindow(&fNPWindow);
	}

void Host_Win::DoPAINT(HWND iHWND, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	PAINTSTRUCT thePS;
	HDC theHDC = ::BeginPaint(iHWND, &thePS);
    RECT theCR;
    ::GetClientRect(iHWND, &theCR);
	::FillRect(theHDC, &theCR, (HBRUSH) ::GetStockObject(GRAY_BRUSH));

    if (fIsWindowless)
    	{
		fNPWindow.window = theHDC;
		this->pStuffNPWindow(
			theCR.left,
			theCR.top,
			theCR.right - theCR.left,
			theCR.bottom - theCR.top);

		NPRect paintRect;
		paintRect.left = theCR.left;
		paintRect.top = theCR.top;
		paintRect.right = theCR.right;
		paintRect.bottom = theCR.bottom;

		NPEvent theNPEvent;
		theNPEvent.event = WM_PAINT;
		theNPEvent.wParam = iWPARAM;
		theNPEvent.lParam = (LPARAM) &paintRect;
		this->Guest_HandleEvent(&theNPEvent);
		}

	::EndPaint(iHWND, &thePS);
	}

LRESULT Host_Win::WindowProc(HWND iHWND, UINT iMessage, WPARAM iWPARAM, LPARAM iLPARAM)
	{
	switch (iMessage)
		{
		case WM_TIMER:
			{
			this->DeliverData();
			return 0;
			}
		case WM_PAINT:
			{
			this->DoPAINT(iHWND, iWPARAM, iLPARAM);
			return 0;
			}
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			{
			if (fIsWindowless)
				{
				NPEvent theNPEvent;
				theNPEvent.event = iMessage;
				theNPEvent.wParam = iWPARAM;
				theNPEvent.lParam = iLPARAM;
				this->Guest_HandleEvent(&theNPEvent);
				return 0;				
				}
			break;
			}
		case WM_WINDOWPOSCHANGED:
			{
			WINDOWPOS* theWPOS = reinterpret_cast<WINDOWPOS*>(iLPARAM);
			if ((theWPOS->flags & SWP_FRAMECHANGED))
				{
				this->pStuffNPWindow(
					theWPOS->x,
					theWPOS->y,
					theWPOS->cx,
					theWPOS->cy);
				}			
			break;
			}
		}
	return ZWNDSubClassA::WindowProc(iHWND, iMessage, iWPARAM, iLPARAM);
	}

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // defined(XP_WIN)
