/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZCartesian_NS_h__
#define __ZCartesian_NS_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZCartesian.h"

#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX)

#if defined(__OBJC__)
	#import <Foundation/NSGeometry.h>
	#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME \
		(defined(NSGEOMETRY_TYPES_SAME_AS_CGGEOMETRY_TYPES) \
		&& NSGEOMETRY_TYPES_SAME_AS_CGGEOMETRY_TYPES)
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

#if not ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME

namespace ZooLib {
namespace ZCartesian {

template <>
struct PointTraits<NSPoint>
	{
	typedef float Ord_t;
	typedef NSPoint Point_t;
	typedef NSRect Rect_t;

	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{ return NSPointMake(iX, iY); }
	};

// =================================================================================================
// MARK: - Point accessors

const float& X(const NSPoint& iNSPoint) { return iNSPoint.x; }
float& X(NSPoint& ioNSPoint) { return ioNSPoint.x; }

const float& Y(const NSPoint& iNSPoint) { return iNSPoint.y; }
float& Y(NSPoint& ioNSPoint) { return ioNSPoint.y; }

inline const float L(const NSPoint& iNSPoint) { return 0; }
inline const float T(const NSPoint& iNSPoint) { return 0; }

inline const float& R(const NSPoint& iNSPoint) { return iNSPoint.x; }
inline float& R(NSPoint& ioNSPoint) { return ioNSPoint.x; }

inline const float& B(const NSPoint& iNSPoint) { return iNSPoint.y; }
inline float B(NSPoint& ioNSPoint) { return ioNSPoint.y; }

inline const float& W(const NSPoint& iNSPoint) { return iNSPoint.x; }
inline float& W(NSPoint& ioNSPoint) { return ioNSPoint.x; }

inline const float& H(const NSPoint& iNSPoint) { return iNSPoint.y; }
inline float& H(NSPoint& ioNSPoint) { return ioNSPoint.y; }

inline const NSPoint& Ex(const NSPoint& iNSPoint) { return iNSPoint; }
inline NSPoint& Ex(NSPoint& ioNSPoint) { return ioNSPoint; }

// =================================================================================================
// MARK: - Extent accessors

const float& X(const NSExtent& iNSExtent) { return iNSExtent.width; }
float& X(NSExtent& ioNSExtent) { return ioNSExtent.width; }

const float& Y(const NSExtent& iNSExtent) { return iNSExtent.height; }
float& Y(NSExtent& ioNSExtent) { return ioNSExtent.height; }

inline const float L(const NSExtent& iNSExtent) { return 0; }
inline const float T(const NSExtent& iNSExtent) { return 0; }

inline const float& R(const NSExtent& iNSExtent) { return iNSExtent.width; }
inline float& R(NSExtent& ioNSExtent) { return ioNSExtent.width; }

inline const float& B(const NSExtent& iNSExtent) { return iNSExtent.height; }
inline float B(NSExtent& ioNSExtent) { return ioNSExtent.height; }

inline const float& W(const NSExtent& iNSExtent) { return iNSExtent.width; }
inline float& W(NSExtent& ioNSExtent) { return ioNSExtent.width; }

inline const float& H(const NSExtent& iNSExtent) { return iNSExtent.height; }
inline float& H(NSExtent& ioNSExtent) { return ioNSExtent.height; }

inline const NSExtent& Ex(const NSExtent& iNSExtent) { return iNSExtent; }
inline NSExtent& Ex(NSExtent& ioNSExtent) { return ioNSExtent; }

// =================================================================================================
// MARK: - Rect accessors

template <>
struct RectTraits<NSRect>
	{
	typedef float Ord_t;
	typedef NSPoint Point_t;
	typedef NSRect Rect_t;

	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return NSRectMake(iL, iT, iR - iL, iB - iT); }
	};

inline const float& L(const NSRect& iNSRect) { return iNSRect.origin.x; }
inline float& L(NSRect& ioNSRect) { return ioNSRect.origin.x; }

inline const float& T(const NSRect& iNSRect) { return iNSRect.origin.y; }
inline float& T(NSRect& ioNSRect) { return ioNSRect.origin.y; }

inline const float R(const NSRect& iNSRect) { return iNSRect.origin.x + iNSRect.extent.width; }

inline const float B(const NSRect& iNSRect) { return iNSRect.origin.y + iNSRect.extent.height; }

inline const float& W(const NSRect& iNSRect) { return iNSRect.extent.width; }
inline float& W(NSRect& ioNSRect) { return ioNSRect.extent.width; }

inline const float& H(const NSRect& iNSRect) { return iNSRect.extent.height; }
inline float& H(NSRect& ioNSRect) { return ioNSRect.extent.height; }

inline const NSExtent& Ex(const NSRect& iNSRect) { return iNSRect.extent; }
inline NSExtent& Ex(NSRect& ioNSRect) { return ioNSRect.extent; }

inline const NSPoint& LT(const NSRect& iNSRect) { return iNSRect.origin; }
inline NSPoint& LT(NSRect& ioNSRect) { return ioNSRect.origin; }

inline const NSPoint RB(const NSRect& iNSRect) { return sPoint<NSPoint>(R(iRect), B(iRect)); }
inline const NSPoint LB(const NSRect& iNSRect) { return sPoint<NSPoint>(L(iRect), B(iRect)); }
inline const NSPoint RT(const NSRect& iNSRect) { return sPoint<NSPoint>(R(iRect), T(iRect)); }

} // namespace ZCartesian
} // namespace ZooLib

#endif // not ZMACRO_NS_AND_CG_CartesianETRY_ARE_SAME
#endif // ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX)
#endif // __ZCartesian_NS_h__
