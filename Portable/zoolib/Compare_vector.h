// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_vector_h__
#define __ZooLib_Compare_vector_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"

#include <vector>

namespace ZooLib {

template <class T>
inline int sCompare_T(const std::vector<T>& iLeft, const std::vector<T>& iRight)
	{ return sCompareIterators_T(iLeft.begin(), iLeft.end(), iRight.begin(), iRight.end()); }

} // namespace ZooLib

#endif // __ZooLib_Compare_vector_h__
