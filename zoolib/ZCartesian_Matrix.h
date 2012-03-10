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

#ifndef __ZCartesian_Matrix_h__
#define __ZCartesian_Matrix_h__ 1
#include "zconfig.h"

#include "zoolib/ZCartesian.h"
#include "zoolib/ZMatrixArray.h"

namespace ZooLib {
namespace ZCartesian {

// =================================================================================================
// MARK: - PointTraits<ZMatrix<Ord_p,1,2> > (point)

template <>
template <class Ord_p>
struct PointTraits<ZMatrix<Ord_p,1,2> >
:	public PointTraits_Std_Base<Ord_p,ZMatrix<Ord_p,1,2>,ZMatrixArray<Ord_p,1,2,2> >
	{
	typedef ZMatrix<Ord_p,1,2> Point_t;

	typedef const Ord_p& XC_t;
	static XC_t sX(const Point_t& iPoint) { return iPoint[0]; }

	typedef Ord_p& X_t;
	static X_t sX(Point_t& ioPoint) { return ioPoint[0]; }

	typedef const Ord_p& YC_t;
	static YC_t sY(const Point_t& iPoint) { return iPoint[1]; }

	typedef Ord_p& Y_t;
	static Y_t sY(Point_t& ioPoint) { return ioPoint[1]; }

	static Point_t sMake(const Ord_p& iX, const Ord_p& iY)
		{
		Point_t result(null);
		result[0] = iX;
		result[1] = iY;
		return result;
		}
	};

// =================================================================================================
// MARK: - PointTraits<ZMatrix<Ord_p,1,3> > (CVec3 used as a 2D point)

template <>
template <class Ord_p>
struct PointTraits<ZMatrix<Ord_p,1,3> >
:	public PointTraits_Std_Base<Ord_p,ZMatrix<Ord_p,1,3>,ZMatrixArray<Ord_p,1,2,2> >
	{
	typedef ZMatrix<Ord_p,1,3> Point_t;

	typedef const Ord_p& XC_t;
	static XC_t sX(const Point_t& iPoint) { return iPoint[0]; }

	typedef Ord_p& X_t;
	static X_t sX(Point_t& ioPoint) { return ioPoint[0]; }

	typedef const Ord_p& YC_t;
	static YC_t sY(const Point_t& iPoint) { return iPoint[1]; }

	typedef Ord_p& Y_t;
	static Y_t sY(Point_t& ioPoint) { return ioPoint[1]; }
	};

// =================================================================================================
// MARK: - RectTraits<ZMatrixArray<Ord,1,2,2> > (rectangle)

template <>
template <class Ord_p>
struct RectTraits<ZMatrixArray<Ord_p,1,2,2> >
:	public RectTraits_Std_Base<Ord_p,ZMatrix<Ord_p,1,3>,ZMatrixArray<Ord_p,1,2,2> >
	{
	typedef ZMatrix<Ord_p,1,2> Point_t;
	typedef ZMatrixArray<Ord_p,1,2,2> Rect_t;

	typedef const Ord_p& LC_t;
	static LC_t sL(const Rect_t& iRect) { return iRect[0][0]; }

	typedef Ord_p& L_t;
	static L_t sL(Rect_t& ioRect) { return ioRect[0][0]; }

	typedef const Ord_p& TC_t;
	static TC_t sT(const Rect_t& iRect) { return iRect[0][1]; }

	typedef Ord_p& T_t;
	static T_t sT(Rect_t& ioRect) { return ioRect[0][1]; }

	typedef const Ord_p& RC_t;
	static RC_t sR(const Rect_t& iRect) { return iRect[1][0]; }

	typedef Ord_p& R_t;
	static R_t sR(Rect_t& ioRect) { return ioRect[1][0]; }

	typedef const Ord_p& BC_t;
	static BC_t sB(const Rect_t& iRect) { return iRect[1][1]; }

	typedef Ord_p& B_t;
	static B_t sB(Rect_t& ioRect) { return ioRect[1][1]; }

	typedef const Point_t& LTC_t;
	static LTC_t sLT(const Rect_t& iRect) { return iRect[0]; }

	typedef Point_t& LT_t;
	static LT_t sLT(Rect_t& ioRect) { return ioRect[0]; }

	typedef const Point_t& RBC_t;
	static RBC_t sRB(const Rect_t& iRect) { return iRect[1]; }

	typedef Point_t& RB_t;
	static RB_t sRB(Rect_t& ioRect) { return ioRect[1]; }

	static Rect_t sMake(const Ord_p& iL, const Ord_p& iT, const Ord_p& iR, const Ord_p& iB)
		{
		Rect_t result(null);
		result[0][0] = iL;
		result[0][1] = iT;
		result[1][0] = iR;
		result[1][1] = iB;
		return result;
		}
	};

} // namespace ZCartesian
} // namespace ZooLib

#endif // __ZCartesian_Matrix_h__
