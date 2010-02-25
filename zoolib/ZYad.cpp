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

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZYad.h"

NAMESPACE_ZOOLIB_BEGIN

using std::min;
using std::string;

/**
\defgroup Yad Yad (Yet Another Data)
Yad is another neologism by Eric Cooper. As an acronym it stands for Yet Another Data, but
interestingly it is also a hebrew word. From <http://en.wikipedia.org/wiki/Yad>:

	\par
	<em>A yad (Hebrew: יד‎), literally, 'hand,' is a Jewish ritual pointer, used to point to the text
	during the Torah reading from the parchment Torah scrolls. It is intended to prevent anyone
	from touching the parchment, which is considered sacred; additionally, the fragile parchment
	can be damaged by the oils of the skin. While not required when chanting from the Torah, it
	is used frequently.</em>

An instance of ZYadR points within some data source, can be moved through that source, and can
return further instances of ZYadR referencing substructures of that source. Basically it's a
generic facility for accessing data that looks like ZooLib ZVal suite -- CFDictionary,
NSDictionary, PList, XMLRPC, JSON, Javascript types etc.

The idea is that there are a Map-like and Seq-like entities in many APIs, and that abstracting
access to them allows code to be applied to any of them.
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZYadOptions

ZYadOptions::ZYadOptions(bool iDoIndentation)
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
#pragma mark -
#pragma mark * ZYadParseException

ZYadParseException::ZYadParseException(const string& iWhat)
:	runtime_error(iWhat)
	{}

ZYadParseException::ZYadParseException(const char* iWhat)
:	runtime_error(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR

/**
\class ZYadR
\ingroup Yad
\sa Yad

*/

ZYadR::ZYadR()
	{}

void ZYadR::Finish()
	{}

ZRef<ZYadR> ZYadR::Meta()
	{ return ZRef<ZYadR>(); }

bool ZYadR::Accept(ZVisitor_Yad& iVisitor)
	{ return iVisitor.Visit_YadR(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR

/**
\class ZYadPrimR
\ingroup Yad
\sa Yad

*/

bool ZYadPrimR::Accept(ZVisitor_Yad& iVisitor)
	{ return iVisitor.Visit_YadPrimR(this); }

bool ZYadPrimR::IsSimple(const ZYadOptions& iOptions)
	{ return true; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR

/**
\class ZYadStreamR
\ingroup Yad
\sa Yad

*/

bool ZYadStreamR::Accept(ZVisitor_Yad& iVisitor)
	{ return iVisitor.Visit_YadStreamR(this); }

bool ZYadStreamR::IsSimple(const ZYadOptions& iOptions)
	{ return false; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR

/**
\class ZYadStrimR
\ingroup Yad
\sa Yad

*/

bool ZYadStrimR::Accept(ZVisitor_Yad& iVisitor)
	{ return iVisitor.Visit_YadStrimR(this); }

bool ZYadStrimR::IsSimple(const ZYadOptions& iOptions)
	{ return false; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR

/**
\class ZYadSeqR
\ingroup Yad
\sa Yad

*/

bool ZYadSeqR::Accept(ZVisitor_Yad& iVisitor)
	{ return iVisitor.Visit_YadSeqR(this); }

bool ZYadSeqR::IsSimple(const ZYadOptions& iOptions)
	{ return false; }

bool ZYadSeqR::Skip()
	{ return this->ReadInc(); }

void ZYadSeqR::SkipAll()
	{
	while (this->Skip())
		{}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos

/**
\class ZYadSeqRPos
\ingroup Yad
\sa Yad

*/

bool ZYadSeqRPos::Accept(ZVisitor_Yad& iVisitor)
	{ return iVisitor.Visit_YadSeqRPos(this); }

bool ZYadSeqRPos::IsSimple(const ZYadOptions& iOptions)
	{
	uint64 theSize = this->GetSize();
	if (theSize == 0)
		return true;

	uint64 thePosition = this->GetPosition();
	if (thePosition == theSize - 1)
		{
		ZRef<ZYadR> theYadR = this->ReadInc();
		this->SetPosition(thePosition);
		return theYadR->IsSimple(iOptions);
		}

	return false;
	}

bool ZYadSeqRPos::Skip()
	{
	uint64 theSize = this->GetSize();
	uint64 thePosition = this->GetPosition();
	if (thePosition < theSize)
		{
		this->SetPosition(thePosition + 1);
		return true;
		}
	return false;
	}

void ZYadSeqRPos::SkipAll()
	{ this->SetPosition(this->GetSize()); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR

/**
\class ZYadMapR
\ingroup Yad
\sa Yad

*/

bool ZYadMapR::Accept(ZVisitor_Yad& iVisitor)
	{ return iVisitor.Visit_YadMapR(this); }

bool ZYadMapR::IsSimple(const ZYadOptions& iOptions)
	{ return false; }

bool ZYadMapR::Skip()
	{
	string dummy;
	return this->ReadInc(dummy);
	}

void ZYadMapR::SkipAll()
	{
	while (this->Skip())
		{}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos

/**
\class ZYadMapRPos
\ingroup Yad
\sa Yad

*/

bool ZYadMapRPos::Accept(ZVisitor_Yad& iVisitor)
	{ return iVisitor.Visit_YadMapRPos(this); }

bool ZYadMapRPos::IsSimple(const ZYadOptions& iOptions)
	{
	if (ZRef<ZYadMapRPos> clone = this->Clone())
		{
		string dummy;
		if (ZRef<ZYadR> theYadR = clone->ReadInc(dummy))
			{
			if (!clone->Skip())
				{
				// We've exhausted ouselves, so we had just one entry, and
				// we're simple if that entry is simple.
				return theYadR->IsSimple(iOptions);
				}
			else
				{
				// We have at least one more entry, so we're not simple.
				return false;
				}
			}
		else
			{
			// We're empty, and thus simple.
			return true;
			}
		}
	else
		{
		// We couldn't clone, so assume we're complex.
		return false;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Yad

/**
\class ZVisitor_Yad
\ingroup Yad
\sa Yad

*/

ZVisitor_Yad::ZVisitor_Yad()
	{}

ZVisitor_Yad::~ZVisitor_Yad()
	{}

bool ZVisitor_Yad::Visit_YadR(ZRef<ZYadR> iYadR)
	{ return false; }

bool ZVisitor_Yad::Visit_YadPrimR(ZRef<ZYadPrimR> iYadPrimR)
	{ return this->Visit_YadR(iYadPrimR); }

bool ZVisitor_Yad::Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR)
	{ return this->Visit_YadR(iYadStreamR); }

bool ZVisitor_Yad::Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR)
	{ return this->Visit_YadR(iYadStrimR); }

bool ZVisitor_Yad::Visit_YadSeqR(ZRef<ZYadSeqR> iYadSeqR)
	{ return this->Visit_YadR(iYadSeqR); }

bool ZVisitor_Yad::Visit_YadSeqRPos(ZRef<ZYadSeqRPos> iYadSeqRPos)
	{ return this->Visit_YadSeqR(iYadSeqRPos); }

bool ZVisitor_Yad::Visit_YadMapR(ZRef<ZYadMapR> iYadMapR)
	{ return this->Visit_YadR(iYadMapR); }

bool ZVisitor_Yad::Visit_YadMapRPos(ZRef<ZYadMapRPos> iYadMapRPos)
	{ return this->Visit_YadMapR(iYadMapRPos); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_Any

ZYadPrimR_Any::ZYadPrimR_Any(const ZAny& iAny)
:	fAny(iAny)
	{}

ZYadPrimR_Any::~ZYadPrimR_Any()
	{}

ZAny ZYadPrimR_Any::AsAny()
	{ return fAny; }

const ZAny& ZYadPrimR_Any::GetAny()
	{ return fAny; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimU_String

ZYadStrimU_String::ZYadStrimU_String(const string& iString)
:	ZStrimmerU_T<ZStrimU_String>(iString)
	{}

bool ZYadStrimU_String::IsSimple(const ZYadOptions& iOptions)
	{ return true; }

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const string& iVal)
	{ return new ZYadStrimU_String(iVal); }

NAMESPACE_ZOOLIB_END
