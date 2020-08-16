// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ARM_Matrix_armv7_h__
#define __ZooLib_ARM_Matrix_armv7_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"

#include "zoolib/ARM/Compat_arm.h"

#ifndef ZCONFIG_API_Desired__Matrix_armv7
	#define ZCONFIG_API_Desired__Matrix_armv7 1
#endif

#ifndef ZCONFIG_API_Avail__Matrix_armv7
	#if ZCONFIG_API_Desired__Matrix_armv7
		#if __arm__
			#if defined(_ARM_ARCH_7)
				#define ZCONFIG_API_Avail__Matrix_armv7 1
			#endif
		#endif
	#endif
#endif

#ifndef ZCONFIG_API_Avail__Matrix_armv7
	#define ZCONFIG_API_Avail__Matrix_armv7 0
#endif

#if ZCONFIG_API_Enabled(Matrix_armv7)

namespace ZooLib {
namespace Matrix_armv7 {

// =================================================================================================
#pragma mark -

void Matrix4Mul(const float* src_mat_1, const float* src_mat_2, float* dst_mat);

void Matrix4Vector4Mul(const float* src_mat, const float* src_vec, float* dst_vec);

} // namespace Matrix_armv7
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Matrix_armv7)

#endif // __ZooLib_ARM_Matrix_armv7_h__
