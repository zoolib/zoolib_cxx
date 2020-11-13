// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_UTF_MatrixArray_h__
#define __ZooLib_Util_Chan_UTF_MatrixArray_h__ 1
#include "zconfig.h"

#include "zoolib/MatrixArray.h"

namespace ZooLib {

template <class E, size_t C, size_t R, size_t Length_p>
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const MatrixArray<E,C,R,Length_p>& iArray)
	{
	ww << "Array(" << Length_p << ")[";
	for (size_t xx = 0; xx < Length_p; ++xx)
		ww << iArray[xx];
	ww << "]";
	
	return ww;
	}

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_MatrixArray_h__
