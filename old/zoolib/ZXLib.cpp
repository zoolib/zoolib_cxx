/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZXLib.h"

// =================================================================================================
#if ZCONFIG_SPI_Enabled(X11)

#include "zoolib/ZThreadOld.h"

#include <X11/Xproto.h> // For definitions of request codes

namespace ZooLib {

ZMutex ZXLib::sMutex;

// =================================================================================================
// MARK: - ZXLib::ErrorTrap

ZXLib::ErrorTrap* ZXLib::ErrorTrap::sCurrent;

struct RequestCodeLookup_t
	{
	int fCodeAsInt;
	const char* fCodeAsText;
	};

#define XX(a) { a, #a }

RequestCodeLookup_t sRequestCodeLookup [] =
	{
	XX(X_CreateWindow),
	XX(X_ChangeWindowAttributes),
	XX(X_GetWindowAttributes),
	XX(X_DestroyWindow),
	XX(X_DestroySubwindows),
	XX(X_ChangeSaveSet),
	XX(X_ReparentWindow),
	XX(X_MapWindow),
	XX(X_MapSubwindows),
	XX(X_UnmapWindow),
	XX(X_UnmapSubwindows),
	XX(X_ConfigureWindow),
	XX(X_CirculateWindow),
	XX(X_GetGeometry),
	XX(X_QueryTree),
	XX(X_InternAtom),
	XX(X_GetAtomName),
	XX(X_ChangeProperty),
	XX(X_DeleteProperty),
	XX(X_GetProperty),
	XX(X_ListProperties),
	XX(X_SetSelectionOwner),
	XX(X_GetSelectionOwner),
	XX(X_ConvertSelection),
	XX(X_SendEvent),
	XX(X_GrabPointer),
	XX(X_UngrabPointer),
	XX(X_GrabButton),
	XX(X_UngrabButton),
	XX(X_ChangeActivePointerGrab),
	XX(X_GrabKeyboard),
	XX(X_UngrabKeyboard),
	XX(X_GrabKey),
	XX(X_UngrabKey),
	XX(X_AllowEvents),
	XX(X_GrabServer),
	XX(X_UngrabServer),
	XX(X_QueryPointer),
	XX(X_GetMotionEvents),
	XX(X_TranslateCoords),
	XX(X_WarpPointer),
	XX(X_SetInputFocus),
	XX(X_GetInputFocus),
	XX(X_QueryKeymap),
	XX(X_OpenFont),
	XX(X_CloseFont),
	XX(X_QueryFont),
	XX(X_QueryTextExtents),
	XX(X_ListFonts),
	XX(X_ListFontsWithInfo),
	XX(X_SetFontPath),
	XX(X_GetFontPath),
	XX(X_CreatePixmap),
	XX(X_FreePixmap),
	XX(X_CreateGC),
	XX(X_ChangeGC),
	XX(X_CopyGC),
	XX(X_SetDashes),
	XX(X_SetClipRectangles),
	XX(X_FreeGC),
	XX(X_ClearArea),
	XX(X_CopyArea),
	XX(X_CopyPlane),
	XX(X_PolyPoint),
	XX(X_PolyLine),
	XX(X_PolySegment),
	XX(X_PolyRectangle),
	XX(X_PolyArc),
	XX(X_FillPoly),
	XX(X_PolyFillRectangle),
	XX(X_PolyFillArc),
	XX(X_PutImage),
	XX(X_GetImage),
	XX(X_PolyText8),
	XX(X_PolyText16),
	XX(X_ImageText8),
	XX(X_ImageText16),
	XX(X_CreateColormap),
	XX(X_FreeColormap),
	XX(X_CopyColormapAndFree),
	XX(X_InstallColormap),
	XX(X_UninstallColormap),
	XX(X_ListInstalledColormaps),
	XX(X_AllocColor),
	XX(X_AllocNamedColor),
	XX(X_AllocColorCells),
	XX(X_AllocColorPlanes),
	XX(X_FreeColors),
	XX(X_StoreColors),
	XX(X_StoreNamedColor),
	XX(X_QueryColors),
	XX(X_LookupColor),
	XX(X_CreateCursor),
	XX(X_CreateGlyphCursor),
	XX(X_FreeCursor),
	XX(X_RecolorCursor),
	XX(X_QueryBestSize),
	XX(X_QueryExtension),
	XX(X_ListExtensions),
	XX(X_ChangeKeyboardMapping),
	XX(X_GetKeyboardMapping),
	XX(X_ChangeKeyboardControl),
	XX(X_GetKeyboardControl),
	XX(X_Bell),
	XX(X_ChangePointerControl),
	XX(X_GetPointerControl),
	XX(X_SetScreenSaver),
	XX(X_GetScreenSaver),
	XX(X_ChangeHosts),
	XX(X_ListHosts),
	XX(X_SetAccessControl),
	XX(X_SetCloseDownMode),
	XX(X_KillClient),
	XX(X_RotateProperties),
	XX(X_ForceScreenSaver),
	XX(X_SetPointerMapping),
	XX(X_GetPointerMapping),
	XX(X_SetModifierMapping),
	XX(X_GetModifierMapping),
	XX(X_NoOperation)
	};

#undef XX

ZXLib::ErrorTrap::ErrorTrap()
	{
	ZAssertStop(0, (sMutex.IsLocked()));
	fErrorCode = 0;
	fPrev = sCurrent;
	sCurrent = this;
	}

ZXLib::ErrorTrap::~ErrorTrap()
	{
	ZAssertStop(0, (sMutex.IsLocked()));
	if (sCurrent == this)
		sCurrent = fPrev;
	}

int ZXLib::ErrorTrap::GetError()
	{
	return fErrorCode;
	}

void ZXLib::ErrorTrap::Release()
	{
	ZAssertStop(0, (sMutex.IsLocked()));
	ZAssertStop(0, (sCurrent == this));
	sCurrent = fPrev;
	fPrev = nullptr;
	}

int ZXLib::ErrorTrap::sErrorHandler(Display* inDisplay, XErrorEvent* inErrorEvent)
	{
	if (sCurrent)
		{
		sCurrent->fErrorCode = inErrorEvent->error_code;
		}
	else
		{
		char errorMessage[256];
		ZXLib::GetErrorText(inDisplay, inErrorEvent->error_code, errorMessage, 256);
		const char* requestCodeText = "";
		for (size_t x = 0; x < countof(sRequestCodeLookup); ++x)
			{
			if (sRequestCodeLookup[x].fCodeAsInt == inErrorEvent->request_code)
				{
				requestCodeText = sRequestCodeLookup[x].fCodeAsText;
				break;
				}
			}
		ZDebugLogf(0, ("X Error, error_code: %d, %s\n"
						"\tserial: %d\n"
						"\trequest_code: %d, (%s)\n"
						"\tminor_code: %d",
							inErrorEvent->error_code,
							errorMessage,
							inErrorEvent->serial,
							inErrorEvent->request_code,
							requestCodeText,
							inErrorEvent->error_code,
							inErrorEvent->minor_code));
		ZDebugStopf(0, ("Error was not expected"));
		}
	return 0; // Result is ignored.
	}

class ZXLib::ErrorTrap::Init
	{
public:
	Init();
	~Init();
	};

ZXLib::ErrorTrap::Init::Init()
	{
	::XSetErrorHandler(ZXLib::ErrorTrap::sErrorHandler);
	}

ZXLib::ErrorTrap::Init::~Init()
	{}

ZXLib::ErrorTrap::Init ZXLib::ErrorTrap::sInit;

// =================================================================================================
// MARK: - ZXLib

Display* ZXLib::OpenDisplay(const char* a0)
	{
	ZMutexLocker locker(sMutex);
	return ::XOpenDisplay(a0);
	}

int ZXLib::CloseDisplay(Display* a0)
	{
	ZMutexLocker locker(sMutex);
	return ::XCloseDisplay(a0);
	}

XErrorHandler ZXLib::SetErrorHandler(XErrorHandler a0)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetErrorHandler(a0);
	}

int ZXLib::GetErrorText(Display* a0, int a1, char* a2, int a3)
	{
	ZMutexLocker locker(sMutex);
	return ::XGetErrorText(a0, a1, a2, a3);
	}

int ZXLib::Pending(Display* a0)
	{
	ZMutexLocker locker(sMutex);
	return ::XPending(a0);
	}

int ZXLib::NextEvent(Display* a0, XEvent* a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XNextEvent(a0, a1);
	}

Status ZXLib::SendEvent(Display* a0, Window a1, Bool a2, long a3, XEvent* a4)
	{
	ZMutexLocker locker(sMutex);
	return ::XSendEvent(a0, a1, a2, a3, a4);
	}

XContext ZXLib::UniqueContext()
	{
	ZMutexLocker locker(sMutex);
	return XUniqueContext();
	}

int ZXLib::DeleteContext(Display* a0, XID a1, XContext a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XDeleteContext(a0, a1, a2);
	}

int ZXLib::SaveContext(Display* a0, XID a1, XContext a2, const char* a3)
	{
	ZMutexLocker locker(sMutex);
	return ::XSaveContext(a0, a1, a2, a3);
	}

int ZXLib::FindContext(Display* a0, XID a1, XContext a2, XPointer* a3)
	{
	ZMutexLocker locker(sMutex);
	return ::XFindContext(a0, a1, a2, a3);
	}

Window ZXLib::CreateSimpleWindow(Display* a0, Window a1, int a2, int a3, unsigned int a4,
	unsigned int a5, unsigned int a6, unsigned long a7, unsigned long a8)
	{
	ZMutexLocker locker(sMutex);
	return ::XCreateSimpleWindow(a0, a1, a2, a3, a4, a5, a6, a7, a8);
	}

Window ZXLib::CreateWindow(Display* a0, Window a1, int a2, int a3, unsigned int a4,
	unsigned int a5, unsigned int a6, int a7, unsigned int a8, Visual* a9,
		unsigned long a10, XSetWindowAttributes* a11)
	{
	ZMutexLocker locker(sMutex);
	return ::XCreateWindow(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
	}

int ZXLib::DestroyWindow(Display* a0, Window a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XDestroyWindow(a0, a1);
	}

int ZXLib::MapWindow(Display* a0, Window a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XMapWindow(a0, a1);
	}

int ZXLib::UnmapWindow(Display* a0, Window a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XUnmapWindow(a0, a1);
	}

int ZXLib::MoveWindow(Display* a0, Window a1, int a2, int a3)
	{
	ZMutexLocker locker(sMutex);
	
	int result = ::XMoveWindow(a0, a1, a2, a3);
	return result;
	}

int ZXLib::ResizeWindow(Display* a0, Window a1, int a2, int a3)
	{
	ZMutexLocker locker(sMutex);

	// MDC If the creation attributes passed to ZApp::CreateOSWindow had
	// fResizable set to false, attempting to programmatically change the
	// size with XResizeWindow will fail with a result of BadRequest.
	// So before attempting the resize, we check to see if the window
	// is non-resizable, and if so, set it to a new non-resizable size
	// with XSetWMNormalHints.
	//
	// X11 has a different behavior than Mac or Windows, which allow one
	// to programmatically resize a window even if the user can't.

	XSizeHints *sizeHints = ::XAllocSizeHints();

	// We could return BadAlloc if sizeHints is nullptr, but in most cases we
	// would still be able to resize the window, so we don't.  If the window is
	// not resizable, XResizeWindow will return BadRequest, so there will still be
	// an error result.
	
	if ( sizeHints != nullptr ){
	
		long flags;
		
		Status stat = ::XGetWMNormalHints( a0, a1, sizeHints, &flags );
		
		if ( stat ){
		
			if ( ( flags & PMaxSize ) && ( flags & PMinSize ) ){
				if ( ( sizeHints->min_width == sizeHints->max_width )
						&& ( sizeHints->min_height == sizeHints->max_height ) ){
					
					// The window is not resizable.  Resize it to a new, non-resizable
					// size by setting both min and max sizes to the requested size
					
					sizeHints->flags = PMaxSize | PMinSize | PSize;
					
					sizeHints->min_width = a2;
					sizeHints->max_width = a2;
					sizeHints->width = a2;
					
					sizeHints->min_height = a3;
					sizeHints->max_height = a3;
					sizeHints->height = a3;
					
					::XSetWMNormalHints( a0, a1, sizeHints );
					
					::XFree( sizeHints );
					
					return Success;
				}
			}
		
		}
		::XFree( sizeHints );
	}
	
	int result = ::XResizeWindow(a0, a1, a2, a3);
	
	ZAssertStop( 2, ( result == Success ) || ( sizeHints == nullptr ) );

	return result;
	}

int ZXLib::MoveResizeWindow(Display* a0, Window a1, int a2, int a3, int a4, int a5)
	{
	ZMutexLocker locker(sMutex);
	return ::XMoveResizeWindow(a0, a1, a2, a3, a4, a5);
	}

void ZXLib::StoreName(Display* inDisplay, Window inWindow, const char* inWindowName)
	{
	ZMutexLocker locker(sMutex);
	::XStoreName(inDisplay, inWindow, const_cast<char*>(inWindowName));
	}

void ZXLib::SetWMName(Display* a0, Window a1, const XTextProperty* a2)
	{
	ZMutexLocker locker(sMutex);
	::XSetWMName(a0, a1, const_cast<XTextProperty*>(a2));
	}

void ZXLib::SetWMIconName(Display* a0, Window a1, const XTextProperty* a2)
	{
	ZMutexLocker locker(sMutex);
	::XSetWMIconName(a0, a1, const_cast<XTextProperty*>(a2));
	}

Atom ZXLib::InternAtom(Display* a0, _Xconst char* a1, Bool a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XInternAtom(a0, a1, a2);
	}

char* ZXLib::GetAtomName(Display* a0, Atom a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XGetAtomName(a0, a1);
	}

void ZXLib::SetWMProtocols(Display* a0, Window a1, Atom* a2, int a3)
	{
	ZMutexLocker locker(sMutex);
	::XSetWMProtocols(a0, a1, a2, a3);
	}

XFontStruct* ZXLib::LoadQueryFont(Display* a0, _Xconst char* a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XLoadQueryFont(a0, a1);
	}

ZooLib_X11_Cursor ZXLib::CreateFontCursor(Display* a0, unsigned int a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XCreateFontCursor(a0, a1);
	}

ZooLib_X11_Cursor ZXLib::CreatePixmapCursor(Display* a0, Pixmap a1, Pixmap a2, XColor* a3,
	XColor* a4, unsigned int a5, unsigned int a6)
	{
	ZMutexLocker locker(sMutex);
	return ::XCreatePixmapCursor(a0, a1, a2, a3, a4, a5, a6);
	}

int ZXLib::FreeCursor(Display* a0, ZooLib_X11_Cursor a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XFreeCursor(a0, a1);
	}

int ZXLib::DefineCursor(Display* a0, Window a1, ZooLib_X11_Cursor a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XDefineCursor(a0, a1, a2);
	}

int ZXLib::GrabPointer(Display* a0, Window a1, Bool a2, unsigned int a3, int a4, int a5,
	Window a6, ZooLib_X11_Cursor a7, Time a8)
	{
	ZMutexLocker locker(sMutex);
	return ::XGrabPointer(a0, a1, a2, a3, a4, a5, a6, a7, a8);
	}

int ZXLib::UngrabPointer(Display* a0, Time a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XUngrabPointer(a0, a1);
	}

int ZXLib::GrabKeyboard(Display* a0, Window a1, Bool a2, int a3, int a4, Time a5)
	{
	ZMutexLocker locker(sMutex);
	return ::XGrabKeyboard(a0, a1, a2, a3, a4, a5);
	}

int ZXLib::UngrabKeyboard(Display* a0, Time a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XUngrabKeyboard(a0, a1);
	}

GC ZXLib::CreateGC(Display* a0, Drawable a1, unsigned long a2, XGCValues* a3)
	{
	ZMutexLocker locker(sMutex);
	return ::XCreateGC(a0, a1, a2, a3);
	}

int ZXLib::FreeGC(Display* a0, GC a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XFreeGC(a0, a1);
	}

int ZXLib::SetForeground(Display* a0, GC a1, unsigned long a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetForeground(a0, a1, a2);
	}

int ZXLib::SetBackground(Display* a0, GC a1, unsigned long a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetBackground(a0, a1, a2);
	}

int ZXLib::SetRegion(Display* a0, GC a1, Region a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetRegion(a0, a1, a2);
	}

int ZXLib::SetFunction(Display* a0, GC a1, int a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetFunction(a0, a1, a2);
	}

int ZXLib::SetTSOrigin(Display* a0, GC a1, int a2, int a3)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetTSOrigin(a0, a1, a2, a3);
	}

int ZXLib::SetLineAttributes(Display* a0, GC a1, unsigned int a2, int a3, int a4, int a5)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetLineAttributes(a0, a1, a2, a3, a4, a5);
	}

int ZXLib::SetFillStyle(Display* a0, GC a1, int a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetFillStyle(a0, a1, a2);
	}

int ZXLib::CopyArea(Display* a0, Drawable a1, Drawable a2, GC a3, int a4, int a5,
	unsigned int a6, unsigned int a7, int a8, int a9)
	{
	ZMutexLocker locker(sMutex);
	return ::XCopyArea(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	}

int ZXLib::DrawPoint(Display* a0, Drawable a1, GC a2, int a3, int a4)
	{
	ZMutexLocker locker(sMutex);
	return ::XDrawPoint(a0, a1, a2, a3, a4);
	}

int ZXLib::DrawLine(Display* a0, Drawable a1, GC a2, int a3, int a4, int a5, int a6)
	{
	ZMutexLocker locker(sMutex);
	return ::XDrawLine(a0, a1, a2, a3, a4, a5, a6);
	}

int ZXLib::FillRectangle(Display* a0, Drawable a1, GC a2, int a3, int a4,
	unsigned int a5, unsigned int a6)
	{
	ZMutexLocker locker(sMutex);
	return ::XFillRectangle(a0, a1, a2, a3, a4, a5, a6);
	}

int ZXLib::DrawRectangle(Display* a0, Drawable a1, GC a2, int a3, int a4,
	unsigned int a5, unsigned int a6)
	{
	ZMutexLocker locker(sMutex);
	return ::XDrawRectangle(a0, a1, a2, a3, a4, a5, a6);
	}

int ZXLib::FillPolygon(Display* a0, Drawable a1, GC a2, XPoint* a3, int a4, int a5, int a6)
	{
	ZMutexLocker locker(sMutex);
	return ::XFillPolygon(a0, a1, a2, a3, a4, a5, a6);
	}

int ZXLib::DrawArc(Display* a0, Drawable a1, GC a2, int a3, int a4, unsigned int a5,
	unsigned int a6, int a7, int a8)
	{
	ZMutexLocker locker(sMutex);
	return ::XDrawArc(a0, a1, a2, a3, a4, a5, a6, a7, a8);
	}

int ZXLib::FillArc(Display* a0, Drawable a1, GC a2, int a3, int a4, unsigned int a5,
	unsigned int a6, int a7, int a8)
	{
	ZMutexLocker locker(sMutex);
	return ::XFillArc(a0, a1, a2, a3, a4, a5, a6, a7, a8);
	}

int ZXLib::Sync(Display* a0, Bool a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XSync(a0, a1);
	}

int ZXLib::Flush(Display* a0)
	{
	ZMutexLocker locker(sMutex);
	return ::XFlush(a0);
	}

int ZXLib::SetStipple(Display* a0, GC a1, Pixmap a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetStipple(a0, a1, a2);
	}

int ZXLib::SetTile(Display* a0, GC a1, Pixmap a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetTile(a0, a1, a2);
	}

int ZXLib::SetFont(Display* a0, GC a1, Font a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XSetFont(a0, a1, a2);
	}

int ZXLib::DrawString(Display* a0, Drawable a1, GC a2, int a3, int a4, _Xconst char* a5, int a6)
	{
	ZMutexLocker locker(sMutex);
	return ::XDrawString(a0, a1, a2, a3, a4, a5, a6);
	}

int ZXLib::TextWidth(XFontStruct* a0, _Xconst char* a1, int a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XTextWidth(a0, a1, a2);
	}

int ZXLib::SelectInput(Display* a0, Window a1, long a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XSelectInput(a0, a1, a2);
	}

Pixmap ZXLib::CreateBitmapFromData(Display* a0, Drawable a1, _Xconst char* a2,
	unsigned int a3, unsigned int a4)
	{
	ZMutexLocker locker(sMutex);
	return ::XCreateBitmapFromData(a0, a1, a2, a3, a4);
	}

Pixmap ZXLib::CreatePixmap(Display* a0, Drawable a1, unsigned int a2, unsigned int a3, int a4)
	{
	ZMutexLocker locker(sMutex);
	return ::XCreatePixmap(a0, a1, a2, a3, a4);
	}

int ZXLib::FreePixmap(Display* a0, Pixmap a1)
	{
	ZMutexLocker locker(sMutex);
	return ::XFreePixmap(a0, a1);
	}

Status ZXLib::AllocColor(Display* a0, Colormap a1, XColor* a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XAllocColor(a0, a1, a2);
	}

int ZXLib::Free(void* a0)
	{
	ZMutexLocker locker(sMutex);
	return ::XFree(a0);
	}

XSizeHints* ZXLib::AllocSizeHints()
	{
	ZMutexLocker locker(sMutex);
	return ::XAllocSizeHints();
	}

XWMHints* ZXLib::AllocWMHints()
	{
	ZMutexLocker locker(sMutex);
	return ::XAllocWMHints();
	}

XClassHint* ZXLib::AllocClassHint()
	{
	ZMutexLocker locker(sMutex);
	return ::XAllocClassHint();
	}

Status ZXLib::StringListToTextProperty(const char** a0, int a1, XTextProperty* a2)
	{
	ZMutexLocker locker(sMutex);
	return ::XStringListToTextProperty(const_cast<char**>(a0), a1, a2);
	}

void ZXLib::SetWMProperties(Display* a0, Window a1, XTextProperty* a2,
	XTextProperty* a3, char** a4, int a5, XSizeHints* a6, XWMHints* a7, XClassHint* a8)
	{
	ZMutexLocker locker(sMutex);
	::XSetWMProperties(a0, a1, a2, a3, a4, a5, a6, a7, a8);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(X11)

// =================================================================================================
