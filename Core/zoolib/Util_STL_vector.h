// Copyright (c) 2011-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_vector_h__
#define __ZooLib_Util_STL_vector_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h" // find
#include "zoolib/Util_STL_Base_Seq.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - sPaC for vectors

template <class EE>
PaC<EE> sPaC(std::vector<EE>& ioVector)
	{ return PaC<EE>(ioVector.size() ? &ioVector[0] : nullptr, ioVector.size()); }

template <class EE>
PaC<const EE> sPaC(const std::vector<EE>& iVector)
	{ return PaC<const EE>(iVector.size() ? &iVector[0] : nullptr, iVector.size()); }

namespace Util_STL {

// =================================================================================================
#pragma mark - Util_STL

template <typename PP>
auto sFirstOrNil(PP& iParam) -> decltype(sFrontOrNullPtr(iParam))
	{ return sFrontOrNullPtr(iParam); }

template <typename PP>
auto sFirstOrNil(const PP& iParam) -> decltype(sFrontOrNullPtr(iParam))
	{ return sFrontOrNullPtr(iParam); }

// =================================================================================================
#pragma mark - Util_STL, unsorted vectors

template <typename Base, typename Derived>
bool sContains(const std::vector<Base>& iVec, const Derived& iElement)
	{ return iVec.end() != std::find(iVec.begin(), iVec.end(), iElement); }

// -----

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

// -----

template <typename Base, typename Derived>
bool sQPushBack(std::vector<Base>& ioVec, const Derived& iElement)
	{
	if (ioVec.end() != std::find(ioVec.begin(), ioVec.end(), iElement))
		return false;
	ioVec.push_back(iElement);
	return true;
	}

// =================================================================================================
#pragma mark - Util_STL, sXXXMust

template <typename Base, typename Derived>
void sEraseMust(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	if (iDebugLevel > ZCONFIG_Debug)
		sErase(ioVec, iElement);
	else if (not sQErase(ioVec, iElement))
		ZDebugStop(iDebugLevel);
	}

template <typename Base, typename Derived>
void sEraseMust(std::vector<Base>& ioVec, const Derived& iElement)
	{ sEraseMust(1, ioVec, iElement); }

template <typename Base, typename Derived>
void sPushBackMust(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	if (iDebugLevel > ZCONFIG_Debug)
		sPushBack(ioVec, iElement);
	else if (not sQPushBack(ioVec, iElement))
		ZDebugStop(iDebugLevel);
	}

template <typename Base, typename Derived>
void sPushBackMust(std::vector<Base>& ioVec, const Derived& iElement)
	{ sPushBackMust(1, ioVec, iElement); }

// =================================================================================================
#pragma mark - Util_STL, vectors sorted by less<Base>

//template <typename Base, typename Derived>
//bool sQContainsSorted(const std::vector<Base>& iVec, const Derived& iElement)
//	{
//	typename std::vector<Base>::const_iterator ii =
//		lower_bound(iVec.begin(), iVec.end(), iElement);
//	return ii != iVec.end() && *ii == iElement;
//	}
//
//template <typename Base, typename Derived>
//bool sQEraseSorted(std::vector<Base>& ioVec, const Derived& iElement)
//	{
//	typename std::vector<Base>::iterator ii = lower_bound(ioVec.begin(), ioVec.end(), iElement);
//
//	if (ii == ioVec.end() || not (*ii == iElement))
//		return false;
//
//	ioVec.erase(ii);
//	return true;
//	}
//
//template <typename Base, typename Derived>
//bool sQInsertSorted(std::vector<Base>& ioVec, const Derived& iElement)
//	{
//	typename std::vector<Base>::iterator ii = lower_bound(ioVec.begin(), ioVec.end(), iElement);
//
//	if (ii != ioVec.end() && *ii == iElement)
//		return false;
//
//	ioVec.insert(ii, iElement);
//	return true;
//	}

template <typename Base, typename Derived>
ZQ<size_t> sQFindSorted(const std::vector<Base>& iVec, const Derived& iElement)
	{
	typename std::vector<Base>::const_iterator ii = lower_bound(iVec.begin(), iVec.end(), iElement);
	if (ii != iVec.end() && *ii == iElement)
		return ii - iVec.begin();
	return null;
	}

// =================================================================================================
#pragma mark - Util_STL, sXXXMust

//template <typename Base, typename Derived>
//void sEraseSortedMust(const int iDebugLevel,
//	std::vector<Base>& ioVec, const Derived& iElement)
//	{
//	if (iDebugLevel > ZCONFIG_Debug)
//		sQEraseSorted(ioVec, iElement);
//	else if (not sQEraseSorted(ioVec, iElement))
//		ZDebugStop(iDebugLevel);
//	}
//
//template <typename Base, typename Derived>
//void sEraseSortedMust(std::vector<Base>& ioVec, const Derived& iElement)
//	{ sEraseSortedMust(1, ioVec, iElement); }
//
//template <typename Base, typename Derived>
//void sInsertSortedMust(const int iDebugLevel,
//	std::vector<Base>& ioVec, const Derived& iElement)
//	{
//	if (iDebugLevel > ZCONFIG_Debug)
//		sQInsertSorted(ioVec, iElement);
//	else if (not sQInsertSorted(ioVec, iElement))
//		ZDebugStop(iDebugLevel);
//	}
//
//template <typename Base, typename Derived>
//void sInsertSortedMust(std::vector<Base>& ioVec, const Derived& iElement)
//	{ sInsertSortedMust(1, ioVec, iElement); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_vector_h__
