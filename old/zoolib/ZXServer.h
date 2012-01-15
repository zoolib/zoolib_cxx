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

#ifndef __ZXServer_h__
#define __ZXServer_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(X11)

#include "zoolib/ZCounted.h"
#include "zoolib/ZDCFont.h"
#include "zoolib/ZDCPixmapNS.h"
#include "zoolib/ZDCRgn.h"
#include "zoolib/ZGeom.h"
#include "zoolib/ZRGBColor.h"
#include "zoolib/ZThreadOld.h"

#include "zoolib/ZXLib.h"

#include <map> // for fCachedColors

#include <X11/extensions/XShm.h>

namespace ZooLib {

class ZDCPixmap;
class ZXWindow;

// =================================================================================================
// MARK: - ZXServer

class ZXServer : public ZCounted
	{
public:
	ZXServer(const char* inDisplayName);
	virtual ~ZXServer();

	void Close();

	bool Pending();
	void NextEvent(XEvent& outEvent);
	void DispatchXEvent(const XEvent& inEvent);
	Display* GetDisplay();

	Status SendEvent(Window theWindow, Bool propagate, long event_mask, const XEvent& inEvent);

	bool CheckWindowEvent(Window inWindow, long event_mask, XEvent& outEvent);

	void DestroyWindow(Window inWindow);
	void MapWindow(Window inWindow);
	void UnmapWindow(Window inWindow);
	void StoreName(Window inWindow, const char* inWindowName);
	void SetWMName(Window inWindow, const XTextProperty* inWindowName);
	void SetWMIconName(Window inWindow, const XTextProperty* inWindowName);
	Atom InternAtom(const char* inAtomName);
	std::string GetAtomName(Atom inAtom);
	void SetWMProtocols(Window inWindow, const Atom* inAtoms, int inCount);

	void MoveWindow(Window inWindow, int inLocationH, int inLocationV);
	void ResizeWindow(Window inWindow, unsigned int inSizeH, unsigned int inSizeV);
	void MoveResizeWindow(Window inWindow, int inLocationH, int inLocationV,
		unsigned int inSizeH, unsigned int inSizeV);
	ZooLib_X11_Cursor CreateFontCursor(int inCursorShape);
	ZooLib_X11_Cursor CreateCursorFromDCPixmaps(Drawable inDrawable,
		const ZDCPixmap& inPixmap, const ZDCPixmap& inMask, ZPoint inHotPoint);
	void FreeCursor(ZooLib_X11_Cursor inCursor);
	void DefineCursor(Window inWindow, ZooLib_X11_Cursor inCursor);
	int GrabPointer(Window inWindow, Bool inOwnerEvents,
		unsigned int inEventMask, int inPointerMode, int inKeyboardMode,
		Window inConfineTo, ZooLib_X11_Cursor inCursor, Time inTime);
	int UngrabPointer(Time inTime);
	int GrabKeyboard(Window inWindow, Bool inOwnerEvents,
		int inPointerMode, int inKeyboardMode, Time inTime);
	int UngrabKeyboard(Time inTime);
	void SetForeground(GC inGC, const ZRGBColor& inColor);
	void SetForeground(GC inGC, uint32 inPixVal);
	void SetBackground(GC inGC, const ZRGBColor& inColor);
	void SetBackground(GC inGC, uint32 inPixVal);
	void SetRegion(GC inGC, Region inRegion);
	void SetClipMask(GC inGC, Pixmap inMask);
	void SetClipOrigin(GC inGC, int inOriginH, int inOriginV);
	void SetFunction(GC inGC, int inFunction);
	void SetTSOrigin(GC inGC, int inOriginH, int inOriginV);
	void SetLineAttributes(GC inGC, unsigned int inLineWidth, int inLineStyle,
		int inCapStyle, int inJoinStyle);
	void SetFillStyle(GC inGC, int inFillStyle);
	void CopyArea(Drawable inSourceDrawable, Drawable inDestDrawable, GC inGC,
		int inSourceLocationH, int inSourceLocationV,
		unsigned int inSizeH, unsigned int inSizeV, int inDestLocationH, int inDestLocationV);
	void DrawPoint(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV);
	void DrawLine(Drawable inDrawable, GC inGC, int inStartH, int inStartV, int inEndH, int EndV);
	void FillRectangle(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV,
		unsigned int inSizeH, unsigned int inSizeV);
	void DrawRectangle(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV,
		unsigned int inSizeH, unsigned int inSizeV);
	void FillPolygon(Drawable inDrawable, GC inGC, XPoint* inPoints, size_t inCount,
		int inShape, int inMode);
	void DrawArc(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV,
		unsigned int inSizeH, unsigned int inSizeV, int inAngle1, int inAngle2);
	void FillArc(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV,
		unsigned int inSizeH, unsigned int inSizeV, int inAngle1, int inAngle2);
	void Sync(bool inDiscard);
	void Flush();
	void SetStipple(GC inGC, Pixmap inStipple);
	void SetTile(GC inGC, Pixmap inStipple);
	void SetFont(GC inGC, const ZDCFont& inFont);
	void DrawString(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV,
		const char* inText, size_t inTextLength);
	int TextWidth(const ZDCFont& inFont, const char* inText, size_t inTextLength);
	void GetFontInfo(const ZDCFont& inFont, short& outAscent, short& outDescent, short& outLeading);
	Window CreateWindow(Window inParent, int inLocationH, int inLocationV,
		unsigned int inSizeH, unsigned int inSizeV,unsigned int inBorderWidth,
		int inDepth, unsigned int inClass, Visual* inVisual, unsigned long inValueMask,
		XSetWindowAttributes* inAttributes);
	Window CreateSimpleWindow(Window inParent, int inLocationH, int inLocationV,
		unsigned int inSizeH, unsigned int inSizeV, unsigned int inBorderWidth,
			unsigned long inBorderPixel, unsigned long inBackgroundPixel);
	void SelectInput(Window inWindow, long inEventMask);
	Pixmap CreateBitmapFromData(Drawable inDrawable, char* inData, int inSizeH, int inSizeV);
	Pixmap CreatePixmap(Drawable inDrawable,
		unsigned int inSizeH, unsigned int inSizeV, unsigned int inDepth);
	Pixmap CreatePixmapFromDCPixmap(Drawable inDrawable, const ZDCPixmap& inDCPixmap);
	Pixmap CreateBitmapFromDCPixmap(Drawable inDrawable, const ZDCPixmap& inDCPixmap,
		const ZRect& inSourceBounds, bool inInvert);
	void FreePixmap(Pixmap inPixmap);

	void DrawDCPixmap(Drawable inDrawable, GC inGC, ZPoint inLocation,
		const ZDCPixmap& inSourcePixmap, const ZRect& inSourceBounds);

	GC CreateGC(Drawable inDrawable, unsigned long inValueMask, XGCValues* inValues);
	void FreeGC(GC inGC);

	void SetWMProperties(Window a1, XTextProperty* a2, XTextProperty* a3,
		char** a4, int a5, XSizeHints* a6, XWMHints* a7, XClassHint* a8);

protected:
	size_t pGetNextImage();
	size_t pSetupImage(int32 inWidth, int32 inHeight, int32& outLocationH, int32& outLocationV);
	unsigned long pLookupColor(const ZRGBColor& inColor);

	void InitGraphics();

	std::map<ZRGBColor, unsigned long, std::less<ZRGBColor> > fCachedColors;

	ZMutex fMutex_Structure;

	Display* fDisplay;
	XFontStruct* fFont;

	XVisualInfo fVisualInfo;
	Colormap fColormap;

	bool fHasSharedImages;
	bool fHasSharedPixmaps;
	int fShmCompletionEvent;
	int fImageWidth;
	int fImageHeight;
	size_t fCurrentImageIndex;
	size_t fHorizIndex;
	int32 fHorizV;
	size_t fVertIndex;
	int32 fVertH;
	size_t fTileIndex;
	int32 fTileH;
	int32 fTileV1;
	int32 fTileV2;
	std::vector<std::pair<XImage*, XShmSegmentInfo*> > fImages;

	ZDCPixmapNS::PixelDesc fPixelDesc;
	};

// =================================================================================================
// MARK: - ZXWindow

class ZXWindow
	{
protected:
	ZXWindow();
	~ZXWindow();

public:
	Window GetWindow();
	ZRef<ZXServer> GetXServer();

	virtual void HandleXEvent(const XEvent& inEvent);

	void AttachToWindow(ZRef<ZXServer> inXServer, Window inWindow);
	void DetachFromWindow();

	static ZXWindow* sFromDisplayAndWindow(Display* inDisplay, Window inWindow);
	static ZXWindow* sFromDisplayAndWindowNilOkay(Display* inDisplay, Window inWindow);

protected:
	Window fWindow;
	ZRef<ZXServer> fXServer;
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(X11)

#endif // __ZXServer_h__
