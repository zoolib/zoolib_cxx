// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_UTF_Matrix_h__
#define __ZooLib_Util_Chan_UTF_Matrix_h__ 1
#include "zconfig.h"

#include "zoolib/Matrix.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

namespace ZooLib {

template <class E, size_t R>
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const Matrix<E,1,R>& mat)
	{
	ww << "C[";
	for (size_t rr = 0; rr < R; ++rr)
		{
		if (rr)
			ww << ", ";
		w << mat.fE[0][rr];
		}
	ww << "]";
	return ww;
	}

template <class E, size_t C>
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const Matrix<E,C,1>& mat)
	{
	ww << "R[";
	for (size_t cc = 0; cc < C; ++cc)
		{
		if (cc)
			ww << ", ";
		ww << mat.fE[cc][0];
		}
	ww << "]";
	return ww;
	}

template <class E, size_t C, size_t R>
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const Matrix<E,C,R>& mat)
	{
	ww << "[";
	for (size_t rr = 0; rr < R; ++rr)
		{
		ww << "[";
		for (size_t cc = 0; cc < C; ++cc)
			{
			if (cc)
				ww << ", ";
			w << mat.fE[cc][rr];
			}
		ww << "]";
		}
	ww << "]";
	return ww;
	}

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_Matrix_h__
