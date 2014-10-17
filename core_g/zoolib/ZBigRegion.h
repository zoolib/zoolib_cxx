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

#ifndef __ZBigRegion_h__
#define __ZBigRegion_h__ 1
#include "zconfig.h"

#include "zoolib/Accumulator_T.h"

#include "zoolib/ZGeomPOD.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - ZBigRegion

// AG 98-08-13. ZBigRegion provides a region-type facility that works with 32 bit coordinates.
// The implementation is ripped from the XFree86 source.

class ZBigRegion
	{
public:
	static ZBigRegion sRects(const ZRectPOD* iRects, size_t iCount, bool iAlreadySorted);

	ZBigRegion();
	ZBigRegion(const ZBigRegion& iOther);
	ZBigRegion(const ZRectPOD& iBounds);
	ZBigRegion(const ZPointPOD& iSize);
	~ZBigRegion();

	ZBigRegion& operator=(const ZBigRegion& iOther);
	ZBigRegion& operator=(const ZRectPOD& iBounds);

	ZBigRegion Inset(const ZPointPOD& iInset) const
		{ return this->Inset(iInset.h, iInset.v); }
	ZBigRegion Inset(int32 iInsetH, int32 iInsetV) const
		{
		ZBigRegion tempRgn(*this);
		tempRgn.MakeInset(iInsetH, iInsetV);
		return tempRgn;
		}

	void MakeInset(const ZPointPOD& iInset)
		{ this->MakeInset(iInset.h, iInset.v); }
	void MakeInset(int32 iInsetH, int32 iInsetV);

	bool Contains(const ZPointPOD& iPoint) const;
	bool Contains(int32 iH, int32 iV) const;

	void MakeEmpty();
	bool IsEmpty() const;

	ZRectPOD Bounds() const;
	bool IsSimpleRect() const;

	void Decompose(std::vector<ZRectPOD>& oRects) const;
	typedef bool (*DecomposeProc)(const ZRectPOD& iRect, void* iRefcon);
	int32 Decompose(DecomposeProc iProc, void* iRefcon) const;

	bool operator==(const ZBigRegion& iOther) const;
	bool operator!=(const ZBigRegion& iOther) const
		{ return ! (*this == iOther); }

	ZBigRegion& operator+=(const ZPointPOD& iOffset);
	ZBigRegion operator+(const ZPointPOD& iOffset) const
		{
		ZBigRegion newRegion(*this);
		return newRegion += iOffset;
		}

	ZBigRegion& operator-=(const ZPointPOD& iOffset)
		{ return *this += sPointPOD(-iOffset.h, -iOffset.v); }
	ZBigRegion operator-(const ZPointPOD& iOffset) const
		{
		ZBigRegion newRegion(*this);
		return newRegion -= iOffset;
		}

	ZBigRegion& operator|=(const ZBigRegion& iOther);
	ZBigRegion operator|(const ZBigRegion& iOther) const
		{
		ZBigRegion newRegion(*this);
		return newRegion |= iOther;
		}

	ZBigRegion& operator&=(const ZBigRegion& iOther);
	ZBigRegion operator&(const ZBigRegion& iOther) const
		{
		ZBigRegion newRegion(*this);
		return newRegion &= iOther;
		}

	ZBigRegion& operator-=(const ZBigRegion& iOther);
	ZBigRegion operator-(const ZBigRegion& iOther) const
		{
		ZBigRegion newRegion(*this);
		return newRegion -= iOther;
		}

	ZBigRegion& operator^=(const ZBigRegion& iOther);
	ZBigRegion operator^(const ZBigRegion& iOther) const
		{
		ZBigRegion newRegion(*this);
		return newRegion ^= iOther;
		}

	static void sUnion(const ZBigRegion& iSource1, const ZBigRegion& iSource2,
		ZBigRegion& oDestination);

	static void sIntersection(const ZBigRegion& iSource1, const ZBigRegion& iSource2,
		ZBigRegion& oDestination);

	static void sDifference(const ZBigRegion& iSource1, const ZBigRegion& iSource2,
		ZBigRegion& oDestination);

	static void sExclusiveOr(const ZBigRegion& iSource1, const ZBigRegion& iSource2,
		ZBigRegion& oDestination);

protected:
	static void spSpaceCheck(ZBigRegion& ioRegion, ZRectPOD*& oRect);
	static void spReallocate(ZBigRegion& ioRegion, ZRectPOD*& oRect);
	static void spCopy(const ZBigRegion& iSource, ZBigRegion& oDestination);
	static void spSetExtents(ZBigRegion& ioRegion);

	static void spUnionNonOverlapping(ZBigRegion& ioRegion,
		ZRectPOD* r, ZRectPOD* rEnd, int32 y1, int32 y2);

	static void spUnionOverlapping(ZBigRegion& ioRegion,
		ZRectPOD* r1, ZRectPOD* r1End,
		ZRectPOD* r2, ZRectPOD* r2End,
		int32 y1, int32 y2);

	static void spIntersectionOverlapping(ZBigRegion& ioRegion,
		ZRectPOD* r1, ZRectPOD* r1End,
		ZRectPOD* r2, ZRectPOD* r2End,
		int32 y1, int32 y2);

	static void spDifferenceNonOverlapping(ZBigRegion& ioRegion,
		ZRectPOD* r, ZRectPOD* rEnd,
		int32 y1, int32 y2);

	static void spDifferenceOverlapping(ZBigRegion& ioRegion,
		ZRectPOD* r1, ZRectPOD* r1End,
		ZRectPOD* r2, ZRectPOD* r2End,
		int32 y1, int32 y2);

	typedef void (*NonOverlappingFuncPtr)(ZBigRegion& ioRegion,
		ZRectPOD* r, ZRectPOD* rEnd, int32 y1, int32 y2);

	typedef void (*OverlappingFuncPtr)(ZBigRegion& ioRegion,
		ZRectPOD* r1, ZRectPOD* r1End,
		ZRectPOD* r2, ZRectPOD* r2End,
		int32 y1, int32 y2);

	static void spRegionOp(ZBigRegion& ioNewRegion,
		const ZBigRegion& iRegion1, const ZBigRegion& iRegion2,
		OverlappingFuncPtr iOverlapFunc,
		NonOverlappingFuncPtr iNonOverlapFunc1,
		NonOverlappingFuncPtr iNonOverlapFunc2);

	static int32 spCoalesce(ZBigRegion& ioRegion, int32 prevStart, int32 curStart);

	ZRectPOD* fRects;
	size_t fNumRectsAllocated;
	size_t fNumRects;
	ZRectPOD fExtent;
	};

// =================================================================================================
// MARK: - ZBigRegionAccumulator

class ZBigRegionUnioner_t
	{
public:
	void operator()(ZBigRegion& ioRgn, const ZBigRegion& iOther) const
		{ ioRgn |= iOther; }
	};

typedef Accumulator_T<ZBigRegion, ZBigRegionUnioner_t, std::vector<ZBigRegion> >
	ZBigRegionAccumulator;

} // namespace ZooLib

#endif // __ZBigRegion_h__
