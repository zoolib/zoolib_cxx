// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Util_CF_ZZ_h__
#define __ZooLib_Apple_Util_CF_ZZ_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/Val_ZZ.h"

#include "zoolib/Apple/Util_CF_Any.h"
#include "zoolib/Apple/ZP_CF.h"

namespace ZooLib {
namespace Util_CF {

// =================================================================================================
#pragma mark - Util_CF

ZQ<Val_ZZ> sQAsZZ(CFTypeRef iVal);
Val_ZZ sDAsZZ(const Val_ZZ& iDefault, CFTypeRef iVal);
Val_ZZ sAsZZ(CFTypeRef iVal);

ZP<CFTypeRef> sDAsCFType(CFTypeRef iDefault, const Val_ZZ& iVal);
ZP<CFTypeRef> sAsCFType(const Val_ZZ& iVal);

Seq_ZZ sAsSeq_ZZ(const Val_ZZ& iDefault, CFArrayRef iCFArray);
Map_ZZ sAsMap_ZZ(const Val_ZZ& iDefault, CFDictionaryRef iCFDictionary);

} // namespace Util_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Util_CF_ZZ_h__
