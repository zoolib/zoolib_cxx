// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

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
