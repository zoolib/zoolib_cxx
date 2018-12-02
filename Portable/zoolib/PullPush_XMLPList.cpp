/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/PullPush_XMLPList.h"

//#include "zoolib/ChanR_Bin_HexStrim.h"
//#include "zoolib/ChanR_XX_Boundary.h"
//#include "zoolib/ChanR_XX_Terminated.h"
//#include "zoolib/Chan_XX_Buffered.h"
//#include "zoolib/Chan_UTF_string.h"
//#include "zoolib/Chan_Bin_ASCIIStrim.h"
//#include "zoolib/Chan_Bin_Base64.h"
//#include "zoolib/Chan_UTF_Escaped.h"
//#include "zoolib/NameUniquifier.h" // For sName
//#include "zoolib/ParseException.h"
//#include "zoolib/Unicode.h"

#include "zoolib/Chan_Bin_ASCIIStrim.h"
#include "zoolib/Chan_Bin_Base64.h"
#include "zoolib/ChanRU_UTF.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/Coerce_Any.h"
#include "zoolib/Data_Any.h"
#include "zoolib/UTCDateTime.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_Time.h"

namespace ZooLib {

using namespace PullPush;
using std::string;

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

//static void spPull_String_Push(const ZooLib::ChanRU_UTF& iChanRU, const ChanW_Any& iChanW)
//	{
//	PullPushPair<UTF32> thePullPushPair = sMakePullPushPair<UTF32>();
//	sPush(sGetClear(thePullPushPair.second), iChanW);
//	sECopyAll(iChanRU, *thePullPushPair.first);
//	sDisconnectWrite(*thePullPushPair.first);
//	}

static void spPull_Base64_Push(const ZooLib::ChanRU_UTF& iChanRU, const ChanW_Any& iChanW)
	{
	PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
	sPush(sGetClear(thePullPushPair.second), iChanW);

	ChanR_Bin_ASCIIStrim theStreamR_ASCIIStrim(iChanRU);
	ChanR_Bin_Base64Decode theStreamR_Base64Decode(theStreamR_ASCIIStrim);
	sECopyAll(theStreamR_Base64Decode, *thePullPushPair.first);
	sDisconnectWrite(*thePullPushPair.first);
	}

// =================================================================================================
#pragma mark -

bool sPull_XMLPList_Push(ML::ChanRU_UTF& iChanRU, const ChanW_Any& iChanW)
	{
	sSkipText(iChanRU);
	if (iChanRU.Current() == ML::eToken_TagEmpty)
		{
		string theName = iChanRU.Name();
		if (theName == "dict")
			{
			iChanRU.Advance();
			sPush(kStartMap, iChanW);
			sPush(kEnd, iChanW);
			return true;
			}
		else if (theName == "array")
			{
			iChanRU.Advance();
			sPush(kStartSeq, iChanW);
			sPush(kEnd, iChanW);
			return true;
			}
		else if (theName == "string")
			{
			iChanRU.Advance();
			sPush(string(), iChanW);
			return true;
			}
		else if (theName == "data")
			{
			iChanRU.Advance();
			sPush(Data_Any(), iChanW);
			return true;
			}
		else
			{
			spThrowParseException("Unknown empty tag " + theName);
			}
		}
	else if (iChanRU.Current() == ML::eToken_TagBegin)
		{
		if (iChanRU.Name() == "plist")
			{
			iChanRU.Advance();
			sPull_XMLPList_Push(iChanRU, iChanW);
			spSkipThenEndOrThrow(iChanRU, "plist");
			return true;
			}
		else if (iChanRU.Name() == "dict")
			{
			iChanRU.Advance();
			sPush(kStartMap, iChanW);
			for (;;)
				{
				sSkipText(iChanRU);
				if (sTryRead_End(iChanRU, "dict"))
					{
					sPush(kEnd, iChanW);
					return true;
					}

				if (not sTryRead_Begin(iChanRU, "key"))
					spThrowParseException("Expected <key>");

				Name theName = sReadAllUTF8(iChanRU);

				sPush(theName, iChanW);

				spSkipThenEndOrThrow(iChanRU, "key");

				if (not sPull_XMLPList_Push(iChanRU, iChanW))
					spThrowParseException("Expected value after <key>...</key>");
				}
			}
		else if (iChanRU.Name() == "array")
			{
			iChanRU.Advance();
			sPush(kStartSeq, iChanW);
			for (;;)
				{
				sSkipText(iChanRU);
				if (sTryRead_End(iChanRU, "array"))
					{
					sPush(kEnd, iChanW);
					return true;
					}

				if (not sPull_XMLPList_Push(iChanRU, iChanW))
					spThrowParseException("Expected a value");
				}
			}
		else if (iChanRU.Name() == "string")
			{
			iChanRU.Advance();
			sPull_UTF_Push(iChanRU, iChanW);
			spSkipThenEndOrThrow(iChanRU, "string");
			return true;
			}
		else if (iChanRU.Name() == "data")
			{
			iChanRU.Advance();
			spPull_Base64_Push(iChanRU, iChanW);
			spSkipThenEndOrThrow(iChanRU, "data");
			return true;
			}
		}
	else
		{
		Any theVal;
		if (spTryRead_Any(iChanRU, theVal))
			{
			sPush(theVal, iChanW);
			return true;
			}
		}
	return false;
	}

// =================================================================================================
#pragma mark -


static bool spPull_Push_XMLPList(const Any& iAny, const ChanR_Any& iChanR, const ML::StrimW& iChanW)
	{
	const ML::StrimW& s = iChanW;

	if (false)
		{}

	else if (const string* theString = sPGet<string>(iAny))
		{
		s.Begin("string");
			sEWrite(s, *theString);
		s.End("string");
		}

	else if (ZRef<ChannerR_UTF> theChanner = sGet<ZRef<ChannerR_UTF>>(iAny))
		{
		s.Begin("string");
			sECopyAll(*theChanner, s);
		s.End("string");
		}

	else if (const Data_Any* theData = sPGet<Data_Any>(iAny))
		{
		s.Begin("data");
			sEWriteMem(ChanW_Bin_Base64Encode(ChanW_Bin_ASCIIStrim(s)),
				theData->GetPtr(), theData->GetSize());
		s.End("data");
		}

	else if (ZRef<ChannerR_Bin> theChanner = sGet<ZRef<ChannerR_Bin>>(iAny))
		{
		s.Begin("data");
			sECopyAll(*theChanner, ChanW_Bin_Base64Encode(ChanW_Bin_ASCIIStrim(s)));
		s.End("data");
		}

	else if (sPGet<PullPush::StartMap>(iAny))
		{
		s.Begin("dict");
			for (;;)
				{
				ZQ<Any> theNameOrEndQ = sQRead(iChanR);
				if (not theNameOrEndQ)
					return false;

				if (sPGet<PullPush::End>(*theNameOrEndQ))
					break;

				const Name* theNameStar = sPGet<Name>(*theNameOrEndQ);
				if (not theNameStar)
					return false;

				s.Begin("key");
					sEWrite(s, *theNameStar);
				s.End("key");

				if (ZQ<Any,false> theNotQ = sQRead(iChanR))
					{
					return false;
					}
				else if (not spPull_Push_XMLPList(*theNotQ, iChanR, iChanW))
					{
					return false;
					}
				}
		s.End("dict");
		}

	else if (sPGet<PullPush::StartSeq>(iAny))
		{
		s.Begin("array");
			for (;;)
				{
				if (ZQ<Any,false> theNotQ = sQRead(iChanR))
					{
					return false;
					}
				else if (sPGet<PullPush::End>(*theNotQ))
					{
					break;
					}
				else if (not spPull_Push_XMLPList(*theNotQ, iChanR, iChanW))
					{
					return false;
					}
				}
		s.End("array");
		}

	else if (iAny.IsNull())
		{
		s.Empty("nil");
		}
	else if (const bool* asBool = iAny.PGet<bool>())
		{
		if (*asBool)
			s.Empty("true");
		else
			s.Empty("false");
		}
	else if (ZQ<int64> asIntQ = sQCoerceInt(iAny))
		{
		s.Begin("integer");
			sEWritef(s, "%lld", *asIntQ);
		s.End("integer");
		}
	else if (ZQ<double> asDoubleQ = sQCoerceRat(iAny))
		{
		s.Begin("real");
			Util_Chan::sWriteExact(s, *asDoubleQ);
		s.End("real");
		}
	else if (const UTCDateTime* theValue = iAny.PGet<UTCDateTime>())
		{
		s.Begin("date");
			s << Util_Time::sAsString_ISO8601(*theValue, true);
		s.End("date");
		}
	else
		{
		s.Begin("nil");
			s.Raw() << "<!--!! Unhandled: */" << iAny.Type().name() << " !!-->";
		s.End("nil");
		}

	return true;
	}

bool sPull_Push_XMLPList(const ChanR_Any& iChanR, const ML::StrimW& iChanW)
	{
	if (ZQ<Any> theQ = sQRead(iChanR))
		{
		spPull_Push_XMLPList(*theQ, iChanR, iChanW);
		return true;
		}
	return false;
	}

void sWriteXMLPListPreamble(const ML::StrimW& s)
	{
	s.PI("xml");
		s.Attr("version", "1.0");
		s.Attr("encoding", "UTF-8");

	s.Tag("!DOCTYPE");
		s.Attr("plist");
		s.Attr("PUBLIC");
		s.Attr("\"-//Apple Computer//DTD PLIST 1.0//EN\"");
		s.Attr("\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");

	s.Begin("plist");
		s.Attr("version", "1.0");
	}

void sWriteXMLPListPostamble(const ML::StrimW& s)
	{
	s.End("plist");
	}

} // namespace ZooLib
