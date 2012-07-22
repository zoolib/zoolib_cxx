/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZUtil_STL_vector_h__
#define __ZUtil_STL_vector_h__
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h" // find
#include "zoolib/ZDebug.h"

#include <vector>

namespace ZooLib {
namespace ZUtil_STL {

// =================================================================================================
// MARK: - ZUtil_STL

template <class T>
T* sFirstOrNil(std::vector<T>& iVec)
	{ return iVec.empty() ? nullptr : &iVec[0]; }

template <class T>
const T* sFirstOrNil(const std::vector<T>& iVec)
	{ return iVec.empty() ? nullptr : &iVec[0]; }

// -----

template <class T>
bool sIsEmpty(const std::vector<T>& iVec)
	{ return iVec.empty(); }

template <class T>
bool sNotEmpty(const std::vector<T>& iVec)
	{ return not sIsEmpty(iVec); }

// -----

// =================================================================================================
// MARK: - ZUtil_STL, unsorted vectors

template <typename Base, typename Derived>
bool sContains(const std::vector<Base>& iVec, const Derived& iElement)
	{ return iVec.end() != std::find(iVec.begin(), iVec.end(), iElement); }

template <typename Base, typename Derived>
bool sQErase(std::vector<Base>& ioVec, const Derived& iElement)
	{
	typename std::vector<Base>::iterator ii = std::find(ioVec.begin(), ioVec.end(), iElement);
	if (ii == ioVec.end())
		return false;
	ioVec.erase(ii);
	return true;
	}

template <typename Base, typename Derived>
void sErase(std::vector<Base>& ioVec, const Derived& iElement)
	{ sQErase(ioVec, iElement); }

template <typename Base, typename Derived>
bool sQPushBack(std::vector<Base>& ioVec, const Derived& iElement)
	{
	if (ioVec.end() != std::find(ioVec.begin(), ioVec.end(), iElement))
		return false;
	ioVec.push_back(iElement);
	return true;
	}

template <typename Base, typename Derived>
void sPushBack(std::vector<Base>& ioVec, const Derived& iElement)
	{ ioVec.push_back(iElement); }

// =================================================================================================
// MARK: - ZUtil_STL, sXXXMust

template <typename Base, typename Derived>
void sEraseMust(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	const bool result = sQErase(ioVec, iElement);
	ZAssertStop(iDebugLevel, result);
	}

template <typename Base, typename Derived>
void sEraseMust(std::vector<Base>& ioVec, const Derived& iElement)
	{ sEraseMust(1, ioVec, iElement); }

template <typename Base, typename Derived>
void sPushBackMust(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	const bool result = sQPushBack(ioVec, iElement);
	ZAssertStop(iDebugLevel, result);
	}

template <typename Base, typename Derived>
void sPushBackMust(std::vector<Base>& ioVec, const Derived& iElement)
	{ sPushBackMust(1, ioVec, iElement); }

// =================================================================================================
// MARK: - ZUtil_STL, vectors sorted by less<Base>

template <typename Base, typename Derived>
bool sQContainsSorted(const std::vector<Base>& iVec, const Derived& iElement)
	{
	typename std::vector<Base>::const_iterator ii =
		lower_bound(iVec.begin(), iVec.end(), iElement);
	return ii != iVec.end() && *ii == iElement;
	}

template <typename Base, typename Derived>
bool sQEraseSorted(std::vector<Base>& ioVec, const Derived& iElement)
	{
	typename std::vector<Base>::iterator ii = lower_bound(ioVec.begin(), ioVec.end(), iElement);

	if (ii == ioVec.end() || not (*ii == iElement))
		return false;

	ioVec.erase(ii);
	return true;
	}

template <typename Base, typename Derived>
bool sQInsertSorted(std::vector<Base>& ioVec, const Derived& iElement)
	{
	typename std::vector<Base>::iterator ii = lower_bound(ioVec.begin(), ioVec.end(), iElement);

	if (ii != ioVec.end() && *ii == iElement)
		return false;

	ioVec.insert(ii, iElement);
	return true;
	}

// =================================================================================================
// MARK: - ZUtil_STL, sXXXMust

template <typename Base, typename Derived>
void sEraseSortedMust(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	const bool result = sQEraseSorted(ioVec, iElement);
	ZAssertStop(iDebugLevel, result);
	}

template <typename Base, typename Derived>
void sEraseSortedMust(std::vector<Base>& ioVec, const Derived& iElement)
	{ sEraseSortedMust(1, ioVec, iElement); }

template <typename Base, typename Derived>
void sInsertSortedMust(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	const bool result = sQInsertSorted(ioVec, iElement);
	ZAssertStop(iDebugLevel, result);
	}

template <typename Base, typename Derived>
void sInsertSortedMust(std::vector<Base>& ioVec, const Derived& iElement)
	{ sInsertSortedMust(1, ioVec, iElement); }

} // namespace ZUtil_STL
} // namespace ZooLib

#endif // __ZUtil_STL_vector_h__
