// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pull_XMLAttr_h__
#define __ZooLib_Pull_XMLAttr_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF_ML.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

bool sPull_XMLAttr_Push_PPT(ChanRU_UTF_ML& ioChanRU, const ChanW_PPT& iChanW);

ZP<ChannerR_PPT> sChannerR_PPT_XMLAttr(const ZP<ChannerRU_UTF_ML>& iChannerRU);

} // namespace ZooLib

#endif // __ZooLib_Pull_XMLAttr_h__
