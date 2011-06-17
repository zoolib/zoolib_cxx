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

#ifndef __ZUtil_STL_set__
#define __ZUtil_STL_set__
#include "zconfig.h"

#include "zoolib/ZDebug.h"

#include <set>

namespace ZooLib {
namespace ZUtil_STL {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_STL

template <typename T, typename Comparator>
typename std::set<T,Comparator>::iterator
sEraseInc(std::set<T,Comparator>& ioSet, typename std::set<T,Comparator>::iterator iter)
	{
	if (ioSet.end() != iter)
		{
		const T theVal = *iter;
		ioSet.erase(iter);
		iter = ioSet.lower_bound(theVal);
		}
	return iter;
	}


/** Returns true if iSet contains iElement. */
template <typename Base, typename Comparator, typename Derived>
bool sContains(const std::set<Base,Comparator>& iSet, const Derived& iElement)
	{ return iSet.end() != iSet.find(iElement); }


/** Inserts iElement to ioSet. We first assert, controlled
by iDebugLevel, that iElement is not already present in ioSet. */
template <typename Base, typename Comparator, typename Derived>
void sInsertMustNotContain(const int iDebugLevel,
	std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{
	bool didInsert = ioSet.insert(iElement).second;
	ZAssertStop(iDebugLevel, didInsert);
	}


/** Inserts iElement in ioSet, if it's not already contained. */
template <typename Base, typename Comparator, typename Derived>
bool sInsertIfNotContains(std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{ return ioSet.insert(iElement).second; }


/** If ioSet contains iElement then it is removed and true returned.
Otherwise no change is made to ioSet and false is returned. */
template <typename Base, typename Comparator, typename Derived>
bool sEraseIfContains(std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{ return ioSet.erase(iElement); }


/** Removes iElement from ioSet, asserting that it is present. */
template <typename Base, typename Comparator, typename Derived>
void sEraseMustContain(const int iDebugLevel,
	std::set<Base,Comparator>& ioSet, const Derived& iElement)
	{
	size_t count = ioSet.erase(iElement);
	ZAssertStop(iDebugLevel, count);
	}

// ==================================================


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

#endif // __ZUtil_STL_set__
