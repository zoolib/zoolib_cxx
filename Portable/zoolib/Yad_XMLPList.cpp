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

#include "zoolib/UTCDateTime.h"
#include "zoolib/Util_Any.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_Time.h"
#include "zoolib/Yad_XMLPList.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {
namespace Yad_XMLPList {

using std::string;

#if 0

// =================================================================================================
// MARK: -
// MARK:	Static parsing functions

static void spThrowParseException(const string& iMessage)
	{
	throw ParseException(iMessage);
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
		if (not ZUtil_Strim::sTryRead_SignedDecimalInteger(r, theInt64))
			spThrowParseException("Expected valid integer");

		oVal = int32(theInt64);
		}
	else if (tagName == "real")
		{
		double theDouble;
		if (not ZUtil_Strim::sTryRead_SignedDouble(r, theDouble))
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

	if (theR.Current() == ZML::eToken_TagEmpty)
		{
		if (theR.Name() == "dict")
			{
			return new ZYadMapR_XMLPList(iStrimmerU, ZYadR_XMLPlist::eRead_EmptyTag);
			}
		else if (theR.Name() == "array")
			{
			return new ZYadSeqR_XMLPList(iStrimmerU, ZYadR_XMLPlist::eRead_EmptyTag);
			}
		else if (theR.Name() == "data")
			{
			return new ZYadStreamerR_XMLPList(iStrimmerU, ZYadR_XMLPlist::eRead_EmptyTag);
			}
		else if (theR.Name() == "string")
			{
			return new ZYadStrimmerR_XMLPList(iStrimmerU, ZYadR_XMLPlist::eRead_EmptyTag);
			}
		}
	else if (theR.Current() == ZML::eToken_TagBegin)
		{
		if (theR.Name() == "dict")
			{
			theR.Advance();
			return new ZYadMapR_XMLPList(iStrimmerU, ZYadR_XMLPlist::eRead_EndTag);
			}
		else if (theR.Name() == "array")
			{
			theR.Advance();
			return new ZYadSeqR_XMLPList(iStrimmerU, ZYadR_XMLPlist::eRead_EndTag);
			}
		else if (theR.Name() == "data")
			{
			theR.Advance();
			return new ZYadStreamerR_XMLPList(iStrimmerU, ZYadR_XMLPlist::eRead_EndTag);
			}
		else if (theR.Name() == "string")
			{
			theR.Advance();
			return new ZYadStrimmerR_XMLPList(iStrimmerU, ZYadR_XMLPlist::eRead_EndTag);
			}
		}

	ZAny theVal;
	if (spTryRead_Any(theR, theVal))
		return sMake_YadAtomR_Any(theVal);

	return null;
	}

// =================================================================================================
// MARK: - ParseException

ParseException::ParseException(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ParseException::ParseException(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
// MARK: - ZYadStreamerR_XMLPList

ZYadStreamerR_XMLPList::ZYadStreamerR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead)
:	fStrimmerU(iStrimmerU),
	fRead(iRead),
	fStreamR_ASCIIStrim(fStrimmerU->GetStrimR()),
	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{}

void ZYadStreamerR_XMLPList::Finish()
	{
	if (fRead == eRead_EmptyTag)
		{
		if (!sTryRead_Empty(fStrimmerU->GetStrim(), "data"))
			spThrowParseException("Expected </data>");
		return;
		}
	
	fStreamR_Base64Decode.SkipAll();

	if (fRead == eRead_EndTag)
		spEnd(fStrimmerU->GetStrim(), "data");
	}

const ZStreamR& ZYadStreamerR_XMLPList::GetStreamR()
	{ return fStreamR_Base64Decode; }

// =================================================================================================
// MARK: - ZYadStrimmerR_XMLPList

ZYadStrimmerR_XMLPList::ZYadStrimmerR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead)
:	fStrimmerU(iStrimmerU),
	fRead(iRead)
	{}

void ZYadStrimmerR_XMLPList::Finish()
	{
	if (fRead == eRead_EmptyTag)
		{
		if (sTryRead_Empty(fStrimmerU->GetStrim(), "string"))
			return;
		spThrowParseException("Expected </string>");
		}

	fStrimmerU->GetStrim().Advance();

	if (fRead == eRead_EndTag)
		spEnd(fStrimmerU->GetStrim(), "string");
	}

const ZStrimR& ZYadStrimmerR_XMLPList::GetStrimR()
	{ return fStrimmerU->GetStrimR(); }

// =================================================================================================
// MARK: - ZYadReaderRep_XMLPList

ZYadSeqR_XMLPList::ZYadSeqR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead)
:	fStrimmerU(iStrimmerU),
	fRead(iRead)
	{}

void ZYadSeqR_XMLPList::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	if (fRead == eRead_EmptyTag)
		{
		if (sTryRead_Empty(theR, "array"))
			return;
		spThrowParseException("Expected </array>");
		}
	
	sSkipText(theR);

	if (fRead == eRead_EndTag)
		{
		if (sTryRead_End(theR, "array"))
			return;
		}

	if (!(oYadR = spMakeYadR_XMLPList(fStrimmerU)))
		{
		if (fRead == eRead_NoTag)
			return;
		spThrowParseException("Expected a value");
		}
	}

// =================================================================================================
// MARK: - ZYadMapR_XMLPList

ZYadMapR_XMLPList::ZYadMapR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead)
:	fStrimmerU(iStrimmerU),
	fRead(iRead)
	{}

void ZYadMapR_XMLPList::Imp_ReadInc(bool iIsFirst, ZName& oName, ZRef<ZYadR>& oYadR)
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	if (fRead == eRead_EmptyTag)
		{
		if (sTryRead_Empty(theR, "dict"))
			return;
		spThrowParseException("Expected </dict>");
		}
	
	sSkipText(theR);

	if (fRead == eRead_EndTag)
		{
		if (sTryRead_End(theR, "dict"))
			return;
		}

	if (!sTryRead_Begin(theR, "key"))
		{
		if (fRead == eRead_NoTag)
			return;
		spThrowParseException("Expected <key>");
		}

	oName = theR.ReadAll8();

	spEnd(theR, "key");

	if (!(oYadR = spMakeYadR_XMLPList(fStrimmerU)))
		spThrowParseException("Expected value after <key>...</key>");
	}

#endif // 0

} // namespace Yad_XMLPlist

// =================================================================================================
// MARK: - Yad_XMLPList

#if 0

ZRef<YadR> Yad_XMLPList::sYadR(ZRef<ZML::StrimmerU> iStrimmerU)
	{
	if (not iStrimmerU)
		return null;

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

#endif

static void spToStrim_Stream(const ML::StrimW& s, const ChanR_Bin& iChanR)
	{
	s.Begin("data");
		sCopyAll(iChanR, ChanW_Bin_Base64Encode(ChanW_Bin_ASCIIStrim(s)));
	s.End("data");
	}

static void spToStrim_Strim(const ML::StrimW& s, const ChanR_UTF& iStrimR)
	{
	s.Begin("string");
		sCopyAll(iStrimR, s);
	s.End("string");
	}

static void spToStrim_List(const ML::StrimW& s, ZRef<YadSeqR> iYadSeqR)
	{
	s.Begin("array");
		while (ZRef<YadR> theChild = sReadInc(*iYadSeqR))
			Yad_XMLPList::sToStrim(theChild, s);
	s.End("array");
	}

static void spToStrim_Map(const ML::StrimW& s, ZRef<YadMapR> iYadMapR)
	{
	s.Begin("dict");
		Name theName;
		while (ZRef<YadR> theChild = sReadInc(iYadMapR, theName))
			{
			s.Begin("key");
				s << string8(theName);
			s.End("key");
			Yad_XMLPList::sToStrim(theChild, s);
			}
	s.End("dict");
	}

static void spToStrim_Any(const ML::StrimW& s, const Any& iVal)
	{
	int64 asInt64;
	double asDouble;

	if (false)
		{}
	else if (iVal.IsNull())
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
			sEWritef(s, "%lld", asInt64);
		s.End("integer");
		}
	else if (sQCoerceRat(iVal, asDouble))
		{
		s.Begin("real");
			Util_Chan::sWriteExact(s, asDouble);
		s.End("real");
		}
	else if (const UTCDateTime* theValue = iVal.PGet<UTCDateTime>())
		{
		s.Begin("date");
			s << Util_Time::sAsString_ISO8601(*theValue, true);
		s.End("date");
		}
	else
		{
		s.Begin("nil");
			s.Raw() << "<!--!! Unhandled: */" << iVal.Type().name() << " !!-->";
		s.End("nil");
		}
	}

void Yad_XMLPList::sToStrim(ZRef<YadR> iYadR, const ML::StrimW& s)
	{
	if (not iYadR)
		{
		return;
		}
	else if (ZRef<YadMapR> theYadMapR = iYadR.DynamicCast<YadMapR>())
		{
		spToStrim_Map(s, theYadMapR);
		}
	else if (ZRef<YadSeqR> theYadSeqR = iYadR.DynamicCast<YadSeqR>())
		{
		spToStrim_List(s, theYadSeqR);
		}
	else if (ZRef<YadStreamerR> theYadStreamerR = iYadR.DynamicCast<YadStreamerR>())
		{
		spToStrim_Stream(s, *theYadStreamerR);
		}
	else if (ZRef<YadStrimmerR> theYadStrimmerR = iYadR.DynamicCast<YadStrimmerR>())
		{
		spToStrim_Strim(s, *theYadStrimmerR);
		}
	else if (ZRef<YadAtomR> theYadAtomR = iYadR.DynamicCast<YadAtomR>())
		{
		spToStrim_Any(s, theYadAtomR->AsAny());
		}
	else
		{
		s.Begin("nil");
			s.Raw() << "<!--!! Unhandled Yad !!-->";
		s.End("nil");
		}
	}

void Yad_XMLPList::sWritePreamble(const ML::StrimW& s)
	{
	s.PI("xml");
		s.Attr("version", "1.0");
		s.Attr("encoding", "UTF-8");

	s.Tag("!DOCTYPE");
		s.Attr("plist");
		s.Attr("PUBLIC");
		s.Attr("\"-//Apple Computer//DTD PLIST 1.0//EN\"");
		s.Attr("\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");

	s.Tag("plist");
		s.Attr("version", "1.0");
	}

} // namespace ZooLib
