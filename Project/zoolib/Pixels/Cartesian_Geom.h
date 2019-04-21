/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#ifndef __ZooLib_Pixels_Cartesian_Geom_h__
#define __ZooLib_Pixels_Cartesian_Geom_h__ 1
#include "zconfig.h"

#include "zoolib/Cartesian.h"
#include "zoolib/Pixels/Geom.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Cartesian::PointTraits<PointPOD>

namespace Cartesian {

template <>
struct PointTraits<Pixels::PointPOD>
:	public PointTraits_Std_HV<Pixels::Ord,Pixels::PointPOD,Pixels::RectPOD>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{ return Pixels::sPointPOD(iX, iY); }
	};

} // namespace Cartesian

// =================================================================================================
#pragma mark - Cartesian::RectTraits<RectPOD>

namespace Cartesian {

template <>
struct RectTraits<Pixels::RectPOD>
:	public RectTraits_Std_LeftTopRightBottom<Pixels::Ord,Pixels::PointPOD,Pixels::RectPOD>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return Pixels::sRectPOD(iL, iT, iR, iB); }
	};

} // namespace Cartesian

} // namespace ZooLib

#endif // __ZooLib_Pixels_Cartesian_Geom_h__
