// Copyright (c) 2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_Base_Set_h__
#define __ZooLib_Util_STL_Base_Set_h__ 1
#include "zconfig.h"

#include "zoolib/Util_STL.h"
#include "zoolib/Util_STL_Base_MapOrSet.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
#pragma mark - Util_STL

template <typename Set_p,
	typename key_type = typename Set_p::key_type,
	typename value_type = typename Set_p::value_type
	>
struct IsASet : std::false_type
	{};

template <typename Set_p>
struct IsASet<Set_p,
	typename Set_p::key_type,
	typename Set_p::value_type> : std::true_type
	{};

// -------------------------------------------------------------------------------------------------

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
ZQ<typename Set_p::key_type>>
sQPopFront(Set_p& ioSet)
	{
	if (ioSet.empty())
		return null;
	const typename Set_p::key_type result = *ioSet.begin();
	ioSet.erase(ioSet.begin());
	return result;
	}

// -------------------------------------------------------------------------------------------------

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
bool>
sQInsert(Set_p& ioSet, const typename Set_p::key_type& iElement)
	{ return ioSet.insert(iElement).second; }

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
bool>
sQInsert(Set_p& ioSet, typename Set_p::key_type&& rElement)
	{ return ioSet.insert(std::move(rElement)).second; }

// -------------------------------------------------------------------------------------------------

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
void>
sInsert(Set_p& ioSet, const typename Set_p::key_type& iElement)
	{ ioSet.insert(iElement); }

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
void>
sInsert(Set_p& ioSet, typename Set_p::key_type&& rElement)
	{ ioSet.insert(std::move(rElement)); }

// =================================================================================================
#pragma mark - sXXXMust

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
void>
sInsertMust(const int iDebugLevel,
	Set_p& ioSet, const typename Set_p::key_type& iElement)
	{
	const bool result = sQInsert(ioSet, iElement);
	ZAssertStop(iDebugLevel, result);
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
void>
sInsertMust(
	Set_p& ioSet, const typename Set_p::key_type& iElement)
	{ sInsertMust(1, ioSet, iElement); }

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
void>
sInsertMust(const int iDebugLevel,
	Set_p& ioSet, typename Set_p::key_type&& rElement)
	{
	const bool result = sQInsert(ioSet, std::move(rElement));
	ZAssertStop(iDebugLevel, result);
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
void>
sInsertMust(
	Set_p& ioSet, typename Set_p::key_type&& rElement)
	{ sInsertMust(1, ioSet, std::move(rElement)); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_Base_Set_h__
