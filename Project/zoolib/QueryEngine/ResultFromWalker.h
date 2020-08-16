// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_ResultFromWalker_h__
#define __ZooLib_QueryEngine_ResultFromWalker_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Result.h"
#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - sQuery

ZP<Result> sResultFromWalker(ZP<Walker> iWalker);

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_ResultFromWalker_h__
