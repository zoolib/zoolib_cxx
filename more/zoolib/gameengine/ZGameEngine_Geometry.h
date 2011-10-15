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

#ifndef __ZGameEngine_Geometry__
#define __ZGameEngine_Geometry__ 1
#include "zconfig.h"

#include "zoolib/ZMatrix.h"

namespace ZooLib {
namespace ZGameEngine {

// =================================================================================================
#pragma mark -
#pragma mark * CVec ctors

template <class Val, class Tag>
ZMatrix<Val,1,1,Tag> sCVec(Val i0)
	{
	ZMatrix<Val,1,1,Tag> result;
	result.fE[0][0] = i0;
	return result;
	}

template <class Val, class Tag>
ZMatrix<Val,2,1,Tag> sCVec(Val i0, Val i1)
	{
	ZMatrix<Val,2,1,Tag> result;
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	return result;
	}

template <class Val, class Tag>
ZMatrix<Val,3,1,Tag> sCVec(Val i0, Val i1, Val i2)
	{
	ZMatrix<Val,3,1,Tag> result;
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	result.fE[0][2] = i2;
	return result;
	}

template <class Val, class Tag>
ZMatrix<Val,4,1,Tag> sCVec(Val i0, Val i1, Val i2, Val i3)
	{
	ZMatrix<Val,4,1,Tag> result;
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	result.fE[0][2] = i2;
	result.fE[0][3] = i3;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * CVec3 ctors

template <class Val, class Tag>
ZMatrix<Val,3,1,Tag> sCVec3(Val x, Val y, Val z)
	{ return sCVec<Val,Tag>(x, y, z); }

template <class Val, class Tag>
ZMatrix<Val,3,1,Tag> sCVec3(Val iVal)
	{ return sCVec3<Val,Tag>(iVal, iVal, iVal); }

template <class Val, class Tag>
ZMatrix<Val,3,1,Tag> sCVec3()
	{ return ZMatrix<Val,3,1,Tag>(); }

// =================================================================================================
#pragma mark -
#pragma mark * RVec ctors

template <class Val, class Tag>
ZMatrix<Val,1,1,Tag> sRVec(Val i0)
	{
	ZMatrix<Val,1,1,Tag> result;
	result.fE[0][0] = i0;
	return result;
	}

template <class Val, class Tag>
ZMatrix<Val,1,2,Tag> sRVec(Val i0, Val i1)
	{
	ZMatrix<Val,1,2,Tag> result;
	result.fE[0][0] = i0;
	result.fE[1][0] = i1;
	return result;
	}

template <class Val, class Tag>
ZMatrix<Val,1,3,Tag> sRVec(Val i0, Val i1, Val i2)
	{
	ZMatrix<Val,1,3,Tag> result;
	result.fE[0][0] = i0;
	result.fE[1][0] = i1;
	result.fE[2][0] = i2;
	return result;
	}

template <class Val, class Tag>
ZMatrix<Val,1,4,Tag> sRVec(Val i0, Val i1, Val i2, Val i3)
	{
	ZMatrix<Val,1,4,Tag> result;
	result.fE[0][0] = i0;
	result.fE[1][0] = i1;
	result.fE[2][0] = i2;
	result.fE[3][0] = i3;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * RVec3 ctors

template <class Val, class Tag>
ZMatrix<Val,1,3,Tag> sRVec3(Val x, Val y, Val z)
	{ return sRVec(x, y, z); }

template <class Val, class Tag>
ZMatrix<Val,1,3,Tag> sRVec3(Val iVal)
	{ return sRVec3(iVal, iVal, iVal); }

template <class Val, class Tag>
ZMatrix<Val,1,3,Tag> sRVec3()
	{ return ZMatrix<Val,1,3>(); }

// =================================================================================================
#pragma mark -
#pragma mark * sScale

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sScale(Val x, Val y, Val z)
	{
	ZMatrix<Val,4,4,Tag> result;
	result[0][0] = x;
	result[1][1] = y;
	result[2][2] = z;
	result[3][3] = 1;
	return result;
	}

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sScaleX(Val x)
	{ return sScale<Val,Tag>(x, 1, 1); }

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sScaleY(Val y)
	{ return sScale<Val,Tag>(1, y, 1); }

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sScaleZ(Val z)
	{ return sScale<Val,Tag>(1, 1, z); }

// =================================================================================================
#pragma mark -
#pragma mark * sTranslate

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sTranslate(Val x, Val y, Val z)
	{
	ZMatrix<Val,4,4,Tag> result = sIdentity<Val,4,Tag>();
	result[3][0] = x;
	result[3][1] = y;
	result[3][2] = z;
	return result;
	}

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sTranslateX(Val x)
	{ return sTranslate<Val,Tag>(x, 0, 0); }

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sTranslateY(Val y)
	{ return sTranslate<Val,Tag>(0, y, 0); }

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sTranslateZ(Val z)
	{ return sTranslate<Val,Tag>(0, 0, z); }

// =================================================================================================
#pragma mark -
#pragma mark * sRotateX

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sRotateX(Val radians)
	{
	Val s = sin(radians);
	Val c = cos(radians);
	ZMatrix<Val,4,4,Tag> result;
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
#pragma mark * sRotateY

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sRotateY(Val radians)
	{
	Val s = sin(radians);
	Val c = cos(radians);
	ZMatrix<Val,4,4,Tag> result;
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
#pragma mark * sRotateZ

template <class Val, class Tag>
ZMatrix<Val,4,4,Tag> sRotateZ(Val radians)
	{
	Val s = sin(radians);
	Val c = cos(radians);
	ZMatrix<Val,4,4,Tag> result;
	result.fE[0][0] = c;
	result.fE[0][1] = s;
	result.fE[1][0] = -s;
	result.fE[1][1] = c;
	result.fE[2][2] = 1;
	result.fE[3][3] = 1;
	return result;
	}

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Geometry__
