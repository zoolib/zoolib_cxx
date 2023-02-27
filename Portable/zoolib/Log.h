// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Log_h__
#define __ZooLib_Log_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"
#include "zoolib/Counted.h"
#include "zoolib/Safe.h"
#include "zoolib/ThreadVal.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ZQ.h"

#if ZCONFIG(Compiler, GCC)
	#define ZMACRO_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif ZCONFIG(Compiler, Clang)
	#define ZMACRO_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif ZCONFIG(Compiler, MSVC)
	// __FUNCDNAME__ is wordy and unintelligible
	#define ZMACRO_PRETTY_FUNCTION __FUNCTION__
#else
	#define ZMACRO_PRETTY_FUNCTION __FUNCTION__
#endif

// c == chan name
// p == priority
// f == facility (or function)2

#define ZLOGPF(c, p) const ZooLib::Log::ChanW& c = \
	ZooLib::Log::ChanW(ZooLib::Log::p, ZMACRO_PRETTY_FUNCTION, __LINE__)

#define ZLOGF(c, p) const ZooLib::Log::ChanW& c = \
	ZooLib::Log::ChanW(ZooLib::Log::p, __FUNCTION__, __LINE__)

#define ZLOG(c, p, f) const ZooLib::Log::ChanW& c = \
	ZooLib::Log::ChanW(ZooLib::Log::p, f)

#define ZLOGFUNCTION(p) ZooLib::Log::FunctionEntryExit \
	ZMACRO_Concat(theLogFEE_,__LINE__)(ZooLib::Log::p, ZMACRO_PRETTY_FUNCTION)

#define ZLOGFUNV(p, v) ZooLib::Log::FunctionEntryExit \
	ZMACRO_Concat(theLogFEE_,__LINE__)(ZooLib::Log::p, ZMACRO_PRETTY_FUNCTION, v)

#define ZLOGTRACE(p) \
	ZooLib::Log::sLogTrace(ZooLib::Log::p, __FILE__, __LINE__, ZMACRO_PRETTY_FUNCTION)

namespace ZooLib {
namespace Log {

// =================================================================================================
#pragma mark - Log::CallDepth

class CallDepth
	{
public:
	CallDepth(bool iActive = true);
	~CallDepth();

	static const CallDepth* sCurrent();

	static size_t sCount();
	static size_t sCountActive();

private:
	bool const fActive;
	const CallDepth* const fPrior;
	typedef ThreadVal<const CallDepth* , struct Tag_CallDepth> TVCallDepth;
	TVCallDepth fTV;
	};

// =================================================================================================
#pragma mark - EPriority

enum
	{
	ePriority_Emerg = 0, ///< System on fire?
	ePriority_Emergency = ePriority_Emerg,

	ePriority_Alert = 1, ///< action must be taken immediately

	ePriority_Crit = 2, ///< critical conditions
	ePriority_Critical = ePriority_Crit,

	ePriority_Err = 3, ///< error conditions
	ePriority_Error = ePriority_Err,

	ePriority_Warn = 4,///< warning conditions
	ePriority_Warning = ePriority_Warn,

	ePriority_Notice = 5, ///< normal but significant condition
	ePriority_Info = 6, ///< informational
	ePriority_Debug = 7, ///< debug-level messages

// more succinct aliases
	eErr = ePriority_Err,
	eError = ePriority_Err,

	eWarning = ePriority_Warn,
	eWarn = ePriority_Warn,

	eNotice = ePriority_Notice,

	eInfo = ePriority_Info,

	eDebug = ePriority_Debug
	};

typedef int EPriority;

EPriority sPriorityFromName(const std::string& iString);
std::string sNameFromPriority(EPriority iPriority);

// =================================================================================================
#pragma mark - Log::LogMeister

class LogMeister
:	public Counted
	{
public:
	virtual bool Enabled(EPriority iPriority, const std::string& iName);
	virtual bool Enabled(EPriority iPriority, const char* iName);
	virtual void LogIt(EPriority iPriority, const std::string& iName,
		size_t iDepth, const std::string& iMessage) = 0;
	};

extern ZP<LogMeister> sLogMeister;

typedef ThreadVal<ZP<LogMeister>, struct Tag_LogMeister> ThreadVal_LogMeister;

ZP<LogMeister> sCurrentLogMeister();

// =================================================================================================
#pragma mark - Log::ChanW

class ChanW : public virtual ChanW_UTF_Native8
	{
public:
	ChanW(EPriority iPriority, const std::string& iName_String);
	ChanW(EPriority iPriority, const char* iName_CharStar, int iLine = -1);
	~ChanW();

// From ChanW_UTF_Native8
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU);

// Our protocol
	explicit operator bool() const;

	void Emit() const;

private:
	void pEmit();

	const ZP<LogMeister> fLM;
	const int fPriority;
	mutable ZQ<std::string> fName_StringQ;
	const ZQ<const char*> fName_CharStarQ;
	const int fLine;
	mutable ZQ<std::string> fMessageQ;

public:
	mutable bool fOutdent;
	};

typedef ChanW CC;

// =================================================================================================
#pragma mark - Log::FunctionEntryExit

class FunctionEntryExit
	{
public:
	FunctionEntryExit(EPriority iPriority, const char* iFunctionName, const std::string& iMessage);
	FunctionEntryExit(EPriority iPriority, const char* iFunctionName);
	~FunctionEntryExit();

private:
	const CallDepth fCallDepth;
	EPriority fPriority;
	const char* fFunctionName;
	};

// =================================================================================================
#pragma mark - Log::sLogTrace

void sLogTrace(EPriority iPriority, const char* iFile, int iLine, const char* iFunctionName);

} // namespace Log
} // namespace ZooLib

#endif // __ZooLib_Log_h__
