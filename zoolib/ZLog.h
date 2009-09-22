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

#ifndef __ZLog__
#define __ZLog__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

#include "zoolib/ZCompat_operator_bool.h"

// s == strim name
// p == priority
// f == facility


#if ZCONFIG(Compiler, GCC)
#	define ZMACRO_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif ZCONFIG(Compiler, MSVC)
#	define ZMACRO_PRETTY_FUNCTION __FUNCDNAME__
#else
#	define ZMACRO_PRETTY_FUNCTION __FUNCTION__
#endif

#define ZLOGPF(s, p) const ZooLib::ZLog::S& s = ZLog::S(ZLog::p, ZMACRO_PRETTY_FUNCTION)
#define ZLOGF(s, p) const ZooLib::ZLog::S& s = ZLog::S(ZLog::p, __FUNCTION__)
#define ZLOG(s, p, f) const ZooLib::ZLog::S& s = ZLog::S(ZLog::p, f)

#define ZLOGFUNCTION(p) ZooLib::ZLog::FunctionEntryExit \
	theLogFEE##__LINE__(ZooLib::ZLog::p, ZMACRO_PRETTY_FUNCTION)

#define ZLOGTRACE(p) ZooLib::ZLog::sLogTrace(ZooLib::ZLog::p, __FILE__, __LINE__)

NAMESPACE_ZOOLIB_BEGIN

namespace ZLog {

// =================================================================================================
#pragma mark -
#pragma mark * EPriority

enum
	{
	ePriority_Emerg = 0, ///< System on fire?
	ePriority_Alert = 1, ///< action must be taken immediately
	ePriority_Crit = 2,  ///< critical conditions
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
#pragma mark -
#pragma mark * String/integer mapping

EPriority sPriorityFromName(const std::string& iString);
std::string sNameFromPriority(EPriority iPriority);

// =================================================================================================
#pragma mark -
#pragma mark * ZLog::StrimW

class StrimW : public ZStrimW
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(StrimW, operator_bool_generator_type, operator_bool_type);
public:
	StrimW(EPriority iPriority, const std::string& iName);
	StrimW(EPriority iPriority, const char* iName);
	~StrimW();

// From ZStrimW
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU);

// Our protocol
	operator operator_bool_type() const;
	void Emit() const;

private:
	void pEmit();

	const int fPriority;
	const std::string fName;
	mutable std::string fMessage;
	};

typedef StrimW S;

// =================================================================================================
#pragma mark -
#pragma mark * ZLog::LogMeister

class LogMeister
	{
protected:
	LogMeister();
	LogMeister(const LogMeister&);
	LogMeister& operator=(const LogMeister&);
	
public:
	virtual ~LogMeister();
	virtual bool Enabled(EPriority iPriority, const std::string& iName);
	virtual void LogIt(
		EPriority iPriority, const std::string& iName, const std::string& iMessage) = 0;
	};

void sSetLogMeister(LogMeister* iLogMeister);

// =================================================================================================
#pragma mark -
#pragma mark * ZLog::FunctionEntryExit

class FunctionEntryExit
	{
public:
	FunctionEntryExit(EPriority iPriority, const char* iFunctionName);
	~FunctionEntryExit();

private:
	EPriority fPriority;
	const char* fFunctionName;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZLog::sLogLine

void sLogTrace(EPriority iPriority, const char* iFile, int iLine);

} // namespace ZLog

NAMESPACE_ZOOLIB_END

#endif // __ZLog__
