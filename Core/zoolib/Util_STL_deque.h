// Copyright (c) 2012-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_deque_h__
#define __ZooLib_Util_STL_deque_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h" // find
#include "zoolib/Util_STL_Seq_Base.h"

#include "zoolib/ZDebug.h"

#include <deque>

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
#pragma mark - Util_STL

template <typename Base, typename Derived>
bool sContains(const std::deque<Base>& iCont, const Derived& iElement)
	{ return iCont.end() != std::find(iCont.begin(), iCont.end(), iElement); }

// -----

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

// -----

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

// -----

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
#pragma mark - Util_STL, sXXXMust

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
