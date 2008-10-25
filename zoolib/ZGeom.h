/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZGeom__
#define __ZGeom__
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZTypes.h" // For ZRectPOD and ZPointPOD

#if ZCONFIG_SPI_Enabled(CoreGraphics)
#	include <ApplicationServices/ApplicationServices.h>
#endif

#if ZCONFIG_SPI_Enabled(QuickDraw)
#	include ZMACINCLUDE(ApplicationServices,MacTypes.h)
#	include ZMACINCLUDE(ApplicationServices,QuickDraw.h)
#endif

#if ZCONFIG_SPI_Enabled(GDI)
#	include "zoolib/ZWinHeader.h"
#endif

#if ZCONFIG_SPI_Enabled(X11)
#	include "zoolib/ZCompat_Xlib.h"
#	include <X11/Xutil.h>
#endif

#if ZCONFIG_SPI_Enabled(BeOS)
#	include <interface/Point.h>
#	include <interface/Rect.h>
#endif

// Include these after the platform files -- cmath causes problems
// for Mac headers ('relation' is undefined) when building with CW.
#include "zoolib/ZCompat_algorithm.h" // For min and max
#include "zoolib/ZCompat_cmath.h" // For sqrt
#include "zoolib/ZCompat_operator_bool.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZCoord

typedef int32 ZCoord;

// =================================================================================================
#pragma mark -
#pragma mark * ZPoint_T

template <class T>
class ZPoint_T
	{
public:
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZPoint_T<T>,
    	operator_bool_generator_type, operator_bool_type);
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(h || v); }

	static const ZPoint_T sZero;

	T h;
	T v;

	ZPoint_T() {}

	ZPoint_T(T iVal) : h(iVal), v(iVal) {}

	ZPoint_T(T iH, T iV) : h(iH), v(iV) {}

	template <class S> ZPoint_T(const ZPoint_T<S>& other)
	:	h(T(other.h)), v(T(other.v))
		{}

	template <class S> ZPoint_T& operator=(const ZPoint_T<S>& other)
		{ h = T(other.h); v = T(other.v); return *this; }

	ZPoint_T(const ZPointPOD& iPoint)
	:	h(T(iPoint.h)),
		v(T(iPoint.v))
		{}

	ZPoint_T& operator=(const ZPointPOD& other)
		{ h = T(other.h); v = T(other.v); return *this; }

	operator ZPointPOD() const;

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZPoint_T(const CGPoint& pt)
		:	h(T(pt.x)),
			v(T(pt.y))
			{}
		operator CGPoint() const;

		ZPoint_T(const CGSize& iOther)
		:	h(T(iOther.width)),
			v(T(iOther.height))
			{}
		operator CGSize() const;
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZPoint_T(const Point& pt)
		:	h(T(pt.h)),
			v(T(pt.v))
			{}
		operator Point() const;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZPoint_T(const POINT& pt)
		:	h(T(pt.x)),
			v(T(pt.y))
			{}
		operator POINT() const;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZPoint_T(const XPoint& pt)
		:	h(T(pt.x)),
			v(T(pt.y))
			{}
		operator XPoint() const;
	#endif

	#if ZCONFIG_SPI_Enabled(BeOS)
		ZPoint_T(const BPoint& pt)
		:	h(T(pt.x)),
			v(T(pt.y))
			{}
		operator BPoint() const;
	#endif

// Arithmetic operators
	ZPoint_T operator+(const ZPoint_T& pt) const
		{ return ZPoint_T(T(h + pt.h), T(v + pt.v)); }
	ZPoint_T& operator+=(const ZPoint_T& pt)
		{
		h += pt.h;
		v += pt.v;
		return *this;
		}

	ZPoint_T operator-(const ZPoint_T& pt) const
		{ return ZPoint_T(T(h - pt.h), T(v - pt.v)); }
	ZPoint_T& operator-=(const ZPoint_T& pt)
		{
		h -= pt.h;
		v -= pt.v;
		return *this;
		}

	ZPoint_T operator-() const
		{ return ZPoint_T(T(-h), T(-v)); }

	ZPoint_T operator*(int iVal) const
		{ return ZPoint_T(T(h * iVal), T(v * iVal)); }
	ZPoint_T& operator*=(int iVal)
		{
		h = T(h * iVal);
		v = T(v * iVal);
		return *this;
		}

	ZPoint_T operator*(double iVal) const
		{ return ZPoint_T(T(h * iVal), T(v * iVal)); }
	ZPoint_T& operator*=(double iVal)
		{
		h = T(h * iVal);
		v = T(v * iVal);
		return *this;
		}

	ZPoint_T operator*(const ZPoint_T<T>& other) const
		{ return ZPoint_T(T(h * other.h), T(v * other.v)); }
	ZPoint_T& operator*=(const ZPoint_T<T>& other)
		{
		h *= other.h;
		v *= other.v;
		return *this;
		}

	ZPoint_T operator/(int iVal) const
		{ return ZPoint_T(T(h / iVal), T(v / iVal)); }
	ZPoint_T& operator/=(int iVal)
		{
		h = T(h / iVal);
		v = T(v / iVal);
		return *this;
		}

	ZPoint_T operator/(double iVal) const
		{ return ZPoint_T(T(h / iVal), T(v / iVal)); }
	ZPoint_T& operator/=(double iVal)
		{
		h = T(h / iVal);
		v = T(v / iVal);
		return *this;
		}

	ZPoint_T operator/(const ZPoint_T<T>& other) const
		{ return ZPoint_T(T(h / other.h), T(v / other.v)); }
	ZPoint_T& operator/=(const ZPoint_T<T>& other)
		{
		h /= other.h;
		v /= other.v;
		return *this;
		}

// Relational operators
	bool operator!=(const ZPoint_T& pt) const { return v != pt.v || h != pt.h; }
	bool operator==(const ZPoint_T& pt) const	{ return v == pt.v && h == pt.h; }
	bool operator>(const ZPoint_T& pt) const { return v > pt.v && h > pt.h; }
	bool operator<(const ZPoint_T& pt) const { return v < pt.v && h < pt.h; }
	bool operator>=(const ZPoint_T& pt) const { return v >= pt.v && h >= pt.h; }
	bool operator<=(const ZPoint_T& pt) const { return v <= pt.v && h <= pt.h; }

// Odds
	ZPoint_T Flipped() const
		{ return ZPoint_T(v,h); }

	T DistanceSquaredTo(const ZPoint_T& other) const
		{ return (v - other.v) * (v - other.v) + (h - other.h) * (h - other.h); }

	T DistanceTo(const ZPoint_T& other) const
		{ return T(sqrt(double(this->DistanceSquaredTo(other)))); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRect_T

template <class T>
class ZRect_T
	{
public:
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZRect_T<T>,
    	operator_bool_generator_type, operator_bool_type);
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(left < right && top < bottom); }

	static const ZRect_T sZero;

	T left;
	T top;
	T right;
	T bottom;

	ZRect_T() {}

	ZRect_T(T hSize, T vSize)
	:	left(T(0)),
		top(T(0)),
		right(hSize),
		bottom(vSize)
		{}

	ZRect_T(T iLeft, T iTop, T iRight, T iBottom)
	:	left(iLeft),
		top(iTop),
		right(iRight),
		bottom(iBottom)
		{}

	template <class S>
	ZRect_T(const ZPoint_T<S>& iSize)
	:	left(T(0)),
		top(T(0)),
		right(T(iSize.h)),
		bottom(T(iSize.v))
		{}

	template <class S>
	ZRect_T(const ZRect_T<S>& iRect)
	:	left(T(iRect.left)),
		top(T(iRect.top)),
		right(T(iRect.right)),
		bottom(T(iRect.bottom))
		{}

	template <class S>
	ZRect_T(const ZPoint_T<S>& topLeftPt, const ZPoint_T<S>& botRightPt)
	:	left(T(topLeftPt.h)),
		top(T(topLeftPt.v)),
		right(T(botRightPt.h)),
		bottom(T(botRightPt.v))
		{}

	template <class S>
	ZRect_T& operator=(const ZPoint_T<S>& iSize)
		{
		left = T(0);
		top = T(0);
		right = T(iSize.h);
		bottom = T(iSize.v);
		return *this;
		}

	template <class S>
	ZRect_T& operator=(const ZRect_T<S>& other)
		{
		left = T(other.left);
		top = T(other.top);
		right = T(other.right);
		bottom = T(other.bottom);
		return *this;
		}

	ZRect_T(const ZRectPOD& iRect)
	:	left(iRect.left),
		top(iRect.top),
		right(iRect.right),
		bottom(iRect.bottom)
		{}

	ZRect_T& operator=(const ZRectPOD& iRect)
		{
		left = T(iRect.left);
		top = T(iRect.top);
		right = T(iRect.right);
		bottom = T(iRect.bottom);
		return *this;
		}

	operator ZRectPOD() const;

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZRect_T(const CGRect& iRect)
		:	left(T(iRect.origin.x)),
			top(T(iRect.origin.y)),
			right(T(iRect.origin.x + iRect.size.width)),
			bottom(T(iRect.origin.y + iRect.size.height))
			{}
		operator CGRect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZRect_T(const Rect& iRect)
		:	left(T(iRect.left)),
			top(T(iRect.top)),
			right(T(iRect.right)),
			bottom(T(iRect.bottom))
			{}
		operator Rect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZRect_T(const RECT& iRect)
		:	left(T(iRect.left)),
			top(T(iRect.top)),
			right(T(iRect.right)),
			bottom(T(iRect.bottom))
			{}
		operator RECT() const;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZRect_T(const XRectangle& iRect)
		:	left(T(iRect.x)),
			top(T(iRect.y)),
			right(T(iRect.x + iRect.width)),
			bottom(T(iRect.y + iRect.height))
			{}
		operator XRectangle() const;
	#endif

	#if ZCONFIG_SPI_Enabled(Be)
		ZRect_T(const BRect& iRect)
		:	left(T(iRect.left)),
			top(T(iRect.top)),
			right(T(iRect.right + 1)),
			bottom(T(iRect.bottom + 1))
			{}
		operator BRect() const;
	#endif

	ZRect_T operator+(const ZRect_T& other) const
		{
		return ZRect_T(T(left + other.left), T(top + other.top),
			T(right + other.right), T(bottom + other.bottom));
		}

	ZRect_T& operator+=(const ZRect_T& other)
		{
		left += other.left;
		top += other.top;
		right += other.right;
		bottom += other.bottom;
		return *this;
		}

	ZRect_T operator-(const ZRect_T& other) const
		{
		return ZRect_T(T(left - other.left), T(top - other.top),
			T(right - other.right), T(bottom - other.bottom));
		}

	ZRect_T& operator-=(const ZRect_T& other)
		{
		left -= other.left;
		top -= other.top;
		right -= other.right;
		bottom -= other.bottom;
		return *this;
		}

	ZRect_T operator+(const ZPoint_T<T>& pt) const
		{
		return ZRect_T(T(left + pt.h), T(top + pt.v),
			T(right + pt.h), T(bottom + pt.v));
		}

	ZRect_T& operator+=(const ZPoint_T<T>& pt)
		{
		left+= pt.h;
		top += pt.v;
		right+= pt.h;
		bottom+= pt.v;
		return *this;
		}

	ZRect_T operator-(const ZPoint_T<T>& pt) const
		{
		return ZRect_T(T(left - pt.h), T(top - pt.v),
			T(right - pt.h), T(bottom - pt.v));
		}

	ZRect_T& operator-=(const ZPoint_T<T>& pt)
		{
		left -= pt.h;
		top -= pt.v;
		right -= pt.h;
		bottom -= pt.v;
		return *this;
		}

	ZRect_T operator-() const
		{ return ZRect_T(-left, -top, -right, -bottom); }

	ZRect_T operator*(int iVal) const
		{
		return ZRect_T(T(left * iVal), T(top * iVal),
			T(right * iVal), T(bottom * iVal));
		}

	ZRect_T& operator*=(int iVal)
		{
		left = T(left * iVal);
		top = T(top * iVal);
		right = T(right * iVal);
		bottom = T(bottom * iVal);
		return *this;
		}

	ZRect_T operator*(double iVal) const
		{
		return ZRect_T(T(left * iVal), T(top * iVal),
			T(right * iVal), T(bottom * iVal));
		}

	ZRect_T& operator*=(double iVal)
		{
		left = T(left * iVal);
		top = T(top * iVal);
		right = T(right * iVal);
		bottom = T(bottom * iVal);
		return *this;
		}

	ZRect_T operator*(const ZPoint_T<T>& point) const
		{
		return ZRect_T(T(left * point.h), T(top * point.v),
			T(right * point.h), T(bottom * point.v));
		}

	ZRect_T& operator*=(const ZPoint_T<T>& point)
		{
		left *= point.h;
		top *= point.v;
		right*= point.h;
		bottom *= point.v;
		return *this;
		}

	ZRect_T operator/(int iVal) const
		{
		return ZRect_T(T(left / iVal), T(top / iVal),
			T(right / iVal), T(bottom / iVal));
		}

	ZRect_T& operator/=(int iVal)
		{
		left = T(left / iVal);
		top = T(top / iVal);
		right = T(right / iVal);
		bottom = T(bottom / iVal);
		return *this;
		}

	ZRect_T operator/(double iVal) const
		{
		return ZRect_T(T(left / iVal), T(top / iVal),
		T(right / iVal), T(bottom / iVal));
		}

	ZRect_T& operator/=(double iVal)
		{
		left = T(left / iVal);
		top = T(top / iVal);
		right = T(right / iVal);
		bottom = T(bottom / iVal);
		return *this;
		}

	ZRect_T operator/(const ZPoint_T<T>& point) const
		{
		return ZRect_T(T(left / point.h), T(top / point.v),
		T(right / point.h), T(bottom / point.v));
		}

	ZRect_T& operator/=(const ZPoint_T<T>& point)
		{
		left /= point.h;
		top /= point.v;
		right/= point.h;
		bottom /= point.v;
		return *this;
		}


	ZRect_T Centered(const ZRect_T& other) const
		{
		T width = T(right - left);
		T height = T(bottom - top);
		T newLeft = T((other.right + other.left - width) / 2);
		T newTop = T((other.bottom + other.top - height) / 2);

		return ZRect_T(newLeft, newTop, T(newLeft + width), T(newTop + height));
		}

	ZRect_T CenteredH(const ZRect_T& other) const
		{
		T width = T(right - left);
		T newLeft = T((other.right + other.left - width) / 2);
		return ZRect_T(newLeft, top, T(newLeft + width), bottom);
		}

	ZRect_T CenteredV(const ZRect_T& other) const
		{
		T height = T(bottom - top);
		T newTop = T((other.bottom + other.top - height) / 2);
		return ZRect_T(left, newTop, right, newTop + height);
		}

	ZPoint_T<T> Center() const
		{ return ZPoint_T<T>(T((left + right) / 2), T((top + bottom) / 2)); }

	T CenterH() const
		{ return T((left + right) / 2); }

	T CenterV() const
		{ return T((top + bottom) / 2); }

	ZPoint_T<T> Pin(const ZPoint_T<T>& inPoint) const 
		{
		return ZPoint_T<T>(std::max(left, std::min(inPoint.h, right)),
			std::max(top, std::min(inPoint.v, bottom)));
		}

	ZRect_T Inset(const ZPoint_T<T>& inset) const
		{
		return ZRect_T(T(left + inset.h), T(top + inset.v), T(right - inset.h),
			T(bottom - inset.v));
		}

	ZRect_T Inset(T hInset, T vInset) const
		{
		return ZRect_T(T(left + hInset), T(top + vInset), T(right - hInset), T(bottom - vInset));
		}

	void MakeInset(const ZPoint_T<T>& thePoint)
		{
		left += thePoint.h;
		top += thePoint.v;
		right -= thePoint.h;
		bottom -= thePoint.v;
		}
	void MakeInset(T hInset, T vInset)
		{
		left += hInset;
		top += vInset;
		right -= hInset;
		bottom -= vInset;
		}

	bool operator==(const ZRect_T& other) const
		{
		return top == other.top && left == other.left &&
			bottom == other.bottom && right == other.right;
		}
	bool operator!=(const ZRect_T& other) const
		{
		return top != other.top || left != other.left ||
			bottom != other.bottom || right != other.right;
		}

	ZRect_T operator&(const ZRect_T& other) const
		{
		ZRect_T theRect(std::max(left, other.left), std::max(top, other.top),
			std::min(right, other.right), std::min(bottom, other.bottom));

		theRect.left = std::min(theRect.left, theRect.right);
		theRect.top = std::min(theRect.top, theRect.bottom);
		return theRect;
		}

	ZRect_T& operator&=(const ZRect_T& other)
		{
		left = std::max(left, other.left);
		top = std::max(top, other.top);
		right = std::min(right, other.right);
		bottom = std::min(bottom, other.bottom);
		left = std::min(left, right);
		top = std::min(top, bottom);
		return *this;
		}

	ZRect_T operator|(const ZRect_T& other) const
		{
		return ZRect_T(std::min(left, other.left), std::min(top, other.top),
			std::max(right, other.right), std::max(bottom, other.bottom));
		}

	ZRect_T& operator|=(const ZRect_T& other)
		{
		// If we're empty, then just copy from the other
		if (left >= right || top >= bottom)
			{
			left = other.left;
			right = other.right;
			top = other.top;
			bottom = other.bottom;
			}
		else if (other.left < other.right && other.top < other.bottom)
			{
			left = std::min(left, other.left);
			top = std::min(top, other.top);
			right = std::max(right, other.right);
			bottom = std::max(bottom, other.bottom);
			}
		return *this;
		}

	bool Contains(const ZPoint_T<T>& pt) const
		{ return pt.v >= top && pt.v < bottom && pt.h >= left && pt.h < right; }

	bool Contains(T hCoord, T vCoord) const
		{ return vCoord >= top && vCoord < bottom && hCoord >= left && hCoord < right; }

	bool Contains(const ZRect_T& other) const
		{
		return other.top >= top && other.bottom <= bottom
			&& other.left >= left && other.right <= right;
		}

	bool IsEmpty() const
		{ return right <= left || bottom <= top; }

	ZPoint_T<T> Size() const
		{ return ZPoint_T<T>(T(right - left), T(bottom - top)); }

	T Height() const
		{ return T(bottom - top); }

	T Width() const
		{ return T(right - left); }

	ZPoint_T<T> TopLeft() const { return ZPoint_T<T>(left, top); }
	ZPoint_T<T> TopRight() const { return ZPoint_T<T>(right, top); }
	ZPoint_T<T> BottomLeft() const { return ZPoint_T<T>(left, bottom); }
	ZPoint_T<T> BottomRight() const { return ZPoint_T<T>(right, bottom); }

	T Left() const { return left; }
	T Top() const { return top; }
	T Right() const { return right; }
	T Bottom() const { return bottom; }

// Odds
	ZRect_T Flipped() const
		{ return ZRect_T(top, left, bottom, right); }

	static ZRect_T sMinimalRect(const ZPoint_T<T>& point1, const ZPoint_T<T>& point2)
		{
		ZRect_T theRect;
		if (point1.h < point2.h)
			{
			theRect.left = point1.h;
			theRect.right = T(point2.h + 1);
			}
		else
			{
			theRect.left = point2.h;
			theRect.right = T(point1.h + 1);
			}
		if (point1.v < point2.v)
			{
			theRect.top = point1.v;
			theRect.bottom = T(point2.v + 1);
			}
		else
			{
			theRect.top = point2.v;
			theRect.bottom = T(point1.v + 1);
			}
		return theRect;
		} 
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZGRect_T

template <class T>
class ZGRect_T
	{
public:
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZGRect_T<T>,
    	operator_bool_generator_type, operator_bool_type);
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(extent.h != 0 || extent.v != 0); }

	ZPoint_T<T> origin;
	ZPoint_T<T> extent;

	ZGRect_T()
		{}

	ZGRect_T(const ZGRect_T<T>& iOther)
	:	origin(iOther.origin),
		extent(iOther.extent)
		{}

	ZGRect_T(T iWidth, T iHeight)
	:	origin(0, 0),
		extent(iWidth, iHeight)
		{}

	template <class U>
	ZGRect_T(const ZPoint_T<U>& iExtent)
	:	origin(0, 0),
		extent(iExtent)
		{}

	ZGRect_T(T iOriginX, T iOriginY, T iWidth, T iHeight)
	:	origin(iOriginX, iOriginY),
		extent(iWidth, iHeight)
		{}

	template <class U, class V>
	ZGRect_T(const ZPoint_T<U>& iOrigin, const ZPoint_T<V>& iExtent)
	:	origin(iOrigin),
		extent(iExtent)
		{}

	template <class U>
	ZGRect_T(T iOriginX, T iOriginY, const ZPoint_T<U>& iExtent)
	:	origin(iOriginX, iOriginY),
		extent(iExtent)
		{}

	template <class U>
	ZGRect_T(const ZPoint_T<U>& iOrigin, T iWidth, T iHeight)
	:	origin(iOrigin),
		extent(iWidth, iHeight)
		{}

	template <class U>
	ZGRect_T& operator=(const ZPoint_T<U>& iExtent)
		{
		origin.h = T(0);
		origin.v = T(0);
		extent = iExtent;
		return *this;
		}

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGRect_T(const CGSize& iSize)
		:	origin(0),
			extent(iSize)
			{}

		ZGRect_T(const CGRect& iRect)
		:	origin(iRect.origin),
			extent(iRect.size)
			{}
		operator CGRect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZGRect_T(const Rect& iRect)
		:	origin(T(iRect.left), T(iRect.top)),
			extent(T(iRect.right - iRect.left), T(iRect.bottom - iRect.top))
			{}
		operator Rect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZGRect_T(const RECT& iRect)
		:	origin(T(iRect.left), T(iRect.top)),
			extent(T(iRect.right - iRect.left), T(iRect.bottom - iRect.top))
			{}
		operator RECT() const;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZGRect_T(const XRectangle& iRect)
		:	origin(iRect.x, iRect.y),
			extent(iRect.width, iRect.height)
			{}
		operator XRectangle() const;
	#endif

	bool operator==(const ZGRect_T& other) const
		{ return origin == other.origin && extent == other.extent; }

	bool operator!=(const ZGRect_T& other) const
		{ return origin != other.origin || extent != other.extent; }

	template <class U>
	ZGRect_T operator+(const ZPoint_T<U>& iOffset) const
		{ return ZGRect_T(origin + iOffset, extent); }

	template <class U>
	ZGRect_T& operator+=(const ZPoint_T<U>& iOffset)
		{
		origin += iOffset;
		return *this;
		}

	template <class U>
	ZGRect_T operator-(const ZPoint_T<U>& iOffset) const
		{ return ZGRect_T(origin - iOffset, extent); }

	template <class U>
	ZGRect_T& operator-=(const ZPoint_T<U>& iOffset)
		{
		origin -= iOffset;
		return *this;
		}

	T CenterH() const
		{ return origin.h + extent.h / 2; }

	T CenterV() const
		{ return origin.v + extent.v / 2; }

	ZPoint_T<T> Center() const
		{ return ZPoint_T<T>(this->CenterH(), this->CenterV()); }

	template <class U>
	bool ContainsH(U iCoord) const
		{
		if (extent.h > 0)
			{
			return iCoord >= origin.h && iCoord < origin.h + extent.h;
			}
		else if (extent.h < 0)
			{
			return iCoord < origin.h && iCoord >= origin.h + extent.h;
			}
		else
			{
			return false;
			}
		}

	template <class U>
	bool ContainsV(U iCoord) const
		{
		if (extent.v > 0)
			{
			return iCoord >= origin.v && iCoord < origin.v + extent.v;
			}
		else if (extent.h < 0)
			{
			return iCoord < origin.v && iCoord >= origin.v + extent.v;
			}
		else
			{
			return false;
			}
		}

	template <class U>
	bool Contains(const ZPoint_T<U>& pt) const
		{ return this->ContainsH(pt.h) && this->ContainsV(pt.v); }

	template <class U>
	bool Contains(U h, U v) const
		{ return this->ContainsH(h) && this->ContainsV(v); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZPoint and ZRect

namespace ZooLib {

typedef ZPoint_T<ZCoord> ZPoint;
typedef ZRect_T<ZCoord> ZRect;

typedef ZPoint_T<float> ZPointf;
typedef ZRect_T<float> ZRectf;

typedef ZGRect_T<float> ZGRectf;

} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * Conversion operators

template <class T>
inline ZPoint_T<T>::operator ZPointPOD() const
	{
	ZPointPOD thePointPOD;
	thePointPOD.h = int32(h);
	thePointPOD.v = int32(v);
	return thePointPOD;
	}

template <class T>
inline ZRect_T<T>::operator ZRectPOD() const
	{
	ZRectPOD theRectPOD;
	theRectPOD.left = int32(left);
	theRectPOD.top = int32(top);
	theRectPOD.right = int32(right);
	theRectPOD.bottom = int32(bottom);
	return theRectPOD;
	}

#if ZCONFIG_SPI_Enabled(CoreGraphics)
	template <class T>
	inline ZPoint_T<T>::operator CGPoint() const
		{ return ::CGPointMake(h, v); }

	template <class T>
	inline ZPoint_T<T>::operator CGSize() const
		{ return ::CGSizeMake(h, v); }

	template <class T>
	inline ZRect_T<T>::operator CGRect() const
		{ return CGRectMake(left, top, right - left, bottom - top); }

	template <class T>
	inline ZGRect_T<T>::operator CGRect() const
		{ return CGRectMake(origin.h, origin.v, extent.h, extent.v); }
#endif

#if ZCONFIG_SPI_Enabled(QuickDraw)
	template <class T>
	inline ZPoint_T<T>::operator Point() const
		{
		Point thePoint;
		thePoint.h = h;
		thePoint.v = v;
		return thePoint;
		}

	template <class T>
	inline ZRect_T<T>::operator Rect() const
		{
		Rect theRect;
		theRect.left = left;
		theRect.top = top;
		theRect.right = right;
		theRect.bottom = bottom;
		return theRect;
		}

	template <class T>
	inline ZGRect_T<T>::operator Rect() const
		{
		Rect theRect;

		if (extent.h >= 0)
			{
			theRect.left = origin.h;
			theRect.right = origin.h + extent.h;
			}
		else
			{
			theRect.left = origin.h + extent.h;
			theRect.right = origin.h;
			}

		if (extent.v >= 0)
			{
			theRect.top = origin.v;
			theRect.bottom = origin.v + extent.v;
			}
		else
			{
			theRect.top = origin.v + extent.v;
			theRect.bottom = origin.v;
			}

		return theRect;
		}
#endif

#if ZCONFIG_SPI_Enabled(GDI)
	template <class T>
	inline ZPoint_T<T>::operator POINT() const
		{
		POINT thePOINT;
		thePOINT.x = h;
		thePOINT.y = v;
		return thePOINT;
		}

	template <class T>
	inline ZRect_T<T>::operator RECT() const
		{
		RECT theRECT;
		theRECT.left = left;
		theRECT.top = top;
		theRECT.right = right;
		theRECT.bottom = bottom;
		return theRECT;
		}

	template <class T>
	inline ZGRect_T<T>::operator RECT() const
		{
		RECT theRECT;

		if (extent.h >= 0)
			{
			theRECT.left = origin.h;
			theRECT.right = origin.h + extent.h;
			}
		else
			{
			theRECT.left = origin.h + extent.h;
			theRECT.right = origin.h;
			}

		if (extent.v >= 0)
			{
			theRECT.top = origin.v;
			theRECT.bottom = origin.v + extent.v;
			}
		else
			{
			theRECT.top = origin.v + extent.v;
			theRECT.bottom = origin.v;
			}

		return theRECT;
		}
#endif

#if ZCONFIG_SPI_Enabled(X11)
	template <class T>
	inline ZPoint_T<T>::operator XPoint() const
		{
		XPoint theXPoint;
		theXPoint.x = short(h);
		theXPoint.y = short(v);
		return theXPoint;
		}

	template <class T>
	inline ZRect_T<T>::operator XRectangle() const
		{
		XRectangle theXRectangle;
		theXRectangle.x = short(left);
		theXRectangle.y = short(top);
		theXRectangle.width = (unsigned short)(right - left);
		theXRectangle.height = (unsigned short)(bottom - top);
		return theXRectangle;
		}

	template <class T>
	inline ZGRect_T<T>::operator XRectangle() const
		{
		XRectangle theXRectangle;

		if (extent.h >= 0)
			{
			theXRectangle.x = short(origin.h);
			theXRectangle.width = unsigned short(extent.h);
			}
		else
			{
			theXRectangle.x = short(origin.h + extent.h);
			theXRectangle.width = unsigned short(-extent.h);
			}

		if (extent.v >= 0)
			{
			theXRectangle.y = short(origin.v);
			theXRectangle.height = unsigned short(extent.v);
			}
		else
			{
			theXRectangle.y = short(origin.v + extent.v);
			theXRectangle.height = unsigned short(-origin.v);
			}

		return theXRectangle;
		}
#endif

#if ZCONFIG_SPI_Enabled(BeOS)
	template <class T> inline ZPoint_T<T>::operator BPoint() const
		{ return BPoint(h, v); }

	template <class T> inline ZRect_T<T>::operator BRect() const
		{ return BRect(left, top, right - 1, bottom - 1); }
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Promote types to global namespace

#ifndef ZooLib_SuppressInjection
	using ZooLib::ZPoint;
	using ZooLib::ZRect;

	using ZooLib::ZPointf;
	using ZooLib::ZRectf;

	using ZooLib::ZGRectf;
#endif // ZooLib_SuppressInjection

#endif // __ZGeom__
