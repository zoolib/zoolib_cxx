// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pull_Bencode_h__
#define __ZooLib_Pull_Bencode_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

typedef ChanRU<byte> ChanRU_Bin;

// =================================================================================================
#pragma mark - 

bool sPull_Bencode_Push_PPT(const ChanRU_Bin& iChanRU, const ChanW_PPT& iChanW);

} // namespace ZooLib

#endif // __ZooLib_Pull_Bencode_h__
