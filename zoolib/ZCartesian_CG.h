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

#ifndef __ZGeom_CG_h__
#define __ZGeom_CG_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCartesian.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

#if ZCONFIG_SPI_Enabled(iPhone)
	#include ZMACINCLUDE2(CoreGraphics,CGGeometry.h)
#else
	#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGGeometry.h)
#endif

namespace ZooLib {
namespace ZCartesian {

template <>
struct PointTraits<CGPoint>
	{
	typedef CGFloat Ord_t;
	typedef CGPoint Point_t;
	typedef CGRect Rect_t;

	static CGPoint sMake(const CGFloat& iX, const CGFloat& iY)
		{ return CGPointMake(iX, iY); }
	};

// =================================================================================================
// MARK: - Point accessors

const CGFloat& X(const CGPoint& iCGPoint) { return iCGPoint.x; }
CGFloat& X(CGPoint& ioCGPoint) { return ioCGPoint.x; }

const CGFloat& Y(const CGPoint& iCGPoint) { return iCGPoint.y; }
CGFloat& Y(CGPoint& ioCGPoint) { return ioCGPoint.y; }

inline const CGFloat L(const CGPoint& iCGPoint) { return 0; }
inline const CGFloat T(const CGPoint& iCGPoint) { return 0; }

inline const CGFloat& R(const CGPoint& iCGPoint) { return iCGPoint.x; }
inline CGFloat& R(CGPoint& ioCGPoint) { return ioCGPoint.x; }

inline const CGFloat& B(const CGPoint& iCGPoint) { return iCGPoint.y; }
inline CGFloat B(CGPoint& ioCGPoint) { return ioCGPoint.y; }

inline const CGFloat& W(const CGPoint& iCGPoint) { return iCGPoint.x; }
inline CGFloat& W(CGPoint& ioCGPoint) { return ioCGPoint.x; }

inline const CGFloat& H(const CGPoint& iCGPoint) { return iCGPoint.y; }
inline CGFloat& H(CGPoint& ioCGPoint) { return ioCGPoint.y; }

inline const CGPoint& WH(const CGPoint& iCGPoint) { return iCGPoint; }
inline CGPoint& WH(CGPoint& ioCGPoint) { return ioCGPoint; }

// =================================================================================================
// MARK: - Extent accessors

const CGFloat& X(const CGExtent& iCGExtent) { return iCGExtent.width; }
CGFloat& X(CGExtent& ioCGExtent) { return ioCGExtent.width; }

const CGFloat& Y(const CGExtent& iCGExtent) { return iCGExtent.height; }
CGFloat& Y(CGExtent& ioCGExtent) { return ioCGExtent.height; }

inline const CGFloat L(const CGExtent& iCGExtent) { return 0; }
inline const CGFloat T(const CGExtent& iCGExtent) { return 0; }

inline const CGFloat& R(const CGExtent& iCGExtent) { return iCGExtent.width; }
inline CGFloat& R(CGExtent& ioCGExtent) { return ioCGExtent.width; }

inline const CGFloat& B(const CGExtent& iCGExtent) { return iCGExtent.height; }
inline CGFloat B(CGExtent& ioCGExtent) { return ioCGExtent.height; }

inline const CGFloat& W(const CGExtent& iCGExtent) { return iCGExtent.width; }
inline CGFloat& W(CGExtent& ioCGExtent) { return ioCGExtent.width; }

inline const CGFloat& H(const CGExtent& iCGExtent) { return iCGExtent.height; }
inline CGFloat& H(CGExtent& ioCGExtent) { return ioCGExtent.height; }

inline const CGExtent& WH(const CGExtent& iCGExtent) { return iCGExtent; }
inline CGExtent& WH(CGExtent& ioCGExtent) { return ioCGExtent; }

// =================================================================================================
// MARK: - Rect accessors

template <>
struct RectTraits<CGRect>
	{
	typedef CGFloat Ord_t;
	typedef CGPoint Point_t;
	typedef CGRect Rect_t;

	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return CGRectMake(iL, iT, iR - iL, iB - iT); }
	};

inline const CGFloat& L(const CGRect& iCGRect) { return iCGRect.origin.x; }
inline CGFloat& L(CGRect& ioCGRect) { return ioCGRect.origin.x; }

inline const CGFloat& T(const CGRect& iCGRect) { return iCGRect.origin.y; }
inline CGFloat& T(CGRect& ioCGRect) { return ioCGRect.origin.y; }

inline const CGFloat R(const CGRect& iCGRect) { return iCGRect.origin.x + iCGRect.extent.width; }

inline const CGFloat B(const CGRect& iCGRect) { return iCGRect.origin.y + iCGRect.extent.height; }

inline const CGFloat& W(const CGRect& iCGRect) { return iCGRect.extent.width; }
inline CGFloat& W(CGRect& ioCGRect) { return ioCGRect.extent.width; }

inline const CGFloat& H(const CGRect& iCGRect) { return iCGRect.extent.height; }
inline CGFloat& H(CGRect& ioCGRect) { return ioCGRect.extent.height; }

inline const CGExtent& WH(const CGRect& iCGRect) { return iCGRect.extent; }
inline CGExtent& WH(CGRect& ioCGRect) { return ioCGRect.extent; }

inline const CGPoint& LT(const CGRect& iCGRect) { return iCGRect.origin; }
inline CGPoint& LT(CGRect& ioCGRect) { return ioCGRect.origin; }

inline const CGPoint RB(const CGRect& iCGRect) { return sPoint<CGPoint>(R(iRect), B(iRect)); }
inline const CGPoint LB(const CGRect& iCGRect) { return sPoint<CGPoint>(L(iRect), B(iRect)); }
inline const CGPoint RT(const CGRect& iCGRect) { return sPoint<CGPoint>(R(iRect), T(iRect)); }

} // namespace ZCartesian
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)
#endif // __ZCartesian_CG_h__
