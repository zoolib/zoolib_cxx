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

#include "zoolib/ZFile.h"
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZSafe.h"
#include "zoolib/ZStream_POSIX.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Time.h"

#include "zoolib/ZCompat_cmath.h" // For fmod

#if __MACH__
	#include <mach/mach_init.h> // For mach_thread_self
#endif

namespace ZooLib {
namespace ZUtil_Debug {

bool sCompact;

using std::min;
using std::string;

// =================================================================================================
// MARK: - ZDebug and ZAssert handler (anonymous)

namespace { // anonymous

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
		s.Emit();

		// We don't have stack crawls, which means we're probably
		// on MacOS X. If we force a segfault then the Crash Reporter
		// will dump out stacks for us.
		// From ADC Home > Reference Library > Guides > Tools >
		// Xcode > Xcode 2.3 User Guide > Controlling Execution of Your Code >
		//asm {trap};
		*reinterpret_cast<double*>(1) = 0;
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

} // anonymous namespace

// =================================================================================================
// MARK: - LogMeister (anonymous)

namespace { // anonymous

class LogMeister
:	public ZLog::LogMeister
	{
public:
	LogMeister()
	:	fLogPriority(ZLog::ePriority_Notice)
	,	fExtraSpace(20)
		{}

// From ZLog::LogMeister
	virtual bool Enabled(ZLog::EPriority iPriority, const std::string& iName)
		{ return iPriority <= this->pGetLogPriority(); }

	virtual bool Enabled(ZLog::EPriority iPriority, const char* iName)
		{ return iPriority <= this->pGetLogPriority(); }

	virtual void LogIt
		(ZLog::EPriority iPriority, const std::string& iName, const std::string& iMessage)
		{
		if (iPriority > this->pGetLogPriority())
			return;

		ZRef<ZStrimmerW> theStrimmerW = fStrimmerW;
		if (not theStrimmerW)
			return;

		const ZStrimW& theStrimW = theStrimmerW->GetStrimW();

		ZAcqMtx acq(fMtx);

		const ZTime now = ZTime::sNow();

		const size_t curLength = ZUnicode::sCUToCP(iName.begin(), iName.end());
		// Enabling this code will grow fExtraSpace when a long iName comes through.
		// if (fExtraSpace < curLength)
		//	fExtraSpace = curLength;

		// extraSpace will ensure that the message text from multiple calls lines
		// up, so long as iName is 20 CPs or less in length.
		const string extraSpace(fExtraSpace - min(fExtraSpace, curLength), ' ');

		if (sCompact)
			{
			theStrimW << ZUtil_Time::sAsStringUTC(now, "%M:") << sStringf("%07.4f", fmod(now.fVal, 60));

			#if __MACH__
				theStrimW << sStringf(" %5x", ((int)mach_thread_self()));
			#else
				if (sizeof(ZThread::ID) > 4)
					theStrimW << sStringf(" %016llX", (uint64)ZThread::sID());
				else
					theStrimW << sStringf(" %08llX", (uint64)ZThread::sID());
			#endif
			}
		else
			{
			theStrimW << ZUtil_Time::sAsString_ISO8601_us(now, false);

			#if __MACH__
				// GDB on Mac uses the mach thread ID for the systag.
				theStrimW << sStringf(" %5x/", ((int)mach_thread_self()));
			#else
				theStrimW << " 0x";
			#endif
			if (sizeof(ZThread::ID) > 4)
				theStrimW << sStringf("%016llX", (uint64)ZThread::sID());
			else
				theStrimW << sStringf("%08llX", (uint64)ZThread::sID());
			}

		theStrimW
			<< " P" << sStringf("%X", iPriority)
			<< " " << extraSpace << iName
			<< " - " << iMessage << "\n";

		theStrimW.Flush();
		}

// Our protocol
	void SetStrimmer(ZRef<ZStrimmerW> iStrimmerW)
		{
		ZAcqMtx acq(fMtx);
		fStrimmerW = iStrimmerW;
		}

	void SetLogPriority(ZLog::EPriority iLogPriority)
		{ fLogPriority = iLogPriority; }

	ZLog::EPriority GetLogPriority()
		{ return fLogPriority; }

	ZLog::EPriority pGetLogPriority()
		{
		if (const ZLog::EPriority* thePriorityP = LogPriorityPerThread::sPGet())
			return *thePriorityP;
		return fLogPriority;
		}

private:
	ZMtx fMtx;
	ZRef<ZStrimmerW> fStrimmerW;
	ZLog::EPriority fLogPriority;
	size_t fExtraSpace;
	};

} // anonymous namespace

// =================================================================================================
// MARK: - ZUtil_Debug

void sInstall()
	{
	if (not ZCONFIG_SPI_Enabled(Win))
		{
		static DebugFunction theDF;
		}

	ZRef<LogMeister> theLM = new LogMeister;

	FILE* theStdOut = stdout; // Workaround for VC++
	theLM->SetStrimmer
		(sStrimmerW_Streamer_T<ZStrimW_StreamUTF8>(sStreamerW_T<ZStreamW_FILE>(theStdOut)));

	ZLog::sLogMeister = theLM;
	}

void sSetStrimmer(ZRef<ZStrimmerW> iStrimmerW)
	{
	if (ZRef<ZLog::LogMeister> theLM1 = ZLog::sLogMeister)
		{
		if (ZRef<LogMeister> theLM = theLM1.DynamicCast<LogMeister>())
			theLM->SetStrimmer(iStrimmerW);
		}
	}

void sSetLogPriority(ZLog::EPriority iLogPriority)
	{
	if (ZRef<ZLog::LogMeister> theLM1 = ZLog::sLogMeister)
		{
		if (ZRef<LogMeister> theLM = theLM1.DynamicCast<LogMeister>())
			theLM->SetLogPriority(iLogPriority);
		}
	}

ZLog::EPriority sGetLogPriority()
	{
	if (ZRef<ZLog::LogMeister> theLM1 = ZLog::sLogMeister)
		{
		if (ZRef<LogMeister> theLM = theLM1.DynamicCast<LogMeister>())
			return theLM->GetLogPriority();
		}
	return 0xFF;
	}

} // namespace ZUtil_Debug
} // namespace ZooLib
