// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Startable_h__
#define __ZooLib_Startable_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

typedef Callable<void()> Startable;

} // namespace ZooLib

#endif // __ZooLib_Startable_h__
