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

#ifndef __ZBigRegion__
#define __ZBigRegion__ 1
#include "zconfig.h"

#include "zoolib/ZAccumulator_T.h"
#include "zoolib/ZGeom.h"

#include <vector>

// =================================================================================================
#pragma mark -
#pragma mark * ZBigRegion

// AG 98-08-13. ZBigRegion provides a region-type facility that works with 32 bit coordinates.
// The implementation is ripped from the XFree86 source.

class ZBigRegion
	{
public:
	static ZBigRegion sRects(const ZRect_T<int32>* iRects, size_t iCount, bool iAlreadySorted);

	ZBigRegion();
	ZBigRegion(const ZBigRegion& iOther);
	ZBigRegion(const ZRect_T<int32>& iBounds);
	ZBigRegion(const ZPoint_T<int32>& iSize);
	~ZBigRegion();

	ZBigRegion& operator=(const ZBigRegion& iOther);
	ZBigRegion& operator=(const ZRect_T<int32>& iBounds);

	ZBigRegion Inset(const ZPoint_T<int32>& iInset) const
		{ return this->Inset(iInset.h, iInset.v); }
	ZBigRegion Inset(int32 iInsetH, int32 iInsetV) const
		{
		ZBigRegion tempRgn(*this);
		tempRgn.MakeInset(iInsetH, iInsetV);
		return tempRgn;
		}

	void MakeInset(const ZPoint_T<int32>& iInset)
		{ this->MakeInset(iInset.h, iInset.v); }
	void MakeInset(int32 iInsetH, int32 iInsetV);

	bool Contains(const ZPoint_T<int32>& iPoint) const;
	bool Contains(int32 iH, int32 iV) const;

	void MakeEmpty();
	bool IsEmpty() const;

	ZRect_T<int32> Bounds() const;
	bool IsSimpleRect() const;

	void Decompose(std::vector<ZRect_T<int32> >& oRects) const;
	typedef bool (*DecomposeProc)(const ZRect_T<int32>& iRect, void* iRefcon);
	int32 Decompose(DecomposeProc iProc, void* iRefcon) const;

	bool operator==(const ZBigRegion& iOther) const;
	bool operator!=(const ZBigRegion& iOther) const
		{ return ! (*this == iOther); }

	ZBigRegion& operator+=(const ZPoint_T<int32>& iOffset);
	ZBigRegion operator+(const ZPoint_T<int32>& iOffset) const
		{
		ZBigRegion newRegion(*this);
		return newRegion += iOffset;
		}

	ZBigRegion& operator-=(const ZPoint_T<int32>& iOffset)
		{ return *this += (-iOffset); }
	ZBigRegion operator-(const ZPoint_T<int32>& iOffset) const
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
	static void sInternal_SpaceCheck(ZBigRegion& ioRegion, ZRect_T<int32>*& oRect);
	static void sInternal_Reallocate(ZBigRegion& ioRegion, ZRect_T<int32>*& oRect);
	static void sInternal_Copy(const ZBigRegion& iSource, ZBigRegion& oDestination);
	static void sInternal_SetExtents(ZBigRegion& ioRegion);

	static void sInternal_UnionNonOverlapping(ZBigRegion& ioRegion,
		ZRect_T<int32>* r, ZRect_T<int32>* rEnd, int32 y1, int32 y2);

	static void sInternal_UnionOverlapping(ZBigRegion& ioRegion,
		ZRect_T<int32>* r1, ZRect_T<int32>* r1End,
		ZRect_T<int32>* r2, ZRect_T<int32>* r2End,
		int32 y1, int32 y2);

	static void sInternal_IntersectionOverlapping(ZBigRegion& ioRegion,
		ZRect_T<int32>* r1, ZRect_T<int32>* r1End,
		ZRect_T<int32>* r2, ZRect_T<int32>* r2End,
		int32 y1, int32 y2);

	static void sInternal_DifferenceNonOverlapping(ZBigRegion& ioRegion,
		ZRect_T<int32>* r, ZRect_T<int32>* rEnd,
		int32 y1, int32 y2);

	static void sInternal_DifferenceOverlapping(ZBigRegion& ioRegion,
		ZRect_T<int32>* r1, ZRect_T<int32>* r1End,
		ZRect_T<int32>* r2, ZRect_T<int32>* r2End,
		int32 y1, int32 y2);

	typedef void (*NonOverlappingFuncPtr)(ZBigRegion& ioRegion,
		ZRect_T<int32>* r, ZRect_T<int32>* rEnd, int32 y1, int32 y2);

	typedef void (*OverlappingFuncPtr)(ZBigRegion& ioRegion,
		ZRect_T<int32>* r1, ZRect_T<int32>* r1End,
		ZRect_T<int32>* r2, ZRect_T<int32>* r2End,
		int32 y1, int32 y2);

	static void sInternal_RegionOp(ZBigRegion& ioNewRegion,
		const ZBigRegion& iRegion1, const ZBigRegion& iRegion2,
		OverlappingFuncPtr iOverlapFunc,
		NonOverlappingFuncPtr iNonOverlapFunc1,
		NonOverlappingFuncPtr iNonOverlapFunc2);

	static int32 sInternal_Coalesce(ZBigRegion& ioRegion, int32 prevStart, int32 curStart);

	ZRect_T<int32>* fRects;
	size_t fNumRectsAllocated;
	size_t fNumRects;
	ZRect_T<int32> fExtent;
	};

#endif // __ZBigRegion__
