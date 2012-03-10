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

#ifndef __ZCartesian_QT_h__
#define __ZCartesian_QT_h__ 1
#include "zconfig.h"

#include "zoolib/ZCartesian.h"

#include <QRect> // For QPoint(F), qreal, QRect(F) and QSize

namespace ZooLib {
namespace ZCartesian {

// =================================================================================================
// MARK: - PointTraits<QPoint>

template <>
struct PointTraits<QPoint>
:	public PointTraits_Std_XY<int,QPoint,QRect>
	{};

// =================================================================================================
// MARK: - PointTraits<QSize>

template <>
struct PointTraits<QSize>
:	public PointTraits_Std_XY<int,QSize,QRect>
	{};

// =================================================================================================
// MARK: - RectTraits<QRect>

template <>
struct RectTraits<QRect>
:	public RectTraits_Std_Base<int,QPoint,QRect>
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
// MARK: - PointTraits<QPointF>

template <>
struct PointTraits<QPointF>
:	public PointTraits_Std_XY<qreal,QPointF,QRectF>
	{};

// =================================================================================================
// MARK: - RectTraits<QRectF>

template <>
struct RectTraits<QRectF>
:	public RectTraits_Std_Base<qreal,QPointF,QRectF>
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

} // namespace ZCartesian
} // namespace ZooLib

#endif // __ZCartesian_QT_h__
