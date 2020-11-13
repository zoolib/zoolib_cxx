// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Util_Strim_Daton_h__
#define __ZooLib_Dataspace_Util_Strim_Daton_h__
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"
#include "zoolib/Dataspace/Daton.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const Dataspace::Daton& iDaton);

} // namespace ZooLib

#endif // __ZooLib_Dataspace_Util_Strim_Daton_h__
