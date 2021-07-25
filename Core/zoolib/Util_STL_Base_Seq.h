// Copyright (c) 2020-2021 Andrew Green. MIT License. http://www.zoolib.org

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
auto sPushBack(CC& ioContainer, VV&& rValue)
-> decltype(ioContainer.push_back(std::forward<VV>(rValue)),
	void())
	{ ioContainer.push_back(std::forward<VV>(rValue)); }

// =================================================================================================
// sQPopBack

template <typename CC>
auto sQPopBack(CC& ioContainer)
-> decltype(ioContainer.empty(), ioContainer.back(), ioContainer.pop_back(),
	ZQ<typename CC::value_type>())
	{
	if (ioContainer.empty())
		return null;

	const typename CC::value_type result(std::move(ioContainer.back()));
	ioContainer.pop_back();
	return result;
	}

// =================================================================================================
// sPushFront

template <typename CC, typename VV>
auto sPushBack(CC& ioContainer, VV&& rValue)
-> decltype(ioContainer.push_front(std::forward<VV>(rValue)),
	void())
	{ ioContainer.push_front(std::forward<VV>(rValue)); }

// =================================================================================================
// sQPopFront

template <typename CC>
auto sQPopFront(CC& ioContainer)
-> decltype(ioContainer.empty(), ioContainer.front(), ioContainer.pop_front(),
	ZQ<typename CC::value_type>())
	{
	if (ioContainer.empty())
		return null;

	const typename CC::value_type result(std::move(ioContainer.front()));
	ioContainer.pop_front();
	return result;
	}

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_Base_Seq_h__
