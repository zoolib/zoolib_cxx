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
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZYad_XMLPList.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Static parsing functions

static void spThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_XMLPList(iMessage);
	}

static void spBegin(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_Begin(r, iTagName))
		spThrowParseException("Expected begin tag '" + iTagName + "'");
	}

static void spEnd(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_End(r, iTagName))
		spThrowParseException("Expected end tag '" + iTagName + "'");
	}

static bool spTryRead_Any(ZML::Reader& r, ZAny& oVal)
	{
	if (r.Current() == ZML::eToken_TagEmpty)
		{
		if (r.Name() == "true")
			{
			oVal = true;
			r.Advance();
			return true;
			}

		if (r.Name() == "false")
			{
			oVal = false;
			r.Advance();
			return true;
			}

		return false;
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
		if (!ZUtil_Strim::sTryRead_SignedDecimalInteger(ZStrimU_Unreader(r.TextStrim()), theInt64))
			spThrowParseException("Expected valid integer");

		oVal = int32(theInt64);
		}
	else if (tagName == "real")
		{
		double theDouble;
		if (!ZUtil_Strim::sTryRead_SignedDouble(ZStrimU_Unreader(r.TextStrim()), theDouble))
			spThrowParseException("Expected valid real");

		oVal = theDouble;
		}
	else if (tagName == "date")
		{
		oVal = ZUtil_Time::sFromString_ISO8601(r.TextString());
		}
	else
		{
		// Hmm. Ignore tags we don't recognize?
		spThrowParseException("Invalid begin tag '" + tagName + "'");
		}

	spEnd(r, tagName);
	return true;
	}

static ZRef<ZYadR> spMakeYadR_XMLPList(ZRef<ZML::Readerer> iReaderer)
	{
	ZML::Reader& theR = iReaderer->GetReader();
	sSkipText(theR);

	if (theR.Current() == ZML::eToken_TagBegin)
		{
		if (theR.Name() == "dict")
			{
			theR.Advance();
			return new ZYadMapR_XMLPList(iReaderer, true);
			}
		else if (theR.Name() == "array")
			{
			theR.Advance();
			return new ZYadListR_XMLPList(iReaderer, true);
			}
		else if (theR.Name() == "data")
			{
			theR.Advance();
			return new ZYadStreamR_XMLPList(iReaderer, true);
			}
		else if (theR.Name() == "string")
			{
			theR.Advance();
			return new ZYadStrimR_XMLPList(iReaderer, true);
			}
		}
	
	ZAny theVal;
	if (spTryRead_Any(theR, theVal))
		return new ZYadPrimR_Std(theVal);

	return ZRef<ZYadR>();
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

ZYadStreamR_XMLPList::ZYadStreamR_XMLPList(ZRef<ZML::Readerer> iReaderer, bool iMustReadEndTag)
:	fReaderer(iReaderer),
	fMustReadEndTag(iMustReadEndTag),
	fStreamR_ASCIIStrim(fReaderer->GetReader().TextStrim()),
	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{}

void ZYadStreamR_XMLPList::Finish()
	{
	fStreamR_Base64Decode.SkipAll();

	if (fMustReadEndTag)
		spEnd(fReaderer->GetReader(), "data");
	}

const ZStreamR& ZYadStreamR_XMLPList::GetStreamR()
	{ return fStreamR_Base64Decode; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_XMLPList

ZYadStrimR_XMLPList::ZYadStrimR_XMLPList(ZRef<ZML::Readerer> iReaderer, bool iMustReadEndTag)
:	fReaderer(iReaderer),
	fMustReadEndTag(iMustReadEndTag)
	{}

void ZYadStrimR_XMLPList::Finish()
	{
	fReaderer->GetReader().Advance();
	if (fMustReadEndTag)
		spEnd(fReaderer->GetReader(), "string");
	}

const ZStrimR& ZYadStrimR_XMLPList::GetStrimR()
	{ return fReaderer->GetReader().TextStrim(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadReaderRep_XMLPList

ZYadListR_XMLPList::ZYadListR_XMLPList(ZRef<ZML::Readerer> iReaderer, bool iMustReadEndTag)
:	fReaderer(iReaderer),
	fMustReadEndTag(iMustReadEndTag)
	{}

void ZYadListR_XMLPList::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	ZML::Reader& theR = fReaderer->GetReader();

	sSkipText(theR);

	if (fMustReadEndTag)
		{
		if (sTryRead_End(theR, "array"))
			return;
		}

	if (!(oYadR = spMakeYadR_XMLPList(fReaderer)))
		{
		if (!fMustReadEndTag)
			return;
		spThrowParseException("Expected a value");
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_XMLPList

ZYadMapR_XMLPList::ZYadMapR_XMLPList(ZRef<ZML::Readerer> iReaderer, bool iMustReadEndTag)
:	fReaderer(iReaderer),
	fMustReadEndTag(iMustReadEndTag)
	{}
	
void ZYadMapR_XMLPList::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	ZML::Reader& theR = fReaderer->GetReader();

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

	oName = theR.TextString();

	spEnd(theR, "key");

	if (!(oYadR = spMakeYadR_XMLPList(fReaderer)))
		spThrowParseException("Expected value after <key>...</key>");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_XMLPList

ZRef<ZYadR> ZYad_XMLPList::sMakeYadR(ZRef<ZML::Readerer> iReaderer)
	{
	ZML::Reader& theR = iReaderer->GetReader();

	for (;;)
		{
		sSkipText(theR);
		if (theR.Current() != ZML::eToken_TagBegin || theR.Name() != "plist")
			break;
		theR.Advance();
		}

	return spMakeYadR_XMLPList(iReaderer);
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

static void spToStrim_List(const ZML::StrimW& s, ZRef<ZYadListR> iYadListR)
	{
	s.Begin("array");
		while (ZRef<ZYadR> theChild = iYadListR->ReadInc())
			ZYad_XMLPList::sToStrimW_ML(theChild, s);
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
			ZYad_XMLPList::sToStrimW_ML(theChild, s);
			}
	s.End("dict");
	}

static void spToStrim_Any(const ZML::StrimW& s, const ZAny& iVal)
	{
	int64 asInt64;
	double asDouble;

	if (false)
		{}
	else if (const bool* theValue = ZAnyCast<bool>(&iVal))
		{
		if (*theValue)
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
			s.Writef("%.17g", *theValue);
		s.End("real");
		}
	else if (const ZTime* theValue = ZAnyCast<ZTime>(&iVal))
		{
		s.Begin("date");
			s << ZUtil_Time::sAsString_ISO8601(*theValue, true);
		s.End("date");
		}
	else
		{
		s.Raw() << "<nil/> <!--!! Unhandled: */" << iVal.type().name() << " !!-->";
		}
	}

void ZYad_XMLPList::sToStrimW_ML(ZRef<ZYadR> iYadR, const ZML::StrimW& s)
	{
	if (!iYadR)
		{
		return;
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		spToStrim_Map(s, theYadMapR);
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		spToStrim_List(s, theYadListR);
		}
	else if (ZRef<ZYadStreamR> theYadStreamR = ZRefDynamicCast<ZYadStreamR>(iYadR))
		{
		spToStrim_Stream(s, theYadStreamR->GetStreamR());
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = ZRefDynamicCast<ZYadStrimR>(iYadR))
		{
		spToStrim_Strim(s, theYadStrimR->GetStrimR());
		}
	else if (ZRef<ZYadPrimR> theYadPrimR = ZRefDynamicCast<ZYadPrimR>(iYadR))
		{
		spToStrim_Any(s, theYadPrimR->AsAny());
		}
	else
		{
		s.Raw() << "<nil/> <!--!! Unhandled Yad !!-->";
		}
	}

NAMESPACE_ZOOLIB_END
