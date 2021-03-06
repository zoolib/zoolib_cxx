// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Geometry2D_h__
#define __ZooLib_GameEngine_Geometry2D_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Geometry.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - sScale2

template <class E>
Matrix<E,3,3> sScale2(E x, E y)
	{
	Matrix<E,3,3> result;
	result[0][0] = x;
	result[1][1] = y;
	result[2][2] = 1;
	return result;
	}

template <class E>
Matrix<E,3,3> sScale2X(E x)
	{ return sScale2<E>(x, 1); }

template <class E>
Matrix<E,3,3> sScale2Y(E y)
	{ return sScale2<E>(1, y); }

template <class E>
Matrix<E,3,3> sScale2(const Matrix<E,1,2>& iCVec2)
	{ return sScale(iCVec2); }

// =================================================================================================
#pragma mark - sTranslate2

template <class E>
Matrix<E,3,3> sTranslate2(E x, E y)
	{
	Matrix<E,3,3> result(1);
	result[2][0] = x;
	result[2][1] = y;
	return result;
	}

template <class E>
Matrix<E,3,3> sTranslate2X(E x)
	{ return sTranslate2<E>(x, 0); }

template <class E>
Matrix<E,3,3> sTranslate2Y(E y)
	{ return sTranslate2<E>(0, y); }

template <class E>
Matrix<E,3,3> sTranslate2(const Matrix<E,1,2>& iCVec2)
	{ return sTranslate<E>(iCVec2); }

// =================================================================================================
#pragma mark - sRotate2Z

template <class E>
Matrix<E,3,3> sRotate2Z(E radians)
	{
	const E s = sin(radians);
	const E c = cos(radians);
	Matrix<E,3,3> result;
	result.fE[0][0] = c;
	result.fE[0][1] = s;
	result.fE[1][0] = -s;
	result.fE[1][1] = c;
	result.fE[2][2] = 1;
	return result;
	}

// =================================================================================================
#pragma mark - sShear2

template <class E>
Matrix<E,3,3> sShear2(E xy, E yx)
	{
	Matrix<E,3,3> result(null);
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

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Geometry2D_h__
