// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/Log.h"

#include "zoolib/Compat_string.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_string.h"

namespace ZooLib {
namespace Log {

using std::string;

ZP<LogMeister> sLogMeister;

// =================================================================================================
#pragma mark - Log::CallDepth

CallDepth::CallDepth(bool iActive)
:	fActive(iActive)
,	fPrior(sCurrent())
,	fTV(this)
	{}

CallDepth::~CallDepth()
	{}

const CallDepth* CallDepth::sCurrent()
	{
	if (const CallDepth* const* theCD = TVCallDepth::sPGet())
		return *theCD;
	return nullptr;
	}

size_t CallDepth::sCount()
	{
	size_t count = 0;
	for (const CallDepth* current = sCurrent(); current; current = current->fPrior)
		{ ++count; }
	return count;
	}

size_t CallDepth::sCountActive()
	{
	size_t count = 0;
	for (const CallDepth* current = sCurrent(); current; current = current->fPrior)
		{
		if (current->fActive)
			++count;
		}
	return count;
	}

// =================================================================================================
#pragma mark - String/integer mapping

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
		if (0 == Util_string::sEquali(iString, spNames[priority]))
			return priority;
		}
	return -1;
	}

string sNameFromPriority(EPriority iPriority)
	{
	if (iPriority < 0)
		return sStringf("%d", iPriority);

	if (iPriority <= eDebug)
		return spNames[iPriority];

	return spNames[eDebug] + sStringf("+%d", iPriority - eDebug);
	}

// =================================================================================================
#pragma mark - Log::LogMeister

bool LogMeister::Enabled(EPriority iPriority, const string& iName)
	{ return true; }

bool LogMeister::Enabled(EPriority iPriority, const char* iName)
	{ return true; }

ZP<LogMeister> sCurrentLogMeister()
	{
	if (ZP<LogMeister> theLM = ThreadVal_LogMeister::sGet())
		return theLM;
	return sLogMeister;
	}

// =================================================================================================
#pragma mark - Log::ChanW

ChanW::ChanW(EPriority iPriority, const string& iName_String)
:	fLM(sCurrentLogMeister())
,	fPriority(iPriority)
,	fName_StringQ(iName_String)
,	fLine(-1)
,	fOutdent(false)
	{}

ChanW::ChanW(EPriority iPriority, const char* iName_CharStar, int iLine)
:	fLM(sCurrentLogMeister())
,	fPriority(iPriority)
,	fName_CharStarQ(iName_CharStar)
,	fLine(iLine)
,	fOutdent(false)
	{}

ChanW::~ChanW()
	{
	if (fLM && fMessageQ && not fMessageQ->empty())
		this->pEmit();
	}

size_t ChanW::WriteUTF8(const UTF8* iSource, size_t iCountCU)
	{
	if (fLM && iCountCU)
		sMut(fMessageQ).append(iSource, iCountCU);
	return iCountCU;
	}

ChanW::operator operator_bool() const
	{
	if (not fLM)
		{
		return operator_bool_gen::translate(false);
		}
	else if (fName_StringQ)
		{
		return operator_bool_gen::translate(
			fLM->Enabled(fPriority, *fName_StringQ));
		}
	else
		{
		return operator_bool_gen::translate(
			fLM->Enabled(fPriority, *fName_CharStarQ));
		}
	}

void ChanW::Emit() const
	{
	if (fLM && fMessageQ && not fMessageQ->empty())
		{
		const_cast<ChanW*>(this)->pEmit();
		sMut(fMessageQ).resize(0);
		}
	}

void ChanW::pEmit()
	{
	if (fLM)
		{
		if (not fName_StringQ)
			fName_StringQ = *fName_CharStarQ;

		size_t theDepth = CallDepth::sCountActive();

		if (fOutdent and theDepth > 0)
			theDepth -= 1;

		if (fLine <= 0)
			fLM->LogIt(fPriority, *fName_StringQ, theDepth, *fMessageQ);
		else
			fLM->LogIt(fPriority, *fName_StringQ + sStringf(":%d", fLine), theDepth, *fMessageQ);
		}
	}

// =================================================================================================
#pragma mark - Log::FunctionEntryExit

FunctionEntryExit::FunctionEntryExit(EPriority iPriority, const char* iFunctionName, const string& iMessage)
:	fPriority(iPriority)
,	fFunctionName(iFunctionName)
	{
	if (const ChanW& s = ChanW(fPriority, "ZLF"))
		{
		s.fOutdent = true;
		s << "> " << fFunctionName << iMessage;
		}
	}

FunctionEntryExit::FunctionEntryExit(EPriority iPriority, const char* iFunctionName)
:	fPriority(iPriority)
,	fFunctionName(iFunctionName)
	{
	if (const ChanW& s = ChanW(fPriority, "ZLF"))
		{
		s.fOutdent = true;
		s << "> " << fFunctionName;
		}
	}

FunctionEntryExit::~FunctionEntryExit()
	{
	if (const ChanW& s = ChanW(fPriority, "ZLF"))
		{
		s.fOutdent = true;
		s << "< " << fFunctionName;
		}
	}

// =================================================================================================
#pragma mark - Log::sLogTrace

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
	if (const ChanW& s = ChanW(iPriority, "ZLOGTRACE"))
		{
		s << spTruncateFileName(iFile) << ":" << sStringf("%d", iLine);
		if (iFunctionName && *iFunctionName)
		 	s << ", in " << iFunctionName;
		}
	}

} // namespace Log
} // namespace ZooLib
