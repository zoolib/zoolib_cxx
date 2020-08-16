// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Util_CF_Any_h__
#define __ZooLib_Apple_Util_CF_Any_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/Any.h"

#include "zoolib/Apple/ZP_CF.h"

namespace ZooLib {
namespace Util_CF {

// =================================================================================================
#pragma mark - Util_CF

ZQ<Any> sQSimpleAsAny(CFTypeID iTypeID, CFTypeRef iVal);

} // namespace Util_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Util_CF_Any_h__
