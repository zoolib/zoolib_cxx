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

#ifndef __ZMatrix__
#define __ZMatrix__ 1
#include "zconfig.h"

namespace ZooLib {

// For compatibility with OpenGL we store in column-major order.
// Matrices are specified as the number of rows, then the number of columns.

// =================================================================================================
#pragma mark -
#pragma mark * ZMatrix

template <class E, size_t R, size_t C> class ZMatrix;

template <class E_p, size_t R_p, size_t C_p>
class ZMatrix
	{
public:
	typedef E_p E;
	static const size_t R = R_p;
	static const size_t C = C_p;

	ZMatrix(const null_t&)
		{}

	ZMatrix()
		{
		for (size_t c = 0; c < C; ++c)
			{
			for (size_t r = 0; r < R; ++r)
				fE[c][r] = E(0);
			}
		}

	ZMatrix(const ZMatrix& iOther)
		{
		for (size_t c = 0; c < C; ++c)
			{
			for (size_t r = 0; r < R; ++r)
				fE[c][r] = iOther.fE[c][r];
			}
		}

	ZMatrix& operator=(const ZMatrix& iOther)
		{
		for (size_t c = 0; c < C; ++c)
			{
			for (size_t r = 0; r < R; ++r)
				fE[c][r] = iOther.fE[c][r];
			}
		return *this;
		}

	E* operator[](size_t c)
		{ return fE[c]; }

	const E* operator[](size_t c) const
		{ return fE[c]; }

	E fE[C][R];
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMatrix, specialized for column vector

template <class E_p, size_t R_p>
class ZMatrix<E_p, R_p, 1>
	{
public:
	typedef E_p E;
	static const size_t R = R_p;

	ZMatrix(const null_t&)
		{}

	ZMatrix()
		{
		for (size_t r = 0; r < R; ++r)
			fE[0][r] = E(0);
		}

	ZMatrix(const ZMatrix& iOther)
		{
		for (size_t r = 0; r < R; ++r)
			fE[0][r] = iOther.fE[0][r];
		}

	ZMatrix& operator=(const ZMatrix& iOther)
		{
		for (size_t r = 0; r < R; ++r)
			fE[0][r] = iOther.fE[0][r];
		return *this;
		}

	E& operator[](size_t r)
		{ return fE[0][r]; }

	const E operator[](size_t r) const
		{ return fE[0][r]; }

	E fE[1][R];
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMatrix, specialized for row vector

template <class E_p, size_t C_p>
class ZMatrix<E_p, 1, C_p>
	{
public:
	typedef E_p E;
	static const size_t C = C_p;

	ZMatrix(const null_t&)
		{}

	ZMatrix()
		{
		for (size_t c = 0; c < C; ++c)
			fE[c][0] = E(0);
		}

	ZMatrix(const ZMatrix& iOther)
		{
		for (size_t c = 0; c < C; ++c)
			fE[c][0] = iOther.fE[c][0];
		}

	ZMatrix& operator=(const ZMatrix& iOther)
		{
		for (size_t c = 0; c < C; ++c)
			fE[c][0] = iOther.fE[c][0];
		return *this;
		}

	E& operator[](size_t c)
		{ return fE[c][0]; }

	const E operator[](size_t c) const
		{ return fE[c][0]; }

	E fE[C][1];
	};

// =================================================================================================
#pragma mark -
#pragma mark * sHomogenous (column vector)

template <class E, size_t R>
ZMatrix<E,R+1,1> sHomogenous(const ZMatrix<E,R,1>& iMat)
	{
	ZMatrix<E,R+1,1> result;
	for (size_t r = 0; r < R; ++r)
		result.fE[0][r] = iMat.fE[0][r];
	result.fE[0][R] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sHomogenous (row vector)

template <class E, size_t C>
ZMatrix<E,1,C+1> sHomogenous(const ZMatrix<E,1,C>& iMat)
	{
	ZMatrix<E,1,C+1> result;
	for (size_t c = 0; c < C; ++c)
		result.fE[c][0] = iMat.fE[c][0];
	result.fE[C][0] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sCartesian (column vector)

template <class E, size_t R>
ZMatrix<E,R-1,1> sCartesian(const ZMatrix<E,R,1>& iMat)
	{
	ZMatrix<E,R-1,1> result;
	for (size_t r = 0; r < R - 1; ++r)
		result.fE[0][r] = iMat.fE[0][r];
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sCartesian (row vector)

template <class E, size_t C>
ZMatrix<E,1,C-1> sCartesian(const ZMatrix<E,1,C>& iMat)
	{
	ZMatrix<E,1,C-1> result;
	for (size_t c = 0; c < C - 1; ++c)
		result.fE[c][0] = iMat.fE[c][0];
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sDot (column vector)

template <class E, size_t R>
E sDot(const ZMatrix<E,R,1>& i0, const ZMatrix<E,R,1>& i1)
	{
	E result = 0;
	for (size_t r = 0; r < R; ++r)
		result += i0.fE[0][r] * i1.fE[0][r];
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sDot (row vector)

template <class E, size_t C>
E sDot(const ZMatrix<E,1,C>& i0, const ZMatrix<E,1,C>& i1)
	{
	E result = 0;
	for (size_t c = 0; c < C; ++c)
		result += i0.fE[c][0] * i1.fE[c][0];
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sCross (column vector)

template <class E, size_t R>
ZMatrix<E,R,1> sCross(const ZMatrix<E,R,1>& i0, const ZMatrix<E,R,1>& i1)
	{
	ZMatrix<E,R,1> result;
	for (size_t r = 0; r < R; ++r)
		{
		for (size_t r0 = 0; r0 < R; ++r0)
			{
			if (r0 != r)
				{
				for (size_t r1 = 0; r1 < R; ++r1)
					{
					if (r1 != r)
						result[0][r] += i0[0][r0] * i0[0][r1];
					}
				}
			}
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sCross (row vector)

template <class E, size_t C>
ZMatrix<E,1,C> sCross(const ZMatrix<E,1,C>& i0, const ZMatrix<E,1,C>& i1)
	{
	ZMatrix<E,1,C> result;
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t c0 = 0; c0 < C; ++c0)
			{
			if (c0 != c)
				{
				for (size_t c1 = 0; c1 < C; ++c1)
					{
					if (c1 != c)
						result[c][0] += i0[c0][0] * i0[c1][0];
					}
				}
			}
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sIdentity

template <class E, size_t Dim>
ZMatrix<E,Dim,Dim> sIdentity()
	{
	ZMatrix<E,Dim,Dim> result;
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
ZMatrix<E,C,R> sTransposed(const ZMatrix<E,R,C>& iMat)
	{
	ZMatrix<E,C,R> result;
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iMat.fE[r][c];
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sLengthSquared (column vector)

template <class E, size_t R>
E sLengthSquared(const ZMatrix<E,R,1>& iVec)
	{ return sDot(iVec, iVec); }

// =================================================================================================
#pragma mark -
#pragma mark * sLengthSquared (row vector)

template <class E, size_t C>
E sLengthSquared(const ZMatrix<E,1,C>& iVec)
	{ return sDot(iVec, iVec); }

// =================================================================================================
#pragma mark -
#pragma mark * sLength (column vector)

template <class E, size_t R>
E sLength(const ZMatrix<E,R,1>& iVec)
	{ return sqrt(sLengthSquared(iVec)); }

// =================================================================================================
#pragma mark -
#pragma mark * sLength (row vector)

template <class E, size_t C>
E sLength(const ZMatrix<E,1,C>& iVec)
	{ return sqrt(sLengthSquared(iVec)); }

// =================================================================================================
#pragma mark -
#pragma mark * sNormalized (row vector)

template <class E, size_t C>
ZMatrix<E,1,C> sNormalized(const ZMatrix<E,1,C>& iVec)
	{
	const E length = sLength(iVec);
	ZAssert(length > 0);
	ZMatrix<E,1,C> result(null);
	for (size_t c = 0; c < C; ++c)
		result.fE[c][0] = iVec.fE[c][0] / length;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sNormalized (column vector)

template <class E, size_t R>
ZMatrix<E,R,1> sNormalized(const ZMatrix<E,R,1>& iVec)
	{
	const E length = sLength(iVec);
	ZAssert(length > 0);
	ZMatrix<E,R,1> result(null);
	for (size_t r = 0; r < R; ++r)
		result.fE[0][r] = iVec.fE[0][r] / length;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sApply (binary function)

template <class Fun, class E, size_t R, size_t C>
ZMatrix<E,R,C> sApply(Fun iFun, const ZMatrix<E,R,C>& i0, const ZMatrix<E,R,C>& i1)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iFun(i0.fE[c][r], i1.fE[c][r]);
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sApply (unary function)

template <class Fun, class E, size_t R, size_t C>
ZMatrix<E,R,C> sApply(Fun iFun, const ZMatrix<E,R,C>& iMat)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iFun(iMat.fE[c][r]);
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sNonZero

template <class E, size_t R, size_t C>
ZMatrix<E,R,C> sNonZero(const ZMatrix<E,R,C>& iMat)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iMat.fE[c][r] ? E(1) : E(0);
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Multiplication (aka composition)

template <class E, size_t RL, size_t Common, size_t CR>
ZMatrix<E,RL,CR> operator*(const ZMatrix<E,RL,Common>& iLeft, const ZMatrix<E,Common,CR>& iRight)
	{
	ZMatrix<E,RL,CR> result;
	for (size_t rl = 0; rl < RL; ++rl)
		{//###
		for (size_t cr = 0; cr < CR; ++cr)
			{
			for (size_t o = 0; o < Common; ++o)
				result.fE[cr][rl] += iLeft.fE[o][rl] * iRight.fE[cr][o];
			}
		}
	return result;
	}

template <class E, size_t RL, size_t Common, size_t CR>
ZMatrix<E,RL,CR>& operator*=(ZMatrix<E,RL,Common>& ioLeft, const ZMatrix<E,Common,CR>& iRight)
	{ return ioLeft = ioLeft * iRight; }

// =================================================================================================
#pragma mark -
#pragma mark * Element-by-element multiplication

template <class E, size_t R, size_t C>
ZMatrix<E,R,C> sTimes(const ZMatrix<E,R,C>& iLeft, const ZMatrix<E,R,C>& iRight)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] * iRight.fE[c][r];
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Addition

template <class E, size_t R, size_t C>
ZMatrix<E,R,C> operator+(const ZMatrix<E,R,C>& iLeft, const ZMatrix<E,R,C>& iRight)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] + iRight.fE[c][r];
		}
	return result;
	}

template <class E, size_t R, size_t C>
ZMatrix<E,R,C>& operator+=(ZMatrix<E,R,C>& ioLeft, const ZMatrix<E,R,C>& iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] += iRight.fE[c][r];
		}
	return ioLeft;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Subtraction

template <class E, size_t R, size_t C>
ZMatrix<E,R,C> operator-(const ZMatrix<E,R,C>& iLeft, const ZMatrix<E,R,C>& iRight)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] - iRight.fE[c][r];
		}
	return result;
	}

template <class E, size_t R, size_t C>
ZMatrix<E,R,C>& operator-=(ZMatrix<E,R,C>& ioLeft, const ZMatrix<E,R,C>& iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] -= iRight.fE[c][r];
		}
	return ioLeft;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Scalar multiplication

template <class E, size_t R, size_t C, class T>
ZMatrix<E,R,C> operator*(const ZMatrix<E,R,C>& iLeft, T iRight)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] * E(iRight);
		}
	return result;
	}

template <class E, size_t R, size_t C, class T>
ZMatrix<E,R,C>& operator*=(ZMatrix<E,R,C>& ioLeft, T iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] *= E(iRight);
		}
	return ioLeft;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Scalar division

template <class E, size_t R, size_t C, class T>
ZMatrix<E,R,C> operator/(const ZMatrix<E,R,C>& iLeft, T iRight)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] / E(iRight);
		}
	return result;
	}

template <class E, size_t R, size_t C, class T>
ZMatrix<E,R,C>& operator/=(ZMatrix<E,R,C>& ioLeft, T iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] /= E(iRight);
		}
	return ioLeft;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Scalar addition

template <class E, size_t R, size_t C, class T>
ZMatrix<E,R,C> operator+(const ZMatrix<E,R,C>& iLeft, T iRight)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] + E(iRight);
		}
	return result;
	}

template <class E, size_t R, size_t C, class T>
ZMatrix<E,R,C>& operator+=(ZMatrix<E,R,C>& ioLeft, T iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] += E(iRight);
		}
	return ioLeft;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Scalar subtraction

template <class E, size_t R, size_t C, class T>
ZMatrix<E,R,C> operator-(const ZMatrix<E,R,C>& iLeft, T iRight)
	{
	ZMatrix<E,R,C> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] - E(iRight);
		}
	return result;
	}

template <class E, size_t R, size_t C, class T>
ZMatrix<E,R,C>& operator-=(ZMatrix<E,R,C>& ioLeft, T iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] -= E(iRight);
		}
	return ioLeft;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sScale (taking column vector)

template <class E, size_t Dim>
ZMatrix<E,Dim+1,Dim+1> sScale(const ZMatrix<E,Dim,1>& iVec)
	{
	ZMatrix<E,Dim+1,Dim+1> result;
	for (size_t ii = 0; ii < Dim; ++ii)
		result.fE[ii][ii] = iVec.fE[0][ii];
	result.fE[Dim][Dim] = 1;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sTranslate (taking column vector)

template <class E, size_t Dim>
ZMatrix<E,Dim+1,Dim+1> sTranslate(const ZMatrix<E,Dim,1>& iVec)
	{
	ZMatrix<E,Dim+1,Dim+1> result = sIdentity<E,Dim+1>();
	for (size_t ii = 0; ii < Dim; ++ii)
		result.fE[Dim][ii] = iVec.fE[0][ii];
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sMinor

// ii is row, jj is col

template <class E, size_t Dim>
ZMatrix<E,Dim-1,Dim-1> sMinor(const ZMatrix<E,Dim,Dim>& iMat, size_t iRow, size_t iCol)
	{
	ZMatrix<E,Dim-1,Dim-1> result(null);
	size_t r = 0;
	for (size_t ii = 0; ii < Dim; ++ii)
		{
		if (ii == iRow)
			continue;

		size_t c = 0;
		for (size_t jj = 0; jj < Dim; ++jj)
			{
			if (jj == iCol)
				continue;
			result.fE[c][r] = iMat.fE[jj][ii];
			++c;
			}
		++r;
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sDeterminant (general)

template <class E, size_t Dim>
E sDeterminant(const ZMatrix<E,Dim,Dim>& iMat)
	{
	ZAssert(Dim > 3);

	E result = 0.0;

	for (size_t ii = 0; ii < Dim; ++ii)
		{
		ZMatrix<E,Dim-1,Dim-1> minor = sMinor(iMat, 0, ii);
		if (ii & 1)
			result -= iMat.fE[ii][0] * sDeterminant(minor);
		else
			result += iMat.fE[ii][0] * sDeterminant(minor);
		}

	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sDeterminant (for 1x1 through 3x3)

template <class E>
E sDeterminant(const ZMatrix<E,1,1>& iMat)
	{ return iMat.fE[0][0]; }

template <class E>
E sDeterminant(const ZMatrix<E,2,2>& iMat)
	{ return iMat.fE[0][0] * iMat.fE[1][1] - iMat.fE[1][0] * iMat.fE[0][1]; }

template <class E>
E sDeterminant(const ZMatrix<E,3,3>& iMat)
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
#pragma mark -
#pragma mark * sInverse

template <class E, size_t Dim>
ZMatrix<E,Dim,Dim> sInverse(const ZMatrix<E,Dim,Dim>& iMat)
	{
	const E det = 1.0 / sDeterminant(iMat);

	ZMatrix<E,Dim,Dim> result(null);

	for (size_t jj = 0; jj < Dim; ++jj)
		{
		for (size_t ii = 0; ii < Dim; ++ii)
			{
			const E temp = det * sDeterminant(sMinor(iMat, jj, ii));
			if ((ii + jj) & 1)
				result[jj][ii] = -temp;
			else
				result[jj][ii] = temp;
			}
		}

	return result;
	}

} // namespace ZooLib

#endif // __ZMatrix__
