// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __Zoolib_GameEngine_Geometry_h__
#define __Zoolib_GameEngine_Geometry_h__ 1
#include "zconfig.h"

#include "zoolib/Matrix.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - CVec1 ctors

template <class Val>
Matrix<Val,1,1> sCVec1()
	{ return Matrix<Val,1,1>(); }

template <class Val>
Matrix<Val,1,1> sCVec1(Val i0)
	{
	Matrix<Val,1,1> result(null);
	result.fE[0][0] = i0;
	return result;
	}

// =================================================================================================
#pragma mark - CVec2 ctors

template <class Val>
Matrix<Val,1,2> sCVec2()
	{ return Matrix<Val,1,2>(); }

template <class Val>
Matrix<Val,1,2> sCVec2(Val iVal)
	{ return Matrix<Val,1,2>(iVal); }

template <class Val>
Matrix<Val,1,2> sCVec2(Val i0, Val i1)
	{
	Matrix<Val,1,2> result(null);
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	return result;
	}

// =================================================================================================
#pragma mark - CVec3 ctors

template <class Val>
Matrix<Val,1,3> sCVec3()
	{ return Matrix<Val,1,3>(); }

template <class Val>
Matrix<Val,1,3> sCVec3(Val iVal)
	{ return Matrix<Val,1,3>(iVal); }

template <class Val>
Matrix<Val,1,3> sCVec3(Val i0, Val i1, Val i2)
	{
	Matrix<Val,1,3> result(null);
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	result.fE[0][2] = i2;
	return result;
	}

// =================================================================================================
#pragma mark - CVec4 ctors

template <class Val>
Matrix<Val,1,4> sCVec4()
	{ return Matrix<Val,1,4>(); }

template <class Val>
Matrix<Val,1,4> sCVec4(Val iVal)
	{ return Matrix<Val,1,4>(iVal); }

template <class Val>
Matrix<Val,1,4> sCVec4(Val i0, Val i1, Val i2, Val i3)
	{
	Matrix<Val,1,4> result(null);
	result.fE[0][0] = i0;
	result.fE[0][1] = i1;
	result.fE[0][2] = i2;
	result.fE[0][3] = i3;
	return result;
	}

// =================================================================================================
#pragma mark - sScale (taking column vector)

template <class E, size_t Dim>
Matrix<E,Dim+1,Dim+1> sScale(const Matrix<E,1,Dim>& iVec)
	{
	Matrix<E,Dim+1,Dim+1> result;
	for (size_t ii = 0; ii < Dim; ++ii)
		result.fE[ii][ii] = iVec.fE[0][ii];
	result.fE[Dim][Dim] = 1;
	return result;
	}

template <class E, size_t Dim>
Matrix<E,Dim+1,Dim+1> sScale(E const (&iArray)[Dim])
	{
	Matrix<E,Dim+1,Dim+1> result;
	for (size_t ii = 0; ii < Dim; ++ii)
		result.fE[ii][ii] = iArray[ii];
	result.fE[Dim][Dim] = 1;
	return result;
	}

// =================================================================================================
#pragma mark - sTranslate (taking column vector)

template <class E, size_t Dim>
Matrix<E,Dim+1,Dim+1> sTranslate(const Matrix<E,1,Dim>& iVec)
	{
	Matrix<E,Dim+1,Dim+1> result(E(1));
	for (size_t ii = 0; ii < Dim; ++ii)
		result.fE[Dim][ii] = iVec.fE[0][ii];
	return result;
	}

template <class E, size_t Dim>
Matrix<E,Dim+1,Dim+1> sTranslate(E const (&iArray)[Dim])
	{
	Matrix<E,Dim+1,Dim+1> result(E(1));
	for (size_t ii = 0; ii < Dim; ++ii)
		result.fE[Dim][ii] = iArray[ii];
	return result;
	}

} // namespace GameEngine
} // namespace ZooLib

#endif // __Zoolib_GameEngine_Geometry_h__
