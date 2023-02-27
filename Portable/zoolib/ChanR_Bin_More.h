// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanR_Bin_More_h__
#define __ZooLib_ChanR_Bin_More_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -

ZQ<std::string> sQReadString(const ChanR_Bin& iChanR, size_t iCount);

std::string sReadString(const ChanR_Bin& iChanR, size_t iCount);

std::string sReadAllString(const ChanR_Bin& iChanR);

bool sRead_String(const ChanR_Bin& iChanR, const std::string& iPattern);

bool sTryRead_String(const ChanRU_Bin& iChanRU, const std::string& iPattern);

ZQ<uint64> sQReadCount(const ChanR_Bin& r);

uint64 sReadCount(const ChanR_Bin& r);

std::string sReadCountPrefixedString(const ChanR_Bin& iChanR);

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_More_h__
