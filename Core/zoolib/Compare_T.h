// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_T_h__
#define __ZooLib_Compare_T_h__ 1
#include "zconfig.h"

#include "zoolib/ZDebug.h" // For ZUnimplemented
#include "zoolib/ZTypes.h" // For EnableIfC

#include <functional> // For std::binary_function

namespace ZooLib {

// =================================================================================================
#pragma mark - has_operator_less

template <class T, class Other_p>
struct has_operator_less_impl
	{
    template <class U, class V>
    static auto test(U*) -> decltype(std::declval<U>() < std::declval<V>());

    template <typename, typename>
    static auto test(...) -> std::false_type;

    using type = typename std::is_same<bool, decltype(test<T, Other_p>(0))>::type;
	};

template <class T, class Other_p = T>
struct has_operator_less : has_operator_less_impl<T, Other_p>::type {};

// =================================================================================================
#pragma mark - sCompare_T declaration and default implementation

template <class TT>
typename EnableIfC<has_operator_less<TT>::value,int>::type
sCompare_T(const TT& iL, const TT& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

// =================================================================================================
#pragma mark - sCompare_T default implementation for types without an accesible operator<

template <class TT>
typename EnableIfC<not has_operator_less<TT>::value,int>::type
sCompare_T(const TT& iL, const TT& iR)
	{
	ZUnimplemented();
	}

// =================================================================================================
#pragma mark - sCompare_T specialized for small integers

template <>
inline int sCompare_T(const bool& iL, const bool& iR)
	{ return int(iL) - int(iR); }

template <>
inline int sCompare_T(const char& iL, const char& iR)
	{ return iL - iR; }

template <>
inline int sCompare_T(const unsigned char& iL, const unsigned char& iR)
	{ return int(iL) - int(iR); }

template <>
inline int sCompare_T(const signed char& iL, const signed char& iR)
	{ return iL - iR; }

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
