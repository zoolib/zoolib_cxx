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

#ifndef __ZooLib_Util_Chan_UTF_Matrix_h__
#define __ZooLib_Util_Chan_UTF_Matrix_h__ 1
#include "zconfig.h"

#include "zoolib/Matrix.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

namespace ZooLib {

template <class E, size_t R>
const ChanW_UTF& operator<<(const ChanW_UTF& w, const ZMatrix<E,1,R>& mat)
	{
	w << "C[";
	for (size_t r = 0; r < R; ++r)
		{
		if (r)
			w << ", ";
		w << mat.fE[0][r];
		}
	w << "]";
	return w;
	}

template <class E, size_t C>
const ChanW_UTF& operator<<(const ChanW_UTF& w, const ZMatrix<E,C,1>& mat)
	{
	w << "R[";
	for (size_t c = 0; c < C; ++c)
		{
		if (c)
			w << ", ";
		w << mat.fE[c][0];
		}
	w << "]";
	return w;
	}

template <class E, size_t C, size_t R>
const ChanW_UTF& operator<<(const ChanW_UTF& w, const ZMatrix<E,C,R>& mat)
	{
	w << "[";
	for (size_t r = 0; r < R; ++r)
		{
		w << "[";
		for (size_t c = 0; c < C; ++c)
			{
			if (c)
				w << ", ";
			w << mat.fE[c][r];
			}
		w << "]";
		}
	w << "]";
	return w;
	}

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_Matrix_h__
