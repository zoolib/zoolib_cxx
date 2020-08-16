// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_Cartesian_h__
#define __ZooLib_Compare_Cartesian_h__ 1
#include "zconfig.h"

#include "zoolib/Cartesian.h"
#include "zoolib/Compare_T.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompare_T, point

template <class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,int>::type
sCompare_T(const Point_p& iLHS, const Point_p& iRHS)
	{
	if (int compare = sCompare_T(X(iLHS), X(iRHS)))
		return compare;
	return sCompare_T(Y(iLHS), Y(iRHS));
	}

// =================================================================================================
#pragma mark - sCompare_T, rect

template <class Rect_p>
typename EnableIfC<RectTraits<Rect_p>::value,int>::type
sCompare_T(const Rect_p& iLHS, const Rect_p& iRHS)
	{
	if (int compare = sCompare_T(L(iLHS), L(iRHS)))
		return compare;
	if (int compare = sCompare_T(T(iLHS), T(iRHS)))
		return compare;
	if (int compare = sCompare_T(R(iLHS), R(iRHS)))
		return compare;
	return sCompare_T(B(iLHS), B(iRHS));
	}

} // namespace ZooLib

#endif // __ZooLib_Compare_Cartesian_h__
