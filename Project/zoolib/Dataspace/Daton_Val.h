// Copyright (c) 2014-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Daton_Val_h__
#define __ZooLib_Dataspace_Daton_Val_h__ 1
#include "zconfig.h"

#include "zoolib/Val_DB.h"

#include "zoolib/Dataspace/Daton.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Dataspace {

Val_DB sAsVal(const Daton& iDaton);
Daton sAsDaton(const Val_DB& iVal);

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Daton_Val_h__
