// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_PullPush_SeparatedValues_h__
#define __ZooLib_PullPush_SeparatedValues_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/PullPush.h"
#include "zoolib/PullTextOptions.h"
#include "zoolib/PushTextOptions.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

struct Pull_SeparatedValues_Options : PullTextOptions
	{
	Pull_SeparatedValues_Options(UTF32 iSeparator_Value, UTF32 iTerminator_Line);

	UTF32 fSeparator_Value;
	UTF32 fTerminator_Line;
	};

bool sPull_SeparatedValues_Push_PPT(const ChanR_UTF& iChanR,
	const Pull_SeparatedValues_Options& iOptions,
	const ChanW_PPT& iChanW);

void sPull_SeparatedValues_Push_PPT_Alternate(const Pull_SeparatedValues_Options& iOptions,
	const ChanR_UTF& iChanR,
	const ChanW_PPT& iChanW);

bool sPull_SeparatedValues_Push_PPT(const ChanR_UTF& iChanR,
	UTF32 iSeparator_Value, UTF32 iSeparator_Line,
	const ChanW_PPT& iChanW);

// =================================================================================================
#pragma mark -

struct Push_SeparatedValues_Options : PushTextOptions
	{
	Push_SeparatedValues_Options(UTF32 iSeparator_Value, UTF32 iTerminator_Line);

	UTF32 fSeparator_Value;
	UTF32 fTerminator_Line;

	ZQ<string8> fNamesQ;
	};

bool sPull_PPT_Push_SeparatedValues(const ChanR_PPT& iChanR,
	const Push_SeparatedValues_Options& iOptions,
	const ChanW_UTF& iChanW);

} // namespace ZooLib

#endif // __ZooLib_PullPush_SeparatedValues_h__
