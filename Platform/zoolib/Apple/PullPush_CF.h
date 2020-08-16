// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_PullPush_CF_h__
#define __ZooLib_Apple_PullPush_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/PullPush.h"

#include "zoolib/Apple/ZP_CF.h"

#if ZCONFIG_SPI_Enabled(CFType)

namespace ZooLib {

// =================================================================================================
#pragma mark - 

bool sFromCF_Push_PPT(CFTypeRef iCFTypeRef, const ChanW_PPT& iChanW);

// =================================================================================================
#pragma mark - 

bool sPull_PPT_AsCF(const ChanR_PPT& iChanR, ZP<CFTypeRef>& oCFTypeRef);

ZP<CFTypeRef> sAsCF(const ChanR_PPT& iChanR);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_PullPush_CF_h__
