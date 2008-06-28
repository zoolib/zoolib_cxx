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

#include "zoolib/ZString.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZUtil_Time.h"

#include <cmath> // For fmod

#if ZCONFIG_SPI_Enabled(POSIX)
#	include <csignal>
#endif

using std::min;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Debug::sDumpStackCrawl

#if ZCONFIG_API_Enabled(StackCrawl)

void ZUtil_Debug::sDumpStackCrawl(const ZStackCrawl& iCrawl, const ZStrimW& s)
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
#pragma mark -
#pragma mark * Sync signal handler

#if ZCONFIG_SPI_Enabled(POSIX) && ZCONFIG_API_Enabled(StackCrawl)

static bool sHandlingFatal;
static void sHandleSignal_Sync(int inSignal)
	{
	switch (inSignal)
		{
		case SIGSEGV:
		case SIGBUS:
			{
			if (!sHandlingFatal)
				{
				sHandlingFatal = true;
				const ZLog::S& s = ZLog::S(ZLog::ePriority_Crit, "ZUtil_Debug");
				if (inSignal == SIGSEGV)
					s << "SIGSEGV";
				else
					s << "SIGBUS";

				s << " in frame #4 below:\n";
				ZStackCrawl theCrawl;
				ZUtil_Debug::sDumpStackCrawl(theCrawl, s);
				}
			abort();
			}
		}
	}

#endif // ZCONFIG_SPI_Enabled(POSIX) && ZCONFIG_API_Enabled(StackCrawl)

// =================================================================================================
#pragma mark -
#pragma mark * ZDebug and ZAssert handler

namespace ZUtil_Debug {

static void sDebug_HandleActual(int inLevel, ZDebug_Action inAction, const char* inMessage)
	{
	if (inAction == eDebug_ActionStop)
		{
		const ZLog::S& s = ZLog::S(ZLog::eErr, "ZUtil_Debug");
		s << "STOP: " << inMessage << "\n";

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
	else
		{
		const ZLog::S& s = ZLog::S(ZLog::eErr, "ZUtil_Debug");
		s << "DEBUG: " << inMessage;
		}
	}

} // namespace ZUtil_Debug

// =================================================================================================
#pragma mark -
#pragma mark * Deadlock handler

static void sDeadlockHandler(const string& iString)
	{
	ZDebugStopf(0, ("DEADLOCK DETECTED: %s", iString.c_str()));
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Debug::LogMeister

ZUtil_Debug::LogMeister* ZUtil_Debug::LogMeister::sLogMeister;

ZUtil_Debug::LogMeister::LogMeister()
	{
	fLogPriority = ZLog::ePriority_Notice;
	sLogMeister = this;
	}

bool ZUtil_Debug::LogMeister::Enabled(ZLog::EPriority iPriority, const string& iName)
	{ return iPriority <= fLogPriority; }

void ZUtil_Debug::LogMeister::LogIt(ZLog::EPriority iPriority, const string& iName, const string& iMessage)
	{
	if (iPriority > fLogPriority)
		return;

	ZRef<ZStrimmerW> theStrimmerW = fStrimmerW;
	if (!theStrimmerW)
		return;
	
	const ZStrimW& theStrimW = theStrimmerW->GetStrimW();

	ZTime now = ZTime::sNow();

	// extraSpace will ensure that the message text from multiple calls lines
	// up, so long as iName is 20 CPs or less in length.
	string extraSpace(20 - min(size_t(20), ZUnicode::sCUToCP(iName.begin(), iName.end())), ' ');
	theStrimW
			<< ZUtil_Time::sAsString_ISO8601_us(now, false)
			<< " " << ZString::sFormat("0x%08X", ZThread::sCurrentID())
			<< " P" << ZString::sFormat("%X", iPriority)
			<< " " << extraSpace << iName
			<< " - " << iMessage << "\n";

	theStrimW.Flush();
	}

void ZUtil_Debug::LogMeister::SetStrimmer(ZRef<ZStrimmerW> iStrimmerW)
	{
	fStrimmerW = iStrimmerW;
	}

void ZUtil_Debug::LogMeister::SetLogPriority(ZLog::EPriority iLogPriority)
	{
	fLogPriority = iLogPriority;
	}

ZLog::EPriority ZUtil_Debug::LogMeister::GetLogPriority()
	{
	return fLogPriority;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Debug

void ZUtil_Debug::sInstall()
	{
	// sDebug_HandleActual is a global declared in ZDebug.h and defined in ZDebug.cpp.
	::sDebug_HandleActual = ZUtil_Debug::sDebug_HandleActual;

	ZThread::sSetDeadlockHandler(sDeadlockHandler);

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

void ZUtil_Debug::sSetStrimmer(ZRef<ZStrimmerW> iStrimmerW)
	{
	if (LogMeister::sLogMeister)
		LogMeister::sLogMeister->SetStrimmer(iStrimmerW);
	}

void ZUtil_Debug::sSetLogPriority(ZLog::EPriority iLogPriority)
	{
	if (LogMeister::sLogMeister)
		LogMeister::sLogMeister->SetLogPriority(iLogPriority);
	}

ZLog::EPriority ZUtil_Debug::sGetLogPriority()
	{
	if (LogMeister::sLogMeister)
		return LogMeister::sLogMeister->GetLogPriority();
	return 0xFF;
	}
