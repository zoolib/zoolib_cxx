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

#include "zoolib/ZGeomPOD.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)
	#if ZCONFIG_SPI_Enabled(iPhone)
		#include ZMACINCLUDE2(CoreGraphics,CGGeometry.h)
	#else
		#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGGeometry.h)
	#endif
#endif

#if ZCONFIG_SPI_Enabled(QuickDraw)
	#include ZMACINCLUDE3(CoreServices,CarbonCore,MacTypes.h)
	#include ZMACINCLUDE3(ApplicationServices,QD,QuickDraw.h)
#endif

#if ZCONFIG_SPI_Enabled(GDI)
	#include "zoolib/ZCompat_Win.h"
#endif

#if ZCONFIG_SPI_Enabled(X11)
	#include "zoolib/ZCompat_Xlib.h"
	#include <X11/Xutil.h>
#endif

#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX)

	#if defined(__OBJC__)
		#import <Foundation/NSGeometry.h>
		#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME \
			(defined(NSGEOMETRY_TYPES_SAME_AS_CGGEOMETRY_TYPES) \
			&& NSGEOMETRY_TYPES_SAME_AS_CGGEOMETRY_TYPES)
	#else
		#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 \
			&& (__LP64__ || NS_BUILD_32_LIKE_64)
			#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME 1
		#else
			typedef struct _NSPoint { float x; float y; } NSPoint;
			typedef struct _NSSize { float width; float height; } NSSize;
			typedef struct _NSRect { NSPoint origin; NSSize size; } NSRect;
		#endif
	#endif
#endif

#ifndef ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
	#define ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME 0
#endif

// Include these after the platform files -- cmath causes problems
// for Mac headers ('relation' is undefined) when building with CW.
#include "zoolib/ZCompat_algorithm.h" // For min and max
#include "zoolib/ZCompat_operator_bool.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZGPointPOD_T

template <class T>
class ZGPointPOD_T
	{
public:
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZGPointPOD_T<T>,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(x || y); }

	T x;
	T y;

	ZGPointPOD_T& operator=(T iVal);

	ZGPointPOD_T& operator=(const ZPointPOD& other);
	operator ZPointPOD() const;

	template <class S>
	ZGPointPOD_T& operator=(const ZGPointPOD_T<S>& other);

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGPointPOD_T& operator=(const CGPoint& pt);
		operator CGPoint() const;
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX) \
		&& !ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
		ZGPointPOD_T& operator=(const NSPoint& pt);
		operator NSPoint() const;
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZGPointPOD_T& operator=(const Point& pt);
		operator Point() const;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZGPointPOD_T& operator=(const POINT& pt);
		operator POINT() const;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZGPointPOD_T& operator=(const XPoint& pt);
		operator XPoint() const;
	#endif

// Relational operators
	bool operator!=(const ZGPointPOD_T& other) const { return x != other.x || y != other.y; }
	bool operator==(const ZGPointPOD_T& other) const { return x == other.x && y == other.y; }
	};

template <class T>
ZGPointPOD_T<T>& ZGPointPOD_T<T>::operator=(T iVal)
	{ x = y = iVal; return *this; }

template <class T>
ZGPointPOD_T<T>& ZGPointPOD_T<T>::operator=(const ZPointPOD& other)
	{ x = T(other.h); y = T(other.v); return *this; }

template <class T>
ZGPointPOD_T<T>::operator ZPointPOD() const
	{ return sPointPOD(x, y); }

template <class T>
template <class S>
ZGPointPOD_T<T>& ZGPointPOD_T<T>::operator=(const ZGPointPOD_T<S>& other)
	{ x = T(other.x); y = T(other.y); return *this; }

// =================================================================================================
#pragma mark -
#pragma mark * ZGPoint_T

template <class T>
class ZGPoint_T : public ZGPointPOD_T<T>
	{
public:
	ZGPoint_T();
	ZGPoint_T(T iVal);
	ZGPoint_T(T iX, T iY);

	ZGPoint_T(const ZPointPOD& iPoint);

	template <class S>
	ZGPoint_T(const ZGPoint_T<S>& other);

// Construct from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGPoint_T(const CGPoint& other);
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX) \
		&& !ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
		ZGPoint_T(const NSPoint& other);
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZGPoint_T(const Point& other);
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZGPoint_T(const POINT& other);
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZGPoint_T(const XPoint& other);
	#endif
	};

template <class T>
ZGPoint_T<T>::ZGPoint_T()
	{ this->x = 0; this->y = 0; }

template <class T>
ZGPoint_T<T>::ZGPoint_T(T iVal)
	{ this->x = iVal; this->y = iVal; }

template <class T>
ZGPoint_T<T>::ZGPoint_T(T iX, T iY)
	{ this->x = iX; this->y = iY; }

template <class T>
ZGPoint_T<T>::ZGPoint_T(const ZPointPOD& iPoint)
	{ this->x = T(iPoint.h); this->y = T(iPoint.v); }

template <class T>
template <class S>
ZGPoint_T<T>::ZGPoint_T(const ZGPoint_T<S>& other)
	{ this->x = T(other.x); this->y = T(other.y); }

// =================================================================================================
#pragma mark -
#pragma mark * ZGExtentPOD_T

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

	ZGExtentPOD_T& operator=(T iVal);

	ZGExtentPOD_T& operator=(const ZPointPOD& other);
	operator ZPointPOD() const;

	template <class S>
	ZGExtentPOD_T& operator=(const ZGExtentPOD_T<S>& other);

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGExtentPOD_T& operator=(const CGSize& pt);
		operator CGSize() const;
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX) \
		&& !ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
		ZGExtentPOD_T& operator=(const NSSize& pt);
		operator NSSize() const;
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZGExtentPOD_T& operator=(const Point& pt);
		operator Point() const;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZGExtentPOD_T& operator=(const POINT& pt);
		operator POINT() const;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZGExtentPOD_T& operator=(const XPoint& pt);
		operator XPoint() const;
	#endif

// Relational operators
	bool operator!=(const ZGExtentPOD_T& other) const { return h != other.h || v != other.v; }
	bool operator==(const ZGExtentPOD_T& other) const { return h == other.h && v == other.v; }
	};

template <class T>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(T iVal)
	{ h = v = iVal; return *this; }

template <class T>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(const ZPointPOD& other)
	{ h = T(other.h); v = T(other.v); return *this; }

template <class T>
ZGExtentPOD_T<T>::operator ZPointPOD() const
	{ return sPointPOD(h, v); }

template <class T>
template <class S>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(const ZGExtentPOD_T<S>& other)
	{ h = T(other.h); v = T(other.v); return *this; }

// =================================================================================================
#pragma mark -
#pragma mark * ZGExtent_T

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

// Construct from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGExtent_T(const CGSize& other);
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX) \
		&& !ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
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

template <class T>
ZGExtent_T<T>::ZGExtent_T()
	{ this->h = 0; this->v = 0; }

template <class T>
ZGExtent_T<T>::ZGExtent_T(T iVal)
	{ this->h = iVal; this->v = iVal; }

template <class T>
ZGExtent_T<T>::ZGExtent_T(T iH, T iV)
	{ this->h = iH; this->v = iV; }

template <class T>
template <class S>
ZGExtent_T<T>::ZGExtent_T(const ZGExtentPOD_T<S>& other)
	{ this->h = T(other.h); this->v = T(other.v); }

template <class T>
ZGExtent_T<T>::ZGExtent_T(const ZPointPOD& iPoint)
	{ this->h = T(iPoint.h); this->v = T(iPoint.v); }

// =================================================================================================
#pragma mark -
#pragma mark * ZGRectPOD_T

template <class T>
class ZGRect_T;

template <class T>
class ZGRectPOD_T
	{
public:
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZGRectPOD_T<T>,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(extent); }

	ZGPointPOD_T<T> origin;
	union { ZGExtentPOD_T<T> extent; ZGExtentPOD_T<T> size; };

	ZGRectPOD_T& operator=(const ZRectPOD& other);
	operator ZRectPOD() const;

	template <class U>
	ZGRectPOD_T& operator=(const ZGRectPOD_T<U>& iOther);

	template <class U>
	ZGRectPOD_T& operator=(const ZGExtentPOD_T<U>& iExtent);

// Conversions to & from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGRectPOD_T& operator=(const CGRect& rect);
		operator CGRect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX) \
		&& !ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
		ZGRectPOD_T& operator=(const NSRect& rect);
		operator NSRect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZGRectPOD_T& operator=(const Rect& rect);
		operator Rect() const;
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZGRectPOD_T& operator=(const RECT& rect);
		operator RECT() const;
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZGRectPOD_T& operator=(const XRectangle& rect);
		operator XRectangle() const;
	#endif

// Relational operators
	bool operator==(const ZGRectPOD_T& other) const
		{ return origin == other.origin && extent == other.extent; }

	bool operator!=(const ZGRectPOD_T& other) const
		{ return origin != other.origin || extent != other.extent; }

	T Width() const;
	T Height() const;

	ZGRect_T<T> FlippedY(T iHeight) const;

	ZGRectPOD_T& SetMinX(T iX);
	ZGRectPOD_T& SetMaxX(T iX);

	ZGRectPOD_T& SetMinY(T iY);
	ZGRectPOD_T& SetMaxY(T iY);

	T MinX() const;
	T MaxX() const;

	T MinY() const;
	T MaxY() const;

	T CenterX() const;
	T CenterY() const;
	ZGPoint_T<T> Center() const;

	ZGPoint_T<T> TL() const;
	ZGPoint_T<T> TR() const;
	ZGPoint_T<T> BL() const;
	ZGPoint_T<T> BR() const;

	template <class U>
	bool ContainsX(U iCoord) const;

	template <class U>
	bool ContainsY(U iCoord) const;

	template <class U>
	bool Contains(const ZGPoint_T<U>& pt) const;

	template <class U>
	bool Contains(U x, U y) const;
	};

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::operator=(const ZRectPOD& other)
	{
	this->origin.x = other.left;
	this->origin.y = other.top;
	this->extent.h = other.right - other.left;
	this->extent.v = other.bottom - other.top;
	return *this;
	}

template <class T>
ZGRectPOD_T<T>::operator ZRectPOD() const
	{ return sRectPOD(origin.x, origin.y, origin.x + extent.h, origin.y + extent.v); }

template <class T>
template <class U>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::operator=(const ZGRectPOD_T<U>& iOther)
	{
	origin = iOther.origin;
	extent = iOther.extent;
	return *this;
	}

template <class T>
template <class U>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::operator=(const ZGExtentPOD_T<U>& iExtent)
	{
	origin.x = T(0);
	origin.y = T(0);
	extent = iExtent;
	return *this;
	}

template <class T>
T ZGRectPOD_T<T>::Width() const
	{ return extent.h; }

template <class T>
T ZGRectPOD_T<T>::Height() const
	{ return extent.v; }

template <class T>
ZGRect_T<T> ZGRectPOD_T<T>::FlippedY(T iHeight) const
	{ return ZGRect_T<T>(origin.x, iHeight - origin.y - extent.v, extent); }

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::SetMinX(T iX)
	{
	if (extent.h >= 0)
		{
		extent.h += origin.x - iX;
		origin.x = iX;
		}
	else
		{
		extent.h = origin.x - iX;
		}
	return *this;
	}

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::SetMaxX(T iX)
	{
	if (extent.h >= 0)
		{
		extent.h = iX - origin.x;
		}
	else
		{
		extent.h += iX - origin.x;
		origin.x = iX;
		}
	return *this;
	}

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::SetMinY(T iY)
	{
	if (extent.v >= 0)
		{
		extent.v += origin.x - iY;
		origin.y = iY;
		}
	else
		{
		extent.v = origin.y - iY;
		}
	return *this;
	}

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::SetMaxY(T iY)
	{
	if (extent.v >= 0)
		{
		extent.v = iY - origin.y;
		}
	else
		{
		extent.v += iY - origin.y;
		origin.y = iY;
		}
	return *this;
	}

template <class T>
T ZGRectPOD_T<T>::MinX() const
	{ return extent.h >= 0 ? origin.x : origin.x + extent.h; }

template <class T>
T ZGRectPOD_T<T>::MaxX() const
	{ return extent.h < 0 ? origin.x : origin.x + extent.h; }

template <class T>
T ZGRectPOD_T<T>::MinY() const
	{ return extent.v >= 0 ? origin.y : origin.y + extent.v; }

template <class T>
T ZGRectPOD_T<T>::MaxY() const
	{ return extent.v < 0 ? origin.y : origin.y + extent.v; }

template <class T>
T ZGRectPOD_T<T>::CenterX() const
	{ return origin.x + extent.h / 2; }

template <class T>
T ZGRectPOD_T<T>::CenterY() const
	{ return origin.y + extent.v / 2; }

template <class T>
ZGPoint_T<T> ZGRectPOD_T<T>::Center() const
	{ return ZGPoint_T<T>(this->CenterX(), this->CenterY()); }

template <class T>
ZGPoint_T<T> ZGRectPOD_T<T>::TL() const
	{ return ZGPoint_T<T>(this->MinX(), this->MinY()); }

template <class T>
ZGPoint_T<T> ZGRectPOD_T<T>::TR() const
	{ return ZGPoint_T<T>(this->MaxX(), this->MinY()); }

template <class T>
ZGPoint_T<T> ZGRectPOD_T<T>::BL() const
	{ return ZGPoint_T<T>(this->MinX(), this->MaxY()); }

template <class T>
ZGPoint_T<T> ZGRectPOD_T<T>::BR() const
	{ return ZGPoint_T<T>(this->MaxX(), this->MaxY()); }

template <class T>
template <class U>
bool ZGRectPOD_T<T>::ContainsX(U iCoord) const
	{
	if (extent.h > 0) return iCoord >= origin.x && iCoord < origin.x + extent.h;
	else if (extent.h < 0) return iCoord < origin.x && iCoord >= origin.x + extent.h;
	else return false;
	}

template <class T>
template <class U>
bool ZGRectPOD_T<T>::ContainsY(U iCoord) const
	{
	if (extent.v > 0) return iCoord >= origin.y && iCoord < origin.y + extent.v;
	else if (extent.h < 0) return iCoord < origin.y && iCoord >= origin.y + extent.v;
	else return false;
	}

template <class T>
template <class U>
bool ZGRectPOD_T<T>::Contains(const ZGPoint_T<U>& pt) const
	{ return this->ContainsX(pt.x) && this->ContainsY(pt.y); }

template <class T>
template <class U>
bool ZGRectPOD_T<T>::Contains(U x, U y) const
	{ return this->ContainsX(x) && this->ContainsY(y); }

// =================================================================================================
#pragma mark -
#pragma mark * ZGRect_T

template <class T>
class ZGRect_T : public ZGRectPOD_T<T>
	{
public:
	ZGRect_T();
	ZGRect_T(const ZGRect_T<T>& iOther);
	ZGRect_T(T iWidth, T iHeight);

	ZGRect_T(const ZRectPOD& iRect);

	template <class U>
	ZGRect_T(const ZGExtentPOD_T<U>& iExtent);

	ZGRect_T(T iOriginX, T iOriginY, T iWidth, T iHeight);

	template <class U, class V>
	ZGRect_T(const ZGPointPOD_T<U>& iOrigin, const ZGExtentPOD_T<V>& iExtent);

	template <class U>
	ZGRect_T(T iOriginX, T iOriginY, const ZGExtentPOD_T<U>& iExtent);

	template <class U>
	ZGRect_T(const ZGPointPOD_T<U>& iOrigin, T iWidth, T iHeight);

// Construct from native types
	#if ZCONFIG_SPI_Enabled(CoreGraphics)
		ZGRect_T(const CGSize& iSize);
		ZGRect_T(const CGRect& iRect);
	#endif

	#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX) \
		&& !ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME
		ZGRect_T(const NSSize& iSize);
		ZGRect_T(const NSRect& iRect);
	#endif

	#if ZCONFIG_SPI_Enabled(QuickDraw)
		ZGRect_T(const Rect& iRect);
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
		ZGRect_T(const RECT& iRect);
	#endif

	#if ZCONFIG_SPI_Enabled(X11)
		ZGRect_T(const XRectangle& iRect);
	#endif
	};

template <class T>
ZGRect_T<T>::ZGRect_T()
	{
	this->origin = T(0);
	this->extent = T(0);
	}

template <class T>
ZGRect_T<T>::ZGRect_T(const ZGRect_T<T>& iOther)
	{
	this->origin = iOther.origin;
	this->extent = iOther.extent;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(T iWidth, T iHeight)
	{
	this->origin = T(0);
	this->extent.h = iWidth;
	this->extent.v = iHeight;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(const ZRectPOD& iRect)
	{
	this->origin.x = iRect.left;
	this->origin.y = iRect.top;
	this->extent.h = iRect.right - iRect.left;
	this->extent.v = iRect.bottom - iRect.top;
	}

template <class T>
template <class U>
ZGRect_T<T>::ZGRect_T(const ZGExtentPOD_T<U>& iExtent)
	{
	this->origin = T(0);
	this->extent = iExtent;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(T iOriginX, T iOriginY, T iWidth, T iHeight)
	{
	this->origin.x = iOriginX;
	this->origin.y = iOriginY;
	this->extent.h = iWidth;
	this->extent.v = iHeight;
	}

template <class T>
template <class U, class V>
ZGRect_T<T>::ZGRect_T(const ZGPointPOD_T<U>& iOrigin, const ZGExtentPOD_T<V>& iExtent)
	{
	this->origin = iOrigin;
	this->extent = iExtent;
	}

template <class T>
template <class U>
ZGRect_T<T>::ZGRect_T(T iOriginX, T iOriginY, const ZGExtentPOD_T<U>& iExtent)
	{
	this->origin.x = iOriginX;
	this->origin.y = iOriginY;
	this->extent = iExtent;
	}

template <class T>
template <class U>
ZGRect_T<T>::ZGRect_T(const ZGPointPOD_T<U>& iOrigin, T iWidth, T iHeight)
	{
	this->origin = iOrigin;
	this->extent.h = iWidth;
	this->extent.v = iHeight;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Operations - Extent/Extent

template <class T>
ZGExtentPOD_T<T> operator+(const ZGExtentPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ return ZGExtentPOD_T<T>(l.h + r.h, l.h + r.v); }

template <class T>
ZGExtentPOD_T<T>& operator+=(ZGExtentPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ l.h += r.h; l.h += r.v; return l; }


template <class T>
ZGExtentPOD_T<T> operator-(const ZGExtentPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ return ZGExtentPOD_T<T>(l.h - r.h, l.v - r.v); }

template <class T>
ZGExtentPOD_T<T>& operator-=(ZGExtentPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ l.h -= r.h; l.v -= r.v; return l; }

// =================================================================================================
#pragma mark -
#pragma mark * Operations - Extent/Point

template <class T>
ZGPoint_T<T> operator+(const ZGPointPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ return ZGPoint_T<T>(l.x + r.h, l.y + r.v); }

template <class T>
ZGPoint_T<T>& operator+=(ZGPoint_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ l.x += r.h; l.y += r.v; return l; }


template <class T>
ZGPoint_T<T> operator-(const ZGPointPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ return ZGPoint_T<T>(l.x - r.h, l.y - r.v); }

template <class T>
ZGExtent_T<T> operator-(const ZGPointPOD_T<T>& l, const ZGPoint_T<T>& r)
	{ return ZGExtent_T<T>(l.x - r.x, l.y - r.y); }

template <class T>
ZGPoint_T<T>& operator-=(ZGPoint_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ l.x -= r.h; l.y -= r.v; return l; }

// =================================================================================================
#pragma mark -
#pragma mark * Operations - Rect/Extent

template <class T>
ZGRect_T<T> operator+(const ZGRectPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ return ZGRect_T<T>(l.origin + r, l.extent); }

template <class T>
ZGRectPOD_T<T>& operator+=(ZGRectPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ l.origin += r; return l; }


template <class T>
ZGRect_T<T> operator-(const ZGRectPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ return ZGRect_T<T>(l.origin - r, l.extent); }

template <class T>
ZGRectPOD_T<T>& operator-=(ZGRectPOD_T<T>& l, const ZGExtentPOD_T<T>& r)
	{ l.origin -= r; return l; }

// =================================================================================================
#pragma mark -
#pragma mark * Operations - Rect

template <class T, class U>
ZGRect_T<T> operator&(const ZGRectPOD_T<T>& l, const ZGRectPOD_T<U>& r)
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
ZGRectPOD_T<T>& operator&=(ZGRectPOD_T<T>& l, const ZGRectPOD_T<U>& r)
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
ZGRect_T<T> operator|(const ZGRectPOD_T<T>& l, const ZGRectPOD_T<U>& r)
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
ZGRectPOD_T<T>& operator|=(ZGRectPOD_T<T>& l, const ZGRectPOD_T<U>& r)
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
#pragma mark * CoreGraphics

#if ZCONFIG_SPI_Enabled(CoreGraphics)

template <class T>
ZGPointPOD_T<T>& ZGPointPOD_T<T>::operator=(const CGPoint& pt)
	{
	this->x = T(pt.x);
	this->y = T(pt.y);
	return *this;
	}

template <class T>
ZGPointPOD_T<T>::operator CGPoint() const
	{ CGPoint p; p.x = x; p.y = y; return p; }

template <class T>
ZGPoint_T<T>::ZGPoint_T(const CGPoint& pt)
	{
	this->x = T(pt.x);
	this->y = T(pt.y);
	}

template <class T>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(const CGSize& pt)
	{
	this->h = T(pt.width);
	this->v = T(pt.height);
	return *this;
	}

template <class T>
ZGExtentPOD_T<T>::operator CGSize() const
	{ CGSize size; size.width = h; size.height = v; return size; }

template <class T>
ZGExtent_T<T>::ZGExtent_T(const CGSize& iOther)
	{
	this->h = T(iOther.width);
	this->v = T(iOther.height);
	}

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::operator=(const CGRect& rect)
	{
	this->origin = rect.origin;
	this->extent = rect.size;
	return *this;
	}

template <class T>
ZGRectPOD_T<T>::operator CGRect() const
	{ CGRect rect; rect.origin = origin; rect.size = extent; return rect; }

template <class T>
ZGRect_T<T>::ZGRect_T(const CGSize& iSize)
	{
	this->origin = T(0);
	this->extent = iSize;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(const CGRect& iRect)
	{
	this->origin = iRect.origin;
	this->extent = iRect.size;
	}

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)

// =================================================================================================
#pragma mark -
#pragma mark * Cocoa

#if ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX) \
	&& !ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME

template <class T>
ZGPointPOD_T<T>& ZGPointPOD_T<T>::operator=(const NSPoint& pt)
	{
	this->x = T(pt.x);
	this->y = T(pt.y);
	return *this;
	}

template <class T>
ZGPointPOD_T<T>::operator NSPoint() const
	{ NSPoint p; p.x = x; p.y = y; return p; }

template <class T>
ZGPoint_T<T>::ZGPoint_T(const NSPoint& pt)
	{
	this->x = T(pt.x);
	this->y = T(pt.y);
	}

template <class T>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(const NSSize& pt)
	{
	this->h = T(pt.width);
	this->v = T(pt.height);
	return *this;
	}

template <class T>
ZGExtentPOD_T<T>::operator NSSize() const
	{ NSSize size; size.width = h; size.height = v; return size; }

template <class T>
ZGExtent_T<T>::ZGExtent_T(const NSSize& iOther)
	{
	this->h = T(iOther.width);
	this->v = T(iOther.height);
	}

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::operator=(const NSRect& rect)
	{
	this->origin = rect.origin;
	this->extent = rect.size;
	return *this;
	}

template <class T>
ZGRectPOD_T<T>::operator NSRect() const
	{
	NSRect rect;
	rect.origin = origin;
	rect.size = extent;
	return rect;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(const NSSize& iSize)
	{
	this->origin = T(0);
	this->extent = iSize;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(const NSRect& iRect)
	{
	this->origin = iRect.origin;
	this->extent = iRect.size;
	}

#endif // ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX) \
	&& !ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME

// =================================================================================================
#pragma mark -
#pragma mark * QuickDraw

#if ZCONFIG_SPI_Enabled(QuickDraw)

template <class T>
ZGPointPOD_T<T>& ZGPointPOD_T<T>::operator=(const Point& pt)
	{
	this->x = T(pt.h);
	this->y = T(pt.v);
	return *this;
	}

template <class T>
ZGPointPOD_T<T>::operator Point() const
	{ Point p; p.h = x; p.v = y; return p; }

template <class T>
ZGPoint_T<T>::ZGPoint_T(const Point& pt)
	{
	this->x = T(pt.h);
	this->y = T(pt.v);
	}

template <class T>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(const Point& pt)
	{
	this->h = T(pt.h);
	this->v = T(pt.v);
	return *this;
	}

template <class T>
ZGExtentPOD_T<T>::operator Point() const
	{ Point p; p.h = h; p.v = v; return p; }

template <class T>
ZGExtent_T<T>::ZGExtent_T(const Point& pt)
	{
	this->h = T(pt.h);
	this->v = T(pt.v);
	}

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::operator=(const Rect& rect)
	{
	this->origin.x = T(rect.left);
	this->origin.y = T(rect.top);
	this->extent.h = T(rect.right - rect.left);
	this->extent.v = T(rect.bottom - rect.top);
	return *this;
	}

template <class T>
ZGRectPOD_T<T>::operator Rect() const
	{
	Rect theRect;
	theRect.left = this->MinX();
	theRect.right = this->MaxX();
	theRect.top = this->MinY();
	theRect.bottom = this->MaxY();
	return theRect;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(const Rect& iRect)
	{
	this->origin.x = T(iRect.left);
	this->origin.y = T(iRect.top);
	this->extent.h = T(iRect.right - iRect.left);
	this->extent.v = T(iRect.bottom - iRect.top);
	}

#endif // ZCONFIG_SPI_Enabled(QuickDraw)

// =================================================================================================
#pragma mark -
#pragma mark * GDI

#if ZCONFIG_SPI_Enabled(GDI)

template <class T>
ZGPointPOD_T<T>& ZGPointPOD_T<T>::operator=(const POINT& pt)
	{
	this->x = T(pt.x);
	this->y = T(pt.y);
	return *this;
	}

template <class T>
ZGPointPOD_T<T>::operator POINT() const
	{ POINT p; p.x = x; p.y = y; return p; }

template <class T>
ZGPoint_T<T>::ZGPoint_T(const POINT& pt)
	{
	this->x = T(pt.x);
	this->y = T(pt.y);
	}

template <class T>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(const POINT& pt)
	{
	this->h = T(pt.x);
	this->v = T(pt.y);
	return *this;
	}

template <class T>
ZGExtentPOD_T<T>::operator POINT() const
	{ POINT p; p.x = h; p.y = v; return p; }

template <class T>
ZGExtent_T<T>::ZGExtent_T(const POINT& pt)
	{
	this->h = T(pt.x);
	this->v = T(pt.y);
	}

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::operator=(const RECT& rect)
	{
	this->origin.x = T(rect.left);
	this->origin.y = T(rect.top);
	this->extent.h = T(rect.right - rect.left);
	this->extent.v = T(rect.bottom - rect.top);
	return *this;
	}

template <class T>
ZGRectPOD_T<T>::operator RECT() const
	{
	RECT theRECT;
	theRECT.left = this->MinX();
	theRECT.right = this->MaxX();
	theRECT.top = this->MinY();
	theRECT.bottom = this->MaxY();
	return theRECT;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(const RECT& iRect)
	{
	this->origin.x = T(iRect.left);
	this->origin.y = T(iRect.top);
	this->extent.h = T(iRect.right - iRect.left);
	this->extent.v = T(iRect.bottom - iRect.top);
	}

#endif // ZCONFIG_SPI_Enabled(GDI)

// =================================================================================================
#pragma mark -
#pragma mark * X11

#if ZCONFIG_SPI_Enabled(X11)

template <class T>
ZGPointPOD_T<T>& ZGPointPOD_T<T>::operator=(const XPoint& pt)
	{
	this->x = T(pt.x);
	this->y = T(pt.y);
	return *this;
	}

template <class T>
ZPointPOD_T<T>::operator XPoint() const
	{
	XPoint theXPoint;
	theXPoint.x = short(x);
	theXPoint.y = short(y);
	return theXPoint;
	}

template <class T>
ZGPoint_T<T>::ZGPoint_T(const XPoint& pt)
:	x(T(pt.x)),
	y(T(pt.y))
	{
	this->x = T(pt.x);
	this->y = T(pt.y);
	}

template <class T>
ZGExtentPOD_T<T>& ZGExtentPOD_T<T>::operator=(const XPoint& pt)
	{
	this->h = T(pt.x);
	this->v = T(pt.y);
	return *this;
	}

template <class T>
ZGExtentPOD_T<T>::operator XPoint() const
	{
	XPoint theXPoint;
	theXPoint.x = short(h);
	theXPoint.y = short(v);
	return theXPoint;
	}

template <class T>
ZGExtent_T<T>::ZGExtent_T(const XPoint& pt)
	{
	this->h = T(pt.x);
	this->v = T(pt.y);
	}

template <class T>
ZGRectPOD_T<T>& ZGRectPOD_T<T>::operator=(const XRectangle& rect)
	{
	this->origin.x = T(rect.x);
	this->origin.y = T(rect.y);
	this->extent.x = T(rect.width);
	this->extent.y = T(rect.height);
	return *this;
	}

template <class T>
ZGRectPOD_T<T>::operator XRectangle() const
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

template <class T>
ZGRect_T<T>::ZGRect_T(const XRectangle& iRect)
:	origin(iRect.x, iRect.y),
	extent(iRect.width, iRect.height)
	{
	this->origin.x = T(iRect.x);
	this->origin.y = T(iRect.y);
	this->extent.x = T(iRect.width);
	this->extent.y = T(iRect.height);
	}

#endif // ZCONFIG_SPI_Enabled(X11)

// =================================================================================================
#pragma mark -
#pragma mark * Standard typedefs

typedef ZGPoint_T<float> ZGPointf;
typedef ZGExtent_T<float> ZGExtentf;
typedef ZGRect_T<float> ZGRectf;

} // namespace ZooLib

#endif // __ZGeometry__
