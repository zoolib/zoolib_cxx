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

class YadR
:	public ZCounted
	{
protected:
	YadR();

public:
// Our protocol
	virtual void Finish();
	virtual ZRef<YadR> Meta();
	};

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

class YadStreamerR
:	public virtual YadR
,	public ChannerR_Bin
	{
public:
	};

// =================================================================================================
#pragma mark -
#pragma mark YadStrimmerR

class YadStrimmerR
:	public virtual YadR
,	public ChannerR_UTF
	{
	};

// =================================================================================================
#pragma mark -
#pragma mark YadSeqR

class YadSeqR
:	public virtual YadR
	{
public:
// Our protocol
	virtual ZRef<YadR> ReadInc() = 0;

	virtual bool Skip();
	virtual void SkipAll();
	};

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


// =================================================================================================
#pragma mark -
#pragma mark YadR_Any

class YadR_Any
:	public virtual YadR
	{
public:
	YadR_Any(const Any& iAny);
	virtual ~YadR_Any();

// Our protocol
	const Any& GetAny();

protected:
	Any fAny;
	};

// =================================================================================================
#pragma mark -
#pragma mark YadAtomR_Any

ZRef<YadAtomR> sMake_YadAtomR_Any(const Any& iAny);

// =================================================================================================
#pragma mark -
#pragma mark sYadR

ZRef<YadR> sYadR(const std::string& iVal);

// =================================================================================================
#pragma mark -
#pragma mark YadMapR_WithFirst

class YadMapR_WithFirst
:	public YadMapR
	{
public:
	YadMapR_WithFirst(const ZRef<YadR>& iFirst, const Name& iFirstName,
		const ZRef<YadMapR>& iRemainder);

// From YadMapR
	virtual ZRef<YadR> ReadInc(Name& oName);

private:
	ZRef<YadR> fFirst;
	const Name fFirstName;
	const ZRef<YadMapR> fRemainder;
	};

} // namespace ZooLib

#endif // __ZooLib_Yad_h__
