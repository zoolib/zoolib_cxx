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

#include "zoolib/ZDebug.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Strim_Operators.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZYad_XMLPList.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Static parsing functions

static void spThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_XMLPList(iMessage);
	}

static void spBegin(ZML::StrimU& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_Begin(r, iTagName))
		spThrowParseException("Expected begin tag '" + iTagName + "'");
	}

static void spEnd(ZML::StrimU& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_End(r, iTagName))
		spThrowParseException("Expected end tag '" + iTagName + "'");
	}

static bool spTryRead_Any(ZML::StrimU& r, ZAny& oVal)
	{
	if (r.Current() == ZML::eToken_TagEmpty)
		{
		if (false) {}
		else if (r.Name() == "true") oVal = true;
		else if (r.Name() == "false") oVal = false;
		else if (r.Name() == "nil") oVal = ZAny();
		else return false;

		r.Advance();
		return true;
		}

	// If there's no open tag, then we're not at the start of a value.
	if (r.Current() != ZML::eToken_TagBegin)
		return false;

	const string tagName = r.Name();
	r.Advance();

	// We've read and advanced past an open tag, in tagName.
	if (false)
		{}
	else if (tagName == "integer")
		{
		int64 theInt64;
		if (!ZUtil_Strim::sTryRead_SignedDecimalInteger(r, theInt64))
			spThrowParseException("Expected valid integer");

		oVal = int32(theInt64);
		}
	else if (tagName == "real")
		{
		double theDouble;
		if (!ZUtil_Strim::sTryRead_SignedDouble(r, theDouble))
			spThrowParseException("Expected valid real");

		oVal = theDouble;
		}
	else if (tagName == "date")
		{
		oVal = ZUtil_Time::sFromString_ISO8601(r.ReadAll8());
		}
	else
		{
		// Hmm. Ignore tags we don't recognize?
		spThrowParseException("Invalid begin tag '" + tagName + "'");
		}

	spEnd(r, tagName);
	return true;
	}

static ZRef<ZYadR> spMakeYadR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU)
	{
	ZML::StrimU& theR = iStrimmerU->GetStrim();
	sSkipText(theR);

	if (theR.Current() == ZML::eToken_TagBegin)
		{
		if (theR.Name() == "dict")
			{
			theR.Advance();
			return new ZYadMapR_XMLPList(iStrimmerU, true);
			}
		else if (theR.Name() == "array")
			{
			theR.Advance();
			return new ZYadSeqR_XMLPList(iStrimmerU, true);
			}
		else if (theR.Name() == "data")
			{
			theR.Advance();
			return new ZYadStreamR_XMLPList(iStrimmerU, true);
			}
		else if (theR.Name() == "string")
			{
			theR.Advance();
			return new ZYadStrimR_XMLPList(iStrimmerU, true);
			}
		}

	ZAny theVal;
	if (spTryRead_Any(theR, theVal))
		return new ZYadAtomR_Std(theVal);

	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_XMLPList

ZYadParseException_XMLPList::ZYadParseException_XMLPList(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_XMLPList::ZYadParseException_XMLPList(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR_XMLPList

ZYadStreamR_XMLPList::ZYadStreamR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, bool iMustReadEndTag)
:	fStrimmerU(iStrimmerU),
	fMustReadEndTag(iMustReadEndTag),
	fStreamR_ASCIIStrim(fStrimmerU->GetStrimR()),
	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{}

void ZYadStreamR_XMLPList::Finish()
	{
	fStreamR_Base64Decode.SkipAll();

	if (fMustReadEndTag)
		spEnd(fStrimmerU->GetStrim(), "data");
	}

const ZStreamR& ZYadStreamR_XMLPList::GetStreamR()
	{ return fStreamR_Base64Decode; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_XMLPList

ZYadStrimR_XMLPList::ZYadStrimR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, bool iMustReadEndTag)
:	fStrimmerU(iStrimmerU),
	fMustReadEndTag(iMustReadEndTag)
	{}

void ZYadStrimR_XMLPList::Finish()
	{
	fStrimmerU->GetStrim().Advance();
	if (fMustReadEndTag)
		spEnd(fStrimmerU->GetStrim(), "string");
	}

const ZStrimR& ZYadStrimR_XMLPList::GetStrimR()
	{ return fStrimmerU->GetStrimR(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadReaderRep_XMLPList

ZYadSeqR_XMLPList::ZYadSeqR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, bool iMustReadEndTag)
:	fStrimmerU(iStrimmerU),
	fMustReadEndTag(iMustReadEndTag)
	{}

void ZYadSeqR_XMLPList::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	sSkipText(theR);

	if (fMustReadEndTag)
		{
		if (sTryRead_End(theR, "array"))
			return;
		}

	if (!(oYadR = spMakeYadR_XMLPList(fStrimmerU)))
		{
		if (!fMustReadEndTag)
			return;
		spThrowParseException("Expected a value");
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_XMLPList

ZYadMapR_XMLPList::ZYadMapR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, bool iMustReadEndTag)
:	fStrimmerU(iStrimmerU),
	fMustReadEndTag(iMustReadEndTag)
	{}

void ZYadMapR_XMLPList::Imp_ReadInc(bool iIsFirst, string& oName, ZRef<ZYadR>& oYadR)
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	sSkipText(theR);

	if (fMustReadEndTag)
		{
		if (sTryRead_End(theR, "dict"))
			return;
		}

	if (!sTryRead_Begin(theR, "key"))
		{
		if (!fMustReadEndTag)
			return;
		spThrowParseException("Expected <key>");
		}

	oName = theR.ReadAll8();

	spEnd(theR, "key");

	if (!(oYadR = spMakeYadR_XMLPList(fStrimmerU)))
		spThrowParseException("Expected value after <key>...</key>");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_XMLPList

ZRef<ZYadR> ZYad_XMLPList::sMakeYadR(ZRef<ZML::StrimmerU> iStrimmerU)
	{
	ZML::StrimU& theR = iStrimmerU->GetStrim();

	for (;;)
		{
		sSkipText(theR);
		if (theR.Current() != ZML::eToken_TagBegin || theR.Name() != "plist")
			break;
		theR.Advance();
		}

	return spMakeYadR_XMLPList(iStrimmerU);
	}

static void spToStrim_Stream(const ZML::StrimW& s, const ZStreamR& iStreamR)
	{
	s.Begin("data");
		iStreamR.CopyAllTo(ZStreamW_Base64Encode(ZStreamW_ASCIIStrim(s)));
	s.End("data");
	}

static void spToStrim_Strim(const ZML::StrimW& s, const ZStrimR& iStrimR)
	{
	s.Begin("string");
		iStrimR.CopyAllTo(s);
	s.End("string");
	}

static void spToStrim_List(const ZML::StrimW& s, ZRef<ZYadSeqR> iYadSeqR)
	{
	s.Begin("array");
		while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
			ZYad_XMLPList::sToStrim(theChild, s);
	s.End("array");
	}

static void spToStrim_Map(const ZML::StrimW& s, ZRef<ZYadMapR> iYadMapR)
	{
	s.Begin("dict");
		string theName;
		while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
			{
			s.Begin("key");
				s << theName;
			s.End("key");
			ZYad_XMLPList::sToStrim(theChild, s);
			}
	s.End("dict");
	}

static void spToStrim_Any(const ZML::StrimW& s, const ZAny& iVal)
	{
	int64 asInt64;
	double asDouble;

	if (false)
		{}
	else if (!iVal)
		{
		s.Empty("nil");
		}
	else if (const bool* asBool = iVal.PGet<bool>())
		{
		if (*asBool)
			s.Empty("true");
		else
			s.Empty("false");
		}
	else if (sQCoerceInt(iVal, asInt64))
		{
		s.Begin("integer");
			s.Writef("%lld", asInt64);
		s.End("integer");
		}
	else if (sQCoerceReal(iVal, asDouble))
		{
		s.Begin("real");
			ZUtil_Strim::sWriteExact(s, asDouble);
		s.End("real");
		}
	else if (const ZTime* theValue = iVal.PGet<ZTime>())
		{
		s.Begin("date");
			s << ZUtil_Time::sAsString_ISO8601(*theValue, true);
		s.End("date");
		}
	else
		{
		s.Raw() << "<nil/> <!--!! Unhandled: */";
		s << iVal.Type().name();
		s.Raw() << " !!-->";
		}
	}

void ZYad_XMLPList::sToStrim(ZRef<ZYadR> iYadR, const ZML::StrimW& s)
	{
	if (!iYadR)
		{
		return;
		}
	else if (ZRef<ZYadMapR> theYadMapR = iYadR.DynamicCast<ZYadMapR>())
		{
		spToStrim_Map(s, theYadMapR);
		}
	else if (ZRef<ZYadSeqR> theYadSeqR = iYadR.DynamicCast<ZYadSeqR>())
		{
		spToStrim_List(s, theYadSeqR);
		}
	else if (ZRef<ZYadStreamR> theYadStreamR = iYadR.DynamicCast<ZYadStreamR>())
		{
		spToStrim_Stream(s, theYadStreamR->GetStreamR());
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = iYadR.DynamicCast<ZYadStrimR>())
		{
		spToStrim_Strim(s, theYadStrimR->GetStrimR());
		}
	else if (ZRef<ZYadAtomR> theYadAtomR = iYadR.DynamicCast<ZYadAtomR>())
		{
		spToStrim_Any(s, theYadAtomR->AsAny());
		}
	else
		{
		s.Raw() << "<nil/> <!--!! Unhandled Yad !!-->";
		}
	}

} // namespace ZooLib
