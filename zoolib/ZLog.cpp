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

#include "zoolib/ZCompat_string.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZString.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZUtil_string.h"

using std::string;

namespace ZooLib {
namespace ZLog {

static ZMtx spMutex;
static LogMeister* spLogMeister;

// =================================================================================================
// MARK: - String/integer mapping

static const char* const spNames[] =
	{
	"Emerg",
	"Alert",
	"Crit",
	"Err",
	"Warning",
	"Notice",
	"Info",
	"Debug"
	};

EPriority sPriorityFromName(const string& iString)
	{
	for (int priority = 0; priority <= eDebug; ++priority)
		{
		if (0 == ZUtil_string::sEquali(iString, spNames[priority]))
			return priority;
		}
	return -1;
	}

string sNameFromPriority(EPriority iPriority)
	{
	if (iPriority < 0)
		return ZStringf("%d", iPriority);

	if (iPriority <= eDebug)
		return spNames[iPriority];

	return spNames[eDebug] + ZStringf("+%d", iPriority - eDebug);
	}

// =================================================================================================
// MARK: - ZLog::StrimW

StrimW::StrimW(EPriority iPriority, const string& iName_String)
:	fPriority(iPriority),
	fName_StringQ(iName_String)
	{}

StrimW::StrimW(EPriority iPriority, const char* iName_CharStar)
:	fPriority(iPriority),
	fName_CharStarQ(iName_CharStar)
	{}

StrimW::~StrimW()
	{
	if (fMessageQ && !fMessageQ->empty())
		this->pEmit();
	}

void StrimW::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{
	try
		{
		ZStrimW_String8(sMut(fMessageQ)).Write(iSource, iCountCU, oCountCU);
		}
	catch (...)
		{
		if (oCountCU)
			*oCountCU = 0;
		}
	}

void StrimW::Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU)
	{
	try
		{
		ZStrimW_String8(sMut(fMessageQ)).Write(iSource, iCountCU, oCountCU);
		}
	catch (...)
		{
		if (oCountCU)
			*oCountCU = 0;
		}
	}

void StrimW::Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU)
	{
	try
		{
		ZStrimW_String8(sMut(fMessageQ)).Write(iSource, iCountCU, oCountCU);
		}
	catch (...)
		{
		if (oCountCU)
			*oCountCU = 0;
		}
	}

StrimW::operator operator_bool() const
	{
	if (spLogMeister)
		{
		if (fName_StringQ)
			{
			return operator_bool_gen::translate
				(spLogMeister->Enabled(fPriority, *fName_StringQ));
			}
		else
			{
			return operator_bool_gen::translate
				(spLogMeister->Enabled(fPriority, *fName_CharStarQ));
			}
		}
	else
		{
		return operator_bool_gen::translate(false);
		}
	}

void StrimW::Emit() const
	{
	if (fMessageQ && !fMessageQ->empty())
		{
		const_cast<StrimW*>(this)->pEmit();
		sMut(fMessageQ).resize(0);
		}
	}

void StrimW::pEmit()
	{
	spMutex.Acquire();
	if (spLogMeister)
		{
		try
			{
			if (not fName_StringQ)
				fName_StringQ = *fName_CharStarQ;
			spLogMeister->LogIt(fPriority, *fName_StringQ, *fMessageQ);
			}
		catch (...)
			{}
		}
	spMutex.Release();
	}

// =================================================================================================
// MARK: - ZLog::LogMeister

LogMeister::LogMeister()
	{}

LogMeister::LogMeister(const LogMeister&)
	{}

LogMeister& LogMeister::operator=(const LogMeister&)
	{ return *this; }

LogMeister::~LogMeister()
	{}

bool LogMeister::Enabled(EPriority iPriority, const string& iName)
	{ return true; }

bool LogMeister::Enabled(EPriority iPriority, const char* iName)
	{ return true; }

void sSetLogMeister(LogMeister* iLogMeister)
	{
	spMutex.Acquire();
	delete spLogMeister;
	spLogMeister = iLogMeister;
	spMutex.Release();
	}

// =================================================================================================
// MARK: - ZLog::FunctionEntryExit

FunctionEntryExit::FunctionEntryExit(EPriority iPriority, const char* iFunctionName)
:	fPriority(iPriority)
,	fFunctionName(iFunctionName)
	{
	if (const S& s = S(fPriority, "ZLOGFUNCTION>>"))
		s << fFunctionName;
	}

FunctionEntryExit::~FunctionEntryExit()
	{
	if (const S& s = S(fPriority, "ZLOGFUNCTION<<"))
		s << fFunctionName;
	}

// =================================================================================================
// MARK: - ZLog::sLogTrace

static const char* spTruncateFileName(const char* iFilename)
	{
	#if ZCONFIG_SPI_Enabled(Win)
		if (const char* truncatedFilename = strrchr(iFilename, '\\'))
		return truncatedFilename + 1;
	#else
		if (const char* truncatedFilename = strrchr(iFilename, '/'))
		return truncatedFilename + 1;
	#endif

	return iFilename;
	}

void sLogTrace(EPriority iPriority, const char* iFile, int iLine, const char* iFunctionName)
	{
	if (const S& s = S(iPriority, "ZLOGTRACE"))
		s << spTruncateFileName(iFile) << ":" << ZStringf("%d", iLine) << ", in " << iFunctionName;
	}

} // namespace ZLog
} // namespace ZooLib
