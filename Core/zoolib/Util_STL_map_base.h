// Copyright (c) 2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_map_base_h__
#define __ZooLib_Util_STL_map_base_h__ 1
#include "zconfig.h"

#include "zoolib/Util_STL.h"

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
#pragma mark - Util_STL

template <typename Map_p>
struct IsAMap
	{
	enum { value = TrueIfAreTypes<
		typename Map_p::key_type,
		typename Map_p::mapped_type,
		typename Map_p::value_type>::value
		};
	};

// -----

template <class Map_p, typename Key>
EnableIf<IsAMap<Map_p>::value,
	bool>
sContains(const Map_p& iMap, const Key& iKey)
	{ return iMap.end() != iMap.find(iKey); }

// -----

template <class Map_p, typename Key>
EnableIf<IsAMap<Map_p>::value,
	bool>
sQErase(Map_p& ioMap, const Key& iKey)
	{ return ioMap.erase(iKey); }

//sErase(Map_p& ioMap, const Key& iKey)
//sEraseInc(Map_p& ioMap, typename Map_p::iterator iter)

// -----

template <class Map_p, typename Key>
EnableIf<IsAMap<Map_p>::value,
	const typename Map_p::mapped_type*>
sPGet(const Map_p& iMap,const typename Map_p::key_type& iKey)
	{
	typename Map_p::const_iterator ii = iMap.find(iKey);
	if (iMap.end() == ii)
		return nullptr;
	return &ii->second;
	}

template <class Map_p>
EnableIf<IsAMap<Map_p>::value,
	const typename Map_p::mapped_type&>
sDGet(const typename Map_p::mapped_type& iDefault,
	const Map_p& iMap, const typename Map_p::key_type& iKey)
	{
	if (const typename Map_p::mapped_type* theP = sPGet(iMap, iKey))
		return *theP;
	return iDefault;
	}

template <class Map_p>
EnableIf<IsAMap<Map_p>::value,
	typename Map_p::mapped_type>
sDGet(const typename Map_p::mapped_type&& iDefault,
	const Map_p& iMap, const typename Map_p::key_type& iKey)
	{
	if (const typename Map_p::mapped_type* theP = sPGet(iMap, iKey))
		return *theP;
	return iDefault;
	}

template <class Map_p, typename Key>
const ZQ<Value> sQGet(const Map_p& iMap, const KDerived& iKey)
	{
	if (const Value* theP = sPGet(iMap, iKey))
		return *theP;
	return null;
	}

template <class Map_p, typename Key>
const Value& sGet(const Map_p& iMap, const KDerived& iKey)
	{
	if (const Value* theP = sPGet(iMap, iKey))
		return *theP;
	return sDefault<Value>();
	}

// -----

template <class Map_p, typename Key>
Value* sPMut(std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = iMap.find(iKey);
	if (iMap.end() == ii)
		return nullptr;
	return &ii->second;
	}

template <class Map_p, typename Key>
Value& sDMut(
	const Value& iDefault, Map_p& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii =
		ioMap.insert(typename std::map<KBase,Value,Comparator>::value_type(iKey, iDefault));
	return ii->first.second;
	}

template <class Map_p, typename Key>
Value& sMut(
	std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{ return iMap[iKey]; }

// -----

template <class Map_p, typename Key, typename Value>
void sSet(
	Map_p& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ ioMap[iKey] = iValue; }

// -----

template <class Map_p, typename Key, typename Value>
bool sQInsert(
	Map_p& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	return ioMap.insert(typename std::map<KBase,Value,Comparator>::value_type(iKey, iValue)).second;
	}

// -----

template <class Map_p, typename Key, typename Value>
bool sQReplace(
	Map_p& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return false;
	ii->second = iValue;
	return true;
	}

// -----

template <class Map_p, typename Key>
ZQ<Value> sQGetErase(Map_p& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return null;
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <class Map_p, typename Key>
Value sDGetErase(
	const Value& iDefault, Map_p& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return iDefault;
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <class Map_p, typename Key>
Value sGetErase(Map_p& ioMap, const KDerived& iKey)
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
ZQ<std::pair<KBase,Value>> sQPopFront(Map_p& ioMap)
	{
	if (ioMap.empty())
		return null;
	const std::pair<KBase,Value> result = *ioMap.begin();
	ioMap.erase(ioMap.begin());
	return result;
	}

// =================================================================================================
#pragma mark - sXXXMust

template <class Map_p, typename Key>
void sEraseMust(const int iDebugLevel,
	Map_p& ioMap, const KDerived& iKey)
	{
	const bool result = sQErase(ioMap, iKey);
	ZAssertStop(iDebugLevel, result);
	}

template <class Map_p, typename Key>
void sEraseMust(Map_p& ioMap, const KDerived& iKey)
	{ sEraseMust(1, ioMap, iKey); }

// -----

template <class Map_p, typename Key>
const Value& sGetMust(const int iDebugLevel,
	const Map_p& iMap, const KDerived& iKey)
	{
	const Value* theP = sPGet(iMap, iKey);
	ZAssertStop(iDebugLevel, theP);
	return *theP;
	}

template <class Map_p, typename Key>
const Value& sGetMust(const Map_p& iMap, const KDerived& iKey)
	{ return sGetMust(1, iMap, iKey); }

// -----

template <class Map_p, typename Key, typename Value>
void sInsertMust(const int iDebugLevel,
	Map_p& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	const bool result = sQInsert(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <class Map_p, typename Key, typename Value>
void sInsertMust(
	Map_p& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ sInsertMust(1, ioMap, iKey, iValue);}

// -----

template <class Map_p, typename Key, typename Value>
void sInsertMust(const int iDebugLevel,
	Map_p& ioMap,
	const typename std::map<KBase,Value,Comparator>::iterator& iIter,
	const KDerived& iKey, const VDerived& iValue)
	{
	const typename std::map<KBase,Value,Comparator>::iterator result =
	ioMap.insert(
		iIter,
		typename std::map<KBase,Value,Comparator>::value_type(iKey, iValue));

	ZAssertStop(iDebugLevel, result->second == iValue); // ??? Not sure about this
	}

template <class Map_p, typename Key, typename Value>
void sInsertMust(
	Map_p& ioMap,
	const typename std::map<KBase,Value,Comparator>::iterator& iIter,
	const KDerived& iKey, const VDerived& iValue)
	{ sInsertMust(1, ioMap, iIter, iKey, iValue);}

// -----

template <class Map_p, typename Key, typename Value>
void sReplaceMust(const int iDebugLevel,
	Map_p& ioMap, const KDerived& iKey, const Value& iValue)
	{
	const bool result = sQReplace(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <class Map_p, typename Key, typename Value>
void sReplaceMust(
	Map_p& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ sReplaceMust(1, ioMap, iKey, iValue); }

// -----

template <class Map_p, typename Key>
Value sGetEraseMust(const int iDebugLevel,
	Map_p& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	ZAssertStop(iDebugLevel, ioMap.end() != ii);
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <class Map_p, typename Key>
Value sGetEraseMust(Map_p& ioMap, const KDerived& iKey)
	{ return sGetEraseMust(1, ioMap, iKey); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_map_base_h__
