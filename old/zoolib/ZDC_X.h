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

#ifndef __ZDC_X_h__
#define __ZDC_X_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__DC_X
	#define ZCONFIG_API_Avail__DC_X ZCONFIG_SPI_Enabled(X11)
#endif

#ifndef ZCONFIG_API_Desired__DC_X
	#define ZCONFIG_API_Desired__DC_X 1
#endif

#include "zoolib/ZDC.h"

#if ZCONFIG_API_Enabled(DC_X)

#include "zoolib/ZXServer.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCCanvas_X

class ZDCPixmapRep_XPixmap;

class ZDCCanvas_X : public ZDCCanvas
	{
protected:
	ZDCCanvas_X();
	virtual ~ZDCCanvas_X();

public:
// Force subclasses to implement Finalize
	virtual void Finalize() = 0;

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
	virtual void DrawPixmap(ZDCState& ioState, const ZPoint& inLocation, const ZDCPixmap& inSourcePixmap, const ZDCPixmap* inMaskPixmap);

// Text
	virtual void DrawText(ZDCState& ioState, const ZPoint& inLocation, const char* inText, size_t textLength);

	virtual ZCoord GetTextWidth(ZDCState& ioState, const char* inText, size_t textLength);
	virtual void GetFontInfo(ZDCState& ioState, ZCoord& outAscent, ZCoord& outDescent, ZCoord& outLeading);
	virtual ZCoord GetLineHeight(ZDCState& ioState);
	virtual void BreakLine(ZDCState& ioState, const char* inLineStart, size_t inRemainingTextLength, ZCoord inTargetWidth, size_t& outNextLineDelta);

	virtual void InvalCache();

// Syncing
	virtual void Sync();
	virtual void Flush();

	virtual short GetDepth();
	virtual bool IsOffScreen();
	virtual bool IsPrinting();

// OffScreen
	virtual ZRef<ZDCCanvas> CreateOffScreen(const ZRect& inCanvasRect);
	virtual ZRef<ZDCCanvas> CreateOffScreen(ZPoint inDimensions, ZDCPixmapNS::EFormatEfficient iFormat);

// ----------
// Internal or non-virtual implementation stuff
	virtual ZDCRgn Internal_CalcClipRgn(const ZDCState& inState) = 0;

	Drawable Internal_GetDrawable() { return fDrawable; }
protected:
	long fChangeCount_Origin;
	long fChangeCount_PatternOrigin;
	long fChangeCount_Ink;
	long fChangeCount_PenWidth;
	long fChangeCount_Mode;
	long fChangeCount_Clip;
	long fChangeCount_Font;
	long fChangeCount_TextColor;

	ZRef<ZXServer> fXServer;
	Drawable fDrawable;
	GC fGC;

	Pixmap fCachedInkPixmap;

	ZMutex* fMutexToLock;
	ZMutex* fMutexToCheck;


	class SetupLock;
	friend class SetupLock;
	class SetupClip;
	friend class SetupClip;
	class SetupInk;
	friend class SetupInk;
	};

// =================================================================================================
// MARK: - ZDCCanvas_X::SetupLock

class ZDCCanvas_X::SetupLock
	{
public:
	SetupLock(ZDCCanvas_X* inCanvas);
	~SetupLock();

protected:
	ZDCCanvas_X* fCanvas;
	};

// =================================================================================================
// MARK: - ZDCCanvas_X_NonWindow

class ZDCCanvas_X_NonWindow : public ZDCCanvas_X
	{
public:
	ZDCCanvas_X_NonWindow();
	virtual ~ZDCCanvas_X_NonWindow();

	virtual void Finalize();

// Moving blocks of pixels
	virtual void Scroll(ZDCState& ioState, const ZRect& inRect, ZCoord hDelta, ZCoord vDelta);
	virtual void CopyFrom(ZDCState& ioState, const ZPoint& inDestLocation, ZRef<ZDCCanvas> inSourceCanvas, const ZDCState& inSourceState, const ZRect& inSourceRect);

	virtual ZDCRgn Internal_CalcClipRgn(const ZDCState& inState);

protected:
	ZMutex fMutex;
	};

// =================================================================================================
// MARK: - ZDCCanvas_X_OffScreen

class ZDCCanvas_X_OffScreen : public ZDCCanvas_X_NonWindow
	{
public:
	ZDCCanvas_X_OffScreen(ZRef<ZXServer> inXServer, ZPoint inDimensions, ZDCPixmapNS::EFormatEfficient iFormat);
	virtual ~ZDCCanvas_X_OffScreen();

	virtual bool IsOffScreen();
	};

/*
// =================================================================================================
// MARK: - ZDCPixmapRep_XPixmap

class ZDCPixmapRep_XPixmap : public ZDCPixmapRep
	{
public:
	ZDCPixmapRep_XPixmap();

	Pixmap GetXPixmap();

	ZX11Pixmap* GetX11Pixmap();

protected:
	ZX11Pixmap* fX11Pixmap;
	};
*/

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(DC_X)

#endif // __ZDC_X_h__
