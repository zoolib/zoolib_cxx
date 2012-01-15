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

#include "zoolib/ZDC.h"

using namespace std;

namespace ZooLib {

/**
\defgroup Graphics
*/

/**
\class ZDC
\ingroup Graphics
It has value semantics itself, but references a potentially
shared canvas that can be also be drawn into by other instances of ZDC.
It also holds drawing state: ink, pen width, origin, pattern origin.
*/

// =================================================================================================
// MARK: - ZDC

ZDC::ZDC()
	{
	this->ZeroSettings();
	}

ZDC::ZDC(const ZDC& inOther)
:	fCanvas(inOther.fCanvas)
	{
	// Make sure we don't try to initialize ourselves from ourselves. Sounds dumb,
	// but I did do exactly that after a blind search and replace on some source.
	ZAssertStop(2, &inOther != this);
	this->CopySettings(inOther);
	}

ZDC::ZDC(const ZRef<ZDCCanvas>& inCanvas)
:	fCanvas(inCanvas)
	{
	this->ZeroSettings();
	}

ZDC::ZDC(const ZRef<ZDCCanvas>& inCanvas, ZPoint originComp)
:	fCanvas(inCanvas)
	{
	this->ZeroSettings();
	if (originComp.h != 0)
		{
		fState.fOriginComp.h = -originComp.h;
		fState.fOrigin.h = -originComp.h;

		fState.fPatternOrigin.h = originComp.h;

		fState.fClipOrigin.h = -originComp.h;
		}

	if (originComp.v != 0)
		{
		fState.fOriginComp.v = -originComp.v;
		fState.fOrigin.v = -originComp.v;

		fState.fPatternOrigin.v = originComp.v;

		fState.fClipOrigin.v = -originComp.v;
		}
	}

ZDC::~ZDC()
	{
	}

ZDC& ZDC::operator=(const ZDC& inOther)
	{
	if (&inOther != this)
		{
		fCanvas = inOther.fCanvas;
		this->CopySettings(inOther);
		}
	return *this;
	}

void ZDC::CopySettings(const ZDC& inOther)
	{
	fState.fChangeCount_Origin = inOther.fState.fChangeCount_Origin;
	fState.fChangeCount_PatternOrigin = inOther.fState.fChangeCount_PatternOrigin;
	fState.fChangeCount_Ink = inOther.fState.fChangeCount_Ink;
	fState.fChangeCount_PenWidth = inOther.fState.fChangeCount_PenWidth;
	fState.fChangeCount_Mode = inOther.fState.fChangeCount_Mode;
	fState.fChangeCount_Clip = inOther.fState.fChangeCount_Clip;
	fState.fChangeCount_Font = inOther.fState.fChangeCount_Font;
	fState.fChangeCount_TextColor = inOther.fState.fChangeCount_TextColor;

	fState.fOriginComp = inOther.fState.fOriginComp;
	fState.fOrigin = inOther.fState.fOrigin;
	fState.fPatternOrigin = inOther.fState.fPatternOrigin;
	fState.fInk = inOther.fState.fInk;
	fState.fPenWidth = inOther.fState.fPenWidth;
	fState.fMode = inOther.fState.fMode;
	fState.fClip = inOther.fState.fClip;
	fState.fClipOrigin = inOther.fState.fClipOrigin;
	fState.fFont = inOther.fState.fFont;
	fState.fTextColor = inOther.fState.fTextColor;
	}

void ZDC::ZeroSettings()
	{
	fState.fChangeCount_Origin = 0;
	fState.fChangeCount_PatternOrigin = 0;
	fState.fChangeCount_Ink = 0;
	fState.fChangeCount_PenWidth = 0;
	fState.fChangeCount_Mode = 0;
	fState.fChangeCount_Clip = 0;
	fState.fChangeCount_Font = 0;
	fState.fChangeCount_TextColor = 0;

	// AG 2000-07-17. We don't reference other classes' static members, because
	// we may be being called from a static ZDC constructor ourselves.
	fState.fOriginComp.h = 0;
	fState.fOriginComp.v = 0;
	fState.fOrigin.h = 0;
	fState.fOrigin.v = 0;
	fState.fPatternOrigin.h = 0;
	fState.fPatternOrigin.v = 0;
	fState.fInk = ZDCInk(ZRGBColor(0));
	fState.fPenWidth = 1;
	fState.fMode = modeCopy;
	fState.fClipOrigin.h = 0;
	fState.fClipOrigin.v = 0;
	// AG 98-07-15. Default is gonna be whatever ZDCFont's default is.
	// fState.fFont = ZDCFont();
	fState.fTextColor.red = 0;
	fState.fTextColor.green = 0;
	fState.fTextColor.blue = 0;
	fState.fTextColor.alpha = 0xFFFFU;
	}

void ZDC::ZeroChangeCounts()
	{
	fState.fChangeCount_Origin = 0;
	fState.fChangeCount_PatternOrigin = 0;
	fState.fChangeCount_Ink = 0;
	fState.fChangeCount_PenWidth = 0;
	fState.fChangeCount_Mode = 0;
	fState.fChangeCount_Clip = 0;
	fState.fChangeCount_Font = 0;
	fState.fChangeCount_TextColor = 0;
	}

void ZDC::PenNormal()
	{
	this->SetInk(ZRGBColor::sBlack);
	this->SetMode(modeCopy);
	this->SetPenWidth(1);
	}

void ZDC::SetInk(const ZDCInk& theInk)
	{
	if (fState.fInk.IsSameAs(theInk))
		return;
	fState.fInk = theInk;
	--fState.fChangeCount_Ink;
	}

void ZDC::SetPenWidth(const ZCoord& theWidth)
	{
	if (fState.fPenWidth == theWidth)
		return;
	fState.fPenWidth = theWidth;
	--fState.fChangeCount_PenWidth;
	}

void ZDC::SetMode(short theMode)
	{
	if (fState.fMode == theMode)
		return;
	fState.fMode = theMode;
	--fState.fChangeCount_Mode;
	}

void ZDC::SetFont(const ZDCFont& theFont)
	{
	fState.fFont = theFont;
	--fState.fChangeCount_Font;
	}

void ZDC::SetTextColor(const ZRGBColor& textColor)
	{
	if (fState.fTextColor == textColor)
		return;
	fState.fTextColor = textColor;
	--fState.fChangeCount_TextColor;
	}

void ZDC::SetClip(const ZDCRgn& theRgn)
	{
	// Take a shortcut if possible
	// AG 99-09-20. Don't do the shortcut -- checking for equivalence will be slower
	// than just setting the clip, and will generally fail anyway.
	//	if ((fState.fClipOrigin == fState.fOrigin) && (fState.fClip == theRgn))
	//		return;
	fState.fClip = theRgn;
	fState.fClipOrigin = fState.fOrigin;
	--fState.fChangeCount_Clip;
	}

ZDCRgn ZDC::GetClip() const
	{
	// The clip is *returned* relative to the current origin, but is not shifted as the origin
	// shifts, so it always represents the same set of physical pixels, until explicitly changed.
	return fState.fClip + (fState.fClipOrigin - fState.fOrigin);
	}

void ZDC::SetOrigin(const ZPoint& newOrigin)
	{
	fState.fOrigin = newOrigin - fState.fOriginComp;
	--fState.fChangeCount_Origin;
	}
void ZDC::OffsetOrigin(const ZPoint& delta)
	{
	if (delta.h == 0 && delta.v == 0)
		return;
	fState.fOrigin += delta;
	--fState.fChangeCount_Origin;
	}

void ZDC::SetPatternOrigin(const ZPoint& newPatternOrigin)
	{
	fState.fPatternOrigin = newPatternOrigin + fState.fOriginComp;
	++fState.fChangeCount_PatternOrigin;
	}
void ZDC::OffsetPatternOrigin(const ZPoint& delta)
	{
	if (delta.h == 0 && delta.v == 0)
		return;
	fState.fPatternOrigin += delta;
	++fState.fChangeCount_PatternOrigin;
	}

// =================================================================================================
// MARK: - ZDCCanvas

ZDCCanvas::ZDCCanvas()
	{}

ZDCCanvas::~ZDCCanvas()
	{}

ZRGBColor ZDCCanvas::GetPixel(ZDCState& ioState, ZCoord inLocationH, ZCoord inLocationV)
	{
	// By default ZDCCanvases have no backing store. ZDCPixmap defines GetPixel called
	// outside its bounds as returning black, so we do the same.
	return ZRGBColor::sBlack;
	}

void ZDCCanvas::Pixels(ZDCState& ioState,
	ZCoord inLocationH, ZCoord inLocationV,
	ZCoord inWidth, ZCoord inHeight,
	const char* inPixvals, const ZRGBColorMap* inColorMap, size_t inColorMapSize)
	{
	vector<ZRGBColorPOD> normalColors(256);
	vector<ZRGBColorPOD> maskColors(256);
	// Set everything to opaque black.
	for (size_t x = 0; x < 256; ++x)
		{
		normalColors[x].red = normalColors[x].green = normalColors[x].blue = 0;
		normalColors[x].alpha = 0xFFFFU;
		maskColors[x] = normalColors[x];
		}

	// Fill out those indices for which we have entries in inColorMap
	for (size_t x = 0; x < inColorMapSize; ++x)
		{
		// For the 'normal' pixmap we copy the color as is.
		normalColors[size_t(inColorMap[x].fPixval)] = inColorMap[x].fColor;
		// For the mask pixmap we smear the color's alpha channel across all four channels.
		uint16 alpha = inColorMap[x].fColor.alpha;
		maskColors[size_t(inColorMap[x].fPixval)] = ZRGBColor(alpha, alpha, alpha, alpha);
		}

	ZDCPixmapNS::RasterDesc theRasterDesc;
	theRasterDesc.fPixvalDesc.fDepth = 8;
	theRasterDesc.fPixvalDesc.fBigEndian = true;
	theRasterDesc.fRowBytes = inWidth;
	theRasterDesc.fRowCount = inHeight;
	theRasterDesc.fFlipped = false;
	ZRef<ZDCPixmapRaster> theRaster = new ZDCPixmapRaster_StaticData(inPixvals, theRasterDesc);

	ZDCPixmap maskPixmap = ZDCPixmapRep::sCreate(theRaster, ZPoint(inWidth, inHeight),
		ZDCPixmapNS::PixelDesc(&maskColors[0], 256));

	ZDCPixmap mainPixmap = ZDCPixmapRep::sCreate(theRaster, ZPoint(inWidth, inHeight),
		ZDCPixmapNS::PixelDesc(&normalColors[0], 256));

	this->DrawPixmap(ioState, ZPoint(inLocationH, inLocationV), mainPixmap, &maskPixmap);
	}

void ZDCCanvas::HiliteRect(ZDCState& ioState, const ZRect& inRect, const ZRGBColor& inBackColor)
	{ this->InvertRect(ioState, inRect); }

void ZDCCanvas::HiliteRegion(ZDCState& ioState, const ZDCRgn& inRgn, const ZRGBColor& inBackColor)
	{ this->InvertRegion(ioState, inRgn); }

void ZDCCanvas::HiliteRoundRect(ZDCState& ioState,
	const ZRect& inRect, const ZPoint& inCornerSize, const ZRGBColor& inBackColor)
	{ this->InvertRoundRect(ioState, inRect, inCornerSize); }

void ZDCCanvas::HiliteEllipse(ZDCState& ioState,
	const ZRect& inBounds, const ZRGBColor& inBackColor)
	{ this->InvertEllipse(ioState, inBounds); }

void ZDCCanvas::HilitePoly(ZDCState& ioState, const ZDCPoly& inPoly, const ZRGBColor& inBackColor)
	{ this->InvertPoly(ioState, inPoly); }

void ZDCCanvas::FillPixmap(ZDCState& ioState, const ZPoint& inLocation, const ZDCPixmap& inPixmap)
	{
	// This is here temporarily to allow ZDCanvas_X and ZDCCanvas_Be to be usable until
	// I update them with an implementation of this method.
	ZUnimplemented();
	}

void ZDCCanvas::DrawPixmap(ZDCState& ioState,
	const ZPoint& inLocation, const ZDCPixmap& inPixmap, bool inUseAlpha)
	{
	ZUnimplemented();
	}

ZDCPixmap ZDCCanvas::GetPixmap(ZDCState& ioState, const ZRect& inBounds)
	{ return ZDCPixmap(); }

static ZDCRgn spCalcFloodFillRgn(const ZRef<ZDCCanvas>& inCanvas,
	ZDCState& ioState, ZPoint inSeedLocation);

void ZDCCanvas::FloodFill(ZDCState& ioState, const ZPoint& inSeedLocation)
	{
	ZDCRgn theMaskRgn = spCalcFloodFillRgn(this, ioState, inSeedLocation);
	this->FillRegion(ioState, theMaskRgn);
	}

void ZDCCanvas::Sync()
	{}

void ZDCCanvas::Flush()
	{}

ZRef<ZDCCanvas> ZDCCanvas::CreateOffScreen(const ZRect& iCanvasBounds)
	{ return ZRef<ZDCCanvas>(); }

// =================================================================================================
#pragma mark -
#pragma mark Flood fill generic implementation

static bool spCheckPixel(const ZRef<ZDCCanvas>& inCanvas, ZDCState& ioState,
	ZCoord inLocationH, ZCoord inLocationV, const ZRGBColor& inSeedColor, const ZDCRgn& inMaskRgn)
	{
	if (inMaskRgn.Contains(inLocationH, inLocationV))
		{
		// We've already visited this pixel.
		return false;
		}

	if (inCanvas->GetPixel(ioState, inLocationH, inLocationV) != inSeedColor)
		{
		// This pixel is not the seed color.
		return false;
		}

	return true;
	}

static ZDCRgn spCalcFloodFillRgn(const ZRef<ZDCCanvas>& inCanvas,
	ZDCState& ioState, ZPoint inSeedLocation)
	{
	ZDCRgn theMaskRgn;

	ZRGBColor theSeedColor = inCanvas->GetPixel(ioState, inSeedLocation.h, inSeedLocation.v);
	ZRect theBounds = ioState.fClip.Bounds() + (ioState.fClipOrigin - ioState.fOrigin);

	vector<ZPoint> theStack;
	theStack.push_back(inSeedLocation);

	while (theStack.size())
		{
		ZPoint theLocation = theStack.back();
		theStack.pop_back();
		if (spCheckPixel(inCanvas, ioState, theLocation.h, theLocation.v, theSeedColor, theMaskRgn))
			{
			// Find left and right boundaries
			ZCoord left = theLocation.h;
			while ((left > theBounds.left)
				&& spCheckPixel(inCanvas, ioState,
				left - 1, theLocation.v, theSeedColor, theMaskRgn))
				{
				--left;
				}

			ZCoord right = theLocation.h;
			while ((right < theBounds.right)
				&& spCheckPixel(inCanvas, ioState,
				right, theLocation.v, theSeedColor, theMaskRgn))
				{
				++right;
				}

			theMaskRgn |= ZRect(left, theLocation.v, right, theLocation.v + 1);

			// Do the line above
			ZCoord currentY = theLocation.v - 1;
			ZCoord currentX;
			if (currentY >= theBounds.top)
				{
				currentX = left;
				while (currentX < right)
					{
					while ((currentX < right)
						&& !spCheckPixel(inCanvas, ioState,
						currentX, currentY, theSeedColor, theMaskRgn))
						{
						++currentX;
						}

					if (currentX < right)
						{
						while ((currentX < right)
							&& spCheckPixel(inCanvas, ioState,
							currentX, currentY, theSeedColor, theMaskRgn))
							{
							++currentX;
							}

						theStack.push_back(ZPoint(currentX - 1, currentY));
						}
					}
				}

			// Do the line below
			currentY = theLocation.v + 1;
			if (currentY < theBounds.bottom)
				{
				currentX = left;
				while (currentX < right)
					{
					while ((currentX < right)
						&& !spCheckPixel(inCanvas, ioState,
						currentX, currentY, theSeedColor, theMaskRgn))
						{
						++currentX;
						}

					if (currentX < right)
						{
						while ((currentX < right)
							&& spCheckPixel(inCanvas, ioState,
							currentX, currentY, theSeedColor, theMaskRgn))
							{
							++currentX;
							}

						theStack.push_back(ZPoint(currentX - 1, currentY));
						}
					}
				}
			}
		}
	return theMaskRgn;
	}

// =================================================================================================
// MARK: - ZDCCanvasFactory

ZDCCanvasFactory* ZDCCanvasFactory::spHead = nullptr;

ZDCCanvasFactory::ZDCCanvasFactory()
	{
	fNext = spHead;
	spHead = this;
	}

ZDCCanvasFactory::~ZDCCanvasFactory()
	{
	}

ZRef<ZDCCanvas> ZDCCanvasFactory::sCreateCanvas(ZPoint iSize,
	bool iBigEndian, const ZDCPixmapNS::PixelDesc& iPixelDesc)
	{
	ZDCCanvasFactory* current = spHead;
	while (current)
		{
		if (ZRef<ZDCCanvas> theCanvas = current->CreateCanvas(iSize, iBigEndian, iPixelDesc))
			return theCanvas;
		current = current->fNext;
		}

	return ZRef<ZDCCanvas>();
	}

// =================================================================================================
// MARK: - ZDCScratch

static ZDC spScratchDC;

const ZDC& ZDCScratch::sGet()
	{
	return spScratchDC;
	}

void ZDCScratch::sSet(ZRef<ZDCCanvas> inCanvas)
	{
	spScratchDC = ZDC(inCanvas);
	}

// =================================================================================================
// MARK: - ZDC_Off

ZDC_Off::ZDC_Off(const ZDC& inOther, bool inForceOffscreen)
:	ZDC(inOther)
	{
	// Initially, we copy all settings from inOther, then we attempt to
	// create a new offscreen canvas.
	if (inForceOffscreen || !fCanvas->IsOffScreen())
		{
		ZRect canvasClipRect = this->GetClip().Bounds() + this->GetOrigin();
		ZRef<ZDCCanvas> offCanvas;
		try
			{
			// We allow the offscreen creation to fail if inForceOffscreen is false.
			offCanvas = fCanvas->CreateOffScreen(canvasClipRect);
			}
		catch (...)
			{
			if (inForceOffscreen)
				throw;
			}
		if (offCanvas && offCanvas->IsOffScreen())
			{
			fCanvas = offCanvas;
			this->ZeroChangeCounts();
			fState.fOriginComp = canvasClipRect.TopLeft();
			this->SetOrigin(inOther.GetOrigin());
			this->SetPatternOrigin(inOther.GetPatternOrigin());
			this->SetClip(inOther.GetClip());
			}
		}
	}

ZDC_Off::ZDC_Off(const ZDC& inOther, ZPoint inDimensions, ZDCPixmapNS::EFormatEfficient iFormat)
:	ZDC(inOther)
	{
	ZRef<ZDCCanvas> offCanvas = fCanvas->CreateOffScreen(inDimensions, iFormat);
	if (offCanvas && offCanvas->IsOffScreen())
		{
		fCanvas = offCanvas;
		this->ZeroChangeCounts();
		fState.fClip = ZRect(inDimensions);
		fState.fOrigin = ZPoint::sZero;
		fState.fPatternOrigin = ZPoint::sZero;
		fState.fClipOrigin = ZPoint::sZero;
		fState.fOriginComp = ZPoint::sZero;
		}
	}

// =================================================================================================
// MARK: - ZDC_OffAuto

ZDC_OffAuto::ZDC_OffAuto(const ZDC& inOther, bool inCopyFromOther)
:	ZDC_Off(inOther, false), fDest(inOther)
	{
	// If the other DC is *not* offscreen, and we *are* offscreen (ie we were created okay)
	// then copy the other's current contents.
	if (inCopyFromOther && this->IsOffScreen() && !fDest.IsOffScreen())
		{
		// We copy the destination's clip rgn to our top left corner
		ZRect sourceRect(fDest.GetClip().Bounds());
		this->CopyFrom(sourceRect.TopLeft(), fDest, sourceRect);
		}
	}

ZDC_OffAuto::~ZDC_OffAuto()
	{
	// Now we're being destroyed, blit our contents back again.
	if (this->IsOffScreen() && !fDest.IsOffScreen())
		{
		ZRect sourceRect(fDest.GetClip().Bounds());
		fDest.CopyFrom(sourceRect.TopLeft(), *this, sourceRect);
		}
	}

} // namespace ZooLib
