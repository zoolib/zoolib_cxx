// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_PullPush_JSON_h__
#define __ZooLib_PullPush_JSON_h__ 1
#include "zconfig.h"

#include "zoolib/PullPush.h"
#include "zoolib/Util_Chan_JSON.h" // For TextOptions etc.

namespace ZooLib {

// =================================================================================================
#pragma mark - sPull_JSON_Push_PPT

bool sPull_JSON_Push_PPT(const ChanRU_UTF& iChanRU,
	const Util_Chan_JSON::PullTextOptions_JSON& iOptions,
	const ChanW_PPT& iChanW);

// =================================================================================================
#pragma mark - sPull_PPT_Push_JSON

typedef Callable<bool(const PPT& iPPT, const ChanR_PPT& iChanR, const ChanW_UTF& iChanW)>
	Callable_JSON_WriteFilter;

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR, const ChanW_UTF& iChanW);

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR,
	size_t iIndent,
	const Util_Chan_JSON::PushTextOptions_JSON& iOptions,
	const ChanW_UTF& iChanW);

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR,
	size_t iIndent,
	const Util_Chan_JSON::PushTextOptions_JSON& iOptions,
	const ZP<Callable_JSON_WriteFilter>& iWriteFilter,
	const ChanW_UTF& iChanW);

// =================================================================================================
#pragma mark - sChannerR_PPT_xx

ZP<ChannerR_PPT> sChannerR_PPT_JSON(const ZP<Channer<ChanRU_UTF>>& iChanner,
	const Util_Chan_JSON::PullTextOptions_JSON& iOptions);

} // namespace ZooLib

#endif // __ZooLib_PullPush_JSON_h__
