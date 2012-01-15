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

#include "zoolib/ZXServer.h"

#if ZCONFIG_SPI_Enabled(X11)

#include "zoolib/ZDCPixmap.h"
#include "zoolib/ZDebug.h"

#include <sys/ipc.h>
#include <sys/shm.h>

namespace ZooLib {

using std::bad_alloc;
using std::less;
using std::min;
using std::map;
using std::pair;
using std::string;
using std::runtime_error;
using std::vector;

static XContext sXContext_Window = 0;

// =================================================================================================
// MARK: - kDebug

#define kDebug_X 2

#define kDebug_XHeavy 1

#if 0
// =================================================================================================
// MARK: - ZXServerDummyWindow

class ZXServerDummyWindow : public ZXWindow
	{
public:
	ZXServerDummyWindow(ZRef<ZXServer> inXServer, Window inWindow);
	virtual ~ZXServerDummyWindow();

// From ZXWindow
	virtual void HandleXEvent(const XEvent& inEvent);
	};

ZXServerDummyWindow::ZXServerDummyWindow(ZRef<ZXServer> inXServer, Window inWindow)
	{
	this->AttachToWindow(inXServer, inWindow);
	}

ZXServerDummyWindow::~ZXServerDummyWindow()
	{
	fXServer->DestroyWindow(fWindow);
	this->DetachFromWindow();
	}

// From ZXWindow
void ZXServerDummyWindow::HandleXEvent(const XEvent& inEvent)
	{
	}
#endif

// =================================================================================================
// MARK: - ZXServer

ZXServer::ZXServer(const char* inDisplayName)
	{
	fHasSharedImages = false;
	fHasSharedPixmaps = false;

	if (!sXContext_Window)
		sXContext_Window = ZXLib::UniqueContext();

	fDisplay = ZXLib::OpenDisplay(inDisplayName);
	if (fDisplay == nullptr)
		throw runtime_error("Couldn't connect to display");

	fFont = ZXLib::LoadQueryFont(fDisplay, "fixed");

	this->InitGraphics();
	}

ZXServer::~ZXServer()
	{
	ZAssertStop(2, fDisplay == nullptr);
	}

void ZXServer::Close()
	{
	ZXLib::CloseDisplay(fDisplay);
	fDisplay = nullptr;
	}

bool ZXServer::Pending()
	{
	return ZXLib::Pending(fDisplay);
	}

void ZXServer::NextEvent(XEvent& outEvent)
	{
	ZXLib::NextEvent(fDisplay, &outEvent);
	}

static const char* sXEventNames[] =
	{
	"zero",
	"one",
	"KeyPress",
	"KeyRelease",
	"ButtonPress",
	"ButtonRelease",
	"MotionNotify",
	"EnterNotify",
	"LeaveNotify",
	"FocusIn",
	"FocusOut",
	"KeymapNotify",
	"Expose",
	"GraphicsExpose",
	"NoExpose",
	"VisibilityNotify",
	"CreateNotify",
	"DestroyNotify",
	"UnmapNotify",
	"MapNotify",
	"MapRequest",
	"ReparentNotify",
	"ConfigureNotify",
	"ConfigureRequest",
	"GravityNotify",
	"ResizeRequest",
	"CirculateNotify",
	"CirculateRequest",
	"PropertyNotify",
	"SelectionClear	",
	"SelectionRequest",
	"SelectionNotify",
	"ColormapNotify",
	"ClientMessage",
	"MappingNotify"
	};

void ZXServer::DispatchXEvent(const XEvent& inEvent)
	{
	ZAssert(inEvent.xany.display == fDisplay);

	ZXWindow* theXWindow = ZXWindow::sFromDisplayAndWindowNilOkay(fDisplay, inEvent.xany.window);
	if (inEvent.xany.type == PropertyNotify)
		{
		string theAtomName = this->GetAtomName(inEvent.xproperty.atom);
		ZDebugLogf(kDebug_XHeavy, ("type: %d (PropertyNotify), serial: %d, display: %08X, window: %08X, theXWindow: %08X, Atom: %s, Time: %d, state: %d",
					inEvent.xany.type,
					inEvent.xany.serial,
					inEvent.xany.display,
					inEvent.xany.window,
					theXWindow,
					theAtomName.c_str(),
					inEvent.xproperty.time,
					inEvent.xproperty.state));
		}
	else if (inEvent.xany.type >= 0 && inEvent.xany.type < countof(sXEventNames))
		{
		ZDebugLogf(kDebug_XHeavy, ("type: %d (%s), serial: %d, display: %08X, window: %08X, theXWindow: %08X",
					inEvent.xany.type,
					sXEventNames[inEvent.xany.type],
					inEvent.xany.serial,
					inEvent.xany.display,
					inEvent.xany.window,
					theXWindow));
		}
	else if ((fHasSharedImages || fHasSharedPixmaps) && inEvent.xany.type == fShmCompletionEvent)
		{
		const XShmCompletionEvent& theShmEvent = reinterpret_cast<const XShmCompletionEvent&>(inEvent);
		ZDebugLogf(kDebug_XHeavy, ("type: %d (ShmCompletion), serial: %d, send_event: %d, display: %08X, drawable: %08X, major_code: %d, minor_code: %d, shmseg: %08X, offset: %d",
					theShmEvent.type,
					theShmEvent.serial,
					theShmEvent.send_event,
					theShmEvent.display,
					theShmEvent.drawable,
					theShmEvent.major_code,
					theShmEvent.minor_code,
					theShmEvent.shmseg,
					theShmEvent.offset));
		}
	else
		{
		ZDebugLogf(kDebug_XHeavy, ("type: %d (unknown), serial: %d, display: %08X, window: %08X, theXWindow: %08X",
					inEvent.xany.type,
					inEvent.xany.serial,
					inEvent.xany.display,
					inEvent.xany.window,theXWindow));
		}

	if (theXWindow)
		theXWindow->HandleXEvent(inEvent);
	}

Display* ZXServer::GetDisplay()
	{
	return fDisplay;
	}

Status ZXServer::SendEvent(Window theWindow, Bool propagate, long event_mask, const XEvent& outEvent)
	{
	return ZXLib::SendEvent(fDisplay, theWindow, propagate, event_mask, const_cast<XEvent*>(&outEvent));
	}

bool ZXServer::CheckWindowEvent(Window inWindow, long event_mask, XEvent& outEvent)
	{
	ZMutexLocker locker(ZXLib::sMutex);
	return ::XCheckWindowEvent(fDisplay, inWindow, event_mask, &outEvent);
	}

void ZXServer::DestroyWindow(Window inWindow)
	{
	ZMutexLocker structureLocker(fMutex_Structure);
	map<ZRGBColor, unsigned long, less<ZRGBColor> >::iterator theIter = fCachedColors.begin();
	int index = 0;
	for (map<ZRGBColor, unsigned long, less<ZRGBColor> >::iterator theIter = fCachedColors.begin(); theIter != fCachedColors.end(); ++theIter)
		{
		ZDebugPrintf(kDebug_XHeavy, ("index: %d, pixel: %08X, red: %04X, green: %04X, blue: %04X", index++, (*theIter).second, (*theIter).first.red, (*theIter).first.green, (*theIter).first.blue));
		}

	ZXLib::DestroyWindow(fDisplay, inWindow);
	}

void ZXServer::MapWindow(Window inWindow)
	{
	ZXLib::MapWindow(fDisplay, inWindow);
	}

void ZXServer::UnmapWindow(Window inWindow)
	{
	ZXLib::UnmapWindow(fDisplay, inWindow);
	}

void ZXServer::StoreName(Window inWindow, const char* inWindowName)
	{
	ZXLib::StoreName(fDisplay, inWindow, inWindowName);
	}

void ZXServer::SetWMName(Window inWindow, const XTextProperty* inWindowName)
	{
	ZXLib::SetWMName(fDisplay, inWindow, inWindowName);
	}

void ZXServer::SetWMIconName(Window inWindow, const XTextProperty* inWindowName)
	{
	ZXLib::SetWMIconName(fDisplay, inWindow, inWindowName);
	}

Atom ZXServer::InternAtom(const char* inAtomName)
	{
	return ZXLib::InternAtom(fDisplay, const_cast<char*>(inAtomName), false);
	}

string ZXServer::GetAtomName(Atom inAtom)
	{
	string result;
	if (char* theAtomName = ZXLib::GetAtomName(fDisplay, inAtom))
		{
		result = theAtomName;
		XFree(theAtomName);
		}

	return result;
	}

void ZXServer::SetWMProtocols(Window inWindow, const Atom* inAtoms, int inCount)
	{
	ZXLib::SetWMProtocols(fDisplay, inWindow, const_cast<Atom*>(inAtoms), inCount);
	}

void ZXServer::MoveWindow(Window inWindow, int inLocationH, int inLocationV)
	{
	ZXLib::MoveWindow(fDisplay, inWindow, inLocationH, inLocationV);
	}

void ZXServer::ResizeWindow(Window inWindow, unsigned int inSizeH, unsigned int inSizeV)
	{
	ZXLib::ResizeWindow(fDisplay, inWindow, inSizeH, inSizeV);
	}

void ZXServer::MoveResizeWindow(Window inWindow, int inLocationH, int inLocationV, unsigned int inSizeH, unsigned int inSizeV)
	{
	ZXLib::MoveResizeWindow(fDisplay, inWindow, inLocationH, inLocationV, inSizeH, inSizeV);
	}

ZooLib_X11_Cursor ZXServer::CreateFontCursor(int inCursorShape)
	{
	return ZXLib::CreateFontCursor(fDisplay, inCursorShape);
	}

ZooLib_X11_Cursor ZXServer::CreateCursorFromDCPixmaps(Drawable inDrawable, const ZDCPixmap& inPixmap, const ZDCPixmap& inMask, ZPoint inHotPoint)
	{
	Pixmap theCursorPixmap = this->CreateBitmapFromDCPixmap(inDrawable, inPixmap, ZRect(16, 16), true);
	Pixmap theMaskPixmap = this->CreateBitmapFromDCPixmap(inDrawable, inMask, ZRect(16, 16), false);
	XColor theForeColor = ZRGBColor::sBlack;
	XColor theBackColor = ZRGBColor::sWhite;
	ZooLib_X11_Cursor theCursor = ZXLib::CreatePixmapCursor(fDisplay, theCursorPixmap, theMaskPixmap, &theForeColor, &theBackColor, inHotPoint.h, inHotPoint.v);
	ZXLib::FreePixmap(fDisplay, theCursorPixmap);
	ZXLib::FreePixmap(fDisplay, theMaskPixmap);
	return theCursor;
	}

void ZXServer::FreeCursor(ZooLib_X11_Cursor inCursor)
	{
	ZXLib::FreeCursor(fDisplay, inCursor);
	}

void ZXServer::DefineCursor(Window inWindow, ZooLib_X11_Cursor inCursor)
	{
	ZXLib::DefineCursor(fDisplay, inWindow, inCursor);
	}

int ZXServer::GrabPointer(Window inWindow, Bool inOwnerEvents, unsigned int inEventMask, int inPointerMode, int inKeyboardMode, Window inConfineTo, ZooLib_X11_Cursor inCursor, Time inTime)
	{
	return ZXLib::GrabPointer(fDisplay, inWindow, inOwnerEvents, inEventMask, inPointerMode, inKeyboardMode, inConfineTo, inCursor, inTime);
	}

int ZXServer::UngrabPointer(Time inTime)
	{
	return ZXLib::UngrabPointer(fDisplay, inTime);
	}

int ZXServer::GrabKeyboard(Window inWindow, Bool inOwnerEvents, int inPointerMode, int inKeyboardMode, Time inTime)
	{
	return ZXLib::GrabKeyboard(fDisplay, inWindow, inOwnerEvents, inPointerMode, inKeyboardMode, inTime);
	}

int ZXServer::UngrabKeyboard(Time inTime)
	{
	return ZXLib::UngrabKeyboard(fDisplay, inTime);
	}

// ==================================================

void ZXServer::SetForeground(GC inGC, const ZRGBColor& inColor)
	{
	ZXLib::SetForeground(fDisplay, inGC, this->pLookupColor(inColor));
	}

void ZXServer::SetForeground(GC inGC, uint32 inPixVal)
	{
	ZXLib::SetForeground(fDisplay, inGC, inPixVal);
	}

void ZXServer::SetBackground(GC inGC, const ZRGBColor& inColor)
	{
	ZXLib::SetBackground(fDisplay, inGC, this->pLookupColor(inColor));
	}

void ZXServer::SetBackground(GC inGC, uint32 inPixVal)
	{
	ZXLib::SetBackground(fDisplay, inGC, inPixVal);
	}

void ZXServer::SetRegion(GC inGC, Region inRegion)
	{
	ZXLib::SetRegion(fDisplay, inGC, inRegion);
	}

void ZXServer::SetFunction(GC inGC, int inFunction)
	{
	ZXLib::SetFunction(fDisplay, inGC, inFunction);
	}

void ZXServer::SetClipMask(GC inGC, Pixmap inMask)
	{
	ZMutexLocker locker(ZXLib::sMutex);
	::XSetClipMask(fDisplay, inGC, inMask);
	}

void ZXServer::SetClipOrigin(GC inGC, int inOriginH, int inOriginV)
	{
	ZMutexLocker locker(ZXLib::sMutex);
	::XSetClipOrigin(fDisplay, inGC, inOriginH, inOriginV);
	}

void ZXServer::SetTSOrigin(GC inGC, int inOriginH, int inOriginV)
	{
	ZXLib::SetTSOrigin(fDisplay, inGC, inOriginH, inOriginV);
	}

void ZXServer::SetLineAttributes(GC inGC, unsigned int inLineWidth, int inLineStyle, int inCapStyle, int inJoinStyle)
	{
	ZXLib::SetLineAttributes(fDisplay, inGC, inLineWidth, inLineStyle, inCapStyle, inJoinStyle);
	}

void ZXServer::SetFillStyle(GC inGC, int inFillStyle)
	{
	ZXLib::SetFillStyle(fDisplay, inGC, inFillStyle);
	}

void ZXServer::CopyArea(Drawable inSourceDrawable, Drawable inDestDrawable, GC inGC,
			int inSourceLocationH, int inSourceLocationV, unsigned int inSizeH, unsigned int inSizeV, int inDestLocationH, int inDestLocationV)
	{
	ZXLib::CopyArea(fDisplay, inSourceDrawable, inDestDrawable, inGC,
				inSourceLocationH, inSourceLocationV, inSizeH, inSizeV, inDestLocationH, inDestLocationV);
	}

void ZXServer::DrawPoint(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV)
	{
	ZXLib::DrawPoint(fDisplay, inDrawable, inGC, inLocationH, inLocationV);
	}

void ZXServer::DrawLine(Drawable inDrawable, GC inGC, int inStartH, int inStartV, int inEndH, int EndV)
	{
	ZXLib::DrawLine(fDisplay, inDrawable, inGC, inStartH, inStartV, inEndH, EndV);
	}

void ZXServer::FillRectangle(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV, unsigned int inSizeH, unsigned int inSizeV)
	{
	ZXLib::FillRectangle(fDisplay, inDrawable, inGC, inLocationH, inLocationV, inSizeH, inSizeV);
	}

void ZXServer::DrawRectangle(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV, unsigned int inSizeH, unsigned int inSizeV)
	{
	ZXLib::DrawRectangle(fDisplay, inDrawable, inGC, inLocationH, inLocationV, inSizeH, inSizeV);
	}

void ZXServer::FillPolygon(Drawable inDrawable, GC inGC, XPoint* inPoints, size_t inCount, int inShape, int inMode)
	{
	ZXLib::FillPolygon(fDisplay, inDrawable, inGC, inPoints, inCount, inShape, inMode);
	}

void ZXServer::DrawArc(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV, unsigned int inSizeH, unsigned int inSizeV, int inAngle1, int inAngle2)
	{
	ZXLib::DrawArc(fDisplay, inDrawable, inGC, inLocationH, inLocationV, inSizeH, inSizeV, inAngle1, inAngle2);
	}

void ZXServer::FillArc(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV, unsigned int inSizeH, unsigned int inSizeV, int inAngle1, int inAngle2)
	{
	ZXLib::FillArc(fDisplay, inDrawable, inGC, inLocationH, inLocationV, inSizeH, inSizeV, inAngle1, inAngle2);
	}

void ZXServer::Sync(bool inDiscard)
	{
	ZXLib::Sync(fDisplay, inDiscard);
	}

void ZXServer::Flush()
	{
	ZXLib::Flush(fDisplay);
	}

void ZXServer::SetStipple(GC inGC, Pixmap inStipple)
	{
	int result = ZXLib::SetStipple(fDisplay, inGC, inStipple);
	ZAssertStopf(0, result != 0, ("ZXServer::SetStipple result: %d", result));
	}

void ZXServer::SetTile(GC inGC, Pixmap inTile)
	{
	int result = ZXLib::SetTile(fDisplay, inGC, inTile);
	ZAssertStopf(0, result != 0, ("ZXServer::SetTile result: %d", result));
	}

void ZXServer::SetFont(GC inGC, const ZDCFont& inFont)
	{
	ZMutexLocker structureLocker(fMutex_Structure);
	ZXLib::SetFont(fDisplay, inGC, fFont->fid);
	}

void ZXServer::DrawString(Drawable inDrawable, GC inGC, int inLocationH, int inLocationV, const char* inText, size_t inTextLength)
	{
	ZMutexLocker structureLocker(fMutex_Structure);
	ZXLib::DrawString(fDisplay, inDrawable, inGC, inLocationH, inLocationV, inText, inTextLength);
	}

int ZXServer::TextWidth(const ZDCFont& inFont, const char* inText, size_t inTextLength)
	{
	ZMutexLocker structureLocker(fMutex_Structure);
	return ZXLib::TextWidth(fFont, inText, inTextLength);
	}

void ZXServer::GetFontInfo(const ZDCFont& inFont, short& outAscent, short& outDescent, short& outLeading)
	{
	ZMutexLocker structureLocker(fMutex_Structure);
	outAscent = fFont->ascent;
	outDescent = fFont->descent;
	outLeading = 0;
	}

Window ZXServer::CreateWindow(Window inParent, int inLocationH, int inLocationV, unsigned int inSizeH, unsigned int inSizeV, unsigned int inBorderWidth,
					int inDepth, unsigned int inClass, Visual* inVisual, unsigned long inValueMask, XSetWindowAttributes* inAttributes)
	{
	inVisual = fVisualInfo.visual;
	inDepth = fVisualInfo.depth;
	inAttributes->colormap = fColormap;
	Window result = ZXLib::CreateWindow(fDisplay, inParent, inLocationH, inLocationV, inSizeH, inSizeV, inBorderWidth,
					inDepth, inClass, inVisual, inValueMask, inAttributes);

	this->Sync(false);
	return result;
	}

Window ZXServer::CreateSimpleWindow(Window inParent, int inLocationH, int inLocationV, unsigned int inSizeH, unsigned int inSizeV,
					unsigned int inBorderWidth, unsigned long inBorderPixel, unsigned long inBackgroundPixel)
	{
	return ZXLib::CreateSimpleWindow(fDisplay, inParent, inLocationH, inLocationV, inSizeH, inSizeV,
					inBorderWidth, inBorderPixel, inBackgroundPixel);
	}

void ZXServer::SelectInput(Window inWindow, long inEventMask)
	{
	ZXLib::SelectInput(fDisplay, inWindow, inEventMask);
	}

Pixmap ZXServer::CreateBitmapFromData(Drawable inDrawable, char* inData, int inSizeH, int inSizeV)
	{
	return ZXLib::CreateBitmapFromData(fDisplay, inDrawable, inData, inSizeH, inSizeV);
	}

Pixmap ZXServer::CreatePixmap(Drawable inDrawable, unsigned int inSizeH, unsigned int inSizeV, unsigned int inDepth)
	{
	return ZXLib::CreatePixmap(fDisplay, inDrawable, inSizeH, inSizeV, inDepth);
	}

Pixmap ZXServer::CreatePixmapFromDCPixmap(Drawable inDrawable, const ZDCPixmap& inDCPixmap)
	{
	ZMutexLocker locker(ZXLib::sMutex);

	ZPoint pixmapSize = inDCPixmap.Size();

	Pixmap thePixmap = ZXLib::CreatePixmap(fDisplay, inDrawable, pixmapSize.h, pixmapSize.v, fVisualInfo.depth);
	XGCValues values;
	values.graphics_exposures = 0;
	GC theGC = ::XCreateGC(fDisplay, thePixmap, GCGraphicsExposures, &values);
	this->DrawDCPixmap(thePixmap, theGC, ZPoint(0, 0), inDCPixmap, ZRect(inDCPixmap.Size()));
	ZXLib::FreeGC(fDisplay, theGC);
	return thePixmap;
	}

Pixmap ZXServer::CreateBitmapFromDCPixmap(Drawable inDrawable, const ZDCPixmap& inDCPixmap, const ZRect& inSourceBounds, bool inInvert)
	{
	if (inSourceBounds.IsEmpty() || !inDCPixmap)
		return None;

	ZMutexLocker locker(ZXLib::sMutex);

	size_t rowBytes = ((inSourceBounds.Width() + 7) / 8);
	vector<char> theData(rowBytes * inSourceBounds.Height());
	inDCPixmap.CopyTo(ZPoint(0, 0),
						&theData[0],
						ZDCPixmapNS::RasterDesc(ZDCPixmapNS::PixvalDesc(1, false), rowBytes, inSourceBounds.Height(), false),
						ZDCPixmapNS::PixelDesc(1, 0),
						inSourceBounds);

	if (inInvert)
		{
		for (vector<char>::iterator i = theData.begin(); i != theData.end(); ++i)
			*i = 0xFF ^ *i;
		}

	return this->CreateBitmapFromData(inDrawable, &theData[0], inSourceBounds.Width(), inSourceBounds.Height());
	}

void ZXServer::FreePixmap(Pixmap inPixmap)
	{
	ZXLib::FreePixmap(fDisplay, inPixmap);
	}

void ZXServer::DrawDCPixmap(Drawable inDrawable, GC inGC, ZPoint inLocation, const ZDCPixmap& inSourcePixmap, const ZRect& inSourceBounds)
	{
	ZMutexLocker locker(ZXLib::sMutex);

	for (int32 y = 0; y < inSourceBounds.Height(); y += fImageHeight)
		{
		int32 currentHeight = min(inSourceBounds.Height() - y, fImageHeight);
		for (int32 x = 0; x < inSourceBounds.Width(); x += fImageWidth)
			{
			int32 currentWidth = min(inSourceBounds.Width() - x, fImageWidth);
			int32 imageH, imageV;
			size_t imageIndex = this->pSetupImage(currentWidth, currentHeight, imageH, imageV);
			XImage* theXImage = fImages[imageIndex].first;

			ZDCPixmapNS::RasterDesc theRasterDesc;
			theRasterDesc.fPixvalDesc.fDepth = theXImage->bits_per_pixel;
			if (theXImage->depth > 8)
				theRasterDesc.fPixvalDesc.fBigEndian = (theXImage->byte_order == MSBFirst);
			else
				theRasterDesc.fPixvalDesc.fBigEndian = (theXImage->bitmap_bit_order == MSBFirst);
			theRasterDesc.fRowBytes = theXImage->bytes_per_line;
			theRasterDesc.fRowCount = theXImage->height;
			theRasterDesc.fFlipped = false;

			ZRect sourceBounds(currentWidth, currentHeight);
			sourceBounds += ZPoint(inSourceBounds.left + x, inSourceBounds.top + y);

			inSourcePixmap.CopyTo(ZPoint(imageH, imageV),
									theXImage->data,
									theRasterDesc,
									fPixelDesc,
									ZRect(inSourceBounds.left + x, inSourceBounds.top + y, inSourceBounds.left + x + currentWidth, inSourceBounds.top + y + currentHeight));

			if (fImages[imageIndex].second)
				{
				::XShmPutImage(fDisplay, inDrawable, inGC, theXImage, imageH, imageV,
						inLocation.h + x, inLocation.v + y, currentWidth, currentHeight, False);
				}
			else
				{
				::XPutImage(fDisplay, inDrawable, inGC, theXImage, imageH, imageV,
						inLocation.h + x, inLocation.v + y, currentWidth, currentHeight);
				}
			}
		}
	}

GC ZXServer::CreateGC(Drawable inDrawable, unsigned long inValueMask, XGCValues* inValues)
	{
	return ZXLib::CreateGC(fDisplay, inDrawable, inValueMask, inValues);
	}

void ZXServer::FreeGC(GC inGC)
	{
	ZXLib::FreeGC(fDisplay, inGC);
	}

void ZXServer::SetWMProperties(Window a1, XTextProperty* a2, XTextProperty* a3, char** a4, int a5, XSizeHints* a6, XWMHints* a7, XClassHint* a8)
	{
	ZMutexLocker locker(ZXLib::sMutex);
	ZXLib::SetWMProperties(fDisplay, a1, a2, a3, a4, a5, a6, a7, a8);
	}

size_t ZXServer::pGetNextImage()
	{
	if (fCurrentImageIndex == fImages.size())
		{
		::XSync(fDisplay, False);
		fCurrentImageIndex = 0;
		fHorizV = fImageHeight;
		fVertH = fImageWidth;
		fTileH = fImageWidth;
		fTileV1 = fImageHeight;
		fTileV2 = fImageHeight;
		}
	return fCurrentImageIndex++;
	}

size_t ZXServer::pSetupImage(int32 inWidth, int32 inHeight, int32& outLocationH, int32& outLocationV)
	{
	size_t imageIndex;
	if (inWidth >= fImageWidth / 2)
		{
		if (inHeight >= fImageHeight / 2)
			{
			imageIndex = this->pGetNextImage();
			outLocationH = 0;
			outLocationV = 0;
			}
		else
			{
			if (inHeight + fHorizV > fImageHeight)
				{
				fHorizIndex = this->pGetNextImage();
				fHorizV = 0;
				}
			imageIndex = fHorizIndex;
			outLocationH = 0;
			outLocationV = fHorizV;
			fHorizV += inHeight;
			}
		}
	else
		{
		if (inHeight >= fImageHeight / 2)
			{
			if (inWidth + fVertH > fImageWidth)
				{
				fVertIndex = this->pGetNextImage();
				fVertH = 0;
				}
			imageIndex = fVertIndex;
			outLocationH = fVertH;
			outLocationV = 0;
			fVertH += (inWidth + 7) & (~8);
			}
		else
			{
			if (inWidth + fTileH > fImageWidth)
				{
				fTileV1 = fTileV2;
				fTileH = 0;
				}
			if (inHeight + fTileV1 > fImageHeight)
				{
				fTileIndex = this->pGetNextImage();
				fTileH = 0;
				fTileV1 = 0;
				fTileV2 = 0;
				}
			if (fTileV2 < inHeight + fTileV1)
				fTileV2 = inHeight + fTileV1;
			imageIndex = fTileIndex;
			outLocationH = fTileH;
			outLocationV = fTileV1;
			fTileH += (inWidth + 7) & (~8);
			}
		}
	return imageIndex;
	}

unsigned long ZXServer::pLookupColor(const ZRGBColor& inColor)
	{ return fPixelDesc.AsPixval(inColor); }

// =================================================================================================

static const char* sVisualNames[] =
	{
	"StaticGray",
	"GrayScale",
	"StaticColor",
	"PseudoColor",
	"TrueColor",
	"DirectColor",
	};

static uint32 sScoreVisual(const XVisualInfo& inVisual, bool isDefaultVisual)
	{
	uint32 quality = 0;
	uint32 speed = 1;
	if (inVisual.c_class == TrueColor || inVisual.c_class == DirectColor)
		{
		if (inVisual.depth == 24)
			{
			// Should test for MSB visual here, and set speed if so.
			quality = 9;
			}
		else if (inVisual.depth == 16)
			quality = 8;
		else if (inVisual.depth == 15)
			quality = 7;
		else if (inVisual.depth == 8)
			quality = 4;
		}
	else if (inVisual.c_class == PseudoColor || inVisual.c_class == StaticColor)
		{
		if (inVisual.depth == 8)
			quality = 4;
		else if (inVisual.depth == 4)
			quality = 2;
		else if (inVisual.depth == 1)
			quality = 1;
		}
	else if (inVisual.c_class == StaticGray || inVisual.c_class == GrayScale)
		{
		if (inVisual.depth == 8)
			quality = 4;
		else if (inVisual.depth == 4)
			quality = 2;
		else if (inVisual.depth == 1)
			quality = 1;
//		quality = 10; //##
		}

	if (quality == 0)
		return 0;

	uint32 sys = isDefaultVisual ? 1 : 0;

	uint32 pseudo = 0;
	if (inVisual.c_class == TrueColor || inVisual.c_class == PseudoColor)
		pseudo = 1;

	uint32 score = (quality << 12) | (speed << 8) | (sys << 4) | pseudo;

	ZDebugPrintf(kDebug_XHeavy, ("Visual 0x%x, class = %s, depth = %d, %x:%x:%x%s; score=%x\n",
							inVisual.visualid,
							sVisualNames[inVisual.c_class],
							inVisual.depth,
							inVisual.red_mask,
							inVisual.green_mask,
							inVisual.blue_mask,
							isDefaultVisual ? " (system)" : "",
							score));
	return score;
	}

static bool sTryColormap(int32 inCountRed, int32 inCountGreen, int32 inCountBlue,
				Display* inDisplay, const XVisualInfo& inVisualInfo, Colormap inColormap,
				ZDCPixmapNS::PixelDesc& outPixelDesc)
	{
	ZAssertStop(0, inVisualInfo.colormap_size <= 256);

	const int32 colorsToAllocate = inCountRed * inCountGreen * inCountBlue;
	ZAssertStop(0, colorsToAllocate <= 256);

	// Grab the list of colors that already exist in the color map
	vector<XColor> vectorXColors(inVisualInfo.colormap_size);
	vector<int32> vectorXColorUseCounts(inVisualInfo.colormap_size);
	for (size_t x = 0; x < inVisualInfo.colormap_size; ++x)
		{
		vectorXColors[x].pixel = x;
		vectorXColorUseCounts[x] = 0;
		}
	::XQueryColors(inDisplay, inColormap, &vectorXColors[0], vectorXColors.size());

	// For each color we're wanting to get a pixVal for, keep track of the index into vectorXColors
	// that best matches, and what distance it lies from what we really want.
	vector<size_t> bestPixValIndex;
	vector<int32> bestPixValDistance;
	for (size_t x = 0; x < colorsToAllocate; ++x)
		{
		bestPixValIndex.push_back(0);
		bestPixValDistance.push_back(0x7FFFFFFF);
		}

	// Walk vectorXColors.
	int32 colorsNeeded = colorsToAllocate;
	for (size_t x = 0; x < vectorXColors.size(); ++x)
		{
		// Quantize the color to the resolution implied by inCountRed etc.
		int32 actualRed = vectorXColors[x].red >> 8;
		int32 actualGreen = vectorXColors[x].green >> 8;
		int32 actualBlue = vectorXColors[x].blue >> 8;
		int32 indexRed = (actualRed * (inCountRed - 1) + 128) >> 8;
		int32 indexGreen = (actualGreen * (inCountGreen - 1) + 128) >> 8;
		int32 indexBlue = (actualBlue * (inCountBlue - 1) + 128) >> 8;
		int32 quantRed = indexRed * 255 / (inCountRed - 1);
		int32 quantGreen = indexGreen * 255 / (inCountGreen - 1);
		int32 quantBlue = indexBlue * 255 / (inCountBlue - 1);
		int32 index = (indexRed * inCountGreen + indexGreen) * inCountBlue + indexBlue;
		int32 distance = (quantRed - actualRed) * (quantRed - actualRed)
						+ (quantGreen - actualGreen) * (quantGreen - actualGreen)
						+ (quantBlue - actualBlue) * (quantBlue - actualBlue);
		ZDebugPrintf(kDebug_XHeavy, ("pixVal %d/%d, found #%02X%02X%02X, useCount=%d, quantized #%02X%02X%02X, index=%d, distance=%d",
								x, vectorXColors[x].pixel,
								actualRed, actualGreen, actualBlue,
								vectorXColorUseCounts[x],
								quantRed, quantGreen, quantBlue,
								index,
								distance));
		if (bestPixValDistance[index] != 0x7FFFFFFF)
			{
			ZDebugPrintf(kDebug_XHeavy, (", best pixVal so far is %d/%d with distance %d and useCount %d",
									bestPixValIndex[index],
									vectorXColors[bestPixValIndex[index]].pixel,
									bestPixValDistance[index],
									vectorXColorUseCounts[bestPixValIndex[index]]));
			}
		ZDebugPrintf(kDebug_XHeavy, ("\n"));
		if (bestPixValDistance[index] > distance && 1000 > distance)
			{
			ZDebugPrintf(kDebug_XHeavy, ("At %d, using pixVal %d\n", x, vectorXColors[x].pixel));
			if (bestPixValDistance[index] != 0x7FFFFFFF)
				{
				ZAssertStop(0, vectorXColorUseCounts[bestPixValIndex[index]] > 0);
				if (--vectorXColorUseCounts[bestPixValIndex[index]] == 0)
					{
					unsigned long temp = vectorXColors[bestPixValIndex[index]].pixel;
					ZDebugPrintf(kDebug_XHeavy, ("Freeing pixVal %d\n", x, temp));
					::XFreeColors(inDisplay, inColormap, &temp, 1, 0);
					}
				}
			else
				{
				--colorsNeeded;
				}
			bestPixValIndex[index] = x;
			bestPixValDistance[index] = distance;
			if (++vectorXColorUseCounts[x] == 1)
				{
				ZDebugPrintf(kDebug_XHeavy, ("Allocating pixVal %d\n", x, vectorXColors[x].pixel));
				if (!::XAllocColor(inDisplay, inColormap, &vectorXColors[x]))
					{
					ZDebugStopf(kDebug_XHeavy, ("Failed to allocate pix val %d", vectorXColors[x].pixel));
					}
				}

			}
		}

	if (colorsNeeded)
		{
		ZDebugPrintf(kDebug_XHeavy, ("%d colors could not be mapped to existing colors\n", colorsNeeded));
		vector<unsigned long> junk(colorsNeeded);
		if (!::XAllocColorCells(inDisplay, inColormap, false, 0, 0, &junk[0], colorsNeeded))
			{
			ZDebugPrintf(kDebug_XHeavy, ("Couldn't get necessary writable cells\n", colorsNeeded));
			vector<unsigned long> temp;
			for (size_t x = 0; x < vectorXColors.size(); ++x)
				{
				if (vectorXColorUseCounts[x])
					temp.push_back(vectorXColors[x].pixel);
				}
			ZAssertStop(0, temp.size() == colorsToAllocate - colorsNeeded);
			::XFreeColors(inDisplay, inColormap, &temp[0], temp.size(), 0);
			return false;
			}
		::XFreeColors(inDisplay, inColormap, &junk[0], junk.size(), 0);
		}

	vector<ZRGBColor> colors;
	vector<uint32> pixVals;
	size_t index = 0;
	for (size_t indexRed = 0; indexRed < inCountRed; ++indexRed)
		{
		for (size_t indexGreen = 0; indexGreen < inCountGreen; ++indexGreen)
			{
			for (size_t indexBlue = 0; indexBlue < inCountBlue; ++indexBlue, ++index)
				{
				ZDebugPrintf(kDebug_XHeavy, ("Index %d", index));
				ZRGBColor theRGBColor;

				if (bestPixValDistance[index] == 0x7FFFFFFF)
					{
					const unsigned int componentRed = indexRed * 0xFFFFU / (inCountRed - 1);
					const unsigned int componentGreen = indexGreen * 0xFFFFU / (inCountGreen - 1);
					const unsigned int componentBlue = indexBlue * 0xFFFFU / (inCountBlue - 1);
					theRGBColor = ZRGBColor(componentRed, componentGreen, componentBlue);
					XColor anXColor = theRGBColor;

					anXColor.flags = 0;
					if (!::XAllocColor(inDisplay, inColormap, &anXColor))
						{
						ZDebugLogf(kDebug_XHeavy, (" Failed to allocate pix val"));
						}
					else
						{
						ZDebugPrintf(kDebug_XHeavy, (" pixVal %d (newly allocated)", anXColor.pixel));
						}
					theRGBColor = anXColor; // Hmm. Why?
					pixVals.push_back(anXColor.pixel);
					}
				else
					{
					ZDebugPrintf(kDebug_XHeavy, (" pixVal %d", vectorXColors[bestPixValIndex[index]].pixel));
					theRGBColor = vectorXColors[bestPixValIndex[index]];
					pixVals.push_back(vectorXColors[bestPixValIndex[index]].pixel);
					}
				colors.push_back(theRGBColor);
				ZDebugPrintf(kDebug_XHeavy, (" color  #%02X%02X%02X\n", theRGBColor.red >> 8, theRGBColor.green >> 8, theRGBColor.blue >> 8));
				}
			}
		}

	outPixelDesc = new ZDCPixmapNS::PixelDescRep_Indexed(&colors[0], &pixVals[0], colors.size());
	return true;
	}

static struct
	{
	int32 fRed;
	int32 fGreen;
	int32 fBlue;
	} sCompCounts[] =
	{
	{ 6, 6, 6},
	{ 6, 6, 5},
	{ 6, 6, 4},
	{ 5, 5, 5},
	{ 5, 5, 4},
	{ 4, 4, 4},
	{ 4, 4, 3},
	{ 3, 3, 3},
	{ 3, 3, 2},
	{ 2, 2, 2},
	{ 0, 0, 0},
	};

static void sDoColormaps(Display* inDisplay, const XVisualInfo& inVisualInfo, Colormap inColormap, ZDCPixmapNS::PixelDesc& outPixelDesc)
	{
	ZDebugPrintf(kDebug_XHeavy, ("\nsDoColormaps\n"));
	for (size_t x = 0; sCompCounts[x].fRed != 0; ++x)
		{
		ZDebugPrintf(kDebug_XHeavy, ("Trying with r=%d, g=%d, b=%d\n", sCompCounts[x].fRed, sCompCounts[x].fGreen, sCompCounts[x].fBlue));
		if (sTryColormap(sCompCounts[x].fRed, sCompCounts[x].fGreen, sCompCounts[x].fBlue, inDisplay, inVisualInfo, inColormap, outPixelDesc))
			{
			ZDebugPrintf(kDebug_XHeavy, ("Using r=%d, g=%d, b=%d\n", sCompCounts[x].fRed, sCompCounts[x].fGreen, sCompCounts[x].fBlue));
			return;
			}
		}
	ZDebugStopf(0, ("sDoColormaps, couldn't get the colors needed\n"));
	}

extern "C" int XShmGetEventBase(Display*);

void ZXServer::InitGraphics()
	{
	_XPrivDisplay privDisplay = reinterpret_cast<_XPrivDisplay>(fDisplay);

	ZDebugPrintf(kDebug_X, ("fd: %d\n", privDisplay->fd));
	ZDebugPrintf(kDebug_X, ("proto_major_version: %d\n", privDisplay->proto_major_version));
	ZDebugPrintf(kDebug_X, ("proto_minor_version: %d\n", privDisplay->proto_minor_version));
	ZDebugPrintf(kDebug_X, ("vendor: %s\n", privDisplay->vendor));
	ZDebugPrintf(kDebug_X, ("byte_order: %d\n", privDisplay->byte_order));
	ZDebugPrintf(kDebug_X, ("bitmap_unit: %d\n", privDisplay->bitmap_unit));
	ZDebugPrintf(kDebug_X, ("bitmap_pad: %d\n", privDisplay->bitmap_pad));
	ZDebugPrintf(kDebug_X, ("bitmap_bit_order: %d\n", privDisplay->bitmap_bit_order));
	ZDebugPrintf(kDebug_X, ("nformats: %d\n", privDisplay->nformats));
	ZDebugPrintf(kDebug_X, ("release: %d\n", privDisplay->release));

	ZDebugPrintf(kDebug_X, ("Default visual: 0x%x\n", DefaultVisual(fDisplay, DefaultScreen(fDisplay))));

// Let's see what visuals we have available
	XVisualInfo visualTemplate;
	int count;
	ZMutexLocker locker(ZXLib::sMutex);
	XVisualInfo* visualList = ::XGetVisualInfo(fDisplay, 0, &visualTemplate, &count);

	uint32 bestScore = 0;

	ZDebugPrintf(kDebug_X, ("\nFound %d visuals\n", count));
	for (int x = 0; x < count; ++x)
		{
		ZDebugPrintf(kDebug_X, ("\nVisual index: %d\n", x));
		ZDebugPrintf(kDebug_X, ("visualid: 0x%x\n", visualList[x].visualid));
		ZDebugPrintf(kDebug_X, ("visual: 0x%x\n", visualList[x].visual));
		ZDebugPrintf(kDebug_X, ("screen: 0x%x\n", visualList[x].screen));
		ZDebugPrintf(kDebug_X, ("depth: %d\n", visualList[x].depth));
		ZDebugPrintf(kDebug_X, ("c_class: %d\n", visualList[x].c_class));
		ZDebugPrintf(kDebug_X, ("red_mask: %08X\n", visualList[x].red_mask));
		ZDebugPrintf(kDebug_X, ("green_mask: %08X\n", visualList[x].green_mask));
		ZDebugPrintf(kDebug_X, ("blue_mask: %08X\n", visualList[x].blue_mask));
		ZDebugPrintf(kDebug_X, ("colormap_size: %d\n", visualList[x].colormap_size));
		ZDebugPrintf(kDebug_X, ("bits_per_rgb: %d\n", visualList[x].bits_per_rgb));
		uint32 currentScore = sScoreVisual(visualList[x], visualList[x].visual == DefaultVisual(fDisplay, DefaultScreen(fDisplay)));
		if (bestScore < currentScore)
			{
			bestScore = currentScore;
			fVisualInfo = visualList[x];
			}
		}
	ZDebugPrintf(kDebug_X, ("\nBest visual is 0x%x\n", fVisualInfo.visualid));
	::XFree(visualList);

	if (fVisualInfo.visual == DefaultVisual(fDisplay, DefaultScreen(fDisplay)))
		{
		fColormap = DefaultColormap(fDisplay, DefaultScreen(fDisplay));
		}
	else
		{
		fColormap = ::XCreateColormap(fDisplay, RootWindow(fDisplay, DefaultScreen(fDisplay)), fVisualInfo.visual, AllocNone);
//		vector<XColor> defaultColors(fVisualInfo.colormap_size);
//		for (size_t x = 0; x < defaultColors.size(); ++x)
//			defaultColors[x].pixel = x;
//		::XQueryColors(fDisplay, DefaultColormap(fDisplay, DefaultScreen(fDisplay)), &defaultColors[0], defaultColors.size());
//		::XStoreColors(fDisplay, fColormap, &defaultColors[0], defaultColors.size());
		::XSync(fDisplay, false);
		}

	switch (fVisualInfo.c_class)
		{
		case StaticGray:
		case GrayScale:
		case StaticColor:
		case PseudoColor:
			{
			ZMutexLocker locker(ZXLib::sMutex);
			sDoColormaps(fDisplay, fVisualInfo, fColormap, fPixelDesc);
			break;
			}
		case TrueColor:
		case DirectColor:
			{
			fPixelDesc = ZDCPixmapNS::PixelDesc(fVisualInfo.red_mask, fVisualInfo.green_mask, fVisualInfo.blue_mask, 0);
			break;
			}
		}

	// Set up our shared XImages
	fImageWidth = 256;
	fImageHeight = 64;
	fHorizV = fImageHeight;
	fVertH = fImageWidth;
	fTileH = fImageWidth;
	fTileV1 = fImageHeight;
	fTileV2 = fImageHeight;
	fCurrentImageIndex = 0;

	bool allOkay = false;
	int ignore;
	if (::XQueryExtension(fDisplay, "MIT-SHM", &ignore, &ignore, &ignore))
		{
		int major, minor;
		Bool pixmaps;
		if (::XShmQueryVersion(fDisplay, &major, &minor, &pixmaps ) == True)
			{
			allOkay = true;
			fShmCompletionEvent = ::XShmGetEventBase(fDisplay) + ShmCompletion;
			ZDebugPrintf(kDebug_X, ("\nHas shared images, major: %d, minor: %d, ShmCompletion: %d\n", major, minor, fShmCompletionEvent));
			fHasSharedImages = true;
			if (pixmaps)
				{
				fHasSharedPixmaps = true;
				ZDebugPrintf(kDebug_X, ("Has shared pixmaps\n"));
				}
			for (size_t x = 0; x < 10 && allOkay; ++x)
				{
				XShmSegmentInfo* theXShmSegmentInfo = nullptr;
				XImage* theSHMXImage = nullptr;
				try
					{
					theXShmSegmentInfo = new XShmSegmentInfo;
					theXShmSegmentInfo->shmseg = 0;
					theXShmSegmentInfo->shmid = -1;
					theXShmSegmentInfo->shmaddr = nullptr;
					theXShmSegmentInfo->readOnly = False;

					theSHMXImage = ::XShmCreateImage(fDisplay, fVisualInfo.visual, fVisualInfo.depth, ZPixmap, nullptr, theXShmSegmentInfo, fImageWidth, fImageHeight);
					if (theSHMXImage == nullptr)
						throw runtime_error("XShmCreateImage failed");

					theXShmSegmentInfo->shmid = ::shmget(IPC_PRIVATE, theSHMXImage->bytes_per_line * theSHMXImage->height, IPC_CREAT | 0777);
					if (theXShmSegmentInfo->shmid == -1)
						throw runtime_error("shmget failed");

					theXShmSegmentInfo->shmaddr = reinterpret_cast<char*>(::shmat(theXShmSegmentInfo->shmid, 0, 0));
					if (theXShmSegmentInfo->shmaddr == reinterpret_cast<char*>(-1))
						throw runtime_error("shmat failed");

					ZXLib::ErrorTrap theErrorTrap;
					::XShmAttach(fDisplay, theXShmSegmentInfo);
					::XSync(fDisplay, False);
					if (theErrorTrap.GetError() != 0)
						throw runtime_error("XShmAttach failed");

					theSHMXImage->data = theXShmSegmentInfo->shmaddr;
					fImages.push_back(pair<XImage*, XShmSegmentInfo*>(theSHMXImage, theXShmSegmentInfo));
					}
				catch (...)
					{
					allOkay = false;
					if (theXShmSegmentInfo)
						{
						if (theXShmSegmentInfo->shmaddr != nullptr && theXShmSegmentInfo->shmaddr != reinterpret_cast<char*>(-1))
							{
							::shmdt(theXShmSegmentInfo->shmaddr);
							theXShmSegmentInfo->shmaddr = nullptr;
							}
						if (theXShmSegmentInfo->shmid != -1)
							::shmctl(theXShmSegmentInfo->shmid, IPC_RMID, 0);
						}

					if (theSHMXImage)
						XDestroyImage(theSHMXImage);

					delete theXShmSegmentInfo;
					}
				}

			if (!allOkay)
				{
				ZDebugLogf(kDebug_X, (" Failed to allocate all shared images, releasing what we got"));

				ZXLib::ErrorTrap theErrorTrap;

				for (size_t x = 0; x < fImages.size(); ++x)
					::XShmDetach(fDisplay, fImages[x].second);
				::XSync(fDisplay, False);

				for (size_t x = 0; x < fImages.size(); ++x)
					{
					::shmdt(fImages[x].second->shmaddr);
					::shmctl(fImages[x].second->shmid, IPC_RMID, 0);
					XDestroyImage(fImages[x].first);
					}
				::XSync(fDisplay, False);

				for (size_t x = 0; x < fImages.size(); ++x)
					delete fImages[x].second;
				fImages.clear();
				}
			}
		}

	if (!allOkay)
		{
		allOkay = true;
		for (size_t x = 0; x < 10 && allOkay; ++x)
			{
			XImage* theXImage = nullptr;
			try
				{
				theXImage = ::XCreateImage(fDisplay, fVisualInfo.visual, fVisualInfo.depth, ZPixmap, 0, nullptr, fImageWidth, fImageHeight, 32, 0);
				if (!theXImage)
					throw runtime_error("XCreateImage failed");
				theXImage->data = reinterpret_cast<char*>(malloc(theXImage->bytes_per_line * theXImage->height));
				if (theXImage->data == nullptr)
					throw bad_alloc();
				fImages.push_back(pair<XImage*, XShmSegmentInfo*>(theXImage, nullptr));
				}
			catch (...)
				{
				allOkay = false;
				if (theXImage)
					XDestroyImage(theXImage);
				}
			}

		if (!allOkay)
			{
			ZDebugLogf(kDebug_X, (" Failed to allocate non-shared images, releasing what we got"));
			for (size_t x = 0; x < fImages.size(); ++x)
				XDestroyImage(fImages[x].first);
			fImages.clear();
			}
		}
	}

// =================================================================================================
// MARK: - ZXWindow

ZXWindow::ZXWindow()
	{
	fWindow = 0;
	}

ZXWindow::~ZXWindow()
	{
	ZAssertStop(2, fWindow == 0);
	}

Window ZXWindow::GetWindow()
	{ return fWindow; }

ZRef<ZXServer> ZXWindow::GetXServer()
	{ return fXServer; }

void ZXWindow::HandleXEvent(const XEvent& inEvent)
	{
	}

void ZXWindow::AttachToWindow(ZRef<ZXServer> inXServer, Window inWindow)
	{
	ZAssertStop(2, fWindow == 0 && !fXServer);
	fXServer = inXServer;
	fWindow = inWindow;
	ZXLib::SaveContext(fXServer->GetDisplay(), fWindow, sXContext_Window, reinterpret_cast<XPointer>(this));
	}

void ZXWindow::DetachFromWindow()
	{
	ZAssertStop(2, fWindow && fXServer);
	ZXLib::DeleteContext(fXServer->GetDisplay(), fWindow, sXContext_Window);
	fWindow = 0;
	fXServer.Clear();
	}

ZXWindow* ZXWindow::sFromDisplayAndWindow(Display* inDisplay, Window inWindow)
	{
	ZMutexLocker locker(ZXLib::sMutex);
	ZAssertStop(2, inDisplay && inWindow);

	ZXWindow* theXWindow = nullptr;
	int result = ZXLib::FindContext(inDisplay, inWindow, sXContext_Window, reinterpret_cast<XPointer*>(&theXWindow));
	ZAssertStop(2, result == 0);
	ZAssertStop(2, theXWindow != nullptr);
	ZAssertStop(2, theXWindow->fXServer->GetDisplay() == inDisplay && theXWindow->fWindow == inWindow);
	return theXWindow;
	}

ZXWindow* ZXWindow::sFromDisplayAndWindowNilOkay(Display* inDisplay, Window inWindow)
	{
	ZXWindow* theXWindow = nullptr;
	if (inWindow)
		int result = ZXLib::FindContext(inDisplay, inWindow, sXContext_Window, reinterpret_cast<XPointer*>(&theXWindow));
	return theXWindow;
	}

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(X11)
