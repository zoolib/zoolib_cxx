// Copyright (c) 2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_map_base_h__
#define __ZooLib_Util_STL_map_base_h__ 1
#include "zconfig.h"

#include "zoolib/Util_STL.h"

namespace ZooLib {
namespace Util_STL {

// =================================================================================================

//template <typename Container>
//EnableIf_t<is_member_function_pointer<decltype(&Container::end)>::value,
//	EnableIf_t<is_member_function_pointer<decltype(&Container::erase)>::value,
//		EnableIf_t<IsAType<typename Container::iterator>::value,
//			typename Container::Iterator>>>
//sEraseInc(Container& ioContainer, typename Container::iterator iter)
//	{
//	ZAssert(ioContainer.end() != iter);
//	return ioContainer.erase(iter);
//	}


// =================================================================================================
#pragma mark - Util_STL

template <typename Map_p>
struct IsAMap
	{
	enum { value = AreTypes<
		typename Map_p::key_type,
		typename Map_p::mapped_type,
		typename Map_p::value_type>::value
		};
	};

// -----

//template <class Map_p, typename Key>
//EnableIf_t<IsAMap<Map_p>::value,
//	bool>
//sContains(const Map_p& iMap, const Key& iKey)
//	{ return iMap.end() != iMap.find(iKey); }
//
//// -----
//
//template <class Map_p, typename Key>
//EnableIf_t<IsAMap<Map_p>::value,
//	bool>
//sQErase(Map_p& ioMap, const Key& iKey)
//	{ return ioMap.erase(iKey); }

//sErase(Map_p& ioMap, const Key& iKey)
//sEraseInc(Map_p& ioMap, typename Map_p::iterator iter)

// -----

template <class Map_p, typename Key>
EnableIf_t<IsAMap<Map_p>::value,
	const typename Map_p::mapped_type*>
sPGet(const Map_p& iMap,const typename Map_p::key_type& iKey)
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


} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_map_base_h__
