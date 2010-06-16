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

#include "zoolib/ZLog.h"
#include "zoolib/ZString.h"
#include "zoolib/ZThread.h"

using std::string;

namespace ZooLib {

namespace ZLog {

static ZMtx spMutex;
static LogMeister* spLogMeister;

// =================================================================================================
#pragma mark -
#pragma mark * String/integer mapping

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
		if (0 == ZString::sEquali(iString, spNames[priority]))
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
#pragma mark -
#pragma mark * ZLog::StrimW

StrimW::StrimW(EPriority iPriority, const string& iName)
:	fPriority(iPriority),
	fName(iName)
	{}

StrimW::StrimW(EPriority iPriority, const char* iName)
:	fPriority(iPriority),
	fName(iName)
	{}

StrimW::~StrimW()
	{
	if (!fMessage.empty())
		this->pEmit();
	}

void StrimW::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{
	try
		{
		ZStrimW_String(fMessage).Write(iSource, iCountCU, oCountCU);
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
		ZStrimW_String(fMessage).Write(iSource, iCountCU, oCountCU);
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
		ZStrimW_String(fMessage).Write(iSource, iCountCU, oCountCU);
		}
	catch (...)
		{
		if (oCountCU)
			*oCountCU = 0;
		}
	}

StrimW::operator operator_bool_type() const
	{
	return operator_bool_generator_type::translate(spLogMeister
		&& spLogMeister->Enabled(fPriority, fName));
	}

void StrimW::Emit() const
	{
	if (!fMessage.empty())
		{
		const_cast<StrimW*>(this)->pEmit();
		fMessage.clear();
		}
	}

void StrimW::pEmit()
	{
	spMutex.Acquire();
	if (spLogMeister)
		{
		try
			{
			spLogMeister->LogIt(fPriority, fName, fMessage);
			}
		catch (...)
			{}
		}
	spMutex.Release();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZLog::LogMeister

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

void sSetLogMeister(LogMeister* iLogMeister)
	{
	spMutex.Acquire();
	delete spLogMeister;
	spLogMeister = iLogMeister;
	spMutex.Release();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZLog::FunctionEntryExit

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
#pragma mark -
#pragma mark * ZLog::sLogTrace

void sLogTrace(EPriority iPriority, const char* iFile, int iLine)
	{
	if (const S& s = S(iPriority, "ZLOGTRACE"))
		s << iFile << ":" << ZStringf("%d", iLine);
	}

} // namespace ZLog

} // namespace ZooLib
