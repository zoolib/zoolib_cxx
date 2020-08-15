// Copyright (c) 2011-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_map_h__
#define __ZooLib_Util_STL_map_h__ 1
#include "zconfig.h"

#include "zoolib/Util_STL_Base_Map.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

#include <map>

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
#pragma mark - Util_STL

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
bool sQInsert(
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	return ioMap.insert(typename std::map<KBase,Value,Comparator>::value_type(iKey, iValue)).second;
	}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
bool sQReplace(
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return false;
	ii->second = iValue;
	return true;
	}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
ZQ<Value> sQGetErase(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return null;
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value sDGetErase(
	const Value& iDefault, std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return iDefault;
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value sGetErase(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return sDefault<Value>();
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

// -----

template <typename KBase, typename Value, typename Comparator>
ZQ<std::pair<KBase,Value>> sQPopFront(std::map<KBase,Value,Comparator>& ioMap)
	{
	if (ioMap.empty())
		return null;
	const std::pair<KBase,Value> result = *ioMap.begin();
	ioMap.erase(ioMap.begin());
	return result;
	}

// =================================================================================================
#pragma mark - sXXXMust

template <typename KBase, typename Value, typename Comparator, typename KDerived>
void sEraseMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	const bool result = sQErase(ioMap, iKey);
	ZAssertStop(iDebugLevel, result);
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
void sEraseMust(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{ sEraseMust(1, ioMap, iKey); }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
const Value& sGetMust(const int iDebugLevel,
	const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	const Value* theP = sPGet(iMap, iKey);
	ZAssertStop(iDebugLevel, theP);
	return *theP;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
const Value& sGetMust(const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{ return sGetMust(1, iMap, iKey); }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sInsertMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	const bool result = sQInsert(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sInsertMust(
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ sInsertMust(1, ioMap, iKey, iValue);}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sInsertMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap,
	const typename std::map<KBase,Value,Comparator>::iterator& iIter,
	const KDerived& iKey, const VDerived& iValue)
	{
	const typename std::map<KBase,Value,Comparator>::iterator result =
	ioMap.insert(
		iIter,
		typename std::map<KBase,Value,Comparator>::value_type(iKey, iValue));

	ZAssertStop(iDebugLevel, result->second == iValue); // ??? Not sure about this
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sInsertMust(
	std::map<KBase,Value,Comparator>& ioMap,
	const typename std::map<KBase,Value,Comparator>::iterator& iIter,
	const KDerived& iKey, const VDerived& iValue)
	{ sInsertMust(1, ioMap, iIter, iKey, iValue);}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sReplaceMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const Value& iValue)
	{
	const bool result = sQReplace(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sReplaceMust(
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ sReplaceMust(1, ioMap, iKey, iValue); }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value sGetEraseMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	ZAssertStop(iDebugLevel, ioMap.end() != ii);
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value sGetEraseMust(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{ return sGetEraseMust(1, ioMap, iKey); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_map_h__
