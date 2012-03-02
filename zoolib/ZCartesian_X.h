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

namespace ZooLib {
namespace ZCartesian {

// =================================================================================================
// MARK: - XPoint

template <>
struct Traits<XPoint>
	{
	typedef short Ord_t;
	typedef XPoint Point_t;
	typedef XRectangle Rect_t;

	typedef const Ord_t& XC_t;
	static XC_t sX(const Point_t& iPoint) { return iPoint.x; }

	typedef Ord_t& X_t;
	static X_t sX(Point_t& ioPoint) { return ioPoint.x; }

	typedef const Ord_t& YC_t;
	static YC_t sY(const Point_t& iPoint) { return iPoint.y; }

	typedef Ord_t& Y_t;
	static Y_t sY(Point_t& ioPoint) { return ioPoint.y; }

	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		XPoint result;
		theXPoint.x = iX;
		theXPoint.y = iY;
		return result;
		}
	};

inline bool operator==(const XPoint& iL, const XPoint& iR)
	{ return iL.x == iR.x && iL.y == iR.y; }

inline bool operator!=(const XPoint& iL, const XPoint& iR)
	{ return not (iL == iR); }

// =================================================================================================
// MARK: - XRectangle

template <>
struct Traits<XRectangle>
	{
	typedef short Ord_t;
	typedef XPoint Point_t;
	typedef XRectangle Rect_t;

	typedef const Ord_t& LC_t;
	static LC_t sL(const Rect_t& iRect) { return iRect.x; }

	typedef Ord_t& L_t;
	static L_t sL(Rect_t& ioRect) { return ioRect.x; }

	typedef const Ord_t& TC_t;
	static TC_t sT(const Rect_t& iRect) { return iRect.y; }

	typedef Ord_t& T_t;
	static T_t sT(Rect_t& ioRect) { return ioRect.y; }

	typedef const Ord_t RC_t;
	static RC_t sR(const Rect_t& iRect) { return iRect.x + iRect.width; }

	typedef const Ord_t R_t;
	static R_t sR(Rect_t& ioRect) { return ioRect.x + ioRect.width; }

	typedef const Ord_t BC_t;
	static BC_t sB(const Rect_t& iRect) { return iRect.y + iRect.height; }

	typedef const Ord_t B_t;
	static B_t sB(Rect_t& ioRect) { return ioRect.y + ioRect.height; }

	typedef const Ord_t& WC_t;
	static WC_t sW(const Rect_t& iRect) { return iRect.width; }

	typedef Ord_t& W_t;
	static W_t sW(Rect_t& ioRect) { return ioRect.width; }

	typedef const Ord_t& HC_t;
	static HC_t sH(const Rect_t& iRect) { return iRect.height; }

	typedef Ord_t& H_t;
	static H_t sH(Rect_t& ioRect) { return ioRect.height; }

	typedef const Point_t WHC_t;
	static WHC_t sWH(const Rect_t& iRect) { return sPoint<Point_t>(W(iRect), H(iRect)); }

	typedef const Point_t WH_t;
	static WH_t sWH(Rect_t& ioRect) { return sPoint<Point_t>(W(ioRect), H(ioRect)); }

	typedef const Point_t TLC_t;
	static TLC_t sTL(const Rect_t& iRect) { return sPoint<Point_t>(T(iRect), L(iRect)); }

	typedef const Point_t TL_t;
	static TL_t sTL(Rect_t& ioRect) { return sPoint<Point_t>(T(ioRect), L(ioRect)); }

	typedef const Point_t RBC_t;
	static RBC_t sRB(const Rect_t& iRect) { return sPoint<Point_t>(R(iRect), B(iRect)); }

	typedef const Point_t RB_t;
	static RB_t sRB(Rect_t& ioRect) { return sPoint<Point_t>(R(ioRect), B(ioRect)); }

	typedef const Point_t LBC_t;
	static LBC_t sLB(const Rect_t& iRect) { return sPoint<Point_t>(L(iRect), B(iRect)); }

	typedef const Point_t LB_t;
	static LB_t sLB(Rect_t& ioRect) { return sPoint<Point_t>(L(ioRect), B(ioRect)); }

	typedef const Point_t RTC_t;
	static RTC_t sRT(const Rect_t& iRect) { return sPoint<Point_t>(R(iRect), T(iRect)); }

	typedef const Point_t RT_t;
	static RT_t sRT(Rect_t& ioRect) { return sPoint<Point_t>(R(ioRect), T(ioRect)); }

	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return XRectangleMake(iL, iT, iR - iL, iB - iT); }
	};

inline bool operator==(const XRectangle& iL, const XRectangle& iR)
	{
	return iL.x == iR.x && iL.y == iR.y
		&& iL.width == iR.width && iL.height == iR.height;
	}

inline bool operator!=(const XRectangle& iL, const XRectangle& iR)
	{ return not (iL == iR); }

} // namespace ZCartesian
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(X11)
#endif // __ZCartesian_X_h__
