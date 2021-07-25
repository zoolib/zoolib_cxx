// Copyright (c) 2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_Base_Map_h__
#define __ZooLib_Util_STL_Base_Map_h__ 1
#include "zconfig.h"

#include "zoolib/Default.h"
#include "zoolib/Util_STL.h"
#include "zoolib/Util_STL_Base_MapOrSet.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
#pragma mark - Util_STL

template <typename Map_p,
	typename key_type = typename Map_p::key_type,
	typename mapped_type = typename Map_p::mapped_type,
	typename value_type = typename Map_p::value_type
	>
struct IsAMap : std::false_type
	{};

template <typename Map_p>
struct IsAMap<Map_p,
	typename Map_p::key_type,
	typename Map_p::mapped_type,
	typename Map_p::value_type> : std::true_type
	{};

// -------------------------------------------------------------------------------------------------
#pragma mark - Get

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
const typename Map_p::mapped_type*>
sPGet(const Map_p& iMap, const typename Map_p::key_type& iKey)
	{
	typename Map_p::const_iterator ii = iMap.find(iKey);
	if (iMap.end() == ii)
		return nullptr;
	return &ii->second;
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
const ZQ<typename Map_p::mapped_type>>
sQGet(const Map_p& iMap, const typename Map_p::key_type& iKey)
	{
	if (const typename Map_p::mapped_type* theP = sPGet(iMap, iKey))
		return *theP;
	return null;
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
const typename Map_p::mapped_type&>
sGet(const Map_p& iMap, const typename Map_p::key_type& iKey)
	{
	if (const typename Map_p::mapped_type* theP = sPGet(iMap, iKey))
		return *theP;
	return sDefault<typename Map_p::mapped_type>();
	}

// -------------------------------------------------------------------------------------------------
#pragma mark - Mut

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
typename Map_p::mapped_type*>
sPMut(Map_p& iMap, const typename Map_p::key_type& iKey)
	{
	typename Map_p::iterator ii = iMap.find(iKey);
	if (iMap.end() == ii)
		return nullptr;
	return &ii->second;
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
typename Map_p::mapped_type&>
sMut(Map_p& iMap, const typename Map_p::key_type& iKey)
	{ return iMap[iKey]; }

// -------------------------------------------------------------------------------------------------
#pragma mark - Set

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
void>
sSet(Map_p& ioMap, const typename Map_p::key_type& iKey,
	const typename Map_p::mapped_type& iValue)
	{ ioMap[iKey] = iValue; }

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
void>
sSet(Map_p& ioMap, const typename Map_p::key_type& iKey,
	typename Map_p::mapped_type&& rValue)
	{ ioMap[iKey] = std::move(rValue); }

// -------------------------------------------------------------------------------------------------
#pragma mark - QInsert

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
bool>
sQInsert(Map_p& ioMap, const typename Map_p::key_type& iKey,
	const typename Map_p::mapped_type& iValue)
	{
	std::pair<typename Map_p::iterator, bool> ii =
		ioMap.insert(typename Map_p::value_type(iKey, iValue));
	return ii.second;
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
bool>
sQInsert(Map_p& ioMap, const typename Map_p::key_type& iKey,
	typename Map_p::mapped_type&& rValue)
	{
	std::pair<typename Map_p::iterator, bool> ii =
		ioMap.insert(typename Map_p::value_type(iKey, std::move(rValue)));
	return ii.second;
	}

// -------------------------------------------------------------------------------------------------
#pragma mark - QReplace

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
bool>
sQReplace(Map_p& ioMap, const typename Map_p::key_type& iKey,
	const typename Map_p::mapped_type& iValue)
	{
	typename Map_p::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return false;
	ii->second = iValue;
	return true;
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
bool>
sQReplace(Map_p& ioMap, const typename Map_p::key_type& iKey,
	typename Map_p::mapped_type&& rValue)
	{
	typename Map_p::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return false;
	ii->second = std::move(rValue);
	return true;
	}

// -------------------------------------------------------------------------------------------------
#pragma mark - GetErase

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
ZQ<typename Map_p::mapped_type>>
sQGetErase(Map_p& ioMap, const typename Map_p::key_type& iKey)
	{
	typename Map_p::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return null;
	const typename Map_p::mapped_type result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
typename Map_p::mapped_type>
sGetErase(Map_p& ioMap, const typename Map_p::key_type& iKey)
	{
	typename Map_p::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return sDefault<Map_p::mapped_type>();
	const typename Map_p::mapped_type result = ii->second;
	ioMap.erase(ii);
	return result;
	}

// -------------------------------------------------------------------------------------------------
#pragma mark - QPopFront

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
ZQ<typename Map_p::value_type>>
sQPopFront(Map_p& ioMap)
	{
	if (ioMap.empty())
		return null;
	const typename Map_p::value_type result = *ioMap.begin();
	ioMap.erase(ioMap.begin());
	return result;
	}

// =================================================================================================
#pragma mark - sXXXMust

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
typename Map_p::mapped_type>
sGetMust(const int iDebugLevel,
	const Map_p& iMap, const typename Map_p::key_type& iKey)
	{
	const typename Map_p::mapped_type* theP = sPGet(iMap, iKey);
	ZAssertStop(iDebugLevel, theP);
	return *theP;
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
typename Map_p::mapped_type>
sGetMust(const Map_p& iMap, const typename Map_p::key_type& iKey)
	{ return sGetMust(1, iMap, iKey); }

// -------------------------------------------------------------------------------------------------

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
void>
sInsertMust(const int iDebugLevel,
	Map_p& ioMap, const typename Map_p::key_type& iKey,
	const typename Map_p::mapped_type& iValue)
	{
	const bool result = sQInsert(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
void>
sInsertMust(
	Map_p& ioMap,
	const typename Map_p::key_type& iKey,
	const typename Map_p::mapped_type& iValue)
	{ sInsertMust(1, ioMap, iKey, iValue); }

// -----

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
void>
sInsertMust(const int iDebugLevel,
	Map_p& ioMap,
	const typename Map_p::key_type& iKey,
	typename Map_p::mapped_type&& rValue)
	{
	const bool result = sQInsert(ioMap, iKey, std::move(rValue));
	ZAssertStop(iDebugLevel, result);
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
void>
sInsertMust(
	Map_p& ioMap,
	const typename Map_p::key_type& iKey,
	typename Map_p::mapped_type&& rValue)
	{ sInsertMust(1, ioMap, iKey, std::move(rValue)); }

// -----

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
void>
sInsertMust(const int iDebugLevel,
	Map_p& ioMap,
	const typename Map_p::iterator& iIter,
	const typename Map_p::key_type& iKey,
	const typename Map_p::mapped_type& iValue)
	{
	const typename Map_p::iterator result =
		ioMap.insert(iIter, typename Map_p::value_type(iKey, iValue));

	ZAssertStop(iDebugLevel, result->second == iValue); // ??? Not sure about this
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
void>
sInsertMust(
	Map_p& ioMap,
	const typename Map_p::iterator& iIter,
	const typename Map_p::key_type& iKey,
	const typename Map_p::mapped_type& iValue)
	{ sInsertMust(1, ioMap, iIter, iKey, iValue); }

// -------------------------------------------------------------------------------------------------

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
bool>
sQReplaceMust(const int iDebugLevel,
	Map_p& ioMap,
	const typename Map_p::key_type& iKey,
	const typename Map_p::mapped_type& iValue)
	{
	const bool result = sQReplace(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
bool>
sQReplaceMust(
	Map_p& ioMap,
	const typename Map_p::key_type& iKey,
	const typename Map_p::mapped_type& iValue)
	{ sQReplaceMust(ioMap, iKey, iValue); }

// -----

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
bool>
sQReplaceMust(const int iDebugLevel,
	Map_p& ioMap,
	const typename Map_p::key_type& iKey,
	typename Map_p::mapped_type&& rValue)
	{
	const bool result = sQReplace(ioMap, iKey, std::move(rValue));
	ZAssertStop(iDebugLevel, result);
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
bool>
sQReplaceMust(
	Map_p& ioMap,
	const typename Map_p::key_type& iKey,
	typename Map_p::mapped_type&& rValue)
	{ sQReplaceMust(ioMap, iKey, std::move(rValue)); }

// -------------------------------------------------------------------------------------------------

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
typename Map_p::mapped_type>
sGetEraseMust(const int iDebugLevel,
	Map_p& ioMap, const typename Map_p::key_type& iKey)
	{
	typename Map_p::iterator ii = ioMap.find(iKey);
	ZAssertStop(iDebugLevel, ioMap.end() != ii);
	const typename Map_p::mapped_type result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
typename Map_p::mapped_type>
sGetEraseMust(Map_p& ioMap, const typename Map_p::key_type& iKey)
	{ return sGetEraseMust(1, ioMap, iKey); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_Base_Map_h__
