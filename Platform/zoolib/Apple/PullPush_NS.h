// Copyright (c) 2023 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_PullPush_NS_h__
#define __ZooLib_Apple_PullPush_NS_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/PullPush.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#include "zoolib/Apple/ZP_NS.h"

#import <Foundation/NSObject.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - 

bool sFromNS_Push_PPT(NSObject* iNSObject, const ChanW_PPT& iChanW);

// =================================================================================================
#pragma mark - 

bool sPull_PPT_AsNS(const ChanR_PPT& iChanR, NSObject*& oNSObject);

NSObject* sAsNS(const ChanR_PPT& iChanR);

} // namespace ZooLib

// =================================================================================================
#pragma mark - pushPPT

@interface NSObject (ZooLib_pushPPT)
-(bool)pushPPT:(const ZooLib::ChanW_PPT&)iChanW;
@end

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)

#endif // __ZooLib_Apple_PullPush_NS_h__
