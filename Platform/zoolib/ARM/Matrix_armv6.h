// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ARM_Matrix_armv6_h__
#define __ZooLib_ARM_Matrix_armv6_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"

#include "zoolib/ARM/Compat_arm.h"

#ifndef ZCONFIG_API_Desired__Matrix_armv6
	#define ZCONFIG_API_Desired__Matrix_armv6 1
#endif

#ifndef ZCONFIG_API_Avail__Matrix_armv6
	#if ZCONFIG_API_Desired__Matrix_armv6
		#if __arm__
			// clang 4 is having trouble with fldmias instructions, disable for now.
			#if defined(_ARM_ARCH_6) && (not defined(__clang_major__) || __clang_major__ < 4)
				#define ZCONFIG_API_Avail__Matrix_armv6 1
			#endif
		#endif
	#endif
#endif

#ifndef ZCONFIG_API_Avail__Matrix_armv6
	#define ZCONFIG_API_Avail__Matrix_armv6 0
#endif

#if ZCONFIG_API_Enabled(Matrix_armv6)

namespace ZooLib {
namespace Matrix_armv6 {

// =================================================================================================
#pragma mark -

void Matrix4Mul(const float* src_mat_1, const float* src_mat_2, float* dst_mat);

void Matrix4Vector4Mul(const float* src_mat, const float* src_vec, float* dst_vec);

void Matrix4Vector3Mul(const float* src_mat, const float* src_vec, float* dst_vec);

void Matrix4Invert(const float* src_mat, float* dst_mat);

} // namespace Matrix_armv6
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Matrix_armv6)

#endif // __ZooLib_ARM_Matrix_armv6_h__
