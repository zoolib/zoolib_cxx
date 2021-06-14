// Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Debug_h__
#define __ZooLib_Util_Debug_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_UTF.h"
#include "zoolib/Log.h"
#include "zoolib/ThreadVal.h"

namespace ZooLib {
namespace Util_Debug {

// =================================================================================================
#pragma mark - Util_Debug

extern bool sCompact;

void sWriteBacktrace(const ChanW_UTF& iChanW);

void sInstall();

void sSetChanner(ZP<ChannerW_UTF> iChannerW);

void sSetLogPriority(Log::EPriority iLogPriority);

Log::EPriority sGetLogPriority();

typedef ThreadVal<Log::EPriority,struct Tag_LogPriorityPerThread> LogPriorityPerThread;

// =================================================================================================
#pragma mark -

string8 sPrettyName(const std::type_info& iTI);

} // namespace Util_Debug
} // namespace ZooLib

#endif // __ZooLib_Util_Debug_h__
