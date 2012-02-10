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

#include "zoolib/ZYad.h"

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

An instance of ZYadR points within some data source, can be moved through that source, and can
return further instances of ZYadR referencing substructures of that source. Basically it's a
generic facility for accessing data that looks like ZooLib ZVal suite -- CFDictionary,
NSDictionary, PList, XMLRPC, JSON, Javascript types etc.

The idea is that there are a Map-like and Seq-like entities in many APIs, and that abstracting
access to them allows code to be applied to any of them.
*/

// =================================================================================================
// MARK: - ZYadOptions

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
// MARK: - ZYadParseException

ZYadParseException::ZYadParseException(const string& iWhat)
:	runtime_error(iWhat)
	{}

ZYadParseException::ZYadParseException(const char* iWhat)
:	runtime_error(iWhat)
	{}

// =================================================================================================
// MARK: - ZYadR

/**
\class ZYadR
\ingroup Yad
\sa Yad

*/

ZYadR::ZYadR()
	{}

void ZYadR::Accept(const ZVisitor& iVisitor)
	{
	if (ZVisitor_Yad* theVisitor = DynNonConst<ZVisitor_Yad>(&iVisitor))
		this->Accept_Yad(*theVisitor);
	else
		ZVisitee::Accept(iVisitor);
	}

void ZYadR::Finish()
	{}

ZRef<ZYadR> ZYadR::Meta()
	{ return null; }

void ZYadR::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadR(this); }

// =================================================================================================
// MARK: - ZYadAtomR

/**
\class ZYadAtomR
\ingroup Yad
\sa Yad

*/

void ZYadAtomR::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadAtomR(this); }

bool ZYadAtomR::IsSimple(const ZYadOptions& iOptions)
	{ return true; }

// =================================================================================================
// MARK: - ZYadStreamR

/**
\class ZYadStreamR
\ingroup Yad
\sa Yad

*/

void ZYadStreamR::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadStreamR(this); }

bool ZYadStreamR::IsSimple(const ZYadOptions& iOptions)
	{ return false; }

// =================================================================================================
// MARK: - ZYadStrimR

/**
\class ZYadStrimR
\ingroup Yad
\sa Yad

*/

void ZYadStrimR::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadStrimR(this); }

bool ZYadStrimR::IsSimple(const ZYadOptions& iOptions)
	{ return false; }

// =================================================================================================
// MARK: - ZYadSeqR

/**
\class ZYadSeqR
\ingroup Yad
\sa Yad

*/

void ZYadSeqR::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadSeqR(this); }

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
// MARK: - ZYadSeqRClone

void ZYadSeqRClone::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadSeqR(this); }

bool ZYadSeqRClone::IsSimple(const ZYadOptions& iOptions)
	{
	if (ZRef<ZYadSeqRClone> clone = this->Clone())
		{
		if (ZRef<ZYadR> theYadR = clone->ReadInc())
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
// MARK: - ZYadSeqRPos

/**
\class ZYadSeqRPos
\ingroup Yad
\sa Yad

*/

void ZYadSeqRPos::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadSeqRPos(this); }

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
// MARK: - ZYadMapR

/**
\class ZYadMapR
\ingroup Yad
\sa Yad

*/

void ZYadMapR::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadMapR(this); }

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
// MARK: - ZYadMapRClone

void ZYadMapRClone::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadMapRClone(this); }

// =================================================================================================
// MARK: - ZYadMapRPos

void ZYadMapRPos::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadMapRPos(this); }

// =================================================================================================
// MARK: - ZYadSat

void ZYadSat::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadSat(this); }

// =================================================================================================
// MARK: - ZYadMat

void ZYadMat::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadMat(this); }

ZQ<ZAny> ZYadMat::QAsAny()
	{ return null; }

// =================================================================================================
// MARK: - ZYadSatRPos

void ZYadSatRPos::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadSatRPos(this); }

// =================================================================================================
// MARK: - ZYadMatRPos

void ZYadMatRPos::Accept_Yad(ZVisitor_Yad& iVisitor)
	{ iVisitor.Visit_YadMatRPos(this); }

// =================================================================================================
// MARK: - ZVisitor_Yad

/**
\class ZVisitor_Yad
\ingroup Yad
\sa Yad

*/

void ZVisitor_Yad::Visit_YadR(const ZRef<ZYadR>& iYadR)
	{ this->Visit(iYadR); }

void ZVisitor_Yad::Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
	{ this->Visit_YadR(iYadAtomR); }

void ZVisitor_Yad::Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR)
	{ this->Visit_YadR(iYadStreamR); }

void ZVisitor_Yad::Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR)
	{ this->Visit_YadR(iYadStrimR); }

void ZVisitor_Yad::Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
	{ this->Visit_YadR(iYadSeqR); }

void ZVisitor_Yad::Visit_YadSeqRClone(const ZRef<ZYadSeqRClone>& iYadSeqRClone)
	{ this->Visit_YadSeqR(iYadSeqRClone); }

void ZVisitor_Yad::Visit_YadSeqRPos(const ZRef<ZYadSeqRPos>& iYadSeqRPos)
	{ this->Visit_YadSeqRClone(iYadSeqRPos); }

void ZVisitor_Yad::Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
	{ this->Visit_YadR(iYadMapR); }

void ZVisitor_Yad::Visit_YadMapRClone(const ZRef<ZYadMapRClone>& iYadMapRClone)
	{ this->Visit_YadMapR(iYadMapRClone); }

void ZVisitor_Yad::Visit_YadMapRPos(const ZRef<ZYadMapRPos>& iYadMapRPos)
	{ this->Visit_YadMapRClone(iYadMapRPos); }

void ZVisitor_Yad::Visit_YadSat(const ZRef<ZYadSat>& iYadSat)
	{ this->Visit_YadR(iYadSat); }

void ZVisitor_Yad::Visit_YadSatRPos(const ZRef<ZYadSatRPos>& iYadSatRPos)
	{
	this->Visit_YadSat(iYadSatRPos);
	this->Visit_YadSeqRPos(iYadSatRPos);
	}

void ZVisitor_Yad::Visit_YadMat(const ZRef<ZYadMat>& iYadMat)
	{ this->Visit_YadR(iYadMat); }

void ZVisitor_Yad::Visit_YadMatRPos(const ZRef<ZYadMatRPos>& iYadMatRPos)
	{
	this->Visit_YadMat(iYadMatRPos);
	this->Visit_YadMapRPos(iYadMatRPos);
	}

// =================================================================================================
// MARK: - ZVisitor_Yad_PreferRPos

void ZVisitor_Yad_PreferRPos::Visit_YadSatRPos(const ZRef<ZYadSatRPos>& iYadSatRPos)
	{ this->Visit_YadSeqRPos(iYadSatRPos); }

void ZVisitor_Yad_PreferRPos::Visit_YadMatRPos(const ZRef<ZYadMatRPos>& iYadMatRPos)
	{ this->Visit_YadMapRPos(iYadMatRPos); }

// =================================================================================================
// MARK: - ZVisitor_Yad_PreferAt

void ZVisitor_Yad_PreferAt::Visit_YadSatRPos(const ZRef<ZYadSatRPos>& iYadSatRPos)
	{ this->Visit_YadSat(iYadSatRPos); }

void ZVisitor_Yad_PreferAt::Visit_YadMatRPos(const ZRef<ZYadMatRPos>& iYadMatRPos)
	{ this->Visit_YadMat(iYadMatRPos); }

// =================================================================================================
// MARK: - ZYadAtomR_Any

ZYadAtomR_Any::ZYadAtomR_Any(const ZAny& iAny)
:	fAny(iAny)
	{}

ZYadAtomR_Any::~ZYadAtomR_Any()
	{}

ZAny ZYadAtomR_Any::AsAny()
	{ return fAny; }

const ZAny& ZYadAtomR_Any::GetAny()
	{ return fAny; }

// =================================================================================================
// MARK: - ZYadStrimU_String

ZYadStrimU_String::ZYadStrimU_String(const string& iString)
:	ZStrimmerU_T<ZStrimU_String>(iString)
	{}

bool ZYadStrimU_String::IsSimple(const ZYadOptions& iOptions)
	{ return true; }

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(const string& iVal)
	{ return new ZYadStrimU_String(iVal); }

} // namespace ZooLib
