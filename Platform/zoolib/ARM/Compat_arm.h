// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ARM_Compat_arm_h__
#define __ZooLib_ARM_Compat_arm_h__ 1
#include "zconfig.h"

#if __arm__
	#if defined (__ANDROID__)
		#include <machine/cpu-features.h>
//	#else
//		#include <arm/arch.h>
	#endif
#endif

#endif // __ZooLib_ARM_Compat_arm_h__
