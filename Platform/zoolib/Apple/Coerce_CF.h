// Copyright (c) 2018 Andrew Green and Mark/Space, Inc
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Coerce_CF_h__
#define __ZooLib_Apple_Coerce_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/StdInt.h" // For int64
#include "zoolib/ZQ.h"
#include "zoolib/Apple/ZP_CF.h"

// =================================================================================================
#pragma mark - Coercion

namespace ZooLib {

ZQ<bool> sQCoerceBool(CFTypeRef iCF);
bool sQCoerceBool(CFTypeRef iCF, bool& oVal);
bool sCoerceBool(CFTypeRef iCF);
bool sCoerceBool(const ZP<CFTypeRef>& iCF);

ZQ<int64> sQCoerceInt(CFTypeRef iCF);
bool sQCoerceInt(CFTypeRef iCF, int64& oVal);
int64 sCoerceInt(CFTypeRef iCF);
int64 sCoerceInt(const ZP<CFTypeRef>& iCF);

ZQ<double> sQCoerceRat(CFTypeRef iCF);
bool sQCoerceRat(CFTypeRef iCF, double& oVal);
double sCoerceRat(CFTypeRef iCF);
double sCoerceRat(const ZP<CFTypeRef>& iCF);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Coerce_CF_h__
