// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_PullPush_JSON_h__
#define __ZooLib_PullPush_JSON_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/PullPush.h"
#include "zoolib/Util_Chan_JSON.h" // For TextOptions etc.

namespace ZooLib {

// =================================================================================================
#pragma mark - 

bool sPull_JSON_Push_PPT(const ChanRU_UTF& iChanRU,
	const Util_Chan_JSON::PullTextOptions_JSON& iRO,
	const ChanW_PPT& iChanW);

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR, const ChanW_UTF& iChanW);

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR,
	size_t iInitialIndent, const Util_Chan_JSON::PushTextOptions_JSON& iOptions,
	const ChanW_UTF& iChanW);

} // namespace ZooLib

#endif // __ZooLib_PullPush_JSON_h__
