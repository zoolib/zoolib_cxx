/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZCompare.h"

#include "zoolib/ZCompat_cmath.h"

NAMESPACE_ZOOLIB_BEGIN

template <>
int sCompare_T(const ZRectPOD& iL, const ZRectPOD& iR)
	{
	if (int compare = sCompare_T(iL.left, iR.left))
		return compare;
	if (int compare = sCompare_T(iL.top, iR.top))
		return compare;
	if (int compare = sCompare_T(iL.right, iR.right))
		return compare;
	return sCompare_T(iL.bottom, iR.bottom);
	}

template <>
int sCompare_T(const ZPointPOD& iL, const ZPointPOD& iR)
	{
	if (int compare = sCompare_T(iL.h, iR.h))
		return compare;
	return sCompare_T(iL.v, iR.v);
	}

// To provide stable ordering between floating point values we
// arbitrarily say that a nan is less than any non-nan value.
template <>
int sCompare_T(const float& iL, const float& iR)
	{
	if (isnan(iL))
		{
		if (isnan(iR))
			return 0;
		return -1;
		}
	else if (isnan(iR))
		return 1;
	else if (iL < iR)
		return -1;
	else if (iL > iR)
		return 1;
	return 0;
	}

template <>
int sCompare_T(const double& iL, const double& iR)
	{
	if (isnan(iL))
		{
		if (isnan(iR))
			return 0;
		return -1;
		}
	else if (isnan(iR))
		return 1;
	else if (iL < iR)
		return -1;
	else if (iL > iR)
		return 1;
	return 0;
	}

NAMESPACE_ZOOLIB_END
