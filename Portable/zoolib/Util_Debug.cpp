// Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/Util_Debug.h"

#include "zoolib/Compat_cmath.h" // For fmod
#include "zoolib/ChanW_UTF.h"
#include "zoolib/Chan_Bin_FILE.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/FunctionChain.h"
#include "zoolib/Stringf.h"
#include "zoolib/Time.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Time.h"

#include "zoolib/ZThread.h"

#if __MACH__ || (__linux__ && !defined(__ANDROID__))
	#include <execinfo.h> // For backtrace
#endif

#if __MACH__
	#include <mach/mach_init.h> // For mach_thread_self
#endif

#if defined(__ANDROID__)
	#include <android/log.h>
#endif

#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
	#include <cxxabi.h>
	#include <stdlib.h> // For free, required by __cxa_demangle
#endif

namespace ZooLib {
namespace Util_Debug {

bool sCompact;

using std::string;

// =================================================================================================
#pragma mark - ZDebug and ZAssert handler (anonymous)

namespace { // anonymous

static void spHandleDebug(const ZDebug::Params_t& iParams, va_list iArgs)
	{
	char theBuf[4096];
	sFormatStandardMessage(theBuf, sizeof(theBuf), iParams);

	Log::ChanW cc(Log::eErr, "Util_Debug");
	if (iParams.fStop)
		cc << "STOP: ";
	else
		cc << "DEBUG: ";

	cc << theBuf;

	if (iParams.fUserMessage)
		sWritev(cc, nullptr, nullptr, iParams.fUserMessage, iArgs);

	if (iParams.fStop)
		{
		cc << "\n";
		sWriteBacktrace(cc);
		cc.Emit();
		abort();
		}
	}

class DebugFunction
:	public FunctionChain<ZDebug::Function_t, const ZDebug::Params_t&>
	{
public:
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = spHandleDebug;
		return true;
		}
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - LogMeister_Base

LogMeister_Base::LogMeister_Base(Log::EPriority iPriority)
:	fLogPriority(iPriority)
	{}

bool LogMeister_Base::Enabled(Log::EPriority iPriority, const string& iName)
	{ return iPriority <= this->pGetLogPriority(); }

bool LogMeister_Base::Enabled(Log::EPriority iPriority, const char* iName)
	{ return iPriority <= this->pGetLogPriority(); }

void LogMeister_Base::SetLogPriority(Log::EPriority iLogPriority)
	{ fLogPriority = iLogPriority; }

Log::EPriority LogMeister_Base::GetLogPriority()
	{ return fLogPriority; }

Log::EPriority LogMeister_Base::pGetLogPriority()
	{
	if (const Log::EPriority* thePriorityP = LogPriorityPerThread::sPGet())
		return *thePriorityP;
	return fLogPriority;
	}

// =================================================================================================
#pragma mark - LogMeister_Android (anonymous)

namespace { // anonymous

#if defined(__ANDROID__)

class LogMeister_Android
:	public LogMeister_Base
	{
public:
	LogMeister_Android()
	:	LogMeister_Base(Log::ePriority_Notice)
		{}

	virtual void LogIt(Log::EPriority iPriority,
		const string& iName, size_t iDepth, const string& iMessage)
		{
		if (iPriority > this->pGetLogPriority())
			return;

		int theLevel = ANDROID_LOG_VERBOSE;

		if (false) {}
		else if (iPriority < Log::ePriority_Err) theLevel = ANDROID_LOG_FATAL;
		else if (iPriority <= Log::ePriority_Err) theLevel = ANDROID_LOG_ERROR;
		else if (iPriority <= Log::ePriority_Warning) theLevel = ANDROID_LOG_WARN;
		else if (iPriority <= Log::ePriority_Info) theLevel = ANDROID_LOG_INFO;
		else if (iPriority <= Log::ePriority_Debug) theLevel = ANDROID_LOG_DEBUG;

		__android_log_print(theLevel, iName.c_str(), "%s", iMessage.c_str());
		}

	};

#endif // defined(__ANDROID__)

} // anonymous namespace

// =================================================================================================
#pragma mark - LogMeister_Default (anonymous)

namespace { // anonymous

class LogMeister_Default
:	public LogMeister_Base
	{
public:
	LogMeister_Default()
	:	LogMeister_Base(Log::ePriority_Notice)
	,	fExtraSpace(20)
		{}

	virtual void LogIt(Log::EPriority iPriority,
		const string& iName, size_t iDepth, const string& iMessage)
		{
		if (iPriority > this->pGetLogPriority())
			return;

		// We use the mutex so we don't intermingle characters from separate threads
		ZAcqMtx acq(fMtx);

		ZP<ChannerW_UTF> theChannerW = fChannerW;
		if (not theChannerW)
			return;

		sWriteStandardLoggingText(*theChannerW, fExtraSpace, sCompact, iPriority, iName, iDepth, iMessage);

		*theChannerW << "\n";

		sFlush(*theChannerW);
		}

// Our protocol
	void SetChanner(ZP<ChannerW_UTF> iChannerW)
		{
		ZAcqMtx acq(fMtx);
		fChannerW = iChannerW;
		}

private:
	ZMtx fMtx;
	ZP<ChannerW_UTF> fChannerW;
	size_t fExtraSpace;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - Util_Debug

void sWriteBacktrace(const ChanW_UTF& iChanW)
	{
	#if __MACH__ || (__linux__ &&  !defined(__ANDROID__))
		void* callstack[128];
		int count = backtrace(callstack, 128);
		char** strs = backtrace_symbols(callstack, count);
		for (int ii = 0; ii < count; ++ii)
			iChanW << strs[ii] << "\n";
	#endif
	}


void sInstall()
	{
	if (not ZCONFIG_SPI_Enabled(Win))
		{
		static DebugFunction theDF;
		}

	#if defined(__ANDROID__)
		Log::sLogMeister = new LogMeister_Android;
	#else
		ZP<LogMeister_Default> theLM = new LogMeister_Default;

		FILE* theStdOut = stdout; // Workaround for VC++
		ZP<Channer<ChanW_Bin>> asChannerW_Bin = sChanner_T<ChanW_Bin_FILE>(theStdOut);
		ZP<Channer<ChanW_UTF>> theChannerW_UTF = sChanner_Channer_T<ChanW_UTF_Chan_Bin_UTF8>(asChannerW_Bin);

		theLM->SetChanner(theChannerW_UTF);

		Log::sLogMeister = theLM;
	#endif
	}

void sSetChanner(ZP<ChannerW_UTF> iChannerW)
	{
	if (ZP<Log::LogMeister> theLM1 = Log::sLogMeister)
		{
		if (ZP<LogMeister_Default> theLM = theLM1.DynamicCast<LogMeister_Default>())
			theLM->SetChanner(iChannerW);
		}
	}

void sSetLogPriority(Log::EPriority iLogPriority)
	{
	if (ZP<Log::LogMeister> theLM1 = Log::sLogMeister)
		{
		if (ZP<LogMeister_Base> theLM = theLM1.DynamicCast<LogMeister_Base>())
			theLM->SetLogPriority(iLogPriority);
		}
	}

Log::EPriority sGetLogPriority()
	{
	if (ZP<Log::LogMeister> theLM1 = Log::sLogMeister)
		{
		if (ZP<LogMeister_Base> theLM = theLM1.DynamicCast<LogMeister_Base>())
			return theLM->GetLogPriority();
		}
	return 0xFF;
	}

// =================================================================================================
#pragma mark -

void sWriteStandardLoggingText(const ChanW_UTF& iChanW,
	size_t iExtraSpace, bool iCompact,
	Log::EPriority iPriority,
	const string& iName, size_t iDepth, const string& iMessage)
	{
	const double now = Time::sNow();

	const size_t curLength = Unicode::sCUToCP(iName.begin(), iName.end());

	// extraSpace will ensure that the message text from multiple calls lines
	// up, so long as iName is 20 CPs or less in length.
	const string extraSpace(iExtraSpace - std::min(iExtraSpace, curLength), ' ');

	if (iCompact)
		{
		iChanW << Util_Time::sAsStringUTC(now, "%M:") << sStringf("%07.4f", fmod(now, 60));

		#if __MACH__
			iChanW << sStringf(" %5x", ((int)::pthread_mach_thread_np(::pthread_self())));
		#else
			if (sizeof(ZThread::ID) > 4)
				iChanW << sStringf(" %016llX", (unsigned long long)ZThread::sID());
			else
				iChanW << sStringf(" %08llX", (unsigned long long)ZThread::sID());
		#endif
		}
	else
		{
		iChanW << Util_Time::sAsString_ISO8601_us(now, false);

		#if __MACH__
			// GDB on Mac uses the mach thread ID for the systag.
			iChanW << sStringf(" %5x/", ((int)mach_thread_self()));
		#else
			iChanW << " 0x";
		#endif
		if (sizeof(ZThread::ID) > 4)
			iChanW << sStringf("%016llX", (unsigned long long)ZThread::sID());
		else
			iChanW << sStringf("%08llX", (unsigned long long)ZThread::sID());
		}

	iChanW
		<< " P" << sStringf("%X", iPriority)
		<< " " << extraSpace << iName
		<< " - " ;

	while (iDepth--)
		iChanW << "    ";

	iChanW
		<< iMessage;
	}

// =================================================================================================
#pragma mark -

string sPrettyName(const std::type_info& iTI)
	{
	#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
	if (char* unmangled = abi::__cxa_demangle(iTI.name(), 0, 0, 0))
		{
		string result = unmangled;
		free(unmangled);
		return result;
		}
	#endif

	return iTI.name();
	}

} // namespace Util_Debug
} // namespace ZooLib
