// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_MatrixArray_h__
#define __ZooLib_MatrixArray_h__ 1
#include "zconfig.h"

#include "zoolib/Matrix.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

template <class E, size_t C, size_t R, size_t Length_p>
class MatrixArray
	{
public:
	MatrixArray() {}
	MatrixArray(const null_t&) {}

	const Matrix<E,C,R>& operator[](size_t i) const
		{ return fArray[i]; }

	Matrix<E,C,R>& operator[](size_t i)
		{ return fArray[i]; }

	Matrix<E,C,R> fArray[Length_p];
	};

// =================================================================================================
#pragma mark - operator== and operator!=

template <class E, size_t C, size_t R, size_t Length_p>
bool operator==(const MatrixArray<E,C,R,Length_p>& iL, const MatrixArray<E,C,R,Length_p>& iR)
	{
	for (size_t ii = 0; ii < Length_p; ++ii)
		{
		if (iL.fArray[ii] != iR.fArray[ii])
			return false;
		}
	return true;
	}

template <class E, size_t C, size_t R, size_t Length_p>
bool operator!=(const MatrixArray<E,C,R,Length_p>& iL, const MatrixArray<E,C,R,Length_p>& iR)
	{ return not (iL == iR); }

// =================================================================================================
#pragma mark -

template <class E, size_t C, size_t R, size_t Length_p, class P>
MatrixArray<E,C,R,Length_p> operator*(const MatrixArray<E,C,R,Length_p>& iLeft, const P& iRight)
	{
	MatrixArray<E,C,R,Length_p> result(null);
	for (size_t ii = 0; ii < Length_p; ++ii)
		result[ii] = iLeft[ii] * iRight;
	return result;
	}

template <class E, size_t C, size_t R, size_t Length_p, class P>
MatrixArray<E,C,R,Length_p>& operator*=(MatrixArray<E,C,R,Length_p>& ioLeft, const P& iRight)
	{
	for (size_t ii = 0; ii < Length_p; ++ii)
		ioLeft[ii] *= iRight;
	return ioLeft;
	}

// =================================================================================================
#pragma mark -

template <class E, size_t C, size_t R, size_t Length_p, class P>
MatrixArray<E,C,R,Length_p> operator/(const MatrixArray<E,C,R,Length_p>& iLeft, const P& iRight)
	{
	MatrixArray<E,C,R,Length_p> result(null);
	for (size_t ii = 0; ii < Length_p; ++ii)
		result[ii] = iLeft[ii] / iRight;
	return result;
	}

template <class E, size_t C, size_t R, size_t Length_p, class P>
MatrixArray<E,C,R,Length_p>& operator/=(MatrixArray<E,C,R,Length_p>& ioLeft, const P& iRight)
	{
	for (size_t ii = 0; ii < Length_p; ++ii)
		ioLeft[ii] /= iRight;
	return ioLeft;
	}

// =================================================================================================
#pragma mark -

template <class E, size_t C, size_t R, size_t Length_p, class P>
MatrixArray<E,C,R,Length_p> operator+(const MatrixArray<E,C,R,Length_p>& iLeft, const P& iRight)
	{
	MatrixArray<E,C,R,Length_p> result(null);
	for (size_t ii = 0; ii < Length_p; ++ii)
		result[ii] = iLeft[ii] + iRight;
	return result;
	}

template <class E, size_t C, size_t R, size_t Length_p, class P>
MatrixArray<E,C,R,Length_p>& operator+=(MatrixArray<E,C,R,Length_p>& ioLeft, const P& iRight)
	{
	for (size_t ii = 0; ii < Length_p; ++ii)
		ioLeft[ii] += iRight;
	return ioLeft;
	}

// =================================================================================================
#pragma mark -

template <class E, size_t C, size_t R, size_t Length_p, class P>
MatrixArray<E,C,R,Length_p> operator-(const MatrixArray<E,C,R,Length_p>& iLeft, const P& iRight)
	{
	MatrixArray<E,C,R,Length_p> result(null);
	for (size_t ii = 0; ii < Length_p; ++ii)
		result[ii] = iLeft[ii] - iRight;
	return result;
	}

template <class E, size_t C, size_t R, size_t Length_p, class P>
MatrixArray<E,C,R,Length_p>& operator-=(MatrixArray<E,C,R,Length_p>& ioLeft, const P& iRight)
	{
	for (size_t ii = 0; ii < Length_p; ++ii)
		ioLeft[ii] -= iRight;
	return ioLeft;
	}

} // namespace ZooLib

#endif // __ZooLib_MatrixArray_h__
