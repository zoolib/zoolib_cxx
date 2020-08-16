// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pull_SeparatedValues_h__
#define __ZooLib_Pull_SeparatedValues_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

struct Pull_SeparatedValues_Options
	{
	Pull_SeparatedValues_Options(UTF32 iSeparator_Value, UTF32 iSeparator_Line);

	UTF32 fSeparator_Value;
	UTF32 fSeparator_Line;
	};

bool sPull_SeparatedValues_Push_PPT(const ChanR_UTF& iChanR,
	const Pull_SeparatedValues_Options& iOptions,
	const ChanW_PPT& iChanW);

bool sPull_SeparatedValues_Push_PPT(const ChanR_UTF& iChanR,
	UTF32 iSeparator_Value, UTF32 iSeparator_Line,
	const ChanW_PPT& iChanW);

} // namespace ZooLib

#endif // __ZooLib_Pull_SeparatedValues_h__
