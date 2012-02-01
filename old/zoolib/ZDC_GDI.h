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

#ifndef __ZDC_GDI_h__
#define __ZDC_GDI_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__DC_GDI
	#define ZCONFIG_API_Avail__DC_GDI ZCONFIG_SPI_Enabled(GDI)
#endif

#ifndef ZCONFIG_API_Desired__DC_GDI
	#define ZCONFIG_API_Desired__DC_GDI 1
#endif

#include "zoolib/ZDC.h"

#if ZCONFIG_API_Enabled(DC_GDI)

#include "zoolib/ZCompat_MSVCStaticLib.h"
ZMACRO_MSVCStaticLib_Reference(DC_GDI)

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZThreadOld.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCCanvas_GDI

class ZDCSetupForGDI;

class ZDCCanvas_GDI : public ZDCCanvas
	{
public:
	static ZRef<ZDCCanvas_GDI> sFindCanvasOrCreateNative(HDC iHDC);

protected:
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
	virtual void Pixel(ZDCState& ioState,
		ZCoord iLocationH, ZCoord iLocationV, const ZRGBColor& iColor);

	virtual void Line(ZDCState& ioState,
		ZCoord iStartH, ZCoord iStartV, ZCoord iEndH, ZCoord iEndV);

// Rectangle
	virtual void FrameRect(ZDCState& ioState, const ZRect& iRect);
	virtual void FillRect(ZDCState& ioState, const ZRect& iRect);
	virtual void InvertRect(ZDCState& ioState, const ZRect& iRect);

// Region
	virtual void FrameRegion(ZDCState& ioState, const ZDCRgn& iRgn);
	virtual void FillRegion(ZDCState& ioState, const ZDCRgn& iRgn);
	virtual void InvertRegion(ZDCState& ioState, const ZDCRgn& iRgn);

// Round cornered rect
	virtual void FrameRoundRect(ZDCState& ioState, const ZRect& iRect, const ZPoint& iCornerSize);
	virtual void FillRoundRect(ZDCState& ioState, const ZRect& iRect, const ZPoint& iCornerSize);
	virtual void InvertRoundRect(ZDCState& ioState, const ZRect& iRect, const ZPoint& iCornerSize);

// Ellipse
	virtual void FrameEllipse(ZDCState& ioState, const ZRect& iBounds);
	virtual void FillEllipse(ZDCState& ioState, const ZRect& iBounds);
	virtual void InvertEllipse(ZDCState& ioState, const ZRect& iBounds);

// Poly
	virtual void FillPoly(ZDCState& ioState, const ZDCPoly& iPoly);
	virtual void InvertPoly(ZDCState& ioState, const ZDCPoly& iPoly);

// Pixmap
	virtual void FillPixmap(ZDCState& ioState, const ZPoint& iLocation, const ZDCPixmap& iPixmap);
	virtual void DrawPixmap(ZDCState& ioState,
		const ZPoint& iLocation, const ZDCPixmap& iSourcePixmap, const ZDCPixmap* iMaskPixmap);

// Text
	virtual void DrawText(ZDCState& ioState,
		const ZPoint& iLocation, const char* iText, size_t iTextLength);

	virtual ZCoord GetTextWidth(ZDCState& ioState, const char* iText, size_t iTextLength);
	virtual void GetFontInfo(ZDCState& ioState,
		ZCoord& oAscent, ZCoord& oDescent, ZCoord& oLeading);
	virtual ZCoord GetLineHeight(ZDCState& ioState);
	virtual void BreakLine(ZDCState& ioState, const char* iLineStart,
		size_t iRemainingTextLength, ZCoord iTargetWidth, size_t& oNextLineDelta);

	virtual void Flush();
	virtual void Sync();

	virtual void InvalCache();

	virtual short GetDepth();
	virtual bool IsOffScreen();
	virtual bool IsPrinting();

// OffScreen
	virtual ZRef<ZDCCanvas> CreateOffScreen(const ZRect& iCanvasRect);
	virtual ZRef<ZDCCanvas> CreateOffScreen(
		ZPoint dimensions, ZDCPixmapNS::EFormatEfficient iFormat);

// ----------
// Internal or non-virtual implementation stuff
	ZPoint Internal_GDIStart(ZDCState& ioState, bool zeroOrigin);
	void Internal_GDIEnd();

	virtual ZDCRgn Internal_CalcClipRgn(const ZDCState& iState) = 0;

	HDC Internal_GetHDC() { return fHDC; }

	void Internal_TileRegion(ZDCState& ioState, const ZDCRgn& iRgn, const ZDCPixmap& iPixmap);

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
// MARK: - ZDCCanvas_GDI_NonWindow

class ZDCCanvas_GDI_NonWindow : public ZDCCanvas_GDI
	{
public:
	ZDCCanvas_GDI_NonWindow();
	virtual ~ZDCCanvas_GDI_NonWindow();

	virtual void Finalize();
	virtual void Scroll(ZDCState& ioState, const ZRect& iRect, ZCoord iDeltaH, ZCoord iDeltaV);
	virtual void CopyFrom(ZDCState& ioState, const ZPoint& iDestLocation,
		ZRef<ZDCCanvas> iSourceCanvas, const ZDCState& iSourceState, const ZRect& iSourceRect);
	virtual ZDCRgn Internal_CalcClipRgn(const ZDCState& iState);

protected:
	ZMutex fMutex;
	};

// =================================================================================================
// MARK: - ZDCCanvas_GDI_OffScreen

class ZDCCanvas_GDI_OffScreen : public ZDCCanvas_GDI_NonWindow
	{
public:
	ZDCCanvas_GDI_OffScreen(HDC iOther, const ZRect& iGlobalRect);
	ZDCCanvas_GDI_OffScreen(HDC iOther, ZPoint iDimensions, ZDCPixmapNS::EFormatEfficient iFormat);
	virtual ~ZDCCanvas_GDI_OffScreen();

	virtual ZRGBColor GetPixel(ZDCState& ioState, ZCoord iLocationH, ZCoord iLocationV);

	virtual void FloodFill(ZDCState& ioState, const ZPoint& iSeedLocation);

	virtual ZDCPixmap GetPixmap(ZDCState& ioState, const ZRect& iBounds);

	virtual bool IsOffScreen();

protected:
	HBITMAP fHBITMAP;
	};

// =================================================================================================
// MARK: - ZDCCanvas_GDI_IC

class ZDCCanvas_GDI_IC : public ZDCCanvas_GDI_NonWindow
	{
public:
	ZDCCanvas_GDI_IC();
	virtual ~ZDCCanvas_GDI_IC();
	};

// =================================================================================================
// MARK: - ZDC_NativeGDI

class ZDC_NativeGDI : public ZDC
	{
public:
	ZDC_NativeGDI(HDC iHDC);
	~ZDC_NativeGDI();

protected:
	HDC fHDC;
	int fSavedDCIndex;
	};

// =================================================================================================
// MARK: - ZDCCanvas_GDI::SetupDC

class ZDCCanvas_GDI::SetupDC
	{
public:
	SetupDC(ZDCCanvas_GDI* iCanvas, const ZDCState& iState);
	~SetupDC();

protected:
	ZDCCanvas_GDI* fCanvas;
	const ZDCState* fState;
	};

// =================================================================================================
// MARK: - ZDCCanvas_GDI::SetupDC

class ZDCSetupForGDI
	{
public:
	ZDCSetupForGDI(const ZDC& iDC, bool iZeroOrigin);
	~ZDCSetupForGDI();
	ZPoint GetOffset();
	HDC GetHDC();

protected:
	ZRef<ZDCCanvas_GDI> fCanvas;
	ZPoint fOffset;
	};

// =================================================================================================
// MARK: - ZDCPixmapRep_DIB

class ZDCPixmapRep_DIB : public ZDCPixmapRep
	{
public:
	ZDCPixmapRep_DIB(ZRef<ZDCPixmapRaster> iBuffer,
		const ZRect& iBounds,
		const ZDCPixmapNS::PixelDesc& iPixelDesc);

	ZDCPixmapRep_DIB(HDC iHDC, HBITMAP iHBITMAP, bool iForce32BPP = false);

	virtual ~ZDCPixmapRep_DIB();

	void GetDIBStuff(BITMAPINFO*& oBITMAPINFO, char*& oBits, ZRect* oBounds);

	static ZRef<ZDCPixmapRep_DIB> sAsPixmapRep_DIB(ZRef<ZDCPixmapRep> iRep);

protected:
	BITMAPINFO* fBITMAPINFO;
	int fChangeCount;
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(DC_GDI)

#endif // __ZDC_GDI_h__
