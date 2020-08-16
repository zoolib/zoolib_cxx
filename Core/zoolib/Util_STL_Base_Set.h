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

// =================================================================================================
#pragma mark - Set operations

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
bool>
sIncludes(
	const Set_p& iLHS,
	const Set_p& iRHS)
	{ return includes(iLHS.begin(), iLHS.end(), iRHS.begin(), iRHS.end()); }

// =================================================================================================
#pragma mark - Intersection

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator&(const Set_p& iLHS, const Set_p& iRHS)
	{
	Set_p result;
	set_intersection(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(result, result.end()));
	return result;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p&>
operator&=(Set_p& ioLHS, const Set_p& iRHS)
	{
	Set_p tmp;
	set_intersection(ioLHS.begin(), ioLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(tmp, tmp.end()));
	ioLHS.swap(tmp);
	return ioLHS;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator&(const Set_p& iLHS, const typename Set_p::key_type& iElem)
	{
	if (iLHS.find(iElem) != iLHS.end())
		return Set_p(&iElem, &iElem + 1);
	return Set_p();
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator&(const typename Set_p::key_type& iElem, const Set_p& iRHS)
	{ return iRHS & iElem; }

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p&>
operator&=(Set_p& ioLHS, const typename Set_p::key_type& iElem)
	{
	if (ioLHS.find(iElem) != ioLHS.end())
		{
		ioLHS.clear();
		ioLHS.insert(iElem);
		}
	else
		{
		ioLHS.clear();
		}
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Union

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator|(const Set_p& iLHS, const Set_p& iRHS)
	{
	Set_p result;
	set_union(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(result, result.end()));
	return result;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p&>
operator|=(Set_p& ioLHS, const Set_p& iRHS)
	{
	ioLHS.insert(iRHS.begin(), iRHS.end());
	return ioLHS;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator|(const Set_p& iLHS, const typename Set_p::key_type& iElem)
	{
	Set_p tmp = iLHS;
	tmp.insert(iElem);
	return tmp;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator|(const typename Set_p::key_type& iElem, const Set_p& iRHS)
	{ return iRHS | iElem; }

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p&>
operator|=(Set_p& ioLHS, const typename Set_p::key_type& iElem)
	{
	ioLHS.insert(iElem);
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Xor

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator^(const Set_p& iLHS, const Set_p& iRHS)
	{
	Set_p result;
	set_symmetric_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(result, result.end()));
	return result;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p&>
operator^=(Set_p& ioLHS, const Set_p& iRHS)
	{
	Set_p tmp;
	set_symmetric_difference(ioLHS.begin(), ioLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(tmp, tmp.end()));
	ioLHS.swap(tmp);
	return ioLHS;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator^(const Set_p& iLHS, const typename Set_p::key_type& iElem)
	{
	Set_p result;
	set_symmetric_difference(iLHS.begin(), iLHS.end(),
		&iElem, &iElem + 1,
		inserter(result, result.end()));
	return result;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator^(const typename Set_p::key_type& iElem, const Set_p& iRHS)
	{ return iRHS ^ iElem; }

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p&>
operator^=(Set_p& ioLHS, const typename Set_p::key_type& iElem)
	{
	typename Set_p::iterator iter = ioLHS.find(iElem);
	if (iter == ioLHS.end() || iElem != *iter)
		ioLHS.insert(iter, iElem);
	else
		ioLHS.erase(iter);
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Minus

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator-(const Set_p& iLHS, const Set_p& iRHS)
	{
	Set_p result;
	set_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(result, result.end()));
	return result;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p&>
operator-=(Set_p& ioLHS, const Set_p& iRHS)
	{
	for (typename Set_p::const_iterator ii = iRHS.begin(), end = iRHS.end();
		ii != end; ++ii)
		{ ioLHS.erase(*ii); }
	return ioLHS;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p>
operator-(const Set_p& iLHS, const typename Set_p::key_type& iElem)
	{
	Set_p result = iLHS;
	result.erase(iElem);
	return result;
	}

template <class Set_p>
	EnableIf_t<IsASet<Set_p>::value,
Set_p&>
operator-=(Set_p& ioLHS, const typename Set_p::key_type& iElem)
	{
	ioLHS.erase(iElem);
	return ioLHS;
	}

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_Base_Set_h__
