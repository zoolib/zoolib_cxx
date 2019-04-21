/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#ifndef __Zoolib_GameEngine_ColorMatrix_h__
#define __Zoolib_GameEngine_ColorMatrix_h__ 1
#include "zconfig.h"

#include "zoolib/Matrix.h"

namespace ZooLib {
namespace GameEngine {

// From https://docs.rainmeter.net/tips/colormatrix-guide/

template <class E>
using ColorMatrix = Matrix<E,5,5>;

template <class E>
ColorMatrix<E> sBrightness(E iBrightness)
	{
	ColorMatrix<E> result(E(1));

	// Brightness is a translation on the RGB elements.
	result.fE[4][0] = iBrightness;
	result.fE[4][1] = iBrightness;
	result.fE[4][2] = iBrightness;

	return result;
	}

template <class E>
ColorMatrix<E> sContrast(E iContrast)
	{
	ColorMatrix<E> result;

	// Contrast is a scaling on the RGB elements.
	result.fE[0][0] = iContrast;
	result.fE[1][1] = iContrast;
	result.fE[2][2] = iContrast;
	result.fE[3][3] = 1;
	result.fE[4][4] = 1;

	// With a translation so the scaling is centered around 0.5.
	E t = (1 - iContrast) / 2;

	result.fE[4][0] = t;
	result.fE[4][1] = t;
	result.fE[4][2] = t;

	return result;
	}

template <class E>
ColorMatrix<E> sSaturation(E iSaturation)
	{
	ColorMatrix<E> result;

	const E s = iSaturation;
	const E sr = (1 - s) * 0.3086;
	const E sg = (1 - s) * 0.6094;
	const E sb = (1 - s) * 0.0820;

	result.fE[0][0] = sr + s;
	result.fE[0][1] = sr;
	result.fE[0][2] = sr;

	result.fE[1][0] = sg;
	result.fE[1][1] = sg + s;
	result.fE[1][2] = sg;

	result.fE[2][0] = sb;
	result.fE[2][1] = sb;
	result.fE[2][2] = sb + s;

	result.fE[3][3] = 1;
	result.fE[4][4] = 1;

	return result;
	}

template <class E>
ColorMatrix<E> sInvert()
	{
	ColorMatrix<E> result;

	result.fE[0][0] = -1;
	result.fE[1][1] = -1;
	result.fE[2][2] = -1;
	result.fE[3][3] = 1;
	result.fE[4][4] = 1;

	result.fE[4][0] = 1;
	result.fE[4][1] = 1;
	result.fE[4][2] = 1;

	return result;
	}

// This one is a little different from sSaturation(0)
template <class E>
ColorMatrix<E> sGrayscale()
	{
	ColorMatrix<E> result;

	result.fE[0][0] = 0.33;
	result.fE[0][1] = 0.33;
	result.fE[0][2] = 0.33;

	result.fE[1][0] = 0.59;
	result.fE[1][1] = 0.59;
	result.fE[1][2] = 0.59;

	result.fE[2][0] = 0.11;
	result.fE[2][1] = 0.11;
	result.fE[2][2] = 0.11;

	result.fE[3][3] = 1;
	result.fE[4][4] = 1;

	return result;
	}

template <class E>
ColorMatrix<E> sBlackAndWhite()
	{
	ColorMatrix<E> result;

	result.fE[0][0] = 1.5;
	result.fE[0][1] = 1.5;
	result.fE[0][2] = 1.5;

	result.fE[1][0] = 1.5;
	result.fE[1][1] = 1.5;
	result.fE[1][2] = 1.5;

	result.fE[2][0] = 1.5;
	result.fE[2][1] = 1.5;
	result.fE[2][2] = 1.5;

	result.fE[3][3] = 1;

	result.fE[4][0] = -1;
	result.fE[4][1] = -1;
	result.fE[4][2] = -1;

	result.fE[4][4] = 1;

	return result;
	}

template <class E>
ColorMatrix<E> sWhiteToAlpha()
	{
	ColorMatrix<E> result(E(1));

	result.fE[3][0] = -1;
	result.fE[3][1] = -1;
	result.fE[3][2] = -1;

	return result;
	}

// Check these
template <class E>
ColorMatrix<E> sSepia()
	{
	ColorMatrix<E> result;

	result.fE[0][0] = 0.393;
	result.fE[0][1] = 0.769;
	result.fE[0][2] = 0.189;

	result.fE[1][0] = 0.349;
	result.fE[1][1] = 0.686;
	result.fE[1][2] = 0.168;

	result.fE[2][0] = 0.272;
	result.fE[2][1] = 0.534;
	result.fE[2][2] = 0.131;

	result.fE[3][3] = 1;
	result.fE[4][4] = 1;

	return result;
	}

template <class E>
ColorMatrix<E> sPolaroid()
	{
	ColorMatrix<E> result;

	result.fE[0][0] = 1;

	result.fE[0][1] = -0.122;
	result.fE[0][2] = -0.016;
	result.fE[0][4] = -0.03;

	result.fE[1][1] = 1.378;
	result.fE[1][2] = -0.016;
	result.fE[1][4] = 0.05;

	result.fE[2][1] = -0.122;
	result.fE[2][2] = 1.483;
	result.fE[2][4] = -0.02;

	result.fE[3][3] = 1;
	result.fE[4][4] = 1;

	return result;
	}

} // namespace GameEngine
} // namespace ZooLib

#endif // __Zoolib_GameEngine_ColorMatrix_h__
