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

#ifndef __ZXLib_h__
#define __ZXLib_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

// =================================================================================================
#if ZCONFIG_SPI_Enabled(X11)

#include "zoolib/ZThreadOld.h"

#include "zoolib/ZCompat_Xlib.h"
#include <X11/Xresource.h>
#include <X11/Xutil.h>

namespace ZooLib {

namespace ZXLib {

extern ZMutex sMutex;

class ErrorTrap
	{
private:
	ErrorTrap(const ErrorTrap&); // Not implemented
	ErrorTrap& operator=(const ErrorTrap&); // Not implemented

public:
	ErrorTrap();
	~ErrorTrap();

	int GetError();

	void Release();

protected:
	class Init;
	friend class Init;
	static Init sInit;

	static int sErrorHandler(Display* inDisplay, XErrorEvent* inErrorEvent);
	static ErrorTrap* sCurrent;

	ErrorTrap* fPrev;
	int fErrorCode;
	};

// ==================================================

Display* OpenDisplay(const char*);
int CloseDisplay(Display*);

XErrorHandler SetErrorHandler(XErrorHandler a0);
int GetErrorText(Display*, int, char*, int);

int Pending(Display*);
int NextEvent(Display*, XEvent*);

Status SendEvent(Display* a0, Window a1, Bool a2, long a3, XEvent* a4);

XContext UniqueContext();
int DeleteContext(Display* a0, XID a1, XContext a2);
int SaveContext(Display*, XID, XContext, const char*);
int FindContext(Display*, XID, XContext, XPointer*);

Window CreateSimpleWindow(Display* a0, Window a1, int a2, int a3, unsigned int a4,
	unsigned int a5, unsigned int a6, unsigned long a7, unsigned long a8);

Window CreateWindow(Display* a0, Window a1, int a2, int a3, unsigned int a4,
	unsigned int a5, unsigned int a6, int a7, unsigned int a8, Visual* a9,
	unsigned long a10, XSetWindowAttributes* a11);

int DestroyWindow(Display*, Window);

int MapWindow(Display*, Window);
int UnmapWindow(Display* a0, Window a1);

int MoveWindow(Display*, Window, int, int);
int ResizeWindow(Display*, Window, int, int);
int MoveResizeWindow(Display*, Window, int, int, int, int);


void StoreName(Display* a0, Window a1, const char* a2);
void SetWMName(Display* a0, Window a1, const XTextProperty* a2);
void SetWMIconName(Display* a0, Window a1, const XTextProperty* a2);

Atom InternAtom(Display* a0, _Xconst char* a1, Bool a2);
char* GetAtomName(Display* a0, Atom a1);

void SetWMProtocols(Display* a0, Window a1, Atom* a2, int a3);

XFontStruct* LoadQueryFont(Display* a0, _Xconst char* a1);

ZooLib_X11_Cursor CreateFontCursor(Display* a0, unsigned int a1);
ZooLib_X11_Cursor CreatePixmapCursor(Display* a0, Pixmap a1, Pixmap a2, XColor* a3,
	XColor* a4, unsigned int a5, unsigned int a6);
int FreeCursor(Display* a0, ZooLib_X11_Cursor a1);

int DefineCursor(Display* a0, Window a1, ZooLib_X11_Cursor a2);

int GrabPointer(Display* a0, Window a1, Bool a2, unsigned int a3, int a4, int a5,
	Window a6, ZooLib_X11_Cursor a7, Time a8);
int UngrabPointer(Display* a0, Time a1);
int GrabKeyboard(Display* a0, Window a1, Bool a2, int a3, int a4, Time a5);
int UngrabKeyboard(Display* a0, Time a1);

GC CreateGC(Display* a0, Drawable a1, unsigned long a2, XGCValues* a3);
int FreeGC(Display* a0, GC a1);

int SetForeground(Display* a0, GC a1, unsigned long a2);
int SetBackground(Display* a0, GC a1, unsigned long a2);
int SetRegion(Display* a0, GC a1, Region a2);
int SetFunction(Display* a0, GC a1, int a2);
int SetTSOrigin(Display* a0, GC a1, int a2, int a3);
int SetLineAttributes(Display* a0, GC a1, unsigned int a2, int a3, int a4, int a5);
int SetFillStyle(Display* a0, GC a1, int a2);
int CopyArea(Display* a0, Drawable a1, Drawable a2, GC a3, int a4, int a5,
	unsigned int a6, unsigned int a7, int a8, int a9);
int DrawPoint(Display* a0, Drawable a1, GC a2, int a3, int a4);
int DrawLine(Display* a0, Drawable a1, GC a2, int a3, int a4, int a5, int a6);
int FillRectangle(Display* a0, Drawable a1, GC a2, int a3, int a4,
	unsigned int a5, unsigned int a6);
int DrawRectangle(Display* a0, Drawable a1, GC a2, int a3, int a4,
	unsigned int a5, unsigned int a6);
int FillPolygon(Display* a0, Drawable a1, GC a2, XPoint* a3, int a4, int a5, int a6);
int DrawArc(Display* a0, Drawable a1, GC a2, int a3, int a4, unsigned int a5,
	unsigned int a6, int a7, int a8);
int FillArc(Display* a0, Drawable a1, GC a2, int a3, int a4, unsigned int a5,
	unsigned int a6, int a7, int a8);
int Sync(Display* a0, Bool a1);
int Flush(Display* a0);
int SetStipple(Display* a0, GC a1, Pixmap a2);
int SetTile(Display* a0, GC a1, Pixmap a2);
int SetFont(Display* a0, GC a1, Font a2);
int DrawString(Display* a0, Drawable a1, GC a2, int a3, int a4, _Xconst char* a5, int a6);
int TextWidth(XFontStruct* a0, _Xconst char* a1, int a2);
int SelectInput(Display* a0, Window a1, long a2);
Pixmap CreateBitmapFromData(Display* a0, Drawable a1, _Xconst char* a2,
	unsigned int a3, unsigned int a4);
Pixmap CreatePixmap(Display* a0, Drawable a1, unsigned int a2, unsigned int a3, int a4);
int FreePixmap(Display* a0, Pixmap a1);
Status AllocColor(Display* a0, Colormap a1, XColor* a2);

int Free(void* a0);
XSizeHints* AllocSizeHints();
XWMHints* AllocWMHints();
XClassHint* AllocClassHint();
Status StringListToTextProperty(const char** a0, int a1, XTextProperty* a2);
void SetWMProperties(Display* a0, Window a1, XTextProperty* a2, XTextProperty* a3,
	char** a4, int a5, XSizeHints* a6, XWMHints* a7, XClassHint* a8);

} // namespace ZXLib

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(X11)

// =================================================================================================

#endif // __ZXLib_h__
