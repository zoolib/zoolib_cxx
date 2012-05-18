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

#ifndef __ZMatrix_h__
#define __ZMatrix_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_cmath.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZTypes.h" // For null

namespace ZooLib {

// For compatibility with OpenGL we store in column-major order.
// Matrices are specified as the number of columns, then the number of rows.

// =================================================================================================
// MARK: - ZMatrix

template <class E, size_t C, size_t R> class ZMatrix;

template <class E_p, size_t C_p, size_t R_p>
class ZMatrix
	{
public:
	typedef E_p E;

	static const size_t C = C_p;
	static const size_t R = R_p;

	ZMatrix(const null_t&)
		{}

	ZMatrix()
		{
		for (size_t i = 0; i < C * R; ++i)
			fE[0][i] = E(0);
		}

	ZMatrix(const ZMatrix& iOther)
		{
		for (size_t i = 0; i < C * R; ++i)
			fE[0][i] = iOther[0][i];
		}

	ZMatrix& operator=(const ZMatrix& iOther)
		{
		for (size_t i = 0; i < C * R; ++i)
			fE[0][i] = iOther[0][i];
		return *this;
		}

	E* operator[](size_t c)
		{ return fE[c]; }

	const E* operator[](size_t c) const
		{ return fE[c]; }

	E fE[C][R];
	};

// =================================================================================================
// MARK: - ZMatrix, specialized for column vector

template <class E_p, size_t R_p>
class ZMatrix<E_p,1,R_p>
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

	const E& operator[](size_t r) const
		{ return fE[0][r]; }

	E& GetMutable(size_t r)
		{ return fE[0][r]; }

	const E Get(size_t r) const
		{ return fE[0][r]; }

	E fE[1][R];
	};

// =================================================================================================
// MARK: - ZMatrix, specialized for row vector

template <class E_p, size_t C_p>
class ZMatrix<E_p,C_p,1>
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

	const E& operator[](size_t c) const
		{ return fE[c][0]; }

	E& GetMutable(size_t c)
		{ return fE[c][0]; }

	const E Get(size_t c) const
		{ return fE[c][0]; }

	E fE[C][1];
	};

// =================================================================================================
// MARK: - operator== and operator!=

template <class E, size_t C, size_t R>
bool operator==(const ZMatrix<E,C,R>& iL, const ZMatrix<E,C,R>& iR)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			{
			if (iL[c][r] != iR[c][r])
				return false;
			}
		}
	return true;
	}

template <class E, size_t C, size_t R>
bool operator!=(const ZMatrix<E,C,R>& iL, const ZMatrix<E,C,R>& iR)
	{ return not (iL == iR); }

// =================================================================================================
// MARK: - sHomogenous (column vector)

template <class E, size_t R>
ZMatrix<E,1,R+1> sHomogenous(const ZMatrix<E,1,R>& iMat)
	{
	ZMatrix<E,1,R+1> result;
	for (size_t r = 0; r < R; ++r)
		result.fE[0][r] = iMat.fE[0][r];
	result.fE[0][R] = 1;
	return result;
	}

// =================================================================================================
// MARK: - sHomogenous (row vector)

template <class E, size_t C>
ZMatrix<E,C+1,1> sHomogenous(const ZMatrix<E,C,1>& iMat)
	{
	ZMatrix<E,C+1,1> result;
	for (size_t c = 0; c < C; ++c)
		result.fE[c][0] = iMat.fE[c][0];
	result.fE[C][0] = 1;
	return result;
	}

// =================================================================================================
// MARK: - sCartesian (column vector)

template <class E, size_t R>
ZMatrix<E,1,R-1> sCartesian(const ZMatrix<E,1,R>& iMat)
	{
	ZMatrix<E,1,R-1> result;
	const E last = iMat.fE[0][R-1];
	for (size_t r = 0; r < R - 1; ++r)
		result.fE[0][r] = iMat.fE[0][r]/last;
	return result;
	}

// =================================================================================================
// MARK: - sCartesian (row vector)

template <class E, size_t C>
ZMatrix<E,C-1,1> sCartesian(const ZMatrix<E,C,1>& iMat)
	{
	ZMatrix<E,C-1,1> result;
	const E last = iMat.fE[C-1][0];
	for (size_t c = 0; c < C - 1; ++c)
		result.fE[c][0] = iMat.fE[c][0]/last;
	return result;
	}

// =================================================================================================
// MARK: - sDot (column vector)

template <class E, size_t R>
E sDot(const ZMatrix<E,1,R>& i0, const ZMatrix<E,1,R>& i1)
	{
	E result = 0;
	for (size_t r = 0; r < R; ++r)
		result += i0.fE[0][r] * i1.fE[0][r];
	return result;
	}

// =================================================================================================
// MARK: - sDot (row vector)

template <class E, size_t C>
E sDot(const ZMatrix<E,C,1>& i0, const ZMatrix<E,C,1>& i1)
	{
	E result = 0;
	for (size_t c = 0; c < C; ++c)
		result += i0.fE[c][0] * i1.fE[c][0];
	return result;
	}

// =================================================================================================
// MARK: - sCross (column vector)

template <class E, size_t R>
ZMatrix<E,1,R> sCross(const ZMatrix<E,1,R>& i0, const ZMatrix<E,1,R>& i1)
	{
	ZMatrix<E,1,R> result;
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
// MARK: - sCross (row vector)

template <class E, size_t C>
ZMatrix<E,C,1> sCross(const ZMatrix<E,C,1>& i0, const ZMatrix<E,C,1>& i1)
	{
	ZMatrix<E,C,1> result;
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
// MARK: - sIdentity

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

template <class E, size_t Dim>
bool sIsIdentity(const ZMatrix<E,Dim,Dim>& iMat)
	{
	const E& theID = iMat[0][0];
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
// MARK: - sTransposed

template <class E, size_t C, size_t R>
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
// MARK: - sLengthSquared (column vector)

template <class E, size_t R>
E sLengthSquared(const ZMatrix<E,1,R>& iVec)
	{ return sDot(iVec, iVec); }

// =================================================================================================
// MARK: - sLengthSquared (row vector)

template <class E, size_t C>
E sLengthSquared(const ZMatrix<E,C,1>& iVec)
	{ return sDot(iVec, iVec); }

// =================================================================================================
// MARK: - sLength (column vector)

template <class E, size_t R>
E sLength(const ZMatrix<E,1,R>& iVec)
	{ return sqrt(sLengthSquared(iVec)); }

// =================================================================================================
// MARK: - sLength (row vector)

template <class E, size_t C>
E sLength(const ZMatrix<E,C,1>& iVec)
	{ return sqrt(sLengthSquared(iVec)); }

// =================================================================================================
// MARK: - sNormalized (row vector)

template <class E, size_t C>
ZMatrix<E,C,1> sNormalized(const ZMatrix<E,C,1>& iVec)
	{
	const E length = sLength(iVec);
	ZAssert(length > 0);
	ZMatrix<E,C,1> result(null);
	for (size_t c = 0; c < C; ++c)
		result.fE[c][0] = iVec.fE[c][0] / length;
	return result;
	}

// =================================================================================================
// MARK: - sNormalized (column vector)

template <class E, size_t R>
ZMatrix<E,1,R> sNormalized(const ZMatrix<E,1,R>& iVec)
	{
	const E length = sLength(iVec);
	ZAssert(length > 0);
	ZMatrix<E,1,R> result(null);
	for (size_t r = 0; r < R; ++r)
		result.fE[0][r] = iVec.fE[0][r] / length;
	return result;
	}

// =================================================================================================
// MARK: - sApply (binary function)

template <class Fun, class E, size_t C, size_t R>
ZMatrix<E,C,R> sApply(Fun iFun, const ZMatrix<E,C,R>& i0, const ZMatrix<E,C,R>& i1)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iFun(i0.fE[c][r], i1.fE[c][r]);
		}
	return result;
	}

// =================================================================================================
// MARK: - sApply (unary function)

template <class Fun, class E, size_t C, size_t R>
ZMatrix<E,C,R> sApply(Fun iFun, const ZMatrix<E,C,R>& iMat)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iFun(iMat.fE[c][r]);
		}
	return result;
	}

// =================================================================================================
// MARK: - sNonZero

template <class E, size_t C, size_t R>
ZMatrix<E,C,R> sNonZero(const ZMatrix<E,C,R>& iMat)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iMat.fE[c][r] ? E(1) : E(0);
		}
	return result;
	}

// =================================================================================================
// MARK: - Multiplication (aka composition)

template <class E, size_t RL, size_t Common, size_t CR>
ZMatrix<E,CR,RL> operator*(const ZMatrix<E,Common,RL>& iLeft, const ZMatrix<E,CR,Common>& iRight)
	{
	ZMatrix<E,CR,RL> result(null);
	for (size_t rl = 0; rl < RL; ++rl)
		{
		for (size_t cr = 0; cr < CR; ++cr)
			{
			result.fE[cr][rl] = E(0);
			for (size_t o = 0; o < Common; ++o)
				result.fE[cr][rl] += iLeft.fE[o][rl] * iRight.fE[cr][o];
			}
		}
	return result;
	}

template <class E, size_t Dim>
ZMatrix<E,Dim,Dim>& operator*=(ZMatrix<E,Dim,Dim>& ioLeft, const ZMatrix<E,Dim,Dim>& iRight)
	{ return ioLeft = ioLeft * iRight; }

// =================================================================================================
// MARK: - Composition

template <class E, size_t Common, size_t RL>
ZMatrix<E,1,Common> operator*(const ZMatrix<E,Common+1,RL>& iLeft, const ZMatrix<E,1,Common>& iRight)
	{ return sCartesian(iLeft * sHomogenous(iRight)); }

// =================================================================================================
// MARK: - Element-by-element multiplication

template <class E, size_t C, size_t R>
ZMatrix<E,C,R> sTimes(const ZMatrix<E,C,R>& iLeft, const ZMatrix<E,C,R>& iRight)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] * iRight.fE[c][r];
		}
	return result;
	}

// =================================================================================================
// MARK: - Addition

template <class E, size_t C, size_t R>
ZMatrix<E,C,R> operator+(const ZMatrix<E,C,R>& iLeft, const ZMatrix<E,C,R>& iRight)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] + iRight.fE[c][r];
		}
	return result;
	}

template <class E, size_t C, size_t R>
ZMatrix<E,C,R>& operator+=(ZMatrix<E,C,R>& ioLeft, const ZMatrix<E,C,R>& iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] += iRight.fE[c][r];
		}
	return ioLeft;
	}

// =================================================================================================
// MARK: - Subtraction

template <class E, size_t C, size_t R>
ZMatrix<E,C,R> operator-(const ZMatrix<E,C,R>& iLeft, const ZMatrix<E,C,R>& iRight)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] - iRight.fE[c][r];
		}
	return result;
	}

template <class E, size_t C, size_t R>
ZMatrix<E,C,R>& operator-=(ZMatrix<E,C,R>& ioLeft, const ZMatrix<E,C,R>& iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] -= iRight.fE[c][r];
		}
	return ioLeft;
	}

// =================================================================================================
// MARK: - Negate

template <class E, size_t C, size_t R>
ZMatrix<E,C,R> operator-(const ZMatrix<E,C,R>& iMat)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = -iMat.fE[c][r];
		}
	return result;
	}

// =================================================================================================
// MARK: - Scalar multiplication

template <class E, size_t C, size_t R, class T>
ZMatrix<E,C,R> operator*(const ZMatrix<E,C,R>& iLeft, T iRight)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] * E(iRight);
		}
	return result;
	}

template <class E, size_t C, size_t R, class T>
ZMatrix<E,C,R>& operator*=(ZMatrix<E,C,R>& ioLeft, T iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] *= E(iRight);
		}
	return ioLeft;
	}

// =================================================================================================
// MARK: - Scalar division

template <class E, size_t C, size_t R, class T>
ZMatrix<E,C,R> operator/(const ZMatrix<E,C,R>& iLeft, T iRight)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] / E(iRight);
		}
	return result;
	}

template <class E, size_t C, size_t R, class T>
ZMatrix<E,C,R>& operator/=(ZMatrix<E,C,R>& ioLeft, T iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] /= E(iRight);
		}
	return ioLeft;
	}

// =================================================================================================
// MARK: - Scalar addition

template <class E, size_t C, size_t R, class T>
ZMatrix<E,C,R> operator+(const ZMatrix<E,C,R>& iLeft, T iRight)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] + E(iRight);
		}
	return result;
	}

template <class E, size_t C, size_t R, class T>
ZMatrix<E,C,R>& operator+=(ZMatrix<E,C,R>& ioLeft, T iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] += E(iRight);
		}
	return ioLeft;
	}

// =================================================================================================
// MARK: - Scalar subtraction

template <class E, size_t C, size_t R, class T>
ZMatrix<E,C,R> operator-(const ZMatrix<E,C,R>& iLeft, T iRight)
	{
	ZMatrix<E,C,R> result(null);
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			result.fE[c][r] = iLeft.fE[c][r] - E(iRight);
		}
	return result;
	}

template <class E, size_t C, size_t R, class T>
ZMatrix<E,C,R>& operator-=(ZMatrix<E,C,R>& ioLeft, T iRight)
	{
	for (size_t c = 0; c < C; ++c)
		{
		for (size_t r = 0; r < R; ++r)
			ioLeft.fE[c][r] -= E(iRight);
		}
	return ioLeft;
	}

// =================================================================================================
// MARK: - sMinor

template <class E, size_t Dim>
ZMatrix<E,Dim-1,Dim-1> sMinor(const ZMatrix<E,Dim,Dim>& iMat, size_t iRow, size_t iCol)
	{
	ZMatrix<E,Dim-1,Dim-1> result(null);
	size_t r = 0;
	for (size_t rr = 0; rr < Dim; ++rr)
		{
		if (rr == iRow)
			continue;

		size_t c = 0;
		for (size_t cc = 0; cc < Dim; ++cc)
			{
			if (cc == iCol)
				continue;
			result.fE[c][r] = iMat.fE[cc][rr];
			++c;
			}
		++r;
		}
	return result;
	}

// =================================================================================================
// MARK: - sDeterminant (general)

template <class E, size_t Dim>
E sDeterminant(const ZMatrix<E,Dim,Dim>& iMat)
	{
	ZAssert(Dim > 3);

	E result = 0.0;

	for (size_t c = 0; c < Dim; ++c)
		{
		ZMatrix<E,Dim-1,Dim-1> minor = sMinor(iMat, 0, c);
		if (c & 1)
			result -= iMat.fE[c][0] * sDeterminant(minor);
		else
			result += iMat.fE[c][0] * sDeterminant(minor);
		}

	return result;
	}

// =================================================================================================
// MARK: - sDeterminant (for 1x1 through 3x3)

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
// MARK: - sInverse

template <class E, size_t Dim>
ZMatrix<E,Dim,Dim> sInverse(const ZMatrix<E,Dim,Dim>& iMat)
	{
	const E det = 1.0 / sDeterminant(iMat);

	ZMatrix<E,Dim,Dim> result(null);

	for (size_t c = 0; c < Dim; ++c)
		{
		for (size_t r = 0; r < Dim; ++r)
			{
			const E temp = det * sDeterminant(sMinor(iMat, c, r));
			if ((r + c) & 1)
				result[c][r] = -temp;
			else
				result[c][r] = temp;
			}
		}

	return result;
	}

} // namespace ZooLib

#endif // __ZMatrix_h__
