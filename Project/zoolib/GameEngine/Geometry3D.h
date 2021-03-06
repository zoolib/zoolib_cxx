// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Geometry3D_h__
#define __ZooLib_GameEngine_Geometry3D_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Geometry2D.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - sScale3

template <class Val>
Matrix<Val,4,4> sScale3(Val x, Val y, Val z)
	{
	Matrix<Val,4,4> result;
	result[0][0] = x;
	result[1][1] = y;
	result[2][2] = z;
	result[3][3] = 1;
	return result;
	}

template <class Val>
Matrix<Val,4,4> sScale3X(Val x)
	{ return sScale3<Val>(x, 1, 1); }

template <class Val>
Matrix<Val,4,4> sScale3Y(Val y)
	{ return sScale3<Val>(1, y, 1); }

template <class Val>
Matrix<Val,4,4> sScale3Z(Val z)
	{ return sScale3<Val>(1, 1, z); }

template <class Val>
Matrix<Val,4,4> sScale3XY(Val x, Val y)
	{ return sScale3<Val>(x, y, 1); }

template <class Val>
Matrix<Val,4,4> sScale3XZ(Val x, Val z)
	{ return sScale3<Val>(x, 1, z); }

template <class Val>
Matrix<Val,4,4> sScale3YZ(Val y, Val z)
	{ return sScale3<Val>(1, y, z); }

template <class Val>
Matrix<Val,4,4> sScale3(const Matrix<Val,1,3>& iCVec3)
	{ return sScale(iCVec3); }

// =================================================================================================
#pragma mark - sTranslate3

template <class Val>
Matrix<Val,4,4> sTranslate3(Val x, Val y, Val z)
	{
	Matrix<Val,4,4> result(1);
	result[3][0] = x;
	result[3][1] = y;
	result[3][2] = z;
	return result;
	}

template <class Val>
Matrix<Val,4,4> sTranslate3X(Val x)
	{ return sTranslate3<Val>(x, 0, 0); }

template <class Val>
Matrix<Val,4,4> sTranslate3Y(Val y)
	{ return sTranslate3<Val>(0, y, 0); }

template <class Val>
Matrix<Val,4,4> sTranslate3Z(Val z)
	{ return sTranslate3<Val>(0, 0, z); }

template <class Val>
Matrix<Val,4,4> sTranslate3XY(Val x, Val y)
	{ return sTranslate3<Val>(x, y, 0); }

template <class Val>
Matrix<Val,4,4> sTranslate3XZ(Val x, Val z)
	{ return sTranslate3<Val>(x, 0, z); }

template <class Val>
Matrix<Val,4,4> sTranslate3YZ(Val y, Val z)
	{ return sTranslate3<Val>(0, y, z); }

template <class Val>
Matrix<Val,4,4> sTranslate3(const Matrix<Val,1,3>& iCVec3)
	{ return sTranslate<Val>(iCVec3); }

// =================================================================================================
#pragma mark - sRotate3X

template <class Val>
Matrix<Val,4,4> sRotate3X(Val radians)
	{
	const Val s = sin(radians);
	const Val c = cos(radians);
	Matrix<Val,4,4> result;
	result.fE[0][0] = 1;
	result.fE[1][1] = c;
	result.fE[1][2] = s;
	result.fE[2][1] = -s;
	result.fE[2][2] = c;
	result.fE[3][3] = 1;
	return result;
	}

// =================================================================================================
#pragma mark - sRotate3Y

template <class Val>
Matrix<Val,4,4> sRotate3Y(Val radians)
	{
	const Val s = sin(radians);
	const Val c = cos(radians);
	Matrix<Val,4,4> result;
	result.fE[0][0] = c;
	result.fE[1][1] = 1;
	result.fE[0][2] = s;
	result.fE[2][0] = -s;
	result.fE[2][2] = c;
	result.fE[3][3] = 1;
	return result;
	}

// =================================================================================================
#pragma mark - sRotate3Z

template <class Val>
Matrix<Val,4,4> sRotate3Z(Val radians)
	{
	const Val s = sin(radians);
	const Val c = cos(radians);
	Matrix<Val,4,4> result;
	result.fE[0][0] = c;
	result.fE[0][1] = s;
	result.fE[1][0] = -s;
	result.fE[1][1] = c;
	result.fE[2][2] = 1;
	result.fE[3][3] = 1;
	return result;
	}

// =================================================================================================
#pragma mark - sShear3

template <class Val>
Matrix<Val,4,4> sShear3(Val xy, Val xz, Val yx, Val yz, Val zx, Val zy)
	{
	Matrix<Val,4,4> result(null);
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
#pragma mark - Extraction of coordinate subsets

// If you want X and Y, include Cartesian_Matrix.

template <class Val>
Val& Z(Matrix<Val,1,3>& iMat)
	{ return iMat[2]; }

template <class Val>
Val Z(const Matrix<Val,1,3>& iMat)
	{ return iMat[2]; }

template <class Val>
Matrix<Val,1,2> sXY(const Matrix<Val,1,3>& iMat)
	{ return Matrix<Val,1,2>(iMat[0], iMat[1]); }

template <class Val>
Matrix<Val,1,2> sYX(const Matrix<Val,1,3>& iMat)
	{ return Matrix<Val,1,2>(iMat[1], iMat[0]); }

template <class Val>
Matrix<Val,1,2> sXZ(const Matrix<Val,1,3>& iMat)
	{ return Matrix<Val,1,2>(iMat[0], iMat[2]); }

template <class Val>
Matrix<Val,1,2> sZX(const Matrix<Val,1,3>& iMat)
	{ return Matrix<Val,1,2>(iMat[2], iMat[0]); }

template <class Val>
Matrix<Val,1,2> sYZ(const Matrix<Val,1,3>& iMat)
	{ return Matrix<Val,1,2>(iMat[1], iMat[2]); }

template <class Val>
Matrix<Val,1,2> sZY(const Matrix<Val,1,3>& iMat)
	{ return Matrix<Val,1,2>(iMat[2], iMat[1]); }

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Geometry3D_h__
