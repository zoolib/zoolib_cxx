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

#ifndef __ZUtil_Strim_Matrix__
#define __ZUtil_Strim_Matrix__ 1
#include "zconfig.h"

#include "zoolib/ZMatrix.h"
#include "zoolib/ZUtil_Strim_Operators.h"

namespace ZooLib {

template <class E, size_t R>
const ZStrimW& operator<<(const ZStrimW& w, const ZMatrix<E,R,1>& mat)
	{
	w << "C[";
	for (size_t r = 0; r < R; ++r)
		{
		if (r)
			w << ", ";
		w << mat.fE[r][0];
		}
	w << "]";
	return w;
	}

template <class E, size_t C>
const ZStrimW& operator<<(const ZStrimW& w, const ZMatrix<E,1,C>& mat)
	{
	w << "R[";
	for (size_t c = 0; c < C; ++c)
		{
		if (c)
			w << ", ";
		w << mat.fE[0][c];
		}
	w << "]";
	return w;
	}

template <class E, size_t R, size_t C>
const ZStrimW& operator<<(const ZStrimW& w, const ZMatrix<E,R,C>& mat)
	{
	w << "[";
	for (size_t r = 0; r < R; ++r)
		{
		w << "[";
		for (size_t c = 0; c < C; ++c)
			{
			if (c)
				w << ", ";
			w << mat.fE[r][c];
			}
		w << "]";
		}
	w << "]";
	return w;
	}

} // namespace ZooLib

#endif // __ZUtil_Strim_Matrix__
