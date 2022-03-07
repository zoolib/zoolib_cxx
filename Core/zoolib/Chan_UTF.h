// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_UTF_h__
#define __ZooLib_Chan_UTF_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/UnicodeCU.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

using ChanR_UTF = ChanR<UTF32>;
using ChanU_UTF = ChanU<UTF32>;
using ChanW_UTF = ChanW<UTF32>;

using ChanRU_UTF = ChanRU<UTF32>;
using ChanRPos_UTF = ChanRPos<UTF32>;
using ChanWPos_UTF = ChanWPos<UTF32>;
using ChanRWPos_UTF = ChanRWPos<UTF32>;
using ChanRW_UTF = ChanRW<UTF32>;
using ChanRAbort_UTF = ChanRAbort<UTF32>;
using ChanWAbort_UTF = ChanWAbort<UTF32>;
using ChanRWAbort_UTF = ChanRWAbort<UTF32>;
using ChanRCon_UTF = ChanRCon<UTF32>;
using ChanWCon_UTF = ChanWCon<UTF32>;
using ChanRWCon_UTF = ChanRWCon<UTF32>;

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_h__
