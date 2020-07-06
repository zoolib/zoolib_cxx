// Copyright (c) 2012-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_unordered_map_h__
#define __ZooLib_Util_STL_unordered_map_h__ 1
#include "zconfig.h"

#include "zoolib/Util_STL_Base_Map.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

#include "zoolib/Compat_unordered_map.h"

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
#pragma mark - Util_STL

template <typename KBase, typename Value, typename KDerived>
const Value* sPGet(const unordered_map<KBase,Value>& iMap, const KDerived& iKey)
	{
	typename unordered_map<KBase,Value>::const_iterator ii = iMap.find(iKey);
	if (iMap.end() == ii)
		return nullptr;
	return &ii->second;
	}

template <typename KBase, typename Value, typename KDerived>
const Value& sDGet(
	const Value& iDefault, const unordered_map<KBase,Value>& iMap, const KDerived& iKey)
	{
	if (const Value* theP = sPGet(iMap, iKey))
		return *theP;
	return iDefault;
	}

template <typename KBase, typename Value, typename KDerived>
const ZQ<Value> sQGet(const unordered_map<KBase,Value>& iMap, const KDerived& iKey)
	{
	if (const Value* theP = sPGet(iMap, iKey))
		return *theP;
	return null;
	}

template <typename KBase, typename Value, typename KDerived>
const Value& sGet(const unordered_map<KBase,Value>& iMap, const KDerived& iKey)
	{
	if (const Value* theP = sPGet(iMap, iKey))
		return *theP;
	return sDefault<Value>();
	}

// -----

template <typename KBase, typename Value, typename KDerived>
Value* sPMut(unordered_map<KBase,Value>& iMap, const KDerived& iKey)
	{
	typename unordered_map<KBase,Value>::iterator ii = iMap.find(iKey);
	if (iMap.end() == ii)
		return nullptr;
	return &ii->second;
	}

template <typename KBase, typename Value, typename KDerived>
Value& sDMut(
	const Value& iDefault, unordered_map<KBase,Value>& ioMap, const KDerived& iKey)
	{
	typename unordered_map<KBase,Value>::iterator ii =
		ioMap.insert(typename unordered_map<KBase,Value>::value_type(iKey, iDefault));
	return ii->first.second;
	}

template <typename KBase, typename Value, typename KDerived>
Value& sMut(
	unordered_map<KBase,Value>& iMap, const KDerived& iKey)
	{ return iMap[iKey]; }

// -----

template <typename KBase, typename Value, typename KDerived, typename VDerived>
void sSet(
	unordered_map<KBase,Value>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ ioMap[iKey] = iValue; }

// -----

template <typename KBase, typename Value, typename KDerived, typename VDerived>
bool sQInsert(
	unordered_map<KBase,Value>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	return ioMap.insert(typename unordered_map<KBase,Value>::value_type(iKey, iValue)).second;
	}

// -----

template <typename KBase, typename Value, typename KDerived, typename VDerived>
bool sQReplace(
	unordered_map<KBase,Value>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	typename unordered_map<KBase,Value>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return false;
	ii->second = iValue;
	return true;
	}

// -----

template <typename KBase, typename Value, typename KDerived>
ZQ<Value> sQGetErase(unordered_map<KBase,Value>& ioMap, const KDerived& iKey)
	{
	typename unordered_map<KBase,Value>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return null;
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <typename KBase, typename Value, typename KDerived>
Value sDGetErase(
	const Value& iDefault, unordered_map<KBase,Value>& ioMap, const KDerived& iKey)
	{
	typename unordered_map<KBase,Value>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return iDefault;
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <typename KBase, typename Value, typename KDerived>
Value sGetErase(unordered_map<KBase,Value>& ioMap, const KDerived& iKey)
	{
	typename unordered_map<KBase,Value>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return sDefault<Value>();
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

// =================================================================================================
#pragma mark - sXXXMust

template <typename KBase, typename Value, typename KDerived>
void sEraseMust(const int iDebugLevel,
	unordered_map<KBase,Value>& ioMap, const KDerived& iKey)
	{
	const bool result = sQErase(ioMap, iKey);
	ZAssertStop(iDebugLevel, result);
	}

template <typename KBase, typename Value, typename KDerived>
void sEraseMust(unordered_map<KBase,Value>& ioMap, const KDerived& iKey)
	{ sEraseMust(1, ioMap, iKey); }

// -----

template <typename KBase, typename Value, typename KDerived>
const Value& sGetMust(const int iDebugLevel,
	const unordered_map<KBase,Value>& iMap, const KDerived& iKey)
	{
	const Value* theP = sPGet(iMap, iKey);
	ZAssertStop(iDebugLevel, theP);
	return *theP;
	}

template <typename KBase, typename Value, typename KDerived>
const Value& sGetMust(const unordered_map<KBase,Value>& iMap, const KDerived& iKey)
	{ return sGetMust(1, iMap, iKey); }

// -----

template <typename KBase, typename Value, typename KDerived, typename VDerived>
void sInsertMust(const int iDebugLevel,
	unordered_map<KBase,Value>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	const bool result = sQInsert(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <typename KBase, typename Value, typename KDerived, typename VDerived>
void sInsertMust(
	unordered_map<KBase,Value>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ sInsertMust(1, ioMap, iKey, iValue);}

// -----

template <typename KBase, typename Value, typename KDerived, typename VDerived>
void sReplaceMust(const int iDebugLevel,
	unordered_map<KBase,Value>& ioMap, const KDerived& iKey, const Value& iValue)
	{
	const bool result = sQReplace(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <typename KBase, typename Value, typename KDerived, typename VDerived>
void sReplaceMust(
	unordered_map<KBase,Value>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ sReplaceMust(1, ioMap, iKey, iValue); }

// -----

template <typename KBase, typename Value, typename KDerived>
Value sGetEraseMust(const int iDebugLevel,
	unordered_map<KBase,Value>& ioMap, const KDerived& iKey)
	{
	typename unordered_map<KBase,Value>::iterator ii = ioMap.find(iKey);
	ZAssertStop(iDebugLevel, ioMap.end() != ii);
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <typename KBase, typename Value, typename KDerived>
Value sGetEraseMust(unordered_map<KBase,Value>& ioMap, const KDerived& iKey)
	{ return sGetEraseMust(1, ioMap, iKey); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_unordered_map_h__
