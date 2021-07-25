// Copyright (c) 2020-2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_Base_MapOrSet_h__
#define __ZooLib_Util_STL_Base_MapOrSet_h__ 1
#include "zconfig.h"

#include "zoolib/Util_STL.h"

#include "zoolib/ZDebug.h"

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
// sEraseMust(key)

template <typename CC, typename KK>
auto sEraseMust(const int iDebugLevel, CC& ioContainer, KK iKey)
-> decltype(void(ioContainer.erase(iKey)))
	{
	bool result = ioContainer.erase(iKey);
	ZAssertStop(iDebugLevel, result);
	}

template <typename CC, typename KK>
auto sEraseMust(CC& ioContainer, KK iKey)
-> decltype(sEraseMust(1, ioContainer, iKey))
	{ sEraseMust(1, ioContainer, iKey); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_Base_MapOrSet_h__
