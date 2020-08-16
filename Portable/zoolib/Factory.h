// Copyright (c) 2017 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Factory_h__
#define __ZooLib_Factory_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

template <class R>
using Factory = Callable<R()>;

} // namespace ZooLib

#endif // __ZooLib_Factory_h__
