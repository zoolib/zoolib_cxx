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

	std::string fEOLString;
	std::string fIndentString;

	size_t fRawChunkSize;
	std::string fRawByteSeparator;
	bool fRawAsASCII;
	ZQ<uint64> fRawSizeCap;

	bool fBreakStrings;
	int fStringLineLength;

	bool fIDsHaveDecimalVersionComment;

	bool fTimesHaveUserLegibleComment;

	bool DoIndentation() const { return !fIndentString.empty(); }
	};

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

void sFinish(const RefYad& iYad);

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

inline RefYad sReadInc(const ChanR<RefYad>& iAspectRead)
	{
	RefYad result;
	if (1 == sQRead(iAspectRead, &result, 1))
		return result;
	return null;
	}

inline RefYad sReadInc(const ZRef<YadSeqR>& iYadSeqR)
	{ return sReadInc(*iYadSeqR); }

// =================================================================================================
#pragma mark -
#pragma mark YadMapR

class YadMapR
:	public virtual YadR
	{
public:
// Our protocol
	virtual ZRef<YadR> ReadInc(Name& oName) = 0;
	virtual bool Skip();
	virtual void SkipAll();
	};

class ZYadMapAtR : public YadMapR
	{
public:
	virtual ZRef<YadR> ReadAt(const Name&) = 0;
	};

class ZYadMapAtRPos : public ZYadMapAtR
	{
public:
//	virtual ZRef<YadR> ReadAt(const std::string&) = 0;
	};

} // namespace ZooLib

#endif // __ZooLib_Yad_h__
