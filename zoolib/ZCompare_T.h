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

#ifndef __ZCompare_T_h__
#define __ZCompare_T_h__ 1
#include "zconfig.h"

#include <functional> // For std::binary_function
#include <memory> // For std::pair

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T declaration

template <class T> int sCompare_T(const T& iL, const T& iR);

template <class S, class T>
inline int sCompare_T(const std::pair<S,T>& iLeft, const std::pair<S,T>& iRight)
	{
	if (int compare = sCompare_T(iLeft.first, iRight.first))
		return compare;
	return sCompare_T(iLeft.second, iRight.second);
	}

// =================================================================================================
// MARK: - Less_Compare_T

template <class T>
struct Less_Compare_T : public std::binary_function<T,T,bool>
	{
	bool operator()(const T& iLeft, const T& iRight) const
		{ return sCompare_T(iLeft, iRight) < 0; }
	};

// =================================================================================================
// MARK: - sCompareIterators_T

template <class InputIterator>
inline int sCompareIterators_T
	(InputIterator leftIter, InputIterator leftEnd,
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

} // namespace ZooLib

#endif // __ZCompare_Vector_h__
