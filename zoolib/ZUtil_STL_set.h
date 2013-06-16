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

#ifndef __ZUtil_STL_set_h__
#define __ZUtil_STL_set_h__
#include "zconfig.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

#include <iterator> // For inserter
#include <set>

namespace ZooLib {
namespace ZUtil_STL {

// =================================================================================================
// MARK: - ZUtil_STL

template <typename T, typename Comparator>
bool
sIsEmpty(const std::set<T,Comparator>& iSet)
	{ return iSet.empty(); }

template <typename T, typename Comparator>
bool
sNotEmpty(const std::set<T,Comparator>& iSet)
	{ return not iSet.empty(); }

// -----

template <typename Base, typename Comparator, typename Derived>
bool sContains(const std::set<Base,Comparator>& iSet, const Derived& iElement)
	{ return iSet.end() != iSet.find(iElement); }

// -----

template <typename Base, typename Comparator, typename Derived>
bool sQErase(std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{ return ioSet.erase(iElement); }

template <typename Base, typename Comparator, typename Derived>
void sErase(std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{ ioSet.erase(iElement); }

template <typename T, typename Comparator>
typename std::set<T,Comparator>::iterator
sEraseInc(std::set<T,Comparator>& ioSet, typename std::set<T,Comparator>::iterator iter)
	{
	if (ioSet.end() == iter)
		return iter;

	const T theVal = *iter;
	ioSet.erase(iter);
	return ioSet.lower_bound(theVal);
	}

// -----

template <typename Base, typename Comparator, typename Derived>
bool sQInsert(std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{ return ioSet.insert(iElement).second; }

template <typename Base, typename Comparator, typename Derived>
void sInsert(std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{ ioSet.insert(iElement); }

// -----

template <typename Base, typename Comparator>
ZQ<Base> sQPopFront(std::set<Base,Comparator>& ioSet)
	{
	if (ioSet.empty())
		return null;
	const Base result = *ioSet.begin();
	ioSet.erase(ioSet.begin());
	return result;
	}

// =================================================================================================
// MARK: - sXXXMust

template <typename Base, typename Comparator, typename Derived>
void sInsertMust(const int iDebugLevel, std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{
	const bool result = sQInsert(ioSet, iElement);
	ZAssertStop(iDebugLevel, result);
	}

template <typename Base, typename Comparator, typename Derived>
void sInsertMust(std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{ sInsertMust(1, ioSet, iElement); }

// -----

template <typename Base, typename Comparator, typename Derived>
void sEraseMust(const int iDebugLevel, std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{
	const bool result = sQErase(ioSet, iElement);
	ZAssertStop(iDebugLevel, result);
	}

template <typename Base, typename Comparator, typename Derived>
void sEraseMust(std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{ sEraseMust(1, ioSet, iElement); }

// =================================================================================================
// MARK: - ZUtil_STL

template <typename T, typename Comparator>
void sOr(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS,
	std::set<T,Comparator>& ioResult)
	{
	set_union(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(ioResult, ioResult.end()));
	}

template <typename T, typename Comparator>
std::set<T,Comparator> sOr(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS)
	{
	std::set<T,Comparator> result;
	sOr(iLHS, iRHS, result);
	return result;
	}

template <typename T, typename Comparator>
void sAnd(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS,
	std::set<T,Comparator>& ioResult)
	{
	set_intersection(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(ioResult, ioResult.end()));
	}

template <typename T, typename Comparator>
std::set<T,Comparator> sAnd(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS)
	{
	std::set<T,Comparator> result;
	sAnd(iLHS, iRHS, result);
	return result;
	}

template <typename T, typename Comparator>
void sMinus(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS,
	std::set<T,Comparator>& ioResult)
	{
	set_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(ioResult, ioResult.end()));
	}

template <typename T, typename Comparator>
std::set<T,Comparator> sMinus(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS)
	{
	std::set<T,Comparator> result;
	sMinus(iLHS, iRHS, result);
	return result;
	}

template <typename T, typename Comparator>
void sXor(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS,
	std::set<T,Comparator>& ioResult)
	{
	set_symmetric_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(ioResult, ioResult.end()));
	}

template <typename T, typename Comparator>
std::set<T,Comparator> sXor(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS)
	{
	std::set<T,Comparator> result;
	sXor(iLHS, iRHS, result);
	return result;
	}

template <typename T, typename Comparator>
bool sIncludes(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS)
	{
	return includes(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end());
	}

} // namespace ZUtil_STL
} // namespace ZooLib

#endif // __ZUtil_STL_set_h__
