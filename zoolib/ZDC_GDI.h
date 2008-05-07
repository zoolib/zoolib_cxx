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

#ifndef __ZDC_GDI__
#define __ZDC_GDI__ 1
#include "zconfig.h"

#include "ZDC.h"

#if ZCONFIG(API_Graphics, GDI)

#include "ZWinHeader.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI

class ZDCSetupForGDI;

class ZDCCanvas_GDI : public ZDCCanvas
	{
public:
	static ZRef<ZDCCanvas_GDI> sFindCanvasOrCreateNative(HDC inHDC);

protected:
	static ZMutex sMutex_List;
	static ZDCCanvas_GDI* sCanvas_Head;
	ZDCCanvas_GDI* fCanvas_Prev;
	ZDCCanvas_GDI* fCanvas_Next;

protected:
	ZDCCanvas_GDI();
	virtual ~ZDCCanvas_GDI();

public:
// Force subclasses to implement Finalize
	virtual void Finalize() = 0;

// Our protocol, linked list of all canvases
	void Internal_Link();
	void Internal_Unlink();

// From ZDCCanvas
// Simple pixel and lines
	virtual void Pixel(ZDCState& ioState, ZCoord inLocationH, ZCoord inLocationV, const ZRGBColor& inColor);

	virtual void Line(ZDCState& ioState, ZCoord inStartH, ZCoord inStartV, ZCoord inEndH, ZCoord inEndV);

// Rectangle
	virtual void FrameRect(ZDCState& ioState, const ZRect& inRect);
	virtual void FillRect(ZDCState& ioState, const ZRect& inRect);
	virtual void InvertRect(ZDCState& ioState, const ZRect& inRect);

// Region
	virtual void FrameRegion(ZDCState& ioState, const ZDCRgn& inRgn);
	virtual void FillRegion(ZDCState& ioState, const ZDCRgn& inRgn);
	virtual void InvertRegion(ZDCState& ioState, const ZDCRgn& inRgn);

// Round cornered rect
	virtual void FrameRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize);
	virtual void FillRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize);
	virtual void InvertRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize);

// Ellipse
	virtual void FrameEllipse(ZDCState& ioState, const ZRect& inBounds);
	virtual void FillEllipse(ZDCState& ioState, const ZRect& inBounds);
	virtual void InvertEllipse(ZDCState& ioState, const ZRect& inBounds);

// Poly
	virtual void FillPoly(ZDCState& ioState, const ZDCPoly& inPoly);
	virtual void InvertPoly(ZDCState& ioState, const ZDCPoly& inPoly);

// Pixmap
	virtual void FillPixmap(ZDCState& ioState, const ZPoint& inLocation, const ZDCPixmap& inPixmap);
	virtual void DrawPixmap(ZDCState& ioState, const ZPoint& inLocation, const ZDCPixmap& inSourcePixmap, const ZDCPixmap* inMaskPixmap);

// Text
	virtual void DrawText(ZDCState& ioState, const ZPoint& inLocation, const char* inText, size_t inTextLength);

	virtual ZCoord GetTextWidth(ZDCState& ioState, const char* inText, size_t inTextLength);
	virtual void GetFontInfo(ZDCState& ioState, ZCoord& outAscent, ZCoord& outDescent, ZCoord& outLeading);
	virtual ZCoord GetLineHeight(ZDCState& ioState);
	virtual void BreakLine(ZDCState& ioState, const char* inLineStart, size_t inRemainingTextLength, ZCoord inTargetWidth, size_t& outNextLineDelta);

	virtual void Flush();
	virtual void Sync();

	virtual void InvalCache();

	virtual short GetDepth();
	virtual bool IsOffScreen();
	virtual bool IsPrinting();

// OffScreen
	virtual ZRef<ZDCCanvas> CreateOffScreen(const ZRect& inCanvasRect);
	virtual ZRef<ZDCCanvas> CreateOffScreen(ZPoint dimensions, ZDCPixmapNS::EFormatEfficient iFormat);

// ----------
// Internal or non-virtual implementation stuff
	ZPoint Internal_GDIStart(ZDCState& ioState, bool zeroOrigin);
	void Internal_GDIEnd();

	virtual ZDCRgn Internal_CalcClipRgn(const ZDCState& inState) = 0;

	HDC Internal_GetHDC() { return fHDC; }

	void Internal_TileRegion(ZDCState& ioState, const ZDCRgn& inRgn, const ZDCPixmap& inPixmap);

	bool fOriginValid;
	long fChangeCount_Clip;

	HDC fHDC;

	ZMutex* fMutexToLock;
	ZMutex* fMutexToCheck;

	class SetupDC;
	friend class SetupDC;
	class SetupHBRUSH;
	friend class SetupHBRUSH;
	class SetupHPEN;
	friend class SetupHPEN;
	class SetupModeColor;
	friend class SetupModeColor;
	class SetupHFONT;
	friend class SetupHFONT;

	friend class ZDCSetupForGDI;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI_NonWindow

class ZDCCanvas_GDI_NonWindow : public ZDCCanvas_GDI
	{
public:
	ZDCCanvas_GDI_NonWindow();
	virtual ~ZDCCanvas_GDI_NonWindow();

	virtual void Finalize();
	virtual void Scroll(ZDCState& ioState, const ZRect& inRect, ZCoord inDeltaH, ZCoord inDeltaV);
	virtual void CopyFrom(ZDCState& ioState, const ZPoint& inDestLocation, ZRef<ZDCCanvas> inSourceCanvas, const ZDCState& inSourceState, const ZRect& inSourceRect);
	virtual ZDCRgn Internal_CalcClipRgn(const ZDCState& inState);

protected:
	ZMutex fMutex;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI_OffScreen

class ZDCCanvas_GDI_OffScreen : public ZDCCanvas_GDI_NonWindow
	{
public:
	ZDCCanvas_GDI_OffScreen(HDC inOther, const ZRect& inGlobalRect);
	ZDCCanvas_GDI_OffScreen(HDC inOther, ZPoint inDimensions, ZDCPixmapNS::EFormatEfficient iFormat);
	virtual ~ZDCCanvas_GDI_OffScreen();

	virtual ZRGBColor GetPixel(ZDCState& ioState, ZCoord inLocationH, ZCoord inLocationV);

	virtual void FloodFill(ZDCState& ioState, const ZPoint& inSeedLocation);

	virtual ZDCPixmap GetPixmap(ZDCState& ioState, const ZRect& inBounds);

	virtual bool IsOffScreen();

protected:
	HBITMAP fHBITMAP;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI_IC

class ZDCCanvas_GDI_IC : public ZDCCanvas_GDI_NonWindow
	{
public:
	ZDCCanvas_GDI_IC();
	virtual ~ZDCCanvas_GDI_IC();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDC_NativeGDI

class ZDC_NativeGDI : public ZDC
	{
public:
	ZDC_NativeGDI(HDC inHDC);
	~ZDC_NativeGDI();

protected:
	HDC fHDC;
	int fSavedDCIndex;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI::SetupDC

class ZDCCanvas_GDI::SetupDC
	{
public:
	SetupDC(ZDCCanvas_GDI* inCanvas, const ZDCState& inState);
	~SetupDC();

protected:
	ZDCCanvas_GDI* fCanvas;
	const ZDCState* fState;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI::SetupDC

class ZDCSetupForGDI
	{
public:
	ZDCSetupForGDI(const ZDC& inDC, bool inZeroOrigin);
	~ZDCSetupForGDI();
	ZPoint GetOffset();
	HDC GetHDC();
protected:
	ZRef<ZDCCanvas_GDI> fCanvas;
	ZPoint fOffset;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapRep_DIB

class ZDCPixmapRep_DIB : public ZDCPixmapRep
	{
public:
	ZDCPixmapRep_DIB(ZRef<ZDCPixmapRaster> inBuffer,
								const ZRect& inBounds,
								const ZDCPixmapNS::PixelDesc& inPixelDesc);

	ZDCPixmapRep_DIB(HDC inHDC, HBITMAP inHBITMAP, bool inForce32BPP = false);

	virtual ~ZDCPixmapRep_DIB();

	void GetDIBStuff(BITMAPINFO*& outBITMAPINFO, char*& outBits, ZRect* outBounds);

	static ZRef<ZDCPixmapRep_DIB> sAsPixmapRep_DIB(ZRef<ZDCPixmapRep> inRep);

protected:
	BITMAPINFO* fBITMAPINFO;
	int fChangeCount;
	};

// =================================================================================================

#endif // ZCONFIG(API_Graphics, GDI)

#endif // __ZDC_GDI__
