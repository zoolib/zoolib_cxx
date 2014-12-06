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

#include "zoolib/Cartesian.h"

#include "zoolib/ZStdInt.h" // For int32

namespace ZooLib {

// =================================================================================================
// MARK: - ZCoord (deprecated)

typedef int32 ZCoord;

// =================================================================================================
// MARK: - ZPointPOD

struct ZPointPOD
	{
	int32 h;
	int32 v;
	};

inline ZPointPOD sPointPOD(int32 iH, int32 iV)
	{
	const ZPointPOD result = {iH, iV};
	return result;
	}

inline ZPointPOD sPointPOD(int32 iHV)
	{ return sPointPOD(iHV, iHV); }

// =================================================================================================
// MARK: - ZRectPOD

struct ZRectPOD
	{
	int32 left;
	int32 top;
	int32 right;
	int32 bottom;
	};

inline ZRectPOD sRectPOD(int32 iLeft, int32 iTop, int32 iRight, int32 iBottom)
	{
	const ZRectPOD result = {iLeft, iTop, iRight, iBottom};
	return result;
	}

inline ZRectPOD sRectPOD(int32 iWidth, int32 iHeight)
	{ return sRectPOD(0, 0, iWidth, iHeight); }

// =================================================================================================
// MARK: - Cartesian::PointTraits<ZPointPOD>

namespace Cartesian {

template <>
struct PointTraits<ZPointPOD>
:	public PointTraits_Std_HV<int32,ZPointPOD,ZRectPOD>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{ return sPointPOD(iX, iY); }
	};

} // namespace Cartesian

// =================================================================================================
// MARK: - Cartesian::RectTraits<ZRectPOD>

namespace Cartesian {

template <>
struct RectTraits<ZRectPOD>
:	public RectTraits_Std_LeftTopRightBottom<int32,ZPointPOD,ZRectPOD>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return sRectPOD(iL, iT, iR, iB); }
	};

} // namespace Cartesian

} // namespace ZooLib

#endif // ZGeomPOD
