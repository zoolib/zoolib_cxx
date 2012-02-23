/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZGameEngine_Geometry_h__
#define __ZGameEngine_Geometry_h__ 1
#include "zconfig.h"

#include "zoolib/ZMatrix.h"

namespace ZooLib {
namespace ZGameEngine {

// =================================================================================================
// MARK: - CVec ctors

template <class Val>
ZMatrix<Val,1,1> sCVec(Val i0)
	{
	ZMatrix<Val,1,1> result;
	result.fE[0][0] = i0;
	return result;
	}

template <class Val>
ZMatrix<Val,1,2> sCVec(Val i0, Val i1)
	{
	ZMatrix<Val,1,2> result;
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	return result;
	}

template <class Val>
ZMatrix<Val,1,3> sCVec(Val i0, Val i1, Val i2)
	{
	ZMatrix<Val,1,3> result;
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	result.fE[0][2] = i2;
	return result;
	}

template <class Val>
ZMatrix<Val,1,4> sCVec(Val i0, Val i1, Val i2, Val i3)
	{
	ZMatrix<Val,1,4> result;
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	result.fE[0][2] = i2;
	result.fE[0][3] = i3;
	return result;
	}

// =================================================================================================
// MARK: - CVec3 ctors

template <class Val>
ZMatrix<Val,1,3> sCVec3(Val x, Val y, Val z)
	{ return sCVec(x, y, z); }

template <class Val>
ZMatrix<Val,1,3> sCVec3(Val iVal)
	{ return sCVec3(iVal, iVal, iVal); }

template <class Val>
ZMatrix<Val,1,3> sCVec3()
	{ return ZMatrix<Val,1,3>(); }

// =================================================================================================
// MARK: - RVec ctors

template <class Val>
ZMatrix<Val,1,1> sRVec(Val i0)
	{
	ZMatrix<Val,1,1> result;
	result.fE[0][0] = i0;
	return result;
	}

template <class Val>
ZMatrix<Val,2,1> sRVec(Val i0, Val i1)
	{
	ZMatrix<Val,1,2> result;
	result.fE[0][0] = i0;
	result.fE[1][0] = i1;
	return result;
	}

template <class Val>
ZMatrix<Val,3,1> sRVec(Val i0, Val i1, Val i2)
	{
	ZMatrix<Val,1,3> result;
	result.fE[0][0] = i0;
	result.fE[1][0] = i1;
	result.fE[2][0] = i2;
	return result;
	}

template <class Val>
ZMatrix<Val,4,1> sRVec(Val i0, Val i1, Val i2, Val i3)
	{
	ZMatrix<Val,1,4> result;
	result.fE[0][0] = i0;
	result.fE[1][0] = i1;
	result.fE[2][0] = i2;
	result.fE[3][0] = i3;
	return result;
	}

// =================================================================================================
// MARK: - RVec3 ctors

template <class Val>
ZMatrix<Val,3,1> sRVec3(Val x, Val y, Val z)
	{ return sRVec(x, y, z); }

template <class Val>
ZMatrix<Val,3,1> sRVec3(Val iVal)
	{ return sRVec3(iVal, iVal, iVal); }

template <class Val>
ZMatrix<Val,3,1> sRVec3()
	{ return ZMatrix<Val,3,1>(); }

// =================================================================================================
// MARK: - sScale

template <class Val>
ZMatrix<Val,4,4> sScale(Val x, Val y, Val z)
	{
	ZMatrix<Val,4,4> result;
	result[0][0] = x;
	result[1][1] = y;
	result[2][2] = z;
	result[3][3] = 1;
	return result;
	}

template <class Val>
ZMatrix<Val,4,4> sScaleX(Val x)
	{ return sScale<Val>(x, 1, 1); }

template <class Val>
ZMatrix<Val,4,4> sScaleY(Val y)
	{ return sScale<Val>(1, y, 1); }

template <class Val>
ZMatrix<Val,4,4> sScaleZ(Val z)
	{ return sScale<Val>(1, 1, z); }

template <class Val>
ZMatrix<Val,4,4> sScale(const ZMatrix<Val,1,3>& iCVec3)
	{ return sScale(iCVec3[0], iCVec3[1], iCVec3[2]); }

// =================================================================================================
// MARK: - sTranslate

template <class Val>
ZMatrix<Val,4,4> sTranslate(Val x, Val y, Val z)
	{
	ZMatrix<Val,4,4> result = sIdentity<Val,4>();
	result[3][0] = x;
	result[3][1] = y;
	result[3][2] = z;
	return result;
	}

template <class Val>
ZMatrix<Val,4,4> sTranslateX(Val x)
	{ return sTranslate<Val>(x, 0, 0); }

template <class Val>
ZMatrix<Val,4,4> sTranslateY(Val y)
	{ return sTranslate<Val>(0, y, 0); }

template <class Val>
ZMatrix<Val,4,4> sTranslateZ(Val z)
	{ return sTranslate<Val>(0, 0, z); }

template <class Val>
ZMatrix<Val,4,4> sTranslate(const ZMatrix<Val,1,3>& iCVec3)
	{ return sTranslate(iCVec3[0], iCVec3[1], iCVec3[2]); }

// =================================================================================================
// MARK: - sRotateX

template <class Val>
ZMatrix<Val,4,4> sRotateX(Val radians)
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
// MARK: - sRotateY

template <class Val>
ZMatrix<Val,4,4> sRotateY(Val radians)
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
// MARK: - sRotateZ

template <class Val>
ZMatrix<Val,4,4> sRotateZ(Val radians)
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
// MARK: - sShear

template <class Val>
ZMatrix<Val,4,4> sShear(Val xy, Val xz, Val yx, Val yz, Val zx, Val zy)
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

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Geometry_h__
