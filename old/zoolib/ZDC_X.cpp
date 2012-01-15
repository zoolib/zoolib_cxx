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

#include "zoolib/ZDC_X.h"

#if ZCONFIG_API_Enabled(DC_X)

#include "zoolib/ZFunctionChain.h"

namespace ZooLib {

// =================================================================================================
// MARK: - kDebug

#define kDebug_X 2

// ==================================================

static short spModeLookup[] = { GXcopy, GXor, GXxor };

// =================================================================================================
// MARK: - ZDCCanvas_X::SetupLock

ZDCCanvas_X::SetupLock::SetupLock(ZDCCanvas_X* inCanvas)
	{
	fCanvas = inCanvas;
	ZAssertStop(kDebug_X, fCanvas->fMutexToCheck == nullptr || fCanvas->fMutexToCheck->IsLocked());
	fCanvas->fMutexToLock->Acquire();
	}

ZDCCanvas_X::SetupLock::~SetupLock()
	{
	fCanvas->fMutexToLock->Release();
	}

// =================================================================================================
// MARK: - ZDCCanvas_X::SetupClip

class ZDCCanvas_X::SetupClip
	{
public:
	SetupClip(ZDCCanvas_X* inCanvas, ZDCState& ioState);
	~SetupClip();

	bool IsEmpty() { return fClipIsEmpty; }

protected:
	ZDCCanvas_X* fCanvas;
	bool fClipIsEmpty;
	};

ZDCCanvas_X::SetupClip::SetupClip(ZDCCanvas_X* inCanvas, ZDCState& ioState)
	{
	fCanvas = inCanvas;

	fClipIsEmpty = false;

	if (ioState.fChangeCount_Clip != fCanvas->fChangeCount_Clip)
		{
		if (ioState.fClip.IsEmpty())
			{
			fClipIsEmpty = true;
			}
		else
			{
			if (ZDCRgn newClip = fCanvas->Internal_CalcClipRgn(ioState))
				{
				ioState.fChangeCount_Clip = ++fCanvas->fChangeCount_Clip;
				fCanvas->fXServer->SetRegion(fCanvas->fGC, newClip.GetRegion());
				}
			else
				{
				fClipIsEmpty = true;
				}
			}
		}
	}

ZDCCanvas_X::SetupClip::~SetupClip()
	{}

// =================================================================================================
// MARK: - ZDCCanvas_X::SetupInk

class ZDCCanvas_X::SetupInk
	{
public:
	SetupInk(ZDCCanvas_X* inCanvas, ZDCState& ioState);
	~SetupInk();

protected:
	ZDCCanvas_X* fCanvas;
	};

ZDCCanvas_X::SetupInk::SetupInk(ZDCCanvas_X* inCanvas, ZDCState& ioState)
	{
	fCanvas = inCanvas;

	if (ioState.fChangeCount_Mode != fCanvas->fChangeCount_Mode)
		{
		fCanvas->fXServer->SetFunction(fCanvas->fGC, spModeLookup[ioState.fMode]);
		ioState.fChangeCount_Mode = ++fCanvas->fChangeCount_Mode;
		}

	if (ioState.fChangeCount_PatternOrigin != fCanvas->fChangeCount_PatternOrigin)
		{
		fCanvas->fXServer->SetTSOrigin(fCanvas->fGC, -ioState.fPatternOrigin.h, -ioState.fPatternOrigin.v);
		ioState.fChangeCount_PatternOrigin = ++fCanvas->fChangeCount_PatternOrigin;
		}

	if (ioState.fChangeCount_PenWidth != fCanvas->fChangeCount_PenWidth)
		{
		ZCoord realPenWidth = ioState.fPenWidth;
		if (realPenWidth == 1)
			realPenWidth = 0;
		fCanvas->fXServer->SetLineAttributes(fCanvas->fGC, realPenWidth, LineSolid, CapButt, JoinMiter);
		ioState.fChangeCount_PenWidth = ++fCanvas->fChangeCount_PenWidth;
		}

	if (ioState.fChangeCount_Ink != fCanvas->fChangeCount_Ink)
		{
		ioState.fChangeCount_Ink = ++fCanvas->fChangeCount_Ink;
		ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
		if (theRep->fType == ZDCInk::eTypeSolidColor)
			{
			fCanvas->fXServer->SetForeground(fCanvas->fGC, theRep->fAsSolidColor.fColor);
			fCanvas->fXServer->SetFillStyle(fCanvas->fGC, FillSolid);
			}
		else if (theRep->fType == ZDCInk::eTypeTwoColor)
			{
			Pixmap oldPixmap = fCanvas->fCachedInkPixmap;

			// ZXServer::CreateBitmapFromData uses X11's XCreateBitmapFromData, which is documented in
			// Xlib Reference Manual volume 2, page 151 as being assumed to have byte_order == LSB_FIRST,
			// which is okay, but also bit_order == LSB_FIRST. ZDCPattern.pat is defined to be
			// big endian, so we have to take a local copy with the bits reversed. Note that
			// ZXServer::CreateBitmapFromDCPixmap uses ZDCPixmapNS to transfer the bits, and
			// correctly describes the destination has having little endian bit order.
			
			static const uint8 spBitsReversed[256] =
				{
				0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
				8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
				4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
				12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
				2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
				10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
				6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246,
				14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254,
				1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241,
				9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249,
				5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245,
				13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253,
				3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243,
				11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251,
				7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247,
				15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255
				};
			
			uint8 tempPattern[8];
			for (size_t x = 0; x < 8; ++x)
				tempPattern[x] = spBitsReversed[theRep->fAsTwoColor.fPattern.pat[x]];
			fCanvas->fCachedInkPixmap = fCanvas->fXServer->CreateBitmapFromData(fCanvas->fDrawable, reinterpret_cast<char*>(&tempPattern[0]), 8, 8);

			fCanvas->fXServer->SetStipple(fCanvas->fGC, fCanvas->fCachedInkPixmap);

			if (oldPixmap)
				fCanvas->fXServer->FreePixmap(oldPixmap);

			fCanvas->fXServer->SetForeground(fCanvas->fGC, theRep->fAsTwoColor.fForeColor);
			fCanvas->fXServer->SetBackground(fCanvas->fGC, theRep->fAsTwoColor.fBackColor);
			fCanvas->fXServer->SetFillStyle(fCanvas->fGC, FillOpaqueStippled);
			}
		else if (theRep->fType == ZDCInk::eTypeMultiColor)
			{
			if (fCanvas->fCachedInkPixmap)
				fCanvas->fXServer->FreePixmap(fCanvas->fCachedInkPixmap);
			fCanvas->fCachedInkPixmap = fCanvas->fXServer->CreatePixmapFromDCPixmap(fCanvas->fDrawable, *theRep->fAsMultiColor.fPixmap);
			fCanvas->fXServer->SetTile(fCanvas->fGC, fCanvas->fCachedInkPixmap);
			fCanvas->fXServer->SetFillStyle(fCanvas->fGC, FillTiled);
			}
		else
			{
			ZDebugStopf(kDebug_X, ("ZDCCanvas_X::Internal_SetupInk, unknown ink type"));
			}
		}
	}

ZDCCanvas_X::SetupInk::~SetupInk()
	{}

// =================================================================================================
// MARK: - ZDCCanvas_X

ZDCCanvas_X::ZDCCanvas_X()
	{
	fChangeCount_Origin = 1;
	fChangeCount_PatternOrigin = 1;
	fChangeCount_Ink = 1;
	fChangeCount_PenWidth = 1;
	fChangeCount_Mode = 1;
	fChangeCount_Clip = 1;
	fChangeCount_Font = 1;
	fChangeCount_TextColor = 1;

	fDrawable = 0;
	fGC = nullptr;

	fCachedInkPixmap = NULL;

	fMutexToLock = nullptr;
	fMutexToCheck = nullptr;
	}

ZDCCanvas_X::~ZDCCanvas_X()
	{
	// Subclasses must unlink themselves and detach/destroy grafports before this destructor is called.
	ZAssertStop(kDebug_X, !fDrawable);
	ZAssertStop(kDebug_X, fGC == nullptr);
	ZAssertStop(kDebug_X, fMutexToLock == nullptr);
	ZAssertStop(kDebug_X, fMutexToCheck == nullptr);
	if (fCachedInkPixmap)
		fXServer->FreePixmap(fCachedInkPixmap);
	}

// =================================================================================================

void ZDCCanvas_X::Pixel(ZDCState& ioState, ZCoord inLocationH, ZCoord inLocationV, const ZRGBColor& inColor)
	{
	if (!fDrawable)
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	fXServer->SetForeground(fGC, inColor);
	++fChangeCount_Ink;

	fXServer->DrawPoint(fDrawable, fGC, inLocationH + ioState.fOrigin.h, inLocationV + ioState.fOrigin.v);
	}

void ZDCCanvas_X::Line(ZDCState& ioState, ZCoord inStartH, ZCoord inStartV, ZCoord inEndH, ZCoord inEndV)
	{
	if (!fDrawable)
		return;

	if (!ioState.fInk)
		return;

	if (ioState.fPenWidth <= 0)
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	SetupInk theSetupInk(this, ioState);

	inStartH += ioState.fOrigin.h;
	inStartV += ioState.fOrigin.v;
	inEndH += ioState.fOrigin.h;
	inEndV += ioState.fOrigin.v;

	ZPoint leftMostPoint, rightMostPoint;
	if (inStartH < inEndH)
		{
		leftMostPoint.h = inStartH;
		leftMostPoint.v = inStartV;
		rightMostPoint.h = inEndH;
		rightMostPoint.v = inEndV;
		}
	else
		{
		leftMostPoint.h = inEndH;
		leftMostPoint.v = inEndV;
		rightMostPoint.h = inStartH;
		rightMostPoint.v = inStartV;
		}

	if (ioState.fPenWidth == 1)
		{
		fXServer->DrawLine(fDrawable, fGC, leftMostPoint.h, leftMostPoint.v, rightMostPoint.h, rightMostPoint.v);
		}
	else
		{
		// We're going to fill a rectangle or a polygon, depending on whether the line is horizontal/vertical or not
		if (leftMostPoint.h == rightMostPoint.h)
			{
			// Vertical line
			if (leftMostPoint.v < rightMostPoint.v)
				fXServer->FillRectangle(fDrawable, fGC, leftMostPoint.h, leftMostPoint.v, ioState.fPenWidth, rightMostPoint.v - leftMostPoint.v + ioState.fPenWidth);
			else
				fXServer->FillRectangle(fDrawable, fGC, leftMostPoint.h, rightMostPoint.v, ioState.fPenWidth, leftMostPoint.v - rightMostPoint.v + ioState.fPenWidth);
			}
		else if (leftMostPoint.v == rightMostPoint.v)
			{
			// Horizontal line
			fXServer->FillRectangle(fDrawable, fGC, leftMostPoint.h, leftMostPoint.v, rightMostPoint.h - leftMostPoint.h + ioState.fPenWidth, ioState.fPenWidth);
			}
		else
			{
			// For notes on this see AG's log book, 97-03-05
			// Okay, build a polygon which will emulate a normal square pen on the Mac
			XPoint thePoints[6];
			// Two cases, from top left down to bottom right, or from bottom left up to top right
			if (leftMostPoint.v < rightMostPoint.v)
				{
				thePoints[0].x = leftMostPoint.h;
				thePoints[0].y = leftMostPoint.v;

				thePoints[1].x = leftMostPoint.h + ioState.fPenWidth;
				thePoints[1].y = leftMostPoint.v;

				thePoints[2].x = rightMostPoint.h + ioState.fPenWidth;
				thePoints[2].y = rightMostPoint.v;

				thePoints[3].x = rightMostPoint.h + ioState.fPenWidth;
				thePoints[3].y = rightMostPoint.v + ioState.fPenWidth;

				thePoints[4].x = rightMostPoint.h;
				thePoints[4].y = rightMostPoint.v + ioState.fPenWidth;

				thePoints[5].x = leftMostPoint.h;
				thePoints[5].y = leftMostPoint.v + ioState.fPenWidth;
				}
			else
				{
				thePoints[0].x = leftMostPoint.h;
				thePoints[0].y = leftMostPoint.v;

				thePoints[1].x = rightMostPoint.h;
				thePoints[1].y = rightMostPoint.v;

				thePoints[2].x = rightMostPoint.h + ioState.fPenWidth;
				thePoints[2].y = rightMostPoint.v;

				thePoints[3].x = rightMostPoint.h + ioState.fPenWidth;
				thePoints[3].y = rightMostPoint.v + ioState.fPenWidth;

				thePoints[4].x = leftMostPoint.h + ioState.fPenWidth;
				thePoints[4].y = leftMostPoint.v + ioState.fPenWidth;

				thePoints[5].x = leftMostPoint.h;
				thePoints[5].y = leftMostPoint.v + ioState.fPenWidth;
				}
			fXServer->FillPolygon(fDrawable, fGC, thePoints, 6, Convex, CoordModeOrigin);
			}
		}
	}

void ZDCCanvas_X::FrameRect(ZDCState& ioState, const ZRect& inRect)
	{
	if (!fDrawable)
		return;
	if (!ioState.fInk)
		return;
	if (ioState.fPenWidth <= 0)
		return;
	if (inRect.IsEmpty())
		return;

	if (ioState.fPenWidth == 1)
		{
		SetupLock theSetupLock(this);
		SetupClip theSetupClip(this, ioState);
		if (theSetupClip.IsEmpty())
			return;
		SetupInk theSetupInk(this, ioState);
		ZRect realRect = inRect + ioState.fOrigin;
		fXServer->DrawRectangle(fDrawable, fGC, inRect.left + ioState.fOrigin.h, inRect.top + ioState.fOrigin.v, inRect.Width() - 1, inRect.Height() - 1);
		}
	else
		{
		this->Line(ioState, inRect.left, inRect.top, inRect.right - ioState.fPenWidth, inRect.top);
		this->Line(ioState, inRect.right - ioState.fPenWidth, inRect.top, inRect.right - ioState.fPenWidth, inRect.bottom - ioState.fPenWidth);
		this->Line(ioState, inRect.right - ioState.fPenWidth, inRect.bottom - ioState.fPenWidth, inRect.left, inRect.bottom - ioState.fPenWidth);
		this->Line(ioState, inRect.left, inRect.bottom - ioState.fPenWidth, inRect.left, inRect.top);
		}
	}

void ZDCCanvas_X::FillRect(ZDCState& ioState, const ZRect& inRect)
	{
	if (!fDrawable)
		return;
	if (!ioState.fInk)
		return;
	if (inRect.IsEmpty())
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	SetupInk theSetupInk(this, ioState);

	fXServer->FillRectangle(fDrawable, fGC, inRect.left + ioState.fOrigin.h, inRect.top + ioState.fOrigin.v, inRect.Width(), inRect.Height());
	}

void ZDCCanvas_X::InvertRect(ZDCState& ioState, const ZRect& inRect)
	{
	if (!fDrawable)
		return;
	if (inRect.IsEmpty())
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	fXServer->SetFunction(fGC, GXinvert);
	++fChangeCount_Mode;

	fXServer->FillRectangle(fDrawable, fGC, inRect.left + ioState.fOrigin.h, inRect.top + ioState.fOrigin.v, inRect.Width(), inRect.Height());
	}

void ZDCCanvas_X::FrameRegion(ZDCState& ioState, const ZDCRgn& inRgn)
	{
	if (!fDrawable)
		return;
	if (!ioState.fInk)
		return;
	if (ioState.fPenWidth <= 0)
		return;

	SetupLock theSetupLock(this);

	if (ZDCRgn localRgn = this->Internal_CalcClipRgn(ioState) & ((inRgn - inRgn.Inset(ioState.fPenWidth, ioState.fPenWidth)) + ioState.fOrigin))
		{
		SetupInk theSetupInk(this, ioState);
		ZRect localBounds = localRgn.Bounds();

		fXServer->SetRegion(fGC, localRgn.GetRegion());
		++fChangeCount_Clip;

		fXServer->FillRectangle(fDrawable, fGC, localBounds.left, localBounds.top, localBounds.Width(), localBounds.Height());
		}
	}

void ZDCCanvas_X::FillRegion(ZDCState& ioState, const ZDCRgn& inRgn)
	{
	if (!fDrawable)
		return;
	if (!ioState.fInk)
		return;

	SetupLock theSetupLock(this);

	if (ZDCRgn localRgn = this->Internal_CalcClipRgn(ioState) & (inRgn + ioState.fOrigin))
		{
		SetupInk theSetupInk(this, ioState);
		ZRect localBounds = localRgn.Bounds();

		fXServer->SetRegion(fGC, localRgn.GetRegion());
		++fChangeCount_Clip;

		fXServer->FillRectangle(fDrawable, fGC, localBounds.left, localBounds.top, localBounds.Width(), localBounds.Height());
		}
	}

void ZDCCanvas_X::InvertRegion(ZDCState& ioState, const ZDCRgn& inRgn)
	{
	if (!fDrawable)
		return;

	SetupLock theSetupLock(this);

	if (ZDCRgn localRgn = this->Internal_CalcClipRgn(ioState) & (inRgn + ioState.fOrigin))
		{
		ZRect localBounds = localRgn.Bounds();

		fXServer->SetRegion(fGC, localRgn.GetRegion());
		++fChangeCount_Clip;

		fXServer->SetFunction(fGC, GXinvert);
		++fChangeCount_Mode;

		fXServer->FillRectangle(fDrawable, fGC, localBounds.left, localBounds.top, localBounds.Width(), localBounds.Height());
		}
	}

void ZDCCanvas_X::FrameRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize)
	{
	if (!fDrawable)
		return;
	if (!ioState.fInk)
		return;
	if (ioState.fPenWidth <= 0)
		return;
	if (inRect.IsEmpty())
		return;

	this->FrameRegion(ioState, ZDCRgn::sRoundRect(inRect, inCornerSize));
	}

void ZDCCanvas_X::FillRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize)
	{
	if (!fDrawable)
		return;
	if (!ioState.fInk)
		return;
	if (inRect.IsEmpty())
		return;

	this->FillRegion(ioState, ZDCRgn::sRoundRect(inRect, inCornerSize));
	}

void ZDCCanvas_X::InvertRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize)
	{
	if (!fDrawable)
		return;
	if (inRect.IsEmpty())
		return;

	this->InvertRegion(ioState, ZDCRgn::sRoundRect(inRect, inCornerSize));
	}

void ZDCCanvas_X::FrameEllipse(ZDCState& ioState, const ZRect& inBounds)
	{
	if (!fDrawable)
		return;
	if (!ioState.fInk)
		return;
	if (ioState.fPenWidth <= 0)
		return;
	if (inBounds.IsEmpty())
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	SetupInk theSetupInk(this, ioState);
	ZPoint ellipseSize = inBounds.Size();
	ZCoord twicePenWidth = ioState.fPenWidth * 2;
	if (twicePenWidth >= ellipseSize.h || twicePenWidth >= ellipseSize.v)
		{
		fXServer->FillArc(fDrawable, fGC,
			inBounds.left + ioState.fOrigin.h,
			inBounds.top + ioState.fOrigin.v,
			ellipseSize.h,
			ellipseSize.v,
			0, 360*64);
		}
	else
		{
		ZCoord halfPenWidth = ioState.fPenWidth / 2;
		fXServer->DrawArc(fDrawable, fGC,
			inBounds.left + ioState.fOrigin.h + halfPenWidth,
			inBounds.top + ioState.fOrigin.v + halfPenWidth,
			ellipseSize.h - 2 * halfPenWidth,
			ellipseSize.v - 2 * halfPenWidth,
			0, 360 * 64);
		}
	}

void ZDCCanvas_X::FillEllipse(ZDCState& ioState, const ZRect& inBounds)
	{
	if (!fDrawable)
		return;
	if (!ioState.fInk)
		return;
	if (inBounds.IsEmpty())
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	SetupInk theSetupInk(this, ioState);

	fXServer->FillArc(fDrawable, fGC, inBounds.left + ioState.fOrigin.h, inBounds.top + ioState.fOrigin.v, inBounds.Width(), inBounds.Height(), 0, 360*64);
	}

void ZDCCanvas_X::InvertEllipse(ZDCState& ioState, const ZRect& inBounds)
	{
	if (!fDrawable)
		return;
	if (inBounds.IsEmpty())
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	fXServer->SetFunction(fGC, GXinvert);
	++fChangeCount_Mode;

	fXServer->FillArc(fDrawable, fGC, inBounds.left + ioState.fOrigin.h, inBounds.top + ioState.fOrigin.v, inBounds.Width(), inBounds.Height(), 0, 360*64);
	}

void ZDCCanvas_X::FillPoly(ZDCState& ioState, const ZDCPoly& inPoly)
	{
	if (!fDrawable)
		return;
	if (!ioState.fInk)
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	SetupInk theSetupInk(this, ioState);

	size_t theCount;
	XPoint* theXPoints;
	inPoly.GetXPoints(ioState.fOrigin, theXPoints, theCount);
	fXServer->FillPolygon(fDrawable, fGC, theXPoints, theCount, Complex, CoordModeOrigin);
	}

void ZDCCanvas_X::InvertPoly(ZDCState& ioState, const ZDCPoly& inPoly)
	{
	if (!fDrawable)
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	fXServer->SetFunction(fGC, GXinvert);
	++fChangeCount_Mode;

	size_t theCount;
	XPoint* theXPoints;
	inPoly.GetXPoints(ioState.fOrigin, theXPoints, theCount);
	fXServer->FillPolygon(fDrawable, fGC, theXPoints, theCount, Complex, CoordModeOrigin);
	}

void ZDCCanvas_X::DrawPixmap(ZDCState& ioState, const ZPoint& inLocation, const ZDCPixmap& inSourcePixmap, const ZDCPixmap* inMaskPixmap)
	{
	if (!fDrawable)
		return;

	if (!inSourcePixmap)
		return;

	ZRect realSourceBounds = inSourcePixmap.Size();

	ZPoint realLocation = inLocation + ioState.fOrigin;

	if (inMaskPixmap)
		{
		if (!*inMaskPixmap)
			return;

		SetupLock theSetupLock(this);

		ZDCRgn sourceBoundsRgn = ZRect(realSourceBounds.Size());
		ZDCRgn realClip = (this->Internal_CalcClipRgn(ioState) - realLocation) & sourceBoundsRgn;
		if (realClip.IsEmpty())
			return;

		// Take a one bit copy of the mask
		Pixmap monoPixmap = fXServer->CreateBitmapFromDCPixmap(fDrawable, *inMaskPixmap, realSourceBounds, false);
		ZAssert(monoPixmap != None);

		// Paint with zeroes the area of the mask that's not part of the clip, if any
		if (ZDCRgn inverseClip = sourceBoundsRgn - realClip)
			{
			XGCValues values;
			values.graphics_exposures = 0;
			GC monoGC = fXServer->CreateGC(monoPixmap, GCGraphicsExposures, &values);
			fXServer->SetRegion(monoGC, inverseClip.GetRegion());
			fXServer->SetForeground(monoGC, 0);
			fXServer->FillRectangle(monoPixmap, monoGC, 0, 0, realSourceBounds.Width(), realSourceBounds.Height());
			fXServer->FreeGC(monoGC);
			}

		fXServer->SetFunction(fGC, GXcopy);
		++fChangeCount_Mode;
		fXServer->SetClipMask(fGC, monoPixmap);
		fXServer->SetClipOrigin(fGC, realLocation.h, realLocation.v);
		++fChangeCount_Clip;
		fXServer->DrawDCPixmap(fDrawable, fGC, realLocation, inSourcePixmap, realSourceBounds);
		fXServer->SetClipMask(fGC, None);
		fXServer->SetClipOrigin(fGC, 0, 0);
		fXServer->FreePixmap(monoPixmap);
		}
	else
		{
		SetupLock theSetupLock(this);
		SetupClip theSetupClip(this, ioState);
		if (theSetupClip.IsEmpty())
			return;
		fXServer->SetFunction(fGC, GXcopy);
		++fChangeCount_Mode;
		fXServer->DrawDCPixmap(fDrawable, fGC, realLocation, inSourcePixmap, realSourceBounds);
		}
	}

void ZDCCanvas_X::DrawText(ZDCState& ioState, const ZPoint& inLocation, const char* inText, size_t textLength)
	{
	if (!fDrawable)
		return;

	SetupLock theSetupLock(this);
	SetupClip theSetupClip(this, ioState);
	if (theSetupClip.IsEmpty())
		return;

	fXServer->SetForeground(fGC, ioState.fTextColor);
	++fChangeCount_TextColor;
	++fChangeCount_Ink;

	fXServer->SetFont(fGC, ioState.fFont);
	ZPoint realLocation = inLocation + ioState.fOrigin;
	fXServer->DrawString(fDrawable, fGC, realLocation.h, realLocation.v, inText, textLength);
	}

ZCoord ZDCCanvas_X::GetTextWidth(ZDCState& ioState, const char* inText, size_t textLength)
	{
	SetupLock theSetupLock(this);

	return fXServer->TextWidth(ioState.fFont, inText, textLength);
	}

void ZDCCanvas_X::GetFontInfo(ZDCState& ioState, ZCoord& outAscent, ZCoord& outDescent, ZCoord& outLeading)
	{
	SetupLock theSetupLock(this);

	short ascent, descent, leading;
	fXServer->GetFontInfo(ioState.fFont, ascent, descent, leading);
	outAscent = ascent;
	outDescent = descent;
	outLeading = leading;
	}

ZCoord ZDCCanvas_X::GetLineHeight(ZDCState& ioState)
	{
	SetupLock theSetupLock(this);

	short ascent, descent, leading;
	fXServer->GetFontInfo(ioState.fFont, ascent, descent, leading);
	return ascent + descent;
	}

void ZDCCanvas_X::BreakLine(ZDCState& ioState, const char* inLineStart, size_t inRemainingTextLength, ZCoord inTargetWidth, size_t& outNextLineDelta)
	{
	SetupLock theSetupLock(this);

	outNextLineDelta = inRemainingTextLength;
	}

void ZDCCanvas_X::InvalCache()
	{
	SetupLock theSetupLock(this);

	++fChangeCount_Origin;
	++fChangeCount_PatternOrigin;
	++fChangeCount_Ink;
	++fChangeCount_PenWidth;
	++fChangeCount_Mode;
	++fChangeCount_Clip;
	++fChangeCount_Font;
	++fChangeCount_TextColor;
	}

void ZDCCanvas_X::Sync()
	{
	SetupLock theSetupLock(this);

	fXServer->Sync(false);
	}

void ZDCCanvas_X::Flush()
	{
	SetupLock theSetupLock(this);

	fXServer->Flush();
	}

short ZDCCanvas_X::GetDepth()
	{
	SetupLock theSetupLock(this);

	// Hard coded for now
	return 8;
	}

bool ZDCCanvas_X::IsOffScreen()
	{
	SetupLock theSetupLock(this);

	return false;
	}

bool ZDCCanvas_X::IsPrinting()
	{
	SetupLock theSetupLock(this);
	return false;
	}

ZRef<ZDCCanvas> ZDCCanvas_X::CreateOffScreen(const ZRect& inCanvasRect)
	{
	using namespace ZDCPixmapNS;

	SetupLock theSetupLock(this);
	return new ZDCCanvas_X_OffScreen(fXServer, inCanvasRect.Size(), eFormatEfficient_Color_32);
	}

ZRef<ZDCCanvas> ZDCCanvas_X::CreateOffScreen(ZPoint inDimensions, ZDCPixmapNS::EFormatEfficient iFormat)
	{
	SetupLock theSetupLock(this);
	return new ZDCCanvas_X_OffScreen(fXServer, inDimensions, iFormat);
	}

// =================================================================================================
// MARK: - ZDCCanvas_X_NonWindow

ZDCCanvas_X_NonWindow::ZDCCanvas_X_NonWindow()
	{
	fMutexToLock = &fMutex;
	}

ZDCCanvas_X_NonWindow::~ZDCCanvas_X_NonWindow()
	{
	ZAssertStop(kDebug_X, !fDrawable);
	ZAssertStop(kDebug_X, !fGC);
	fMutexToLock = nullptr;
	}

void ZDCCanvas_X_NonWindow::Finalize()
	{
	fMutexToLock->Acquire();

	if (this->GetRefCount() > 1)
		{
		this->FinalizationComplete();
		fMutexToLock->Release();
		}
	else
		{
		this->FinalizationComplete();
		fMutexToLock->Release();
		delete this;
		}
	}

void ZDCCanvas_X_NonWindow::Scroll(ZDCState& ioState, const ZRect& inRect, ZCoord hDelta, ZCoord vDelta)
	{
	if (!fDrawable)
		return;
	SetupLock theSetupLock(this);

	++fChangeCount_Clip;

	ZPoint offset(hDelta, vDelta);

	// destRgn is the pixels we want and are able to draw to.
	ZDCRgn destRgn = ((ioState.fClip + ioState.fClipOrigin) & (inRect + ioState.fOrigin));

	// srcRgn is the set of pixels we're want and are able to copy from.
	ZDCRgn srcRgn = ((destRgn - offset) & (inRect + ioState.fOrigin));

	// drawnRgn is the destination pixels that will be drawn by the CopyBits call, it's the srcRgn
	// shifted back to the destination.
	ZDCRgn drawnRgn = srcRgn + offset;

	// invalidRgn is the destination pixels we wanted to draw but could not because they were
	// outside the visRgn, or were in the excludeRgn
	ZDCRgn invalidRgn = destRgn - drawnRgn;

	// And set the clip (drawnRgn)
	fXServer->SetRegion(fGC, drawnRgn.GetRegion());
	++fChangeCount_Clip;

	fXServer->SetFunction(fGC, GXcopy);
	++fChangeCount_Mode;

	ZRect drawnBounds = drawnRgn.Bounds();

	fXServer->CopyArea(fDrawable, fDrawable, fGC,
				drawnBounds.Left() - offset.h, drawnBounds.Top() - offset.v,
				drawnBounds.Width(), drawnBounds.Height(),
				drawnBounds.Left(), drawnBounds.Top());
	}

void ZDCCanvas_X_NonWindow::CopyFrom(ZDCState& ioState, const ZPoint& inDestLocation, ZRef<ZDCCanvas> inSourceCanvas, const ZDCState& inSourceState, const ZRect& inSourceRect)
	{
	ZRef<ZDCCanvas_X> sourceCanvasX = ZRefDynamicCast<ZDCCanvas_X>(inSourceCanvas);
	ZAssertStop(kDebug_X, sourceCanvasX != nullptr);

	if (!fDrawable || !sourceCanvasX->Internal_GetDrawable())
		return;

	SetupLock theSetupLock(this);

	// We can (currently) only copy from one drawable to another if they're on the same display
	//	ZAssertStop(kDebug_X, fXServer == sourceCanvasX->fXServer);

	ZRect destRect = inSourceRect + (ioState.fOrigin + inDestLocation - inSourceRect.TopLeft());
	ZRect sourceRect = inSourceRect + inSourceState.fOrigin;

	ZDCRgn realClip = this->Internal_CalcClipRgn(ioState);

	fXServer->SetRegion(fGC, realClip.GetRegion());
	++fChangeCount_Clip;

	fXServer->SetFunction(fGC, GXcopy);
	++fChangeCount_Mode;

	fXServer->CopyArea(sourceCanvasX->Internal_GetDrawable(), fDrawable, fGC,
				sourceRect.Left(), sourceRect.Top(),
				sourceRect.Width(), sourceRect.Height(),
				destRect.Left(), destRect.Top());
	}

ZDCRgn ZDCCanvas_X_NonWindow::Internal_CalcClipRgn(const ZDCState& inState)
	{
	return inState.fClip + inState.fClipOrigin;
	}

// =================================================================================================
// MARK: - ZDCCanvas_X_OffScreen

ZDCCanvas_X_OffScreen::ZDCCanvas_X_OffScreen(ZRef<ZXServer> inXServer, ZPoint inDimensions, ZDCPixmapNS::EFormatEfficient iFormat)
	{
	ZAssertStop(kDebug_X, inDimensions.h > 0 && inDimensions.v > 0);

	fXServer = inXServer;

	Drawable otherDrawable = RootWindow(fXServer->GetDisplay(), DefaultScreen(fXServer->GetDisplay()));
	fDrawable = fXServer->CreatePixmap(otherDrawable, inDimensions.h, inDimensions.v,
							DefaultDepth(fXServer->GetDisplay(), DefaultScreen(fXServer->GetDisplay())));

	XGCValues values;
	values.graphics_exposures = 0;
	fGC = fXServer->CreateGC(fDrawable, GCGraphicsExposures, &values);
	}

ZDCCanvas_X_OffScreen::~ZDCCanvas_X_OffScreen()
	{
	fXServer->FreePixmap(fDrawable);
	fDrawable = 0;
	fXServer->FreeGC(fGC);
	fGC = nullptr;
	}

bool ZDCCanvas_X_OffScreen::IsOffScreen()
	{ return true; }

// =================================================================================================
// MARK: - Factory functions

namespace { // anonymous

class Make_CreateRaster
:	public ZFunctionChain_T<ZRef<ZDCPixmapRep>, const ZDCPixmapRep::CreateRaster_t&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		return false;
		}	
	} sMaker0;


class Make_CreateRasterDesc
:	public ZFunctionChain_T<ZRef<ZDCPixmapRep>, const ZDCPixmapRep::CreateRasterDesc_t&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		return false;
		}	
	} sMaker1;


class Make_EfficientToStandard
:	public ZFunctionChain_T<ZDCPixmapNS::EFormatStandard, ZDCPixmapNS::EFormatEfficient>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		using namespace ZDCPixmapNS;

		switch (iParam)
			{
			case eFormatEfficient_Gray_1: oResult = eFormatStandard_Gray_1; return true;
			case eFormatEfficient_Gray_8: oResult = eFormatStandard_Gray_8; return true;

			#if ZCONFIG(Endian, Big)
				case eFormatEfficient_Color_16: oResult = eFormatStandard_xRGB_16_BE; return true;
				case eFormatEfficient_Color_24: oResult = eFormatStandard_RGB_24; return true;
				case eFormatEfficient_Color_32: oResult = eFormatStandard_ARGB_32; return true;
			#else
				case eFormatEfficient_Color_16: oResult = eFormatStandard_xRGB_16_LE; return true;
				case eFormatEfficient_Color_24: oResult = eFormatStandard_BGR_24; return true;
				case eFormatEfficient_Color_32: oResult = eFormatStandard_BGRA_32; return true;
			#endif
			}

		return false;
		}	
	} sMaker2;

} // anonymous namespace

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(DC_X)
