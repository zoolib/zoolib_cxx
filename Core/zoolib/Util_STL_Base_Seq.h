// Copyright (c) 2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_Base_Seq_h__
#define __ZooLib_Util_STL_Base_Seq_h__ 1
#include "zconfig.h"

#include "zoolib/Util_STL.h"

#include "zoolib/ZQ.h"

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
// sPushBack

template <typename CC, typename VV>
	EnableIf_t<IsMFP<decltype(static_cast<void(CC::*)(const typename CC::value_type&)>
		(&CC::push_back))>::value,
void>
sPushBack(CC& ioContainer, const VV& iValue)
	{ ioContainer.push_back(iValue); }

// =================================================================================================
// sQPopBack

template <typename CC>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::value_type&(CC::*)()>
		(&CC::back))>::value,
		EnableIf_t<IsMFP<decltype(static_cast<void(CC::*)()>
			(&CC::pop_back))>::value,
ZQ<typename CC::value_type>>>
sQPopBack(CC& ioContainer)
	{
	if (ioContainer.empty())
		return null;

	const typename CC::value_type result = std::move(ioContainer.back());
	ioContainer.pop_back();
	return result;
	}

// =================================================================================================
// sPushFront

template <typename CC, typename VV>
	EnableIf_t<IsMFP<decltype(static_cast<void(CC::*)(const typename CC::value_type&)>
		(&CC::push_front))>::value,
void>
sPushFront(CC& ioContainer, const VV& iValue)
	{ ioContainer.push_front(iValue); }

// =================================================================================================
// sQPopFront

template <typename CC>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::value_type&(CC::*)()>
		(&CC::front))>::value,
		EnableIf_t<IsMFP<decltype(static_cast<void(CC::*)()>
			(&CC::pop_front))>::value,
ZQ<typename CC::value_type>>>
sQPopFront(CC& ioContainer)
	{
	if (ioContainer.empty())
		return null;

	const typename CC::value_type result = std::move(ioContainer.front());
	ioContainer.pop_front();
	return result;
	}

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_Base_Seq_h__
