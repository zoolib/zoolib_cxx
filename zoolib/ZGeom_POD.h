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

#ifndef __ZGeomPOD__
#define __ZGeomPOD__
#include "zconfig.h"

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZStdInt.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCoord

typedef int32 ZCoord;

// =================================================================================================
#pragma mark -
#pragma mark * ZPointPOD

struct ZPointPOD;

ZPointPOD sPointPOD(int32 iH, int32 iV);

struct ZPointPOD
	{
	int32 h;
	int32 v;

	ZPointPOD operator+(ZPointPOD p) const
		{ return sPointPOD(h + p.h, v + p.v); }
	ZPointPOD& operator+=(ZPointPOD p)
		{ h += p.h; v += p.v; return *this; }

	ZPointPOD operator-(ZPointPOD p) const
		{ return sPointPOD(h - p.h, v - p.v); }
	ZPointPOD& operator-=(ZPointPOD p)
		{ h -= p.h; v -= p.v; return *this; }
	};

template <> int sCompare_T(const ZPointPOD& iL, const ZPointPOD& iR);

// =================================================================================================
#pragma mark -
#pragma mark * ZRectPOD

struct ZRectPOD;

ZRectPOD sRectPOD(int32 iLeft, int32 iTop, int32 iRight, int32 iBottom);

struct ZRectPOD
	{
	int32 left;
	int32 top;
	int32 right;
	int32 bottom;

	bool operator==(const ZRectPOD& other) const
		{
		return top == other.top && left == other.left &&
			bottom == other.bottom && right == other.right;
		}

	bool operator!=(const ZRectPOD& other) const
		{
		return top != other.top || left != other.left ||
			bottom != other.bottom || right != other.right;
		}

	bool Contains(int32 h, int32 v) const
		{ return h >= left && h < right && v >= top && h < bottom; }

	bool Contains(ZPointPOD p) const
		{ return p.h >= left && p.h < right && p.v >= top && p.h < bottom; }

	bool IsEmpty() const
		{ return left >= right || top >= bottom; }

	int32 Width() const { return right - left; }
	int32 Height() const { return bottom - top; }
	ZPointPOD Size() const { return sPointPOD(right - left, bottom - top); }

	ZPointPOD TopLeft() const { return sPointPOD(left, top); }
	ZPointPOD TopRight() const { return sPointPOD(right, top); }
	ZPointPOD BottomLeft() const { return sPointPOD(left, bottom); }
	ZPointPOD BottomRight() const { return sPointPOD(right, bottom); }

	ZRectPOD operator+(ZPointPOD p) const
		{ return sRectPOD(left + p.h, top + p.v, right + p.h, bottom + p.v); }
	ZRectPOD& operator+=(ZPointPOD p)
		{ left += p.h; top += p.v; right += p.h; bottom += p.v; return *this; }

	ZRectPOD operator-(ZPointPOD p) const
		{ return sRectPOD(left - p.h, top - p.v, right - p.h, bottom - p.v); }
	ZRectPOD& operator-=(ZPointPOD p)
		{ left -= p.h; top -= p.v; right -= p.h; bottom -= p.v; return *this; }


	static inline int32 sMin(int32 a, int32 b)
		{ return a < b ? a : b; }

	ZRectPOD operator&(const ZRectPOD& other) const
		{
		ZRectPOD result;
		result.left = sMin(left, other.left);
		result.top = sMin(top, other.top);
		result.right = sMin(right, other.right);
		result.bottom = sMin(bottom, other.bottom);
		result.left = sMin(left, right);
		result.top = sMin(top, bottom);
		return result;
		}

	ZRectPOD& operator&=(const ZRectPOD& other)
		{
		left = sMin(left, other.left);
		top = sMin(top, other.top);
		right = sMin(right, other.right);
		bottom = sMin(bottom, other.bottom);
		left = sMin(left, right);
		top = sMin(top, bottom);
		return *this;
		}
	};

template <> int sCompare_T(const ZRectPOD& iL, const ZRectPOD& iR);

inline ZRectPOD sRectPOD(ZPointPOD iSize)
	{ return sRectPOD(0, 0, iSize.h, iSize.v); }

inline ZRectPOD sRectPOD(int32 iWidth, int32 iHeight)
	{ return sRectPOD(0, 0, iWidth, iHeight); }

} // namespace ZooLib

#endif // ZGeomPOD
