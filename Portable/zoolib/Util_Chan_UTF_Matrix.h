// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_UTF_Matrix_h__
#define __ZooLib_Util_Chan_UTF_Matrix_h__ 1
#include "zconfig.h"

#include "zoolib/Matrix.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

namespace ZooLib {

template <class E, size_t R>
const ChanW_UTF& operator<<(const ChanW_UTF& w, const Matrix<E,1,R>& mat)
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
const ChanW_UTF& operator<<(const ChanW_UTF& w, const Matrix<E,C,1>& mat)
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
const ChanW_UTF& operator<<(const ChanW_UTF& w, const Matrix<E,C,R>& mat)
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
