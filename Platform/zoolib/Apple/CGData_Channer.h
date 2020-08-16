// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_CGData_Channer_h__
#define __ZooLib_Apple_CGData_Channer_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

#if ZMACRO_IOS
	#include ZMACINCLUDE2(CoreGraphics,CGDataProvider.h)
	#include ZMACINCLUDE2(CoreGraphics,CGDataConsumer.h)
#else
	#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGDataProvider.h)
	#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGDataConsumer.h)
#endif

#include "zoolib/Channer_Bin.h"

namespace ZooLib {
namespace CGData_Channer {

// =================================================================================================
#pragma mark - CGData_Channer

ZP<CGDataProviderRef> sProvider(ZP<ChannerR_Bin> iChanner);

ZP<CGDataProviderRef> sProvider(ZP<ChannerRPos_Bin> iChanner);

ZP<CGDataConsumerRef> sConsumer(ZP<ChannerW_Bin> iChanner);

} // namespace CGData_Channer
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)

#endif // __ZooLib_Apple_CGData_Channer_h__
