// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Strim_Cartesian_h__
#define __ZooLib_Util_Strim_Cartesian_h__ 1
#include "zconfig.h"

#include "zoolib/Cartesian.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

namespace ZooLib {

template <class Point_p>
typename EnableIfC<Cartesian::PointTraits<Point_p>::value,const ChanW_UTF&>::type
operator<<(const ChanW_UTF& w, const Point_p& iPoint)
	{ return w << "(" << X(iPoint) << "," << Y(iPoint) << ")"; }

template <class Rect_p>
typename EnableIfC<Cartesian::RectTraits<Rect_p>::value,const ChanW_UTF&>::type
operator<<(const ChanW_UTF& w, const Rect_p& iRect)
	{ return w << "(" << L(iRect) << "," << T(iRect) << "," << R(iRect) << "," << B(iRect) << ")"; }

} // namespace ZooLib

#endif // __ZooLib_Util_Strim_Cartesian_h__
