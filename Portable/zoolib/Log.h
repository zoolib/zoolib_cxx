/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZooLib_Log_h__
#define __ZooLib_Log_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"
#include "zoolib/Compat_operator_bool.h"
#include "zoolib/Safe.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ZCounted.h"
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

// s == strim name
// p == priority
// f == facility

#define ZLOGPF(s, p) const ZooLib::Log::S& s = \
	ZooLib::Log::S(ZooLib::Log::p, ZMACRO_PRETTY_FUNCTION, __LINE__)

#define ZLOGF(s, p) const ZooLib::Log::S& s = \
	ZooLib::Log::S(ZooLib::Log::p, __FUNCTION__, __LINE__)

#define ZLOG(s, p, f) const ZooLib::Log::S& s = \
	ZooLib::Log::S(ZooLib::Log::p, f)

#define ZLOGFUNCTION(p) ZooLib::Log::FunctionEntryExit \
	ZMACRO_Concat(theLogFEE_,__LINE__)(ZooLib::Log::p, ZMACRO_PRETTY_FUNCTION)

#define ZLOGFUNV(p, v) ZooLib::Log::FunctionEntryExit \
	ZMACRO_Concat(theLogFEE_,__LINE__)(ZooLib::Log::p, ZMACRO_PRETTY_FUNCTION, v)

#define ZLOGTRACE(p) \
	ZooLib::Log::sLogTrace(ZooLib::Log::p, __FILE__, __LINE__, ZMACRO_PRETTY_FUNCTION)

namespace ZooLib {
namespace Log {

// =================================================================================================
// MARK: - Log::CallDepth

class CallDepth
	{
public:
	CallDepth(bool iActive = true);
	~CallDepth();

	static size_t sCount();
	static size_t sCountActive();

private:
	bool const fActive;
	const CallDepth* const fPrior;
	};

// =================================================================================================
// MARK: - EPriority

enum
	{
	ePriority_Emerg = 0, ///< System on fire?
	ePriority_Alert = 1, ///< action must be taken immediately
	ePriority_Crit = 2, ///< critical conditions
	ePriority_Err = 3, ///< error conditions
	ePriority_Warning = 4,///< warning conditions
	ePriority_Notice = 5, ///< normal but significant condition
	ePriority_Info = 6, ///< informational
	ePriority_Debug = 7, ///< debug-level messages

// more succinct aliases
	eErr = ePriority_Err,
	eWarning = ePriority_Warning,
	eNotice = ePriority_Notice,
	eInfo = ePriority_Info,
	eDebug = ePriority_Debug
	};

typedef int EPriority;

// =================================================================================================
// MARK: - String/integer mapping

EPriority sPriorityFromName(const std::string& iString);
std::string sNameFromPriority(EPriority iPriority);

// =================================================================================================
// MARK: - Log::StrimW

class ChanW : public ChanW_UTF_Native8
	{
public:
	ChanW(EPriority iPriority, const std::string& iName_String);
	ChanW(EPriority iPriority, const char* iName_CharStar, int iLine = -1);
	~ChanW();

// From ChanW_UTF_Native8
	virtual size_t QWriteUTF8(const UTF8* iSource, size_t iCountCU);

// Our protocol
	ZMACRO_operator_bool(ChanW, operator_bool) const;

	void Emit() const;

private:
	void pEmit();

	const int fPriority;
	mutable ZQ<std::string> fName_StringQ;
	const ZQ<const char*> fName_CharStarQ;
	const int fLine;
	mutable ZQ<std::string> fMessageQ;

public:
	mutable bool fOutdent;
	};

typedef ChanW S;

// =================================================================================================
// MARK: - Log::LogMeister

class LogMeister
:	public ZCounted
	{
public:
	virtual bool Enabled(EPriority iPriority, const std::string& iName);
	virtual bool Enabled(EPriority iPriority, const char* iName);
	virtual void LogIt(EPriority iPriority, const std::string& iName,
		size_t iDepth, const std::string& iMessage) = 0;
	};

extern ZRef<LogMeister> sLogMeister;
//extern Safe<ZRef<LogMeister> > sLogMeister;

void sLogIt(EPriority iPriority, const std::string& iName,
	size_t iDepth, const std::string& iMessage);


// =================================================================================================
// MARK: - Log::FunctionEntryExit

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
// MARK: - Log::sLogTrace

void sLogTrace(EPriority iPriority, const char* iFile, int iLine, const char* iFunctionName);

} // namespace Log

} // namespace ZooLib

#endif // __ZooLib_Log_h__
