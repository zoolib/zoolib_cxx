// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_UTF_MatrixArray_h__
#define __ZooLib_Util_Chan_UTF_MatrixArray_h__ 1
#include "zconfig.h"

#include "zoolib/MatrixArray.h"

namespace ZooLib {

template <class E, size_t C, size_t R, size_t Length_p>
const ChanW_UTF& operator<<(const ChanW_UTF& w, const MatrixArray<E,C,R,Length_p>& iArray)
	{
	w << "Array(" << Length_p << ")[";
	for (size_t x = 0; x < Length_p; ++x)
		w << iArray[x];
	w << "]";
	
	return w;
	}

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_MatrixArray_h__
