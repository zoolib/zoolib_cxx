// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Util_Strim_Walker_h__
#define __ZooLib_QueryEngine_Util_Strim_Walker_h__
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"
#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - sDumpWalkers

void sDumpWalkers(ZP<QueryEngine::Walker> iWalker, const ChanW_UTF& w);

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Util_Strim_Walker_h__
