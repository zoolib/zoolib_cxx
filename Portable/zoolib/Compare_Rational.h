// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_Rational_h__
#define __ZooLib_Compare_Rational_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"
#include "zoolib/Compat_MSVCStaticLib.h"

ZMACRO_MSVCStaticLib_Reference(Compare_Rational)

namespace ZooLib {

template <>
int sCompare_T(const float& iL, const float& iR);

template <>
int sCompare_T(const double& iL, const double& iR);

template <>
int sCompare_T(const long double& iL, const long double& iR);

} // namespace ZooLib

#endif // __ZooLib_Compare_Rational_h__
