/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/Compat_cmath.h" // For fmod
#include "zoolib/ChanW_UTF.h"
#include "zoolib/Chan_Bin_FILE.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/FunctionChain.h"
#include "zoolib/Stringf.h"
#include "zoolib/Time.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Debug.h"
#include "zoolib/Util_Time.h"

#include "zoolib/ZThread.h"

#if __MACH__
	#include <mach/mach_init.h> // For mach_thread_self
#endif

#if defined(__ANDROID__)
	#include <android/log.h>
#endif

namespace ZooLib {
namespace Util_Debug {

bool sCompact;

using std::min;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark ZDebug and ZAssert handler (anonymous)

namespace { // anonymous

static void spHandleDebug(const ZDebug::Params_t& iParams, va_list iArgs)
	{
	char theBuf[4096];
	sFormatStandardMessage(theBuf, sizeof(theBuf), iParams);

	Log::S s(Log::eErr, "Util_Debug");
	if (iParams.fStop)
		s << "STOP: ";
	else
		s << "DEBUG: ";

	s << theBuf;

	if (iParams.fUserMessage)
		sWritev(s, nullptr, nullptr, iParams.fUserMessage, iArgs);

	if (iParams.fStop)
		{
		s.Emit();

		// We don't have stack crawls, which means we're probably
		// on MacOS X. If we force a segfault then the Crash Reporter
		// will dump out stacks for us.
		*reinterpret_cast<double*>(1) = 0;
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
#pragma mark -
#pragma mark LogMeister_Base (anonymous)

namespace { // anonymous

class LogMeister_Base
:	public Log::LogMeister
	{
public:
	LogMeister_Base()
	:	fLogPriority(Log::ePriority_Notice)
		{}

// From Log::LogMeister
	virtual bool Enabled(Log::EPriority iPriority, const std::string& iName)
		{ return iPriority <= this->pGetLogPriority(); }

	virtual bool Enabled(Log::EPriority iPriority, const char* iName)
		{ return iPriority <= this->pGetLogPriority(); }

// Our protocol
	void SetLogPriority(Log::EPriority iLogPriority)
		{ fLogPriority = iLogPriority; }

	Log::EPriority GetLogPriority()
		{ return fLogPriority; }

	Log::EPriority pGetLogPriority()
		{
		if (const Log::EPriority* thePriorityP = LogPriorityPerThread::sPGet())
			return *thePriorityP;
		return fLogPriority;
		}

protected:
	ZMtx fMtx;

private:
	Log::EPriority fLogPriority;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark LogMeister_Android (anonymous)

namespace { // anonymous

#if defined(__ANDROID__)

class LogMeister_Android
:	public LogMeister_Base
	{
public:
	virtual void LogIt(Log::EPriority iPriority,
		const std::string& iName, size_t iDepth, const std::string& iMessage)
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
#pragma mark -
#pragma mark LogMeister_Default (anonymous)

namespace { // anonymous

class LogMeister_Default
:	public LogMeister_Base
	{
public:
	LogMeister_Default()
	:	fExtraSpace(20)
		{}

	virtual void LogIt(Log::EPriority iPriority,
		const std::string& iName, size_t iDepth, const std::string& iMessage)
		{
		if (iPriority > this->pGetLogPriority())
			return;

		ZRef<ChannerW_UTF> theChannerW = fChannerW;
		if (not theChannerW)
			return;

		const ChanW_UTF& theStrimW = sGetChan(theChannerW);

		ZAcqMtx acq(fMtx);

		const double now = Time::sNow();

		const size_t curLength = Unicode::sCUToCP(iName.begin(), iName.end());
		// Enabling this code will grow fExtraSpace when a long iName comes through.
		// if (fExtraSpace < curLength)
		//	fExtraSpace = curLength;

		// extraSpace will ensure that the message text from multiple calls lines
		// up, so long as iName is 20 CPs or less in length.
		const string extraSpace(fExtraSpace - min(fExtraSpace, curLength), ' ');

		if (sCompact)
			{
			theStrimW << Util_Time::sAsStringUTC(now, "%M:") << sStringf("%07.4f", fmod(now, 60));

			#if __MACH__
				theStrimW << sStringf(" %5x", ((int)mach_thread_self()));
			#else
				if (sizeof(ZThread::ID) > 4)
					theStrimW << sStringf(" %016llX", (unsigned long long)ZThread::sID());
				else
					theStrimW << sStringf(" %08llX", (unsigned long long)ZThread::sID());
			#endif
			}
		else
			{
			theStrimW << Util_Time::sAsString_ISO8601_us(now, false);

			#if __MACH__
				// GDB on Mac uses the mach thread ID for the systag.
				theStrimW << sStringf(" %5x/", ((int)mach_thread_self()));
			#else
				theStrimW << " 0x";
			#endif
			if (sizeof(ZThread::ID) > 4)
				theStrimW << sStringf("%016llX", (unsigned long long)ZThread::sID());
			else
				theStrimW << sStringf("%08llX", (unsigned long long)ZThread::sID());
			}

		theStrimW
			<< " P" << sStringf("%X", iPriority)
			<< " " << extraSpace << iName
			<< " - " ;
		while (iDepth--)
			theStrimW << "    ";
		theStrimW
			<< iMessage << "\n";

		sFlush(theStrimW);
		}

// Our protocol
	void SetChanner(ZRef<ChannerW_UTF> iChannerW)
		{
		ZAcqMtx acq(fMtx);
		fChannerW = iChannerW;
		}

private:
	ZRef<ChannerW_UTF> fChannerW;
	size_t fExtraSpace;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Util_Debug

void sInstall()
	{
	if (not ZCONFIG_SPI_Enabled(Win))
		{
		static DebugFunction theDF;
		}

	#if defined(__ANDROID__)
		Log::sLogMeister = new LogMeister_Android;
	#else
		ZRef<LogMeister_Default> theLM = new LogMeister_Default;

		FILE* theStdOut = stdout; // Workaround for VC++
		ZRef<Channer<ChanW_UTF> > theChannerW_UTF =
			new Channer_Channer_T<ChanW_UTF_Chan_Bin_UTF8,ChanW_Bin>(
				new Channer_T<ChanW_Bin_FILE>(theStdOut));

		theLM->SetChanner(theChannerW_UTF);


		Log::sLogMeister = theLM;
	#endif
	}

void sSetChanner(ZRef<ChannerW_UTF> iChannerW)
	{
	if (ZRef<Log::LogMeister> theLM1 = Log::sLogMeister)
		{
		if (ZRef<LogMeister_Default> theLM = theLM1.DynamicCast<LogMeister_Default>())
			theLM->SetChanner(iChannerW);
		}
	}

void sSetLogPriority(Log::EPriority iLogPriority)
	{
	if (ZRef<Log::LogMeister> theLM1 = Log::sLogMeister)
		{
		if (ZRef<LogMeister_Base> theLM = theLM1.DynamicCast<LogMeister_Base>())
			theLM->SetLogPriority(iLogPriority);
		}
	}

Log::EPriority sGetLogPriority()
	{
	if (ZRef<Log::LogMeister> theLM1 = Log::sLogMeister)
		{
		if (ZRef<LogMeister_Base> theLM = theLM1.DynamicCast<LogMeister_Base>())
			return theLM->GetLogPriority();
		}
	return 0xFF;
	}

} // namespace Util_Debug
} // namespace ZooLib
