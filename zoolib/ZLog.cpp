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

#include "ZLog.h"
#include "ZString.h"
#include "ZThread.h"

// Metrowerks Standard Library doesn't include strcasecmp
#if _MSL_USING_MW_C_HEADERS && __MACH__
#	include "/usr/include/string.h"
#endif

using std::string;

static ZMutexNR sMutex;
static ZLog::LogMeister* sLogMeister;

// =================================================================================================
#pragma mark -
#pragma mark * String/integer mapping

static const char* const sNames[] =
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

ZLog::EPriority ZLog::sPriorityFromName(const string& iString)
	{
	const char* asCharStar = iString.c_str();
	for (int priority = 0; priority <= eDebug; ++priority)
		{
		if (0 == strcasecmp(asCharStar, sNames[priority]))
			return priority;
		}
	return -1;
	}

string ZLog::sNameFromPriority(EPriority iPriority)
	{
	if (iPriority < 0)
		return ZString::sFormat("%d", iPriority);

	if (iPriority <= eDebug)
		return sNames[iPriority];

	return sNames[eDebug] + ZString::sFormat("+%d", iPriority - eDebug);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZLog::StrimW

ZLog::StrimW::StrimW(EPriority iPriority, const string& iName)
:	fPriority(iPriority),
	fName(iName)
	{}

ZLog::StrimW::StrimW(EPriority iPriority, const char* iName)
:	fPriority(iPriority),
	fName(iName)
	{}

ZLog::StrimW::~StrimW()
	{
	if (!fMessage.empty())
		this->pEmit();
	}

void ZLog::StrimW::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
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

void ZLog::StrimW::Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU)
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

void ZLog::StrimW::Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU)
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

ZLog::StrimW::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(sLogMeister && sLogMeister->Enabled(fPriority, fName)); }

void ZLog::StrimW::Emit() const
	{
	if (!fMessage.empty())
		{
		const_cast<StrimW*>(this)->pEmit();
		fMessage.clear();
		}
	}

void ZLog::StrimW::pEmit()
	{
	sMutex.Acquire();
	if (sLogMeister)
		{
		try
			{
			sLogMeister->LogIt(fPriority, fName, fMessage);
			}
		catch (...)
			{}
		}
	sMutex.Release();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZLog::LogMeister

ZLog::LogMeister::LogMeister()
	{}

ZLog::LogMeister::LogMeister(const LogMeister&)
	{}

ZLog::LogMeister& ZLog::LogMeister::operator=(const LogMeister&)
	{ return *this; }
	
ZLog::LogMeister::~LogMeister()
	{}

bool ZLog::LogMeister::Enabled(EPriority iPriority, const string& iName)
	{ return true; }

void ZLog::sSetLogMeister(LogMeister* iLogMeister)
	{
	sMutex.Acquire();
	delete sLogMeister;
	sLogMeister = iLogMeister;
	sMutex.Release();
	}
