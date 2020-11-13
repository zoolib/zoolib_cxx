// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Util_Strim_h__
#define __ZooLib_Dataspace_Util_Strim_h__
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark -

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const std::set<RelationalAlgebra::RelHead>& iSet);

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Util_Strim_h__
