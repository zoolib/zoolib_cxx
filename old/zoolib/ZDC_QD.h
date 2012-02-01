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

#ifndef __ZDC_QD_h__
#define __ZDC_QD_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__DC_QD
	#define ZCONFIG_API_Avail__DC_QD ZCONFIG_SPI_Enabled(QuickDraw)
#endif

#ifndef ZCONFIG_API_Desired__DC_QD
	#define ZCONFIG_API_Desired__DC_QD 1
#endif


#include "zoolib/ZDC.h"

#if ZCONFIG_API_Enabled(DC_QD)

#include "zoolib/ZThreadOld.h"
#include "zoolib/ZUtil_Mac_LL.h"

#include ZMACINCLUDE3(ApplicationServices,QD,QDOffscreen.h)

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCCanvas_QD

class ZDCCanvas_QD_OffScreen;
class ZDCSetupForQD;
class ZDCPixmapRep_QD;

class ZDCCanvas_QD : public ZDCCanvas
	{
public:
	static ZRef<ZDCCanvas_QD> sFindCanvasOrCreateNative(CGrafPtr inGrafPtr);

protected:
// Linked list of all canvases
	void Internal_Link(CGrafPtr inGrafPtr);
	void Internal_Unlink();
	static ZMutex sMutex_List;
	static ZDCCanvas_QD* sCanvas_Head;
	ZDCCanvas_QD* fCanvas_Prev;
	ZDCCanvas_QD* fCanvas_Next;

protected:
	ZDCCanvas_QD();
	virtual ~ZDCCanvas_QD();

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
	virtual void HiliteRect(ZDCState& ioState, const ZRect& inRect, const ZRGBColor& inBackColor);

// Region
	virtual void FrameRegion(ZDCState& ioState, const ZDCRgn& inRgn);
	virtual void FillRegion(ZDCState& ioState, const ZDCRgn& inRgn);
	virtual void InvertRegion(ZDCState& ioState, const ZDCRgn& inRgn);
	virtual void HiliteRegion(ZDCState& ioState, const ZDCRgn& inRgn, const ZRGBColor& inBackColor);

// Round cornered rect
	virtual void FrameRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize);
	virtual void FillRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize);
	virtual void InvertRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize);
	virtual void HiliteRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize, const ZRGBColor& inBackColor);

// Ellipse
	virtual void FrameEllipse(ZDCState& ioState, const ZRect& inBounds);
	virtual void FillEllipse(ZDCState& ioState, const ZRect& inBounds);
	virtual void InvertEllipse(ZDCState& ioState, const ZRect& inBounds);
	virtual void HiliteEllipse(ZDCState& ioState, const ZRect& inBounds, const ZRGBColor& inBackColor);

// Poly
	virtual void FillPoly(ZDCState& ioState, const ZDCPoly& inPoly);
	virtual void InvertPoly(ZDCState& ioState, const ZDCPoly& inPoly);
	virtual void HilitePoly(ZDCState& ioState, const ZDCPoly& inPoly, const ZRGBColor& inBackColor);

// Pixmap
	virtual void FillPixmap(ZDCState& ioState, const ZPoint& inLocation, const ZDCPixmap& inPixmap);
	virtual void DrawPixmap(ZDCState& ioState, const ZPoint& inLocation, const ZDCPixmap& inSourcePixmap, const ZDCPixmap* inMaskPixmap);

// Flood Fill
	virtual void FloodFill(ZDCState& ioState, const ZPoint& inSeedLocation);

// Text
	virtual void DrawText(ZDCState& ioState, const ZPoint& iLocation, const char* iText, size_t iTextLength);

	virtual ZCoord GetTextWidth(ZDCState& ioState, const char* iText, size_t iTextLength);
	virtual void GetFontInfo(ZDCState& ioState, ZCoord& oAscent, ZCoord& oDescent, ZCoord& oLeading);
	virtual ZCoord GetLineHeight(ZDCState& ioState);
	virtual void BreakLine(ZDCState& ioState, const char* iLineStart, size_t iRemainingTextLength, ZCoord iTargetWidth, size_t& oNextLineDelta);

// Escape hatch
	virtual void InvalCache();

// Syncing
	virtual void Sync();
	virtual void Flush();

// Info
	virtual short GetDepth();
	virtual bool IsOffScreen();
	virtual bool IsPrinting();

// OffScreen
	virtual ZRef<ZDCCanvas> CreateOffScreen(const ZRect& inCanvasRect);
	virtual ZRef<ZDCCanvas> CreateOffScreen(ZPoint inDimensions, ZDCPixmapNS::EFormatEfficient iFormat);

// ----------
// Internal or non-virtual implementation stuff
	ZPoint Internal_QDStart(ZDCState& ioState, bool inUsingPatterns);
	void Internal_QDEnd();

	virtual ZDCRgn Internal_CalcClipRgn(const ZDCState& inState) = 0;
	virtual ZDCRgn Internal_GetExcludeRgn();

	CGrafPtr Internal_GetGrafPtr() { return fGrafPtr; }

	void Internal_SetupPortReal(ZDCState& ioState, bool usingPatterns);
	void Internal_SetupInk(ZDCState& ioState);
	void Internal_SetupText(ZDCState& ioState);

	void Internal_TileRegion(ZDCState& ioState, const ZDCRgn& inRgn, ZRef<ZDCPixmapRep_QD> inPixmapRep);

protected:
	long fChangeCount_Origin;
	long fChangeCount_PatternOrigin;
	long fChangeCount_Ink;
	long fChangeCount_PenWidth;
	long fChangeCount_Mode;
	long fChangeCount_Clip;
	long fChangeCount_Font;

	CGrafPtr fGrafPtr;

	ZMutex* fMutexToLock;
	ZMutex* fMutexToCheck;

	PixPatHandle fPixPatHandle;

	class SetupPort;
	friend class SetupPort;
	class SetupInk;
	friend class SetupInk;
	class SetupText;
	friend class SetupText;

	friend class ZDCSetupForQD;
	friend class ZDCCanvas_QD_OffScreen;
	};

// =================================================================================================
// MARK: - ZDCCanvas_QD_NonWindow

class ZDCCanvas_QD_NonWindow : public ZDCCanvas_QD
	{
public:
	ZDCCanvas_QD_NonWindow();
	virtual ~ZDCCanvas_QD_NonWindow();

	virtual void Finalize();
	virtual void Scroll(ZDCState& ioState, const ZRect& inRect, ZCoord hDelta, ZCoord vDelta);
	virtual void CopyFrom(ZDCState& ioState, const ZPoint& inDestLocation, ZRef<ZDCCanvas> inSourceCanvas, const ZDCState& inSourceState, const ZRect& inSourceRect);
	virtual ZDCRgn Internal_CalcClipRgn(const ZDCState& inState);

protected:
	ZMutex fMutex;
	};

// =================================================================================================
// MARK: - ZDCCanvas_QD_OffScreen

class ZDCCanvas_QD_OffScreen : public ZDCCanvas_QD_NonWindow
	{
public:
	ZDCCanvas_QD_OffScreen(const ZRect& inGlobalRect);
	ZDCCanvas_QD_OffScreen(ZPoint inDimensions, ZDCPixmapNS::EFormatEfficient iFormat);
	virtual ~ZDCCanvas_QD_OffScreen();

	virtual ZRGBColor GetPixel(ZDCState& ioState, ZCoord inLocationH, ZCoord inLocationV);

	virtual ZDCPixmap GetPixmap(ZDCState& ioState, const ZRect& inBounds);

protected:
	GWorldPtr fGWorldPtr;
	};

// =================================================================================================
// MARK: - ZDCCanvas_QD_Print

class ZDCCanvas_QD_Print : public ZDCCanvas_QD_NonWindow
	{
public:
	ZDCCanvas_QD_Print(GrafPtr inGrafPtr);
	virtual ~ZDCCanvas_QD_Print();

	virtual bool IsPrinting();
	};

// =================================================================================================
// MARK: - ZDC_PICT

class ZStreamW;
class ZDC_PICT : public ZDC
	{
public:
	ZDC_PICT(const ZRect& inBounds, const ZStreamW& inStream);
	~ZDC_PICT();
	};

// =================================================================================================
// MARK: - ZDC_NativeQD

class ZDC_NativeQD : public ZDC
	{
public:
	ZDC_NativeQD(CGrafPtr inGrafPtr);
	~ZDC_NativeQD();

protected:
	CGrafPtr fSavedGrafPtr;
	GDHandle fSavedGDHandle;

	CGrafPtr fGrafPtr;

	ZDCRgn fOldClip;
	ZPoint fOldOrigin;
	RGBColor fOldForeColor;
	RGBColor fOldBackColor;
	PenState fOldState;
	short fOldTextMode;
	short fOldTextSize;
	short fOldTextFont;
	short fOldTextFace;
	};

// =================================================================================================
// MARK: - ZDCSetupForQD

class ZDCSetupForQD
	{
public:
	ZDCSetupForQD(const ZDC& inDC, bool inUsingPatterns);
	~ZDCSetupForQD();
	ZPoint GetOffset();
	GrafPtr GetGrafPtr();
	CGrafPtr GetCGrafPtr();

protected:
	ZRef<ZDCCanvas_QD> fCanvas;
	ZPoint fOffset;
	};

// =================================================================================================
// MARK: - ZDCPixmapRep_QD

class ZDCPixmapRep_QD : public ZDCPixmapRep
	{
public:
	ZDCPixmapRep_QD(ZRef<ZDCPixmapRaster> inRaster,
								const ZRect& inBounds,
								const ZDCPixmapNS::PixelDesc& inPixelDesc);

	ZDCPixmapRep_QD(const PixMap* inSourcePixMap, const void* inSourceData, const ZRect& inSourceBounds);
	ZDCPixmapRep_QD(const BitMap* inSourceBitMap, const void* inSourceData, const ZRect& inSourceBounds, bool inInvert);

	virtual ~ZDCPixmapRep_QD();

	const PixMap& GetPixMap();

	static ZRef<ZDCPixmapRep_QD> sAsPixmapRep_QD(ZRef<ZDCPixmapRep> inRep);

protected:
	PixMap fPixMap;
	int fChangeCount;
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(DC_QD)

#endif // __ZDC_QD_h__
