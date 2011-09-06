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
#pragma mark * Quaternion -- a beginning.

template <class Val>
class Quaternion
	{
public:
	typedef ZMatrix<Val,3,1> CVec3;

	Quaternion()
	:	fS(0)
		{}

	Quaternion(const Quaternion& iOther)
	:	fS(iOther.fS)
	,	fV(iOther.fV)
		{}
	
	Quaternion& operator=(const Quaternion& iOther)
		{
		fS = iOther.fS;
		fV = iOther.fV;
		return *this;
		}

	Quaternion(Val iS, const CVec3& iV)
	:	fS(iS)
	,	fV(iV)
		{}

	Val fS;
	CVec3 fV;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CVec3 ctors

template <class Val>
ZMatrix<Val,3,1> sCVec3(Val x, Val y, Val z)
	{
	ZMatrix<Val,3,1> result;
	result[0] = x;
	result[1] = y;
	result[2] = z;
	return result;
	}

template <class Val>
ZMatrix<Val,3,1> sCVec3(Val iVal)
	{ return sCVec3(iVal, iVal, iVal); }

template <class Val>
ZMatrix<Val,3,1> sCVec3()
	{ return ZMatrix<Val,3,1>(); }

// =================================================================================================
#pragma mark -
#pragma mark * RVec3 ctors

template <class Val>
ZMatrix<Val,1,3> sRVec3(Val x, Val y, Val z)
	{
	ZMatrix<Val,1,3> result;
	result[0] = x;
	result[1] = y;
	result[2] = z;
	return result;
	}

template <class Val>
ZMatrix<Val,1,3> sRVec3(Val iVal)
	{ return sRVec3(iVal, iVal, iVal); }

template <class Val>
ZMatrix<Val,1,3> sRVec3()
	{ return ZMatrix<Val,1,3>(); }

// =================================================================================================
#pragma mark -
#pragma mark * sScale

template <class Val>
ZMatrix<Val,4,4> sScale(Val x, Val y, Val z)
	{
	ZMatrix<Val,4,4> result;
	result[0][0] = x;
	result[1][1] = y;
	result[2][2] = z;
	return result;
	}

template <class Val>
ZMatrix<Val,4,4> sScaleX(Val x)
	{ return sScale<Val>(x, 0, 0); }

template <class Val>
ZMatrix<Val,4,4> sScaleY(Val y)
	{ return sScale<Val>(0, y, 0); }

template <class Val>
ZMatrix<Val,4,4> sScaleZ(Val z)
	{ return sScale<Val>(0, 0, z); }

// =================================================================================================
#pragma mark -
#pragma mark * sTranslate

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

// =================================================================================================
#pragma mark -
#pragma mark * sRotateX

template <class Val>
ZMatrix<Val,4,4> sRotateX(Val radians)
	{
	Val s = sin(radians);
	Val c = cos(radians);
	ZMatrix<Val,4,4> result;
	result[0][0] = 1;
	result[1][1] = c;
	result[2][1] = s;
	result[1][2] = -s;
	result[2][2] = c;
	result[3][3] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sRotateY

template <class Val>
ZMatrix<Val,4,4> sRotateY(Val radians)
	{
	Val s = sin(radians);
	Val c = cos(radians);
	ZMatrix<Val,4,4> result;
	result[0][0] = c;
	result[1][1] = 1;
	result[2][0] = s;
	result[0][2] = -s;
	result[2][2] = c;
	result[3][3] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sRotateZ

template <class Val>
ZMatrix<Val,4,4> sRotateZ(Val radians)
	{
	Val s = sin(radians);
	Val c = cos(radians);
	ZMatrix<Val,4,4> result;
	result[0][0] = c;
	result[1][0] = s;
	result[0][1] = -s;
	result[1][1] = c;
	result[2][2] = 1;
	result[3][3] = 1;
	return result;
	}

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Geometry__
