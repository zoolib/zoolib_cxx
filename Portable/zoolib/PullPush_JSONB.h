// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_PullPush_JSONB_h__
#define __ZooLib_PullPush_JSONB_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

typedef Callable<bool(uint8 iType, const ChanR_Bin& iChanR, const ChanW_PPT& iChanW)>
	Callable_JSONB_ReadFilter;

bool sPull_JSONB_Push_PPT(const ChanR_Bin& iChanR,
	const ZP<Callable_JSONB_ReadFilter>& iReadFilter,
	const ChanW_PPT& iChanW);

typedef Callable<bool(const PPT& iPPT, const ChanR_PPT& iChanR, const ChanW_Bin& iChanW)>
	Callable_JSONB_WriteFilter;

bool sPull_PPT_Push_JSONB(const ChanR_PPT& iChanR,
	const ZP<Callable_JSONB_WriteFilter>& iWriteFilter,
	const ChanW_Bin& iChanW);

} // namespace ZooLib

#endif // __ZooLib_PullPush_JSONB_h__
