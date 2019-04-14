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

#ifndef __ZooLib_Cartesian_h__
#define __ZooLib_Cartesian_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h" // For std::min, std::max

#include "zoolib/ZStdInt.h" // For int64
#include "zoolib/ZTypes.h" // For EnableIfC

namespace ZooLib {
namespace Cartesian {

/*
Abbreviations
-------------
X = X coordinate (horizontal)
Y = Y coordinate (vertical)
Z = Z coordinate (not used here)

L = Left
T = Top
R = Right
B = Bottom
N = Near (not used here)
F = Far (not used here)

W = Width (distance on X axis)
H = Height (distance on Y axis)
D = Depth (distance on Z axis, not used here)

LT = Left Top
RB = Right Bottom
LB = Left Bottom
RT = Right Top

WH = Width Height (extent or size)

C = Center
CX = aka CLR, Center X
CY = aka CTB, Center Y

Operations
----------
Contains
Aligned, Centered, Flipped, Insetted, Offsetted, With
*/

// =================================================================================================
#pragma mark - PointTraits and accessors

template <class Type_p> struct PointTraits;

template <class Type_p>
typename PointTraits<Type_p>::XC_t X(const Type_p& iT) { return PointTraits<Type_p>::sX(iT); }
template <class Type_p>
typename PointTraits<Type_p>::X_t X(Type_p& iT) { return PointTraits<Type_p>::sX(iT); }

template <class Type_p>
typename PointTraits<Type_p>::YC_t Y(const Type_p& iT) { return PointTraits<Type_p>::sY(iT); }
template <class Type_p>
typename PointTraits<Type_p>::Y_t Y(Type_p& iT) { return PointTraits<Type_p>::sY(iT); }

// =================================================================================================
#pragma mark - PointTraits for scalars

#define ZMACRO_Cartesian(T) \
	template <> struct PointTraits<T> \
		{ \
		enum {value = 1}; \
		typedef T Ord_t; \
		typedef const Ord_t& XC_t; \
		inline static XC_t sX(const Ord_t& iT) { return iT; } \
		typedef Ord_t& X_t; \
		inline static X_t sX(Ord_t& ioT) { return ioT; } \
		typedef const Ord_t& YC_t; \
		inline static YC_t sY(const Ord_t& iT) { return iT; } \
		typedef Ord_t& Y_t; \
		inline static Y_t sY(Ord_t& ioT) { return ioT; } \
		};

ZMACRO_Cartesian(char)
ZMACRO_Cartesian(signed char)
ZMACRO_Cartesian(short)
ZMACRO_Cartesian(int)
ZMACRO_Cartesian(long)
ZMACRO_Cartesian(__int64)
ZMACRO_Cartesian(float)
ZMACRO_Cartesian(double)
ZMACRO_Cartesian(long double)

#undef ZMACRO_Cartesian

// =================================================================================================
#pragma mark - Fundamental Point Pseudo-ctor

template <class Point_p>
typename PointTraits<Point_p>::Point_t
sPoint(
	const typename PointTraits<Point_p>::Ord_t& iX,
	const typename PointTraits<Point_p>::Ord_t& iY)
	{ return PointTraits<Point_p>::sMake(iX, iY); }

// =================================================================================================
#pragma mark - Other Point Pseudo-ctors.

// Null == (0,0)
// Usage: sPoint<PointType>()
template <class Point_p>
typename PointTraits<Point_p>::Point_t
sPoint(
	)
	{ return PointTraits<Point_p>::sMake(0, 0); }

// From a single parameter that's point-like (will work for scalars too).
// Usage: sPoint<PointType>(OtherPointType)
template <class Point_p, class OtherPoint_p>
typename EnableIfC<PointTraits<OtherPoint_p>::value,typename PointTraits<Point_p>::Point_t>::type
sPoint(
	const OtherPoint_p& iOther)
	{ return sPoint<Point_p>(X(iOther), Y(iOther)); }

// =================================================================================================
#pragma mark - RectTraits and accessors

template <class Type_p> struct RectTraits;

template <class Type_p>
typename RectTraits<Type_p>::LC_t L(const Type_p& iT) { return RectTraits<Type_p>::sL(iT); }
template <class Type_p>
typename RectTraits<Type_p>::L_t L(Type_p& iT) { return RectTraits<Type_p>::sL(iT); }

template <class Type_p>
typename RectTraits<Type_p>::TC_t T(const Type_p& iT) { return RectTraits<Type_p>::sT(iT); }
template <class Type_p>
typename RectTraits<Type_p>::T_t T(Type_p& iT) { return RectTraits<Type_p>::sT(iT); }

template <class Type_p>
typename RectTraits<Type_p>::RC_t R(const Type_p& iT) { return RectTraits<Type_p>::sR(iT); }
template <class Type_p>
typename RectTraits<Type_p>::R_t R(Type_p& iT) { return RectTraits<Type_p>::sR(iT); }

template <class Type_p>
typename RectTraits<Type_p>::BC_t B(const Type_p& iT) { return RectTraits<Type_p>::sB(iT); }
template <class Type_p>
typename RectTraits<Type_p>::B_t B(Type_p& iT) { return RectTraits<Type_p>::sB(iT); }

template <class Type_p>
typename RectTraits<Type_p>::WC_t W(const Type_p& iT) { return RectTraits<Type_p>::sW(iT); }
template <class Type_p>
typename RectTraits<Type_p>::W_t W(Type_p& iT) { return RectTraits<Type_p>::sW(iT); }

template <class Type_p>
typename RectTraits<Type_p>::HC_t H(const Type_p& iT) { return RectTraits<Type_p>::sH(iT); }
template <class Type_p>
typename RectTraits<Type_p>::H_t H(Type_p& iT) { return RectTraits<Type_p>::sH(iT); }

template <class Type_p>
typename RectTraits<Type_p>::WHC_t WH(const Type_p& iT) { return RectTraits<Type_p>::sWH(iT); }
template <class Type_p>
typename RectTraits<Type_p>::WH_t WH(Type_p& iT) { return RectTraits<Type_p>::sWH(iT); }

template <class Type_p>
typename RectTraits<Type_p>::LTC_t LT(const Type_p& iT) { return RectTraits<Type_p>::sLT(iT); }
template <class Type_p>
typename RectTraits<Type_p>::LT_t LT(Type_p& iT) { return RectTraits<Type_p>::sLT(iT); }

template <class Type_p>
typename RectTraits<Type_p>::RBC_t RB(const Type_p& iT) { return RectTraits<Type_p>::sRB(iT); }
template <class Type_p>
typename RectTraits<Type_p>::RB_t RB(Type_p& iT) { return RectTraits<Type_p>::sRB(iT); }

template <class Type_p>
typename RectTraits<Type_p>::LBC_t LB(const Type_p& iT) { return RectTraits<Type_p>::sLB(iT); }
template <class Type_p>
typename RectTraits<Type_p>::LB_t LB(Type_p& iT) { return RectTraits<Type_p>::sLB(iT); }

template <class Type_p>
typename RectTraits<Type_p>::RTC_t RT(const Type_p& iT) { return RectTraits<Type_p>::sRT(iT); }
template <class Type_p>
typename RectTraits<Type_p>::RT_t RT(Type_p& iT) { return RectTraits<Type_p>::sRT(iT); }

// =================================================================================================
#pragma mark - Fundamental Rect Pseudo-ctor.

template <class Rect_p>
typename RectTraits<Rect_p>::Rect_t
sRect(
	const typename RectTraits<Rect_p>::Ord_t& iL,
	const typename RectTraits<Rect_p>::Ord_t& iT,
	const typename RectTraits<Rect_p>::Ord_t& iR,
	const typename RectTraits<Rect_p>::Ord_t& iB)
	{ return RectTraits<Rect_p>::sMake(iL, iT, iR, iB); }

// =================================================================================================
#pragma mark - Other Rect Pseudo-ctors.

// Null == ((0,0),(0,0)).
// Usage: sRect<RectType>()
template <class Rect_p>
typename RectTraits<Rect_p>::Rect_t
sRect(
	)
	{ return RectTraits<Rect_p>::sMake(0, 0, 0, 0); }

// From a single parameter whose PointTraits determines the returned rect type.
// Usage: sRect(PointType)
template <class Point_p>
typename PointTraits<Point_p>::Rect_t
sRect(
	const Point_p& iPoint)
	{ return sRect<typename PointTraits<Point_p>::Rect_t>(0, 0, X(iPoint), Y(iPoint)); }

// From a single parameter that is Point-like (scalars too by virtue of the
// PointTraits above), but with the rect type as a template parameter.
// Usage: sRect<RectType>(PointType)
template <class Rect_p, class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,typename RectTraits<Rect_p>::Rect_t>::type
sRect(
	const Point_p& iPoint)
	{ return sRect<Rect_p>(0, 0, X(iPoint), Y(iPoint)); }

// From a single parameter that is Rect-like, with the returned rect type as a template parameter.
// Usage: sRect<RectType>(OtherRectType)
template <class Rect_p, class OtherRect_p>
typename EnableIfC<RectTraits<OtherRect_p>::value,typename RectTraits<Rect_p>::Rect_t>::type
sRect(
	const OtherRect_p& iOther)
	{ return sRect<Rect_p>(L(iOther), T(iOther), R(iOther), B(iOther)); }

// From a pair of parameters, expected to be point-like LT and RB.
// Usage: sRect(PointType, PointType)
template <class Point_p>
typename PointTraits<Point_p>::Rect_t
sRect(
	const Point_p& iLT,
	const Point_p& iRB)
	{ return sRect<typename PointTraits<Point_p>::Rect_t>(X(iLT), Y(iLT), X(iRB), Y(iRB)); }

// From a pair of parameters, something convertible to the width and height for
// the rect type specified as a template parameter.
// Usage: sRect<RectType>(WidthType, HeightType)
template <class Rect_p, class Width_p, class Height_p>
typename RectTraits<Rect_p>::Rect_t
sRect(
	const Width_p& iW,
	const Height_p& iH)
	{ return RectTraits<Rect_p>::sMake(0, 0, iW, iH); }

// =================================================================================================
#pragma mark - Point Comparison Operators

namespace Operators {

template <class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,bool>::type
operator==(const Point_p& iLHS, const Point_p& iRHS)
	{ return X(iLHS) == X(iRHS) && Y(iLHS) == Y(iRHS); }

template <class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,bool>::type
operator!=(const Point_p& iLHS, const Point_p& iRHS)
	{ return not (iLHS == iRHS); }

template <class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,bool>::type
operator<(const Point_p& iLHS, const Point_p& iRHS)
	{ return X(iLHS) < X(iRHS) || (X(iLHS) == X(iRHS) && Y(iLHS) < Y(iRHS)); }

template <class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,bool>::type
operator>(const Point_p& iLHS, const Point_p& iRHS)
	{ return iRHS < iLHS; }

template <class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,bool>::type
operator<=(const Point_p& iLHS, const Point_p& iRHS)
	{ return not (iRHS < iLHS); }

template <class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,bool>::type
operator>=(const Point_p& iLHS, const Point_p& iRHS)
	{ return not (iLHS < iRHS); }

} // namespace Operators

// =================================================================================================
#pragma mark - Point Manipulation functions

template <class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,Point_p>::type
sTransposed(const Point_p& iPoint)
	{ return sPoint<Point_p>(Y(iPoint), X(iPoint)); }

// =================================================================================================
#pragma mark - Point Manipulation Operators

namespace Operators {

template <class Point_p>
typename PointTraits<Point_p>::Point_t
operator-(const Point_p& iPoint)
	{ return sPoint<Point_p>(-X(iPoint), -Y(iPoint)); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t
operator+(const Point_p& iLHS, const Other& iRHS)
	{ return sPoint<Point_p>(X(iLHS) + X(iRHS), Y(iLHS) + Y(iRHS)); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t&
operator+=(Point_p& ioL, const Other& iRHS)
	{
	X(ioL) += X(iRHS);
	Y(ioL) += Y(iRHS);
	return ioL;
	}

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t
operator-(const Point_p& iLHS, const Other& iRHS)
	{ return sPoint<Point_p>(X(iLHS) - X(iRHS), Y(iLHS) - Y(iRHS)); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t&
operator-=(Point_p& ioL, const Other& iRHS)
	{
	X(ioL) -= X(iRHS);
	Y(ioL) -= Y(iRHS);
	return ioL;
	}

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t
operator*(const Point_p& iLHS, const Other& iRHS)
	{ return sPoint<Point_p>(X(iLHS) * X(iRHS), Y(iLHS) * Y(iRHS)); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t&
operator*=(Point_p& ioL, const Other& iRHS)
	{
	X(ioL) *= X(iRHS);
	Y(ioL) *= Y(iRHS);
	return ioL;
	}

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t
operator/(const Point_p& iLHS, const Other& iRHS)
	{ return sPoint<Point_p>(X(iLHS) / X(iRHS), Y(iLHS) / Y(iRHS)); }

template <class Point_p, class Other>
typename PointTraits<Point_p>::Point_t&
operator/=(Point_p& ioL, const Other& iRHS)
	{
	X(ioL) /= X(iRHS);
	Y(ioL) /= Y(iRHS);
	return ioL;
	}

} // namespace Operators

// =================================================================================================
#pragma mark - Rect Comparison Operators

namespace Operators {

template <class Rect_p>
typename EnableIfC<RectTraits<Rect_p>::value,bool>::type
operator==(const Rect_p& iLHS, const Rect_p& iRHS)
	{ return LT(iLHS) == LT(iRHS) && RB(iLHS) == RB(iRHS); }

template <class Rect_p>
typename EnableIfC<RectTraits<Rect_p>::value,bool>::type
operator!=(const Rect_p& iLHS, const Rect_p& iRHS)
	{ return not (iLHS == iRHS); }

template <class Rect_p>
typename EnableIfC<RectTraits<Rect_p>::value,bool>::type
operator<(const Rect_p& iLHS, const Rect_p& iRHS)
	{ return LT(iLHS) < LT(iRHS) || (LT(iLHS) == LT(iRHS) && RB(iLHS) < RB(iRHS)); }

template <class Rect_p>
typename EnableIfC<RectTraits<Rect_p>::value,bool>::type
operator>(const Rect_p& iLHS, const Rect_p& iRHS)
	{ return iRHS < iLHS; }

template <class Rect_p>
typename EnableIfC<RectTraits<Rect_p>::value,bool>::type
operator<=(const Rect_p& iLHS, const Rect_p& iRHS)
	{ return not (iRHS < iLHS); }

template <class Rect_p>
typename EnableIfC<RectTraits<Rect_p>::value,bool>::type
operator>=(const Rect_p& iLHS, const Rect_p& iRHS)
	{ return not (iLHS < iRHS); }

} // namespace Operators

// =================================================================================================
#pragma mark - Rect Manipulation Operators

namespace Operators {

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t
operator+(const Rect_p& iLHS, const Other& iRHS)
	{
	return sRect<Rect_p>(
		L(iLHS) + X(iRHS),
		T(iLHS) + Y(iRHS),
		R(iLHS) + X(iRHS),
		B(iLHS) + Y(iRHS));
	}

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t&
operator+=(Rect_p& ioL, const Other& iRHS)
	{ return ioL = ioL + iRHS; }

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t
operator-(const Rect_p& iLHS, const Other& iRHS)
	{
	return sRect<Rect_p>(
		L(iLHS) - X(iRHS),
		T(iLHS) - Y(iRHS),
		R(iLHS) - X(iRHS),
		B(iLHS) - Y(iRHS));
	}

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t&
operator-=(Rect_p& ioL, const Other& iRHS)
	{ return ioL = ioL - iRHS; }

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t
operator*(const Rect_p& iLHS, const Other& iRHS)
	{
	return sRect<Rect_p>(
		L(iLHS) * X(iRHS),
		T(iLHS) * Y(iRHS),
		R(iLHS) * X(iRHS),
		B(iLHS) * Y(iRHS));
	}

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t&
operator*=(Rect_p& ioL, const Other& iRHS)
	{ return ioL = ioL * iRHS; }

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t
operator/(const Rect_p& iLHS, const Other& iRHS)
	{
	return sRect<Rect_p>(
		L(iLHS) / X(iRHS),
		T(iLHS) / Y(iRHS),
		R(iLHS) / X(iRHS),
		B(iLHS) / Y(iRHS));
	}

template <class Rect_p, class Other>
typename RectTraits<Rect_p>::Rect_t&
operator/=(Rect_p& ioL, const Other& iRHS)
	{ return ioL = ioL / iRHS; }

template <class Rect_p>
typename RectTraits<Rect_p>::Rect_t
operator&(const Rect_p& iLHS, const Rect_p& iRHS)
	{
	return sRect<Rect_p>(
		std::max(L(iLHS), L(iRHS)),
		std::max(T(iLHS), T(iRHS)),
		std::min(R(iLHS), R(iRHS)),
		std::min(B(iLHS), B(iRHS)));
	}

template <class Rect_p>
typename RectTraits<Rect_p>::Rect_t&
operator&=(Rect_p& ioL, const Rect_p& iRHS)
	{ return ioL = ioL & iRHS; }

template <class Rect_p>
typename RectTraits<Rect_p>::Rect_t
operator|(const Rect_p& iLHS, const Rect_p& iRHS)
	{
	return sRect<Rect_p>(
		std::min(L(iLHS), L(iRHS)),
		std::min(T(iLHS), T(iRHS)),
		std::max(R(iLHS), R(iRHS)),
		std::max(B(iLHS), B(iRHS)));
	}

template <class Rect_p>
typename RectTraits<Rect_p>::Rect_t&
operator|=(Rect_p& ioL, const Rect_p& iRHS)
	{ return ioL = ioL | iRHS; }

} // namespace Operators

// =================================================================================================
#pragma mark - sManhattanLength and sLength

template <class Point_p>
typename PointTraits<Point_p>::Ord_t
sManhattanLength(const Point_p& iPoint)
	{ return X(iPoint) + Y(iPoint); }

template <class Point_p>
typename PointTraits<Point_p>::Ord_t
sLength(const Point_p& iPoint)
	{ return sqrt(sManhattanLength(iPoint * iPoint)); }

// =================================================================================================
#pragma mark - sIsEmpty

template <class Rect_p>
bool sIsEmpty(const Rect_p& iRect)
	{ return L(iRect) >= R(iRect) || T(iRect) >= B(iRect); }

// =================================================================================================
#pragma mark - sNotEmpty

template <class Rect_p>
bool sNotEmpty(const Rect_p& iRect)
	{ return not sIsEmpty(iRect); }

// =================================================================================================
#pragma mark - Center

template <class Rect_p>
typename RectTraits<Rect_p>::Ord_t
CX(const Rect_p& iRect)
	{ return (L(iRect) + R(iRect)) / 2; }

template <class Rect_p>
typename RectTraits<Rect_p>::Ord_t
CY(const Rect_p& iRect)
	{ return (T(iRect) + B(iRect)) / 2; }

template <class Rect_p>
typename RectTraits<Rect_p>::Point_t
CC(const Rect_p& iRect)
	{ return sPoint<typename RectTraits<Rect_p>::Point_t>(CX(iRect), CY(iRect)); }

template <class Rect_p>
typename RectTraits<Rect_p>::Point_t
CT(const Rect_p& iRect)
	{ return sPoint<typename RectTraits<Rect_p>::Point_t>(CX(iRect), T(iRect)); }

template <class Rect_p>
typename RectTraits<Rect_p>::Point_t
CB(const Rect_p& iRect)
	{ return sPoint<typename RectTraits<Rect_p>::Point_t>(CX(iRect), B(iRect)); }

template <class Rect_p>
typename RectTraits<Rect_p>::Point_t
LC(const Rect_p& iRect)
	{ return sPoint<typename RectTraits<Rect_p>::Point_t>(L(iRect), CY(iRect)); }

template <class Rect_p>
typename RectTraits<Rect_p>::Point_t
RC(const Rect_p& iRect)
	{ return sPoint<typename RectTraits<Rect_p>::Point_t>(R(iRect), CY(iRect)); }

// =================================================================================================
#pragma mark - sContains

template <class Rect_p, class OtherX_p>
bool sContainsX(const Rect_p& iRect, const OtherX_p& iX)
	{ return X(iX) >= L(iRect) && X(iX) < R(iRect); }

template <class Rect_p, class OtherY_p>
bool sContainsY(const Rect_p& iRect, const OtherY_p& iY)
	{ return Y(iY) >= T(iRect) && Y(iY) < B(iRect); }

template <class Rect_p, class OtherX_p, class OtherY_p>
bool sContains(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{
	return X(iX) >= L(iRect) && X(iX) < R(iRect)
		&& Y(iY) >= T(iRect) && Y(iY) < B(iRect);
	}

template <class Rect_p, class Other>
bool sContains(const Rect_p& iRect, const Other& iOther)
	{
	return X(iOther) >= L(iRect) && X(iOther) < R(iRect)
		&& Y(iOther) >= T(iRect) && Y(iOther) < B(iRect);
	}

// =================================================================================================
#pragma mark - sOffsetted, whole

template <class Rect_p, class OtherX_p>
Rect_p sOffsettedX(const Rect_p& iRect, const OtherX_p& iX)
	{ return sRect<Rect_p>(L(iRect) + X(iX), T(iRect), R(iRect) + X(iX), B(iRect)); }

template <class Rect_p, class OtherY_p>
Rect_p sOffsettedY(const Rect_p& iRect, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iY), R(iRect), B(iRect) + Y(iY)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
typename RectTraits<Rect_p>::Rect_t
sOffsetted(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{
	return sRect<Rect_p>(
		L(iRect) + X(iX), T(iRect) + Y(iY),
		R(iRect) + X(iX), B(iRect) + Y(iY));
	}

template <class Rect_p, class OtherL_p, class OtherT_p, class OtherR_p, class OtherB_p>
typename RectTraits<Rect_p>::Rect_t
sOffsetted(const OtherL_p& iL, const OtherT_p& iT, const OtherR_p& iR, const OtherB_p& iB,
	const Rect_p& iRect)
	{
	return sRect<Rect_p>(
		L(iRect) + iL, T(iRect) + iT,
		R(iRect) + iR, B(iRect) + iB);
	}

template <class Rect_p, class Other>
Rect_p sOffsetted(const Rect_p& iRect, const Other& iOther)
	{
	return sRect<Rect_p>(
		L(iRect) + X(iOther), T(iRect) + Y(iOther), R(iRect) + X(iOther), B(iRect) + Y(iOther));
	}

// =================================================================================================
#pragma mark - sOffsetted, edge

template <class Rect_p, class OtherX_p>
Rect_p sOffsettedL(const Rect_p& iRect, const OtherX_p& iX)
	{ return sRect<Rect_p>(L(iRect) + X(iX), T(iRect), R(iRect), B(iRect)); }

template <class Rect_p, class OtherY_p>
Rect_p sOffsettedT(const Rect_p& iRect, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX_p>
Rect_p sOffsettedR(const Rect_p& iRect, const OtherX_p& iX)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect) + X(iX), B(iRect)); }

template <class Rect_p, class OtherY_p>
Rect_p sOffsettedB(const Rect_p& iRect, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect), B(iRect) + Y(iY)); }

// =================================================================================================
#pragma mark - sOffsetted, corner

template <class Rect_p, class Other_p>
Rect_p sOffsettedLT(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(L(iRect) + X(iOther), T(iRect) + Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sOffsettedRB(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect) + X(iOther), B(iRect) + Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sOffsettedLB(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(L(iRect) + X(iOther), T(iRect), R(iRect), B(iRect) + Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sOffsettedRT(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iOther), R(iRect) + X(iOther), B(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sOffsettedLT(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect) + X(iX), T(iRect) + Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sOffsettedRB(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect) + X(iX), B(iRect) + Y(iY)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sOffsettedLB(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect) + X(iX), T(iRect), R(iRect), B(iRect) + Y(iY)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sOffsettedRT(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iY), R(iRect) + X(iX), B(iRect)); }

// =================================================================================================
#pragma mark - sAligned, edge

template <class Rect_p, class OtherX_p>
Rect_p sAlignedL(const Rect_p& iRect, const OtherX_p& iX)
	{ return sOffsetted(iRect, X(iX) - L(iRect), 0); }

template <class Rect_p, class OtherY_p>
Rect_p sAlignedT(const Rect_p& iRect, const OtherY_p& iY)
	{ return sOffsetted(iRect, 0, Y(iY) - T(iRect)); }

template <class Rect_p, class OtherX_p>
Rect_p sAlignedR(const Rect_p& iRect, const OtherX_p& iX)
	{ return sOffsetted(iRect, X(iX) - R(iRect), 0); }

template <class Rect_p, class OtherY_p>
Rect_p sAlignedB(const Rect_p& iRect, const OtherY_p& iY)
	{ return sOffsetted(iRect, 0, Y(iY) - B(iRect)); }

// =================================================================================================
#pragma mark - sAligned, corner

template <class Rect_p, class Other_p>
Rect_p sAlignedLT(const Rect_p& iRect, const Other_p& iOther)
	{ return sOffsetted(iRect, X(iOther) - L(iRect), Y(iOther) - T(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sAlignedRB(const Rect_p& iRect, const Other_p& iOther)
	{ return sOffsetted(iRect, X(iOther) - R(iRect), Y(iOther) - B(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sAlignedLB(const Rect_p& iRect, const Other_p& iOther)
	{ return sOffsetted(iRect, X(iOther) - L(iRect), Y(iOther) - B(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sAlignedRT(const Rect_p& iRect, const Other_p& iOther)
	{ return sOffsetted(iRect, X(iOther) - R(iRect), Y(iOther) - T(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sAlignedLT(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sOffsetted(iRect, X(iX) - L(iRect), Y(iY) - T(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sAlignedRB(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sOffsetted(iRect, X(iX) - R(iRect), Y(iY) - B(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sAlignedLB(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sOffsetted(iRect, X(iX) - L(iRect), Y(iY) - B(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sAlignedRT(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sOffsetted(iRect, X(iX) - R(iRect), Y(iY) - T(iRect)); }

// =================================================================================================
#pragma mark - sCentered

template <class Rect_p, class OtherX_p>
Rect_p sCenteredX(const Rect_p& iRect, const OtherX_p& iX)
	{ return sOffsetted(iRect, X(iX) - CX(iRect), 0); }

template <class Rect_p, class OtherY_p>
Rect_p sCenteredY(const Rect_p& iRect, const OtherY_p& iY)
	{ return sOffsetted(iRect, 0, Y(iY) - CY(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sCentered(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sOffsetted(iRect, X(iX) - CX(iRect), Y(iY) - CY(iRect)); }

template <class Rect_p, class Other>
Rect_p sCentered(const Rect_p& iRect, const Other& iOther)
	{ return sOffsetted(iRect, X(iOther) - CX(iRect), Y(iOther) - CY(iRect)); }

// =================================================================================================
#pragma mark - sFlippedY

template <class Rect_p, class Other_p>
Rect_p sFlippedY(const Rect_p& iRect, const Other_p& iY)
	{ return sRect<Rect_p>(L(iRect), Y(iY) - B(iRect), R(iRect), Y(iY) - T(iRect)); }

// =================================================================================================
#pragma mark - sInsetted

template <class Rect_p, class OtherX_p>
Rect_p sInsettedX(const Rect_p& iRect, const OtherX_p& iX)
	{ return sRect<Rect_p>(L(iRect) + X(iX), T(iRect), R(iRect) - X(iX), B(iRect)); }

template <class Rect_p, class OtherY_p>
Rect_p sInsettedY(const Rect_p& iRect, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iY), R(iRect), B(iRect) - Y(iY)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sInsetted(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{
	return sRect<Rect_p>(
		L(iRect) + X(iX), T(iRect) + Y(iY),
		R(iRect) - X(iX), B(iRect) - Y(iY));
	}

template <class Rect_p, class Other>
Rect_p sInsetted(const Rect_p& iRect, const Other& iOther)
	{
	return sRect<Rect_p>(
		L(iRect) + X(iOther), T(iRect) + Y(iOther), R(iRect) - X(iOther), B(iRect) - Y(iOther));
	}

// =================================================================================================
#pragma mark - sWith, edge

template <class Rect_p, class OtherX_p>
Rect_p sWithL(const Rect_p& iRect, const OtherX_p& iX)
	{ return sRect<Rect_p>(X(iX), T(iRect), R(iRect), B(iRect)); }

template <class Rect_p, class OtherY_p>
Rect_p sWithT(const Rect_p& iRect, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX_p>
Rect_p sWithR(const Rect_p& iRect, const OtherX_p& iX)
	{ return sRect<Rect_p>(L(iRect), T(iRect), X(iX), B(iRect)); }

template <class Rect_p, class OtherY_p>
Rect_p sWithB(const Rect_p& iRect, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect), Y(iY)); }

// =================================================================================================
#pragma mark - sWith, corner

template <class Rect_p, class Other_p>
Rect_p sWithLT(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(X(iOther), Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sWithRB(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(L(iRect), T(iRect), X(iOther), Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithLB(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(X(iOther), T(iRect), R(iRect), Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithRT(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(L(iOther), Y(iOther), X(iOther), B(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sWithLT(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(X(iX), Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sWithRB(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect), X(iX), Y(iY)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sWithLB(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(X(iX), T(iRect), R(iRect), Y(iY)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sWithRT(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), Y(iY), X(iX), B(iRect)); }

template <class Rect_p, class OtherL_p, class OtherR_p>
Rect_p sWithLR(const Rect_p& iRect, const OtherL_p& iLHS, const OtherR_p& iR)
	{ return sRect<Rect_p>(X(iLHS), T(iRect), X(iR), B(iRect)); }

template <class Rect_p, class OtherT_p, class OtherB_p>
Rect_p sWithTB(const Rect_p& iRect, const OtherT_p& iT, const OtherB_p& iB)
	{ return sRect<Rect_p>(L(iRect), Y(iT), R(iRect), Y(iB)); }

// =================================================================================================
#pragma mark - sWithW, with Width by moving L or R edge

template <class Rect_p, class OtherX_p>
Rect_p sWithWL(const Rect_p& iRect, const OtherX_p& iX)
	{ return sRect<Rect_p>(R(iRect) - X(iX), T(iRect), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX_p>
Rect_p sWithWR(const Rect_p& iRect, const OtherX_p& iX)
	{ return sRect<Rect_p>(L(iRect), T(iRect), L(iRect) + X(iX), B(iRect)); }

// =================================================================================================
#pragma mark - sWithH, with Height by moving T or B edge

template <class Rect_p, class OtherY_p>
Rect_p sWithHT(const Rect_p& iRect, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), B(iRect) - Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherY_p>
Rect_p sWithHB(const Rect_p& iRect, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect), T(iRect) + Y(iY)); }

// =================================================================================================
#pragma mark - sWithWH, with Width and Height by moving LT, RB, LB, RT corner

template <class Rect_p, class Other_p>
Rect_p sWithWHLT(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(R(iRect) - X(iOther), B(iRect) - Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sWithWHRB(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(L(iRect), T(iRect), L(iRect) + X(iOther), B(iRect) + Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithWHLB(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(R(iRect) - X(iOther), T(iRect), R(iRect), B(iRect) + Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithWHRT(const Rect_p& iRect, const Other_p& iOther)
	{ return sRect<Rect_p>(L(iRect), B(iRect) - Y(iOther), L(iRect) + X(iOther), B(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sWithWHLT(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(R(iRect) - X(iX), B(iRect) - Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sWithWHRB(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), T(iRect), L(iRect) + X(iX), B(iRect) + Y(iY)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sWithWHLB(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(R(iRect) - X(iX), T(iRect), R(iRect), B(iRect) + Y(iY)); }

template <class Rect_p, class OtherX_p, class OtherY_p>
Rect_p sWithWHRT(const Rect_p& iRect, const OtherX_p& iX, const OtherY_p& iY)
	{ return sRect<Rect_p>(L(iRect), B(iRect) - Y(iY), L(iRect) + X(iX), B(iRect)); }

// =================================================================================================
#pragma mark - PointTraitsBase

template <class Type_p>
struct PointTraitsBase
	{
	enum {value=1};
	};

// =================================================================================================
#pragma mark - PointTraits_Std

template <class Ord_p, class Point_p, class Rect_p>
struct PointTraits_Std
:	public PointTraitsBase<Point_p>
	{
	typedef Ord_p Ord_t;
	typedef Point_p Point_t;
	typedef Rect_p Rect_t;

	static Point_t sMake(const Ord_t& iX, const Ord_t& iY)
		{ return Point_t(iX, iY); }
	};

// =================================================================================================
#pragma mark - PointTraits_Std_XY

template <class Ord_p, class Point_p, class Rect_p>
struct PointTraits_Std_XY
:	public PointTraits_Std<Ord_p,Point_p,Rect_p>
	{
	typedef const Ord_p& XC_t;
	static XC_t sX(const Point_p& iPoint) { return iPoint.x; }

	typedef Ord_p& X_t;
	static X_t sX(Point_p& ioPoint) { return ioPoint.x; }

	typedef const Ord_p& YC_t;
	static YC_t sY(const Point_p& iPoint) { return iPoint.y; }

	typedef Ord_p& Y_t;
	static Y_t sY(Point_p& ioPoint) { return ioPoint.y; }
	};

// =================================================================================================
#pragma mark - PointTraits_Std_HV

template <class Ord_p, class Point_p, class Rect_p>
struct PointTraits_Std_HV
:	public PointTraits_Std<Ord_p,Point_p,Rect_p>
	{
	typedef const Ord_p& XC_t;
	static XC_t sX(const Point_p& iPoint) { return iPoint.h; }

	typedef Ord_p& X_t;
	static X_t sX(Point_p& ioPoint) { return ioPoint.h; }

	typedef const Ord_p& YC_t;
	static YC_t sY(const Point_p& iPoint) { return iPoint.v; }

	typedef Ord_p& Y_t;
	static Y_t sY(Point_p& ioPoint) { return ioPoint.v; }
	};

// =================================================================================================
#pragma mark - PointTraits_Std_WidthHeight

template <class Ord_p, class Point_p, class Rect_p>
struct PointTraits_Std_WidthHeight
:	public PointTraits_Std<Ord_p,Point_p,Rect_p>
	{
	typedef const Ord_p& XC_t;
	static XC_t sX(const Point_p& iPoint) { return iPoint.width; }

	typedef Ord_p& X_t;
	static X_t sX(Point_p& ioPoint) { return ioPoint.width; }

	typedef const Ord_p& YC_t;
	static YC_t sY(const Point_p& iPoint) { return iPoint.height; }

	typedef Ord_p& Y_t;
	static Y_t sY(Point_p& ioPoint) { return ioPoint.height; }
	};

// =================================================================================================
#pragma mark - RectTraitsBase

template <class Type_p>
struct RectTraitsBase
	{
	enum {value=1};
	};

// =================================================================================================
#pragma mark - RectTraits_Std

template <class Ord_p, class Point_p, class Rect_p>
struct RectTraits_Std
:	public RectTraitsBase<Rect_p>
	{
	typedef Ord_p Ord_t;
	typedef Point_p Point_t;
	typedef Rect_p Rect_t;

	typedef const Ord_p WC_t;
	typedef WC_t W_t;
	static WC_t sW(const Rect_p& iRect) { return R(iRect) - L(iRect); }

	typedef const Ord_p HC_t;
	typedef HC_t H_t;
	static HC_t sH(const Rect_p& iRect) { return B(iRect) - T(iRect); }

	typedef const Point_p WHC_t;
	typedef WHC_t WH_t;
	static WHC_t sWH(const Rect_p& iRect) { return sPoint<Point_p>(W(iRect), H(iRect)); }

	typedef const Point_p LTC_t;
	typedef LTC_t LT_t;
	static LTC_t sLT(const Rect_p& iRect) { return sPoint<Point_p>(L(iRect), T(iRect)); }

	typedef const Point_p RBC_t;
	typedef RBC_t RB_t;
	static RBC_t sRB(const Rect_p& iRect) { return sPoint<Point_p>(R(iRect), B(iRect)); }

	typedef const Point_p LBC_t;
	typedef LBC_t LB_t;
	static LBC_t sLB(const Rect_p& iRect) { return sPoint<Point_p>(L(iRect), B(iRect)); }

	typedef const Point_p RTC_t;
	typedef RTC_t RT_t;
	static RTC_t sRT(const Rect_p& iRect) { return sPoint<Point_p>(R(iRect), T(iRect)); }
	};

// =================================================================================================
#pragma mark - RectTraits_Std_OriginSize

template <class Ord_p, class Origin_p, class Size_p, class Rect_p>
struct RectTraits_Std_OriginSize
:	public RectTraits_Std<Ord_p,Origin_p,Rect_p>
	{
	typedef const Ord_p& LC_t;
	static LC_t sL(const Rect_p& iRect) { return X(iRect.origin); }

	typedef Ord_p& L_t;
	static L_t sL(Rect_p& ioRect) { return X(ioRect.origin); }

	typedef const Ord_p& TC_t;
	static TC_t sT(const Rect_p& iRect) { return Y(iRect.origin); }

	typedef Ord_p& T_t;
	static T_t sT(Rect_p& ioRect) { return Y(ioRect.origin); }

	typedef const Ord_p RC_t;
	typedef RC_t R_t;
	static RC_t sR(const Rect_p& iRect) { return X(iRect.origin) + X(iRect.size); }

	typedef const Ord_p BC_t;
	typedef BC_t B_t;
	static BC_t sB(const Rect_p& iRect) { return Y(iRect.origin) + Y(iRect.size); }

	typedef const Ord_p& WC_t;
	static WC_t sW(const Rect_p& iRect) { return X(iRect.size); }

	typedef Ord_p& W_t;
	static W_t sW(Rect_p& ioRect) { return X(ioRect.size); }

	typedef const Ord_p& HC_t;
	static HC_t sH(const Rect_p& iRect) { return Y(iRect.size); }

	typedef Ord_p& H_t;
	static H_t sH(Rect_p& ioRect) { return Y(ioRect.size); }

	typedef const Size_p& WHC_t;
	static WHC_t sWH(const Rect_p& iRect) { return iRect.size; }

	typedef Size_p& WH_t;
	static WH_t sWH(Rect_p& ioRect) { return ioRect.size; }

	typedef const Origin_p& LTC_t;
	static LTC_t sLT(const Rect_p& iRect) { return iRect.origin; }

	typedef Origin_p& LT_t;
	static LT_t sLT(Rect_p& ioRect) { return ioRect.origin; }
	};

// =================================================================================================
#pragma mark - RectTraits_Std_XYWH_Base

template <class Ord_p, class Point_p, class Rect_p>
struct RectTraits_Std_XYWH_Base
:	public RectTraits_Std<Ord_p,Point_p,Rect_p>
	{
	typedef const Ord_p& LC_t;
	static LC_t sL(const Rect_p& iRect) { return iRect.x; }

	typedef Ord_p& L_t;
	static L_t sL(Rect_p& ioRect) { return ioRect.x; }

	typedef const Ord_p& TC_t;
	static TC_t sT(const Rect_p& iRect) { return iRect.y; }

	typedef Ord_p& T_t;
	static T_t sT(Rect_p& ioRect) { return ioRect.y; }

	typedef const Ord_p RC_t;
	typedef RC_t R_t;
	static RC_t sR(const Rect_p& iRect) { return iRect.x + W(iRect); }

	typedef const Ord_p BC_t;
	typedef BC_t B_t;
	static BC_t sB(const Rect_p& iRect) { return iRect.y + H(iRect); }
	};

// =================================================================================================
#pragma mark - RectTraits_Std_XYWH

template <class Ord_p, class Point_p, class Rect_p>
struct RectTraits_Std_XYWH
:	public RectTraits_Std_XYWH_Base<Ord_p,Point_p,Rect_p>
	{
	typedef const Ord_p& WC_t;
	static WC_t sW(const Rect_p& iRect) { return iRect.w; }

	typedef Ord_p& W_t;
	static W_t sW(Rect_p& ioRect) { return ioRect.w; }

	typedef const Ord_p& HC_t;
	static HC_t sH(const Rect_p& iRect) { return iRect.h; }

	typedef Ord_p& H_t;
	static H_t sH(Rect_p& ioRect) { return ioRect.h; }
	};

// =================================================================================================
#pragma mark - RectTraits_Std_XYWidthHeight

template <class Ord_p, class Point_p, class Rect_p>
struct RectTraits_Std_XYWidthHeight
:	public RectTraits_Std_XYWH_Base<Ord_p,Point_p,Rect_p>
	{
	typedef const Ord_p& WC_t;
	static WC_t sW(const Rect_p& iRect) { return iRect.width; }

	typedef Ord_p& W_t;
	static W_t sW(Rect_p& ioRect) { return ioRect.width; }

	typedef const Ord_p& HC_t;
	static HC_t sH(const Rect_p& iRect) { return iRect.height; }

	typedef Ord_p& H_t;
	static H_t sH(Rect_p& ioRect) { return ioRect.height; }
	};

// =================================================================================================
#pragma mark - RectTraits_Std_LeftTopRightBottom

template <class Ord_p, class Point_p, class Rect_p>
struct RectTraits_Std_LeftTopRightBottom
:	public RectTraits_Std<Ord_p,Point_p,Rect_p>
	{
	typedef const Ord_p& LC_t;
	static LC_t sL(const Rect_p& iRect) { return iRect.left; }

	typedef Ord_p& L_t;
	static L_t sL(Rect_p& ioRect) { return ioRect.left; }

	typedef const Ord_p& TC_t;
	static TC_t sT(const Rect_p& iRect) { return iRect.top; }

	typedef Ord_p& T_t;
	static T_t sT(Rect_p& ioRect) { return ioRect.top; }

	typedef const Ord_p& RC_t;
	static RC_t sR(const Rect_p& iRect) { return iRect.right; }

	typedef Ord_p& R_t;
	static R_t sR(Rect_p& ioRect) { return ioRect.right; }

	typedef const Ord_p& BC_t;
	static BC_t sB(const Rect_p& iRect) { return iRect.bottom; }

	typedef Ord_p& B_t;
	static B_t sB(Rect_p& ioRect) { return ioRect.bottom; }
	};

#if not defined(ZMACRO_Cartesian_SuppressOperators) || not ZMACRO_Cartesian_SuppressOperators
	using namespace Operators;
#endif

} // namespace Cartesian

using namespace Cartesian;

} // namespace ZooLib

#endif // __ZooLib_Cartesian_h__
