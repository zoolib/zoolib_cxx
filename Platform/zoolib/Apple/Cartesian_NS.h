// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Cartesian_NS_h__
#define __ZooLib_Apple_Cartesian_NS_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/Cartesian.h"

#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX)

#if defined(__OBJC__)
	#import <Foundation/NSGeometry.h>
	#if defined(NSGEOMETRY_TYPES_SAME_AS_CGGEOMETRY_TYPES) \
		&& NSGEOMETRY_TYPES_SAME_AS_CGGEOMETRY_TYPES
		#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME 1
	#endif
#else
	#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 \
		&& (__LP64__ || NS_BUILD_32_LIKE_64)
		#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME 1
	#else
		typedef struct _NSPoint { float x; float y; } NSPoint;
		typedef struct _NSSize { float width; float height; } NSSize;
		typedef struct _NSRect { NSPoint origin; NSSize size; } NSRect;
	#endif
#endif

#ifndef ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
	#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME 0
#endif

#if ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
	#include "zoolib/Apple/Cartesian_CG.h"
#endif

#if not ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME

namespace ZooLib {
namespace Cartesian {

// =================================================================================================
#pragma mark - PointTraits<NSPoint>

template <>
struct PointTraits<NSPoint>
:	public PointTraits_Std_XY<float,NSPoint,NSRect>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		const Point_t result = { iX, iY };
		return result;
		}
	};

// =================================================================================================
#pragma mark - PointTraits<NSSize>

template <>
struct PointTraits<NSSize>
:	public PointTraits_Std_WidthHeight<float,NSSize,NSRect>
	{
	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		const Point_t result = { iX, iY };
		return result;
		}
	};

// =================================================================================================
#pragma mark - RectTraits<NSRect>

template <>
struct RectTraits<NSRect>
:	public RectTraits_Std_OriginSize<float,NSPoint,NSSize,NSRect>
	{
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{
		const Rect_t result = { {iL, iT}, {iR - iL, iB - iT} };
		return result;
		}
	};

} // namespace Cartesian
} // namespace ZooLib

#endif // not ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
#endif // ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX)
#endif // __ZooLib_Apple_Cartesian_NS_h__
