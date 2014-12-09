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

#include "zoolib/Yad.h"

namespace ZooLib {

using std::min;
using std::string;

/**
\defgroup Yad Yad (Yet Another Data)
Yad is another neologism by <a href="http://www.linkedin.com/in/ericbwcooper">Eric Cooper</a>.
It's an acronym for <em>Yet Another Data</em>, but interestingly is also a Hebrew word.
From <a href="http://en.wikipedia.org/wiki/Yad">Wikipedia</a>:

	\par
	<em>A yad (Hebrew: יד‎), literally, 'hand,' is a Jewish ritual pointer, used to point to the text
	during the Torah reading from the parchment Torah scrolls. It is intended to prevent anyone
	from touching the parchment, which is considered sacred; additionally, the fragile parchment
	can be damaged by the oils of the skin. While not required when chanting from the Torah, it
	is used frequently.</em>

An instance of YadR points within some data source, can be moved through that source, and can
return further instances of YadR referencing substructures of that source. Basically it's a
generic facility for accessing data that looks like ZooLib ZVal suite -- CFDictionary,
NSDictionary, PList, XMLRPC, JSON, Javascript types etc.

The idea is that there are a Map-like and Seq-like entities in many APIs, and that abstracting
access to them allows code to be applied to any of them.
*/

// =================================================================================================
// MARK: - YadOptions

YadOptions::YadOptions(bool iDoIndentation)
:	fRawChunkSize(16),
	fRawByteSeparator(" "),
	fRawAsASCII(iDoIndentation),
	fBreakStrings(true),
	fStringLineLength(80),
	fIDsHaveDecimalVersionComment(iDoIndentation),
	fTimesHaveUserLegibleComment(true)
	{
	if (iDoIndentation)
		{
		fEOLString = "\n";
		fIndentString = "  ";
		}
	}

// =================================================================================================
// MARK: - YadParseException

YadParseException::YadParseException(const string& iWhat)
:	runtime_error(iWhat)
	{}

YadParseException::YadParseException(const char* iWhat)
:	runtime_error(iWhat)
	{}

// =================================================================================================
// MARK: - YadR

/**
\class YadR
\ingroup Yad
\sa Yad

*/

YadR::YadR()
	{}

void YadR::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Yad* theVisitor = sDynNonConst<Visitor_Yad>(&iVisitor))
		this->Accept_Yad(*theVisitor);
	else
		Visitee::Accept(iVisitor);
	}

void YadR::Finish()
	{}

ZRef<YadR> YadR::Meta()
	{ return null; }

void YadR::Accept_Yad(Visitor_Yad& iVisitor)
	{ iVisitor.Visit_YadR(this); }

// =================================================================================================
// MARK: - YadAtomR

/**
\class YadAtomR
\ingroup Yad
\sa Yad

*/

void YadAtomR::Accept_Yad(Visitor_Yad& iVisitor)
	{ iVisitor.Visit_YadAtomR(this); }

bool YadAtomR::IsSimple(const YadOptions& iOptions)
	{ return true; }

// =================================================================================================
// MARK: - YadStreamerR

/**
\class YadStreamerR
\ingroup Yad
\sa Yad

*/

void YadStreamerR::Accept_Yad(Visitor_Yad& iVisitor)
	{ iVisitor.Visit_YadStreamerR(this); }

bool YadStreamerR::IsSimple(const YadOptions& iOptions)
	{ return false; }

// =================================================================================================
// MARK: - YadStrimmerR

/**
\class YadStrimmerR
\ingroup Yad
\sa Yad

*/

void YadStrimmerR::Accept_Yad(Visitor_Yad& iVisitor)
	{ iVisitor.Visit_YadStrimmerR(this); }

bool YadStrimmerR::IsSimple(const YadOptions& iOptions)
	{ return false; }

// =================================================================================================
// MARK: - YadSeqR

/**
\class YadSeqR
\ingroup Yad
\sa Yad

*/

void YadSeqR::Accept_Yad(Visitor_Yad& iVisitor)
	{ iVisitor.Visit_YadSeqR(this); }

bool YadSeqR::IsSimple(const YadOptions& iOptions)
	{ return false; }

bool YadSeqR::Skip()
	{ return this->ReadInc(); }

void YadSeqR::SkipAll()
	{
	while (this->Skip())
		{}
	}

// =================================================================================================
// MARK: - YadMapR

/**
\class YadMapR
\ingroup Yad
\sa Yad

*/

void YadMapR::Accept_Yad(Visitor_Yad& iVisitor)
	{ iVisitor.Visit_YadMapR(this); }

bool YadMapR::IsSimple(const YadOptions& iOptions)
	{ return false; }

bool YadMapR::Skip()
	{
	Name dummy;
	return this->ReadInc(dummy);
	}

void YadMapR::SkipAll()
	{
	while (this->Skip())
		{}
	}

// =================================================================================================
// MARK: - Visitor_Yad

/**
\class Visitor_Yad
\ingroup Yad
\sa Yad

*/

void Visitor_Yad::Visit_YadR(const ZRef<YadR>& iYadR)
	{ this->Visit(iYadR); }

void Visitor_Yad::Visit_YadAtomR(const ZRef<YadAtomR>& iYadAtomR)
	{ this->Visit_YadR(iYadAtomR); }

void Visitor_Yad::Visit_YadStreamerR(const ZRef<YadStreamerR>& iYadStreamerR)
	{ this->Visit_YadR(iYadStreamerR); }

void Visitor_Yad::Visit_YadStrimmerR(const ZRef<YadStrimmerR>& iYadStrimmerR)
	{ this->Visit_YadR(iYadStrimmerR); }

void Visitor_Yad::Visit_YadSeqR(const ZRef<YadSeqR>& iYadSeqR)
	{ this->Visit_YadR(iYadSeqR); }

void Visitor_Yad::Visit_YadMapR(const ZRef<YadMapR>& iYadMapR)
	{ this->Visit_YadR(iYadMapR); }

// =================================================================================================
// MARK: - YadR_Any

YadR_Any::YadR_Any(const ZAny& iAny)
:	fAny(iAny)
	{}

YadR_Any::~YadR_Any()
	{}

const ZAny& YadR_Any::GetAny()
	{ return fAny; }

// =================================================================================================
// MARK: - YadAtomR_Any

namespace {

class YadAtomR_Any;

class SafeStackLink_YadAtomR_Any
:	public SafePtrStackLink<YadAtomR_Any,SafeStackLink_YadAtomR_Any>
	{};

SafePtrStack_WithDestroyer<YadAtomR_Any,SafeStackLink_YadAtomR_Any> spSafeStack_YadAtomR_Any;

class YadAtomR_Any
:	public virtual YadAtomR
,	public virtual YadR_Any
,	public SafeStackLink_YadAtomR_Any
	{
public:
	YadAtomR_Any(const ZAny& iAny)
	:	YadR_Any(iAny)
		{}

	virtual ~YadAtomR_Any()
		{}

// From ZCounted
	virtual void Finalize()
		{
		bool finalized = this->FinishFinalize();
		ZAssert(finalized);
		ZAssert(not this->IsReferenced());
		fAny.Clear();

		spSafeStack_YadAtomR_Any.Push(this);
		}

// From YadAtomR
	virtual ZAny AsAny()
		{ return this->GetAny(); }

// Our protocol
	void SetAny(const ZAny& iAny)
		{ fAny = iAny; }
	};

} // anonymous namespace

ZRef<YadAtomR> sMake_YadAtomR_Any(const ZAny& iAny)
	{
	if (YadAtomR_Any* result = spSafeStack_YadAtomR_Any.PopIfNotEmpty<YadAtomR_Any>())
		{
		result->SetAny(iAny);
		return result;
		}
	return new YadAtomR_Any(iAny);
	}

// =================================================================================================
// MARK: - YadStrimmerU_String

//YadStrimmerU_String::YadStrimmerU_String(const ZAny& iAny)
//:	YadR_Any(iAny)
//,	ZStrimmerU_T<ZStrimU_String8Ref>(fAny.Get<string8>())
//	{}
//
//YadStrimmerU_String::YadStrimmerU_String(const string& iString)
//:	YadR_Any(ZAny(iString))
//,	ZStrimmerU_T<ZStrimU_String8Ref>(fAny.Get<string8>())
//	{}
//
//bool YadStrimmerU_String::IsSimple(const YadOptions& iOptions)
//	{ return true; }

// =================================================================================================
// MARK: - sYadR

//ZRef<YadR> sYadR(const string& iVal)
//	{ return new YadStrimmerU_String(iVal); }

// =================================================================================================
// MARK: - YadMapR_WithFirst

YadMapR_WithFirst::YadMapR_WithFirst(const ZRef<YadR>& iFirst, const Name& iFirstName,
	const ZRef<YadMapR>& iRemainder)
:	fFirst(iFirst)
,	fFirstName(iFirstName)
,	fRemainder(iRemainder)
	{}

ZRef<YadR> YadMapR_WithFirst::ReadInc(Name& oName)
	{
	if (fFirst)
		{
		oName = fFirstName;
		return sGetSet(fFirst, null);
		}
	return fRemainder->ReadInc(oName);
	}

} // namespace ZooLib
