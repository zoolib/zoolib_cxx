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

#include "zoolib/ZDC_ZooLib.h"
#include "zoolib/ZDCPixmapBlit.h"
#include "zoolib/ZUnicode.h"

namespace ZooLib {

using ZDCPixmapNS::EFormatEfficient;
using ZDCPixmapNS::PixelDesc;
using ZDCPixmapNS::PixvalDesc;
using ZDCPixmapNS::RasterDesc;

// =================================================================================================
// MARK: - ZDCCanvas_ZooLib

ZDCCanvas_ZooLib::ZDCCanvas_ZooLib(ZRef<ZDCPixmapRep> iPixmapRep,
	ZRef<ZDCGlyphServer> iGlyphServer)
:	fRep(iPixmapRep),
	fGlyphServer(iGlyphServer)
	{}

ZDCCanvas_ZooLib::ZDCCanvas_ZooLib(const ZDCPixmap& iPixmap,
	ZRef<ZDCGlyphServer> iGlyphServer)
:	fRep(iPixmap.GetRep()),
	fGlyphServer(iGlyphServer)
	{}

ZDCCanvas_ZooLib::ZDCCanvas_ZooLib(ZPoint iSize, ZDCPixmapNS::EFormatEfficient iFormat,
	ZRef<ZDCGlyphServer> iGlyphServer)
:	fRep(ZDCPixmap(iSize, iFormat).GetRep()),
	fGlyphServer(iGlyphServer)
	{}

ZDCCanvas_ZooLib::ZDCCanvas_ZooLib(ZPoint iSize, ZDCPixmapNS::EFormatStandard iFormat,
	ZRef<ZDCGlyphServer> iGlyphServer)
:	fRep(ZDCPixmap(iSize, iFormat).GetRep()),
	fGlyphServer(iGlyphServer)
	{}

ZDCCanvas_ZooLib::~ZDCCanvas_ZooLib()
	{}

void ZDCCanvas_ZooLib::Pixel(ZDCState& ioState,
	ZCoord iLocationH, ZCoord iLocationV, const ZRGBColor& iColor)
	{
	fRep->SetPixel(iLocationH + ioState.fOrigin.h, iLocationV + ioState.fOrigin.v, iColor);
	}

void ZDCCanvas_ZooLib::Line(ZDCState& ioState,
	ZCoord iStartH, ZCoord iStartV, ZCoord iEndH, ZCoord iEndV)
	{
	if (ioState.fPenWidth <= 0)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	iStartH += ioState.fOrigin.h;
	iStartV += ioState.fOrigin.v;
	iEndH += ioState.fOrigin.h;
	iEndV += ioState.fOrigin.v;

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	
	if (iStartH == iEndH)
		{
		// Vertical line
		ZRect theRect;
		theRect.left = iStartH;
		theRect.right = iEndH + ioState.fPenWidth;
		if (iStartV < iEndV)
			{
			theRect.top = iStartV;
			theRect.bottom = iEndV + ioState.fPenWidth;
			}
		else
			{
			theRect.top = iEndV;
			theRect.bottom = iStartV + ioState.fPenWidth;
			}
		this->pFillRegion(theClip & theRect, theRep, ioState.fPatternOrigin, ioState.fMode);
		}
	else if (iStartV == iEndV)
		{
		// Horizontal line
		ZRect theRect;
		theRect.top = iStartV;
		theRect.bottom = iStartV + ioState.fPenWidth;
		if (iStartH < iEndH)
			{
			theRect.left = iStartH;
			theRect.right = iEndH + ioState.fPenWidth;
			}
		else
			{
			theRect.left = iEndH;
			theRect.right = iStartH + ioState.fPenWidth;
			}
		this->pFillRegion(theClip & theRect, theRep, ioState.fPatternOrigin, ioState.fMode);
		}
	else
		{
		ZDCRgn theRgn = ZDCRgn::sLine(iStartH, iStartV, iEndH, iEndV, ioState.fPenWidth);
		this->pFillRegion(theClip & theRgn, theRep, ioState.fPatternOrigin, ioState.fMode);
		}
	}

void ZDCCanvas_ZooLib::FrameRect(ZDCState& ioState, const ZRect& iRect)
	{
	if (iRect.IsEmpty())
		return;

	if (ioState.fPenWidth <= 0)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZRect realRect = iRect + ioState.fOrigin;
	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	if (realRect.Width() <= ioState.fPenWidth * 2 || realRect.Height() <= ioState.fPenWidth * 2)
		{
		this->pFillRegion(theClip & realRect, theRep, ioState.fPatternOrigin, ioState.fMode);
		}
	else
		{
		ZRect theBounds;
		// Top
		theBounds.left = realRect.left;
		theBounds.top = realRect.top;
		theBounds.right = realRect.right;
		theBounds.bottom = realRect.top + ioState.fPenWidth;
		this->pFillRegion(theClip & theBounds,
						theRep, ioState.fPatternOrigin, ioState.fMode);
		// Bottom
		theBounds.top = realRect.bottom - ioState.fPenWidth;
		theBounds.bottom = realRect.bottom;
		this->pFillRegion(theClip & theBounds,
						theRep, ioState.fPatternOrigin, ioState.fMode);
		// Left
		theBounds.top = realRect.top + ioState.fPenWidth;
		theBounds.right = realRect.left + ioState.fPenWidth;
		theBounds.bottom -= ioState.fPenWidth;
		this->pFillRegion(theClip & theBounds,
						theRep, ioState.fPatternOrigin, ioState.fMode);
		// Right
		theBounds.left = realRect.right - ioState.fPenWidth;
		theBounds.right = realRect.right;
		this->pFillRegion(theClip & theBounds,
						theRep, ioState.fPatternOrigin, ioState.fMode);
		}
	}

void ZDCCanvas_ZooLib::FillRect(ZDCState& ioState, const ZRect& iRect)
	{
	if (iRect.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZRect realRect = iRect + ioState.fOrigin;

	ZDCRgn theClip = ioState.fClip + (ioState.fClipOrigin - ioState.fOriginComp);
	this->pFillRegion(theClip & realRect, theRep,
					ioState.fPatternOrigin + ioState.fOriginComp,
					ioState.fMode);
	}

void ZDCCanvas_ZooLib::InvertRect(ZDCState& ioState, const ZRect& iRect)
	{
	if (iRect.IsEmpty())
		return;

	ZRect realRect = iRect + ioState.fOrigin;

	ZDCRgn theClip = ioState.fClip + (ioState.fClipOrigin - ioState.fOriginComp);
	this->pInvertRegion(theClip & realRect);
	}

void ZDCCanvas_ZooLib::FrameRegion(ZDCState& ioState, const ZDCRgn& iRgn)
	{
	if (ioState.fPenWidth <= 0)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZDCRgn realRgn = iRgn + ioState.fOrigin;
	realRgn -= realRgn.Inset(ioState.fPenWidth, ioState.fPenWidth);

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pFillRegion(theClip & realRgn, theRep,
					ioState.fPatternOrigin, ioState.fMode);
	}

void ZDCCanvas_ZooLib::FillRegion(ZDCState& ioState, const ZDCRgn& iRgn)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZDCRgn realRgn = iRgn + ioState.fOrigin;

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pFillRegion(theClip & realRgn, theRep,
					ioState.fPatternOrigin, ioState.fMode);
	}

void ZDCCanvas_ZooLib::InvertRegion(ZDCState& ioState, const ZDCRgn& iRgn)
	{
	ZDCRgn realRgn = iRgn + ioState.fOrigin;
	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pInvertRegion(theClip & realRgn);
	}

void ZDCCanvas_ZooLib::FrameRoundRect(ZDCState& ioState,
	const ZRect& iRect, const ZPoint& iCornerSize)
	{
	if (ioState.fPenWidth <= 0)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZRect realRect = iRect + ioState.fOrigin;
	ZDCRgn realRgn = ZDCRgn::sRoundRect(realRect, iCornerSize);

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pFillRegion(theClip & realRect, theRep,
					ioState.fPatternOrigin, ioState.fMode);
	}

void ZDCCanvas_ZooLib::FillRoundRect(ZDCState& ioState,
	const ZRect& iRect, const ZPoint& iCornerSize)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZRect realRect = iRect + ioState.fOrigin;
	ZDCRgn realRgn = ZDCRgn::sRoundRect(realRect, iCornerSize);

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pFillRegion(theClip & realRgn, theRep,
					ioState.fPatternOrigin, ioState.fMode);
	}

void ZDCCanvas_ZooLib::InvertRoundRect(ZDCState& ioState,
	const ZRect& iRect, const ZPoint& iCornerSize)
	{
	ZRect realRect = iRect + ioState.fOrigin;
	ZDCRgn realRgn = ZDCRgn::sRoundRect(realRect, iCornerSize);

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pInvertRegion(theClip & realRgn);
	}

void ZDCCanvas_ZooLib::FrameEllipse(ZDCState& ioState, const ZRect& iBounds)
	{
	if (ioState.fPenWidth <= 0)
		return;

	if (iBounds.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZRect realBounds = iBounds + ioState.fOrigin;
	ZDCRgn realRgn = ZDCRgn::sEllipse(realBounds)
		- ZDCRgn::sEllipse(realBounds.Inset(ioState.fPenWidth, ioState.fPenWidth));

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pFillRegion(theClip & realRgn, theRep,
		ioState.fPatternOrigin, ioState.fMode);
	}

void ZDCCanvas_ZooLib::FillEllipse(ZDCState& ioState, const ZRect& iBounds)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZRect realBounds = iBounds + ioState.fOrigin;
	ZDCRgn realRgn = ZDCRgn::sEllipse(realBounds);

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pFillRegion(theClip & realRgn, theRep,
		ioState.fPatternOrigin, ioState.fMode);
	}

void ZDCCanvas_ZooLib::InvertEllipse(ZDCState& ioState, const ZRect& iBounds)
	{
	ZDCRgn realRgn = ZDCRgn::sEllipse(iBounds + ioState.fOrigin);
	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pInvertRegion(theClip & realRgn);
	}

void ZDCCanvas_ZooLib::FillPoly(ZDCState& ioState, const ZDCPoly& iPoly)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZDCRgn realRgn = ZDCRgn::sPoly(iPoly) + ioState.fOrigin;

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pFillRegion(theClip & realRgn, theRep,
		ioState.fPatternOrigin, ioState.fMode);
	}

void ZDCCanvas_ZooLib::InvertPoly(ZDCState& ioState, const ZDCPoly& iPoly)
	{
	ZDCRgn realRgn = ZDCRgn::sPoly(iPoly) + ioState.fOrigin;

	ZDCRgn theClip = ioState.fClip + ioState.fClipOrigin;
	this->pInvertRegion(theClip & realRgn);
	}

void ZDCCanvas_ZooLib::DrawPixmap(ZDCState& ioState,
	const ZPoint& iLocation, const ZDCPixmap& iSourcePixmap, const ZDCPixmap* iMaskPixmap)
	{	
	}

ZDCPixmap ZDCCanvas_ZooLib::GetPixmap(ZDCState& ioState, const ZRect& inBounds)
	{
	if (fRep)
		{
		ZRect realBounds = (inBounds + ioState.fOrigin) & fRep->GetBounds();
		if (!realBounds.IsEmpty())
			{
			return ZDCPixmapRep::sCreate(
				fRep->GetRaster(), realBounds, fRep->GetPixelDesc());
			}
		}

	return ZDCPixmap();
	}

void ZDCCanvas_ZooLib::Scroll(ZDCState& ioState, const ZRect& inRect,
	ZCoord inDeltaH, ZCoord inDeltaV)
	{
	}

void ZDCCanvas_ZooLib::CopyFrom(ZDCState& ioState, const ZPoint& iLocation,
	ZRef<ZDCCanvas> iSourceCanvas, const ZDCState& iSourceState, const ZRect& iSourceRect)
	{
	}

ZDCPixmapBlit::EOp sModeToOp[] =
	{
	ZDCPixmapBlit::eOp_Copy, // modeCopy
	ZDCPixmapBlit::eOp_Over, // modeOr
	ZDCPixmapBlit::eOp_Copy  // modeXor
	};

void ZDCCanvas_ZooLib::DrawText(ZDCState& ioState, const ZPoint& iLocation,
	const char* iText, size_t iTextLength)
	{
	if (!fGlyphServer)
		return;

	ZPoint theLocation = iLocation;
	const UTF8* source = iText;
	const UTF8* sourceEnd = iText + iTextLength;
	for (;;)
		{
		UTF32 theCP;
		if (!ZUnicode::sReadInc(source, sourceEnd, theCP))
			break;

		ZDCPixmap thePixmap;
		ZPoint origin;
		ZCoord escapement;
		if (fGlyphServer->GetGlyph(ioState.fFont, theCP, origin, escapement, thePixmap))
			{
			ZRect matteBounds = thePixmap.GetBounds();
			ZRect destBounds = matteBounds.Size();
			ZRect repBounds = fRep->GetBounds();
			destBounds += theLocation;
			destBounds += repBounds.TopLeft();
			destBounds -= origin;
			destBounds &= repBounds;
			if (!destBounds.IsEmpty())
				{
				ZDCPixmapBlit::sColor(
					thePixmap.GetBaseAddress(),
					thePixmap.GetRasterDesc(),
					matteBounds,
					thePixmap.GetPixelDesc(),
					fRep->GetRaster()->GetBaseAddress(),
					fRep->GetRaster()->GetRasterDesc(),
					destBounds,
					fRep->GetPixelDesc(),
					ioState.fTextColor,
					ZDCPixmapBlit::eOp_Over);
				}
			theLocation.h += escapement;
			}
		}
	}

ZCoord ZDCCanvas_ZooLib::GetTextWidth(ZDCState& ioState, const char* iText, size_t iTextLength)
	{
	ZCoord theWidth = 0;
	const UTF8* source = iText;
	const UTF8* sourceEnd = iText + iTextLength;
	for (;;)
		{
		UTF32 theCP;
		if (!ZUnicode::sReadInc(source, sourceEnd, theCP))
			break;
		theWidth += fGlyphServer->GetEscapement(ioState.fFont, theCP);
		}
	return theWidth;
	}

void ZDCCanvas_ZooLib::GetFontInfo(ZDCState& ioState,
	ZCoord& oAscent, ZCoord& oDescent, ZCoord& oLeading)
	{
	if (fGlyphServer)
		{
		fGlyphServer->GetFontInfo(ioState.fFont, oAscent, oDescent, oLeading);
		}
	else
		{
		oAscent = 1;
		oDescent = 1;
		oLeading = 0;
		}
	}

ZCoord ZDCCanvas_ZooLib::GetLineHeight(ZDCState& ioState)
	{
	ZCoord ascent, descent, leading;
	this->GetFontInfo(ioState, ascent, descent, leading);
	return ascent + descent;
	}

void ZDCCanvas_ZooLib::BreakLine(ZDCState& ioState, const char* iLineStart,
	size_t iRemainingTextLength, ZCoord iTargetWidth, size_t& oNextLineDelta)
	{
	// I'm not bothering with proper text breaking for now.
	oNextLineDelta = iRemainingTextLength;
	}

void ZDCCanvas_ZooLib::InvalCache()
	{}

void ZDCCanvas_ZooLib::Sync()
	{}

void ZDCCanvas_ZooLib::Flush()
	{}

short ZDCCanvas_ZooLib::GetDepth()
	{
	return 8;
	}

bool ZDCCanvas_ZooLib::IsOffScreen()
	{ return true; }

bool ZDCCanvas_ZooLib::IsPrinting()
	{ return false; }

ZRef<ZDCCanvas> ZDCCanvas_ZooLib::CreateOffScreen(const ZRect& iCanvasRect)
	{
	ZPoint theSize = iCanvasRect.Size();
	const RasterDesc& myRasterDesc = fRep->GetRaster()->GetRasterDesc();

	RasterDesc theRasterDesc(myRasterDesc.fPixvalDesc,
		ZDCPixmapNS::sCalcRowBytes(theSize.h, myRasterDesc.fPixvalDesc.fDepth, 4),
		theSize.v, false);

	ZRef<ZDCPixmapRep> theRep
		= ZDCPixmapRep::sCreate(theRasterDesc, theSize, fRep->GetPixelDesc());

	return new ZDCCanvas_ZooLib(theRep, fGlyphServer);
	}

ZRef<ZDCCanvas> ZDCCanvas_ZooLib::CreateOffScreen(ZPoint iDimensions, EFormatEfficient iFormat)
	{
	return new ZDCCanvas_ZooLib(iDimensions, iFormat, fGlyphServer);
	}

// =================================================================================================

#define ALLONES ZUINT64_C(0xFFFFFFFFFFFFFFFF)

void ZDCCanvas_ZooLib::pFillRegion(const ZDCRgn& iRgn,
	const ZRef<ZDCInk::Rep>& iRep, ZPoint iPatternOrigin, short iMode)
	{
	ZDCRgn realRgn = iRgn & fRep->GetBounds();
	if (iRep->fType == ZDCInk::eTypeSolidColor)
		{
		this->pFillRegion(realRgn, iRep->fAsSolidColor.fColor, iMode);
		}
	else if (iRep->fType == ZDCInk::eTypeTwoColor)
		{
		if ((*(uint64*)(iRep->fAsTwoColor.fPattern.pat)) == ALLONES)
			{
			this->pFillRegion(realRgn, iRep->fAsTwoColor.fForeColor, iMode);
			}
		else if ((*(uint64*)(iRep->fAsTwoColor.fPattern.pat)) == 0)
			{
			this->pFillRegion(realRgn, iRep->fAsTwoColor.fBackColor, iMode);
			}
		else
			{
			this->pFillRegion(realRgn, iRep->fAsTwoColor.fForeColor,
							iRep->fAsTwoColor.fBackColor,
							iRep->fAsTwoColor.fPattern,
							iPatternOrigin, iMode);
			}
		}
	else if (iRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->pFillRegion(realRgn, *iRep->fAsMultiColor.fPixmap, iPatternOrigin, iMode);
		}
	}

struct DrawSolidColor_t
	{
	void* fAddress;
	RasterDesc fRasterDesc;
	PixelDesc fPixelDesc;

	ZPoint fOffset;

	ZRGBColor fColor;

	ZDCPixmapBlit::EOp fOp;
	};

static bool spDraw_SolidColor(const ZRect& iRect, void* iRefcon)
	{
	DrawSolidColor_t* theInfo = static_cast<DrawSolidColor_t*>(iRefcon);
	ZDCPixmapBlit::sColor(
		theInfo->fAddress, theInfo->fRasterDesc, iRect + theInfo->fOffset, theInfo->fPixelDesc,
		theInfo->fColor, theInfo->fOp);
	return false;
	}

void ZDCCanvas_ZooLib::pFillRegion(const ZDCRgn& iRgn, const ZRGBColorPOD& iColor, short iMode)
	{
	DrawSolidColor_t theInfo;
	theInfo.fAddress = fRep->GetRaster()->GetBaseAddress();
	theInfo.fRasterDesc = fRep->GetRaster()->GetRasterDesc();
	theInfo.fPixelDesc = fRep->GetPixelDesc();
	theInfo.fOffset = fRep->GetBounds().TopLeft();
	theInfo.fColor = iColor;
	theInfo.fOp = sModeToOp[iMode];
	(iRgn & fRep->GetBounds()).Decompose(spDraw_SolidColor, &theInfo);
	}

struct DrawPixmap_t
	{
	const void* fSourceAddress;
	RasterDesc fSourceRasterDesc;
	ZRect fSourceBounds;
	PixelDesc fSourcePixelDesc;

	void* fDestAddress;
	RasterDesc fDestRasterDesc;
	PixelDesc fDestPixelDesc;

	ZPoint fOffset;
	ZPoint fPatternOffset;

	ZDCPixmapBlit::EOp fOp;
	};

static bool spDraw_Pixmap(const ZRect& iRect, void* iRefcon)
	{
	DrawPixmap_t* theInfo = static_cast<DrawPixmap_t*>(iRefcon);
	
	ZDCPixmapBlit::sBlit(theInfo->fSourceAddress,
		theInfo->fSourceRasterDesc,
		theInfo->fSourceBounds,
		theInfo->fSourcePixelDesc,
		iRect.TopLeft() + theInfo->fPatternOffset,
		theInfo->fDestAddress,
		theInfo->fDestRasterDesc,
		iRect + theInfo->fOffset,
		theInfo->fDestPixelDesc,
		theInfo->fOp);

	return false;
	}

void ZDCCanvas_ZooLib::pFillRegion(const ZDCRgn& iRgn,
	const ZRGBColorPOD& iForeColor, const ZRGBColorPOD& iBackColor, const ZDCPattern& iPattern,
	ZPoint iPatternOrigin, short iMode)
	{
	DrawPixmap_t theInfo;
	theInfo.fSourceAddress = iPattern.pat;
	theInfo.fSourceRasterDesc = RasterDesc(PixvalDesc(1, true), 1, 8, false);
	theInfo.fSourceBounds = ZRect(8, 8);

	ZRGBColorPOD theColors[2];
	theColors[0] = iBackColor;
	theColors[1] = iForeColor;
	theInfo.fSourcePixelDesc = PixelDesc(theColors, 2);

	theInfo.fDestAddress = fRep->GetRaster()->GetBaseAddress();
	theInfo.fDestRasterDesc = fRep->GetRaster()->GetRasterDesc();
	theInfo.fDestPixelDesc = fRep->GetPixelDesc();

	theInfo.fOffset = fRep->GetBounds().TopLeft();

	theInfo.fPatternOffset = iPatternOrigin + theInfo.fOffset;

	theInfo.fOp = sModeToOp[iMode];

	(iRgn & fRep->GetBounds()).Decompose(spDraw_Pixmap, &theInfo);
	}

void ZDCCanvas_ZooLib::pFillRegion(const ZDCRgn& iRgn,
	const ZDCPixmap& iPixmap, ZPoint iPatternOrigin, short iMode)
	{
	DrawPixmap_t theInfo;
	theInfo.fSourceAddress = iPixmap.GetBaseAddress();
	theInfo.fSourceRasterDesc = iPixmap.GetRasterDesc();
	theInfo.fSourcePixelDesc = iPixmap.GetPixelDesc();
	theInfo.fSourceBounds = iPixmap.GetBounds();

	theInfo.fDestAddress = fRep->GetRaster()->GetBaseAddress();
	theInfo.fDestRasterDesc = fRep->GetRaster()->GetRasterDesc();
	theInfo.fDestPixelDesc = fRep->GetPixelDesc();

	theInfo.fOffset = fRep->GetBounds().TopLeft();
	theInfo.fPatternOffset = iPatternOrigin + theInfo.fOffset;

	theInfo.fOp = sModeToOp[iMode];

	(iRgn & fRep->GetBounds()).Decompose(spDraw_Pixmap, &theInfo);
	}

struct Invert_t
	{
	void* fAddress;
	RasterDesc fRasterDesc;
	PixelDesc fPixelDesc;

	ZPoint fOffset;
	};

static bool spInvert(const ZRect& iRect, void* iRefcon)
	{
	Invert_t* theInfo = static_cast<Invert_t*>(iRefcon);
	ZDCPixmapBlit::sInvert(theInfo->fAddress, theInfo->fRasterDesc,
		iRect + theInfo->fOffset, theInfo->fPixelDesc);
	return false;
	}

void ZDCCanvas_ZooLib::pInvertRegion(const ZDCRgn& iRgn)
	{
	Invert_t theInfo;
	theInfo.fAddress = fRep->GetRaster()->GetBaseAddress();
	theInfo.fRasterDesc = fRep->GetRaster()->GetRasterDesc();
	theInfo.fPixelDesc = fRep->GetPixelDesc();

	theInfo.fOffset = fRep->GetBounds().TopLeft();
	(iRgn & fRep->GetBounds()).Decompose(spInvert, &theInfo);
	}

// =================================================================================================
// MARK: - ZDCCanvasFactory_ZooLib

class ZDCCanvasFactory_ZooLib : public ZDCCanvasFactory
	{
public:
	virtual ZRef<ZDCCanvas> CreateCanvas(
		ZPoint iSize, bool iBigEndian, const PixelDesc& iPixelDesc);
	};

static ZDCCanvasFactory_ZooLib spsZDCCanvasFactory_ZooLib;

ZRef<ZDCCanvas> ZDCCanvasFactory_ZooLib::CreateCanvas(
	ZPoint iSize, bool iBigEndian, const PixelDesc& iPixelDesc)
	{
	return ZRef<ZDCCanvas>();
	}

} // namespace ZooLib
