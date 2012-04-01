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

#ifndef __ZCompare_Cartesian_h__
#define __ZCompare_Cartesian_h__ 1
#include "zconfig.h"

#include "zoolib/ZCartesian.h"
#include "zoolib/ZCompare_T.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T, point

template <class Point_p>
typename EnableIfC<PointTraits<Point_p>::value,int>::type
sCompare_T(const Point_p& iLHS, const Point_p& iRHS)
	{
	if (int compare = sCompare_T(X(iLHS), X(iRHS)))
		return compare;
	return sCompare_T(Y(iLHS), Y(iRHS));
	}

// =================================================================================================
// MARK: - sCompare_T, rect

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

#endif // __ZCompare_Cartesian_h__
