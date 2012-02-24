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

#ifndef __ZGameEngine_Geometry2D_h__
#define __ZGameEngine_Geometry2D_h__ 1
#include "zconfig.h"

#include "zoolib/gameengine/ZGameEngine_Geometry.h"

namespace ZooLib {
namespace ZGameEngine {
// =================================================================================================
// MARK: - sScale2

template <class Val>
ZMatrix<Val,3,3> sScale2(Val x, Val y)
	{
	ZMatrix<Val,3,3> result;
	result[0][0] = x;
	result[1][1] = y;
	result[2][2] = 1;
	return result;
	}

template <class Val>
ZMatrix<Val,3,3> sScale2X(Val x)
	{ return sScale2<Val>(x, 1); }

template <class Val>
ZMatrix<Val,3,3> sScale2Y(Val y)
	{ return sScale2<Val>(1, y); }

template <class Val>
ZMatrix<Val,3,3> sScale2(const ZMatrix<Val,1,2>& iCVec2)
	{ return sScale(iCVec2); }

// =================================================================================================
// MARK: - sTranslate2

template <class Val>
ZMatrix<Val,3,3> sTranslate2(Val x, Val y)
	{
	ZMatrix<Val,3,3> result = sIdentity<Val,3>();
	result[2][0] = x;
	result[2][1] = y;
	return result;
	}

template <class Val>
ZMatrix<Val,3,3> sTranslate2X(Val x)
	{ return sTranslate2<Val>(x, 0); }

template <class Val>
ZMatrix<Val,3,3> sTranslate2Y(Val y)
	{ return sTranslate2<Val>(0, y); }

template <class Val>
ZMatrix<Val,3,3> sTranslate2(const ZMatrix<Val,1,2>& iCVec2)
	{ return sTranslate<Val>(iCVec2); }

// =================================================================================================
// MARK: - sRotate2Z

template <class Val>
ZMatrix<Val,3,3> sRotate2Z(Val radians)
	{
	const Val s = sin(radians);
	const Val c = cos(radians);
	ZMatrix<Val,3,3> result;
	result.fE[0][0] = c;
	result.fE[0][1] = s;
	result.fE[1][0] = -s;
	result.fE[1][1] = c;
	result.fE[2][2] = 1;
	return result;
	}

// =================================================================================================
// MARK: - sShear2

template <class Val>
ZMatrix<Val,3,3> sShear2(Val xy, Val yx)
	{
	ZMatrix<Val,3,3> result(null);
	result.fE[0][0] = 1;
	result.fE[0][1] = xy;
	result.fE[0][2] = 0;

	result.fE[1][0] = yx;
	result.fE[1][1] = 1;
	result.fE[1][2] = 0;

	result.fE[2][0] = 0;
	result.fE[2][1] = 0;
	result.fE[2][2] = 1;

	return result;
	}

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Geometry2D_h__
