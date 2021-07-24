// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Channer_Bin_h__
#define __ZooLib_Channer_Bin_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_Bin.h"
#include "zoolib/Channer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

typedef ChannerR<byte> ChannerR_Bin;
typedef ChannerU<byte> ChannerU_Bin;
typedef ChannerW<byte> ChannerW_Bin;

typedef ChannerRU<byte> ChannerRU_Bin;

typedef ChannerRSize<byte> ChannerRSize_Bin;

typedef ChannerRPos<byte> ChannerRPos_Bin;
typedef ChannerWPos<byte> ChannerWPos_Bin;
typedef ChannerRWPos<byte> ChannerRWPos_Bin;

typedef ChannerRW<byte> ChannerRW_Bin;

typedef ChannerRAbort<byte> ChannerRAbort_Bin;
typedef ChannerWAbort<byte> ChannerWAbort_Bin;
typedef ChannerRWAbort<byte> ChannerRWAbort_Bin;

typedef ChannerRCon<byte> ChannerRCon_Bin;
typedef ChannerWCon<byte> ChannerWCon_Bin;
typedef ChannerRWCon<byte> ChannerRWCon_Bin;

} // namespace ZooLib

#endif // __ZooLib_Channer_Bin_h__
