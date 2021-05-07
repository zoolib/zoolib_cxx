// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ARM_Compat_arm_h__
#define __ZooLib_ARM_Compat_arm_h__ 1
#include "zconfig.h"

#if __arm__
	#include <endian.h> // picks up arm macros, doesn't choke on android
#endif

#endif // __ZooLib_ARM_Compat_arm_h__
