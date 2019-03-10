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

static void spSkipThenEndOrThrow(ML::ChanRU& r, const string& iTagName)
	{
	sSkipText(r);
	sTryRead_End(r, iTagName) || sThrow_ParseException("Expected end tag '" + iTagName + "'");
	}

static void spPull_Base64_Push_PPT(const ZooLib::ChanRU_UTF& iChanRU, const ChanW_PPT& iChanW)
	{
	PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	ChanR_Bin_ASCIIStrim theStreamR_ASCIIStrim(iChanRU);
	ChanR_Bin_Base64Decode theStreamR_Base64Decode(theStreamR_ASCIIStrim);
	sECopyAll(theStreamR_Base64Decode, *thePullPushPair.first);
	sDisconnectWrite(*thePullPushPair.first);
	}

// =================================================================================================
#pragma mark -

void sPull_XMLPList_Push_PPT(ML::ChanRU& iChanRU, const ChanW_PPT& iChanW)
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
			}
		else if (theName == "array")
			{
			iChanRU.Advance();
			sPush(kStartSeq, iChanW);
			sPush(kEnd, iChanW);
			}
		else if (theName == "string")
			{
			iChanRU.Advance();
			sPush(string(), iChanW);
			}
		else if (theName == "data")
			{
			iChanRU.Advance();
			sPush(Data_Any(), iChanW);
			}
		else if (iChanRU.Name() == "true")
			{
			iChanRU.Advance();
			sPush(true, iChanW);
			}
		else if (iChanRU.Name() == "false")
			{
			iChanRU.Advance();
			sPush(false, iChanW);
			}
		else if (iChanRU.Name() == "nil")
			{
			iChanRU.Advance();
			sPush(null, iChanW);
			}
		else
			{
			sThrow_ParseException("Unhandled empty tag " + theName);
			}
		}
	else if (iChanRU.Current() == ML::eToken_TagBegin)
		{
		if (iChanRU.Name() == "plist")
			{
			iChanRU.Advance();
			sPull_XMLPList_Push_PPT(iChanRU, iChanW);
			spSkipThenEndOrThrow(iChanRU, "plist");
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
					break;
					}

				if (not sTryRead_Begin(iChanRU, "key"))
					sThrow_ParseException("Expected <key>");

				Name theName = sReadAllUTF8(iChanRU);

				sPush(theName, iChanW);

				spSkipThenEndOrThrow(iChanRU, "key");

				sPull_XMLPList_Push_PPT(iChanRU, iChanW);
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
					break;
					}

				sPull_XMLPList_Push_PPT(iChanRU, iChanW);
				}
			}
		else if (iChanRU.Name() == "string")
			{
			iChanRU.Advance();
			sPull_UTF_Push_PPT(iChanRU, iChanW);
			spSkipThenEndOrThrow(iChanRU, "string");
			}
		else if (iChanRU.Name() == "data")
			{
			iChanRU.Advance();
			spPull_Base64_Push_PPT(iChanRU, iChanW);
			spSkipThenEndOrThrow(iChanRU, "data");
			}
		else if (iChanRU.Name() == "integer")
			{
			iChanRU.Advance();
			int64 theInt64;
			if (not Util_Chan::sTryRead_SignedDecimalInteger(iChanRU, theInt64))
				sThrow_ParseException("Expected valid integer");
			sPush(theInt64, iChanW);
			spSkipThenEndOrThrow(iChanRU, "integer");
			}
		else if (iChanRU.Name() == "real")
			{
			iChanRU.Advance();
			int64 theInt64;
			double theDouble;
			bool isDouble;
			if (not Util_Chan::sTryRead_SignedGenericNumber(iChanRU, theInt64, theDouble, isDouble))
				sThrow_ParseException("Expected valid real");

			if (isDouble)
				sPush(theDouble, iChanW);
			else
				sPush(theInt64, iChanW);
			spSkipThenEndOrThrow(iChanRU, "real");
			}
		else if (iChanRU.Name() == "date")
			{
			iChanRU.Advance();
			UTCDateTime theUTCDateTime = Util_Time::sFromString_ISO8601(sReadAllUTF8(iChanRU));
			sPush(theUTCDateTime, iChanW);
			spSkipThenEndOrThrow(iChanRU, "date");
			}
		else
			{
			sThrow_ParseException("Unhandled begin tag '" + iChanRU.Name() + "'");
			}
		}
	}

// =================================================================================================
#pragma mark -

static void spPull_PPT_Push_XMLPList(const PPT& iPPT, const ChanR_PPT& iChanR, const ML::StrimW& iChanW)
	{
	const ML::StrimW& s = iChanW;

	if (false)
		{}

	else if (const string* theString = sPGet<string>(iPPT))
		{
		s.Begin("string");
			sEWrite(s, *theString);
		s.End("string");
		}

	else if (ZRef<ChannerR_UTF> theChanner = sGet<ZRef<ChannerR_UTF>>(iPPT))
		{
		s.Begin("string");
			sECopyAll(*theChanner, s);
		s.End("string");
		}

	else if (const Data_Any* theData = sPGet<Data_Any>(iPPT))
		{
		s.Begin("data");
			sEWriteMem(ChanW_Bin_Base64Encode(ChanW_Bin_ASCIIStrim(s)),
				theData->GetPtr(), theData->GetSize());
		s.End("data");
		}

	else if (ZRef<ChannerR_Bin> theChanner = sGet<ZRef<ChannerR_Bin>>(iPPT))
		{
		s.Begin("data");
			sECopyAll(*theChanner, ChanW_Bin_Base64Encode(ChanW_Bin_ASCIIStrim(s)));
		s.End("data");
		}

	else if (sPGet<PullPush::StartMap>(iPPT))
		{
		s.Begin("dict");
			for (;;)
				{
				if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
					{
					break;
					}
				else
					{
					s.Begin("key");
						sEWrite(s, *theNameQ);
					s.End("key");

					if (NotQ<PPT> theNotQ = sQRead(iChanR))
						{
						sThrow_ParseException("Require value after Name from ChanR_PPT");
						}
					else
						{
						spPull_PPT_Push_XMLPList(*theNotQ, iChanR, iChanW);
						}
					}
				}
		s.End("dict");
		}

	else if (sPGet<PullPush::StartSeq>(iPPT))
		{
		s.Begin("array");
			for (;;)
				{
				if (NotQ<PPT> theNotQ = sQEReadPPTOrEnd(iChanR))
					break;
				else
					spPull_PPT_Push_XMLPList(*theNotQ, iChanR, iChanW);
				}
		s.End("array");
		}

	else if (iPPT.IsNull())
		{
		s.Empty("nil");
		}
	else if (const bool* asBool = iPPT.PGet<bool>())
		{
		if (*asBool)
			s.Empty("true");
		else
			s.Empty("false");
		}
	else if (ZQ<int64> asIntQ = sQCoerceInt(iPPT.As<Any>()))
		{
		s.Begin("integer");
			sEWritef(s, "%lld", *asIntQ);
		s.End("integer");
		}
	else if (ZQ<double> asDoubleQ = sQCoerceRat(iPPT.As<Any>()))
		{
		s.Begin("real");
			Util_Chan::sWriteExact(s, *asDoubleQ);
		s.End("real");
		}
	else if (const UTCDateTime* theValue = iPPT.PGet<UTCDateTime>())
		{
		s.Begin("date");
			s << Util_Time::sAsString_ISO8601(*theValue, true);
		s.End("date");
		}
	else
		{
		s.Begin("nil");
			s.Raw() << "<!--!! Unhandled: */" << iPPT.Type().name() << " !!-->";
		s.End("nil");
		}
	}

bool sPull_PPT_Push_XMLPList(const ChanR_PPT& iChanR, const ML::StrimW& iChanW)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		spPull_PPT_Push_XMLPList(*theQ, iChanR, iChanW);
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
