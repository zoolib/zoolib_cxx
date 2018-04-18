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

#include "zoolib/Any_Coerce.h"
#include "zoolib/UTCDateTime.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_Time.h"
#include "zoolib/Yad_Any.h" // For sYadAtomR_Any
#include "zoolib/Yad_XMLPList.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {
namespace Yad_XMLPList {

using std::string;

// =================================================================================================
// MARK: -
// MARK:	Static parsing functions

static bool spThrowParseException(const string& iMessage)
	{
	throw ParseException(iMessage);
	return false;
	}

static void spSkipThenEndOrThrow(ML::ChanRU_UTF& r, const string& iTagName)
	{
	sSkipText(r);

	sTryRead_End(r, iTagName) || spThrowParseException("Expected end tag '" + iTagName + "'");
	}

static void spEmptyOrThrow(ML::ChanRU_UTF& r, const string& iTagName)
	{
	sTryRead_Empty(r, iTagName) || spThrowParseException("*Really* expected empty tag '" + iTagName + "'");
	}

static bool spTryRead_Any(ML::ChanRU_UTF& r, Any& oVal)
	{
	if (r.Current() == ML::eToken_TagEmpty)
		{
		if (false) {}
		else if (r.Name() == "true") oVal = true;
		else if (r.Name() == "false") oVal = false;
		else if (r.Name() == "nil") oVal = Any();
		else return false;

		r.Advance();
		return true;
		}

	// If there's no open tag, then we're not at the start of a value.
	if (r.Current() != ML::eToken_TagBegin)
		return false;

	const string tagName = r.Name();
	r.Advance();

	// We've read and advanced past an open tag, in tagName.
	if (false)
		{}
	else if (tagName == "integer")
		{
		int64 theInt64;
		if (not Util_Chan::sTryRead_SignedDecimalInteger(r, r, theInt64))
			spThrowParseException("Expected valid integer");

		oVal = int32(theInt64);
		}
	else if (tagName == "real")
		{
		int64 theInt64;
		double theDouble;
		bool isDouble;
		if (not Util_Chan::sTryRead_SignedGenericNumber(r, r, theInt64, theDouble, isDouble))
			spThrowParseException("Expected valid real");

		if (isDouble)
			oVal = theDouble;
		else
			oVal = double(theInt64);
		}
	else if (tagName == "date")
		{
		oVal = UTCDateTime(Util_Time::sFromString_ISO8601(sReadAllUTF8(r)));
		}
	else
		{
		// Hmm. Ignore tags we don't recognize?
		spThrowParseException("Invalid begin tag '" + tagName + "'");
		}

	spSkipThenEndOrThrow(r, tagName);
	return true;
	}

static ZRef<YadR> spMakeYadR_XMLPList(ZRef<ML::ChannerRU_UTF> iStrimmerU)
	{
	ML::ChanRU_UTF& theChan = *iStrimmerU;
	sSkipText(theChan);

	if (theChan.Current() == ML::eToken_TagEmpty)
		{
		if (theChan.Name() == "dict")
			{
			return new Channer_T<ChanR_NameRefYad_XMLPList>(iStrimmerU, true);
			}
		else if (theChan.Name() == "array")
			{
			return new Channer_T<ChanR_RefYad_XMLPList>(iStrimmerU, true);
			}
		else if (theChan.Name() == "data")
			{
			return new ChannerR_Bin_XMLPList(iStrimmerU, true);
			}
		else if (theChan.Name() == "string")
			{
			return new ChannerR_UTF_XMLPList(iStrimmerU, true);
			}
		}
	else if (theChan.Current() == ML::eToken_TagBegin)
		{
		if (theChan.Name() == "dict")
			{
			theChan.Advance();
			return new Channer_T<ChanR_NameRefYad_XMLPList>(iStrimmerU, false);
			}
		else if (theChan.Name() == "array")
			{
			theChan.Advance();
			return new Channer_T<ChanR_RefYad_XMLPList>(iStrimmerU, false);
			}
		else if (theChan.Name() == "data")
			{
			theChan.Advance();
			return new ChannerR_Bin_XMLPList(iStrimmerU, false);
			}
		else if (theChan.Name() == "string")
			{
			theChan.Advance();
			return new ChannerR_UTF_XMLPList(iStrimmerU, false);
			}
		}

	Any theVal;
	if (spTryRead_Any(theChan, theVal))
		return sYadAtomR_Any(theVal);

	return null;
	}

// =================================================================================================
// MARK: - ParseException

ParseException::ParseException(const string& iWhat)
:	YadParseException_Std(iWhat)
	{}

ParseException::ParseException(const char* iWhat)
:	YadParseException_Std(iWhat)
	{}

// =================================================================================================
// MARK: - ChannerR_Bin_XMLPList

ChannerR_Bin_XMLPList::ChannerR_Bin_XMLPList(ZRef<ML::ChannerRU_UTF> iStrimmerU, bool iIsEmptyTag)
:	fStrimmerU(iStrimmerU)
,	fIsEmptyTag(iIsEmptyTag)
,	fStreamR_ASCIIStrim(*iStrimmerU)
,	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{}

void ChannerR_Bin_XMLPList::Finalize()
	{
	ZRef<ML::ChannerRU_UTF> theStrimmerU = fStrimmerU;
	const bool isEmptyTag = fIsEmptyTag;

	inherited::Finalize();

	if (isEmptyTag)
		{
		spEmptyOrThrow(*theStrimmerU, "data");
		}
	else
		{
		spSkipThenEndOrThrow(*theStrimmerU, "data");
		}
	}

size_t ChannerR_Bin_XMLPList::Read(byte* oDest, size_t iCount)
	{ return sRead(fStreamR_Base64Decode, oDest, iCount); }

// =================================================================================================
// MARK: - ChannerR_UTF_XMLPList

ChannerR_UTF_XMLPList::ChannerR_UTF_XMLPList(ZRef<ML::ChannerRU_UTF> iStrimmerU, bool iIsEmptyTag)
:	fStrimmerU(iStrimmerU)
,	fIsEmptyTag(iIsEmptyTag)
	{}

void ChannerR_UTF_XMLPList::Finalize()
	{
	ZRef<ML::ChannerRU_UTF> theStrimmerU = fStrimmerU;
	const bool isEmptyTag = fIsEmptyTag;

	inherited::Finalize();

	if (isEmptyTag)
		{
		spEmptyOrThrow(*theStrimmerU, "string");
		}
	else
		{
		spSkipThenEndOrThrow(*theStrimmerU, "string");
		}
	}

size_t ChannerR_UTF_XMLPList::Read(UTF32* oDest, size_t iCount)
	{ return sRead(*fStrimmerU, oDest, iCount); }

// =================================================================================================
// MARK: - ChanR_RefYad_XMLPList

ChanR_RefYad_XMLPList::ChanR_RefYad_XMLPList(
	ZRef<ML::ChannerRU_UTF> iStrimmerU, bool iIsEmptyTag)
:	fStrimmerU(iStrimmerU)
,	fIsEmptyTag(iIsEmptyTag)
	{}

void ChanR_RefYad_XMLPList::Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR)
	{
	ML::ChanRU_UTF& theChan = *fStrimmerU;

	if (fIsEmptyTag)
		{
		spEmptyOrThrow(theChan, "array");
		}
	else
		{
		sSkipText(theChan);

		if (sTryRead_End(theChan, "array"))
			return;

		if (not (oYadR = spMakeYadR_XMLPList(fStrimmerU)))
			{
			spThrowParseException("Expected a value");
			}
		}
	}

// =================================================================================================
// MARK: - ChanR_NameRefYad_XMLPList

ChanR_NameRefYad_XMLPList::ChanR_NameRefYad_XMLPList(
	ZRef<ML::ChannerRU_UTF> iStrimmerU, bool iIsEmptyTag)
:	fStrimmerU(iStrimmerU)
,	fIsEmptyTag(iIsEmptyTag)
	{}

void ChanR_NameRefYad_XMLPList::Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR)
	{
	ML::ChanRU_UTF& theChan = *fStrimmerU;

	if (fIsEmptyTag)
		{
		spEmptyOrThrow(theChan, "dict");
		}
	else
		{
		sSkipText(theChan);

		if (sTryRead_End(theChan, "dict"))
			return;

		if (not sTryRead_Begin(theChan, "key"))
			spThrowParseException("Expected <key>");

		oName = sReadAllUTF8(theChan);

		spSkipThenEndOrThrow(theChan, "key");

		if (not (oYadR = spMakeYadR_XMLPList(fStrimmerU)))
			spThrowParseException("Expected value after <key>...</key>");
		}
	}

} // namespace Yad_XMLPlist

// =================================================================================================
// MARK: - Yad_XMLPList

ZRef<YadR> Yad_XMLPList::sYadR(ZRef<ML::ChannerRU_UTF> iStrimmerU)
	{
	if (not iStrimmerU)
		return null;

	ML::ChanRU_UTF& theChan = *iStrimmerU;

	for (;;)
		{
		sSkipText(theChan);
		if (theChan.Current() != ML::eToken_TagBegin || theChan.Name() != "plist")
			break;
		theChan.Advance();
		}

	return spMakeYadR_XMLPList(iStrimmerU);
	}

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
	else if (ZQ<int64> asIntQ = sQCoerceInt(iVal))
		{
		s.Begin("integer");
			sEWritef(s, "%lld", *asIntQ);
		s.End("integer");
		}
	else if (ZQ<double> asDoubleQ = sQCoerceRat(iVal))
		{
		s.Begin("real");
			Util_Chan::sWriteExact(s, *asDoubleQ);
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
