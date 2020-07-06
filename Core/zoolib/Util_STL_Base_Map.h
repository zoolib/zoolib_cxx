// Copyright (c) 2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_Base_Map_h__
#define __ZooLib_Util_STL_Base_Map_h__ 1
#include "zconfig.h"

#include "zoolib/Default.h"
#include "zoolib/Util_STL.h"
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

// -----

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
const typename Map_p::mapped_type&>
sDGet(const typename Map_p::mapped_type& iDefault,
	const Map_p& iMap, const typename Map_p::key_type& iKey)
	{
	if (const typename Map_p::mapped_type* theP = sPGet(iMap, iKey))
		return *theP;
	return iDefault;
	}

template <class Map_p>
	EnableIf_t<IsAMap<Map_p>::value,
typename Map_p::mapped_type>
sDGet(const typename Map_p::mapped_type&& iDefault,
	const Map_p& iMap, const typename Map_p::key_type& iKey)
	{
	if (const typename Map_p::mapped_type* theP = sPGet(iMap, iKey))
		return *theP;
	return std::move(iDefault);
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


} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_Base_Map_h__
