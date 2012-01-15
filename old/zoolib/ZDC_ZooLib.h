/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZDC_ZooLib_h__
#define __ZDC_ZooLib_h__ 1
#include "zconfig.h"

#include "zoolib/ZDC.h"
#include "zoolib/ZDCGlyphServer.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCCanvas_ZooLib

class ZDCCanvas_ZooLib : public ZDCCanvas
	{
public:
	ZDCCanvas_ZooLib(ZRef<ZDCPixmapRep> iPixmapRep, ZRef<ZDCGlyphServer> iGlyphServer);

	ZDCCanvas_ZooLib(const ZDCPixmap& iPixmap, ZRef<ZDCGlyphServer> iGlyphServer);

	ZDCCanvas_ZooLib(ZPoint iSize, ZDCPixmapNS::EFormatEfficient iFormat,
		ZRef<ZDCGlyphServer> iGlyphServer);

	ZDCCanvas_ZooLib(ZPoint iSize, ZDCPixmapNS::EFormatStandard iFormat,
		ZRef<ZDCGlyphServer> iGlyphServer);

	virtual ~ZDCCanvas_ZooLib();

protected:
// From ZDCCanvas
// Simple pixel and lines
	virtual void Pixel(ZDCState& ioState, ZCoord iLocationH, ZCoord iLocationV,
		const ZRGBColor& iColor);

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
	virtual void DrawPixmap(ZDCState& ioState,
		const ZPoint& iLocation, const ZDCPixmap& iSourcePixmap, const ZDCPixmap* iMaskPixmap);

	virtual ZDCPixmap GetPixmap(ZDCState& iState, const ZRect& iBounds);

// Moving blocks of pixels
	virtual void Scroll(ZDCState& ioState, const ZRect& iRect, ZCoord iDeltaH, ZCoord iDeltaV);
	virtual void CopyFrom(ZDCState& ioState, const ZPoint& iLocation,
		ZRef<ZDCCanvas> iSourceCanvas, const ZDCState& iSourceState, const ZRect& iSourceRect);

// Text
	virtual void DrawText(ZDCState& ioState, const ZPoint& iLocation,
		const char* iText, size_t iTextLength);

	virtual ZCoord GetTextWidth(ZDCState& ioState, const char* iText, size_t iTextLength);

	virtual void GetFontInfo(ZDCState& ioState,
		ZCoord& oAscent, ZCoord& oDescent, ZCoord& oLeading);

	virtual ZCoord GetLineHeight(ZDCState& ioState);

	virtual void BreakLine(ZDCState& ioState, const char* iLineStart, size_t iRemainingTextLength,
		ZCoord iTargetWidth, size_t& oNextLineDelta);

// Escape hatch
	virtual void InvalCache();

// Syncing
	virtual void Sync();
	virtual void Flush();

	virtual short GetDepth();
	virtual bool IsOffScreen();
	virtual bool IsPrinting();

// OffScreen
	virtual ZRef<ZDCCanvas> CreateOffScreen(const ZRect& iCanvasRect);
	virtual ZRef<ZDCCanvas> CreateOffScreen(ZPoint iDimensions,
		ZDCPixmapNS::EFormatEfficient iFormat);

private:
// ----------
// Internal or non-virtual implementation stuff
	void pFillRegion(const ZDCRgn& iRgn,
		const ZRef<ZDCInk::Rep>& iRep, ZPoint iPatternOrigin, short iMode);

	void pFillRegion(const ZDCRgn& iRgn, const ZRGBColorPOD& iColor, short iMode);

	void pFillRegion(const ZDCRgn& iRgn,
		const ZRGBColorPOD& iForeColor, const ZRGBColorPOD& iBackColor, const ZDCPattern& iPattern,
		ZPoint iPatternOrigin, short iMode);

	void pFillRegion(const ZDCRgn& iRgn,
		const ZDCPixmap& iPixmap, ZPoint iPatternOrigin, short iMode);

	void pInvertRegion(const ZDCRgn& iRgn);

	ZRef<ZDCPixmapRep> fRep;
	ZRef<ZDCGlyphServer> fGlyphServer;
	};

} // namespace ZooLib

#endif // __ZDC_ZooLib_h__
