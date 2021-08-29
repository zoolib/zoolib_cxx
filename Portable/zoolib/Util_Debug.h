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
#pragma mark - LogMeister_Base

class LogMeister_Base
:	public Log::LogMeister
	{
public:
	LogMeister_Base(Log::EPriority iPriority);

// From Log::LogMeister
	virtual bool Enabled(Log::EPriority iPriority, const std::string& iName);

	virtual bool Enabled(Log::EPriority iPriority, const char* iName);

// Our protocol
	void SetLogPriority(Log::EPriority iLogPriority);

	Log::EPriority GetLogPriority();

	Log::EPriority pGetLogPriority();

private:
	Log::EPriority fLogPriority;
	};


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

void sWriteStandardLoggingText(const ChanW_UTF& iChanW,
	size_t iExtraSpace, bool iCompact,
	Log::EPriority iPriority,
	const std::string& iName, size_t iDepth, const std::string& iMessage);

// =================================================================================================
#pragma mark -

string8 sPrettyName(const std::type_info& iTI);

} // namespace Util_Debug
} // namespace ZooLib

#endif // __ZooLib_Util_Debug_h__
