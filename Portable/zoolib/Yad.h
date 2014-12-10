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

#include "zoolib/Channer_UTF.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/Name.h"
#include "zoolib/SafePtrStack.h"
#include "zoolib/Visitor.h"

#include "zoolib/ZAny.h"

#include <string>

namespace ZooLib {

class Visitor_Yad;

// =================================================================================================
// MARK: - YadOptions

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
// MARK: - YadParseException

class YadParseException : public std::runtime_error
	{
public:
	YadParseException(const std::string& iWhat);
	YadParseException(const char* iWhat);
	};

// =================================================================================================
// MARK: - YadR

class YadR
:	public Visitee
	{
protected:
	YadR();

public:
// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// Our protocol
	virtual void Finish();
	virtual ZRef<YadR> Meta();
	virtual void Accept_Yad(Visitor_Yad& iVisitor);
	virtual bool IsSimple(const YadOptions& iOptions) = 0;
	};

// =================================================================================================
// MARK: - YadAtomR

class YadAtomR
:	public virtual YadR
	{
public:
// From YadR
	virtual void Accept_Yad(Visitor_Yad& iVisitor);
	virtual bool IsSimple(const YadOptions& iOptions);

// Our protocol
	virtual ZAny AsAny() = 0;
	};

// =================================================================================================
// MARK: - YadStreamerR

class YadStreamerR
:	public virtual YadR
,	public virtual ChannerR_Bin
	{
public:
// From YadR
	virtual void Accept_Yad(Visitor_Yad& iVisitor);
	virtual bool IsSimple(const YadOptions& iOptions);
	};

// =================================================================================================
// MARK: - YadStrimmerR

class YadStrimmerR
:	public virtual YadR
,	public virtual ChannerR_UTF
	{
// From YadR
	virtual void Accept_Yad(Visitor_Yad& iVisitor);
	virtual bool IsSimple(const YadOptions& iOptions);
	};

// =================================================================================================
// MARK: - YadSeqR

class YadSeqR
:	public virtual YadR
	{
public:
// From YadR
	virtual void Accept_Yad(Visitor_Yad& iVisitor);
	virtual bool IsSimple(const YadOptions& iOptions);

// Our protocol
	virtual ZRef<YadR> ReadInc() = 0;

	virtual bool Skip();
	virtual void SkipAll();
	};

// =================================================================================================
// MARK: - YadMapR

class YadMapR
:	public virtual YadR
	{
public:
// From YadR
	virtual void Accept_Yad(Visitor_Yad& iVisitor);
	virtual bool IsSimple(const YadOptions& iOptions);

// Our protocol
	virtual ZRef<YadR> ReadInc(Name& oName) = 0;
	virtual bool Skip();
	virtual void SkipAll();
	};

// =================================================================================================
// MARK: - Visitor_Yad

class Visitor_Yad
:	public virtual Visitor
	{
public:
	virtual void Visit_YadR(const ZRef<YadR>& iYadR);
	virtual void Visit_YadAtomR(const ZRef<YadAtomR>& iYadAtomR);
	virtual void Visit_YadStreamerR(const ZRef<YadStreamerR>& iYadStreamerR);
	virtual void Visit_YadStrimmerR(const ZRef<YadStrimmerR>& iYadStrimmerR);

	virtual void Visit_YadSeqR(const ZRef<YadSeqR>& iYadSeqR);

	virtual void Visit_YadMapR(const ZRef<YadMapR>& iYadMapR);
	};

// =================================================================================================
// MARK: - YadR_Any

class YadR_Any
:	public virtual YadR
	{
public:
	YadR_Any(const ZAny& iAny);
	virtual ~YadR_Any();

// Our protocol
	const ZAny& GetAny();

protected:
	ZAny fAny;
	};

// =================================================================================================
// MARK: - YadAtomR_Any

ZRef<YadAtomR> sMake_YadAtomR_Any(const ZAny& iAny);

// =================================================================================================
// MARK: - sYadR

ZRef<YadR> sYadR(const std::string& iVal);

// =================================================================================================
// MARK: - YadMapR_WithFirst

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
