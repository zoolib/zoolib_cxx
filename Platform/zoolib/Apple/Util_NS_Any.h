// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Util_NS_Any_h__
#define __ZooLib_Apple_Util_NS_Any_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#include "zoolib/Val_ZZ.h"

#import <Foundation/NSObject.h>

namespace ZooLib {
namespace Util_NS {

// =================================================================================================
#pragma mark - Util_NS

Val_ZZ sDAsZZ(const Val_ZZ& iDefault, NSObject* iVal);
Val_ZZ sAsZZ(NSObject* iVal);

NSObject* sDAsNSObject(NSObject* iDefault, const Val_ZZ& iVal);
NSObject* sAsNSObject(const Val_ZZ& iVal);

} // namespace Util_NS
} // namespace ZooLib

// =================================================================================================
#pragma mark - asAnyWithDefault

@interface NSObject (ZooLib_Any_Additions)
-(ZooLib::Val_ZZ)asZZWithDefault:(const ZooLib::Val_ZZ&)iDefault;
@end

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)

#endif // __ZooLib_Apple_Util_NS_Any_h__
