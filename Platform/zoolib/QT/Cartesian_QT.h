// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QT_Cartesian_QT_h__
#define __ZooLib_QT_Cartesian_QT_h__ 1
#include "zconfig.h"

#include "zoolib/Cartesian.h"

#include <QRect> // For QPoint(F), qreal, QRect(F) and QSize

namespace ZooLib {
namespace Cartesian {

// =================================================================================================
#pragma mark - PointTraits<QPoint>

template <>
struct PointTraits<QPoint>
:	public PointTraits_Std<int,QPoint,QRect>
	{
	typedef const Ord_t XC_t;
	static XC_t sX(const Point_t& iPoint) { return iPoint.x(); }

	typedef Ord_t& X_t;
	static X_t sX(Point_t& ioPoint) { return ioPoint.rx(); }

	typedef const Ord_t YC_t;
	static YC_t sY(const Point_t& iPoint) { return iPoint.y(); }

	typedef Ord_t& Y_t;
	static Y_t sY(Point_t& ioPoint) { return ioPoint.ry(); }
	};

// =================================================================================================
#pragma mark - PointTraits<QSize>

template <>
struct PointTraits<QSize>
:	public PointTraits_Std<int,QSize,QRect>
	{
	typedef const Ord_t XC_t;
	static XC_t sX(const Point_t& iPoint) { return iPoint.width(); }

	typedef Ord_t& X_t;
	static X_t sX(Point_t& ioPoint) { return ioPoint.rwidth(); }

	typedef const Ord_t YC_t;
	static YC_t sY(const Point_t& iPoint) { return iPoint.height(); }

	typedef Ord_t& Y_t;
	static Y_t sY(Point_t& ioPoint) { return ioPoint.rheight(); }
	};

// =================================================================================================
#pragma mark - RectTraits<QRect>

template <>
struct RectTraits<QRect>
:	public RectTraits_Std<int,QPoint,QRect>
	{
	typedef const Ord_t LC_t;
	static LC_t sL(const Rect_t& iRect) { return iRect.left(); }

	typedef const Ord_t TC_t;
	static TC_t sT(const Rect_t& iRect) { return iRect.top(); }

	typedef const Ord_t RC_t;
	static RC_t sR(const Rect_t& iRect) { return iRect.right(); }

	typedef const Ord_t BC_t;
	static BC_t sB(const Rect_t& iRect) { return iRect.bottom(); }

	#if defined(QT3_SUPPORT)
		typedef Ord_t& L_t;
		static L_t sL(Rect_t& ioRect) { return ioRect.rLeft(); }

		typedef Ord_t& T_t;
		static T_t sT(Rect_t& ioRect) { return ioRect.rTop(); }

		typedef Ord_t& R_t;
		static R_t sL(Rect_t& ioRect) { return ioRect.rRight(); }

		typedef Ord_t& B_t;
		static B_t sB(Rect_t& ioRect) { return ioRect.rBottom(); }
	#else
		typedef LC_t L_t;
		typedef TC_t T_t;
		typedef RC_t R_t;
		typedef BC_t B_t;
	#endif

	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{
		// QRect's ctor takes l, t, w and h, but width and height of a qrect are interpreted
		// differently than in most situations (and differently than QRectF), hence the +1s.
		return Rect_t(iL, iT, iR-iL+1, iB-iT+1);
		}
	};

// =================================================================================================
#pragma mark - PointTraits<QPointF>

template <>
struct PointTraits<QPointF>
:	public PointTraits_Std<qreal,QPointF,QRectF>
	{
	typedef const Ord_t XC_t;
	static XC_t sX(const Point_t& iPoint) { return iPoint.x(); }

	typedef Ord_t& X_t;
	static X_t sX(Point_t& ioPoint) { return ioPoint.rx(); }

	typedef const Ord_t YC_t;
	static YC_t sY(const Point_t& iPoint) { return iPoint.y(); }

	typedef Ord_t& Y_t;
	static Y_t sY(Point_t& ioPoint) { return ioPoint.ry(); }
	};

// =================================================================================================
#pragma mark - RectTraits<QRectF>

template <>
struct RectTraits<QRectF>
:	public RectTraits_Std<qreal,QPointF,QRectF>
	{
	typedef const Ord_t LC_t;
	typedef LC_t L_t;
	static LC_t sL(const Rect_t& iRect) { return iRect.left(); }

	typedef const Ord_t TC_t;
	typedef TC_t T_t;
	static TC_t sT(const Rect_t& iRect) { return iRect.top(); }

	typedef const Ord_t RC_t;
	typedef RC_t R_t;
	static RC_t sR(const Rect_t& iRect) { return iRect.right(); }

	typedef const Ord_t BC_t;
	typedef BC_t B_t;
	static BC_t sB(const Rect_t& iRect) { return iRect.bottom(); }

	static Rect_t sMake(const Ord_t& iL, const Ord_t& iT, const Ord_t& iR, const Ord_t& iB)
		{ return Rect_t(iL, iT, iR - iL, iB - iT); }
	};

} // namespace Cartesian
} // namespace ZooLib

#endif // __ZooLib_QT_Cartesian_QT_h__
