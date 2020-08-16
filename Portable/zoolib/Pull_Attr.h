// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pull_Attr_h__
#define __ZooLib_Pull_Attr_h__ 1
#include "zconfig.h"

#include "zoolib/Pull_Basic.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

inline bool sPull_Attr_Push_PPT(const ChanR_UTF& iChanR,
	const ChanW_PPT& iChanW)
	{ return sPull_Basic_Push_PPT(iChanR, Pull_Basic_Options('=', ','), iChanW); }

} // namespace ZooLib

#endif // __ZooLib_Pull_Attr_h__
