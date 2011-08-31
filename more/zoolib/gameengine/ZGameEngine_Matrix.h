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

#ifndef __ZGameEngine_Matrix__
#define __ZGameEngine_Matrix__ 1
#include "zconfig.h"

namespace ZooLib {
namespace ZGameEngine {

// =================================================================================================
#pragma mark -
#pragma mark * Mat_T

template <class E, size_t R, size_t C> class Mat_T;

template <class E_p, size_t R_p, size_t C_p>
class Mat_T
	{
public:
	typedef E_p E;
	static const size_t R = R_p;
	static const size_t C = C_p;

	Mat_T()
		{
		for (size_t r = 0; r < R; ++r)
			{
			for (size_t c = 0; c < C; ++c)
				fE[r][c] = E(0);
			}
		}

	Mat_T(const Mat_T& iOther)
		{
		for (size_t r = 0; r < R; ++r)
			{
			for (size_t c = 0; c < C; ++c)
				fE[r][c] = iOther.fE[r][c];
			}
		}

	Mat_T& operator=(const Mat_T& iOther)
		{
		for (size_t r = 0; r < R; ++r)
			{
			for (size_t c = 0; c < C; ++c)
				fE[r][c] = iOther.fE[r][c];
			}
		return *this;
		}

	E* operator[](size_t r)
		{ return fE[r]; }

	const E* operator[](size_t r) const
		{ return fE[r]; }

	E fE[R][C];
	};

// =================================================================================================
#pragma mark -
#pragma mark * Mat_T, specialized for column vector

template <class E_p, size_t R_p>
class Mat_T<E_p, R_p, 1>
	{
public:
	typedef E_p E;
	static const size_t R = R_p;

	Mat_T()
		{
		for (size_t r = 0; r < R; ++r)
			fE[r][0] = E(0);
		}

	Mat_T(const Mat_T& iOther)
		{
		for (size_t r = 0; r < R; ++r)
			fE[r][0] = iOther.fE[r][0];
		}

	Mat_T& operator=(const Mat_T& iOther)
		{
		for (size_t r = 0; r < R; ++r)
			fE[r][0] = iOther.fE[r][0];
		return *this;
		}

	E& operator[](size_t r)
		{ return fE[r][0]; }

	const E operator[](size_t r) const
		{ return fE[r][0]; }

	E fE[R][1];
	};

// =================================================================================================
#pragma mark -
#pragma mark * Mat_T, specialized for row vector

template <class E_p, size_t C_p>
class Mat_T<E_p, 1, C_p>
	{
public:
	typedef E_p E;
	static const size_t C = C_p;

	Mat_T()
		{
		for (size_t c = 0; c < C; ++c)
			fE[c] = E(0);
		}

	Mat_T(const Mat_T& iOther)
		{
		for (size_t c = 0; c < C; ++c)
			fE[c] = iOther.fE[c];
		}

	Mat_T& operator=(const Mat_T& iOther)
		{
		for (size_t c = 0; c < C; ++c)
			fE[c] = iOther.fE[c];
		return *this;
		}

	E& operator[](size_t c)
		{ return fE[0][c]; }

	const E operator[](size_t c) const
		{ return fE[0][c]; }

	E fE[1][C];
	};

// =================================================================================================
#pragma mark -
#pragma mark * sHomogenous

template <class E, size_t R>
Mat_T<E,R+1,1> sHomogenous(const Mat_T<E,R,1>& iMat)
	{
	Mat_T<E,R+1,1> result;
	for (size_t r = 0; r < R; ++r)
		result.fE[r] = iMat.fE[r];
	result.fE[R] = 1;
	return result;
	}

template <class E, size_t C>
Mat_T<E,1,C+1> sHomogenous(const Mat_T<E,1,C>& iMat)
	{
	Mat_T<E,1,C+1> result;
	for (size_t c = 0; c < C; ++c)
		result.fE[c] = iMat.fE[c];
	result.fE[C] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sCartesian

template <class E, size_t R>
Mat_T<E,R-1,1> sCartesian(const Mat_T<E,R,1>& iMat)
	{
	Mat_T<E,R-1,1> result;
	for (size_t x = 0; x < R - 1; ++x)
		result.fE[x][0] = iMat[x][0];
	return result;
	}

template <class E, size_t C>
Mat_T<E,1,C-1> sCartesian(const Mat_T<E,1,C>& iMat)
	{
	Mat_T<E,1,C-1> result;
	for (size_t x = 0; x < C - 1; ++x)
		result.fE[0][x] = iMat[0][x];
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sIdentity

template <class E, size_t Dim>
Mat_T<E,Dim,Dim> sIdentity()
	{
	Mat_T<E,Dim,Dim> result;
	for (size_t x = 0; x < Dim; ++x)
		result.fE[x][x] = E(1);
	return result;
	}

template <class Mat>
Mat sIdentity()
	{ return sIdentity<typename Mat::E, Mat::R>(); }

// =================================================================================================
#pragma mark -
#pragma mark * sTransposed

template <class E, size_t R, size_t C>
Mat_T<E,C,R> sTransposed(const Mat_T<E,R,C>& iMat)
	{
	Mat_T<E,C,R> result;
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iMat.fE[r][c];
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sNonZero

template <class E, size_t R, size_t C>
Mat_T<E,R,C> sNonZero(const Mat_T<E,R,C>& iMat)
	{
	Mat_T<E,R,C> result;
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[4][c] = iMat.fE[r][c] ? E(1) : E(0);
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Multiplication (aka composition)

template <class E, size_t RL, size_t Common, size_t CR>
Mat_T<E,RL,CR> operator*(const Mat_T<E,RL,Common>& iLeft, const Mat_T<E,Common,CR>& iRight)
	{
	Mat_T<E,RL,CR> result;
	for (size_t r = 0; r < RL; ++r)
		{
		for (size_t c = 0; c < CR; ++c)
			{
			E val = 0;
			for (size_t o = 0; o < Common; ++o)
				val += iLeft.fE[r][o] * iRight.fE[o][c];
			result.fE[r][c] = val;
			}
		}
	return result;
	}

template <class E, size_t Dim>
Mat_T<E,Dim,Dim>& operator*=(Mat_T<E,Dim,Dim>& ioLeft, const Mat_T<E,Dim,Dim>& iRight)
	{ return ioLeft = ioLeft * iRight; }

// =================================================================================================
#pragma mark -
#pragma mark * Element-by-element multiplication

template <class E, size_t R, size_t C>
Mat_T<E,R,C> sTimes(const Mat_T<E,R,C>& iLeft, const Mat_T<E,R,C>& iRight)
	{
	Mat_T<E,R,C> result;
	for (size_t r = 0; r < R; ++r)
		{
		for (size_t c = 0; c < C; ++c)
			result.fE[r][c] = iLeft.fE[r][c] * iRight.fE[r][c];
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Addition

template <class E, size_t R, size_t C>
Mat_T<E,R,C> operator+(const Mat_T<E,R,C>& iLeft, const Mat_T<E,R,C>& iRight)
	{
	Mat_T<E,R,C> result;
	for (size_t r = 0; r < R; ++r)
		{
		for (size_t c = 0; c < C; ++c)
			result.fE[r][c] = iLeft.fE[r][c] + iRight.fE[r][c];
		}
	return result;
	}

template <class E, size_t R, size_t C>
Mat_T<E,R,C>& operator+=(Mat_T<E,R,C>& ioLeft, const Mat_T<E,R,C>& iRight)
	{ return ioLeft = ioLeft + iRight; }

// =================================================================================================
#pragma mark -
#pragma mark * Subtraction

template <class E, size_t R, size_t C>
Mat_T<E,R,C> operator-(const Mat_T<E,R,C>& iLeft, const Mat_T<E,R,C>& iRight)
	{
	Mat_T<E,R,C> result;
	for (size_t r = 0; r < R; ++r)
		{
		for (size_t c = 0; c < C; ++c)
			result.fE[r][c] = iLeft.fE[r][c] - iRight.fE[r][c];
		}
	return result;
	}

template <class E, size_t R, size_t C>
Mat_T<E,R,C>& operator-=(Mat_T<E,R,C>& ioLeft, const Mat_T<E,R,C>& iRight)
	{ return ioLeft = ioLeft - iRight; }

// =================================================================================================
#pragma mark -
#pragma mark * Scalar multiplication

template <class E, size_t R, size_t C, class T>
Mat_T<E,R,C> operator*(const Mat_T<E,R,C>& iLeft, T iRight)
	{
	Mat_T<E,R,C> result;
	for (size_t r = 0; r < R; ++r)
		{
		for (size_t c = 0; c < C; ++c)
			result.fE[r][c] = iLeft.fE[r][c] * E(iRight);
		}
	return result;
	}

template <class E, size_t R, size_t C, class T>
Mat_T<E,R,C>& operator*=(Mat_T<E,R,C>& ioLeft, T iRight)
	{ return ioLeft = ioLeft * iRight; }

// =================================================================================================
#pragma mark -
#pragma mark * Scalar division

template <class E, size_t R, size_t C, class T>
Mat_T<E,R,C> operator/(const Mat_T<E,R,C>& iLeft, T iRight)
	{
	Mat_T<E,R,C> result;
	for (size_t r = 0; r < R; ++r)
		{
		for (size_t c = 0; c < C; ++c)
			result.fE[r][c] = iLeft.fE[r][c] / E(iRight);
		}
	return result;
	}

template <class E, size_t R, size_t C, class T>
Mat_T<E,R,C>& operator/=(Mat_T<E,R,C>& ioLeft, T iRight)
	{ return ioLeft = ioLeft / E(iRight); }

// =================================================================================================
#pragma mark -
#pragma mark * Scalar addition

template <class E, size_t R, size_t C, class T>
Mat_T<E,R,C> operator+(const Mat_T<E,R,C>& iLeft, T iRight)
	{
	Mat_T<E,R,C> result;
	for (size_t r = 0; r < R; ++r)
		{
		for (size_t c = 0; c < C; ++c)
			result.fE[r][c] = iLeft.fE[r][c] + E(iRight);
		}
	return result;
	}

template <class E, size_t R, size_t C, class T>
Mat_T<E,R,C>& operator+=(Mat_T<E,R,C>& ioLeft, T iRight)
	{ return ioLeft = ioLeft + iRight; }

// =================================================================================================
#pragma mark -
#pragma mark * Scalar subtraction

template <class E, size_t R, size_t C, class T>
Mat_T<E,R,C> operator-(const Mat_T<E,R,C>& iLeft, T iRight)
	{
	Mat_T<E,R,C> result;
	for (size_t r = 0; r < R; ++r)
		{
		for (size_t c = 0; c < C; ++c)
			result.fE[r][c] = iLeft.fE[r][c] - E(iRight);
		}
	return result;
	}

template <class E, size_t R, size_t C, class T>
Mat_T<E,R,C>& operator-=(Mat_T<E,R,C>& ioLeft, T iRight)
	{ return ioLeft = ioLeft - iRight; }

// =================================================================================================
#pragma mark -
#pragma mark * sScale

template <class E, size_t Dim>
Mat_T<E,Dim,Dim> sScale(const Mat_T<E,Dim,1>& iVec)
	{
	Mat_T<E,Dim,Dim> result;
	for (size_t x = 0; x < Dim; ++x)
		result.fE[x][x] = iVec.fE[x][0];
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sTranslate

template <class E, size_t Dim>
Mat_T<E,Dim+1,Dim+1> sTranslate(const Mat_T<E,Dim,1>& iVec)
	{
	Mat_T<E,Dim+1,Dim+1> result = sIdentity<E,Dim+1>();
	for (size_t x = 0; x < Dim; ++x)
		result.fE[Dim][x] = iVec.fE[x][0];
	return result;
	}

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Matrix__
