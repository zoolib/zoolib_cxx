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

// MakeInset, CompressHRGN, CompressBRegion are derived from X Windows source.
/************************************************************************

Copyright (c) 1987, 1988 X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts.

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

************************************************************************/

#include "ZDCRgn.h"
#include "ZDCPoly.h"
#include "ZDebug.h"
#include "ZMemory.h"

#include <cstdio>

using std::vector;

#define kDebug_DCRgn 0

// =================================================================================================

#if ZCONFIG_SPI_Enabled(QuickDraw)
#	include ZMACINCLUDE(CarbonCore,MacMemory.h)
#endif

#if !defined(TARGET_OS_MAC) || (TARGET_OS_MAC && !defined(MacOffsetRgn))
#	define MacOffsetRgn OffsetRgn
#	define MacUnionRgn UnionRgn
#	define MacXorRgn XorRgn
#	define MacSetRectRgn SetRectRgn
#	define MacEqualRgn EqualRgn
#	define MacCopyRgn CopyRgn
#endif

#if ZCONFIG_SPI_Enabled(BeOS)
#	ifndef ZCONFIG_BRegionAlternateEnabled
#		define ZCONFIG_BRegionAlternateEnabled 0
#	endif
#	if ZCONFIG_BRegionAlternateEnabled
#		include "ZBRegionAlternate.h"
#	endif
#endif

#if ZCONFIG_SPI_Enabled(GDI)
#	include "ZUtil_Win.h"
#endif


#if ZCONFIG_SPI_Enabled(X11)
// These definitions are used in Decompose, and should be generally correct
// but the code will break silently if used by an unusal xlib implementation.
struct _BOX
	{
	short x1;
	short x2;
	short y1;
	short y2;
	};

struct _XRegion
	{
	long size;
	long numRects;
	_BOX* rects;
	_BOX extents;
	};
#endif // ZCONFIG_SPI_Enabled(X11)

// ==================================================

// Macros that handle invocation of format swapping code, they drop out if not
// needed so the actual code that's compiled is the same as in the old version of ZDCRgn

#if ZCONFIG_API_Graphics_Multi
#	define CheckFormat(repFormat, checkedFormat) (repFormat == checkedFormat)
#	define Harmonize(current, iOther) if (current->fRep->fFormat != iOther.fRep->fFormat) \
		current->Internal_Harmonize(iOther)

#	define TouchAndSetFormat(current, requiredFormat) \
	if (!current->fRep || current->fRep->GetRefCount() > 1 \
		|| current->fRep->fFormat != requiredFormat) \
		current->Internal_TouchAndSetFormat(requiredFormat)

#	define StoreFormat(formatField, formatValue) (formatField = formatValue)
#else
#	define CheckFormat(repFormat, checkedFormat) (true)
#	define Harmonize(current, iOther) ((void)(0))
#	define TouchAndSetFormat(current, requiredFormat) current->Internal_Touch()
#	define StoreFormat(formatField, formatValue) ((void)(0))
#endif

enum
	{
	ZCONFIG_API_Graphics_QD = 1,
	ZCONFIG_API_Graphics_GDI,
	ZCONFIG_API_Graphics_X,
	ZCONFIG_API_Graphics_Be,
	ZCONFIG_API_Graphics_ZooLib
	};

// ==================================================

ZDCRgn::Rep::~Rep()
	{
	if (false) {}

	#if ZCONFIG_SPI_Enabled(QuickDraw)
	else if (CheckFormat(fFormat, ZCONFIG_API_Graphics_QD))
		::DisposeRgn(fRgnHandle);
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
	else if (CheckFormat(fFormat, ZCONFIG_API_Graphics_GDI))
		::DeleteObject(fHRGN);
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
	else if (CheckFormat(fFormat, ZCONFIG_API_Graphics_X))
		::XDestroyRegion(fRegion);
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS)
	else if (CheckFormat(fFormat, ZCONFIG_API_Graphics_Be))
		delete fBRegion;
	#endif

	#if 1//ZCONFIG_SPI_Enabled(ZooLib)
	else if (CheckFormat(fFormat, ZCONFIG_API_Graphics_ZooLib))
		delete fBigRegion;
	#endif
	}

// ==================================================

#if ZCONFIG_API_Graphics_Multi

void ZDCRgn::Internal_Harmonize(const ZDCRgn& iOther) const
	{
	// Whichever rep has the lowest count gets its format switched. If they're the
	// same, then our rep gets munged (as we're the rep being operated on that's fair).
	if (fRep->GetRefCount() <= iOther.fRep->GetRefCount())
		this->Internal_ChangeFormat(iOther.fRep->fFormat);
	else
		iOther.Internal_ChangeFormat(fRep->fFormat);
	}

void ZDCRgn::Internal_ChangeFormat(short iRequiredFormat) const
	{
#if ZCONFIG_SPI_Enabled(QuickDraw) && ZCONFIG_SPI_Enabled(GDI)
	ZAssertStop(kDebug_DCRgn, fRep && fRep->fFormat != iRequiredFormat);

	ZDCRgn* nonConstThis = const_cast<ZDCRgn*>(this);

	if (iRequiredFormat == ZCONFIG_API_Graphics_QD)
		{
		RgnHandle theRgnHandle = ::NativeRegionToMacRegion(fRep->fHRGN);
		nonConstThis->fRep->fRgnHandle = theRgnHandle;
		nonConstThis->fRep->fFormat = ZCONFIG_API_Graphics_QD;
		}
	else
		{
		HRGN theHRGN = (HRGN)::MacRegionToNativeRegion(fRep->fRgnHandle);
		nonConstThis->fRep->fHRGN = theHRGN;
		nonConstThis->fRep->fFormat = ZCONFIG_API_Graphics_GDI;
		}
#else
	ZUnimplemented();
#endif
	}

void ZDCRgn::Internal_TouchAndSetFormat(short iRequiredFormat)
	{
	#if ZCONFIG_SPI_Enabled(QuickDraw) && ZCONFIG_SPI_Enabled(GDI)

	Rep* newRep = new Rep; // Don't assign to a ZRef, generates extra code unnecessarily

	if (iRequiredFormat == ZCONFIG_API_Graphics_QD)
		{
		if (fRep)
			{
			if (fRep->fFormat == ZCONFIG_API_Graphics_GDI)
				{
				newRep->fRgnHandle = ::NativeRegionToMacRegion(fRep->fHRGN);
				}
			else
				{
				newRep->fRgnHandle = ::NewRgn();
				::MacCopyRgn(fRep->fRgnHandle, newRep->fRgnHandle);
				}
			}
		else
			{
			newRep->fRgnHandle = ::NewRgn();
			}

		newRep->fFormat = ZCONFIG_API_Graphics_QD;
		}
	else
		{
		if (fRep)
			{
			if (fRep->fFormat == ZCONFIG_API_Graphics_QD)
				{
				newRep->fHRGN = (HRGN)::MacRegionToNativeRegion(fRep->fRgnHandle);
				}
			else
				{
				newRep->fHRGN = ::CreateRectRgn(0,0,0,0);
				::CombineRgn(newRep->fHRGN, fRep->fHRGN, nil, RGN_COPY);
				}
			}
		else
			{
			newRep->fHRGN = ::CreateRectRgn(0, 0, 0, 0);
			}

		newRep->fFormat = ZCONFIG_API_Graphics_GDI;
		}
	fRep = newRep;

	#else // ZCONFIG_SPI_Enabled(QuickDraw) && ZCONFIG_SPI_Enabled(GDI)

	this->Internal_TouchReal();

	#endif // ZCONFIG_SPI_Enabled(QuickDraw) && ZCONFIG_SPI_Enabled(GDI)
	}

#endif // ZCONFIG_API_Graphics_Multi

// ==================================================

inline void ZDCRgn::Internal_Touch()
	{
	if (!fRep || fRep->GetRefCount() > 1)
		this->Internal_TouchReal();
	}

void ZDCRgn::Internal_TouchReal()
	{
	Rep* newRep = new Rep; // Don't assign to a ZRef, generates extra code unnecessarily
	if (false) {}

	#if ZCONFIG_SPI_Enabled(GDI)
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
		{
		newRep->fHRGN = ::CreateRectRgn(0,0,0,0);
		StoreFormat(newRep->fFormat, ZCONFIG_API_Graphics_GDI);
		if (fRep)
			::CombineRgn(newRep->fHRGN, fRep->fHRGN, nil, RGN_COPY);
		}
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
		{
		newRep->fRgnHandle = ::NewRgn();
		StoreFormat(newRep->fFormat, ZCONFIG_API_Graphics_QD);
		if (fRep)
			::MacCopyRgn(fRep->fRgnHandle, newRep->fRgnHandle);
		}
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
		{
		newRep->fRegion = ::XCreateRegion();
		StoreFormat(newRep->fFormat, ZCONFIG_API_Graphics_X);
		if (fRep)
			::XUnionRegion(fRep->fRegion, fRep->fRegion, newRep->fRegion);
		}
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS)
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
		{
		if (fRep)
			newRep->fBRegion = new BRegion(*fRep->fBRegion);
		else
			newRep->fBRegion = new BRegion;
		StoreFormat(newRep->fFormat, ZCONFIG_API_Graphics_Be);
		}
	#endif

	#if 1//ZCONFIG_SPI_Enabled(ZooLib)
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
		{
		if (fRep)
			newRep->fBigRegion = new ZBigRegion(*fRep->fBigRegion);
		else
			newRep->fBigRegion = new ZBigRegion;
		StoreFormat(newRep->fFormat, ZCONFIG_API_Graphics_ZooLib);
		}
	#endif

	fRep = newRep;
	}

// =================================================================================================

void ZDCRgn::sInternal_CreateRep_Rect(ZRef<Rep>& oRep,
	ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom)
	{
	// AG 2000-02-17. Windows and X do not create an empty region
	// when passed bad coords, so we filter them here.
	if (iRight <= iLeft || iBottom <= iTop)
		return;
	oRep = new Rep;

#if ZCONFIG_SPI_Enabled(GDI)

	StoreFormat(oRep->fFormat, ZCONFIG_API_Graphics_GDI);
	oRep->fHRGN = ::CreateRectRgn(iLeft, iTop, iRight, iBottom);

#elif ZCONFIG_SPI_Enabled(QuickDraw)

	StoreFormat(oRep->fFormat, ZCONFIG_API_Graphics_QD);
	oRep->fRgnHandle = ::NewRgn();
	::MacSetRectRgn(oRep->fRgnHandle, iLeft, iTop, iRight, iBottom);

#elif ZCONFIG_SPI_Enabled(X11)

	StoreFormat(oRep->fFormat, ZCONFIG_API_Graphics_X);
	oRep->fRegion = ::XCreateRegion();
	XRectangle tempRect;
	tempRect.x = iLeft;
	tempRect.y = iTop;
	tempRect.width = iRight - iLeft;
	tempRect.height = iBottom - iTop;
	::XUnionRectWithRegion(&tempRect, oRep->fRegion, oRep->fRegion);

#elif ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled

	StoreFormat(oRep->fFormat, ZCONFIG_API_Graphics_Be);
	oRep->fBRegion = new BRegion;
	::ZBRegionAlternate_Set(oRep->fBRegion, iLeft, iTop, iRight - 1, iBottom - 1);

#elif ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled

	StoreFormat(oRep->fFormat, ZCONFIG_API_Graphics_Be);
	oRep->fBRegion = new BRegion;
	clipping_rect tempRect;
	tempRect.left = iLeft;
	tempRect.top = iTop;
	tempRect.right = iRight - 1;
	tempRect.bottom = iBottom - 1;
	oRep->fBRegion->Set(tempRect);

#elif 1//ZCONFIG_SPI_Enabled(ZooLib)

	StoreFormat(oRep->fFormat, ZCONFIG_API_Graphics_ZooLib);
	oRep->fBigRegion = new ZBigRegion(ZRect_T<int32>(iLeft, iTop, iRight, iBottom));

#endif
	}

void ZDCRgn::sInternal_CreateRep_RoundRect(ZRef<Rep>& oRep,
	ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom,
	ZCoord iCornerSizeH, ZCoord iCornerSizeV)
	{
	ZAssertStop(kDebug_DCRgn, iCornerSizeH > 1 && iCornerSizeV > 1);

	if (iRight <= iLeft || iBottom <= iTop)
		return;

	// AG 2000-07-27. We're not using Mac native round rects because
	// I do not yet know how to match them exactly on other platforms.
#if 0 // ZCONFIG_SPI_Enabled(QuickDraw)

	oRep = new Rep;
	StoreFormat(oRep->fFormat, ZCONFIG_API_Graphics_QD);
	oRep->fRgnHandle = ::NewRgn();
	::OpenRgn();
	Rect tempRect;
	tempRect.left = iLeft;
	tempRect.top = iTop;
	tempRect.right = iRight;
	tempRect.bottom = iBottom;
	::FrameRoundRect(&tempRect, iCornerSizeH, iCornerSizeV);
	::CloseRgn(oRep->fRgnHandle);

#else // ZCONFIG_SPI_Enabled(QuickDraw)

	// From Graphics Programming Column, DDJ #216 July 1994, page 119. (Source is on page 146.)
	// Force corner sizes into a valid range
	if (iCornerSizeH > iRight - iLeft)
		iCornerSizeH = short(iRight - iLeft);
	if (iCornerSizeV > iBottom - iTop)
		iCornerSizeV = short(iBottom - iTop);

	// Corner sizes are eaier to deal with when they're radii and not diameters
	iCornerSizeH /= 2;
	iCornerSizeV /= 2;

	// Switch to more normal notation. Note that the loops assume x increases
	// left to right and y increases bottom to top (normal math-style), rather
	// than y increasing top to bottom, so we have to juggle the numbers.
	double a = iCornerSizeH;
	double b = iCornerSizeV ;
	double aSquared = a * a;
	double bSquared = b * b;
	double twoASquared = 2 * aSquared;
	double twoBSquared = 2 * bSquared;

	// First octant, top to top-right
	vector<ZRect> rectsUpperFirst;
	vector<ZRect> rectsLowerFirst;
	ZCoord x = -1;
	ZCoord y = ZCoord(b);
	double twoXBSquared = 0;
	double twoYASquared = y * twoASquared;
	double error = -y * aSquared; // b^2 x^2 + a^2 y^2 - a^2 b^2 - a^2 y

	while (twoXBSquared <= twoYASquared)
		{
		++x;
		twoXBSquared += twoBSquared;
		error += twoXBSquared - bSquared;
		if (error >= 0)
			{
			rectsUpperFirst.push_back(ZRect(
				ZCoord(iLeft + iCornerSizeH - x),
				ZCoord(iTop + iCornerSizeV - y),
				ZCoord(iRight - iCornerSizeH + x),
				ZCoord(iTop + iCornerSizeV - y + 1)));

			rectsLowerFirst.push_back(ZRect(
				ZCoord(iLeft + iCornerSizeH - x),
				ZCoord(iBottom - iCornerSizeV + y - 1),
				ZCoord(iRight - iCornerSizeH + x),
				ZCoord(iBottom - iCornerSizeV + y)));

			--y;
			twoYASquared -= twoASquared;
			error -= twoYASquared;
			}
		}

	// Second octant, runs from right to top-right
	vector<ZRect> rectsUpperSecond;
	vector<ZRect> rectsLowerSecond;
	x = ZCoord(a);
	y = 0;
	twoXBSquared = x * twoBSquared;
	twoYASquared = 0;
	error = -x * bSquared; // b^2 x^2 + a^2 y^2 - a^2 b^2 - b^2 x
	while (twoXBSquared > twoYASquared)
		{
		rectsUpperSecond.push_back(ZRect(
			ZCoord(iLeft + iCornerSizeH - x),
			ZCoord(iTop + iCornerSizeV - y),
			ZCoord(iRight - iCornerSizeH + x),
			ZCoord(iTop + iCornerSizeV - y + 1)));

		rectsLowerSecond.push_back(ZRect(
			ZCoord(iLeft + iCornerSizeH - x),
			ZCoord(iBottom - iCornerSizeV + y - 1),
			ZCoord(iRight - iCornerSizeH + x),
			ZCoord(iBottom - iCornerSizeV + y)));

		++y;
		twoYASquared += twoASquared;
		error += twoYASquared - aSquared;
		if (error >= 0)
			{
			--x;
			twoXBSquared -= twoBSquared;
			error -= twoXBSquared;
			}
		}

	vector<ZRect> allRects;
	for (vector<ZRect>::iterator i = rectsUpperFirst.begin();
		i != rectsUpperFirst.end(); ++i)
		{
		allRects.push_back(*i);
		}

	for (vector<ZRect>::reverse_iterator i = rectsUpperSecond.rbegin();
		i != rectsUpperSecond.rend(); ++i)
		{
		allRects.push_back(*i);
		}

	for (vector<ZRect>::iterator i = rectsLowerSecond.begin();
		i != rectsLowerSecond.end(); ++i)
		{
		allRects.push_back(*i);
		}

	for (vector<ZRect>::reverse_iterator i = rectsLowerFirst.rbegin();
		i != rectsLowerFirst.rend(); ++i)
		{
		allRects.push_back(*i);
		}

	// Add in the rectangle in the middle, if any. Note that iCornerSizeV
	// was divided by two at the start of this method.
	ZDCRgn tempRgn = ZDCRgn::sRects(&allRects[0], allRects.size(), false);

	tempRgn |= ZRect(iLeft, ZCoord(iTop + iCornerSizeV), iRight,
		ZCoord(iBottom - iCornerSizeV));

	oRep = tempRgn.fRep;

#endif // ZCONFIG_SPI_Enabled(QuickDraw)
	}

// ==================================================
// Line

ZDCRgn ZDCRgn::sLine(ZCoord iStartH, ZCoord iStartV,
	ZCoord iEndH, ZCoord iEndV, ZCoord iLineWidth)
	{
	// For notes on this see AG's log book, 97-03-05
	ZPoint leftMostPoint, rightMostPoint;
	if (iStartH < iEndH)
		{
		leftMostPoint.h = iStartH;
		leftMostPoint.v = iStartV;
		rightMostPoint.h = iEndH;
		rightMostPoint.v = iEndV;
		}
	else
		{
		leftMostPoint.h = iEndH;
		leftMostPoint.v = iEndV;
		rightMostPoint.h = iStartH;
		rightMostPoint.v = iStartV;
		}

	ZPoint thePoints[6];
	// Two cases, from top left down to bottom right, or from bottom left up to top right
	if (leftMostPoint.v < rightMostPoint.v)
		{
		thePoints[0].h = leftMostPoint.h;
		thePoints[0].v = leftMostPoint.v;

		thePoints[1].h = ZCoord(leftMostPoint.h + iLineWidth);
		thePoints[1].v = leftMostPoint.v;

		thePoints[2].h = ZCoord(rightMostPoint.h + iLineWidth);
		thePoints[2].v = rightMostPoint.v;

		thePoints[3].h = ZCoord(rightMostPoint.h + iLineWidth);
		thePoints[3].v = ZCoord(rightMostPoint.v + iLineWidth);

		thePoints[4].h = rightMostPoint.h;
		thePoints[4].v = ZCoord(rightMostPoint.v + iLineWidth);

		thePoints[5].h = leftMostPoint.h;
		thePoints[5].v = ZCoord(leftMostPoint.v + iLineWidth);
		}
	else
		{
		thePoints[0].h = leftMostPoint.h;
		thePoints[0].v = leftMostPoint.v;

		thePoints[1].h = rightMostPoint.h;
		thePoints[1].v = rightMostPoint.v;

		thePoints[2].h = ZCoord(rightMostPoint.h + iLineWidth);
		thePoints[2].v = rightMostPoint.v;

		thePoints[3].h = ZCoord(rightMostPoint.h + iLineWidth);
		thePoints[3].v = ZCoord(rightMostPoint.v + iLineWidth);

		thePoints[4].h = ZCoord(leftMostPoint.h + iLineWidth);
		thePoints[4].v = ZCoord(leftMostPoint.v + iLineWidth);

		thePoints[5].h = leftMostPoint.h;
		thePoints[5].v = ZCoord(leftMostPoint.v + iLineWidth);
		}
	return ZDCRgn::sPoly(thePoints, 6);
	}

// ==================================================
// Rect

ZDCRgn ZDCRgn::sRect(const ZRect& iRect)
	{
	ZDCRgn theRgn;
	sInternal_CreateRep_Rect(theRgn.fRep, iRect.left, iRect.top, iRect.right, iRect.bottom);
	return theRgn;
	}

ZDCRgn ZDCRgn::sRect(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom)
	{
	ZDCRgn theRgn;
	sInternal_CreateRep_Rect(theRgn.fRep, iLeft, iTop, iRight, iBottom);
	return theRgn;
	}

ZDCRgn ZDCRgn::sRects(const ZRect* iRects, size_t iCount, bool iAlreadySorted)
	{
	ZDCRgn resultRgn;

	if (iCount == 0)
		{}
	else if (iCount == 1)
		{
		resultRgn = iRects[0];
		}

#if ZCONFIG_SPI_Enabled(GDI)

	else if (iAlreadySorted && !ZUtil_Win::sIsWinNT())
		{
		// Windows NT is not happy with our call to ExtCreateRegion, so we don't use it on NT.
		vector<char> rawStorage(iCount * sizeof(RECT) + sizeof(RGNDATAHEADER));

		RGNDATA* theRGNDATA = reinterpret_cast<RGNDATA*>(&rawStorage[0]);

		// Set up bounds so right is to the left of left, and bottom is above top. The
		// first good rectangle we compare against will set all four fields. Each
		// additional rectangle may further expand the overall bounds.
		RECT bounds;
		bounds.top = LONG_MAX;
		bounds.left = LONG_MAX;
		bounds.right = LONG_MIN;
		bounds.bottom = LONG_MIN;

		RECT* currentDest = reinterpret_cast<RECT*>(theRGNDATA->Buffer);
		size_t realCount = 0;
		while (iCount--)
			{
			RECT currentRect = *iRects++;
			// Only non empty rectangles contribute to the final region
			if (currentRect.right <= currentRect.left || currentRect.bottom <= currentRect.top)
				continue;
			++realCount;
			if (bounds.left > currentRect.left)
				bounds.left = currentRect.left;
			if (bounds.top > currentRect.top)
				bounds.top = currentRect.top;
			if (bounds.right < currentRect.right)
				bounds.right = currentRect.right;
			if (bounds.bottom < currentRect.bottom)
				bounds.bottom = currentRect.bottom;
			*currentDest++ = currentRect;
			}

		if (realCount > 0)
			{
			// If we got some non empty rectangles then we've got real data to use, plus our
			// overall bounds must be valid. Fill out the rest of the region data structure.
			theRGNDATA->rdh.dwSize = sizeof(RGNDATAHEADER);
			theRGNDATA->rdh.iType = RDH_RECTANGLES;

			theRGNDATA->rdh.rcBound = bounds;
			theRGNDATA->rdh.nCount = realCount;
			theRGNDATA->rdh.nRgnSize = realCount * sizeof(RECT);

			XFORM theXFORM;
			theXFORM.eM11 = 1.0f; // hScale
			theXFORM.eM12 = 0.0f; // skew
			theXFORM.eM21 = 0.0f; // skew
			theXFORM.eM22 = 1.0f; // vScale
			theXFORM.eDx = 0.0f; // hOffset
			theXFORM.eDy = 0.0f; // vOffset
			HRGN theHRGN = ::ExtCreateRegion(
				&theXFORM,
				realCount * sizeof(RECT) + sizeof(RGNDATAHEADER),
				theRGNDATA);

			resultRgn = ZDCRgn(theHRGN, true);
			}
		}

#elif ZCONFIG_SPI_Enabled(BeOS)

	else if (iAlreadySorted)
		{
		// Be allocates at least 8 slots, then increases by 16 each time it overflows.
		long theAllocatedSize = 8;
		if (iCount >= 8)
			theAllocatedSize += 16 * ((iCount - 8 + 15) / 16);

		clipping_rect* newData =
			reinterpret_cast<clipping_rect*>(::malloc(theAllocatedSize * sizeof(clipping_rect)));

		clipping_rect bounds;
		bounds.top = LONG_MAX;
		bounds.left = LONG_MAX;
		bounds.right = LONG_MIN;
		bounds.bottom = LONG_MIN;

		clipping_rect* currentDest = newData;
		size_t realCount = 0;
		while (iCount--)
			{
			clipping_rect currentRect;
			currentRect.left = iRects->left;
			currentRect.top = iRects->top;
			currentRect.right = iRects->right - 1;
			currentRect.bottom = iRects->bottom - 1;
			++iRects;

			if (iCount > 0)
				{
				// Check that we don't get any out of order rectangles.
				ZAssertStop(kDebug_DCRgn, iRects->top >= iRects[-1].top);
				}

			// Only non empty rectangles contribute to the final region
			if (currentRect.right < currentRect.left || currentRect.bottom < currentRect.top)
				continue;
			++realCount;
			if (bounds.left > currentRect.left)
				bounds.left = currentRect.left;
			if (bounds.top > currentRect.top)
				bounds.top = currentRect.top;
			if (bounds.right < currentRect.right)
				bounds.right = currentRect.right;
			if (bounds.bottom < currentRect.bottom)
				bounds.bottom = currentRect.bottom;
			*currentDest++ = currentRect;
			}

		if (realCount > 0)
			{
			BRegion* theBRegion = new BRegion;
			// We define FakeBRegion to give us public access to BRegion's fields. The virtual
			// destructor must be declared in order that space for a vtbl pointer is allowed for.
			struct FakeBRegion
				{
				virtual ~FakeBRegion() = 0;
				long count;
				long data_size;
				clipping_rect bound;
				clipping_rect *data;
				};

			FakeBRegion* theFakeBRegion = reinterpret_cast<FakeBRegion*>(theBRegion);
			// Stuff the regions fields
			theFakeBRegion->count = realCount;
			theFakeBRegion->data_size = theAllocatedSize;
			theFakeBRegion->bound = bounds;

			// Swap out the clipping_rect storage
			clipping_rect* oldData = theFakeBRegion->data;
			theFakeBRegion->data = newData;

			// And free the old stuff
			free(oldData);
			resultRgn = ZDCRgn(theBRegion, true);
			}
		else
			{
			free(newData);
			}
		}

#endif
	else
		{
		ZDCRgnAccumulator theAccumulator;
		while (iCount--)
			theAccumulator.Add(*iRects++);
		resultRgn = theAccumulator.GetResult();
		}
	return resultRgn;
	}

// ==================================================
// Round rect

ZDCRgn ZDCRgn::sRoundRect(const ZRect& iRect, const ZPoint& iCornerSize)
	{
	ZDCRgn theRgn;
	if (iCornerSize.h <= 1 || iCornerSize.v <= 1)
		{
		sInternal_CreateRep_Rect(theRgn.fRep, iRect.left, iRect.top, iRect.right, iRect.bottom);
		}
	else
		{
		sInternal_CreateRep_RoundRect(theRgn.fRep,
			iRect.left, iRect.top, iRect.right, iRect.bottom, iCornerSize.h, iCornerSize.v);
		}
	return theRgn;
	}

ZDCRgn ZDCRgn::sRoundRect(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom,
	ZCoord iCornerSizeH, ZCoord iCornerSizeV)
	{
	ZDCRgn theRgn;
	if (iCornerSizeH <= 1 || iCornerSizeV <= 1)
		{
		sInternal_CreateRep_Rect(theRgn.fRep, iLeft, iTop, iRight, iBottom);
		}
	else
		{
		sInternal_CreateRep_RoundRect(theRgn.fRep,
			iLeft, iTop, iRight, iBottom, iCornerSizeH, iCornerSizeV);
		}
	return theRgn;
	}

// ==================================================
// Ellipse

ZDCRgn ZDCRgn::sEllipse(const ZRect& iBounds)
	{
	// AG 2000-07-27. We're not using Mac native ellipses because I do not yet know
	// how to match them exactly on other platforms.
#if 0 // ZCONFIG_SPI_Enabled(QuickDraw)
	::OpenRgn();
	Rect tempRect(iBounds);
	::FrameOval(&tempRect);
	ZDCRgn theRgn;
	::CloseRgn(theRgn);
	return theRgn;
#else
	ZDCRgn theRgn;
	ZCoord cornerH = iBounds.Width();
	ZCoord cornerV = iBounds.Height();
	if (cornerH <= 1 || cornerV <= 1)
		{
		sInternal_CreateRep_Rect(theRgn.fRep,
			iBounds.left, iBounds.top, iBounds.right, iBounds.bottom);
		}
	else
		{
		sInternal_CreateRep_RoundRect(theRgn.fRep,
			iBounds.left, iBounds.top, iBounds.right, iBounds.bottom, cornerH, cornerV);
		}
	return theRgn;
#endif
	}

ZDCRgn ZDCRgn::sEllipse(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom)
	{
	// AG 2000-07-27. We're not using Mac native ellipses because I do not yet know
	// how to match them exactly on other platforms.
#if 0 // ZCONFIG_SPI_Enabled(QuickDraw)
	::OpenRgn();
	Rect tempRect;
	tempRect.left = iLeft;
	tempRect.top = iTop;
	tempRect.right = iRight;
	tempRect.bottom = iBottom;
	::FrameOval(&tempRect);
	ZDCRgn theRgn;
	::CloseRgn(theRgn);
	return theRgn;
#else
	ZDCRgn theRgn;
	ZCoord cornerH = ZCoord(iRight - iLeft);
	ZCoord cornerV = ZCoord(iBottom - iTop);
	if (cornerH <= 1 || cornerV <= 1)
		{
		sInternal_CreateRep_Rect(theRgn.fRep, iLeft, iTop, iRight, iBottom);
		}
	else
		{
		sInternal_CreateRep_RoundRect(theRgn.fRep,
			iLeft, iTop, iRight, iBottom, cornerH, cornerV);
		}
	return theRgn;
#endif
	}


// ==================================================
// Polygon

static bool sRegionFromPoly_Accumulator(ZCoord iTopCoord, ZCoord iBottomCoord,
	ZCoord* iCoords, size_t iCoordsCount, void* iRefcon)
	{
	ZAssertStop(kDebug_DCRgn, iTopCoord < iBottomCoord || iCoordsCount == 0);
	ZAssertStop(kDebug_DCRgn, (iCoordsCount % 2) == 0);
	iCoordsCount >>= 1;

	ZDCRgnAccumulator* theAccumulator = reinterpret_cast<ZDCRgnAccumulator*>(iRefcon);

	ZRect theRect(0, iTopCoord, 0, iBottomCoord);

	while (iCoordsCount--)
		{
		theRect.left = *iCoords++;
		theRect.right = *iCoords++;
		if (theRect.left < theRect.right)
			theAccumulator->Add(theRect);
		}
	return false;
	}

static bool sRegionFromPoly_Rects(ZCoord iTopCoord, ZCoord iBottomCoord,
	ZCoord* iCoords, size_t iCoordsCount, void* iRefcon)
	{
	ZAssertStop(kDebug_DCRgn, iTopCoord < iBottomCoord || iCoordsCount == 0);
	ZAssertStop(kDebug_DCRgn, (iCoordsCount % 2) == 0);
	iCoordsCount >>= 1;

	vector<ZRect>* theRectVector = reinterpret_cast<vector<ZRect>*>(iRefcon);

	ZRect theRect(0, iTopCoord, 0, iBottomCoord);

	while (iCoordsCount--)
		{
		theRect.left = *iCoords++;
		theRect.right = *iCoords++;
		if (theRect.left < theRect.right)
			theRectVector->push_back(theRect);
		}
	return false;
	}

#if ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS

namespace ZANONYMOUS {

struct RegionFromPoly_QD_t
	{
	RegionFromPoly_QD_t();
	Rect fBounds;
	size_t fWhichVector;
	vector<ZCoord> fVectors[2];
	vector<short> fRegionData;
	};

RegionFromPoly_QD_t::RegionFromPoly_QD_t()
	{
	fBounds.left = 32767;
	fBounds.top = 32767;
	fBounds.right = -32767;
	fBounds.bottom = -32767;
	fWhichVector = 0;
	}

} // anonymous namespace

static bool sRegionFromPoly_QD(ZCoord iTopCoord, ZCoord iBottomCoord,
	ZCoord* iCoords, size_t iCoordsCount, void* iRefcon)
	{
	RegionFromPoly_QD_t* theState = reinterpret_cast<RegionFromPoly_QD_t*>(iRefcon);

	// Update our vertical limits.
	if (theState->fBounds.top > iTopCoord)
		theState->fBounds.top = iTopCoord;
	if (theState->fBounds.bottom < iTopCoord)
		theState->fBounds.bottom = iTopCoord;

	// Build a vector containing the new set of active coords, and
	// at the same time update our horizontal limits.
	theState->fVectors[1 - theState->fWhichVector].resize(iCoordsCount);
	vector<ZCoord>::iterator newCoordsEnd = theState->fVectors[1 - theState->fWhichVector].begin();
	while (iCoordsCount--)
		{
		ZCoord theCoord = *iCoords++;
		// Update our horizontal limits
		if (theState->fBounds.left > theCoord)
			theState->fBounds.left = theCoord;
		if (theState->fBounds.right < theCoord)
			theState->fBounds.right = theCoord;
		// And transcribe the coordinate
		*newCoordsEnd++ = theCoord;
		}

	// Set up the iterators for the new and old data. newCoordsEnd already
	// references the correct location.
	vector<ZCoord>::iterator currentCoordsIter = theState->fVectors[theState->fWhichVector].begin();
	vector<ZCoord>::iterator currentCoordsEnd = theState->fVectors[theState->fWhichVector].end();
	vector<ZCoord>::iterator newCoordsIter = theState->fVectors[1 - theState->fWhichVector].begin();

	bool writtenData = false;

	// Match up the new coords against the existing ones
	while (currentCoordsIter != currentCoordsEnd && newCoordsIter != newCoordsEnd)
		{
		if (*currentCoordsIter < *newCoordsIter)
			{
			// A coord that was there has gone
			if (!writtenData)
				{
				writtenData = true;
				theState->fRegionData.push_back(iTopCoord);
				}
			theState->fRegionData.push_back(*currentCoordsIter++);
			}
		else if (*currentCoordsIter > *newCoordsIter)
			{
			// A coord that was not there has shown up
			if (!writtenData)
				{
				writtenData = true;
				theState->fRegionData.push_back(iTopCoord);
				}
			theState->fRegionData.push_back(*newCoordsIter++);
			}
		else
			{
			// The coords match, so move both iterators along.
			++currentCoordsIter;
			++newCoordsIter;
			}
		}

	// One or other (or both) of the lists must have been exhausted.
	// Dump all its values into theState->fRegionData.
	while (currentCoordsIter != currentCoordsEnd)
		{
		if (!writtenData)
			{
			writtenData = true;
			theState->fRegionData.push_back(iTopCoord);
			}
		theState->fRegionData.push_back(*currentCoordsIter++);
		}

	while (newCoordsIter != newCoordsEnd)
		{
		if (!writtenData)
			{
			writtenData = true;
			theState->fRegionData.push_back(iTopCoord);
			}
		theState->fRegionData.push_back(*newCoordsIter++);
		}

	if (writtenData)
		{
		// If there were any changes, then switch which vector holds active coordinates and which
		// is the working storage holding new coordinates.
		theState->fWhichVector = 1 - theState->fWhichVector;
		// And we must put in a line terminator.
		theState->fRegionData.push_back(32767);
		}

	return false;
	}
#endif // ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS

ZDCRgn ZDCRgn::sPoly(const ZDCPoly& iPoly, bool iEvenOdd, ZCoord iOffsetH, ZCoord iOffsetV)
	{
	// Prefer GDI HRGN over QD RgnHandle
#if ZCONFIG_SPI_Enabled(GDI)

	POINT* thePOINTs;
	size_t theCount;
	iPoly.GetPOINTs(ZPoint(iOffsetH, iOffsetV), thePOINTs, theCount);
	if (theCount > 2)
		{
		if (HRGN theHRGN = ::CreatePolygonRgn(thePOINTs, theCount, iEvenOdd ? ALTERNATE : WINDING))
			return ZDCRgn(theHRGN, true);
		}
	return ZDCRgn();

#elif ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS

	if (iEvenOdd)
		{
		ZPoint* thePoints;
		size_t theCount;
		iPoly.GetPoints(ZPoint(iOffsetH, iOffsetV), thePoints, theCount);
		if (theCount > 2)
			{
			PenState oldPenState;
			::GetPenState(&oldPenState);

			::OpenRgn();

			::MoveTo(thePoints[0].h, thePoints[0].v);

			for (size_t x = 1; x < theCount; ++x)
				::MacLineTo(thePoints[x].h, thePoints[x].v);

			if (thePoints[0].h != thePoints[theCount-1].h
				|| thePoints[0].v != thePoints[theCount-1].v)
				{
				::MacLineTo(thePoints[0].h + iOffsetH, thePoints[0].v);
				}

			ZDCRgn theRgn;
			::CloseRgn(theRgn.GetRgnHandle());

			::SetPenState(&oldPenState);
			return theRgn;
			}
		}
	else
		{
		// For Winding we have to use our hand rolled implementation.
		RegionFromPoly_QD_t theState;

		iPoly.Decompose(iEvenOdd, iOffsetH, iOffsetV, sRegionFromPoly_QD, &theState);

		if (theState.fRegionData.size())
			{
			theState.fRegionData.push_back(32767);
			size_t theSize = sizeof(MacRegion) + sizeof(short) * theState.fRegionData.size();
			if (theSize < 60000)
				{
				RgnHandle theRgnHandle = (RgnHandle)::NewHandle(theSize);
				theRgnHandle[0]->rgnSize = theSize;
				theRgnHandle[0]->rgnBBox = theState.fBounds;
				ZBlockCopy(&theState.fRegionData[0],
					&theRgnHandle[0][1], sizeof(short) * theState.fRegionData.size());
				return ZDCRgn(theRgnHandle, true);
				}
			}
		}
	return ZDCRgn();

#elif ZCONFIG_SPI_Enabled(X11)

	XPoint* theXPoints;
	size_t theCount;
	iPoly.GetXPoints(ZPoint(iOffsetH, iOffsetV), theXPoints, theCount);
	if (theCount > 2)
		{
		if (Region theXRegion = ::XPolygonRegion(theXPoints, theCount,
			iEvenOdd ? EvenOddRule : WindingRule))
			{
			return ZDCRgn(theXRegion, true);
			}
		}
	return ZDCRgn();

#elif ZCONFIG_SPI_Enabled(BeOS)

	vector<ZRect> theRects;
	iPoly.Decompose(iEvenOdd, iOffsetH, iOffsetV, sRegionFromPoly_Rects, &theRects);
	if (theRects.size())
		return ZDCRgn::sRects(theRects.begin(), theRects.size(), true);
	return ZDCRgn();

#else

	ZDCRgnAccumulator theAccumulator;
	iPoly.Decompose(iEvenOdd, iOffsetH, iOffsetV, sRegionFromPoly_Accumulator, &theAccumulator);
	return theAccumulator.GetResult();

#endif
	}

ZDCRgn ZDCRgn::sPoly(const ZPoint* iPoints, size_t iCount, bool iEvenOdd,
	ZCoord iOffsetH, ZCoord iOffsetV)
	{
	ZDCRgn resultRgn;
	if (iCount <= 2)
		return resultRgn;
	// Prefer GDI HRGN over QD RgnHandle

#if ZCONFIG_SPI_Enabled(GDI)

	vector<POINT> thePOINTs(iCount);
	for (size_t x = 0; x < iCount; ++x)
		{
		thePOINTs[x].x = iPoints[x].h + iOffsetH;
		thePOINTs[x].y = iPoints[x].v + iOffsetV;
		}
	if (HRGN theHRGN = ::CreatePolygonRgn(&thePOINTs[0], iCount, iEvenOdd ? ALTERNATE : WINDING))
		resultRgn = ZDCRgn(theHRGN, true);

#elif ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS

	if (iEvenOdd)
		{
		// For EvenOdd we can use the Mac's native implementation.
		PenState oldPenState;
		::GetPenState(&oldPenState);

		::OpenRgn();

		::MoveTo(iPoints[0].h + iOffsetH, iPoints[0].v + iOffsetV);

		for (size_t x = 1; x < iCount; ++x)
			::MacLineTo(iPoints[x].h + iOffsetH, iPoints[x].v + iOffsetV);

		if (iPoints[0].h != iPoints[iCount-1].h || iPoints[0].v != iPoints[iCount-1].v)
			::MacLineTo(iPoints[0].h + iOffsetH, iPoints[0].v + iOffsetV);

		::CloseRgn(resultRgn.GetRgnHandle());

		::SetPenState(&oldPenState);
		}
	else
		{
		// For Winding we have to use our hand rolled implementation.
		RegionFromPoly_QD_t theState;

		ZDCPoly::sDecompose(iPoints, iCount, iEvenOdd, iOffsetH, iOffsetV,
			sRegionFromPoly_QD, &theState);

		if (theState.fRegionData.size())
			{
			theState.fRegionData.push_back(32767);
			size_t theSize = sizeof(MacRegion) + sizeof(short) * theState.fRegionData.size();
			if (theSize < 60000)
				{
				RgnHandle theRgnHandle = (RgnHandle)::NewHandle(theSize);
				theRgnHandle[0]->rgnSize = theSize;
				theRgnHandle[0]->rgnBBox = theState.fBounds;
				ZBlockCopy(&theState.fRegionData[0],
					&theRgnHandle[0][1], sizeof(short) * theState.fRegionData.size());
				resultRgn = ZDCRgn(theRgnHandle, true);
				}
			}
		}

#elif ZCONFIG_SPI_Enabled(X11)

	vector<XPoint> theXPoints(iCount);
	for (size_t x = 0; x < iCount; ++x)
		{
		theXPoints[x].x = iPoints[x].h + iOffsetH;
		theXPoints[x].y = iPoints[x].v + iOffsetV;
		}
	if (Region theXRegion = ::XPolygonRegion(&theXPoints[0], iCount,
		iEvenOdd ? EvenOddRule : WindingRule))
		{
		resultRgn = ZDCRgn(theXRegion, true);
		}

#elif ZCONFIG_SPI_Enabled(BeOS)

	vector<ZRect> theRects;
	ZDCPoly::sDecompose(iPoints, iCount, iEvenOdd, iOffsetH, iOffsetV,
		sRegionFromPoly_Rects, &theRects);

	if (theRects.size())
		resultRgn = ZDCRgn::sRects(&theRects[0], theRects.size(), true);

#else

	ZDCRgnAccumulator theAccumulator;
	ZDCPoly::sDecompose(iPoints, iCount, iEvenOdd, iOffsetH, iOffsetV,
		sRegionFromPoly_Accumulator, &theAccumulator);

	resultRgn = theAccumulator.GetResult();

#endif

	return resultRgn;
	}

// ==================================================

ZDCRgn::ZDCRgn(const ZRect& iRect)
	{ sInternal_CreateRep_Rect(fRep, iRect.left, iRect.top, iRect.right, iRect.bottom); }

ZDCRgn::ZDCRgn(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom)
	{ sInternal_CreateRep_Rect(fRep, iLeft, iTop, iRight, iBottom); }

ZDCRgn& ZDCRgn::operator=(const ZRect& iRect)
	{
	if (iRect.right <= iRect.left || iRect.bottom <= iRect.top)
		{
		// AG 2001-01-10. Another place where we need to filter empty rectangles.
		fRep.Clear();
		}

	#if ZCONFIG_SPI_Enabled(GDI)
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
		{
		this->Internal_Touch();
		::SetRectRgn(fRep->fHRGN, iRect.left, iRect.top, iRect.right, iRect.bottom);
		}
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
		{
		this->Internal_Touch();
		::MacSetRectRgn(fRep->fRgnHandle, iRect.left, iRect.top, iRect.right, iRect.bottom);
		}
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
		{
		XRectangle tempRect;
		tempRect.x = iRect.left;
		tempRect.y = iRect.top;
		tempRect.width = iRect.Width();
		tempRect.height = iRect.Height();
		Region newRegion = ::XCreateRegion();
		::XUnionRectWithRegion(&tempRect, newRegion, newRegion);
		fRep = new Rep;
		StoreFormat(fRep->fFormat, ZCONFIG_API_Graphics_X);
		fRep->fRegion = newRegion;
		}
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
		{
		this->Internal_Touch();
		::ZBRegionAlternate_Set(fRep->fBRegion, iRect.left, iRect.top,
			iRect.right - 1, iRect.bottom - 1);
		}
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
		{
		this->Internal_Touch();
		clipping_rect tempRect;
		tempRect.left = iRect.left;
		tempRect.top = iRect.top;
		tempRect.right = iRect.right - 1;
		tempRect.bottom = iRect.bottom - 1;
		fRep->fBRegion->Set(tempRect);
		}
	#endif

	#if 1//ZCONFIG_SPI_Enabled(ZooLib)
	else if (!fRep || CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
		{
		fRep = new Rep;
		fRep->fBigRegion = new ZBigRegion(ZRect_T<int32>(iRect));
		StoreFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib);
		}
	#endif

	return *this;
	}

// ==================================================

#if ZCONFIG_SPI_Enabled(QuickDraw)

ZDCRgn::ZDCRgn(RgnHandle iRgnHandle, bool iAdopt)
	{
	fRep = new Rep;
	StoreFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD);
	if (iAdopt)
		{
		fRep->fRgnHandle = iRgnHandle;
		}
	else
		{
		fRep->fRgnHandle = ::NewRgn();
		::MacCopyRgn(iRgnHandle, fRep->fRgnHandle);
		}
	}

ZDCRgn& ZDCRgn::operator=(RgnHandle iRgnHandle)
	{
	TouchAndSetFormat(this, ZCONFIG_API_Graphics_QD);
	::MacCopyRgn(iRgnHandle, fRep->fRgnHandle);
	return *this;
	}

RgnHandle ZDCRgn::GetRgnHandle()
	{
	TouchAndSetFormat(this, ZCONFIG_API_Graphics_QD);
	return fRep->fRgnHandle;
	}

#endif // ZCONFIG_SPI_Enabled(QuickDraw)

// ==================================================

#if ZCONFIG_SPI_Enabled(GDI)

ZDCRgn::ZDCRgn(HRGN iHRGN, bool iAdopt)
	{
	fRep = new Rep;
	StoreFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI);
	if (iAdopt)
		{
		fRep->fHRGN = iHRGN;
		}
	else
		{
		fRep->fHRGN = ::CreateRectRgn(0,0,0,0);
		::CombineRgn(fRep->fHRGN, iHRGN, nil, RGN_COPY);
		}
	}

ZDCRgn& ZDCRgn::operator=(HRGN iHRGN)
	{
	TouchAndSetFormat(this, ZCONFIG_API_Graphics_GDI);
	::CombineRgn(fRep->fHRGN, iHRGN, nil, RGN_COPY);
	return *this;
	}

HRGN ZDCRgn::GetHRGN()
	{
	TouchAndSetFormat(this, ZCONFIG_API_Graphics_GDI);
	return fRep->fHRGN;
	}

#endif // ZCONFIG_SPI_Enabled(GDI)

// ==================================================

#if ZCONFIG_SPI_Enabled(X11)

ZDCRgn::ZDCRgn(Region iRegion, bool iAdopt)
	{
	fRep = new Rep;
	StoreFormat(fRep->fFormat, ZCONFIG_API_Graphics_X);
	if (iAdopt)
		{
		fRep->fRegion = iRegion;
		}
	else
		{
		fRep->fRegion = ::XCreateRegion();
		::XUnionRegion(iRegion, fRep->fRegion, fRep->fRegion);
		}
	}

ZDCRgn& ZDCRgn::operator=(Region iRegion)
	{
	TouchAndSetFormat(this, ZCONFIG_API_Graphics_X);
	::XUnionRegion(iRegion, iRegion, fRep->fRegion);
	return *this;
	}

Region ZDCRgn::GetRegion()
	{
	TouchAndSetFormat(this, ZCONFIG_API_Graphics_X);
	return fRep->fRegion;
	}

#endif // ZCONFIG_SPI_Enabled(X11)

// ==================================================

#if ZCONFIG_SPI_Enabled(BeOS)

ZDCRgn::ZDCRgn(BRegion* iBRegion, bool iAdopt)
	{
	fRep = new Rep;
	StoreFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be);
	if (iAdopt)
		{
		fRep->fBRegion = iBRegion;
		}
	else
		{
		fRep->fBRegion = new BRegion(*iBRegion);
		}
	}

ZDCRgn& ZDCRgn::operator=(BRegion* iBRegion)
	{
	TouchAndSetFormat(this, ZCONFIG_API_Graphics_Be);

	#if ZCONFIG_BRegionAlternateEnabled
	::ZBRegionAlternate_Assign(fRep->fBRegion, iBRegion);
	#else
	*fRep->fBRegion = *iBRegion;
	#endif

	return *this;
	}

BRegion* ZDCRgn::GetBRegion()
	{
	TouchAndSetFormat(this, ZCONFIG_API_Graphics_Be);
	return fRep->fBRegion;
	}

#endif // ZCONFIG_SPI_Enabled(BeOS)

// ==================================================

#if ZCONFIG_SPI_Enabled(GDI)

void ZDCRgn::MakeScale(float m11, float m12, float m21, float m22, float hOffset, float vOffset)
	{
	TouchAndSetFormat(this, ZCONFIG_API_Graphics_GDI);
	long regionLength = ::GetRegionData(fRep->fHRGN, 0, nil);
	vector<char> rawStorage(regionLength);
	RGNDATA* regionData = reinterpret_cast<RGNDATA*>(&rawStorage[0]);
	::GetRegionData(fRep->fHRGN, regionLength, regionData);
	XFORM theXFORM;
	theXFORM.eM11 = m11;
	theXFORM.eM12 = m12;
	theXFORM.eM21 = m21;
	theXFORM.eM22 = m22;
	theXFORM.eDx = hOffset;
	theXFORM.eDy = vOffset;
	HRGN newHRGN = ::ExtCreateRegion(&theXFORM, regionLength, regionData);
	::DeleteObject(fRep->fHRGN);
	fRep->fHRGN = newHRGN;
	}

#endif // ZCONFIG_SPI_Enabled(GDI)

bool ZDCRgn::Contains(ZCoord iH, ZCoord iV) const
	{
	if (fRep)
		{
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			{
			Point thePoint;
			thePoint.h = iH;
			thePoint.v = iV;
			return ::PtInRgn(thePoint, fRep->fRgnHandle);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			return ::PtInRegion(fRep->fHRGN, iH, iV);
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			return ::XPointInRegion(fRep->fRegion, iH, iV);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			return ::ZBRegionAlternate_Contains(fRep->fBRegion, iH, iV);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			return fRep->fBRegion->Contains(iH, iV);
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			return fRep->fBigRegion->Contains(iH, iV);
		#endif
		}
	return false;
	}

void ZDCRgn::MakeOutline()
	{ *this -= this->Inset(1,1); }

ZDCRgn ZDCRgn::Outline() const
	{ return *this - this->Inset(1,1); }


#if ZCONFIG_SPI_Enabled(GDI)

static void CompressHRGN(HRGN iHRGN, HRGN tempHRGN1, HRGN tempHRGN2,
	ZCoord iDelta, bool iHorizontal, bool iGrow)
	{
	int combineOperation = iGrow ? RGN_OR : RGN_AND;
	ZCoord shift = 1;
	::CombineRgn(tempHRGN1, iHRGN, nil, RGN_COPY);
	while (iDelta)
		{
		if (iDelta & shift)
			{
			if (iHorizontal)
				::OffsetRgn(iHRGN, -shift, 0);
			else
				::OffsetRgn(iHRGN, 0, -shift);
			::CombineRgn(iHRGN, iHRGN, tempHRGN1, combineOperation);
			iDelta -= shift;
			if (iDelta == 0)
				break;
			}
		::CombineRgn(tempHRGN2, tempHRGN1, nil, RGN_COPY);
		if (iHorizontal)
			::OffsetRgn(tempHRGN1, -shift, 0);
		else
			::OffsetRgn(tempHRGN1, 0, -shift);
		::CombineRgn(tempHRGN1, tempHRGN1, tempHRGN2, combineOperation);
		shift <<= 1;
		}
	}

#endif // ZCONFIG_SPI_Enabled(GDI)


#if ZCONFIG_SPI_Enabled(BeOS)

static void CompressBRegion(BRegion* iBRegion, BRegion* tempBRegion1, BRegion* tempBRegion2,
	ZCoord iDelta, bool iHorizontal, bool iGrow)
	{
	ZCoord shift = 1;
	*tempBRegion1 = *iBRegion;

	while (iDelta)
		{
		#if ZCONFIG_BRegionAlternateEnabled

		if (iDelta & shift)
			{
			if (iHorizontal)
				::ZBRegionAlternate_OffsetBy(iBRegion, -shift, 0);
			else
				::ZBRegionAlternate_OffsetBy(iBRegion, 0, -shift);

			if (iGrow)
				::ZBRegionAlternate_Include(iBRegion, tempBRegion1);
			else
				::ZBRegionAlternate_IntersectWith(iBRegion, tempBRegion1);
			iDelta -= shift;
			if (iDelta == 0)
				break;
			}

		*tempBRegion2 = *tempBRegion1;

		if (iHorizontal)
			::ZBRegionAlternate_OffsetBy(tempBRegion1, -shift, 0);
		else
			::ZBRegionAlternate_OffsetBy(tempBRegion1, 0, -shift);

		if (iGrow)
			::ZBRegionAlternate_Include(tempBRegion1, tempBRegion2);
		else
			::ZBRegionAlternate_IntersectWith(tempBRegion1, tempBRegion2);

		shift <<= 1;

		#else 

		if (iDelta & shift)
			{
			if (iHorizontal)
				iBRegion->OffsetBy(-shift, 0);
			else
				iBRegion->OffsetBy(0, -shift);

			if (iGrow)
				iBRegion->Include(tempBRegion1);
			else
				iBRegion->IntersectWith(tempBRegion1);

			iDelta -= shift;
			if (iDelta == 0)
				break;
			}

		*tempBRegion2 = *tempBRegion1;

		if (iHorizontal)
			tempBRegion1->OffsetBy(-shift, 0);
		else
			tempBRegion1->OffsetBy(0, -shift);

		if (iGrow)
			tempBRegion1->Include(tempBRegion2);
		else
			tempBRegion1->IntersectWith(tempBRegion2);

		shift <<= 1;

		#endif
		}
	}

#endif // ZCONFIG_SPI_Enabled(BeOS)


void ZDCRgn::MakeInset(ZCoord iInsetH, ZCoord iInsetV)
	{
	if (!fRep || (iInsetH == 0 && iInsetV == 0))
		return;
	this->Internal_Touch();

	if (false) {}

	#if ZCONFIG_SPI_Enabled(QuickDraw)
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
		::InsetRgn(fRep->fRgnHandle, iInsetH, iInsetV);
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
		{
		// AG 98-05-06. This logic was lifted from XFree86's Region.c -- who knows
		// where it came from originally.
		RECT theBounds;
		int regionType = ::GetRgnBox(fRep->fHRGN, &theBounds);
		if (regionType == SIMPLEREGION)
			{
			::SetRectRgn(fRep->fHRGN, theBounds.left + iInsetH, theBounds.top + iInsetV,
				theBounds.right - iInsetH, theBounds.bottom - iInsetV);
			}
		else
			{
			HRGN tempHRGN1 = ::CreateRectRgn(0, 0, 0, 0);
			HRGN tempHRGN2 = ::CreateRectRgn(0, 0, 0, 0);
			bool grow = iInsetH < 0;
			if (grow)
				iInsetH = -iInsetH;
			::CompressHRGN(fRep->fHRGN, tempHRGN1, tempHRGN2, 2 * iInsetH, true, grow);
			grow = iInsetV < 0;
			if (grow)
				iInsetV = -iInsetV;
			::CompressHRGN(fRep->fHRGN, tempHRGN1, tempHRGN2, 2 * iInsetV, false, grow);
			::OffsetRgn(fRep->fHRGN, iInsetH, iInsetV);
			::DeleteObject(tempHRGN1);
			::DeleteObject(tempHRGN2);
			}
		}
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
		::XShrinkRegion(fRep->fRegion, iInsetH, iInsetV);
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS)
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
		{
		if (fRep->fBRegion->CountRects() == 1)
			{
			clipping_rect theBounds = fRep->fBRegion->FrameInt();
			theBounds.left += iInsetH;
			theBounds.top += iInsetV;
			theBounds.right -= iInsetH;
			theBounds.bottom -= iInsetV;
			fRep->fBRegion->Set(theBounds);
			}
		else
			{
			BRegion tempBRegion1, tempBRegion2;
			bool grow = iInsetH < 0;
			if (grow)
				iInsetH = -iInsetH;

			::CompressBRegion(fRep->fBRegion, &tempBRegion1, &tempBRegion2,
				2 * iInsetH, true, grow);

			grow = iInsetV < 0;
			if (grow)
				iInsetV = -iInsetV;

			::CompressBRegion(fRep->fBRegion, &tempBRegion1, &tempBRegion2,
				2 * iInsetV, false, grow);

			fRep->fBRegion->OffsetBy(iInsetH, iInsetV);
			}
		}
	#endif

	#if 1//ZCONFIG_SPI_Enabled(ZooLib)
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
		fRep->fBigRegion->MakeInset(iInsetH, iInsetV);
	#endif
	}

void ZDCRgn::Clear()
	{ fRep.Clear(); }

bool ZDCRgn::IsEmpty() const
	{
	if (fRep)
		{
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			return ::EmptyRgn(fRep->fRgnHandle);
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			{
			RECT dummyRect;
			return NULLREGION == ::GetRgnBox(fRep->fHRGN, &dummyRect);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			return ::XEmptyRegion(fRep->fRegion);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			return ::ZBRegionAlternate_IsEmpty(fRep->fBRegion);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			clipping_rect frame = fRep->fBRegion->FrameInt();
			if (fRep->fBRegion->CountRects() != 0)
				{
				ZAssertStop(kDebug_DCRgn, frame.left <= frame.right && frame.top <= frame.bottom);
				return false;
				}
			ZAssertStop(kDebug_DCRgn, frame.left > frame.right || frame.top > frame.bottom);
			}
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			return fRep->fBigRegion->IsEmpty();
		#endif
		}
	return true;
	}

#if ZCONFIG_API_Graphics_Multi || !ZCONFIG_SPI_Enabled(QuickDraw) || OPAQUE_TOOLBOX_STRUCTS
// The complex conditional compilation is because we define an inline version of this if
// we're compiling for QD only with non-opaque toolbox structs.
ZRect ZDCRgn::Bounds() const
	{
	if (fRep)
		{
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw) && OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			{
			Rect theRect;
			::GetRegionBounds(fRep->fRgnHandle, &theRect);
			return theRect;
			}
		#endif

		#if ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			return ZRect(fRep->fRgnHandle[0]->rgnBBox);
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			{
			RECT theRECT;
			::GetRgnBox(fRep->fHRGN, &theRECT);
			return theRECT;
			}
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			{
			XRectangle theBounds;
			::XClipBox(fRep->fRegion, &theBounds);
			return theBounds;
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			if (fRep->fBRegion->CountRects() > 0)
				{
				clipping_rect frame = fRep->fBRegion->FrameInt();
				if (frame.left <= frame.right && frame.top <= frame.bottom)
					return ZRect(frame.left, frame.top, frame.right + 1, frame.bottom + 1);
				}
			}
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			{
			return fRep->fBigRegion->Bounds();
			}
		#endif
		}
	return ZRect::sZero;
	}
#endif // ZCONFIG_API_Graphics_Multi || !ZCONFIG_SPI_Enabled(QuickDraw) || OPAQUE_TOOLBOX_STRUCTS

bool ZDCRgn::IsSimpleRect() const
	{
	if (fRep)
		{
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw) && OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			return ::IsRegionRectangular(fRep->fRgnHandle);
		#endif

		#if ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			return fRep->fRgnHandle[0]->rgnSize == 10;
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			{
			RECT dummyRect;
			return ::GetRgnBox(fRep->fHRGN, &dummyRect) != COMPLEXREGION;
			}
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			{
			// AG 98-08-11. If regions are implemented as a list of rectangles then we could
			// gain access to that list. But there's no guarantee that that's how any particular
			// xlib implementation works. For now we xor our real region with a rectangular
			// region matching our bounds. If the result is empty then our region
			// must match the rectangular region.
			XRectangle theBounds;
			::XClipBox(fRep->fRegion, &theBounds);
			Region tempRgn = ::XCreateRegion();
			::XUnionRectWithRegion(&theBounds, tempRgn, tempRgn);
			::XXorRegion(tempRgn, fRep->fRegion, tempRgn);
			bool isEmpty = ::XEmptyRegion(tempRgn);
			::XDestroyRegion(tempRgn);
			return isEmpty;
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			return fRep->fBRegion->CountRects() <= 1;
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			return fRep->fBigRegion->IsSimpleRect();
		#endif
		}
	return true;
	}

static bool sDecompose_IntoVector(const ZRect& iRect, void* iRefcon)
	{
	reinterpret_cast<vector<ZRect>*>(iRefcon)->push_back(iRect);
	return false;
	}

void ZDCRgn::Decompose(vector<ZRect>& oRects) const
	{
	oRects.erase(oRects.begin(),oRects.end());
	if (!fRep)
		return;
	this->Decompose(sDecompose_IntoVector, &oRects);
	}


#if ZCONFIG_SPI_Enabled(QuickDraw) && OPAQUE_TOOLBOX_STRUCTS

struct DecomposeRgnHandle_t
	{
	ZDCRgn::DecomposeProc fDecomposeProc;
	void* fRefcon;
	int32 fCallsMade;
	};

static OSStatus sDecompose_RgnHandle(UInt16 iMessage, RgnHandle iRgnHandle,
	const Rect* iRect, void* iRefcon)
	{
	if (iMessage == kQDRegionToRectsMsgParse)
		{
		DecomposeRgnHandle_t* theStruct = reinterpret_cast<DecomposeRgnHandle_t*>(iRefcon);
		++theStruct->fCallsMade;
		if (theStruct->fDecomposeProc(*iRect, theStruct->fRefcon))
			return userCanceledErr;
		}
	return noErr;
	}

static RegionToRectsUPP sDecompose_RgnHandleUPP = NewRegionToRectsUPP(sDecompose_RgnHandle);

#endif // ZCONFIG_SPI_Enabled(QuickDraw) && OPAQUE_TOOLBOX_STRUCTS


#if 1//ZCONFIG_SPI_Enabled(ZooLib)

struct DecomposeBigRegion_t
	{
	ZDCRgn::DecomposeProc fDecomposeProc;
	void* fRefcon;
	};

static bool sDecompose_BigRegion(const ZRect_T<int32>& iRect, void* iRefcon)
	{
	DecomposeBigRegion_t* theStruct = static_cast<DecomposeBigRegion_t*>(iRefcon);
	return theStruct->fDecomposeProc(iRect, theStruct->fRefcon);
	}

#endif // ZCONFIG_SPI_Enabled(ZooLib)


long ZDCRgn::Decompose(DecomposeProc iProc, void* iRefcon) const
	{
	// Decompose will call iProc once for each rectangle making up this region, returning
	// the number of calls made. If there are no rectangles (ie it is an empty region), no
	// calls will be made and zero will be returned. iRefcon is passed to iProc on each
	// invocation, see sDecompose_IntoVector above for an example of its use. If iProc
	// returns true, Decompose will stop sending rectangles to it and (currently) will
	// prematurely quit walking the list of rectangles.

	if (!fRep)
		return 0;

	if (false) {}

	#if ZCONFIG_SPI_Enabled(QuickDraw) && OPAQUE_TOOLBOX_STRUCTS
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
		{
		DecomposeRgnHandle_t theStruct;
		theStruct.fDecomposeProc = iProc;
		theStruct.fRefcon = iRefcon;
		theStruct.fCallsMade = 0;
		::QDRegionToRects(fRep->fRgnHandle, kQDParseRegionFromTopLeft,
			sDecompose_RgnHandleUPP, &theStruct);
		return theStruct.fCallsMade;
		}
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
		{
		// Check for the simple case
		if (fRep->fRgnHandle[0]->rgnSize == 10)
			{
			ZRect theRect = fRep->fRgnHandle[0]->rgnBBox;
			if (theRect.IsEmpty())
				return 0 ;
			iProc(theRect, iRefcon);
			return 1;
			}

		// Gotta do some real work
		SignedByte oldState = ::HGetState((Handle)fRep->fRgnHandle);
		::HLock((Handle)fRep->fRgnHandle);

		// Point to the first short in the region
		short* dataPtr = (short*)(&fRep->fRgnHandle[0]->rgnSize);
		// Skip over rgnSize and rgnBBox. We're now pointing at the
		// first short that makes up the region's data.
		dataPtr += 5;

		short lastYCoord = -32767;
		vector<short> transitions;
		bool aborted = false;
		long callbacksMade = 0;
		while (!aborted)
			{
			short currentYCoord = *dataPtr++;
			if (currentYCoord == 32767)
				break;
			for (vector<short>::iterator i = transitions.begin();
				i != transitions.end(); /*no increment*/)
				{
				Rect aRect;
				aRect.top = lastYCoord;
				aRect.bottom = currentYCoord;
				aRect.left = *i++;
				aRect.right = *i++;
				++callbacksMade;
				if (iProc(aRect, iRefcon))
					{
					aborted = true;
					break;
					}
				}
			if (aborted)
				break;
			lastYCoord = currentYCoord;
			vector<short>::iterator iter = transitions.begin();
			while (true)
				{
				short currentX = *dataPtr++;
				if (currentX == 32767)
					break;
				iter = lower_bound(iter, transitions.end(), currentX);
				if (iter != transitions.end() && *iter == currentX)
					iter = transitions.erase(iter);
				else
					{
					iter = transitions.insert(iter, currentX);
					++iter;
					}
				}
			}
		::HSetState((Handle)fRep->fRgnHandle, oldState);
		return callbacksMade;
		}
	#endif // ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS

	#if ZCONFIG_SPI_Enabled(GDI)
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
		{
		RECT bounds;
		int regionType = ::GetRgnBox(fRep->fHRGN, &bounds);
		if (regionType == NULLREGION)
			return 0 ;
		if (regionType == SIMPLEREGION)
			{
			iProc(bounds, iRefcon);
			return 1;
			}
		if (regionType == COMPLEXREGION)
			{
			long regionLength = ::GetRegionData(fRep->fHRGN, 0, nil);
			vector<char> rawStorage(regionLength);
			RGNDATA* regionData = reinterpret_cast<RGNDATA*>(&rawStorage[0]);
			::GetRegionData(fRep->fHRGN, regionLength, regionData);
			RECT* rectArray = (RECT*)(regionData->Buffer);
			long callbacksMade = 0;
			for (long x = 0; x < regionData->rdh.nCount; ++x)
				{
				++callbacksMade;
				if (iProc(rectArray[x], iRefcon))
					break;
				}
			return callbacksMade;
			}
		ZDebugStopf(1, ("ZDCRgn::Decompose, unknown region type"));
		return 0;
		}
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
		{
		_XRegion* skanky = reinterpret_cast<_XRegion*>(fRep->fRegion);
		long callbacksMade = 0;
		for (long x = 0; x < skanky->numRects; ++x)
			{
			++callbacksMade;
			ZRect theRect(skanky->rects[x].x1, skanky->rects[x].y1,
				skanky->rects[x].x2, skanky->rects[x].y2);
			if (iProc(theRect, iRefcon))
				break;
			}
		return callbacksMade;
		}
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS)
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
		{
		long theCount = fRep->fBRegion->CountRects();
		long callbacksMade = 0;
		for (long x = 0; x < theCount; ++x)
			{
			++callbacksMade;
			clipping_rect theRect = fRep->fBRegion->RectAtInt(x);
			if (iProc(ZRect(theRect.left, theRect.top,
				theRect.right + 1, theRect.bottom + 1), iRefcon))
				{
				break;
				}
			}
		return callbacksMade;
		}
	#endif

	#if 1//ZCONFIG_SPI_Enabled(ZooLib)
	else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
		{
		DecomposeBigRegion_t theStruct;
		theStruct.fDecomposeProc = iProc;
		theStruct.fRefcon = iRefcon;
		return fRep->fBigRegion->Decompose(sDecompose_BigRegion, &theStruct);
		}
	#endif

	return 0;
	}

bool ZDCRgn::IsEqualTo(const ZDCRgn& iOther) const
	{
	if (fRep == iOther.fRep)
		{
		return true;
		}
	else if (!fRep)
		{
		return iOther.IsEmpty();
		}
	else if (!iOther.fRep)
		{
		return this->IsEmpty();
		}
	else
		{
		Harmonize(this, iOther);

		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			return ::MacEqualRgn(fRep->fRgnHandle, iOther.fRep->fRgnHandle);
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			return ::EqualRgn(fRep->fHRGN, iOther.fRep->fHRGN);
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			return ::XEqualRegion(fRep->fRegion, iOther.fRep->fRegion);
		#endif

		// And there I was, expecting BeOS to provide a full suite of regions ops. Ho hum.
		// If we exclude iOther from us, and exlude us from iOther, and both are
		// empty, then both regions must be the same. (A - B <= 0 and B - A <= 0, then
		// A - B == 0, and A == B). An alternative would be to check that the count
		// of rectangles is identical, and that each rectangle in the two regions match up.
		// That will only work if the underlying implementation guarantees that any region
		// will be represented by a single set of rectangles, regardless of how the region
		// was built up. With the use of ZBRegionAlternate that constraint cannot be maintained,
		// so this approach will have to do.

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			BRegion subResult(*fRep->fBRegion);
			::ZBRegionAlternate_Exclude(&subResult, iOther.fRep->fBRegion);
			clipping_rect frameResult = subResult.FrameInt();
			if (frameResult.left <= frameResult.right && frameResult.top <= frameResult.bottom)
				return false;
			::ZBRegionAlternate_Assign(&subResult, iOther.fRep->fBRegion);
			::ZBRegionAlternate_Exclude(&subResult, fRep->fBRegion);
			frameResult = subResult.FrameInt();
			if (frameResult.left <= frameResult.right && frameResult.top <= frameResult.bottom)
				return false;
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			BRegion subResult(*fRep->fBRegion);
			subResult.Exclude(iOther.fRep->fBRegion);
			clipping_rect frameResult = subResult.FrameInt();
			if (frameResult.left <= frameResult.right && frameResult.top <= frameResult.bottom)
				return false;
			subResult = *iOther.fRep->fBRegion;
			subResult.Exclude(fRep->fBRegion);
			frameResult = subResult.FrameInt();
			if (frameResult.left <= frameResult.right && frameResult.top <= frameResult.bottom)
				return false;
			}
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			return *fRep->fBigRegion == *iOther.fRep->fBigRegion;
		#endif

		}
	return true;
	}

ZDCRgn& ZDCRgn::OffsetBy(ZCoord iOffsetH, ZCoord iOffsetV)
	{
	if (fRep && (iOffsetH != 0 || iOffsetV != 0))
		{
		this->Internal_Touch();

		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			::MacOffsetRgn(fRep->fRgnHandle, iOffsetH, iOffsetV);
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			::OffsetRgn(fRep->fHRGN, iOffsetH, iOffsetV);
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			::XOffsetRegion(fRep->fRegion, iOffsetH, iOffsetV);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			::ZBRegionAlternate_OffsetBy(fRep->fBRegion, iOffsetH, iOffsetV);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			fRep->fBRegion->OffsetBy(iOffsetH, iOffsetV);
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			*fRep->fBigRegion += ZPoint_T<int32>(iOffsetH, iOffsetV);
		#endif
		}
	return *this;
	}

ZDCRgn& ZDCRgn::OffsetByNegative(ZCoord iOffsetH, ZCoord iOffsetV)
	{
	if (fRep && (iOffsetH != 0 || iOffsetV != 0))
		{
		this->Internal_Touch();

		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			::MacOffsetRgn(fRep->fRgnHandle, short(-iOffsetH), short(-iOffsetV));
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			::OffsetRgn(fRep->fHRGN, -iOffsetH, -iOffsetV);
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			::XOffsetRegion(fRep->fRegion, -iOffsetH, -iOffsetV);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			::ZBRegionAlternate_OffsetBy(fRep->fBRegion, -iOffsetH, -iOffsetV);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			fRep->fBRegion->OffsetBy(-iOffsetH, -iOffsetV);
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			*fRep->fBigRegion += ZPoint_T<int32>(-iOffsetH, -iOffsetV);
		#endif
		}
	return *this;
	}

// ----------
// In the past we handled applying rectangles to regions by allowing a temporary ZDCRgn to
// be constructed. This works, but some platforms have alternative APIs that let us explicitly
// apply rectangles (eg XUnionRectWithRegion). I'm assuming those APIs will be more efficient
// than our blind creation of temporary regions. There are also some special cases we can detect,
// at least on Mac, which allow us to do significantly less work.

ZDCRgn& ZDCRgn::Include(const ZRect& iRect)
	{
	if (!fRep)
		{
		sInternal_CreateRep_Rect(fRep, iRect.left, iRect.top, iRect.right, iRect.bottom);
		}
	else if (!iRect.IsEmpty())
		{
		this->Internal_Touch();

		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw) && OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			{
			RgnHandle tempRgn = ::NewRgn();
			::MacSetRectRgn(tempRgn, iRect.left, iRect.top, iRect.right, iRect.bottom);
			::MacUnionRgn(fRep->fRgnHandle, tempRgn, fRep->fRgnHandle);
			::DisposeRgn(tempRgn);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			{
			// If iRect completely subsumes our regions bounding
			// box, then our region will become iRect.
			if (iRect.left <= fRep->fRgnHandle[0]->rgnBBox.left
				&& iRect.top <= fRep->fRgnHandle[0]->rgnBBox.top
				&& iRect.right >= fRep->fRgnHandle[0]->rgnBBox.right
				&& iRect.bottom >= fRep->fRgnHandle[0]->rgnBBox.bottom)
				{
				::MacSetRectRgn(fRep->fRgnHandle, iRect.left, iRect.top, iRect.right, iRect.bottom);
				}
			else
				{
				RgnHandle tempRgn = ::NewRgn();
				::MacSetRectRgn(tempRgn, iRect.left, iRect.top, iRect.right, iRect.bottom);
				::MacUnionRgn(fRep->fRgnHandle, tempRgn, fRep->fRgnHandle);
				::DisposeRgn(tempRgn);
				}
			}
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			{
			HRGN tempHRGN = ::CreateRectRgn(iRect.left, iRect.top, iRect.right, iRect.bottom);
			::CombineRgn(fRep->fHRGN, fRep->fHRGN, tempHRGN, RGN_OR);
			::DeleteObject(tempHRGN);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			{
			XRectangle tempRect;
			tempRect.x = iRect.left;
			tempRect.y = iRect.top;
			tempRect.width = iRect.right - iRect.left;
			tempRect.height = iRect.bottom - iRect.top;
			::XUnionRectWithRegion(&tempRect, fRep->fRegion, fRep->fRegion);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			::ZBRegionAlternate_Include(fRep->fBRegion, iRect.left, iRect.top,
				iRect.right - 1, iRect.bottom - 1);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			clipping_rect theRect;
			theRect.left = iRect.left;
			theRect.top = iRect.top;
			theRect.right = iRect.right - 1;
			theRect.bottom = iRect.bottom - 1;
			fRep->fBRegion->Include(theRect);
			}
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			*fRep->fBigRegion |= ZRect_T<int32>(iRect);
		#endif
		}
	return *this;
	}

ZDCRgn& ZDCRgn::Intersect(const ZRect& iRect)
	{
	if (iRect.IsEmpty())
		{
		fRep.Clear();
		}
	else if (fRep)
		{
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw) && OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			{
			this->Internal_Touch();
			RgnHandle tempRgn = ::NewRgn();
			::MacSetRectRgn(tempRgn, iRect.left, iRect.top, iRect.right, iRect.bottom);
			::SectRgn(fRep->fRgnHandle, tempRgn, fRep->fRgnHandle);
			::DisposeRgn(tempRgn);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			{
			if (iRect.left >= fRep->fRgnHandle[0]->rgnBBox.right
				|| iRect.top >= fRep->fRgnHandle[0]->rgnBBox.bottom
				|| iRect.right <= fRep->fRgnHandle[0]->rgnBBox.left
				|| iRect.bottom <= fRep->fRgnHandle[0]->rgnBBox.top)
				{
				// iRect trivially has no intersection with our rep, so our result is empty.
				fRep.Clear();
				}
			else if (iRect.left > fRep->fRgnHandle[0]->rgnBBox.left
				|| iRect.top > fRep->fRgnHandle[0]->rgnBBox.top
				|| iRect.right < fRep->fRgnHandle[0]->rgnBBox.right
				|| iRect.bottom < fRep->fRgnHandle[0]->rgnBBox.bottom)
				{
				// iRect does not completley subsume our bounds, so we
				// have to let the real API do some work.
				this->Internal_Touch();
				RgnHandle tempRgn = ::NewRgn();
				::MacSetRectRgn(tempRgn, iRect.left, iRect.top, iRect.right, iRect.bottom);
				::SectRgn(fRep->fRgnHandle, tempRgn, fRep->fRgnHandle);
				::DisposeRgn(tempRgn);
				}
			}
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			{
			this->Internal_Touch();
			HRGN tempHRGN = ::CreateRectRgn(iRect.left, iRect.top, iRect.right, iRect.bottom);
			::CombineRgn(fRep->fHRGN, fRep->fHRGN, tempHRGN, RGN_AND);
			::DeleteObject(tempHRGN);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			{
			this->Internal_Touch();
			Region tempRegion = ::XCreateRegion();
			XRectangle tempRect;
			tempRect.x = iRect.left;
			tempRect.y = iRect.top;
			tempRect.width = iRect.right - iRect.left;
			tempRect.height = iRect.bottom - iRect.top;
			::XUnionRectWithRegion(&tempRect, tempRegion, tempRegion);
			::XIntersectRegion(fRep->fRegion, tempRegion, fRep->fRegion);
			::XDestroyRegion(tempRegion);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			this->Internal_Touch();
			::ZBRegionAlternate_IntersectWith(fRep->fBRegion, iRect.left, iRect.top,
				iRect.right - 1, iRect.bottom - 1);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			this->Internal_Touch();
			clipping_rect tempRect;
			tempRect.left = iRect.left;
			tempRect.top = iRect.top;
			tempRect.right = iRect.right - 1;
			tempRect.bottom = iRect.bottom - 1;
			BRegion tempBRegion;
			tempBRegion.Set(tempRect);
			fRep->fBRegion->IntersectWith(&tempBRegion);
			}
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			{
			this->Internal_Touch();
			*fRep->fBigRegion &= ZRect_T<int32>(iRect);
			}
		#endif
		}
	return *this;
	}

ZDCRgn& ZDCRgn::Exclude(const ZRect& iRect)
	{
	if (fRep && !iRect.IsEmpty())
		{
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw) && OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			{
			this->Internal_Touch();
			RgnHandle tempRgn = ::NewRgn();
			::MacSetRectRgn(tempRgn, iRect.left, iRect.top, iRect.right, iRect.bottom);
			::DiffRgn(fRep->fRgnHandle, tempRgn, fRep->fRgnHandle);
			::DisposeRgn(tempRgn);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(QuickDraw) && !OPAQUE_TOOLBOX_STRUCTS
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			{
			if (iRect.left <= fRep->fRgnHandle[0]->rgnBBox.left
				&& iRect.top <= fRep->fRgnHandle[0]->rgnBBox.top
				&& iRect.right >= fRep->fRgnHandle[0]->rgnBBox.right
				&& iRect.bottom >= fRep->fRgnHandle[0]->rgnBBox.bottom)
				{
				// iRect completely subsumes our bounding box, the result is empty.
				fRep.Clear();
				}
			else
				{
				this->Internal_Touch();
				RgnHandle tempRgn = ::NewRgn();
				::MacSetRectRgn(tempRgn, iRect.left, iRect.top, iRect.right, iRect.bottom);
				::DiffRgn(fRep->fRgnHandle, tempRgn, fRep->fRgnHandle);
				::DisposeRgn(tempRgn);
				}
			}
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			{
			this->Internal_Touch();
			HRGN tempHRGN = ::CreateRectRgn(iRect.left, iRect.top, iRect.right, iRect.bottom);
			::CombineRgn(fRep->fHRGN, fRep->fHRGN, tempHRGN, RGN_DIFF);
			::DeleteObject(tempHRGN);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			{
			this->Internal_Touch();
			Region tempRegion = ::XCreateRegion();
			XRectangle tempRect;
			tempRect.x = iRect.left;
			tempRect.y = iRect.top;
			tempRect.width = iRect.right - iRect.left;
			tempRect.height = iRect.bottom - iRect.top;
			::XUnionRectWithRegion(&tempRect, tempRegion, tempRegion);
			::XSubtractRegion(fRep->fRegion, tempRegion, fRep->fRegion);
			::XDestroyRegion(tempRegion);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			this->Internal_Touch();
			::ZBRegionAlternate_Exclude(fRep->fBRegion, iRect.left, iRect.top,
				iRect.right - 1, iRect.bottom - 1);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			this->Internal_Touch();
			clipping_rect theRect;
			theRect.left = iRect.left;
			theRect.top = iRect.top;
			theRect.right = iRect.right - 1;
			theRect.bottom = iRect.bottom - 1;
			fRep->fBRegion->Exclude(theRect);
			}
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			{
			this->Internal_Touch();
			*fRep->fBigRegion -= ZBigRegion(iRect);
			}
		#endif
		}
	return *this;
	}

ZDCRgn& ZDCRgn::Include(const ZDCRgn& iOther)
	{
	if (!fRep)
		{
		fRep = iOther.fRep;
		}
	else if (iOther.fRep && fRep != iOther.fRep)
		{
		TouchAndSetFormat(this, iOther.fRep->fFormat);
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			::MacUnionRgn(fRep->fRgnHandle, iOther.fRep->fRgnHandle, fRep->fRgnHandle);
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			::CombineRgn(fRep->fHRGN, fRep->fHRGN, iOther.fRep->fHRGN, RGN_OR);
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			::XUnionRegion(fRep->fRegion, iOther.fRep->fRegion, fRep->fRegion);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			::ZBRegionAlternate_Include(fRep->fBRegion, iOther.fRep->fBRegion);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			fRep->fBRegion->Include(iOther.fRep->fBRegion);
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			*fRep->fBigRegion |= *iOther.fRep->fBigRegion;
		#endif
		}
	return *this;
	}

ZDCRgn& ZDCRgn::Intersect(const ZDCRgn& iOther)
	{
	if (!iOther.fRep)
		{
		fRep.Clear();
		}
	else if (fRep && fRep != iOther.fRep)
		{
		TouchAndSetFormat(this, iOther.fRep->fFormat);
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			::SectRgn(fRep->fRgnHandle, iOther.fRep->fRgnHandle, fRep->fRgnHandle);
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			::CombineRgn(fRep->fHRGN, fRep->fHRGN, iOther.fRep->fHRGN, RGN_AND);
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			::XIntersectRegion(fRep->fRegion, iOther.fRep->fRegion, fRep->fRegion);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			::ZBRegionAlternate_IntersectWith(fRep->fBRegion, iOther.fRep->fBRegion);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			fRep->fBRegion->IntersectWith(iOther.fRep->fBRegion);
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			*fRep->fBigRegion &= *iOther.fRep->fBigRegion;
		#endif
		}
	return *this;
	}

ZDCRgn& ZDCRgn::Exclude(const ZDCRgn& iOther)
	{
	if (fRep && iOther.fRep)
		{
		TouchAndSetFormat(this, iOther.fRep->fFormat);
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			::DiffRgn(fRep->fRgnHandle, iOther.fRep->fRgnHandle, fRep->fRgnHandle);
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			::CombineRgn(fRep->fHRGN, fRep->fHRGN, iOther.fRep->fHRGN, RGN_DIFF);
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			::XSubtractRegion(fRep->fRegion, iOther.fRep->fRegion, fRep->fRegion);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			::ZBRegionAlternate_Exclude(fRep->fBRegion, iOther.fRep->fBRegion);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			fRep->fBRegion->Exclude(iOther.fRep->fBRegion);
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			*fRep->fBigRegion -= *iOther.fRep->fBigRegion;
		#endif
		}
	return *this;
	}

ZDCRgn& ZDCRgn::Xor(const ZDCRgn& iOther)
	{
	if (!fRep)
		{
		fRep = iOther.fRep;
		}
	else if (fRep == iOther.fRep)
		{
		fRep.Clear();
		}
	else if (iOther.fRep)
		{
		TouchAndSetFormat(this, iOther.fRep->fFormat);
		if (false) {}

		#if ZCONFIG_SPI_Enabled(QuickDraw)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_QD))
			::MacXorRgn(fRep->fRgnHandle, iOther.fRep->fRgnHandle, fRep->fRgnHandle);
		#endif

		#if ZCONFIG_SPI_Enabled(GDI)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_GDI))
			::CombineRgn(fRep->fHRGN, fRep->fHRGN, iOther.fRep->fHRGN, RGN_XOR);
		#endif

		#if ZCONFIG_SPI_Enabled(X11)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_X))
			::XXorRegion(fRep->fRegion, iOther.fRep->fRegion, fRep->fRegion);
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			BRegion intersection(*fRep->fBRegion);
			::ZBRegionAlternate_IntersectWith(&intersection, iOther.fRep->fBRegion);
			::ZBRegionAlternate_Include(fRep->fBRegion, iOther.fRep->fBRegion);
			::ZBRegionAlternate_Exclude(fRep->fBRegion, &intersection);
			}
		#endif

		#if ZCONFIG_SPI_Enabled(BeOS) && !ZCONFIG_BRegionAlternateEnabled
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_Be))
			{
			BRegion intersection(*fRep->fBRegion);
			intersection.IntersectWith(iOther.fRep->fBRegion);
			fRep->fBRegion->Include(iOther.fRep->fBRegion);
			fRep->fBRegion->Exclude(&intersection);
			}
		#endif

		#if 1//ZCONFIG_SPI_Enabled(ZooLib)
		else if (CheckFormat(fRep->fFormat, ZCONFIG_API_Graphics_ZooLib))
			*fRep->fBigRegion ^= *iOther.fRep->fBigRegion;
		#endif

		}
	return *this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCRgnAccumulator

// AG 2000-02-17. If we assume that unioning a rectangle into a region requires a linear search
// through the target region, then a region built out of n rectangles will take (n^2)/2 operations.
// Unioning two regions together also requires a linear search, but not for each rectangle in the
// source region -- it's basically a merge sort. So we union rects 0 & 1 together, then 2 & 3, and
// then merge the results. Then we merge 4 & 5, then 6 & 7 and merge those two results, and merge
// that result with the earlier one, and so forth. fStack holds regions from fully merged branches
// of the tree, the number of bit flips in the fCount indicates how much of that stack should be
// merged at each step. It's quite hard to explain in prose, ask me if you're curious. The upshot
// is that we go from O(n^2) operation to O(n log2 n). Which is very noticeable when working with
// hundreds of rectangles.

ZDCRgnAccumulator::ZDCRgnAccumulator()
:	fCount(0)
	{}

ZDCRgnAccumulator::~ZDCRgnAccumulator()
	{}

#if 0
// This implementation does a subtraction, an and, an addition,
// a compare and a branch per set bit. We can expect at four bits to
// be set on average (sum(n=1 to 32) of n/(2^(n-1)). And branches are expensive.
// So the funky version further below is probably going to be cheaper in general.
static inline uint32 sHammingWeight(uint32 iValue)
	{
	uint32 count = 0;
	while (iValue)
		{
		iValue &= iValue - 1;
		++count;
		}
	return count;
	}
#else
// This version does 10 ands, 5 shifts and 5 additions for any value (== 20 operations).
static inline uint32 sHammingWeight(uint32 w)
	{
	uint32 res = (w & 0x55555555) + ((w >> 1) & 0x55555555);
	res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
	res = (res & 0x0F0F0F0F) + ((res >> 4) & 0x0F0F0F0F);
	res = (res & 0x00FF00FF) + ((res >> 8) & 0x00FF00FF);
	return (res & 0x0000FFFF) + ((res >> 16) & 0x0000FFFF);
	}
#endif

void ZDCRgnAccumulator::Add(const ZDCRgn& iRgn)
	{
	fStack.push_back(iRgn);

	uint32 changedBitsCount = sHammingWeight((fCount + 1) ^ fCount);
	++fCount;
	while (--changedBitsCount)
		{
		ZDCRgn tailRgn = fStack.back();
		fStack.pop_back();
		fStack.back() |= tailRgn;
		}
	}

ZDCRgn ZDCRgnAccumulator::GetResult() const
	{
	ZDCRgn result;
	for (vector<ZDCRgn>::const_iterator i = fStack.begin(); i != fStack.end(); ++i)
		result |= *i;
	return result;
	}

// =================================================================================================
