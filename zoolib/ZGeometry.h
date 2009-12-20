/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZGeometry__
#define __ZGeometry__
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZTypes.h" // For ZRectPOD and ZPointPOD

#if ZCONFIG_SPI_Enabled(CoreGraphics)
#	include <ApplicationServices/ApplicationServices.h>
#endif

#if ZCONFIG_SPI_Enabled(QuickDraw)
#	include ZMACINCLUDE3(CoreServices,CarbonCore,MacTypes.h)
#	include ZMACINCLUDE3(ApplicationServices,QD,QuickDraw.h)
#endif

#if ZCONFIG_SPI_Enabled(GDI)
#	include "zoolib/ZCompat_Win.h"
#endif

#if ZCONFIG_SPI_Enabled(X11)
#	include "zoolib/ZCompat_Xlib.h"
#	include <X11/Xutil.h>
#endif

#if ZCONFIG_SPI_Enabled(Cocoa)
#	if defined(__OBJC__)
#		import <Foundation/NSGeometry.h>
#	else
	typedef struct _NSPoint { float x; float y; } NSPoint;
	typedef struct _NSSize { float width; float height; } NSSize;
	typedef struct _NSRect { NSPoint origin; NSSize size; } NSRect;
#	endif
#endif

// Include these after the platform files -- cmath causes problems
// for Mac headers ('relation' is undefined) when building with CW.
#include "zoolib/ZCompat_algorithm.h" // For min and max
//#include "zoolib/ZCompat_cmath.h" // For sqrt
#include "zoolib/ZCompat_operator_bool.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZGPoint_T declaration

template <class T>
class ZGPoint_T
	{
public:
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZGPoint_T<T>,
    	operator_bool_generator_type, operator_bool_type);
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(x || y); }

	T x;
	T y;

	ZGPoint_T();

	ZGPoint_T(T iVal);

	ZGPoint_T(T iX, T iY);

	template <class S>
	ZGPoint_T(const ZGPoint_T<S>& other);

	template <class S>
	ZGPoint_T& operator=(const ZGPoint_T<S>& other);

	ZGPoint_T(const ZPointPOD& iPoint);

	ZGPoint_T& operator=(const ZPointPOD& other);

	operator ZPointPOD() const;

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGPoint_T(const CGPoint& other);
		operator CGPoint() const;
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa)
		ZGPoint_T(const NSPoint& other);
		operator NSPoint() const;
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZGPoint_T(const Point& other);
		operator Point() const;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZGPoint_T(const POINT& other);
		operator POINT() const;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZGPoint_T(const XPoint& other);
		operator XPoint() const;
	#endif

// Relational operators
	bool operator!=(const ZGPoint_T& other) const { return x != other.x || y != other.y; }
	bool operator==(const ZGPoint_T& other) const { return x == other.x && y == other.y; }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZGExtentPOD_T declaration

template <class T>
class ZGExtentPOD_T
	{
public:
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZGExtentPOD_T<T>,
    	operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(h || v); }

	union { T h; T width; };
	union { T v; T height; };

	template <class S>
	ZGExtentPOD_T& operator=(const ZGExtentPOD_T<S>& other);

	ZGExtentPOD_T& operator=(const ZPointPOD& other);

	operator ZPointPOD() const;

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		operator CGSize() const;
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa)
		operator NSSize() const;
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		operator Point() const;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		operator POINT() const;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		operator XPoint() const;
	#endif

// Relational operators
	bool operator!=(const ZGExtentPOD_T& other) const { return h != other.h || v != other.v; }
	bool operator==(const ZGExtentPOD_T& other) const { return h == other.h && v == other.v; }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZGExtent_T declaration

template <class T>
class ZGExtent_T : public ZGExtentPOD_T<T>
	{
public:
	ZGExtent_T();

	ZGExtent_T(T iVal);

	ZGExtent_T(T iH, T iV);

	template <class S>
	ZGExtent_T(const ZGExtentPOD_T<S>& other);

	ZGExtent_T(const ZPointPOD& iPoint);

	operator ZPointPOD() const;

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGExtent_T(const CGSize& other);
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa)
		ZGExtent_T(const NSSize& size);
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZGExtent_T(const Point& other);
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZGExtent_T(const POINT& other);
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZGExtent_T(const XPoint& other);
	#endif
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZGRect_T declaration

template <class T>
class ZGRect_T
	{
public:
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZGRect_T<T>,
    	operator_bool_generator_type, operator_bool_type);
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(extent); }

	ZGPoint_T<T> origin;
	union { ZGExtentPOD_T<T> extent; ZGExtentPOD_T<T> size; };

	ZGRect_T();

	ZGRect_T(const ZGRect_T<T>& iOther);

	ZGRect_T(T iWidth, T iHeight);

	template <class U>
	ZGRect_T(const ZGExtentPOD_T<U>& iExtent);

	ZGRect_T(T iOriginX, T iOriginY, T iWidth, T iHeight);

	template <class U, class V>
	ZGRect_T(const ZGPoint_T<U>& iOrigin, const ZGExtent_T<V>& iExtent);

	template <class U>
	ZGRect_T(T iOriginX, T iOriginY, const ZGExtent_T<U>& iExtent);

	template <class U>
	ZGRect_T(const ZGPoint_T<U>& iOrigin, T iWidth, T iHeight);

	template <class U>
	ZGRect_T& operator=(const ZGExtent_T<U>& iiExtent);

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGRect_T(const CGSize& iSize);
		ZGRect_T(const CGRect& iRect);
		operator CGRect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa)
		ZGRect_T(const NSSize& iSize);
		ZGRect_T(const NSRect& iRect);
		operator NSRect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZGRect_T(const Rect& iRect);
		operator Rect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZGRect_T(const RECT& iRect);
		operator RECT() const;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZGRect_T(const XRectangle& iRect);
		operator XRectangle() const;
	#endif

	bool operator==(const ZGRect_T& other) const
		{ return origin == other.origin && extent == other.extent; }

	bool operator!=(const ZGRect_T& other) const
		{ return origin != other.origin || extent != other.extent; }

	T Width() const;
	T Height() const;

	ZGRect_T FlippedY(T iHeight) const;

	T MinX() const;
	T MaxX() const;

	T MinY() const;
	T MaxY() const;

	T CenterX() const;
	T CenterY() const;
	ZGPoint_T<T> Center() const;

	template <class U>
	bool ContainsX(U iCoord) const;

	template <class U>
	bool ContainsY(U iCoord) const;

	template <class U>
	bool Contains(const ZGPoint_T<U>& pt) const;

	template <class U>
	bool Contains(U x, U y) const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZGPoint_T generic definitions

template <class T>
ZGPoint_T<T>::ZGPoint_T()
	{}

template <class T>
ZGPoint_T<T>::ZGPoint_T(T iVal)
:	x(iVal), y(iVal)
	{}

template <class T>
ZGPoint_T<T>::ZGPoint_T(T iX, T iY)
:	x(iX), y(iY)
	{}

template <class T>
template <class S>
ZGPoint_T<T>::ZGPoint_T(const ZGPoint_T<S>& other)
:	x(T(other.x)), y(T(other.y))
	{}

template <class T>
template <class S>
ZGPoint_T<T>& ZGPoint_T<T>::operator=(const ZGPoint_T<S>& other)
	{ x = T(other.x); y = T(other.y); return *this; }

template <class T>
ZGPoint_T<T>::ZGPoint_T(const ZPointPOD& iPoint)
:	x(T(iPoint.h)),
	y(T(iPoint.v))
	{}

template <class T>
ZGPoint_T<T>& ZGPoint_T<T>::operator=(const ZPointPOD& other)
	{ x = T(other.h); y = T(other.v); return *this; }

// =================================================================================================
#pragma mark -
#pragma mark * ZGExtentPOD_T generic definitions

template <class T>
template <class S>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(const ZGExtentPOD_T<S>& other)
	{ h = T(other.h); v = T(other.v); return *this; }

template <class T>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(const ZPointPOD& other)
	{ h = T(other.h); v = T(other.v); return *this; }

// =================================================================================================
#pragma mark -
#pragma mark * ZGExtent_T generic definitions

template <class T>
ZGExtent_T<T>::ZGExtent_T()
	{}

template <class T>
ZGExtent_T<T>::ZGExtent_T(T iVal)
	{
	this->h = iVal;
	this->v = iVal;
	}

template <class T>
ZGExtent_T<T>::ZGExtent_T(T iH, T iV)
	{
	this->h = iH;
	this->v = iV;
	}

template <class T>
template <class S>
ZGExtent_T<T>::ZGExtent_T(const ZGExtentPOD_T<S>& other)
	{
	this->h = T(other.h);
	this->v = T(other.v);
	}

template <class T>
ZGExtent_T<T>::ZGExtent_T(const ZPointPOD& iPoint)
	{
	this->h = T(iPoint.h);
	this->v = T(iPoint.v);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZGRect_T generic definitions

template <class T>
ZGRect_T<T>::ZGRect_T()
	{}

template <class T>
ZGRect_T<T>::ZGRect_T(const ZGRect_T<T>& iOther)
:	origin(iOther.origin),
	extent(iOther.extent)
	{}

template <class T>
ZGRect_T<T>::ZGRect_T(T iWidth, T iHeight)
:	origin(0, 0)
	{
	extent.h = iWidth;
	extent.v = iHeight;
	}

template <class T>
template <class U>
ZGRect_T<T>::ZGRect_T(const ZGExtentPOD_T<U>& iExtent)
:	origin(0, 0)
	{
	extent.h = iExtent.h;
	extent.v = iExtent.v;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(T iOriginX, T iOriginY, T iWidth, T iHeight)
:	origin(iOriginX, iOriginY)
	{
	extent.h = iWidth;
	extent.v = iHeight;
	}

template <class T>
template <class U, class V>
ZGRect_T<T>::ZGRect_T(const ZGPoint_T<U>& iOrigin, const ZGExtent_T<V>& iExtent)
:	origin(iOrigin),
	extent(iExtent)
	{}

template <class T>
template <class U>
ZGRect_T<T>::ZGRect_T(T iOriginX, T iOriginY, const ZGExtent_T<U>& iExtent)
:	origin(iOriginX, iOriginY),
	extent(iExtent)
	{}

template <class T>
template <class U>
ZGRect_T<T>::ZGRect_T(const ZGPoint_T<U>& iOrigin, T iWidth, T iHeight)
:	origin(iOrigin),
	extent(iWidth, iHeight)
	{}

template <class T>
template <class U>
ZGRect_T<T>& ZGRect_T<T>::operator=(const ZGExtent_T<U>& iExtent)
	{
	origin.x = T(0);
	origin.y = T(0);
	extent = iExtent;
	return *this;
	}

template <class T>
T ZGRect_T<T>::Width() const
	{ return extent.h; }

template <class T>
T ZGRect_T<T>::Height() const
	{ return extent.v; }

template <class T>
ZGRect_T<T> ZGRect_T<T>::FlippedY(T iHeight) const
	{ return ZGRect_T<T>(origin.x, iHeight - origin.y - extent.v, extent); }

template <class T>
T ZGRect_T<T>::MinX() const
	{ return extent.h >= 0 ? origin.x : origin.x + extent.h; }

template <class T>
T ZGRect_T<T>::MaxX() const
	{ return extent.h < 0 ? origin.x : origin.x + extent.h; }

template <class T>
T ZGRect_T<T>::MinY() const
	{ return extent.v >= 0 ? origin.y : origin.y + extent.v; }

template <class T>
T ZGRect_T<T>::MaxY() const
	{ return extent.v < 0 ? origin.y : origin.y + extent.v; }

template <class T>
T ZGRect_T<T>::CenterX() const
	{ return origin.x + extent.h / 2; }

template <class T>
T ZGRect_T<T>::CenterY() const
	{ return origin.y + extent.v / 2; }

template <class T>
ZGPoint_T<T> ZGRect_T<T>::Center() const
	{ return ZGPoint_T<T>(this->CenterX(), this->CenterY()); }

template <class T>
template <class U>
bool ZGRect_T<T>::ContainsX(U iCoord) const
	{
	if (extent.h > 0) return iCoord >= origin.x && iCoord < origin.x + extent.h;
	else if (extent.h < 0) return iCoord < origin.x && iCoord >= origin.x + extent.h;
	else return false;
	}

template <class T>
template <class U>
bool ZGRect_T<T>::ContainsY(U iCoord) const
	{
	if (extent.v > 0) return iCoord >= origin.y && iCoord < origin.y + extent.v;
	else if (extent.h < 0) return iCoord < origin.y && iCoord >= origin.y + extent.v;
	else return false;
	}

template <class T>
template <class U>
bool ZGRect_T<T>::Contains(const ZGPoint_T<U>& pt) const
	{ return this->ContainsX(pt.x) && this->ContainsY(pt.y); }

template <class T>
template <class U>
bool ZGRect_T<T>::Contains(U x, U y) const
	{ return this->ContainsX(x) && this->ContainsY(y); }

// =================================================================================================
#pragma mark -
#pragma mark * Operations

template <class T>
ZGPoint_T<T> operator+(const ZGPoint_T<T>& l, const ZGExtent_T<T>& r)
	{ return ZGPoint_T<T>(l.x + r.h, l.y + r.v); }

template <class T>
ZGPoint_T<T>& operator+=(ZGPoint_T<T>& l, const ZGExtent_T<T>& r)
	{ l.x += r.h; l.y += r.v; return l; }


template <class T>
ZGPoint_T<T> operator-(const ZGPoint_T<T>& l, const ZGExtent_T<T>& r)
	{ return ZGPoint_T<T>(l.x - r.h, l.y - r.v); }

template <class T>
ZGExtent_T<T> operator-(const ZGPoint_T<T>& l, const ZGPoint_T<T>& r)
	{ return ZGExtent_T<T>(l.x - r.x, l.y - r.y); }

template <class T>
ZGPoint_T<T>& operator-=(ZGPoint_T<T>& l, const ZGExtent_T<T>& r)
	{ l.x -= r.h; l.y -= r.v; return l; }


template <class T>
ZGRect_T<T> operator+(const ZGRect_T<T>& l, const ZGExtent_T<T>& r)
	{ return ZGRect_T<T>(l.origin + r, l.extent); }

template <class T>
ZGRect_T<T>& operator+=(ZGRect_T<T>& l, const ZGExtent_T<T>& r)
	{ l.origin += r; return l; }


template <class T>
ZGRect_T<T> operator-(const ZGRect_T<T>& l, const ZGExtent_T<T>& r)
	{ return ZGRect_T<T>(l.origin - r, l.extent); }

template <class T>
ZGRect_T<T>& operator-=(ZGRect_T<T>& l, const ZGExtent_T<T>& r)
	{ l.origin -= r; return l; }


template <class T, class U>
ZGRect_T<T> operator&(const ZGRect_T<T>& l, const ZGRect_T<U>& r)
	{
	using std::min;
	using std::max;

	T ox = max(l.MinX(), T(r.MinX()));
	T eh = min(l.MaxX(), T(r.MaxX()));
	eh = eh > ox ? eh - ox : 0;
	
	T oy = max(l.MinY(), T(r.MinY()));
	T ev = min(l.MaxY(), T(r.MaxY()));
	ev = ev > oy ? ev - oy : 0;

	return ZGRect_T<T>(ox, oy, eh, ev);
	}

template <class T, class U>
ZGRect_T<T>& operator&=(ZGRect_T<T>& l, const ZGRect_T<U>& r)
	{
	using std::min;
	using std::max;

	T ox = max(l.MinX(), T(r.MinX()));
	T eh = min(l.MaxX(), T(r.MaxX()));
	eh = eh > ox ? eh - ox : 0;
	
	T oy = max(l.MinY(), T(r.MinY()));
	T ev = min(l.MaxY(), T(r.MaxY()));
	ev = ev > oy ? ev - oy : 0;

	l.origin.x = ox;
	l.origin.y = oy;
	l.extent.h = eh;
	l.extent.v = ev;

	return l;
	}

template <class T, class U>
ZGRect_T<T> operator|(const ZGRect_T<T>& l, const ZGRect_T<U>& r)
	{
	using std::min;
	using std::max;

	T ox = min(l.MinX(), T(r.MinX()));
	T eh = max(l.MaxX(), T(r.MaxX()));
	eh = eh > ox ? eh - ox : 0;
	
	T oy = min(l.MinY(), T(r.MinY()));
	T ev = max(l.MaxY(), T(r.MaxY()));
	ev = ev > oy ? ev - oy : 0;

	return ZGRect_T<T>(ox, oy, eh, ev);
	}

template <class T, class U>
ZGRect_T<T>& operator|=(ZGRect_T<T>& l, const ZGRect_T<U>& r)
	{
	using std::min;
	using std::max;

	T ox = min(l.MinX(), T(r.MinX()));
	T eh = max(l.MaxX(), T(r.MaxX()));
	eh = eh > ox ? eh - ox : 0;
	
	T oy = min(l.MinY(), T(r.MinY()));
	T ev = max(l.MaxY(), T(r.MaxY()));
	ev = ev > oy ? ev - oy : 0;

	l.origin.x = ox;
	l.origin.y = oy;
	l.extent.h = eh;
	l.extent.v = ev;

	return l;
	}

// =================================================================================================
#pragma mark -
#pragma mark * SPI-specific definitions

#if ZCONFIG_SPI_Enabled(CoreGraphics)
	template <class T>
	inline
	ZGExtentPOD_T<T>::operator CGSize() const
		{ CGSize size; size.width = h; size.height = v; return size; }


	template <class T>
	inline
	ZGPoint_T<T>::ZGPoint_T(const CGPoint& pt)
	:	x(T(pt.x)),
		y(T(pt.y))
		{}

	template <class T>
	inline
	ZGPoint_T<T>::operator CGPoint() const
		{ CGPoint p; p.x = x; p.y = y; return p; }

	template <class T>
	inline
	ZGExtent_T<T>::ZGExtent_T(const CGSize& iOther)
		{
		this->h = T(iOther.width);
		this->v = T(iOther.height);
		}

	template <class T>
	inline
	ZGRect_T<T>::ZGRect_T(const CGSize& iSize)
	:	origin(0),
		extent(iSize)
		{}

	template <class T>
	inline
	ZGRect_T<T>::ZGRect_T(const CGRect& iRect)
	:	origin(iRect.origin)
		{
		extent.h = T(iRect.size.width);
		extent.v = T(iRect.size.height);
		}

	template <class T>
	inline
	ZGRect_T<T>::operator CGRect() const
		{ CGRect rect; rect.origin = origin; rect.size = extent; return rect; }
#endif

#if ZCONFIG_SPI_Enabled(Cocoa)
	template <class T>
	inline
	ZGPoint_T<T>::ZGPoint_T(const NSPoint& pt)
	:	x(T(pt.x)),
		y(T(pt.y))
		{}

	template <class T>
	inline
	ZGPoint_T<T>::operator NSPoint() const
		{ NSPoint p; p.x = x; p.y = y; return p; }

	template <class T>
	inline
	ZGExtentPOD_T<T>::operator NSSize() const
		{ NSSize size; size.width = h; size.height = v; return size; }

	template <class T>
	inline
	ZGExtent_T<T>::ZGExtent_T(const NSSize& iOther)
		{
		this->h = T(iOther.width);
		this->v = T(iOther.height);
		}

	template <class T>
	inline
	ZGRect_T<T>::ZGRect_T(const NSSize& iSize)
	:	origin(0)
		{
		extent.h = iSize.width;
		extent.v = iSize.height;
		}

	template <class T>
	inline ZGRect_T<T>::ZGRect_T(const NSRect& iRect)
	:	origin(iRect.origin)
		{
		extent.h = iRect.size.width;
		extent.v = iRect.size.height;
		}

	template <class T>
	inline
	ZGRect_T<T>::operator NSRect() const
		{
		NSRect rect;
		rect.origin = origin;
		rect.size.width = extent.h;
		rect.size.height = extent.v;
		return rect;
		}
#endif

#if ZCONFIG_SPI_Enabled(QuickDraw)
	template <class T>
	inline
	ZGPoint_T<T>::ZGPoint_T(const Point& pt)
	:	x(T(pt.h)),
		y(T(pt.v))
		{}

	template <class T>
	inline
	ZGPoint_T<T>::operator Point() const
		{ Point p; p.h = x; p.v = y; return p; }

	template <class T>
	inline
	ZGExtentPOD_T<T>::operator Point() const
		{ Point p; p.h = h; p.v = v; return p; }

	template <class T>
	inline
	ZGExtent_T<T>::ZGExtent_T(const Point& pt)
		{
		this->h = T(pt.h);
		this->v = T(pt.v);
		}

	template <class T>
	inline
	ZGRect_T<T>::ZGRect_T(const Rect& iRect)
		{
		origin.x = T(iRect.left);
		origin.y = T(iRect.top);
		extent.h = T(iRect.right - iRect.left);
		extent.v = T(iRect.bottom - iRect.top);
		}

	template <class T>
	inline
	ZGRect_T<T>::operator Rect() const
		{
		Rect theRect;
		theRect.left = this->MinX();
		theRect.right = this->MaxX();
		theRect.top = this->MinY();
		theRect.bottom = this->MaxY();
		return theRect;
		}
#endif

#if ZCONFIG_SPI_Enabled(GDI)
	template <class T>
	inline
	ZGPoint_T<T>::ZGPoint_T(const POINT& pt)
	:	x(T(pt.x)),
		y(T(pt.y))
		{}

	template <class T>
	inline
	ZGPoint_T<T>::operator POINT() const
		{ POINT p; p.x = x; p.y = y; return p; }

	template <class T>
	inline
	ZGExtentPOD_T<T>::operator POINT() const
		{ POINT p; p.x = h; p.y = v; return p; }

	template <class T>
	inline
	ZGExtent_T<T>::ZGExtent_T(const POINT& pt)
		{
		this->h = T(pt.x);
		this->v = T(pt.y);
		}

	template <class T>
	inline
	ZGRect_T<T>::ZGRect_T(const RECT& iRect)
		{
		origin.x = T(iRect.left);
		origin.y = T(iRect.top);
		extent.h = T(iRect.right - iRect.left);
		extent.v = T(iRect.bottom - iRect.top);
		}

	template <class T>
	inline
	ZGRect_T<T>::operator RECT() const
		{
		RECT theRECT;
		theRECT.left = this->MinX();
		theRECT.right = this->MaxX();
		theRECT.top = this->MinY();
		theRECT.bottom = this->MaxY();
		return theRECT;
		}
#endif

#if ZCONFIG_SPI_Enabled(X11)
	template <class T>
	inline ZGPoint_T<T>::ZGPoint_T(const XPoint& pt)
	:	x(T(pt.x)),
		y(T(pt.y))
		{}

	template <class T>
	inline ZPoint_T<T>::operator XPoint() const
		{
		XPoint theXPoint;
		theXPoint.x = short(x);
		theXPoint.y = short(y);
		return theXPoint;
		}

	template <class T>
	inline ZGExtentPOD_T<T>::operator XPoint() const
		{
		XPoint theXPoint;
		theXPoint.x = short(h);
		theXPoint.y = short(v);
		return theXPoint;
		}

	template <class T>
	inline ZGExtent_T<T>::ZGExtent_T(const XPoint& pt)
	:	h(T(pt.x)),
		v(T(pt.y))
		{}

	template <class T>
	inline ZGRect_T<T>::ZGRect_T(const XRectangle& iRect)
	:	origin(iRect.x, iRect.y),
		extent(iRect.width, iRect.height)
		{}

	template <class T>
	inline ZGRect_T<T>::operator XRectangle() const
		{
		XRectangle theXRectangle;

		if (extent.h >= 0)
			{
			theXRectangle.x = short(origin.x);
			theXRectangle.width = unsigned short(extent.h);
			}
		else
			{
			theXRectangle.x = short(origin.x + extent.h);
			theXRectangle.width = unsigned short(-extent.h);
			}

		if (extent.v >= 0)
			{
			theXRectangle.y = short(origin.y);
			theXRectangle.height = unsigned short(extent.v);
			}
		else
			{
			theXRectangle.y = short(origin.y + extent.v);
			theXRectangle.height = unsigned short(-extent.v);
			}

		return theXRectangle;
		}
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Standard typedefs

typedef ZGPoint_T<float> ZGPointf;
typedef ZGExtent_T<float> ZGExtentf;
typedef ZGRect_T<float> ZGRectf;

NAMESPACE_ZOOLIB_END

float ZGRectGetMinX(const ZooLib::ZGRectf&);
float ZGRectGetMaxX(const ZooLib::ZGRectf&);
float ZGRectGetMinY(const ZooLib::ZGRectf&);
float ZGRectGetMaxY(const ZooLib::ZGRectf&);
float ZGRectGetWidth(const ZooLib::ZGRectf&);
float ZGRectGetHeight(const ZooLib::ZGRectf&);

#endif // __ZGeometry__
