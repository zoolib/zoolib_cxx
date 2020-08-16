// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_ValPred_GetNames_h__
#define __ZooLib_ValPred_ValPred_GetNames_h__ 1
#include "zconfig.h"

#include "zoolib/ValPred/ValPred.h"

#include <set>

namespace ZooLib {

std::set<std::string> sGetNames(const ValPred& iValPred);

} // namespace ZooLib

#endif // __ZooLib_ValPred_ValPred_GetNames_h__
