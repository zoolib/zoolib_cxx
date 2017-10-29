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

#ifndef __ZGameEngine_Geometry3D_h__
#define __ZGameEngine_Geometry3D_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/ZGameEngine_Geometry2D.h"

namespace ZooLib {
namespace ZGameEngine {

// =================================================================================================
#pragma mark -
#pragma mark sScale3

template <class Val>
ZMatrix<Val,4,4> sScale3(Val x, Val y, Val z)
	{
	ZMatrix<Val,4,4> result;
	result[0][0] = x;
	result[1][1] = y;
	result[2][2] = z;
	result[3][3] = 1;
	return result;
	}

template <class Val>
ZMatrix<Val,4,4> sScale3X(Val x)
	{ return sScale3<Val>(x, 1, 1); }

template <class Val>
ZMatrix<Val,4,4> sScale3Y(Val y)
	{ return sScale3<Val>(1, y, 1); }

template <class Val>
ZMatrix<Val,4,4> sScale3Z(Val z)
	{ return sScale3<Val>(1, 1, z); }

template <class Val>
ZMatrix<Val,4,4> sScale3(const ZMatrix<Val,1,3>& iCVec3)
	{ return sScale(iCVec3); }

// =================================================================================================
#pragma mark -
#pragma mark sTranslate3

template <class Val>
ZMatrix<Val,4,4> sTranslate3(Val x, Val y, Val z)
	{
	ZMatrix<Val,4,4> result(1);
	result[3][0] = x;
	result[3][1] = y;
	result[3][2] = z;
	return result;
	}

template <class Val>
ZMatrix<Val,4,4> sTranslate3X(Val x)
	{ return sTranslate3<Val>(x, 0, 0); }

template <class Val>
ZMatrix<Val,4,4> sTranslate3Y(Val y)
	{ return sTranslate3<Val>(0, y, 0); }

template <class Val>
ZMatrix<Val,4,4> sTranslate3Z(Val z)
	{ return sTranslate3<Val>(0, 0, z); }

template <class Val>
ZMatrix<Val,4,4> sTranslate3(const ZMatrix<Val,1,3>& iCVec3)
	{ return sTranslate<Val>(iCVec3); }

// =================================================================================================
#pragma mark -
#pragma mark sRotate3X

template <class Val>
ZMatrix<Val,4,4> sRotate3X(Val radians)
	{
	const Val s = sin(radians);
	const Val c = cos(radians);
	ZMatrix<Val,4,4> result;
	result.fE[0][0] = 1;
	result.fE[1][1] = c;
	result.fE[1][2] = s;
	result.fE[2][1] = -s;
	result.fE[2][2] = c;
	result.fE[3][3] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark sRotate3Y

template <class Val>
ZMatrix<Val,4,4> sRotate3Y(Val radians)
	{
	const Val s = sin(radians);
	const Val c = cos(radians);
	ZMatrix<Val,4,4> result;
	result.fE[0][0] = c;
	result.fE[1][1] = 1;
	result.fE[0][2] = s;
	result.fE[2][0] = -s;
	result.fE[2][2] = c;
	result.fE[3][3] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark sRotate3Z

template <class Val>
ZMatrix<Val,4,4> sRotate3Z(Val radians)
	{
	const Val s = sin(radians);
	const Val c = cos(radians);
	ZMatrix<Val,4,4> result;
	result.fE[0][0] = c;
	result.fE[0][1] = s;
	result.fE[1][0] = -s;
	result.fE[1][1] = c;
	result.fE[2][2] = 1;
	result.fE[3][3] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark sShear3

template <class Val>
ZMatrix<Val,4,4> sShear3(Val xy, Val xz, Val yx, Val yz, Val zx, Val zy)
	{
	ZMatrix<Val,4,4> result(null);
	result.fE[0][0] = 1;
	result.fE[0][1] = xy;
	result.fE[0][2] = xz;
	result.fE[0][3] = 0;

	result.fE[1][0] = yx;
	result.fE[1][1] = 1;
	result.fE[1][2] = yz;
	result.fE[1][3] = 0;

	result.fE[2][0] = zx;
	result.fE[2][1] = zy;
	result.fE[2][2] = 1;
	result.fE[2][3] = 0;

	result.fE[3][0] = 0;
	result.fE[3][1] = 0;
	result.fE[3][2] = 0;
	result.fE[3][3] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark Extraction of coordinate subsets

template <class Val>
ZMatrix<Val,1,2> sXY(const ZMatrix<Val,1,3>& iMat)
	{ return ZMatrix<Val,1,2>(iMat[0], iMat[1]); }

template <class Val>
ZMatrix<Val,1,2> sYX(const ZMatrix<Val,1,3>& iMat)
	{ return ZMatrix<Val,1,2>(iMat[1], iMat[0]); }

template <class Val>
ZMatrix<Val,1,2> sXZ(const ZMatrix<Val,1,3>& iMat)
	{ return ZMatrix<Val,1,2>(iMat[0], iMat[2]); }

template <class Val>
ZMatrix<Val,1,2> sZX(const ZMatrix<Val,1,3>& iMat)
	{ return ZMatrix<Val,1,2>(iMat[2], iMat[0]); }

template <class Val>
ZMatrix<Val,1,2> sYZ(const ZMatrix<Val,1,3>& iMat)
	{ return ZMatrix<Val,1,2>(iMat[1], iMat[2]); }

template <class Val>
ZMatrix<Val,1,2> sZY(const ZMatrix<Val,1,3>& iMat)
	{ return ZMatrix<Val,1,2>(iMat[2], iMat[1]); }

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Geometry3D_h__
