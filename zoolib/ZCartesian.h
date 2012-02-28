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

#if ZCONFIG(Compiler,GCC)
	#include <tr1/type_traits>
#endif

namespace ZooLib {
namespace ZCartesian {

// =================================================================================================
// MARK: -

#if 0 // ZCONFIG(Compiler,GCC)

template <class T, bool isArithmetic_p = std::tr1::is_arithmetic<T>::value> struct IfArithmetic;

template <class T>
struct IfArithmetic<T,true>
	{
	typedef T Result_t;
	const Result_t& operator()(const T& iT) { return iT; }
	};

template <class T>
typename IfArithmetic<T>::Result_t X(const T& iT) { return IfArithmetic<T>()(iT); }

template <class T>
typename IfArithmetic<T>::Result_t Y(const T& iT) { return IfArithmetic<T>()(iT); }

#else  // ZCONFIG(Compiler,GCC)

#define ZMACRO_Cartesian_Arithmetic(T) \
	inline const T& X(const T& iT) { return iT; } \
	inline const T& Y(const T& iT) { return iT; }

ZMACRO_Cartesian_Arithmetic(char)
ZMACRO_Cartesian_Arithmetic(signed char)
ZMACRO_Cartesian_Arithmetic(unsigned char)
ZMACRO_Cartesian_Arithmetic(short)
ZMACRO_Cartesian_Arithmetic(unsigned short)
ZMACRO_Cartesian_Arithmetic(int)
ZMACRO_Cartesian_Arithmetic(unsigned int)
ZMACRO_Cartesian_Arithmetic(long)
ZMACRO_Cartesian_Arithmetic(unsigned long)
ZMACRO_Cartesian_Arithmetic(float)
ZMACRO_Cartesian_Arithmetic(double)

#undef ZMACRO_Cartesian_Arithmetic

#endif // ZCONFIG(Compiler,GCC)

// =================================================================================================
// MARK: - Definitions of these must be provided for each cartesian API.

template <class Rect_p> struct RectTraits;
template <class Point_p> struct PointTraits;

// =================================================================================================
// MARK: - Simpler pseudo-ctors.

template <class Point_p>
Point_p sPoint() { return PointTraits<Point_p>::sMake(0, 0); }

template <class Point_p>
Point_p sPoint
	(const typename PointTraits<Point_p>::Ord_t& iX,
	const typename PointTraits<Point_p>::Ord_t& iY)
	{ return PointTraits<Point_p>::sMake(iX, iY); }

template <class Point_p, class Other>
Point_p sPoint(const Other& iOther) { return sPoint(X(iOther), Y(iOther)); }

template <class Rect_p>
Rect_p sRect
	(const typename RectTraits<Rect_p>::Ord_t& iL,
	const typename RectTraits<Rect_p>::Ord_t& iT,
	const typename RectTraits<Rect_p>::Ord_t& iR,
	const typename RectTraits<Rect_p>::Ord_t& iB)
	{ return RectTraits<Rect_p>::sMake(iL, iT, iR, iB); }

// =================================================================================================
// MARK: - Generic pseudo-ctor variants

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
bool sContainsX(const OtherX& iOtherX, const Rect_p& iRect)
	{ return X(iOtherX) >= L(iRect) && X(iOtherX) < R(iRect); }

template <class Rect_p, class OtherY>
bool sContainsY(const OtherY& iOtherY, const Rect_p& iRect)
	{ return Y(iOtherY) >= T(iRect) && Y(iOtherY) < B(iRect); }

template <class Rect_p, class OtherX_p, class OtherY_p>
bool sContains(const OtherX_p& iOtherX, const OtherY_p& iOtherY, const Rect_p& iRect)
	{ return sContainsX(iRect, iOtherX) && sContainsY(iRect, iOtherY); }

template <class Rect_p, class Other>
bool sContains(const Other& iOther, const Rect_p& iRect)
	{
	return X(iOther) >= L(iRect) && X(iOther) < R(iRect)
		&& Y(iOther) >= T(iRect) && Y(iOther) < B(iRect);
	}

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
Rect_p sOffsettedY(OtherY& iOther, const Rect_p& iRect)
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
// MARK: - sCenter

template <class Rect_p>
typename RectTraits<Rect_p>::Ord_t sCenterX(const Rect_p& iRect)
	{ return (L(iRect) + R(iRect)) / 2; }

template <class Rect_p>
typename RectTraits<Rect_p>::Ord_t sCenterY(const Rect_p& iRect)
	{ return (T(iRect) + B(iRect)) / 2; }

template <class Rect_p>
typename RectTraits<Rect_p>::Point_t sCenter(const Rect_p& iRect)
	{ return sPoint<typename RectTraits<Rect_p>::Point_t>(sCenterX(iRect), sCenterY(iRect)); }

// =================================================================================================
// MARK: - sCentered

template <class Rect_p, class OtherX>
Rect_p sCenteredX(const OtherX& iOtherX, const Rect_p& iRect)
	{ return sOffsetted(X(iOtherX) - sCenterX(iRect), 0, iRect); }

template <class Rect_p, class OtherY>
Rect_p sCenteredY(const OtherY& iOtherY, const Rect_p& iRect)
	{ return sOffsetted(0, Y(iOtherY) - sCenterY(iRect), iRect); }

template <class Rect_p, class OtherX, class OtherY>
Rect_p sCentered(const OtherX& iOtherX, const OtherY& iOtherY, const Rect_p& iRect)
	{ return sOffsetted(X(iOtherX) - sCenterX(iRect), Y(iOtherY) - sCenterY(iRect), iRect); }

template <class Rect_p, class Other>
Rect_p sCentered(const Other& iOther, const Rect_p& iRect)
	{ return sOffsetted(X(iOther) - sCenterX(iRect), Y(iOther) - sCenterY(iRect), iRect); }

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
// MARK: - sFlippedY

template <class Rect_p, class Other_p>
Rect_p sFlippedY(const Other_p& iY, const Rect_p& iRect)
	{ return sRect<Rect_p>(L(iRect), iY - T(iRect), R(iRect), iY - B(iRect)); }

} // namespace ZCartesian
} // namespace ZooLib

#endif // __ZCartesian_h__
