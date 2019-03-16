/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#ifndef __ZooLib_PushTextOptions_h__
#define __ZooLib_PushTextOptions_h__ 1
#include "zconfig.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZStdInt.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - PushTextOptions

struct PushTextOptions
	{
	PushTextOptions(bool iDoIndentation = false);

	ZQ<std::string> fEOLStringQ;
	ZQ<std::string> fIndentStringQ;

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
