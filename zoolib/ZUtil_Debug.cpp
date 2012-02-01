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

#include "zoolib/ZUtil_Debug.h"

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZString.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Time.h"

#include <stdlib.h> // For abort

#if __MACH__
	#include <mach/mach_init.h> // For mach_thread_self
#endif

#if ZCONFIG_SPI_Enabled(POSIX)
	#include <csignal>
#endif

namespace ZooLib {
namespace ZUtil_Debug {

using std::min;
using std::string;

// =================================================================================================
// MARK: - ZUtil_Debug::sDumpStackCrawl

#if ZCONFIG_API_Enabled(StackCrawl)

void sDumpStackCrawl(const ZStackCrawl& iCrawl, const ZStrimW& s)
	{
	for (size_t x = 0; x < iCrawl.Count(); ++x)
		{
		const ZStackCrawl::Frame theFrame = iCrawl.At(x);
		s.Writef("#%3d, 0x%08X: ", x, reinterpret_cast<int>(theFrame.fPC));

		s << theFrame.fName;

		if (theFrame.fOffset)
			{
			if (theFrame.fOffset > 9)
				s.Writef("+0x%X {%u}", theFrame.fOffset, theFrame.fOffset);
			else
				s.Writef("+%u", theFrame.fOffset);
			}

		if (theFrame.fCountParams)
			{
			s << " (";
			for (size_t y = 0; y < theFrame.fCountParams; ++y)
				{
				if (y)
					s << ", ";
				if (theFrame.fParams[y] > 9)
					s.Writef("0x%X {%u}", theFrame.fParams[y], theFrame.fParams[y]);
				else
					s.Writef("%u", theFrame.fParams[y]);
				}
			s << ")";
			}
		s << "\n";
		}
	}

#endif // ZCONFIG_API_Enabled(StackCrawl)

// =================================================================================================
// MARK: - Sync signal handler

#if ZCONFIG_SPI_Enabled(POSIX) && ZCONFIG_API_Enabled(StackCrawl)

static bool spHandlingFatal;
static void spHandleSignal_Sync(int inSignal)
	{
	switch (inSignal)
		{
		case SIGSEGV:
		case SIGBUS:
			{
			if (!spHandlingFatal)
				{
				spHandlingFatal = true;
				ZLog::S s(ZLog::ePriority_Crit, "ZUtil_Debug");
				if (inSignal == SIGSEGV)
					s << "SIGSEGV";
				else
					s << "SIGBUS";

				s << " in frame #4 below:\n";
				ZStackCrawl theCrawl;
				sDumpStackCrawl(theCrawl, s);
				}
			abort();
			}
		}
	}

#endif // ZCONFIG_SPI_Enabled(POSIX) && ZCONFIG_API_Enabled(StackCrawl)

// =================================================================================================
// MARK: - ZDebug and ZAssert handler

static void spHandleDebug(const ZDebug::Params_t& iParams, va_list iArgs)
	{
	char theBuf[4096];
	sFormatStandardMessage(theBuf, sizeof(theBuf), iParams);

	ZLog::S s(ZLog::eErr, "ZUtil_Debug");
	if (iParams.fStop)
		s << "STOP: ";
	else
		s << "DEBUG: ";

	s << theBuf;

	if (iParams.fUserMessage)
		s.Writev(iParams.fUserMessage, iArgs);

	if (iParams.fStop)
		{
		#if ZCONFIG_API_Enabled(StackCrawl)
			ZStackCrawl theCrawl;
			sDumpStackCrawl(theCrawl, s);
		#endif
		s.Emit();

		if (ZCONFIG_API_Enabled(StackCrawl))
			{
			// We have stack crawls, so just abort the process.
			abort();
			}
		else
			{
			// We don't have stack crawls, which means we're probably
			// on MacOS X. If we force a segfault then the Crash Reporter
			// will dump out stacks for us.
			// From ADC Home > Reference Library > Guides > Tools >
			// Xcode > Xcode 2.3 User Guide > Controlling Execution of Your Code >
			//asm {trap};
			*reinterpret_cast<double*>(1) = 0;
			}
		}
	}

class DebugFunction
:	public ZFunctionChain_T<ZDebug::Function_t, const ZDebug::Params_t&>
	{
public:
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = spHandleDebug;
		return true;
		}
	};

// =================================================================================================
// MARK: - ZUtil_Debug::LogMeister

LogMeister* LogMeister::sLogMeister;

LogMeister::LogMeister()
:	fLogPriority(ZLog::ePriority_Notice),
	fExtraSpace(20)
	{
	sLogMeister = this;
	}

bool LogMeister::Enabled(ZLog::EPriority iPriority, const string& iName)
	{ return iPriority <= fLogPriority; }

bool LogMeister::Enabled(ZLog::EPriority iPriority, const char* iName)
	{ return iPriority <= fLogPriority; }

void LogMeister::LogIt
	(ZLog::EPriority iPriority, const string& iName, const string& iMessage)
	{
	if (iPriority > fLogPriority)
		return;

	ZRef<ZStrimmerW> theStrimmerW = fStrimmerW;
	if (!theStrimmerW)
		return;

	const ZStrimW& theStrimW = theStrimmerW->GetStrimW();

	ZTime now = ZTime::sNow();

	const size_t curLength = ZUnicode::sCUToCP(iName.begin(), iName.end());
//	if (fExtraSpace < curLength)
//		fExtraSpace = curLength;

	// extraSpace will ensure that the message text from multiple calls lines
	// up, so long as iName is 20 CPs or less in length.
	string extraSpace(fExtraSpace - min(fExtraSpace, curLength), ' ');

	theStrimW << ZUtil_Time::sAsString_ISO8601_us(now, false);
	#if __MACH__
		// GDB on Mac uses the mach thread ID for the systag.
		theStrimW << ZStringf(" %5x/", ((int)mach_thread_self()));
	#else
		theStrimW << " 0x";
	#endif

	if (sizeof(ZThread::ID) > 4)
		theStrimW << ZStringf("%016llX", (uint64)ZThread::sID());
	else
		theStrimW << ZStringf("%08llX", (uint64)ZThread::sID());

	theStrimW
		<< " P" << ZStringf("%X", iPriority)
		<< " " << extraSpace << iName
		<< " - " << iMessage << "\n";

	theStrimW.Flush();
	}

void LogMeister::SetStrimmer(ZRef<ZStrimmerW> iStrimmerW)
	{
	fStrimmerW = iStrimmerW;
	}

void LogMeister::SetLogPriority(ZLog::EPriority iLogPriority)
	{
	fLogPriority = iLogPriority;
	}

ZLog::EPriority LogMeister::GetLogPriority()
	{
	return fLogPriority;
	}

// =================================================================================================
// MARK: - ZUtil_Debug

void sInstall()
	{
	if (not ZCONFIG_SPI_Enabled(Win))
		{
		static DebugFunction theDF;
		}

	ZLog::sSetLogMeister(new LogMeister);

	#if ZCONFIG_SPI_Enabled(POSIX) && ZCONFIG_API_Enabled(StackCrawl)
		// Install the sync handler only if we're on POSIX and
		// we have stack crawl capabilities.
		struct sigaction theSigAction;
		sigemptyset(&theSigAction.sa_mask);
		theSigAction.sa_flags = 0;
		theSigAction.sa_handler = sHandleSignal_Sync;

		sigaddset(&theSigAction.sa_mask, SIGSEGV);
		sigaddset(&theSigAction.sa_mask, SIGBUS);

		struct sigaction oldSigAction;
		int result;
		result = ::sigaction(SIGSEGV, &theSigAction, &oldSigAction);
		result = ::sigaction(SIGBUS, &theSigAction, &oldSigAction);
	#endif
	}

void sSetStrimmer(ZRef<ZStrimmerW> iStrimmerW)
	{
	if (LogMeister::sLogMeister)
		LogMeister::sLogMeister->SetStrimmer(iStrimmerW);
	}

void sSetLogPriority(ZLog::EPriority iLogPriority)
	{
	if (LogMeister::sLogMeister)
		LogMeister::sLogMeister->SetLogPriority(iLogPriority);
	}

ZLog::EPriority sGetLogPriority()
	{
	if (LogMeister::sLogMeister)
		return LogMeister::sLogMeister->GetLogPriority();
	return 0xFF;
	}

} // namespace ZUtil_Debug
} // namespace ZooLib
