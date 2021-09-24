// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_PullPush_XMLPList_h__
#define __ZooLib_PullPush_XMLPList_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF_ML.h"
#include "zoolib/ChanW_UTF_ML.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

void sPull_XMLPList_Push_PPT(ChanRU_UTF_ML& iChanRU, const ChanW_PPT& iChanW);

void sPull_XMLPList_Push_PPT(const ChanR_PPT& iChanR, const ChanW_PPT& iChanW);

bool sPull_PPT_Push_XMLPList(const ChanR_PPT& iChanR, const ChanW_UTF_ML& iChanW);

void sWriteXMLPListPreamble(const ChanW_UTF_ML& s);
void sWriteXMLPListPostamble(const ChanW_UTF_ML& s);

// =================================================================================================
#pragma mark - sChannerR_PPT_xx

ZP<ChannerR_PPT> sChannerR_PPT_XMLPList(const ZP<ChannerRU_UTF_ML>& iChanner);

} // namespace ZooLib

#endif // __ZooLib_PullPush_XMLPList_h__
