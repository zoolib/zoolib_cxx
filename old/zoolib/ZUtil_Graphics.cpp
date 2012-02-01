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

#include "zoolib/ZMacFixup.h"

#include "zoolib/ZUtil_Graphics.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZDC.h"
#include "zoolib/ZDC_QD.h"

#if ZCONFIG_SPI_Enabled(MacClassic) || ZCONFIG_SPI_Enabled(Carbon)
	#include ZMACINCLUDE3(Carbon,HIToolbox,Drag.h) // For GetDragHiliteColor
	#include ZMACINCLUDE3(CoreServices,CarbonCore,LowMem.h) // For LMGetHiliteRGB
#endif

using std::vector;

namespace ZooLib {

// ==================================================

void ZUtil_Graphics::sCalculateRgnDifferences(const ZDCRgn& inOldRgn, const ZDCRgn& inNewRgn,
	ZDCRgn* outOldRgnToPaint, ZDCRgn* outNewRgnToPaint, ZDCRgn* outDiffRgnToPaint)
	{
	if (outOldRgnToPaint)
		*outOldRgnToPaint = (inOldRgn - inNewRgn);
	if (outNewRgnToPaint)
		*outNewRgnToPaint = (inNewRgn - inOldRgn);
	if (outDiffRgnToPaint)
		*outDiffRgnToPaint = (inNewRgn & inOldRgn);
	}

void ZUtil_Graphics::sCalculatePatternDifference(const ZDCPattern& inOldPattern,
	const ZDCPattern& inNewPattern, ZDCPattern* outDiffPattern, bool* outPatternChanged)
	{
	ZDCPattern temp;
	ZDCPattern* diffPattern = outDiffPattern;
	if (!diffPattern)
		diffPattern = &temp;

	bool patternChanged = false;

	for (short x = 0; x < 8; x++)
		{
		diffPattern->pat[x] = inOldPattern.pat[x] ^ inNewPattern.pat[x];
		if (diffPattern->pat[x] != 0)
			patternChanged = true;
		}

	if (outPatternChanged)
		*outPatternChanged= patternChanged;
	}

// ==================================================

void ZUtil_Graphics::sMarchAnts(ZDCPattern& ioPattern)
	{
	uint8 line0 = ioPattern.pat[0];
	ioPattern.pat[0] = ioPattern.pat[1];
	ioPattern.pat[1] = ioPattern.pat[2];
	ioPattern.pat[2] = ioPattern.pat[3];
	ioPattern.pat[3] = ioPattern.pat[4];
	ioPattern.pat[4] = ioPattern.pat[5];
	ioPattern.pat[5] = ioPattern.pat[6];
	ioPattern.pat[6] = ioPattern.pat[7];
	ioPattern.pat[7] = line0;
	}

// ==================================================

void ZUtil_Graphics::sDrawDragDropHilite(const ZDC& inDC, const ZDCRgn& inRgn)
	{
	ZDC localDC(inDC);
	ZDCRgn realRgn = inRgn - inRgn.Inset(3, 3);

	#if (ZCONFIG_SPI_Enabled(MacClassic) || ZCONFIG_SPI_Enabled(Carbon)) && ZCONFIG_SPI_Enabled(QuickDraw)

		if (localDC.GetDepth() >= 4)
			{
			RGBColor theDragHiliteColor;
			ZDCSetupForQD theSetupForQD(localDC, true);
			ZPoint qdOffset = theSetupForQD.GetOffset();
			::GetDragHiliteColor((WindowPtr)theSetupForQD.GetGrafPtr(), &theDragHiliteColor);
			RGBColor oldHiliteColor;
			LMGetHiliteRGB(&oldHiliteColor);
			::HiliteColor(&theDragHiliteColor);
			::PenMode(hilite);
			::PaintRgn((realRgn + qdOffset).GetRgnHandle());
			::HiliteColor(&oldHiliteColor);
			}
		else
			{
			localDC.SetInk(ZDCInk::sGray);
			localDC.SetMode(ZDC::modeXor);
			localDC.Fill(realRgn);
			}

	#else

		localDC.SetInk(ZDCInk::sGray);
		localDC.SetMode(ZDC::modeXor);
		localDC.Fill(realRgn);

	#endif
	}

// ==================================================

ZDCRgn ZUtil_Graphics::sCalculateRgnStrokedByLine(ZCoord inStartH, ZCoord inStartV,
	ZCoord inEndH, ZCoord inEndV, ZCoord inPenWidth)
	{ return ZDCRgn::sLine(inStartH, inStartV, inEndH, inEndV, inPenWidth); }

ZDCRgn ZUtil_Graphics::sCalculateRgnStrokedByLine(ZPoint inStart, ZPoint inEnd, ZCoord inPenWidth)
	{ return ZDCRgn::sLine(inStart.h, inStart.v, inEnd.h, inEnd.v, inPenWidth); }

// ==================================================

void ZUtil_Graphics::sBresenham(ZPoint inStartPoint, ZPoint inEndPoint,
	BresenhamProc inProc, void* inRefcon)
	{
	ZCoord x1 = inStartPoint.h;
	ZCoord x2 = inEndPoint.h;
	ZCoord y1 = inStartPoint.v;
	ZCoord y2 = inEndPoint.v;


	ZCoord dx = x2 - x1; 
	ZCoord ax = abs(int(dx)) << 1;
	ZCoord sx = dx < 0 ? -1 : dx > 0 ? 1 : 0;
	ZCoord dy = y2 - y1; 
	ZCoord ay = abs(int(dy)) << 1;
	ZCoord sy = dy < 0 ? -1 : dy > 0 ? 1 : 0;
	ZCoord x = x1;
	ZCoord y = y1;

	if (ax >= ay)
		{
		// x dominant
		ZCoord d = ay - (ax >> 1);
		while (x != x2)
			{
			inProc(ZPoint(x, y), inRefcon);
			if (d >= 0)
				{
				y += sy;
				d -= ax;
				}
			x += sx;
			d += ay;
			}
		}
	else
		{
		// y dominant
		ZCoord d = ax - (ay >> 1);
		while (y != y2)
			{
			inProc(ZPoint(x, y), inRefcon);
			if (d >= 0)
				{
				x += sx;
				d -= ay;
				}
			y += sy;
			d += ax;
			}
		}
	}

// ==================================================

ZDCRgn ZUtil_Graphics::sCalculateRgnStrokedByFatLine(const ZDCRgn& inPenRgn,
	ZPoint inStartPoint, ZPoint inEndPoint)
	{
	// rvb sept 21 1999: broke down penRgn into rects and then drew line between start and finish
	// This is now almost constant for the length of the line so the line does not take
	// longer to draw as it gets longer. It is linear in the number of rectangles that 
	// make up the penRgn. Should implement a callback for decompose but there will normally be
	// only 25 rects returned max (one for each vertical line in a penRegion if continuous)

	// AG 1999-10-30. One final change -- build a region formed from all the polys and fill that.
	// It avoids the original problem, that of unioning the pen region with the composite region
	// once for each point on the line, and now also avoids tiling one poly for each rect in the
	// pen region.
	int quadrant;
	if (inStartPoint.v > inEndPoint.v)
		{
		if (inStartPoint.h < inEndPoint.h)
			quadrant = 1;
		else
			quadrant = 2;
		}
	else
		{
		if (inStartPoint.h < inEndPoint.h)
			quadrant = 4;
		else
			quadrant = 3;
		}

	vector<ZRect> rects;
	inPenRgn.Decompose(rects);

	ZDCRgnAccumulator theAccumulator;
	ZPoint thePoints[6];
	for (size_t x = 0; x < rects.size(); ++x)
		{
		if (quadrant == 1)
			{
			thePoints[0] = rects[x].TopLeft() + inStartPoint;
			thePoints[1] = rects[x].TopLeft() + inEndPoint;
			thePoints[2] = rects[x].TopRight() + inEndPoint;
			thePoints[3] = rects[x].BottomRight() + inEndPoint;
			thePoints[4] = rects[x].BottomRight() + inStartPoint;
			thePoints[5] = rects[x].BottomLeft() + inStartPoint;
			}
		else if (quadrant == 2)
			{
			thePoints[0] = rects[x].TopRight() + inStartPoint;
			thePoints[1] = rects[x].TopRight() + inEndPoint;
			thePoints[2] = rects[x].TopLeft() + inEndPoint;
			thePoints[3] = rects[x].BottomLeft() + inEndPoint;
			thePoints[4] = rects[x].BottomLeft() + inStartPoint;
			thePoints[5] = rects[x].BottomRight() + inStartPoint;
			}
		else if (quadrant == 3)
			{
			thePoints[0] = rects[x].TopLeft() + inStartPoint;
			thePoints[1] = rects[x].TopLeft() + inEndPoint;
			thePoints[2] = rects[x].BottomLeft()	+ inEndPoint;
			thePoints[3] = rects[x].BottomRight() +inEndPoint;
			thePoints[4] = rects[x].BottomRight() + inStartPoint;
			thePoints[5] = rects[x].TopRight() + inStartPoint;
			}
		else
			{
			thePoints[0] = rects[x].TopRight() + inStartPoint;
			thePoints[1] = rects[x].TopRight() + inEndPoint;
			thePoints[2] = rects[x].BottomRight() + inEndPoint;
			thePoints[3] = rects[x].BottomLeft() + inEndPoint;
			thePoints[4] = rects[x].BottomLeft() + inStartPoint;
			thePoints[5] = rects[x].TopLeft() + inStartPoint;
			}
		ZDCRgn theRgn = ZDCRgn::sPoly(thePoints, 6);
		theAccumulator.Add(theRgn);
		}
	return theAccumulator.GetResult();
	}

void ZUtil_Graphics::sDrawFatLine(const ZDC& inDC, const ZDCRgn& inPenRgn,
	ZPoint inStartPoint, ZPoint inEndPoint)
	{
	ZDC localDC(inDC);
	if (inPenRgn.IsSimpleRect() && inPenRgn.Bounds().Width() == inPenRgn.Bounds().Height())
		{
		// If the pen is square, we can use ZDC::Line
		ZRect penBounds = inPenRgn.Bounds();
		localDC.SetPenWidth(penBounds.Width());
		localDC.Line(inStartPoint + penBounds.TopLeft(), inEndPoint + penBounds.TopLeft());
		}
	else
		{
		localDC.Fill(sCalculateRgnStrokedByFatLine(inPenRgn, inStartPoint, inEndPoint));
		}
	}

// ==================================================

// Handles are numbered 0 - 8, TL TM TR, ML MM MR, BL BM BR. (Top left, top middle etc.).
// Handle 4, which is in the center, gets skipped.
ZRect ZUtil_Graphics::sCalcHandleBounds9(const ZRect& inBounds,
	ZCoord inHandleSize, size_t inHandleIndex)
	{
	ZRect handleBounds = ZRect::sZero;
	if (inHandleIndex < 9 && inHandleIndex != 4)
		{
		// Find the horizontal portion
		switch (inHandleIndex % 3)
			{
			case 0:
				handleBounds.left = inBounds.left - inHandleSize;
				handleBounds.right = inBounds.left;
				break;
			case 1:
				handleBounds.left = inBounds.left + inBounds.Width() / 2 - (inHandleSize / 2);
				handleBounds.right = handleBounds.left + inHandleSize;
				break;
			case 2:
				handleBounds.left = inBounds.right;
				handleBounds.right = handleBounds.left + inHandleSize;
				break;
			}
		// Find the vertical portion
		switch (inHandleIndex / 3)
			{
			case 0:
				handleBounds.top = inBounds.top - inHandleSize;
				handleBounds.bottom = inBounds.top;
				break;
			case 1:
				handleBounds.top = inBounds.top + inBounds.Height() / 2 - (inHandleSize / 2);
				handleBounds.bottom = handleBounds.top + inHandleSize;
				break;
			case 2:
				handleBounds.top = inBounds.bottom;
				handleBounds.bottom = handleBounds.top + inHandleSize;
				break;
			}
		}
	return handleBounds;
	}

// ==================================================

} // namespace ZooLib
