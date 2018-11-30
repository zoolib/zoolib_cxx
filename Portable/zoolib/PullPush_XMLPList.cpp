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
#include "zoolib/Data_Any.h"
#include "zoolib/UTCDateTime.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_Time.h"

namespace ZooLib {

using namespace PullPush;
//using namespace PullPush_JSON;
//using Util_Chan::sSkip_WSAndCPlusPlusComments;
//using Util_Chan::sTryRead_CP;
//using std::min;
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

static void spPull_String_Push(const ZooLib::ChanRU_UTF& iChanRU, const ChanW_Any& iChanW)
	{
	PullPushPair<UTF32> thePullPushPair = sMakePullPushPair<UTF32>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sECopyAll(iChanRU, *thePullPushPair.first);
	sDisconnectWrite(*thePullPushPair.first);
	}

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
			spPull_String_Push(iChanRU, iChanW);
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

//bool sPull_Push_sPull_XMLPList(const ChanR_Any& iChanR, const ML::StrimW& iChanW);


} // namespace ZooLib
