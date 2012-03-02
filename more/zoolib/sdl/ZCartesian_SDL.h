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

#ifndef __ZCartesian_SDL_h__
#define __ZCartesian_SDL_h__ 1
#include "zconfig.h"

#include "zoolib/ZCartesian.h"

#include "SDL.h"

namespace ZooLib {
namespace ZCartesian {

// =================================================================================================
// MARK: - SDL_Point

template <>
struct PointTraits<SDL_Point>
	{
	typedef null_t Dummy_t;
	typedef bool Bool_t;

	typedef int Ord_t;
	typedef SDL_Point Point_t;
	typedef SDL_Rect Rect_t;

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
		const SDL_Point result = { iX, iY };
		return result;
		}
	};

// =================================================================================================
// MARK: - SDL_Rect

template <>
struct RectTraits<SDL_Rect>
	{
	typedef null_t Dummy_t;
	typedef bool Bool_t;

	typedef int Ord_t;
	typedef SDL_Point Point_t;
	typedef SDL_Rect Rect_t;

	typedef const Ord_t& LC_t;
	static LC_t sL(const Rect_t& iRect) { return iRect.x; }

	typedef Ord_t& L_t;
	static L_t sL(Rect_t& ioRect) { return ioRect.x; }

	typedef const Ord_t& TC_t;
	static TC_t sT(const Rect_t& iRect) { return iRect.y; }

	typedef Ord_t& T_t;
	static T_t sT(Rect_t& ioRect) { return ioRect.y; }

	typedef const Ord_t RC_t;
	static RC_t sR(const Rect_t& iRect) { return iRect.x + iRect.w; }

	typedef const Ord_t R_t;
	static R_t sR(Rect_t& ioRect) { return ioRect.x + ioRect.w; }

	typedef const Ord_t BC_t;
	static BC_t sB(const Rect_t& iRect) { return iRect.y + iRect.h; }

	typedef const Ord_t B_t;
	static B_t sB(Rect_t& ioRect) { return ioRect.y + ioRect.h; }

	typedef const Ord_t& WC_t;
	static WC_t sW(const Rect_t& iRect) { return iRect.w; }

	typedef Ord_t& W_t;
	static W_t sW(Rect_t& ioRect) { return ioRect.w; }

	typedef const Ord_t& HC_t;
	static HC_t sH(const Rect_t& iRect) { return iRect.h; }

	typedef Ord_t& H_t;
	static H_t sH(Rect_t& ioRect) { return ioRect.h; }

	typedef const Point_t WHC_t;
	static WHC_t sWH(const Rect_t& iRect) { return sPoint<Point_t>(W(iRect), H(iRect)); }

	typedef const Point_t WH_t;
	static WH_t sWH(Rect_t& ioRect) { return sPoint<Point_t>(W(ioRect), H(ioRect)); }

	typedef const Point_t LTC_t;
	static LTC_t sLT(const Rect_t& iRect) { return sPoint<Point_t>(L(iRect), T(iRect)); }

	typedef const Point_t LT_t;
	static LT_t sLT(Rect_t& ioRect) { return sPoint<Point_t>(L(ioRect), T(ioRect)); }

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
		{
		const SDL_Rect result = { iL, iT, iR - iL, iB - iT };
		return result;
		}
	};

} // namespace ZCartesian
} // namespace ZooLib

#endif // __ZCartesian_SDL_h__
