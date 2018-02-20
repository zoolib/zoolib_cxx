/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZooLib_Yad_h__
#define __ZooLib_Yad_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/Name.h"
#include "zoolib/SafePtrStack.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark YadOptions

struct YadOptions
	{
	YadOptions(bool iDoIndentation = false);

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

inline bool sDoIndentation(const YadOptions& iYO)
	{ return iYO.fIndentStringQ && iYO.fIndentStringQ->size(); }

inline std::string sEOLString(const YadOptions& iYO)
	{ return iYO.fEOLStringQ.DGet("\n"); }

inline std::string sIndentString(const YadOptions& iYO)
	{ return iYO.fIndentStringQ.DGet("  "); }

inline size_t sRawChunkSize(const YadOptions& iYO)
	{ return iYO.fRawChunkSizeQ.DGet(64); }

inline std::string sRawByteSeparator(const YadOptions& iYO)
	{ return iYO.fRawByteSeparatorQ.DGet(" "); }

inline bool sRawAsASCII(const YadOptions& iYO)
	{ return iYO.fRawAsASCIIQ.DGet(sDoIndentation(iYO)); }

inline bool sBreakStrings(const YadOptions& iYO)
	{ return iYO.fBreakStringsQ.DGet(true); }

inline size_t sStringLineLength(const YadOptions& iYO)
	{ return iYO.fStringLineLengthQ.DGet(80); }

inline bool sIDsHaveDecimalVersionComment(const YadOptions& iYO)
	{ return iYO.fIDsHaveDecimalVersionCommentQ.DGet(true); }

inline bool sTimesHaveUserLegibleComment(const YadOptions& iYO)
	{ return iYO.fTimesHaveUserLegibleCommentQ.DGet(true); }

// =================================================================================================
#pragma mark -
#pragma mark YadParseException

class YadParseException : public std::runtime_error
	{
public:
	YadParseException(const std::string& iWhat);
	YadParseException(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark YadR

typedef ZCounted YadR;
typedef ZRef<YadR> RefYad;

// =================================================================================================
#pragma mark -
#pragma mark YadAtomR

class YadAtomR
:	public virtual YadR
	{
public:
// Our protocol
	virtual Any AsAny() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark YadStreamerR

typedef ChannerR_Bin YadStreamerR;

// =================================================================================================
#pragma mark -
#pragma mark YadStrimmerR

typedef ChannerR_UTF YadStrimmerR;

// =================================================================================================
#pragma mark -
#pragma mark YadSeqR

using ChanR_RefYad = DeriveFrom<Aspect_Read<RefYad>>;

using YadSeqR = Channer<ChanR_RefYad>;

inline RefYad sReadInc(const ChanR_RefYad& iChan)
	{
	RefYad result;
	if (1 == sRead(iChan, &result, 1))
		return result;
	return null;
	}

inline RefYad sReadInc(const ZRef<YadSeqR>& iYadSeqR)
	{ return sReadInc(*iYadSeqR); }

// =================================================================================================
#pragma mark -
#pragma mark YadSeqAtR

using ChanAtR_RefYad = DeriveFrom<Aspect_ReadAt<uint64,RefYad>,Aspect_Size>;
using YadSeqAtR = Channer<ChanAtR_RefYad>;

inline RefYad sReadAt(const ChanReadAt<uint64,RefYad>& iChan, uint64 iLoc)
	{
	RefYad result;
	if (1 == sReadAt(iChan, iLoc, &result, 1))
		return result;
	return null;
	}

inline RefYad sReadAt(const ZRef<YadSeqAtR>& iYadSeqAtR, uint64 iLoc)
	{ return sReadAt(*iYadSeqAtR, iLoc); }

// =================================================================================================
#pragma mark -
#pragma mark YadSeqAtRPos

using ChanRPos_RefYad = DeriveFrom<Aspect_Read<RefYad>,Aspect_ReadAt<uint64,RefYad>,Aspect_Size>;
using YadSeqAtRPos = Channer<ChanRPos_RefYad>;

// =================================================================================================
#pragma mark -
#pragma mark YadMapR

typedef std::pair<Name,RefYad> NameRefYad;

using ChanR_NameRefYad = DeriveFrom<Aspect_Read<NameRefYad>>;

using YadMapR = Channer<ChanR_NameRefYad>;

inline RefYad sReadInc(const ChanR_NameRefYad& iChan, Name& oName)
	{
	NameRefYad result;
	if (1 == sRead(iChan, &result, 1))
		{
		oName = result.first;
		return result.second;
		}
	return null;
	}

inline RefYad sReadInc(const ZRef<YadMapR>& iYadMapR, Name& oName)
	{ return sReadInc(*iYadMapR, oName); }

// =================================================================================================
#pragma mark -
#pragma mark YadMapAtR

using ChanAtR_NameRefYad = DeriveFrom<Aspect_ReadAt<Name,RefYad>>;
using YadMapAtR = Channer<ChanAtR_NameRefYad>;

inline RefYad sReadAt(const ChanReadAt<Name,RefYad>& iChan, const Name& iLoc)
	{
	RefYad result;
	if (1 == sReadAt(iChan, iLoc, &result, 1))
		return result;
	return null;
	}

inline RefYad sReadAt(const ZRef<YadMapAtR>& iYadMapAtR, const Name& iLoc)
	{ return sReadAt(*iYadMapAtR, iLoc); }

// =================================================================================================
#pragma mark -
#pragma mark YadMapAtRPos

using ChanRPos_NameRefYad = DeriveFrom<Aspect_Read<NameRefYad>,Aspect_ReadAt<Name,RefYad>>;
using YadMapAtRPos = Channer<ChanRPos_NameRefYad>;

} // namespace ZooLib

#endif // __ZooLib_Yad_h__
