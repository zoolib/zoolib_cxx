// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Startable_SetName_h__
#define __ZooLib_Startable_SetName_h__ 1
#include "zconfig.h"

#include "zoolib/Startable.h"

#include <string>

namespace ZooLib {

ZP<Startable> sStartable_SetName(const std::string& iName, const ZP<Startable>& iStartable);

} // namespace ZooLib

#endif // __ZooLib_Startable_SetName_h__
