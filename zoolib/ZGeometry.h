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

template <class Ord>
class ZGRectPOD_T
	{
public:
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZGRectPOD_T<Ord>,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(extent); }

	ZGPointPOD_T<Ord> origin;
	union { ZGExtentPOD_T<Ord> extent; ZGExtentPOD_T<Ord> size; };

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

	ZGPointPOD_T<Ord> Origin() const;
	ZGExtentPOD_T<Ord> Extent() const;

	Ord Width() const;
	Ord Height() const;

	Ord L() const; // aka MinX
	Ord R() const; // aka MaxX

	Ord T() const; // aka MinY
	Ord B() const; // aka MaxY

	ZGPoint_T<Ord> LT() const;
	ZGPoint_T<Ord> RT() const;
	ZGPoint_T<Ord> LB() const;
	ZGPoint_T<Ord> RB() const;

	Ord CenterX() const;
	Ord CenterY() const;
	ZGPoint_T<Ord> Center() const;

	ZGRect_T<Ord> WithWidth(Ord iOrd) const;
	ZGRect_T<Ord> WithHeight(Ord iOrd) const;

	ZGRect_T<Ord> WithL(Ord iOrd) const;
	ZGRect_T<Ord> WithT(Ord iOrd) const;
	ZGRect_T<Ord> WithR(Ord iOrd) const;
	ZGRect_T<Ord> WithB(Ord iOrd) const;

	ZGRect_T<Ord> WithLT(ZGPoint_T<Ord> iPoint) const;
	ZGRect_T<Ord> WithRT(ZGPoint_T<Ord> iPoint) const;
	ZGRect_T<Ord> WithLB(ZGPoint_T<Ord> iPoint) const;
	ZGRect_T<Ord> WithRB(ZGPoint_T<Ord> iPoint) const;

	ZGRect_T<Ord> CenteredX(Ord iOrd) const;
	ZGRect_T<Ord> CenteredY(Ord iOrd) const;
	ZGRect_T<Ord> Centered(Ord iX, Ord iY) const;
	ZGRect_T<Ord> Centered(ZGPoint_T<Ord> iPoint) const;

	ZGRect_T<Ord> AlignedL(Ord iOrd) const;
	ZGRect_T<Ord> AlignedT(Ord iOrd) const;
	ZGRect_T<Ord> AlignedR(Ord iOrd) const;
	ZGRect_T<Ord> AlignedB(Ord iOrd) const;

	ZGRect_T<Ord> AlignedLT(ZGPoint_T<Ord> iPoint) const;
	ZGRect_T<Ord> AlignedRT(ZGPoint_T<Ord> iPoint) const;
	ZGRect_T<Ord> AlignedLB(ZGPoint_T<Ord> iPoint) const;
	ZGRect_T<Ord> AlignedRB(ZGPoint_T<Ord> iPoint) const;

	ZGRect_T<Ord> InsettedH(Ord iOrd) const;
	ZGRect_T<Ord> InsettedV(Ord iOrd) const;
	ZGRect_T<Ord> Insetted(Ord iH, Ord iV) const;
	ZGRect_T<Ord> Insetted(ZGExtent_T<Ord> iExtent) const;

	ZGRect_T<Ord> FlippedY(Ord iHeight) const;

	template <class U>
	bool ContainsX(U iOrd) const;

	template <class U>
	bool ContainsY(U iOrd) const;

	template <class U>
	bool Contains(const ZGPoint_T<U>& pt) const;

	template <class U>
	bool Contains(U x, U y) const;

// Min/Max API, akin to CoreGraphics
	Ord MinX() const { return this->L(); }
	Ord MaxX() const { return this->R(); }

	Ord MinY() const { return this->T(); }
	Ord MaxY() const { return this->B(); }

	ZGRect_T<Ord> WithMinX(Ord iOrd) const { return this->WithL(iOrd); }
	ZGRect_T<Ord> WithMinY(Ord iOrd) const { return this->WithT(iOrd); }
	ZGRect_T<Ord> WithMaxX(Ord iOrd) const { return this->WithR(iOrd); }
	ZGRect_T<Ord> WithMaxY(Ord iOrd) const { return this->WithB(iOrd); }
	};

template <class Ord>
ZGRectPOD_T<Ord>& ZGRectPOD_T<Ord>::operator=(const ZRectPOD& other)
	{
	this->origin.x = other.left;
	this->origin.y = other.top;
	this->extent.h = other.right - other.left;
	this->extent.v = other.bottom - other.top;
	return *this;
	}

template <class Ord>
ZGRectPOD_T<Ord>::operator ZRectPOD() const
	{ return sRectPOD(origin.x, origin.y, origin.x + extent.h, origin.y + extent.v); }

template <class Ord>
template <class U>
ZGRectPOD_T<Ord>& ZGRectPOD_T<Ord>::operator=(const ZGRectPOD_T<U>& iOther)
	{
	origin = iOther.origin;
	extent = iOther.extent;
	return *this;
	}

template <class Ord>
template <class U>
ZGRectPOD_T<Ord>& ZGRectPOD_T<Ord>::operator=(const ZGExtentPOD_T<U>& iExtent)
	{
	origin.x = T();
	origin.y = T();
	extent = iExtent;
	return *this;
	}

template <class Ord>
ZGPointPOD_T<Ord> ZGRectPOD_T<Ord>::Origin() const
	{ return origin; }

template <class Ord>
ZGExtentPOD_T<Ord> ZGRectPOD_T<Ord>::Extent() const
	{ return extent; }

template <class Ord>
Ord ZGRectPOD_T<Ord>::Width() const
	{ return extent.h; }

template <class Ord>
Ord ZGRectPOD_T<Ord>::Height() const
	{ return extent.v; }

template <class Ord>
Ord ZGRectPOD_T<Ord>::L() const
	{ return extent.h >= 0 ? origin.x : origin.x + extent.h; }

template <class Ord>
Ord ZGRectPOD_T<Ord>::R() const
	{ return extent.h < 0 ? origin.x : origin.x + extent.h; }

template <class Ord>
Ord ZGRectPOD_T<Ord>::T() const
	{ return extent.v >= 0 ? origin.y : origin.y + extent.v; }

template <class Ord>
Ord ZGRectPOD_T<Ord>::B() const
	{ return extent.v < 0 ? origin.y : origin.y + extent.v; }

template <class Ord>
ZGPoint_T<Ord> ZGRectPOD_T<Ord>::LT() const
	{ return ZGPoint_T<Ord>(this->L(), this->T()); }

template <class Ord>
ZGPoint_T<Ord> ZGRectPOD_T<Ord>::RT() const
	{ return ZGPoint_T<Ord>(this->R(), this->T()); }

template <class Ord>
ZGPoint_T<Ord> ZGRectPOD_T<Ord>::LB() const
	{ return ZGPoint_T<Ord>(this->L(), this->B()); }

template <class Ord>
ZGPoint_T<Ord> ZGRectPOD_T<Ord>::RB() const
	{ return ZGPoint_T<Ord>(this->R(), this->B()); }

template <class Ord>
Ord ZGRectPOD_T<Ord>::CenterX() const
	{ return origin.x + extent.h / 2; }

template <class Ord>
Ord ZGRectPOD_T<Ord>::CenterY() const
	{ return origin.y + extent.v / 2; }

template <class Ord>
ZGPoint_T<Ord> ZGRectPOD_T<Ord>::Center() const
	{ return ZGPoint_T<Ord>(this->CenterX(), this->CenterY()); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithWidth(Ord iOrd) const
	{ return ZGRect_T<Ord>(origin, iOrd, extent.v); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithHeight(Ord iOrd) const
	{ return ZGRect_T<Ord>(origin, extent.h, iOrd); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithL(Ord iOrd) const
	{
	if (extent.h >= 0)
		return ZGRect_T<Ord>(iOrd, origin.y, extent.h + origin.x - iOrd, extent.v);
	else
		return ZGRect_T<Ord>(origin, origin.x - iOrd, extent.v);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithT(Ord iOrd) const
	{
	if (extent.v >= 0)
		return ZGRect_T<Ord>(origin.x, iOrd, extent.h, extent.v + origin.y - iOrd);
	else
		return ZGRect_T<Ord>(origin, extent.h, origin.y - iOrd);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithR(Ord iOrd) const
	{
	if (extent.h >= 0)
		return ZGRect_T<Ord>(origin, iOrd - origin.x, extent.v);
	else
		return ZGRect_T<Ord>(iOrd, origin.y, extent.h + iOrd - origin.x, extent.v);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithB(Ord iOrd) const
	{
	if (extent.v >= 0)
		return ZGRect_T<Ord>(origin, extent.h, iOrd - origin.y);
	else
		return ZGRect_T<Ord>(origin.x, iOrd, extent.h, extent.v + iOrd - origin.y);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithLT(ZGPoint_T<Ord> iPoint) const
	{ return this->WithT(iPoint.y).WithL(iPoint.x); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithRT(ZGPoint_T<Ord> iPoint) const
	{ return this->WithT(iPoint.y).WithR(iPoint.x); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithLB(ZGPoint_T<Ord> iPoint) const
	{ return this->WithB(iPoint.y).WithL(iPoint.x); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::WithRB(ZGPoint_T<Ord> iPoint) const
	{ return this->WithB(iPoint.y).WithR(iPoint.x); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::CenteredX(Ord iOrd) const
	{ return ZGRect_T<Ord>(iOrd - extent.h / 2, origin.y, extent); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::CenteredY(Ord iOrd) const
	{ return ZGRect_T<Ord>(origin.x, iOrd - extent.v / 2, extent); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::Centered(Ord iX, Ord iY) const
	{ return ZGRect_T<Ord>(iX - extent.h / 2, iY - extent.v / 2, extent); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::Centered(ZGPoint_T<Ord> iPoint) const
	{ return this->Centered(iPoint.x, iPoint.y); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::AlignedL(Ord iOrd) const
	{
	if (extent.h >= 0)
		return ZGRect_T<Ord>(iOrd, origin.y, extent);
	else
		return ZGRect_T<Ord>(iOrd - origin.x + extent.h, origin.y, extent);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::AlignedT(Ord iOrd) const
	{
	if (extent.v >= 0)
		return ZGRect_T<Ord>(origin.x, iOrd, extent);
	else
		return ZGRect_T<Ord>(origin.x, iOrd - origin.y + extent.v, extent);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::AlignedR(Ord iOrd) const
	{
	if (extent.h >= 0)
		return ZGRect_T<Ord>(iOrd - extent.h, origin.y, extent);
	else
		return ZGRect_T<Ord>(iOrd, origin.y, extent);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::AlignedB(Ord iOrd) const
	{
	if (extent.v >= 0)
		return ZGRect_T<Ord>(origin.x, iOrd - extent.v, extent);
	else
		return ZGRect_T<Ord>(origin.x, iOrd, extent);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::AlignedLT(ZGPoint_T<Ord> iPoint) const
	{ return this->AlignedT(iPoint.y).AlignedL(iPoint.x); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::AlignedRT(ZGPoint_T<Ord> iPoint) const
	{ return this->AlignedT(iPoint.y).AlignedR(iPoint.x); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::AlignedLB(ZGPoint_T<Ord> iPoint) const
	{ return this->AlignedB(iPoint.y).AlignedL(iPoint.x); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::AlignedRB(ZGPoint_T<Ord> iPoint) const
	{ return this->AlignedB(iPoint.y).AlignedR(iPoint.x); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::InsettedH(Ord iOrd) const
	{
	if (extent.h >= 0)
		return ZGRect_T<Ord>(origin.x + iOrd, origin.y, extent.h - iOrd - iOrd, extent.v);
	else
		return ZGRect_T<Ord>(origin.x - iOrd, origin.y, extent.h + iOrd + iOrd, extent.v);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::InsettedV(Ord iOrd) const
	{
	if (extent.v >= 0)
		return ZGRect_T<Ord>(origin.x, origin.y + iOrd, extent.h, extent.v - iOrd - iOrd);
	else
		return ZGRect_T<Ord>(origin.x, origin.y - iOrd, extent.h, extent.v + iOrd + iOrd);
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::Insetted(Ord iH, Ord iV) const
	{
	if (extent.h >= 0)
		{
		if (extent.v >= 0)
			return ZGRect_T<Ord>(origin.x + iH, origin.y + iV, extent.h - iH - iH, extent.v - iV - iV);
		else
			return ZGRect_T<Ord>(origin.x + iH, origin.y - iV, extent.h - iH - iH, extent.v + iV + iV);
		}
	else
		{
		if (extent.v >= 0)
			return ZGRect_T<Ord>(origin.x - iH, origin.y + iV, extent.h + iH + iH, extent.v - iV - iV);
		else
			return ZGRect_T<Ord>(origin.x - iH, origin.y - iV, extent.h + iH + iH, extent.v + iV + iV);
		}
	}

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::Insetted(ZGExtent_T<Ord> iExtent) const
	{ return this->Insetted(iExtent.h, iExtent.v); }

template <class Ord>
ZGRect_T<Ord> ZGRectPOD_T<Ord>::FlippedY(Ord iHeight) const
	{ return ZGRect_T<Ord>(origin.x, iHeight - origin.y - extent.v, extent); }

template <class Ord>
template <class U>
bool ZGRectPOD_T<Ord>::ContainsX(U iOrd) const
	{
	if (extent.h > 0) return iOrd >= origin.x && iOrd < origin.x + extent.h;
	else if (extent.h < 0) return iOrd < origin.x && iOrd >= origin.x + extent.h;
	else return false;
	}

template <class Ord>
template <class U>
bool ZGRectPOD_T<Ord>::ContainsY(U iOrd) const
	{
	if (extent.v > 0) return iOrd >= origin.y && iOrd < origin.y + extent.v;
	else if (extent.h < 0) return iOrd < origin.y && iOrd >= origin.y + extent.v;
	else return false;
	}

template <class Ord>
template <class U>
bool ZGRectPOD_T<Ord>::Contains(const ZGPoint_T<U>& pt) const
	{ return this->ContainsX(pt.x) && this->ContainsY(pt.y); }

template <class Ord>
template <class U>
bool ZGRectPOD_T<Ord>::Contains(U x, U y) const
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
	this->origin = T();
	this->extent = T();
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
	this->origin = T();
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
	this->origin = T();
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
	this->origin = T();
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
	this->origin = T();
	this->extent = iSize;
	}

template <class T>
ZGRect_T<T>::ZGRect_T(const NSRect& iRect)
	{
	this->origin = iRect.origin;
	this->extent = iRect.size;
	}

#endif // ZCONFIG_SPI_Enabled(Cocoa) && ZCONFIG_SPI_Enabled(MacOSX) 
// && !ZMACRO_NS_AND_CG_GEOMETRY_ARE_SAME

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
