/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/GameEngine/ZMatrix.h"
#include "zoolib/GameEngine/ZMatrix_armv6.h"
#include "zoolib/GameEngine/ZMatrix_armv7.h"

namespace ZooLib {

// Prioritize armv7, over armv6, over generic.

// =================================================================================================
#pragma mark -
#pragma mark armv7

#if ZCONFIG_API_Enabled(Matrix_armv7)

#if !defined(IMPLEMENTED_sComposeMat4)
	#define IMPLEMENTED_sComposeMat4 1
	void sComposeMat4(const float iLeft[4][4], const float iRight[4][4], float oDest[4][4])
		{ ZMatrix_armv7::Matrix4Mul(iRight[0], iLeft[0], oDest[0]); }
#endif

#if !defined(IMPLEMENTED_sComposeMat4Vec4)
	#define IMPLEMENTED_sComposeMat4Vec4 1
	void sComposeMat4Vec4(const float iLeft[4][4], const float* iRight, float* oDest)
		{ ZMatrix_armv7::Matrix4Vector4Mul(iLeft[0], iRight, oDest); }
#endif

#endif // ZCONFIG_API_Enabled(Matrix_armv7)

// =================================================================================================
#pragma mark -
#pragma mark armv6

#if ZCONFIG_API_Enabled(Matrix_armv6)

#if !defined(IMPLEMENTED_sInverseMat4)
	#define IMPLEMENTED_sInverseMat4 1
	ZMatrix<float,4,4> sInverse(const ZMatrix<float,4,4>& iMat)
		{
		ZMatrix<float,4,4> dest;
		ZMatrix_armv6::Matrix4Invert(iMat.fE[0], dest.fE[0]);
		return dest;
		}
#endif

#if !defined(IMPLEMENTED_sComposeMat4)
	#define IMPLEMENTED_sComposeMat4 1
	void sComposeMat4(const float iLeft[4][4], const float iRight[4][4], float oDest[4][4])
		{ ZMatrix_armv6::Matrix4Mul(iLeft[0], iRight[0], oDest[0]); }
#endif

#if !defined(IMPLEMENTED_sComposeMat4Vec4)
#define IMPLEMENTED_sComposeMat4Vec4 1
	void sComposeMat4Vec4(const float iLeft[4][4], const float* iRight, float* oDest)
		{ ZMatrix_armv6::Matrix4Vector4Mul(iLeft[0], iRight, oDest); }
#endif

#if !defined(IMPLEMENTED_sComposeMat4Vec3_ToVec4)
	#define IMPLEMENTED_sComposeMat4Vec3_ToVec4 1
	void sComposeMat4Vec3_ToVec4(const float iLeft[4][4], const float* iRight, float* oDest)
		{ ZMatrix_armv6::Matrix4Vector3Mul(iLeft[0], iRight, oDest); }
#endif

#endif // ZCONFIG_API_Enabled(Matrix_armv6)

// =================================================================================================
#pragma mark -
#pragma mark Generic

#if not IMPLEMENTED_sInverseMat4
	#define IMPLEMENTED_sInverseMat4 1
	ZMatrix<float,4,4> sInverse(const ZMatrix<float,4,4>& iMat)
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
