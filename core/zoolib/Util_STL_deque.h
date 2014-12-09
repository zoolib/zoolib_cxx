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

#ifndef __ZooLib_Util_STL_deque_h__
#define __ZooLib_Util_STL_deque_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h" // find
#include "zoolib/ZDebug.h"

#include <deque>

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
// MARK: - Util_STL

// -----

template <class T>
bool sIsEmpty(const std::deque<T>& iVec)
	{ return iVec.empty(); }

template <class T>
bool sNotEmpty(const std::deque<T>& iVec)
	{ return not sIsEmpty(iVec); }

// -----

// =================================================================================================
// MARK: - Util_STL

template <typename Base, typename Derived>
bool sContains(const std::deque<Base>& iVec, const Derived& iElement)
	{ return iVec.end() != std::find(iVec.begin(), iVec.end(), iElement); }

template <typename Base, typename Derived>
bool sQErase(std::deque<Base>& ioCont, const Derived& iElement)
	{
	typename std::deque<Base>::iterator ii = std::find(ioCont.begin(), ioCont.end(), iElement);
	if (ii == ioCont.end())
		return false;
	ioCont.erase(ii);
	return true;
	}

template <typename Base, typename Derived>
void sErase(std::deque<Base>& ioCont, const Derived& iElement)
	{ sQErase(ioCont, iElement); }

template <typename Base, typename Derived>
void sPushBack(std::deque<Base>& ioCont, const Derived& iElement)
	{ ioCont.push_back(iElement); }

template <typename Base, typename Derived>
bool sQPushBack(std::deque<Base>& ioCont, const Derived& iElement)
	{
	if (ioCont.end() != std::find(ioCont.begin(), ioCont.end(), iElement))
		return false;
	ioCont.push_back(iElement);
	return true;
	}

template <typename Base>
ZQ<Base> sQPopBack(std::deque<Base>& ioCont)
	{
	if (ioCont.empty())
		return null;

	const Base result = ioCont.back();
	ioCont.pop_back();
	return result;
	}

template <typename Base, typename Derived>
void sPushFront(std::deque<Base>& ioCont, const Derived& iElement)
	{ ioCont.push_front(iElement); }

template <typename Base, typename Derived>
bool sQPushFront(std::deque<Base>& ioCont, const Derived& iElement)
	{
	if (ioCont.end() != std::find(ioCont.begin(), ioCont.end(), iElement))
		return false;
	ioCont.push_back(iElement);
	return true;
	}

template <typename Base>
ZQ<Base> sQPopFront(std::deque<Base>& ioCont)
	{
	if (ioCont.empty())
		return null;

	const Base result = ioCont.front();
	ioCont.pop_front();
	return result;
	}

// =================================================================================================
// MARK: - Util_STL, sXXXMust

template <typename Base, typename Derived>
void sEraseMust(const int iDebugLevel,
	std::deque<Base>& ioCont, const Derived& iElement)
	{
	if (iDebugLevel > ZCONFIG_Debug)
		sErase(ioCont, iElement);
	else if (not sQErase(ioCont, iElement))
		ZDebugStop(iDebugLevel);
	}

template <typename Base, typename Derived>
void sEraseMust(std::deque<Base>& ioCont, const Derived& iElement)
	{ sEraseMust(1, ioCont, iElement); }

template <typename Base, typename Derived>
void sPushBackMust(const int iDebugLevel,
	std::deque<Base>& ioCont, const Derived& iElement)
	{
	if (iDebugLevel > ZCONFIG_Debug)
		sPushBack(ioCont, iElement);
	else if (not sQPushBack(ioCont, iElement))
		ZDebugStop(iDebugLevel);
	}

template <typename Base, typename Derived>
void sPushBackMust(std::deque<Base>& ioCont, const Derived& iElement)
	{ sPushBackMust(1, ioCont, iElement); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_deque_h__
