
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

#ifndef __Zoolib_Matrix_h__
#define __Zoolib_Matrix_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_cmath.h"
#include "zoolib/Memory.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZTypes.h" // For null

namespace ZooLib {

// Matrices are specified as the number of columns, then the number of rows.

// =================================================================================================
#pragma mark - Matrix, general

template <class E, size_t C, size_t R> class Matrix;

template <class E_p, size_t C_p, size_t R_p>
class Matrix
	{
public:
	typedef E_p E;

	static const size_t C = C_p;
	static const size_t R = R_p;

	Matrix()
		{ sMemZero_T(fE); }

	Matrix(const Matrix& iOther)
		{ sMemCopy(fE, iOther.fE, sizeof(fE)); }

	Matrix& operator=(const Matrix& iOther)
		{
		sMemCopy(fE, iOther.fE, sizeof(fE));
		return *this;
		}

	Matrix(const null_t&)
		{}

	E* operator[](size_t c)
		{ return fE[c]; }

	const E* operator[](size_t c) const
		{ return fE[c]; }

	E fE[C][R];
	};

// =================================================================================================
#pragma mark - Matrix, specialized for column vector

template <class E_p, size_t R_p>
class Matrix<E_p,1,R_p>
	{
public:
	typedef E_p E;
	static const size_t R = R_p;

	Matrix()
		{ sMemZero_T(fE); }

	Matrix(const Matrix& iOther)
		{ sMemCopy(fE, iOther.fE, sizeof(fE)); }

	Matrix& operator=(const Matrix& iOther)
		{
		sMemCopy(fE, iOther.fE, sizeof(fE));
		return *this;
		}

	Matrix(const null_t&)
		{}

	Matrix(E iE)
		{
		for (size_t rr = 0; rr < R; ++rr)
			fE[0][rr] = iE;
		}

	E& operator[](size_t r)
		{ return fE[0][r]; }

	const E& operator[](size_t r) const
		{ return fE[0][r]; }

	E& Mut(size_t r)
		{ return fE[0][r]; }

	const E Get(size_t r) const
		{ return fE[0][r]; }

	E fE[1][R];
	};

// =================================================================================================
#pragma mark - Matrix, specialized for row vector

template <class E_p, size_t C_p>
class Matrix<E_p,C_p,1>
	{
public:
	typedef E_p E;
	static const size_t C = C_p;

	Matrix()
		{ sMemZero_T(fE); }

	Matrix(const Matrix& iOther)
		{ sMemCopy(fE, iOther.fE, sizeof(fE)); }

	Matrix& operator=(const Matrix& iOther)
		{
		sMemCopy(fE, iOther.fE, sizeof(fE));
		return *this;
		}

	Matrix(const null_t&)
		{}

	Matrix(E iE)
		{
		for (size_t cc = 0; cc < C; ++cc)
			fE[0][cc] = iE;
		}

	E& operator[](size_t c)
		{ return fE[c][0]; }

	const E& operator[](size_t c) const
		{ return fE[c][0]; }

	E& Mut(size_t c)
		{ return fE[c][0]; }

	const E Get(size_t c) const
		{ return fE[c][0]; }

	E fE[C][1];
	};

// =================================================================================================
#pragma mark - Matrix, square

template <class E_p, size_t Dim_p>
class Matrix<E_p,Dim_p,Dim_p>
	{
public:
	typedef E_p E;

	static const size_t C = Dim_p;
	static const size_t R = Dim_p;
	static const size_t Dim = Dim_p;

	Matrix()
		{ sMemZero_T(fE); }

	Matrix(const Matrix& iOther)
		{ sMemCopy(fE, iOther.fE, sizeof(fE)); }

	Matrix& operator=(const Matrix& iOther)
		{
		sMemCopy(fE, iOther.fE, sizeof(fE));
		return *this;
		}

	Matrix(const null_t&)
		{}

	Matrix(E iDiagonal)
		{
		for (size_t ii = 0; ii < Dim * Dim; ++ii)
			fE[0][ii] = E(0);
		for (size_t ii = 0; ii < Dim; ++ii)
			fE[ii][ii] = iDiagonal;
		}

	Matrix<E,1,Dim>& operator[](size_t c)
		{ return *reinterpret_cast<Matrix<E,1,Dim>*>(fE[c]); }

	const Matrix<E,1,Dim>& operator[](size_t c) const
		{ return *reinterpret_cast<const Matrix<E,1,Dim>*>(fE[c]); }

	E fE[C][R];
	};

// =================================================================================================
#pragma mark - operator== and operator!=

template <class E, size_t C, size_t R>
bool operator==(const Matrix<E,C,R>& iL, const Matrix<E,C,R>& iR)
	{
	for (size_t ii = 0; ii < C * R; ++ii)
		{
		if (iL.fE[0][ii] != iR.fE[0][ii])
			return false;
		}
	return true;
	}

template <class E, size_t C, size_t R>
bool operator!=(const Matrix<E,C,R>& iL, const Matrix<E,C,R>& iR)
	{ return not (iL == iR); }

// =================================================================================================
#pragma mark - sHomogenous (column vector)

template <class E, size_t R>
Matrix<E,1,R+1> sHomogenous(const Matrix<E,1,R>& iMat)
	{
	Matrix<E,1,R+1> result;
	for (size_t rr = 0; rr < R; ++rr)
		result.fE[0][rr] = iMat.fE[0][rr];
	result.fE[0][R] = 1;
	return result;
	}

// =================================================================================================
#pragma mark - sHomogenous (row vector)

template <class E, size_t C>
Matrix<E,C+1,1> sHomogenous(const Matrix<E,C,1>& iMat)
	{
	Matrix<E,C+1,1> result;
	for (size_t cc = 0; cc < C; ++cc)
		result.fE[cc][0] = iMat.fE[cc][0];
	result.fE[C][0] = 1;
	return result;
	}

// =================================================================================================
#pragma mark - sCartesian (column vector)

template <class E, size_t R>
Matrix<E,1,R-1> sCartesian(const Matrix<E,1,R>& iMat)
	{
	Matrix<E,1,R-1> result;
	const E last = iMat.fE[0][R-1];
	for (size_t r = 0; r < R - 1; ++r)
		result.fE[0][r] = iMat.fE[0][r]/last;
	return result;
	}

// =================================================================================================
#pragma mark - sCartesian (row vector)

template <class E, size_t C>
Matrix<E,C-1,1> sCartesian(const Matrix<E,C,1>& iMat)
	{
	Matrix<E,C-1,1> result;
	const E last = iMat.fE[C-1][0];
	for (size_t cc = 0; cc < C - 1; ++cc)
		result.fE[cc][0] = iMat.fE[cc][0]/last;
	return result;
	}

// =================================================================================================
#pragma mark - sDot (column vector)

template <class E, size_t R>
E sDot(const Matrix<E,1,R>& i0, const Matrix<E,1,R>& i1)
	{
	E result = 0;
	for (size_t rr = 0; rr < R; ++rr)
		result += i0.fE[0][rr] * i1.fE[0][rr];
	return result;
	}

// =================================================================================================
#pragma mark - sDot (row vector)

template <class E, size_t C>
E sDot(const Matrix<E,C,1>& i0, const Matrix<E,C,1>& i1)
	{
	E result = 0;
	for (size_t cc = 0; cc < C; ++cc)
		result += i0.fE[cc][0] * i1.fE[cc][0];
	return result;
	}

// =================================================================================================
#pragma mark - sCross (column vector)

template <class E, size_t R>
Matrix<E,1,R> sCross(const Matrix<E,1,R>& i0, const Matrix<E,1,R>& i1)
	{
	Matrix<E,1,R> result;
	for (size_t rr = 0; rr < R; ++rr)
		{
		for (size_t r0 = 0; r0 < R; ++r0)
			{
			if (r0 != rr)
				{
				for (size_t r1 = 0; r1 < R; ++r1)
					{
					if (r1 != rr)
						result.fE[0][rr] += i0.fE[0][r0] * i0.fE[0][r1];
					}
				}
			}
		}
	return result;
	}

// =================================================================================================
#pragma mark - sCross (row vector)

template <class E, size_t C>
Matrix<E,C,1> sCross(const Matrix<E,C,1>& i0, const Matrix<E,C,1>& i1)
	{
	Matrix<E,C,1> result;
	for (size_t cc = 0; cc < C; ++cc)
		{
		for (size_t c0 = 0; c0 < C; ++c0)
			{
			if (c0 != cc)
				{
				for (size_t c1 = 0; c1 < C; ++c1)
					{
					if (c1 != cc)
						result.fE[cc][0] += i0.fE[c0][0] * i0.fE[c1][0];
					}
				}
			}
		}
	return result;
	}

// =================================================================================================
#pragma mark - sIsIdentity

template <class E, size_t Dim>
bool sIsIdentity(const Matrix<E,Dim,Dim>& iMat)
	{
	const E& theID = iMat.fE[0][0];
	for (size_t c = 0; c < Dim; ++c)
		{
		for (size_t r = 0; r < Dim; ++r)
			{
			const E& theE = iMat.fE[c][r];
			if (c == r && theE != theID)
				return false;
			else if (theE != 0.0)
				return false;
			}
		}
	return true;
	}

// =================================================================================================
#pragma mark - sTransposed

template <class E, size_t C, size_t R>
Matrix<E,C,R> sTransposed(const Matrix<E,R,C>& iMat)
	{
	Matrix<E,C,R> result;
	for (size_t cc = 0; cc < C; ++cc)
		{
		for (size_t rr = 0; rr < R; ++rr)
			result.fE[cc][rr] = iMat.fE[rr][cc];
		}
	return result;
	}

// =================================================================================================
#pragma mark - sLengthSquared (column vector)

template <class E, size_t R>
E sLengthSquared(const Matrix<E,1,R>& iVec)
	{ return sDot(iVec, iVec); }

// =================================================================================================
#pragma mark - sLengthSquared (row vector)

template <class E, size_t C>
E sLengthSquared(const Matrix<E,C,1>& iVec)
	{ return sDot(iVec, iVec); }

// =================================================================================================
#pragma mark - sLength (column vector)

template <class E, size_t R>
E sLength(const Matrix<E,1,R>& iVec)
	{ return sqrt(sLengthSquared(iVec)); }

// =================================================================================================
#pragma mark - sLength (row vector)

template <class E, size_t C>
E sLength(const Matrix<E,C,1>& iVec)
	{ return sqrt(sLengthSquared(iVec)); }

// =================================================================================================
#pragma mark - sNormalized (row vector)

template <class E, size_t C>
Matrix<E,C,1> sNormalized(const Matrix<E,C,1>& iVec)
	{
	const E length = sLength(iVec);
	ZAssert(length > 0);
	Matrix<E,C,1> result(null);
	for (size_t cc = 0; cc < C; ++cc)
		result.fE[cc][0] = iVec.fE[cc][0] / length;
	return result;
	}

// =================================================================================================
#pragma mark - sNormalized (column vector)

template <class E, size_t R>
Matrix<E,1,R> sNormalized(const Matrix<E,1,R>& iVec)
	{
	const E length = sLength(iVec);
	ZAssert(length > 0);
	Matrix<E,1,R> result(null);
	for (size_t rr = 0; rr < R; ++rr)
		result.fE[0][rr] = iVec.fE[0][rr] / length;
	return result;
	}

// =================================================================================================
#pragma mark - sApply (binary function)

template <class Fun, class E, size_t C, size_t R>
Matrix<E,C,R> sApply(Fun iFun, const Matrix<E,C,R>& i0, const Matrix<E,C,R>& i1)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iFun(i0.fE[0][ii], i1.fE[0][ii]);
	return result;
	}

// =================================================================================================
#pragma mark - sApply (unary function)

template <class Fun, class E, size_t C, size_t R>
Matrix<E,C,R> sApply(Fun iFun, const Matrix<E,C,R>& iMat)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iFun(iMat.fE[0][ii]);
	return result;
	}

// =================================================================================================
#pragma mark - sNonZero

template <class E, size_t C, size_t R>
Matrix<E,C,R> sNonZero(const Matrix<E,C,R>& iMat)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iMat.fE[0][ii] ? E(1) : E(0);
	return result;
	}

// =================================================================================================
#pragma mark - Multiplication (aka composition)

template <class E, size_t RL, size_t Common, size_t CR>
Matrix<E,CR,RL> operator*(const Matrix<E,Common,RL>& iLHS, const Matrix<E,CR,Common>& iRHS)
	{
	Matrix<E,CR,RL> result(null);
	for (size_t rl = 0; rl < RL; ++rl)
		{
		for (size_t cr = 0; cr < CR; ++cr)
			{
			result.fE[cr][rl] = E(0);
			for (size_t o = 0; o < Common; ++o)
				result.fE[cr][rl] += iLHS.fE[o][rl] * iRHS.fE[cr][o];
			}
		}
	return result;
	}

template <class E, size_t Dim>
Matrix<E,Dim,Dim>& operator*=(Matrix<E,Dim,Dim>& ioLHS, const Matrix<E,Dim,Dim>& iRHS)
	{ return ioLHS = ioLHS * iRHS; }

// =================================================================================================
#pragma mark - Composition of undersized column vector

template <class E, size_t Common, size_t RL>
Matrix<E,1,Common> operator*(const Matrix<E,Common+1,RL>& iLHS, const Matrix<E,1,Common>& iRHS)
	{ return sCartesian(iLHS * sHomogenous(iRHS)); }

// =================================================================================================
#pragma mark - Element-by-element multiplication (general)

template <class E, size_t C, size_t R>
Matrix<E,C,R> sTimes(const Matrix<E,C,R>& iLHS, const Matrix<E,C,R>& iRHS)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iLHS.fE[0][ii] * iRHS.fE[0][ii];
	return result;
	}

// =================================================================================================
#pragma mark - Element-by-element multiplication (column vector)

template <class E, size_t R>
Matrix<E,1,R> operator*(const Matrix<E,1,R>& iLHS, const Matrix<E,1,R>& iRHS)
	{
	Matrix<E,1,R> result(null);
	for (size_t rr = 0; rr < R; ++rr)
		result.fE[0][rr] = iLHS.fE[0][rr] * iRHS.fE[0][rr];
	return result;
	}

template <class E, size_t R>
Matrix<E,1,R>& operator*=(Matrix<E,1,R>& ioLHS, const Matrix<E,1,R>& iRHS)
	{
	for (size_t rr = 0; rr < R; ++rr)
		ioLHS.fE[0][rr] *= iRHS.fE[0][rr];
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Element-by-element multiplication (row vector)

template <class E, size_t C>
Matrix<E,C,1> operator*(const Matrix<E,C,1>& iLHS, const Matrix<E,C,1>& iRHS)
	{
	Matrix<E,C,1> result(null);
	for (size_t cc = 0; cc < C; ++cc)
		result.fE[cc][0] = iLHS.fE[cc][0] * iRHS.fE[cc][0];
	return result;
	}

template <class E, size_t C>
Matrix<E,C,1>& operator*=(const Matrix<E,C,1>& ioLHS, const Matrix<E,C,1>& iRHS)
	{
	for (size_t cc = 0; cc < C; ++cc)
		ioLHS.fE[cc][0] *= iRHS.fE[cc][0];
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Element-by-element division (column vector)

template <class E, size_t R>
Matrix<E,1,R> operator/(const Matrix<E,1,R>& iLHS, const Matrix<E,1,R>& iRHS)
	{
	Matrix<E,1,R> result(null);
	for (size_t rr = 0; rr < R; ++rr)
		result.fE[0][rr] = iLHS.fE[0][rr] / iRHS.fE[0][rr];
	return result;
	}

template <class E, size_t R>
Matrix<E,1,R>& operator/=(Matrix<E,1,R>& ioLHS, const Matrix<E,1,R>& iRHS)
	{
	for (size_t rr = 0; rr < R; ++rr)
		ioLHS.fE[0][rr] /= iRHS.fE[0][rr];
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Element-by-element division (row vector)

template <class E, size_t C>
Matrix<E,C,1> operator/(const Matrix<E,C,1>& iLHS, const Matrix<E,C,1>& iRHS)
	{
	Matrix<E,C,1> result(null);
	for (size_t cc = 0; cc < C; ++cc)
		result.fE[cc][0] = iLHS.fE[cc][0] / iRHS.fE[cc][0];
	return result;
	}

template <class E, size_t C>
Matrix<E,C,1>& operator/=(const Matrix<E,C,1>& ioLHS, const Matrix<E,C,1>& iRHS)
	{
	for (size_t cc = 0; cc < C; ++cc)
		ioLHS.fE[cc][0] /= iRHS.fE[cc][0];
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Addition

template <class E, size_t C, size_t R>
Matrix<E,C,R> operator+(const Matrix<E,C,R>& iLHS, const Matrix<E,C,R>& iRHS)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iLHS.fE[0][ii] + iRHS.fE[0][ii];
	return result;
	}

template <class E, size_t C, size_t R>
Matrix<E,C,R>& operator+=(Matrix<E,C,R>& ioLHS, const Matrix<E,C,R>& iRHS)
	{
	for (size_t ii = 0; ii < C * R; ++ii)
		ioLHS.fE[0][ii] += iRHS.fE[0][ii];
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Subtraction

template <class E, size_t C, size_t R>
Matrix<E,C,R> operator-(const Matrix<E,C,R>& iLHS, const Matrix<E,C,R>& iRHS)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iLHS.fE[0][ii] - iRHS.fE[0][ii];
	return result;
	}

template <class E, size_t C, size_t R>
Matrix<E,C,R>& operator-=(Matrix<E,C,R>& ioLHS, const Matrix<E,C,R>& iRHS)
	{
	for (size_t ii = 0; ii < C * R; ++ii)
		ioLHS.fE[0][ii] -= iRHS.fE[0][ii];
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Negate

template <class E, size_t C, size_t R>
Matrix<E,C,R> operator-(const Matrix<E,C,R>& iMat)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = -iMat.fE[0][ii];
	return result;
	}

// =================================================================================================
#pragma mark - Scalar multiplication

template <class E, size_t C, size_t R, class T>
Matrix<E,C,R> operator*(const Matrix<E,C,R>& iLHS, T iRHS)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iLHS.fE[0][ii] * E(iRHS);
	return result;
	}

template <class E, size_t C, size_t R, class T>
Matrix<E,C,R>& operator*=(Matrix<E,C,R>& ioLHS, T iRHS)
	{
	for (size_t ii = 0; ii < C * R; ++ii)
		ioLHS.fE[0][ii] *= E(iRHS);
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Scalar division

template <class E, size_t C, size_t R, class T>
Matrix<E,C,R> operator/(const Matrix<E,C,R>& iLHS, T iRHS)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iLHS.fE[0][ii] / E(iRHS);
	return result;
	}

template <class E, size_t C, size_t R, class T>
Matrix<E,C,R>& operator/=(Matrix<E,C,R>& ioLHS, T iRHS)
	{
	for (size_t ii = 0; ii < C * R; ++ii)
		ioLHS.fE[0][ii] /= E(iRHS);
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Scalar addition

template <class E, size_t C, size_t R, class T>
Matrix<E,C,R> operator+(const Matrix<E,C,R>& iLHS, T iRHS)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iLHS.fE[0][ii] + E(iRHS);
	return result;
	}

template <class E, size_t C, size_t R, class T>
Matrix<E,C,R>& operator+=(Matrix<E,C,R>& ioLHS, T iRHS)
	{
	for (size_t ii = 0; ii < C * R; ++ii)
		ioLHS.fE[0][ii] += E(iRHS);
	return ioLHS;
	}

// =================================================================================================
#pragma mark - Scalar subtraction

template <class E, size_t C, size_t R, class T>
Matrix<E,C,R> operator-(const Matrix<E,C,R>& iLHS, T iRHS)
	{
	Matrix<E,C,R> result(null);
	for (size_t ii = 0; ii < C * R; ++ii)
		result.fE[0][ii] = iLHS.fE[0][ii] - E(iRHS);
	return result;
	}

template <class E, size_t C, size_t R, class T>
Matrix<E,C,R>& operator-=(Matrix<E,C,R>& ioLHS, T iRHS)
	{
	for (size_t ii = 0; ii < C * R; ++ii)
		ioLHS.fE[0][ii] -= E(iRHS);
	return ioLHS;
	}

// =================================================================================================
#pragma mark - sMinor

template <class E, size_t Dim>
Matrix<E,Dim-1,Dim-1> sMinor(const Matrix<E,Dim,Dim>& iMat, size_t iRow, size_t iCol)
	{
	Matrix<E,Dim-1,Dim-1> result(null);
	size_t ri = 0;
	for (size_t rr = 0; rr < Dim; ++rr)
		{
		if (rr == iRow)
			continue;

		size_t ci = 0;
		for (size_t cc = 0; cc < Dim; ++cc)
			{
			if (cc == iCol)
				continue;
			result.fE[ci][ri] = iMat.fE[cc][rr];
			++ci;
			}
		++ri;
		}
	return result;
	}

// =================================================================================================
#pragma mark - sDeterminant (general)

template <class E, size_t Dim>
E sDeterminant(const Matrix<E,Dim,Dim>& iMat)
	{
	ZAssert(Dim > 3);

	E result = 0.0;

	for (size_t cc = 0; cc < Dim; ++cc)
		{
		Matrix<E,Dim-1,Dim-1> minor = sMinor(iMat, 0, cc);
		if (cc & 1)
			result -= iMat.fE[cc][0] * sDeterminant(minor);
		else
			result += iMat.fE[cc][0] * sDeterminant(minor);
		}

	return result;
	}

// =================================================================================================
#pragma mark - sDeterminant (for 1x1 through 3x3)

template <class E>
E sDeterminant(const Matrix<E,1,1>& iMat)
	{ return iMat.fE[0][0]; }

template <class E>
E sDeterminant(const Matrix<E,2,2>& iMat)
	{ return iMat.fE[0][0] * iMat.fE[1][1] - iMat.fE[1][0] * iMat.fE[0][1]; }

template <class E>
E sDeterminant(const Matrix<E,3,3>& iMat)
	{
	const E aei = iMat.fE[0][0] * iMat.fE[1][1] * iMat.fE[2][2];
	const E bfg = iMat.fE[1][0] * iMat.fE[2][1] * iMat.fE[0][2];
	const E cdh = iMat.fE[2][0] * iMat.fE[0][1] * iMat.fE[1][2];

	const E ceg = iMat.fE[2][0] * iMat.fE[1][1] * iMat.fE[0][2];
	const E bdi = iMat.fE[1][0] * iMat.fE[0][1] * iMat.fE[2][2];
	const E afh = iMat.fE[0][0] * iMat.fE[2][1] * iMat.fE[1][2];

	return aei + bfg + cdh - ceg - bdi - afh;
	}

// =================================================================================================
#pragma mark - sInverse

template <class E, size_t Dim>
Matrix<E,Dim,Dim> sInverse(const Matrix<E,Dim,Dim>& iMat)
	{
	const E det = 1.0 / sDeterminant(iMat);

	Matrix<E,Dim,Dim> result(null);

	for (size_t cc = 0; cc < Dim; ++cc)
		{
		for (size_t rr = 0; rr < Dim; ++rr)
			{
			const E temp = det * sDeterminant(sMinor(iMat, cc, rr));
			if ((rr + cc) & 1)
				result[cc][rr] = -temp;
			else
				result[cc][rr] = temp;
			}
		}

	return result;
	}

// =================================================================================================
#pragma mark - Provision for out-of-line optimized version.

Matrix<float,4,4> sInverse(const Matrix<float,4,4>& iMat);

// =================================================================================================
#pragma mark - Optimized sComposeMat4

template <class E>
void sComposeMat4(const E iLHS[4][4], const E iRHS[4][4], E oDest[4][4])
	{
	oDest[0][0]
		= iRHS[0][0] * iLHS[0][0]
		+ iRHS[0][1] * iLHS[1][0]
		+ iRHS[0][2] * iLHS[2][0]
		+ iRHS[0][3] * iLHS[3][0]; 

	oDest[0][1]
		= iRHS[0][0] * iLHS[0][1]
		+ iRHS[0][1] * iLHS[1][1]
		+ iRHS[0][2] * iLHS[2][1]
		+ iRHS[0][3] * iLHS[3][1]; 

	oDest[0][2]
		= iRHS[0][0] * iLHS[0][2]
		+ iRHS[0][1] * iLHS[1][2]
		+ iRHS[0][2] * iLHS[2][2]
		+ iRHS[0][3] * iLHS[3][2]; 

	oDest[0][3]
		= iRHS[0][0] * iLHS[0][3]
		+ iRHS[0][1] * iLHS[1][3]
		+ iRHS[0][2] * iLHS[2][3]
		+ iRHS[0][3] * iLHS[3][3]; 

	oDest[1][0]
		= iRHS[1][0] * iLHS[0][0]
		+ iRHS[1][1] * iLHS[1][0]
		+ iRHS[1][2] * iLHS[2][0]
		+ iRHS[1][3] * iLHS[3][0]; 

	oDest[1][1]
		= iRHS[1][0] * iLHS[0][1]
		+ iRHS[1][1] * iLHS[1][1]
		+ iRHS[1][2] * iLHS[2][1]
		+ iRHS[1][3] * iLHS[3][1]; 

	oDest[1][2]
		= iRHS[1][0] * iLHS[0][2]
		+ iRHS[1][1] * iLHS[1][2]
		+ iRHS[1][2] * iLHS[2][2]
		+ iRHS[1][3] * iLHS[3][2]; 

	oDest[1][3]
		= iRHS[1][0] * iLHS[0][3]
		+ iRHS[1][1] * iLHS[1][3]
		+ iRHS[1][2] * iLHS[2][3]
		+ iRHS[1][3] * iLHS[3][3]; 

	oDest[2][0]
		= iRHS[2][0] * iLHS[0][0]
		+ iRHS[2][1] * iLHS[1][0]
		+ iRHS[2][2] * iLHS[2][0]
		+ iRHS[2][3] * iLHS[3][0]; 

	oDest[2][1]
		= iRHS[2][0] * iLHS[0][1]
		+ iRHS[2][1] * iLHS[1][1]
		+ iRHS[2][2] * iLHS[2][1]
		+ iRHS[2][3] * iLHS[3][1]; 

	oDest[2][2]
		= iRHS[2][0] * iLHS[0][2]
		+ iRHS[2][1] * iLHS[1][2]
		+ iRHS[2][2] * iLHS[2][2]
		+ iRHS[2][3] * iLHS[3][2]; 

	oDest[2][3]
		= iRHS[2][0] * iLHS[0][3]
		+ iRHS[2][1] * iLHS[1][3]
		+ iRHS[2][2] * iLHS[2][3]
		+ iRHS[2][3] * iLHS[3][3]; 

	oDest[3][0]
		= iRHS[3][0] * iLHS[0][0]
		+ iRHS[3][1] * iLHS[1][0]
		+ iRHS[3][2] * iLHS[2][0]
		+ iRHS[3][3] * iLHS[3][0]; 

	oDest[3][1]
		= iRHS[3][0] * iLHS[0][1]
		+ iRHS[3][1] * iLHS[1][1]
		+ iRHS[3][2] * iLHS[2][1]
		+ iRHS[3][3] * iLHS[3][1]; 

	oDest[3][2]
		= iRHS[3][0] * iLHS[0][2]
		+ iRHS[3][1] * iLHS[1][2]
		+ iRHS[3][2] * iLHS[2][2]
		+ iRHS[3][3] * iLHS[3][2]; 

	oDest[3][3]
		= iRHS[3][0] * iLHS[0][3]
		+ iRHS[3][1] * iLHS[1][3]
		+ iRHS[3][2] * iLHS[2][3]
		+ iRHS[3][3] * iLHS[3][3]; 
	}

void sComposeMat4(const float iLHS[4][4], const float iRHS[4][4], float oDest[4][4]);

template <class E>
Matrix<E,4,4> operator*(const Matrix<E,4,4>& iLHS, const Matrix<E,4,4>& iRHS)
	{
	Matrix<E,4,4> dest(null);
	sComposeMat4(iLHS.fE, iRHS.fE, dest.fE);
	return dest;
	}

// =================================================================================================
#pragma mark - Optimized sComposeMat4Vec4

template <class E>
void sComposeMat4Vec4(const E iLHS[4][4], const E* iRHS, E* oDest)
	{
	oDest[0]
		= iRHS[0] * iLHS[0][0]
		+ iRHS[1] * iLHS[1][0]
		+ iRHS[2] * iLHS[2][0]
		+ iRHS[3] * iLHS[3][0]; 

	oDest[1]
		= iRHS[0] * iLHS[0][1]
		+ iRHS[1] * iLHS[1][1]
		+ iRHS[2] * iLHS[2][1]
		+ iRHS[3] * iLHS[3][1]; 

	oDest[2]
		= iRHS[0] * iLHS[0][2]
		+ iRHS[1] * iLHS[1][2]
		+ iRHS[2] * iLHS[2][2]
		+ iRHS[3] * iLHS[3][2]; 

	oDest[3]
		= iRHS[0] * iLHS[0][3]
		+ iRHS[1] * iLHS[1][3]
		+ iRHS[2] * iLHS[2][3]
		+ iRHS[3] * iLHS[3][3];
	}

void sComposeMat4Vec4(const float iLHS[4][4], const float* iRHS, float* oDest);

template <class E>
Matrix<E,1,4> operator*(const Matrix<E,4,4>& iLHS, const Matrix<E,1,4>& iRHS)
	{
	Matrix<E,1,4> dest(null);
	sComposeMat4Vec4(iLHS.fE, iRHS.fE[0], dest.fE[0]);
	return dest;
	}

// =================================================================================================
#pragma mark - Optimized sComposeMat4Vec3_ToVec4

template <class E>
void sComposeMat4Vec3_ToVec4(const E iLHS[4][4], const E* iRHS, E* oDest)
	{
	oDest[0]
		= iRHS[0] * iLHS[0][0]
		+ iRHS[1] * iLHS[1][0]
		+ iRHS[2] * iLHS[2][0]
		+ iLHS[3][0]; 

	oDest[1]
		= iRHS[0] * iLHS[0][1]
		+ iRHS[1] * iLHS[1][1]
		+ iRHS[2] * iLHS[2][1]
		+ iLHS[3][1]; 

	oDest[2]
		= iRHS[0] * iLHS[0][2]
		+ iRHS[1] * iLHS[1][2]
		+ iRHS[2] * iLHS[2][2]
		+ iLHS[3][2]; 

	oDest[3]
		= iRHS[0] * iLHS[0][3]
		+ iRHS[1] * iLHS[1][3]
		+ iRHS[2] * iLHS[2][3]
		+ iLHS[3][3];
	}

void sComposeMat4Vec3_ToVec4(const float iLHS[4][4], const float* iRHS, float* oDest);

template <class E>
Matrix<E,1,3> operator*(const Matrix<E,4,4>& iLHS, const Matrix<E,1,3>& iRHS)
	{
	Matrix<E,1,4> dest(null);
	sComposeMat4Vec3_ToVec4(iLHS.fE, iRHS.fE[0], dest.fE[0]);
	return sCartesian(dest);
	}

} // namespace ZooLib

#endif // __Zoolib_Matrix_h__
