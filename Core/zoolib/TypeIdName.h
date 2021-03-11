// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_TypeIdName_h__
#define __ZooLib_TypeIdName_h__ 1
#include "zconfig.h"

#include <typeinfo>

// =================================================================================================

namespace ZooLib {

template <typename T>
const char* sTypeIdName(const T& iObj)
	{ return typeid(iObj).name(); }

} // namespace ZooLib

#endif // __ZooLib_TypeIdName_h__
