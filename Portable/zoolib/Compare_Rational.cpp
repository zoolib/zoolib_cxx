// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Compare_Rational.h"

#include "zoolib/Compare.h"
#include "zoolib/Compat_cmath.h"

ZMACRO_MSVCStaticLib_cpp(Compare_Rational)

namespace ZooLib {

// To provide stable ordering between floating point values we
// arbitrarily say that a nan is less than any non-nan value.

template <>
int sCompare_T(const float& iL, const float& iR)
	{
	if (isnan(iL))
		{
		if (isnan(iR))
			return 0;
		return -1;
		}
	else if (isnan(iR))
		return 1;
	else if (iL < iR)
		return -1;
	else if (iL > iR)
		return 1;
	return 0;
	}

ZMACRO_CompareRegistration_T(float)

template <>
int sCompare_T(const double& iL, const double& iR)
	{
	if (isnan(iL))
		{
		if (isnan(iR))
			return 0;
		return -1;
		}
	else if (isnan(iR))
		return 1;
	else if (iL < iR)
		return -1;
	else if (iL > iR)
		return 1;
	return 0;
	}

ZMACRO_CompareRegistration_T(double)

template <>
int sCompare_T(const long double& iL, const long double& iR)
	{
	if (isnan(iL))
		{
		if (isnan(iR))
			return 0;
		return -1;
		}
	else if (isnan(iR))
		return 1;
	else if (iL < iR)
		return -1;
	else if (iL > iR)
		return 1;
	return 0;
	}

ZMACRO_CompareRegistration_T(long double)

} // namespace ZooLib
