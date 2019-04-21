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

#ifndef __ZooLib_Pixels_BigRegion_h__
#define __ZooLib_Pixels_BigRegion_h__ 1
#include "zconfig.h"

#include "zoolib/PairwiseCombiner_T.h"

#include "zoolib/Pixels/Geom.h"

#include <vector>

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - BigRegion

// AG 98-08-13. BigRegion provides a region-type facility that works with 32 bit coordinates.
// The implementation is ripped from the XFree86 source.

class BigRegion
	{
public:
	static BigRegion sRects(const RectPOD* iRects, size_t iCount, bool iAlreadySorted);

	BigRegion();
	BigRegion(const BigRegion& iOther);
	BigRegion(const RectPOD& iBounds);
	BigRegion(const PointPOD& iSize);
	~BigRegion();

	BigRegion& operator=(const BigRegion& iOther);
	BigRegion& operator=(const RectPOD& iBounds);

	BigRegion Inset(const PointPOD& iInset) const
		{ return this->Inset(iInset.h, iInset.v); }
	BigRegion Inset(int32 iInsetH, int32 iInsetV) const
		{
		BigRegion tempRgn(*this);
		tempRgn.MakeInset(iInsetH, iInsetV);
		return tempRgn;
		}

	void MakeInset(const PointPOD& iInset)
		{ this->MakeInset(iInset.h, iInset.v); }
	void MakeInset(int32 iInsetH, int32 iInsetV);

	bool Contains(const PointPOD& iPoint) const;
	bool Contains(int32 iH, int32 iV) const;

	void MakeEmpty();
	bool IsEmpty() const;

	RectPOD Bounds() const;
	bool IsSimpleRect() const;

	void Decompose(std::vector<RectPOD>& oRects) const;
	typedef bool (*DecomposeProc)(const RectPOD& iRect, void* iRefcon);
	int32 Decompose(DecomposeProc iProc, void* iRefcon) const;

	bool operator==(const BigRegion& iOther) const;
	bool operator!=(const BigRegion& iOther) const
		{ return ! (*this == iOther); }

	BigRegion& operator+=(const PointPOD& iOffset);
	BigRegion operator+(const PointPOD& iOffset) const
		{
		BigRegion newRegion(*this);
		return newRegion += iOffset;
		}

	BigRegion& operator-=(const PointPOD& iOffset)
		{ return *this += sPointPOD(-iOffset.h, -iOffset.v); }
	BigRegion operator-(const PointPOD& iOffset) const
		{
		BigRegion newRegion(*this);
		return newRegion -= iOffset;
		}

	BigRegion& operator|=(const BigRegion& iOther);
	BigRegion operator|(const BigRegion& iOther) const
		{
		BigRegion newRegion(*this);
		return newRegion |= iOther;
		}

	BigRegion& operator&=(const BigRegion& iOther);
	BigRegion operator&(const BigRegion& iOther) const
		{
		BigRegion newRegion(*this);
		return newRegion &= iOther;
		}

	BigRegion& operator-=(const BigRegion& iOther);
	BigRegion operator-(const BigRegion& iOther) const
		{
		BigRegion newRegion(*this);
		return newRegion -= iOther;
		}

	BigRegion& operator^=(const BigRegion& iOther);
	BigRegion operator^(const BigRegion& iOther) const
		{
		BigRegion newRegion(*this);
		return newRegion ^= iOther;
		}

	static void sUnion(const BigRegion& iSource1, const BigRegion& iSource2,
		BigRegion& oDestination);

	static void sIntersection(const BigRegion& iSource1, const BigRegion& iSource2,
		BigRegion& oDestination);

	static void sDifference(const BigRegion& iSource1, const BigRegion& iSource2,
		BigRegion& oDestination);

	static void sExclusiveOr(const BigRegion& iSource1, const BigRegion& iSource2,
		BigRegion& oDestination);

protected:
	static void spSpaceCheck(BigRegion& ioRegion, RectPOD*& oRect);
	static void spReallocate(BigRegion& ioRegion, RectPOD*& oRect);
	static void spCopy(const BigRegion& iSource, BigRegion& oDestination);
	static void spSetExtents(BigRegion& ioRegion);

	static void spUnionNonOverlapping(BigRegion& ioRegion,
		RectPOD* r, RectPOD* rEnd, int32 y1, int32 y2);

	static void spUnionOverlapping(BigRegion& ioRegion,
		RectPOD* r1, RectPOD* r1End,
		RectPOD* r2, RectPOD* r2End,
		int32 y1, int32 y2);

	static void spIntersectionOverlapping(BigRegion& ioRegion,
		RectPOD* r1, RectPOD* r1End,
		RectPOD* r2, RectPOD* r2End,
		int32 y1, int32 y2);

	static void spDifferenceNonOverlapping(BigRegion& ioRegion,
		RectPOD* r, RectPOD* rEnd,
		int32 y1, int32 y2);

	static void spDifferenceOverlapping(BigRegion& ioRegion,
		RectPOD* r1, RectPOD* r1End,
		RectPOD* r2, RectPOD* r2End,
		int32 y1, int32 y2);

	typedef void (*NonOverlappingFuncPtr)(BigRegion& ioRegion,
		RectPOD* r, RectPOD* rEnd, int32 y1, int32 y2);

	typedef void (*OverlappingFuncPtr)(BigRegion& ioRegion,
		RectPOD* r1, RectPOD* r1End,
		RectPOD* r2, RectPOD* r2End,
		int32 y1, int32 y2);

	static void spRegionOp(BigRegion& ioNewRegion,
		const BigRegion& iRegion1, const BigRegion& iRegion2,
		OverlappingFuncPtr iOverlapFunc,
		NonOverlappingFuncPtr iNonOverlapFunc1,
		NonOverlappingFuncPtr iNonOverlapFunc2);

	static int32 spCoalesce(BigRegion& ioRegion, int32 prevStart, int32 curStart);

	RectPOD* fRects;
	size_t fNumRectsAllocated;
	size_t fNumRects;
	RectPOD fExtent;
	};

// =================================================================================================
#pragma mark - BigRegionAccumulator

class BigRegionUnioner_t
	{
public:
	void operator()(BigRegion& ioRgn, const BigRegion& iOther) const
		{ ioRgn |= iOther; }
	};

typedef PairwiseCombiner_T<BigRegion, BigRegionUnioner_t, std::vector<BigRegion> >
	BigRegionAccumulator;

} // namespace Pixels
} // namespace ZooLib

#endif // __BigRegion_h__
