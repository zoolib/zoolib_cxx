// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_PushTextOptions_h__
#define __ZooLib_PushTextOptions_h__ 1
#include "zconfig.h"

#include "zoolib/StdInt.h"
#include "zoolib/ZQ.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - PushTextOptions

struct PushTextOptions
	{
	PushTextOptions(bool iDoIndentation = false);

	ZQ<std::string> fEOLStringQ;
	ZQ<std::string> fIndentStringQ;
	ZQ<bool> fIndentOnlySequencesQ;

	ZQ<size_t> fRawChunkSizeQ;
	ZQ<std::string> fRawByteSeparatorQ;
	ZQ<bool> fRawAsASCIIQ;
	ZQ<uint64> fRawSizeCapQ;

	ZQ<bool> fBreakStringsQ;
	ZQ<size_t> fStringLineLengthQ;

	ZQ<bool> fIDsHaveDecimalVersionCommentQ;

	ZQ<bool> fTimesHaveUserLegibleCommentQ;
	};

inline
PushTextOptions::PushTextOptions(bool iDoIndentation)
	{
	if (iDoIndentation)
		fIndentStringQ = "  ";
	}

inline bool sDoIndentation(const PushTextOptions& iYO)
	{ return iYO.fIndentStringQ && iYO.fIndentStringQ->size(); }

inline std::string sEOLString(const PushTextOptions& iYO)
	{ return iYO.fEOLStringQ.DGet("\n"); }

inline std::string sIndentString(const PushTextOptions& iYO)
	{ return iYO.fIndentStringQ.DGet("  "); }

inline size_t sRawChunkSize(const PushTextOptions& iYO)
	{ return iYO.fRawChunkSizeQ.DGet(64); }

inline std::string sRawByteSeparator(const PushTextOptions& iYO)
	{ return iYO.fRawByteSeparatorQ.DGet(" "); }

inline bool sRawAsASCII(const PushTextOptions& iYO)
	{ return iYO.fRawAsASCIIQ.DGet(sDoIndentation(iYO)); }

inline bool sBreakStrings(const PushTextOptions& iYO)
	{ return iYO.fBreakStringsQ.DGet(true); }

inline size_t sStringLineLength(const PushTextOptions& iYO)
	{ return iYO.fStringLineLengthQ.DGet(80); }

inline bool sIDsHaveDecimalVersionComment(const PushTextOptions& iYO)
	{ return iYO.fIDsHaveDecimalVersionCommentQ.DGet(true); }

inline bool sTimesHaveUserLegibleComment(const PushTextOptions& iYO)
	{ return iYO.fTimesHaveUserLegibleCommentQ.DGet(true); }

} // namespace ZooLib

#endif // __ZooLib_PushTextOptions_h__
