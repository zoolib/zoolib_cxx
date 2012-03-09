/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZGeomPOD_h__
#define __ZGeomPOD_h__
#include "zconfig.h"

#include "zoolib/ZCartesian.h"
#include "zoolib/ZCompare_T.h"
#include "zoolib/ZStdInt.h" // For int32

namespace ZooLib {

// =================================================================================================
// MARK: - ZCoord

typedef int32 ZCoord;

// =================================================================================================
// MARK: - ZPointPOD

struct ZPointPOD
	{
	ZCoord h;
	ZCoord v;
	};

inline ZPointPOD sPointPOD(ZCoord iH, ZCoord iV)
	{
	const ZPointPOD result = {iH, iV};
	return result;
	}

// =================================================================================================
// MARK: - ZRectPOD

struct ZRectPOD
	{
	ZCoord left;
	ZCoord top;
	ZCoord right;
	ZCoord bottom;

	bool Contains(ZCoord h, ZCoord v) const
		{ return h >= left && h < right && v >= top && v < bottom; }

	bool Contains(ZPointPOD p) const
		{ return p.h >= left && p.h < right && p.v >= top && p.v < bottom; }

	bool IsEmpty() const
		{ return left >= right || top >= bottom; }

	ZCoord Width() const { return right - left; }
	ZCoord Height() const { return bottom - top; }
	ZPointPOD Size() const { return sPointPOD(right - left, bottom - top); }

	ZPointPOD TopLeft() const { return sPointPOD(left, top); }
	ZPointPOD TopRight() const { return sPointPOD(right, top); }
	ZPointPOD BottomLeft() const { return sPointPOD(left, bottom); }
	ZPointPOD BottomRight() const { return sPointPOD(right, bottom); }
	};

inline ZRectPOD sRectPOD(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom)
	{
	const ZRectPOD result = {iLeft, iTop, iRight, iBottom};
	return result;
	}

// =================================================================================================
// MARK: - Pseudo-ctors

inline ZRectPOD sRectPOD(ZPointPOD iSize)
	{ return sRectPOD(0, 0, iSize.h, iSize.v); }

inline ZRectPOD sRectPOD(ZCoord iWidth, ZCoord iHeight)
	{ return sRectPOD(0, 0, iWidth, iHeight); }

// =================================================================================================
// MARK: - sCompare_T

template <>
int sCompare_T(const ZPointPOD& iL, const ZPointPOD& iR);

template <>
int sCompare_T(const ZRectPOD& iL, const ZRectPOD& iR);

// =================================================================================================
// MARK: - PointTraits<ZPointPOD>

namespace ZCartesian {

template <>
struct PointTraits<ZPointPOD>
:	public PointTraitsStd_HV<ZCoord,ZPointPOD,ZRectPOD>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{ return sPointPOD(iX, iY); }
	};

} // namespace ZCartesian

// =================================================================================================
// MARK: - RectTraits<ZRectPOD>

namespace ZCartesian {

template <>
struct RectTraits<ZRectPOD>
:	public RectTraitsStd_LeftTopRightBottom<ZCoord,ZPointPOD,ZRectPOD>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return sRectPOD(iL, iT, iR, iB); }
	};

} // namespace ZCartesian

} // namespace ZooLib

#endif // ZGeomPOD
