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

#ifndef __ZCartesian_X_h__
#define __ZCartesian_X_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZCartesian.h"

#if ZCONFIG_SPI_Enabled(X11)

#include "zoolib/ZCompat_Xlib.h"
//#include <X11/Xutil.h>

namespace ZooLib {
namespace ZCartesian {

template <>
struct PointTraits<XPoint>
	{
	typedef short Ord_t;
	typedef XPoint Point_t;
	typedef XRectangle Rect_t;

	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		XPoint result;
		theXPoint.x = iX;
		theXPoint.y = iY;
		return result;
		}
	};

// =================================================================================================
// MARK: - Point accessors

const float& X(const XPoint& iXPoint) { return iXPoint.x; }
float& X(XPoint& ioXPoint) { return ioXPoint.x; }

const float& Y(const XPoint& iXPoint) { return iXPoint.y; }
float& Y(XPoint& ioXPoint) { return ioXPoint.y; }

inline const float L(const XPoint& iXPoint) { return 0; }
inline const float T(const XPoint& iXPoint) { return 0; }

inline const float& R(const XPoint& iXPoint) { return iXPoint.x; }
inline float& R(XPoint& ioXPoint) { return ioXPoint.x; }

inline const float& B(const XPoint& iXPoint) { return iXPoint.y; }
inline float B(XPoint& ioXPoint) { return ioXPoint.y; }

inline const float& W(const XPoint& iXPoint) { return iXPoint.x; }
inline float& W(XPoint& ioXPoint) { return ioXPoint.x; }

inline const float& H(const XPoint& iXPoint) { return iXPoint.y; }
inline float& H(XPoint& ioXPoint) { return ioXPoint.y; }

inline const XPoint& WH(const XPoint& iXPoint) { return iXPoint; }
inline XPoint& WH(XPoint& ioXPoint) { return ioXPoint; }

// =================================================================================================
// MARK: - Rect accessors

template <>
struct RectTraits<XRectangle>
	{
	typedef short Ord_t;
	typedef XPoint Point_t;
	typedef XRectangle Rect_t;

	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return XRectangleMake(iL, iT, iR - iL, iB - iT); }
	};

inline const float& L(const XRectangle& iXRect) { return iXRect.x; }
inline float& L(XRectangle& ioXRect) { return ioXRect.x; }

inline const float& T(const XRectangle& iXRect) { return iXRect.y; }
inline float& T(XRectangle& ioXRect) { return ioXRect.y; }

inline const float R(const XRectangle& iXRect) { return iXRect.x + iXRect.width; }

inline const float B(const XRectangle& iXRect) { return iXRect.y + iXRect.height; }

inline const float& W(const XRectangle& iXRect) { return iXRect.width; }
inline float& W(XRectangle& ioXRect) { return ioXRect.width; }

inline const float& H(const XRectangle& iXRect) { return iXRect.height; }
inline float& H(XRectangle& ioXRect) { return ioXRect.height; }

inline const XPoint WH(const XRectangle& iXRect) { sPoint<XPoint>(iXRect.width, iXRect.height); }

inline const XPoint LT(const XRectangle& iXRect) { return sPoint<XPoint>(L(iXRect), T(iXRect)); }
inline const XPoint RB(const XRectangle& iXRect) { return sPoint<XPoint>(R(iXRect), B(iXRect)); }
inline const XPoint LB(const XRectangle& iXRect) { return sPoint<XPoint>(L(iXRect), B(iXRect)); }
inline const XPoint RT(const XRectangle& iXRect) { return sPoint<XPoint>(R(iXRect), T(iXRect)); }

} // namespace ZCartesian
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(X11)
#endif // __ZCartesian_X_h__
