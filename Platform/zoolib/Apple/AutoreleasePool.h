// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_AutoreleasePool_h__
#define __ZooLib_Apple_AutoreleasePool_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)
#if __OBJC__ && __cplusplus

#include "zoolib/Apple/ZP_NS.h"

#import <Foundation/NSAutoreleasePool.h>

// =================================================================================================
#pragma mark - AutoreleasePool

namespace ZooLib {

struct AutoreleasePool : public ZP<NSAutoreleasePool>
	{ AutoreleasePool() : ZP<NSAutoreleasePool>(sAdopt& [[NSAutoreleasePool alloc] init]) {}; };

} // namespace ZooLib

#endif // __OBJC__ && __cplusplus
#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZooLib_Apple_AutoreleasePool_h__
