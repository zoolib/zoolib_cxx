// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_Bin_h__
#define __ZooLib_Chan_Bin_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/StdInt.h" // For byte

namespace ZooLib {

// =================================================================================================
#pragma mark - 

using ChanR_Bin = ChanR<byte>;
using ChanU_Bin = ChanU<byte>;
using ChanW_Bin = ChanW<byte>;

using ChanRU_Bin = ChanRU<byte>;
using ChanRSize_Bin = ChanRSize<byte>;
using ChanRPos_Bin = ChanRPos<byte>;
using ChanWPos_Bin = ChanWPos<byte>;
using ChanRWPos_Bin = ChanRWPos<byte>;
using ChanRW_Bin = ChanRW<byte>;
using ChanRAbort_Bin = ChanRAbort<byte>;
using ChanWAbort_Bin = ChanWAbort<byte>;
using ChanRWAbort_Bin = ChanRWAbort<byte>;
using ChanRCon_Bin = ChanRCon<byte>;
using ChanWCon_Bin = ChanWCon<byte>;
using ChanRWCon_Bin = ChanRWCon<byte>;

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_h__
