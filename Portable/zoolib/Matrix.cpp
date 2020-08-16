// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Matrix.h"

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ARM/Matrix_armv6.h"
#include "zoolib/ARM/Matrix_armv7.h"

namespace ZooLib {

// Prioritize armv7, over armv6, over generic.

// =================================================================================================
#pragma mark - armv7

#if ZCONFIG_API_Enabled(Matrix_armv7)

#if !defined(IMPLEMENTED_sComposeMat4)
	#define IMPLEMENTED_sComposeMat4 1
	void sComposeMat4(const float iLeft[4][4], const float iRight[4][4], float oDest[4][4])
		{ Matrix_armv7::Matrix4Mul(iRight[0], iLeft[0], oDest[0]); }
#endif

#if !defined(IMPLEMENTED_sComposeMat4Vec4)
	#define IMPLEMENTED_sComposeMat4Vec4 1
	void sComposeMat4Vec4(const float iLeft[4][4], const float* iRight, float* oDest)
		{ Matrix_armv7::Matrix4Vector4Mul(iLeft[0], iRight, oDest); }
#endif

#endif // ZCONFIG_API_Enabled(Matrix_armv7)

// =================================================================================================
#pragma mark - armv6

#if ZCONFIG_API_Enabled(Matrix_armv6)

#if !defined(IMPLEMENTED_sInverseMat4)
	#define IMPLEMENTED_sInverseMat4 1
	Matrix<float,4,4> sInverse(const Matrix<float,4,4>& iMat)
		{
		Matrix<float,4,4> dest;
		Matrix_armv6::Matrix4Invert(iMat.fE[0], dest.fE[0]);
		return dest;
		}
#endif

#if !defined(IMPLEMENTED_sComposeMat4)
	#define IMPLEMENTED_sComposeMat4 1
	void sComposeMat4(const float iLeft[4][4], const float iRight[4][4], float oDest[4][4])
		{ Matrix_armv6::Matrix4Mul(iLeft[0], iRight[0], oDest[0]); }
#endif

#if !defined(IMPLEMENTED_sComposeMat4Vec4)
#define IMPLEMENTED_sComposeMat4Vec4 1
	void sComposeMat4Vec4(const float iLeft[4][4], const float* iRight, float* oDest)
		{ Matrix_armv6::Matrix4Vector4Mul(iLeft[0], iRight, oDest); }
#endif

#if !defined(IMPLEMENTED_sComposeMat4Vec3_ToVec4)
	#define IMPLEMENTED_sComposeMat4Vec3_ToVec4 1
	void sComposeMat4Vec3_ToVec4(const float iLeft[4][4], const float* iRight, float* oDest)
		{ Matrix_armv6::Matrix4Vector3Mul(iLeft[0], iRight, oDest); }
#endif

#endif // ZCONFIG_API_Enabled(Matrix_armv6)

// =================================================================================================
#pragma mark - Generic

#if not IMPLEMENTED_sInverseMat4
	#define IMPLEMENTED_sInverseMat4 1
	Matrix<float,4,4> sInverse(const Matrix<float,4,4>& iMat)
		{ return sInverse<float,4>(iMat); }
#endif

#if not IMPLEMENTED_sComposeMat4
	#define IMPLEMENTED_sComposeMat4 1
	void sComposeMat4(const float iLeft[4][4], const float iRight[4][4], float oDest[4][4])
		{ sComposeMat4<float>(iLeft, iRight, oDest); }
#endif

#if not IMPLEMENTED_sComposeMat4Vec4
	#define IMPLEMENTED_sComposeMat4Vec4 1
	void sComposeMat4Vec4(const float iLeft[4][4], const float* iRight, float* oDest)
		{ sComposeMat4Vec4<float>(iLeft, iRight, oDest); }
#endif

#if not IMPLEMENTED_sComposeMat4Vec3_ToVec4
	#define IMPLEMENTED_sComposeMat4Vec3_ToVec4 1
	void sComposeMat4Vec3_ToVec4(const float iLeft[4][4], const float* iRight, float* oDest)
		{ sComposeMat4Vec3_ToVec4<float>(iLeft, iRight, oDest); }
#endif

} // namespace ZooLib
