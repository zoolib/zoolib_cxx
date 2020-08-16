// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compat_cmath_h__
#define __ZooLib_Compat_cmath_h__ 1
#include "zconfig.h"

#define __USE_ISOC99 1
#include <math.h>

#if ZCONFIG(Compiler, MSVC)

	#include <cfloat>

	#define isnan _isnan

	static unsigned int __qnan[] = { 0x7fc00001 };
	#define NAN (*(float*)__qnan)

	static unsigned int __infinity[] = { 0x7f800000 };
	#define INFINITY (*(float*)__infinity)

#else

	#if !defined(INFINITY)
		#define INFINITY (1.0/0.0)
	#endif

	#if !defined(NAN)
		#define NAN (0.0/0.0)
	#endif

#endif

#endif // __ZooLib_Compat_cmath_h__
