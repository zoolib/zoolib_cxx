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
#pragma mark -
#pragma mark CVec ctors

template <class Val>
ZMatrix<Val,1,1> sCVec1(Val i0)
	{
	ZMatrix<Val,1,1> result(null);
	result.fE[0][0] = i0;
	return result;
	}

template <class Val>
ZMatrix<Val,1,2> sCVec2(Val i0, Val i1)
	{
	ZMatrix<Val,1,2> result(null);
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	return result;
	}

template <class Val>
ZMatrix<Val,1,3> sCVec3(Val i0, Val i1, Val i2)
	{
	ZMatrix<Val,1,3> result(null);
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	result.fE[0][2] = i2;
	return result;
	}

template <class Val>
ZMatrix<Val,1,4> sCVec4(Val i0, Val i1, Val i2, Val i3)
	{
	ZMatrix<Val,1,4> result(null);
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	result.fE[0][2] = i2;
	result.fE[0][3] = i3;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark CVec2 ctors

template <class Val>
ZMatrix<Val,1,2> sCVec2(Val iVal)
	{ return ZMatrix<Val,1,2>(iVal); }

template <class Val>
ZMatrix<Val,1,2> sCVec2()
	{ return ZMatrix<Val,1,2>(); }

// =================================================================================================
#pragma mark -
#pragma mark CVec3 ctors

template <class Val>
ZMatrix<Val,1,3> sCVec3(Val iVal)
	{ return ZMatrix<Val,1,3>(iVal); }

template <class Val>
ZMatrix<Val,1,3> sCVec3()
	{ return ZMatrix<Val,1,3>(); }

// =================================================================================================
#pragma mark -
#pragma mark RVec ctors

template <class Val>
ZMatrix<Val,1,1> sRVec1(Val i0)
	{
	ZMatrix<Val,1,1> result(null);
	result.fE[0][0] = i0;
	return result;
	}

template <class Val>
ZMatrix<Val,2,1> sRVec2(Val i0, Val i1)
	{
	ZMatrix<Val,1,2> result(null);
	result.fE[0][0] = i0;
	result.fE[1][0] = i1;
	return result;
	}

template <class Val>
ZMatrix<Val,3,1> sRVec3(Val i0, Val i1, Val i2)
	{
	ZMatrix<Val,1,3> result(null);
	result.fE[0][0] = i0;
	result.fE[1][0] = i1;
	result.fE[2][0] = i2;
	return result;
	}

template <class Val>
ZMatrix<Val,4,1> sRVec4(Val i0, Val i1, Val i2, Val i3)
	{
	ZMatrix<Val,1,4> result(null);
	result.fE[0][0] = i0;
	result.fE[1][0] = i1;
	result.fE[2][0] = i2;
	result.fE[3][0] = i3;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark RVec2 ctors

template <class Val>
ZMatrix<Val,2,1> sRVec2(Val iVal)
	{ return ZMatrix<Val,2,1>(iVal); }

template <class Val>
ZMatrix<Val,2,1> sRVec2()
	{ return ZMatrix<Val,2,1>(); }

// =================================================================================================
#pragma mark -
#pragma mark RVec3 ctors

template <class Val>
ZMatrix<Val,3,1> sRVec3(Val iVal)
	{ return ZMatrix<Val,3,1>(iVal); }

template <class Val>
ZMatrix<Val,3,1> sRVec3()
	{ return ZMatrix<Val,3,1>(); }

// =================================================================================================
#pragma mark -
#pragma mark sScale (taking column vector)

template <class E, size_t Dim>
ZMatrix<E,Dim+1,Dim+1> sScale(const ZMatrix<E,1,Dim>& iVec)
	{
	ZMatrix<E,Dim+1,Dim+1> result;
	for (size_t ii = 0; ii < Dim; ++ii)
		result.fE[ii][ii] = iVec.fE[0][ii];
	result.fE[Dim][Dim] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark sTranslate (taking column vector)

template <class E, size_t Dim>
ZMatrix<E,Dim+1,Dim+1> sTranslate(const ZMatrix<E,1,Dim>& iVec)
	{
	ZMatrix<E,Dim+1,Dim+1> result(E(1));
	for (size_t ii = 0; ii < Dim; ++ii)
		result.fE[Dim][ii] = iVec.fE[0][ii];
	return result;
	}

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Geometry_h__
