// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Channer_UTF_h__
#define __ZooLib_Channer_UTF_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF.h"
#include "zoolib/Channer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

typedef ChannerR<UTF32> ChannerR_UTF;
typedef ChannerU<UTF32> ChannerU_UTF;
typedef ChannerW<UTF32> ChannerW_UTF;

typedef ChannerRU<UTF32> ChannerRU_UTF;

} // namespace ZooLib

#endif // __ZooLib_Channer_UTF_h__
