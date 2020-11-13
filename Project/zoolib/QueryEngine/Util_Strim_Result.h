// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Util_Strim_Result_h__
#define __ZooLib_QueryEngine_Util_Strim_Result_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Result.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark -

void sToStrim(const ChanW_UTF& iStrimW, const ZP<Result>& iResult);

} // namespace QueryEngine

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const ZP<QueryEngine::Result>& iResult);

} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Util_Strim_Result_h__
