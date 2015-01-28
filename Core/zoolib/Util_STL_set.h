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

#ifndef __ZooLib_Util_STL_set_h__
#define __ZooLib_Util_STL_set_h__ 1
#include "zconfig.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

#include <iterator> // For inserter
#include <set>

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
#pragma mark -
#pragma mark Util_STL

template <typename T, typename Comparator>
bool
sIsEmpty(const std::set<T,Comparator>& iSet)
	{ return iSet.empty(); }

template <typename T, typename Comparator>
bool
sNotEmpty(const std::set<T,Comparator>& iSet)
	{ return not iSet.empty(); }

template <typename T, typename Comparator>
void
sClear(std::set<T,Comparator>& ioSet)
	{ ioSet.clear(); }

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
	if (ioSet.end() != iter)
		ioSet.erase(iter++);
	return iter;
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
#pragma mark -
#pragma mark sXXXMust

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
#pragma mark -
#pragma mark Intersection

template <class T, class C>
std::set<T,C> operator&(const std::set<T,C>& iLHS, const std::set<T,C>& iRHS)
	{
	std::set<T,C> result;
	set_intersection(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(result, result.end()));
	return result;
	}

template <class T, class C>
std::set<T,C>& operator&=(std::set<T,C>& ioLHS, const std::set<T,C>& iRHS)
	{
	std::set<T,C> tmp;
	set_intersection(ioLHS.begin(), ioLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(tmp, tmp.end()));
	ioLHS.swap(tmp);
	return ioLHS;
	}

template <class T, class C>
std::set<T,C> operator&(const std::set<T,C>& iLHS, const T& iElem)
	{
	if (iLHS.find(iElem) != iLHS.end())
		return std::set<T,C>(&iElem, &iElem + 1);
	return std::set<T,C>();
	}

template <class T, class C>
std::set<T,C> operator&(const T& iElem, const std::set<T,C>& iRHS)
	{ return iRHS & iElem; }

template <class T, class C>
std::set<T,C>& operator&=(std::set<T,C>& ioLHS, const T& iElem)
	{
	if (ioLHS.find(iElem) != ioLHS.end())
		{
		ioLHS.clear();
		ioLHS.insert(iElem);
		}
	else
		{
		ioLHS.clear();
		}
	return ioLHS;
	}

// =================================================================================================
#pragma mark -
#pragma mark Union

template <class T, class C>
std::set<T,C> operator|(const std::set<T,C>& iLHS, const std::set<T,C>& iRHS)
	{
	std::set<T,C> result;
	set_union(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(result, result.end()));
	return result;
	}

template <class T, class C>
std::set<T,C>& operator|=(std::set<T,C>& ioLHS, const std::set<T,C>& iRHS)
	{
	ioLHS.insert(iRHS.begin(), iRHS.end());
	return ioLHS;
	}

template <class T, class C>
std::set<T,C> operator|(const std::set<T,C>& iLHS, const T& iElem)
	{
	std::set<T,C> tmp = iLHS;
	tmp.insert(iElem);
	return tmp;
	}

template <class T, class C>
std::set<T,C> operator|(const T& iElem, const std::set<T,C>& iRHS)
	{ return iRHS | iElem; }

template <class T, class C>
std::set<T,C>& operator|=(std::set<T,C>& ioLHS, const T& iElem)
	{
	ioLHS.insert(iElem);
	return ioLHS;
	}

// =================================================================================================
#pragma mark -
#pragma mark Xor

template <class T, class C>
std::set<T,C> operator^(const std::set<T,C>& iLHS, const std::set<T,C>& iRHS)
	{
	std::set<T,C> result;
	set_symmetric_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(result, result.end()));
	return result;
	}

template <class T, class C>
std::set<T,C>& operator^=(std::set<T,C>& ioLHS, const std::set<T,C>& iRHS)
	{
	std::set<T,C> tmp;
	set_symmetric_difference(ioLHS.begin(), ioLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(tmp, tmp.end()));
	ioLHS.swap(tmp);
	return ioLHS;
	}

template <class T, class C>
std::set<T,C> operator^(const std::set<T,C>& iLHS, const T& iElem)
	{
	std::set<T,C> result;
	set_symmetric_difference(iLHS.begin(), iLHS.end(),
		&iElem, &iElem + 1,
		inserter(result, result.end()));
	return result;
	}

template <class T, class C>
std::set<T,C> operator^(const T& iElem, const std::set<T,C>& iRHS)
	{ return iRHS ^ iElem; }

template <class T, class C>
std::set<T,C>& operator^=(std::set<T,C>& ioLHS, const T& iElem)
	{
	typename std::set<T>::iterator iter = ioLHS.find(iElem);
	if (iter == ioLHS.end() || iElem != *iter)
		ioLHS.insert(iter, iElem);
	else
		ioLHS.erase(iter);
	return ioLHS;
	}

// =================================================================================================
#pragma mark -
#pragma mark Minus

template <class T, class C>
std::set<T,C> operator-(const std::set<T,C>& iLHS, const std::set<T,C>& iRHS)
	{
	std::set<T,C> result;
	set_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(result, result.end()));
	return result;
	}

template <class T, class C>
std::set<T,C>& operator-=(std::set<T,C>& ioLHS, const std::set<T,C>& iRHS)
	{
	for (typename std::set<T,C>::const_iterator ii = iRHS.begin(), end = iRHS.end();
		ii != end; ++ii)
		{ ioLHS.erase(*ii); }
	return ioLHS;
	}

template <class T, class C>
std::set<T,C> operator-(const std::set<T,C>& iLHS, const T& iElem)
	{
	std::set<T,C> result = iLHS;
	result.erase(iElem);
	return result;
	}

template <class T, class C>
std::set<T,C>& operator-=(std::set<T,C>& ioLHS, const T& iElem)
	{
	ioLHS.erase(iElem);
	return ioLHS;
	}

// =================================================================================================
#pragma mark -
#pragma mark 

template <typename T, typename Comparator>
bool sIncludes(
	const std::set<T,Comparator>& iLHS,
	const std::set<T,Comparator>& iRHS)
	{ return includes(iLHS.begin(), iLHS.end(), iRHS.begin(), iRHS.end()); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_set_h__
