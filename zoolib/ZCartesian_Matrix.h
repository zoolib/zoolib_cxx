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
// MARK: - PointTraits

template <>
template <class Ord>
struct PointTraits<ZMatrix<Ord,1,2> >
	{
	typedef Ord Ord_t;
	typedef ZMatrix<Ord,1,2> Point_t;
	typedef ZMatrixArray<Ord,1,2,2> Rect_t;

	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{
		ZMatrix<Ord,1,2> result;
		X(result) = iX;
		Y(result) = iY;
		return result;
		}
	};

// =================================================================================================
// MARK: - Point accessors

template <class Ord> const Ord& X(const ZMatrix<Ord,1,2>& iPoint) { return iPoint[0]; }
template <class Ord> Ord& X(ZMatrix<Ord,1,2>& ioPoint) { return ioPoint[0]; }

template <class Ord> const Ord& Y(const ZMatrix<Ord,1,2>& iPoint) { return iPoint[1]; }
template <class Ord> Ord& Y(ZMatrix<Ord,1,2>& ioPoint) { return ioPoint[1]; }

template <class Ord> const Ord L(const ZMatrix<Ord,1,2>& iPoint) { return 0; }

template <class Ord> const Ord T(const ZMatrix<Ord,1,2>& iPoint) { return 0; }

template <class Ord> const Ord& R(const ZMatrix<Ord,1,2>& iPoint) { return X(iPoint); }
template <class Ord> Ord& R(ZMatrix<Ord,1,2>& ioPoint) { return X(ioPoint); }

template <class Ord> const Ord& B(const ZMatrix<Ord,1,2>& iPoint) { return Y(iPoint); }
template <class Ord> Ord B(ZMatrix<Ord,1,2>& ioPoint) { return Y(ioPoint); }

template <class Ord> const Ord& W(const ZMatrix<Ord,1,2>& iPoint) { return X(iPoint); }
template <class Ord> Ord& W(ZMatrix<Ord,1,2>& ioPoint) { return X(ioPoint); }

template <class Ord> const Ord& H(const ZMatrix<Ord,1,2>& iPoint) { return Y(iPoint); }
template <class Ord> Ord& H(ZMatrix<Ord,1,2>& ioPoint) { return Y(ioPoint); }

template <class Ord> const ZMatrix<Ord,1,2>& WH(const ZMatrix<Ord,1,2>& iPoint) { return iPoint; }
template <class Ord> const ZMatrix<Ord,1,2>& WH(ZMatrix<Ord,1,2>& ioPoint) { return ioPoint; }

// =================================================================================================
// MARK: - RectTraits

template <>
template <class Ord>
struct RectTraits<ZMatrixArray<Ord,1,2,2> >
	{
	typedef Ord Ord_t;
	typedef ZMatrix<Ord,1,2> Point_t;
	typedef ZMatrixArray<Ord,1,2,2> Rect_t;
	
	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{
		ZMatrixArray<Ord,1,2,2> result;
		if (iL < iR)
			{
			X(LT(result)) = iL;
			X(RB(result)) = iR;
			}
		else
			{
			X(LT(result)) = iR;
			X(RB(result)) = iL;
			}

		if (iT < iB)
			{
			Y(LT(result)) = iT;
			Y(RB(result)) = iB;
			}
		else
			{
			Y(LT(result)) = iB;
			Y(RB(result)) = iT;
			}
		return result;
		}
	};

// =================================================================================================
// MARK: - Rect accessors

template <class Ord> const Ord& L(const ZMatrixArray<Ord,1,2,2>& iRect) { return X(LT(iRect)); }
template <class Ord> Ord& L(ZMatrixArray<Ord,1,2,2>& ioRect) { return X(LT(ioRect)); }

template <class Ord> const Ord& T(const ZMatrixArray<Ord,1,2,2>& iRect) { return Y(LT(iRect)); }
template <class Ord> Ord& T(ZMatrixArray<Ord,1,2,2>& ioRect) { return Y(LT(ioRect)); }

template <class Ord> const Ord& R(const ZMatrixArray<Ord,1,2,2>& iRect) { return X(RB(iRect)); }
template <class Ord> Ord& R(ZMatrixArray<Ord,1,2,2>& ioRect) { return X(RB(ioRect)); }

template <class Ord> const Ord& B(const ZMatrixArray<Ord,1,2,2>& iRect) { return Y(RB(iRect)); }
template <class Ord> Ord& B(ZMatrixArray<Ord,1,2,2>& ioRect) { return Y(RB(ioRect)); }

template <class Ord>
const Ord W(const ZMatrixArray<Ord,1,2,2>& iRect) { return R(iRect) - L(iRect); }

template <class Ord>
const Ord H(const ZMatrixArray<Ord,1,2,2>& iRect) { return B(iRect) - T(iRect); }

template <class Ord>
const ZMatrix<Ord,1,2> WH(const ZMatrixArray<Ord,1,2,2>& iRect)
	{ return sPoint<ZMatrix<Ord,1,2> >(W(iRect), H(iRect)); }

template <class Ord>
const ZMatrix<Ord,1,2>& LT(const ZMatrixArray<Ord,1,2,2>& iRect) { return iRect[0]; }

template <class Ord> ZMatrix<Ord,1,2>& LT(ZMatrixArray<Ord,1,2,2>& iRect) { return iRect[0]; }

template <class Ord>
const ZMatrix<Ord,1,2>& RB(const ZMatrixArray<Ord,1,2,2>& iRect) { return iRect[1]; }

template <class Ord> ZMatrix<Ord,1,2>& RB(ZMatrixArray<Ord,1,2,2>& iRect) { return iRect[1]; }

template <class Ord>
const ZMatrix<Ord,1,2> LB(const ZMatrixArray<Ord,1,2,2>& iRect)
	{ return sPoint<ZMatrix<Ord,1,2> >(L(iRect), B(iRect)); }

template <class Ord>
const ZMatrix<Ord,1,2> RT(const ZMatrixArray<Ord,1,2,2>& iRect)
	{ return sPoint<ZMatrix<Ord,1,2> >(R(iRect), T(iRect)); }

} // namespace ZCartesian
} // namespace ZooLib

#endif // __ZCartesian_Matrix_h__
