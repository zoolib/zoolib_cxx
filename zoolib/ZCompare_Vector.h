/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZCompare_Vector__
#define __ZCompare_Vector__ 1
#include "zconfig.h"

#include "zoolib/ZCompare_T.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN

// Forward declaration of iterator comparison
template <class InputIterator>
inline int sCompareIterators_T(
	InputIterator leftIter, InputIterator leftEnd,
	InputIterator rightIter, InputIterator rightEnd);

template <class T>
inline int sCompare_T(const std::vector<T>& iLeft, const std::vector<T>& iRight)
	{ return sCompareIterators_T(iLeft.begin(), iLeft.end(), iRight.begin(), iRight.end()); }

// Definition of iterator comparison
template <class InputIterator>
inline int sCompareIterators_T(
	InputIterator leftIter, InputIterator leftEnd,
	InputIterator rightIter, InputIterator rightEnd)
	{
	for (/*no init*/; /*no test*/; ++leftIter, ++rightIter)
		{
		if (leftIter != leftEnd)
			{
			// Left is not exhausted.
			if (rightIter != rightEnd)
				{
				// Right is not exhausted either, so we compare their current values.
				if (int compare = sCompare_T(*leftIter, *rightIter))
					{
					// The current values of left and right
					// are different, so we have a result.
					return compare;
					}
				}
			else
				{
				// Exhausted right, but still have left
				// remaining, so left is greater than right.
				return 1;
				}
			}
		else
			{
			// Exhausted left.
			if (rightIter != rightEnd)
				{
				// Still have right remaining, so left is less than right.
				return -1;
				}
			else
				{
				// Exhausted right. And as left is also
				// exhausted left equals right.
				return 0;
				}
			}
		}
	}


NAMESPACE_ZOOLIB_END

#endif // __ZCompare_Vector__
