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

#ifndef __ZCartesian_h__
#define __ZCartesian_h__ 1
#include "zconfig.h"

#include "zoolib/ZStdInt.h" // For int64
#include "zoolib/ZCompat_algorithm.h" // For std::min/max

namespace ZooLib {
namespace ZCartesian {

/*
Abbreviations
-------------
X = X coordinate (horizontal)
Y = Y coordinate (vertical)

L = Left
T = Top
R = Right
B = Bottom
W = Width
H = Height

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
Contains, operator&, operator|
Aligned, Centered, Flipped, Insetted, Offsetted, With
*/

// =================================================================================================
// MARK: - Definitions of these must be provided for each cartesian API.

template <class Type_p> struct Traits;

#define ZMACRO_Cartesian(T) \
	template <> struct Traits<T> \
		{ \
		typedef T Ord_t; \
		typedef const Ord_t& XC_t; \
		inline static XC_t sX(const T& iT) { return iT; } \
		typedef Ord_t& X_t; \
		inline static X_t sX(T& ioT) { return ioT; } \
		typedef const Ord_t& YC_t; \
		inline static YC_t sY(const T& iT) { return iT; } \
		typedef Ord_t& Y_t; \
		inline static Y_t sY(T& ioT) { return ioT; } \
		};

ZMACRO_Cartesian(char)
ZMACRO_Cartesian(signed char)
ZMACRO_Cartesian(short)
ZMACRO_Cartesian(int)
ZMACRO_Cartesian(long)
ZMACRO_Cartesian(int64)
ZMACRO_Cartesian(float)
ZMACRO_Cartesian(double)
ZMACRO_Cartesian(long double)

#undef ZMACRO_Cartesian

// =================================================================================================
// MARK: - Basic Accessors

template <class Type_p>
typename Traits<Type_p>::XC_t X(const Type_p& iT) { return Traits<Type_p>::sX(iT); }
template <class Type_p>
typename Traits<Type_p>::X_t X(Type_p& iT) { return Traits<Type_p>::sX(iT); }


template <class Type_p>
typename Traits<Type_p>::YC_t Y(const Type_p& iT) { return Traits<Type_p>::sY(iT); }
template <class Type_p>
typename Traits<Type_p>::Y_t Y(Type_p& iT) { return Traits<Type_p>::sY(iT); }


template <class Type_p>
typename Traits<Type_p>::LC_t L(const Type_p& iT) { return Traits<Type_p>::sL(iT); }
template <class Type_p>
typename Traits<Type_p>::L_t L(Type_p& iT) { return Traits<Type_p>::sL(iT); }


template <class Type_p>
typename Traits<Type_p>::TC_t T(const Type_p& iT) { return Traits<Type_p>::sT(iT); }
template <class Type_p>
typename Traits<Type_p>::T_t T(Type_p& iT) { return Traits<Type_p>::sT(iT); }


template <class Type_p>
typename Traits<Type_p>::RC_t R(const Type_p& iT) { return Traits<Type_p>::sR(iT); }
template <class Type_p>
typename Traits<Type_p>::R_t R(Type_p& iT) { return Traits<Type_p>::sR(iT); }


template <class Type_p>
typename Traits<Type_p>::BC_t B(const Type_p& iT) { return Traits<Type_p>::sB(iT); }
template <class Type_p>
typename Traits<Type_p>::B_t B(Type_p& iT) { return Traits<Type_p>::sB(iT); }


template <class Type_p>
typename Traits<Type_p>::WC_t W(const Type_p& iT) { return Traits<Type_p>::sW(iT); }
template <class Type_p>
typename Traits<Type_p>::W_t W(Type_p& iT) { return Traits<Type_p>::sW(iT); }


template <class Type_p>
typename Traits<Type_p>::HC_t H(const Type_p& iT) { return Traits<Type_p>::sH(iT); }
template <class Type_p>
typename Traits<Type_p>::H_t H(Type_p& iT) { return Traits<Type_p>::sH(iT); }


template <class Type_p>
typename Traits<Type_p>::WHC_t WH(const Type_p& iT) { return Traits<Type_p>::sWH(iT); }
template <class Type_p>
typename Traits<Type_p>::WH_t WH(Type_p& iT) { return Traits<Type_p>::sWH(iT); }


template <class Type_p>
typename Traits<Type_p>::LTC_t LT(const Type_p& iT) { return Traits<Type_p>::sLT(iT); }
template <class Type_p>
typename Traits<Type_p>::LT_t LT(Type_p& iT) { return Traits<Type_p>::sLT(iT); }


template <class Type_p>
typename Traits<Type_p>::RBC_t RB(const Type_p& iT) { return Traits<Type_p>::sRB(iT); }
template <class Type_p>
typename Traits<Type_p>::RB_t RB(Type_p& iT) { return Traits<Type_p>::sRB(iT); }


template <class Type_p>
typename Traits<Type_p>::LBC_t LB(const Type_p& iT) { return Traits<Type_p>::sLB(iT); }
template <class Type_p>
typename Traits<Type_p>::LB_t LB(Type_p& iT) { return Traits<Type_p>::sLB(iT); }


template <class Type_p>
typename Traits<Type_p>::RTC_t RT(const Type_p& iT) { return Traits<Type_p>::sRT(iT); }
template <class Type_p>
typename Traits<Type_p>::RT_t RT(Type_p& iT) { return Traits<Type_p>::sRT(iT); }

// =================================================================================================
// MARK: - Fundamental Pseudo-ctors.

template <class Point_p>
Point_p sPoint() { return Traits<Point_p>::sMake(0, 0); }

template <class Rect_p>
Rect_p sRect
	(const typename Traits<Rect_p>::Ord_t& iL,
	const typename Traits<Rect_p>::Ord_t& iT,
	const typename Traits<Rect_p>::Ord_t& iR,
	const typename Traits<Rect_p>::Ord_t& iB)
	{ return Traits<Rect_p>::sMake(iL, iT, iR, iB); }

// =================================================================================================
// MARK: - Derived accessors

template <class Rect_p>
typename Traits<Rect_p>::Ord_t CX(const Rect_p& iRect)
	{ return (L(iRect) + R(iRect)) / 2; }

template <class Rect_p>
typename Traits<Rect_p>::Ord_t CY(const Rect_p& iRect)
	{ return (T(iRect) + B(iRect)) / 2; }

template <class Rect_p>
typename Traits<Rect_p>::Point_t C(const Rect_p& iRect)
	{ return sPoint<typename Traits<Rect_p>::Point_t>(CX(iRect), CY(iRect)); }

// =================================================================================================
// MARK: - Other Pseudo-ctors.

template <class Point_p>
Point_p sPoint
	(const typename Traits<Point_p>::Ord_t& iX,
	const typename Traits<Point_p>::Ord_t& iY)
	{ return Traits<Point_p>::sMake(iX, iY); }

template <class Point_p, class Other>
Point_p sPoint(const Other& iOther) { return sPoint(X(iOther), Y(iOther)); }

template <class Rect_p>
Rect_p sRect()
	{ return sRect<Rect_p>(0, 0, 0, 0); }

template <class Rect_p, class Other>
Rect_p sRect(const Other& iOtherW, const Other& iOtherH)
	{ return sRect<Rect_p>(0, 0, iOtherW, iOtherH); }

template <class Rect_p, class Other>
Rect_p sRect(const Other& iOther)
	{ return sRect<Rect_p>(L(iOther), T(iOther), R(iOther), B(iOther)); }

// =================================================================================================
// MARK: - sContains

template <class Rect_p, class OtherX>
bool sContainsX(const Rect_p& iRect, const OtherX& iOtherX)
	{ return X(iOtherX) >= L(iRect) && X(iOtherX) < R(iRect); }

template <class Rect_p, class OtherY>
bool sContainsY(const Rect_p& iRect, const OtherY& iOtherY)
	{ return Y(iOtherY) >= T(iRect) && Y(iOtherY) < B(iRect); }

template <class Rect_p, class OtherX_p, class OtherY_p>
bool sContains(const Rect_p& iRect, const OtherX_p& iOtherX, const OtherY_p& iOtherY)
	{
	return X(iOtherX) >= L(iRect) && X(iOtherX) < R(iRect)
		&& Y(iOtherY) >= T(iRect) && Y(iOtherY) < B(iRect);
	}

template <class Rect_p, class Other>
bool sContains(const Rect_p& iRect, const Other& iOther)
	{
	return X(iOther) >= L(iRect) && X(iOther) < R(iRect)
		&& Y(iOther) >= T(iRect) && Y(iOther) < B(iRect);
	}

// =================================================================================================
// MARK: - operator&

template <class Rect_p, class Other>
Rect_p operator&(const Rect_p& iL, const Rect_p& iR)
	{
	return sRect<Rect_p>
		(std::max(L(iL),L(iR)), std::max(T(iL),T(iR)),
		std::min(R(iL),R(iR)), std::min(B(iL),B(iR)));
	}

template <class Rect_p, class Other>
Rect_p& operator&=(Rect_p& ioL, const Rect_p& iR)
	{
	L(ioL) = std::max(L(ioL), L(iR));
	T(ioL) = std::max(T(ioL), T(iR));
	R(ioL) = std::min(R(ioL), R(iR));
	B(ioL) = std::min(B(ioL), B(iR));
	return ioL;
	}

// =================================================================================================
// MARK: - operator&

template <class Rect_p, class Other>
Rect_p operator|(const Rect_p& iL, const Rect_p& iR)
	{
	return sRect<Rect_p>
		(std::min(L(iL),L(iR)), std::min(T(iL),T(iR)),
		std::max(R(iL),R(iR)), std::max(B(iL),B(iR)));
	}

template <class Rect_p, class Other>
Rect_p& operator|=(Rect_p& ioL, const Rect_p& iR)
	{
	L(ioL) = std::min(L(ioL), L(iR));
	T(ioL) = std::min(T(ioL), T(iR));
	R(ioL) = std::max(R(ioL), R(iR));
	B(ioL) = std::max(B(ioL), B(iR));
	return ioL;
	}

// =================================================================================================
// MARK: - sAligned

template <class Rect_p, class OtherX>
Rect_p sAlignedL(const OtherX& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - L(iRect), 0, iRect); }

template <class Rect_p, class OtherY>
Rect_p sAlignedT(const OtherY& iOther, const Rect_p& iRect)
	{ return sOffsetted(0, Y(iOther) - T(iRect), iRect); }

template <class Rect_p, class OtherX>
Rect_p sAlignedR(const OtherX& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - R(iRect), 0, iRect); }

template <class Rect_p, class OtherY>
Rect_p sAlignedB(const OtherY& iOther, const Rect_p& iRect)
	{ return sOffsetted(0, Y(iOther) - B(iRect), iRect); }

// =================================================================================================
// MARK: - sAligned

template <class Rect_p, class Other_p>
Rect_p sAlignedLT(const Other_p& iOther, const Rect_p& iRect)
	{ return sAlignedL(iOther, sAlignedT(iOther, iRect)); }

template <class Rect_p, class Other_p>
Rect_p sAlignedRB(const Other_p& iOther, const Rect_p& iRect)
	{ return sAlignedR(iOther, sAlignedB(iOther, iRect)); }

template <class Rect_p, class Other_p>
Rect_p sAlignedLB(const Other_p& iOther, const Rect_p& iRect)
	{ return sAlignedL(iOther, sAlignedB(iOther, iRect)); }

template <class Rect_p, class Other_p>
Rect_p sAlignedRT(const Other_p& iOther, const Rect_p& iRect)
	{ return sAlignedR(iOther, sAlignedT(iOther, iRect)); }

// =================================================================================================
// MARK: - sAligned

template <class Rect_p, class OtherX, class OtherY>
Rect_p sAlignedLT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sAlignedL(iX, sAlignedT(iY, iRect)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sAlignedRB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sAlignedR(iX, sAlignedB(iY, iRect)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sAlignedLB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sAlignedL(iX, sAlignedB(iY, iRect)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sAlignedRT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sAlignedR(iX, sAlignedY(iY, iRect)); }

// =================================================================================================
// MARK: - sCentered

template <class Rect_p, class OtherX>
Rect_p sCenteredX(const OtherX& iOtherX, const Rect_p& iRect)
	{ return sOffsetted(X(iOtherX) - CX(iRect), 0, iRect); }

template <class Rect_p, class OtherY>
Rect_p sCenteredY(const OtherY& iOtherY, const Rect_p& iRect)
	{ return sOffsetted(0, Y(iOtherY) - CY(iRect), iRect); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sCentered(const OtherX& iOtherX, const OtherY& iOtherY, const Rect_p& iRect)
	{ return sOffsetted(X(iOtherX) - CX(iRect), Y(iOtherY) - CY(iRect), iRect); }

template <class Rect_p, class Other>
Rect_p sCentered(const Other& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - CX(iRect), Y(iOther) - CY(iRect), iRect); }

// =================================================================================================
// MARK: - sFlippedY

template <class Rect_p, class Other_p>
Rect_p sFlippedY(const Other_p& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), Y(iY) - T(iRect), R(iRect), Y(iY) - B(iRect)); }

// =================================================================================================
// MARK: - sInsetted

template <class Rect_p, class OtherX>
Rect_p sInsettedX(const OtherX& iOtherX, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect) + X(iOtherX), T(iRect), R(iRect) - X(iOtherX), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sInsettedY(const OtherY& iOtherY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iOtherY), R(iRect), B(iRect) - Y(iOtherY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sInsetted(const OtherX& iOtherX, const OtherY& iOtherY, const Rect_p& iRect)
	{
	return sRect<Rect_p>
		(L(iRect) + X(iOtherX), T(iRect) + Y(iOtherY),
		R(iRect) - X(iOtherX), B(iRect) - Y(iOtherY));
	}

template <class Rect_p, class Other>
Rect_p sInsetted(const Other& iOther, const Rect_p& iRect)
	{
	return sRect<Rect_p>
		(L(iRect) + X(iOther), T(iRect) + Y(iOther), R(iRect) - X(iOther), B(iRect) - Y(iOther));
	}

// =================================================================================================
// MARK: - sOffsetted

template <class Rect_p, class OtherX>
Rect_p sOffsettedX(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect) + X(iOther), T(iRect), R(iRect) + X(iOther), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sOffsettedY(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iOther), R(iRect), B(iRect) + Y(iOther)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sOffsetted(const OtherX& iOtherX, const OtherY& iOtherY, const Rect_p& iRect)
	{
	return sRect<Rect_p>
		(L(iRect) + X(iOtherX), T(iRect) + Y(iOtherY),
		R(iRect) + X(iOtherX), B(iRect) + Y(iOtherY)); }

template <class Rect_p, class Other>
Rect_p sOffsetted(const Other& iOther, const Rect_p& iRect)
	{
	return sRect<Rect_p>
		(L(iRect) + X(iOther), T(iRect) + Y(iOther), R(iRect) + X(iOther), B(iRect) + Y(iOther));
	}

// =================================================================================================
// MARK: - sOffsetted

template <class Rect_p, class OtherX>
Rect_p sOffsettedL(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect) + X(iOther), T(iRect), R(iRect), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sOffsettedT(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect) + Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX>
Rect_p sOffsettedR(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect) + X(iOther), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sOffsettedB(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect), B(iRect) + Y(iOther)); }

// =================================================================================================
// MARK: - sWith

template <class Rect_p, class OtherX>
Rect_p sWithL(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iOther), T(iRect), R(iRect), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sWithT(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX>
Rect_p sWithR(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), X(iOther), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sWithB(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect), Y(iOther)); }

// =================================================================================================
// MARK: - sWith

template <class Rect_p, class Other_p>
Rect_p sWithLT(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iOther), Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sWithRB(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), X(iOther), Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithLB(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iOther), T(iRect), R(iRect), Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithRT(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iOther), Y(iOther), X(iOther), B(iRect)); }

// =================================================================================================
// MARK: - sWith

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithLT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iX), Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithRB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), X(iX), Y(iY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithLB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iX), T(iRect), R(iRect), Y(iY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithRT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), Y(iY), X(iX), B(iRect)); }

template <class Rect_p, class OtherL, class OtherR>
Rect_p sWithLR(const OtherL& iL, const OtherR& iR, const Rect_p& iRect)
	{ return sRect<Rect_p>(X(iL), T(iRect), X(iR), B(iRect)); }

template <class Rect_p, class OtherT, class OtherB>
Rect_p sWithTB(const OtherT& iT, const OtherB& iB, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), Y(iT), R(iRect), Y(iB)); }

// =================================================================================================
// MARK: - sWithW, with Width by moving L or R

template <class Rect_p, class OtherX>
Rect_p sWithWL(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iOther), T(iRect), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX>
Rect_p sWithWR(const OtherX& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), L(iRect) + X(iOther), B(iRect)); }

// =================================================================================================
// MARK: - sWithH, with Height by moving T or B

template <class Rect_p, class OtherY>
Rect_p sWithHT(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), B(iRect) - Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class OtherY>
Rect_p sWithHB(const OtherY& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), R(iRect), T(iRect) + Y(iOther)); }

// =================================================================================================
// MARK: - sWithWH, with Width and Height by moving LT, RB, LB, RT

template <class Rect_p, class Other_p>
Rect_p sWithWHLT(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iOther), B(iRect) - Y(iOther), R(iRect), B(iRect)); }

template <class Rect_p, class Other_p>
Rect_p sWithWHRB(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), L(iRect) + X(iOther), B(iRect) + Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithWHLB(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iOther), T(iRect), R(iRect), B(iRect) + Y(iOther)); }

template <class Rect_p, class Other_p>
Rect_p sWithWHRT(const Other_p& iOther, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), B(iRect) - Y(iOther), L(iRect) + X(iOther), B(iRect)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithWHLT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iX), B(iRect) - Y(iY), R(iRect), B(iRect)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithWHRB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), T(iRect), L(iRect) + X(iX), B(iRect) + Y(iY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithWHLB(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(R(iRect) - X(iX), T(iRect), R(iRect), B(iRect) + Y(iY)); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sWithWHRT(const OtherX& iX, const OtherY& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), B(iRect) - Y(iY), L(iRect) + X(iX), B(iRect)); }

} // namespace ZCartesian
} // namespace ZooLib

#endif // __ZCartesian_h__
