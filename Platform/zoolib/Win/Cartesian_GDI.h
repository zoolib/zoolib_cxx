// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Cartesian_GDI_h__
#define __ZooLib_Cartesian_GDI_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/Cartesian.h"

#if ZCONFIG_SPI_Enabled(GDI)

#include "zoolib/ZCompat_Win.h"

namespace ZooLib {
namespace Cartesian {

// =================================================================================================
#pragma mark - PointTraits<POINT>

template <>
struct PointTraits<POINT>
:	public PointTraits_Std_XY<LONG,POINT,RECT>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		const Point_t result = { iX, iY };
		return result;
		}
	};

// =================================================================================================
#pragma mark - PointTraits<SIZE>

template <>
struct PointTraits<SIZE>
:	public PointTraits_Std<LONG,SIZE,RECT>
	{
	typedef const Ord_t& XC_t;
	static XC_t sX(const Point_t& iPoint) { return iPoint.cx; }

	typedef Ord_t& X_t;
	static X_t sX(Point_t& ioPoint) { return ioPoint.cx; }

	typedef const Ord_t& YC_t;
	static YC_t sY(const Point_t& iPoint) { return iPoint.cy; }

	typedef Ord_t& Y_t;
	static Y_t sY(Point_t& ioPoint) { return ioPoint.cy; }

	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		const Point_t result = { iX, iY };
		return result;
		}
	};

// =================================================================================================
#pragma mark - RectTraits<RECT>

template <>
struct RectTraits<RECT>
:	public RectTraits_Std_LeftTopRightBottom<LONG,POINT,RECT>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{
		const Rect_t result = { iL, iT, iR, iB };
		return result;
		}
	};

} // namespace Cartesian
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(GDI)
#endif // __ZooLib_Cartesian_GDI_h__
