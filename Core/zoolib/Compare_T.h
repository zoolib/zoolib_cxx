// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_T_h__
#define __ZooLib_Compare_T_h__ 1
#include "zconfig.h"

#include <functional> // For std::binary_function
#include <memory> // For std::pair

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompare_T declaration

template <class T> int sCompare_T(const T& iL, const T& iR);

// =================================================================================================
#pragma mark - sCompare_T specialized for void pointers

typedef void* VoidStar_t;

template <> inline int sCompare_T(const VoidStar_t& iL, const VoidStar_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

typedef const void* ConstVoidStar_t;

template <> inline int sCompare_T(const ConstVoidStar_t& iL, const ConstVoidStar_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

// =================================================================================================
#pragma mark - sCompare_T specialized for std::pair

template <class S, class T>
inline int sCompare_T(const std::pair<S,T>& iLeft, const std::pair<S,T>& iRight)
	{
	if (int compare = sCompare_T(iLeft.first, iRight.first))
		return compare;
	return sCompare_T(iLeft.second, iRight.second);
	}

// =================================================================================================
#pragma mark - Less_Compare_T, less<>-style functor implemented in terms of sCompare_T

template <class T>
struct Less_Compare_T : public std::binary_function<T,T,bool>
	{
	bool operator()(const T& iLeft, const T& iRight) const
		{ return sCompare_T(iLeft, iRight) < 0; }
	};

// =================================================================================================
#pragma mark - sCompareIterators_T

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

} // namespace ZooLib

#endif // __ZooLib_Compare_T_h__
