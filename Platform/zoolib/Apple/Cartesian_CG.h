// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Cartesian_CG_h__
#define __ZooLib_Apple_Cartesian_CG_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/Cartesian.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

#if ZCONFIG_SPI_Enabled(iPhone)
	#include ZMACINCLUDE2(CoreGraphics,CGGeometry.h)
#else
	#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGGeometry.h)
#endif

#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
	typedef float CGFloat;
#endif

namespace ZooLib {
namespace Cartesian {

// =================================================================================================
#pragma mark - PointTraits<CGPoint>

template <>
struct PointTraits<CGPoint>
:	public PointTraits_Std_XY<CGFloat,CGPoint,CGRect>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{ return CGPointMake(iX, iY); }
	};

// =================================================================================================
#pragma mark - PointTraits<CGSize>

template <>
struct PointTraits<CGSize>
:	public PointTraits_Std_WidthHeight<CGFloat,CGSize,CGRect>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{ return CGSizeMake(iX, iY); }
	};

// =================================================================================================
#pragma mark - RectTraits<CGRect>

template <>
struct RectTraits<CGRect>
:	public RectTraits_Std_OriginSize<CGFloat,CGPoint,CGSize,CGRect>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return CGRectMake(iL, iT, iR - iL, iB - iT); }
	};

} // namespace Cartesian
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)
#endif // __ZooLib_Apple_Cartesian_CG_h__
