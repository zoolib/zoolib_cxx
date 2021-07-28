// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Coerce_Any_h__
#define __ZooLib_Coerce_Any_h__ 1
#include "zconfig.h"

#include "zoolib/AnyBase.h"
#include "zoolib/StdInt.h" // For int64

// =================================================================================================
#pragma mark - Any coercion

namespace ZooLib {

ZQ<bool> sQCoerceBool(const AnyBase& iAny);
bool sQCoerceBool(const AnyBase& iAny, bool& oVal);
bool sCoerceBool(const AnyBase& iAny);

ZQ<int64> sQCoerceInt(const AnyBase& iAny);
bool sQCoerceInt(const AnyBase& iAny, int64& oVal);
int64 sCoerceInt(const AnyBase& iAny);

ZQ<double> sQCoerceRat(const AnyBase& iAny);
bool sQCoerceRat(const AnyBase& iAny, double& oVal);
double sCoerceRat(const AnyBase& iAny);

ZQ<int64> sQCoerceNumberAsInt(const AnyBase& iAny);
ZQ<double> sQCoerceNumberAsRat(const AnyBase& iAny);

} // namespace ZooLib

#endif // __ZooLib_Coerce_Any_h__
