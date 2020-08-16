// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pull_Basic_h__
#define __ZooLib_Pull_Basic_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

struct Pull_Basic_Options
	{
	Pull_Basic_Options(UTF32 iNameFromValue, UTF32 iEntryFromEntry);

	UTF32 fSeparator_NameFromValue;
	UTF32 fSeparator_EntryFromEntry;

	// Perhaps should make sure we differentiate terminator and separator?
	};

bool sPull_Basic_Push_PPT(const ChanR_UTF& iChanR, const Pull_Basic_Options& iOptions,
	const ChanW_PPT& iChanW);

} // namespace ZooLib

#endif // __ZooLib_Pull_Basic_h__
