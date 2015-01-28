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

#ifndef __ZMatrixArray_h__
#define __ZMatrixArray_h__ 1
#include "zconfig.h"

#include "zoolib/ZMatrix.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark 

template <class E, size_t C, size_t R, size_t Length_p>
class ZMatrixArray
	{
public:
	ZMatrixArray() {}
	ZMatrixArray(const null_t&) {}

	const ZMatrix<E,C,R>& operator[](size_t i) const
		{ return fArray[i]; }

	ZMatrix<E,C,R>& operator[](size_t i)
		{ return fArray[i]; }

	ZMatrix<E,C,R> fArray[Length_p];
	};

// =================================================================================================
#pragma mark -
#pragma mark 

template <class E, size_t C, size_t R, size_t Length_p, class P>
ZMatrixArray<E,C,R,Length_p> operator*(const ZMatrixArray<E,C,R,Length_p>& iLeft, const P& iRight)
	{
	ZMatrixArray<E,C,R,Length_p> result(null);
	for (size_t ii = 0; ii < Length_p; ++ii)
		result[ii] = iLeft[ii] * iRight;
	return result;
	}

template <class E, size_t C, size_t R, size_t Length_p, class P>
ZMatrixArray<E,C,R,Length_p>& operator*=(ZMatrixArray<E,C,R,Length_p>& ioLeft, const P& iRight)
	{
	for (size_t ii = 0; ii < Length_p; ++ii)
		ioLeft[ii] *= iRight;
	return ioLeft;
	}

// =================================================================================================
#pragma mark -
#pragma mark 

template <class E, size_t C, size_t R, size_t Length_p, class P>
ZMatrixArray<E,C,R,Length_p> operator/(const ZMatrixArray<E,C,R,Length_p>& iLeft, const P& iRight)
	{
	ZMatrixArray<E,C,R,Length_p> result(null);
	for (size_t ii = 0; ii < Length_p; ++ii)
		result[ii] = iLeft[ii] / iRight;
	return result;
	}

template <class E, size_t C, size_t R, size_t Length_p, class P>
ZMatrixArray<E,C,R,Length_p>& operator/=(ZMatrixArray<E,C,R,Length_p>& ioLeft, const P& iRight)
	{
	for (size_t ii = 0; ii < Length_p; ++ii)
		ioLeft[ii] /= iRight;
	return ioLeft;
	}

// =================================================================================================
#pragma mark -
#pragma mark 

template <class E, size_t C, size_t R, size_t Length_p, class P>
ZMatrixArray<E,C,R,Length_p> operator+(const ZMatrixArray<E,C,R,Length_p>& iLeft, const P& iRight)
	{
	ZMatrixArray<E,C,R,Length_p> result(null);
	for (size_t ii = 0; ii < Length_p; ++ii)
		result[ii] = iLeft[ii] + iRight;
	return result;
	}

template <class E, size_t C, size_t R, size_t Length_p, class P>
ZMatrixArray<E,C,R,Length_p>& operator+=(ZMatrixArray<E,C,R,Length_p>& ioLeft, const P& iRight)
	{
	for (size_t ii = 0; ii < Length_p; ++ii)
		ioLeft[ii] += iRight;
	return ioLeft;
	}

// =================================================================================================
#pragma mark -
#pragma mark 

template <class E, size_t C, size_t R, size_t Length_p, class P>
ZMatrixArray<E,C,R,Length_p> operator-(const ZMatrixArray<E,C,R,Length_p>& iLeft, const P& iRight)
	{
	ZMatrixArray<E,C,R,Length_p> result(null);
	for (size_t ii = 0; ii < Length_p; ++ii)
		result[ii] = iLeft[ii] - iRight;
	return result;
	}

template <class E, size_t C, size_t R, size_t Length_p, class P>
ZMatrixArray<E,C,R,Length_p>& operator-=(ZMatrixArray<E,C,R,Length_p>& ioLeft, const P& iRight)
	{
	for (size_t ii = 0; ii < Length_p; ++ii)
		ioLeft[ii] -= iRight;
	return ioLeft;
	}

} // namespace ZooLib

#endif // __ZMatrixArray_h__
