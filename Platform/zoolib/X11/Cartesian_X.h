// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_X11_Cartesian_X_h__
#define __ZooLib_X11_Cartesian_X_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/Cartesian.h"

#if ZCONFIG_SPI_Enabled(X11)

#include "zoolib/X11/Compat_Xlib.h"

namespace ZooLib {
namespace Cartesian {

// =================================================================================================
#pragma mark - PointTraits<XPoint>

template <>
struct PointTraits<XPoint>
:	public PointTraits_Std_XY<short,XPoint,XRectangle>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		XPoint result;
		theXPoint.x = iX;
		theXPoint.y = iY;
		return result;
		}
	};

// =================================================================================================
#pragma mark - RectTraits<XRectangle>

template <>
struct RectTraits<XRectangle>
:	public RectTraits_Std_XYWH<short,XPoint,XRectangle>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return XRectangleMake(iL, iT, iR - iL, iB - iT); }
	};

} // namespace Cartesian
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(X11)
#endif // __ZooLib_X11_Cartesian_X_h__
