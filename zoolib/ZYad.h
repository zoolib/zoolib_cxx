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

#ifndef __ZYad__
#define __ZYad__ 1
#include "zconfig.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZStrimmer.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor_Yad;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadOptions

struct ZYadOptions
	{
	ZYadOptions(bool iDoIndentation = false);

	std::string fEOLString;
	std::string fIndentString;

	int fRawChunkSize;
	std::string fRawByteSeparator;
	bool fRawAsASCII;

	bool fBreakStrings;
	int fStringLineLength;

	bool fIDsHaveDecimalVersionComment;

	bool fTimesHaveUserLegibleComment;

	bool DoIndentation() const { return !fIndentString.empty(); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException

class ZYadParseException : public std::runtime_error
	{
public:
	ZYadParseException(const std::string& iWhat);
	ZYadParseException(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR

class ZYadR : public ZRefCountedWithFinalize
	{
protected:
	ZYadR();

public:
// From ZRefCountedWithFinalize
	virtual void Accept(ZVisitor& iVisitor);
	
// Our protocol
	virtual void Finish();
	virtual ZRef<ZYadR> Meta();
	virtual void Accept_Yad(ZVisitor_Yad& iVisitor);
	virtual bool IsSimple(const ZYadOptions& iOptions) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR

class ZYadPrimR
:	public virtual ZYadR
	{
public:
// From ZYadR
	virtual void Accept_Yad(ZVisitor_Yad& iVisitor);
	virtual bool IsSimple(const ZYadOptions& iOptions);

// Our protocol
	virtual ZAny AsAny() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR

class ZYadStreamR
:	public virtual ZYadR
,	public virtual ZStreamerR
	{
public:
// From ZYadR
	virtual void Accept_Yad(ZVisitor_Yad& iVisitor);
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR

class ZYadStrimR
:	public virtual ZYadR
,	public virtual ZStrimmerR
	{
// From ZYadR
	virtual void Accept_Yad(ZVisitor_Yad& iVisitor);
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR

class ZYadSeqR : public virtual ZYadR
	{
public:
// From ZYadR
	virtual void Accept_Yad(ZVisitor_Yad& iVisitor);
	virtual bool IsSimple(const ZYadOptions& iOptions);

// Our protocol
	virtual ZRef<ZYadR> ReadInc() = 0;

	virtual bool Skip();
	virtual void SkipAll();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos

class ZYadSeqRPos : public virtual ZYadSeqR
	{
public:
// From ZYadR
	virtual void Accept_Yad(ZVisitor_Yad& iVisitor);
	virtual bool IsSimple(const ZYadOptions& iOptions);

// Default implementation of ZYadSeqR protocol
	virtual bool Skip();
	virtual void SkipAll();

// Our protocol
	virtual ZRef<ZYadSeqRPos> Clone() = 0;

	virtual uint64 GetPosition() = 0;
	virtual void SetPosition(uint64 iPosition) = 0;

	virtual uint64 GetSize() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR

class ZYadMapR : public virtual ZYadR
	{
public:
// From ZYadR
	virtual void Accept_Yad(ZVisitor_Yad& iVisitor);
	virtual bool IsSimple(const ZYadOptions& iOptions);

// Our protocol
	virtual ZRef<ZYadR> ReadInc(std::string& oName) = 0;
	virtual bool Skip();
	virtual void SkipAll();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos

class ZYadMapRPos : public virtual ZYadMapR
	{
public:
// From ZYadR
	virtual void Accept_Yad(ZVisitor_Yad& iVisitor);
	virtual bool IsSimple(const ZYadOptions& iOptions);

// Our protocol
	virtual ZRef<ZYadMapRPos> Clone() = 0;

	virtual void SetPosition(const std::string& iName) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Yad

class ZVisitor_Yad : public virtual ZVisitor
	{
public:
	virtual void Visit_YadR(ZRef<ZYadR> iYadR);
	virtual void Visit_YadPrimR(ZRef<ZYadPrimR> iYadPrimR);
	virtual void Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR);
	virtual void Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR);
	virtual void Visit_YadSeqR(ZRef<ZYadSeqR> iYadSeqR);
	virtual void Visit_YadSeqRPos(ZRef<ZYadSeqRPos> iYadSeqRPos);
	virtual void Visit_YadMapR(ZRef<ZYadMapR> iYadMapR);
	virtual void Visit_YadMapRPos(ZRef<ZYadMapRPos> iYadMapRPos);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_Any

class ZYadPrimR_Any
:	public ZYadPrimR
	{
public:
	ZYadPrimR_Any(const ZAny& iAny);
	virtual ~ZYadPrimR_Any();

// From ZYadPrimR
	virtual ZAny AsAny();

// Our protocol
	const ZAny& GetAny();

private:
	const ZAny fAny;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimU_String

class ZYadStrimU_String
:	public ZYadStrimR
,	public ZStrimmerU_T<ZStrimU_String>
	{
public:
	ZYadStrimU_String(const std::string& iString);

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const std::string& iVal);

NAMESPACE_ZOOLIB_END

#endif // __ZYad__
