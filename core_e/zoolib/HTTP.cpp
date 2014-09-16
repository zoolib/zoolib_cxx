/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/Chan_Bin_string.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Chan_XX_Unreader.h"

#include "zoolib/HTTP.h"
#include "zoolib/MIME.h"
#include "zoolib/Util_Chan.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"
//#include "zoolib/ZStreamR_Boundary.h"
//#include "zoolib/ZStream_Limited.h"
//#include "zoolib/ZStream_String.h"
//#include "zoolib/ZStreamer.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_string.h"

#include <ctype.h>

namespace ZooLib {
namespace HTTP {

using std::max;
using std::min;
using std::pair;
using std::replace;
using std::vector;

// =================================================================================================
// MARK: - Utility stuff

static void spAppend(Map& ioFields, const string& iName, const Val& iValue)
	{ ioFields.Set(iName, Seq(ioFields.Get<Seq>(iName)).Append(iValue)); }

static uint32 spHexCharToUInt(char iChar)
	{
	if (iChar >= '0' && iChar <= '9')
		return iChar - '0';
	if (iChar >= 'A' && iChar <= 'F')
		return 10 + iChar - 'A';
	if (iChar >= 'a' && iChar <= 'f')
		return 10 + iChar - 'a';
	ZUnimplemented();
	return 0;
	}

static bool spQReadDigit(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, int& oDigit)
	{
	byte readChar;
	if (not sQRead(readChar, iChanR))
		return false;

	if (readChar < '0' || readChar > '9')
		{
		sUnread(readChar, iChanU);
		return false;
		}

	oDigit = readChar - '0';
	return true;
	}

static bool spQReadInt32(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, int32* oInt32)
	{
	if (oInt32)
		*oInt32 = 0;

	int digit;
	if (not spQReadDigit(iChanR, iChanU, digit))
		return false;

	for (;;)
		{
		if (oInt32)
			{
			*oInt32 *= 10;
			*oInt32 += digit;
			}
		if (not spQReadDigit(iChanR, iChanU, digit))
			break;
		}

	return true;
	}

static bool spQReadInt64(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, int64* oInt64)
	{
	if (oInt64)
		*oInt64 = 0;

	int digit;
	if (not spQReadDigit(iChanR, iChanU, digit))
		return false;

	for (;;)
		{
		if (oInt64)
			{
			*oInt64 *= 10;
			*oInt64 += digit;
			}
		if (not spQReadDigit(iChanR, iChanU, digit))
			break;
		}

	return true;
	}

// =================================================================================================
// MARK: - HTTP::Response

Response::Response()
	{
	fResult = 0;
	fIsVersion11 = true;
	}

void Response::Clear()
	{
	fHeaders.clear();
	fResult = 0;
	fMessage.clear();
	}

void Response::SetVersion10()
	{
	fIsVersion11 = false;
	}

void Response::SetResult(int iResult)
	{
	ZAssert(iResult >= 100 && iResult <= 999);
	fResult = iResult;
	}

void Response::SetResult(int iResult, const string& iMessage)
	{
	ZAssert(iResult >= 100 && iResult <= 999);
	fResult = iResult;
	fMessage = iMessage;
	}

void Response::Set(const string& iName, const string& iValue)
	{
	fHeaders.push_back(pair<string, string>(iName, iValue));
	}

void Response::Set(const string& iName, int iValue)
	{
	fHeaders.push_back(pair<string, string>(iName, sStringf("%d", iValue)));
	}

void Response::Set(const string& iName, uint64 iValue)
	{
	fHeaders.push_back(pair<string, string>(iName, sStringf("%lld", iValue)));
	}

void Response::Send(const ChanW_Bin& iChanW) const
	{
	ZAssert(fResult >= 100 && fResult <= 999);
	if (fIsVersion11)
		sWrite("HTTP/1.1 ", iChanW);
	else
		sWrite("HTTP/1.0 ", iChanW);

	sWritef(iChanW, "%d", fResult);
	if (not fMessage.empty())
		{
		sWrite(" ", iChanW);
		sWrite(fMessage, iChanW);
		}

	sWrite("\r\n", iChanW);

	for (vector<pair<string, string> >::const_iterator ii = fHeaders.begin();
		ii != fHeaders.end(); ++ii)
		{ sWrite_HeaderLine(ii->first, ii->second, iChanW); }

	sWrite("\r\n", iChanW);
	}

// =================================================================================================
// MARK: - HTTP, high level parsing

// This method should look at all the range entries in iRangeParam
// and turn them into a list of ascending, non overlapping start/finish
// pairs in oRanges.
bool sOrganizeRanges(size_t iSourceSize, const Val& iRangeParam,
	vector<pair<size_t, size_t> >& oRanges)
	{
	const Map asMap = iRangeParam.Get<Map>();
	if (ZQ<int64> reqBegin = asMap.QGet<int64>("begin"))
		{
		if (reqBegin.Get() <= iSourceSize)
			{
			if (ZQ<int64> reqEnd = asMap.QGet<int64>("end"))
				{
				if (reqEnd.Get() < reqBegin.Get())
					return false;
				if (reqEnd.Get() > iSourceSize)
					return false;
				oRanges.push_back(pair<size_t, size_t>(reqBegin.Get(), reqEnd.Get() + 1));
				return true;
				}
			}
		}
	else if (ZQ<int64> reqLast = asMap.QGet<int64>("last"))
		{
		if (reqLast.Get() <= iSourceSize)
			{
			oRanges.push_back(pair<size_t, size_t>(iSourceSize - reqLast.Get(), iSourceSize));
			return true;
			}
		}
	return false;
	}

bool sQReadRequest(const ChanR_Bin& iChanR, string* oMethod, string* oURL, string* oError)
	{
	MIME::ChanR_Bin_Line theSIL(iChanR);
	Chan_XX_Unreader<byte> theChanU(theSIL);

	if (oMethod)
		oMethod->resize(0);
	if (oURL)
		oURL->resize(0);
	if (oError)
		oError->resize(0);

	if (not sQReadToken(theChanU, theChanU, nullptr, oMethod))
		{
		if (oError)
			*oError = "Failed to read method";
		return false;
		}

	sSkipLWS(theChanU, theChanU);

	if (not sQReadURI(theChanU, theChanU, oURL))
		{
		if (oError)
			*oError = "Failed to read URI";
		return false;
		}

	sSkipLWS(theChanU, theChanU);

	int32 major, minor;
	if (not sQReadHTTPVersion(theChanU, theChanU, &major, &minor))
		{
		if (oError)
			*oError = "Failed to read version";
		return false;
		}

	sSkipAll(theChanU);
	return true;
	}

bool sQReadResponse(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	int32* oResultCode, string* oResultMessage)
	{
	if (oResultMessage)
		oResultMessage->resize(0);

	int32 major, minor;
	if (not sQReadHTTPVersion(iChanR, iChanU, &major, &minor))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not spQReadInt32(iChanR, iChanU, oResultCode))
		return false;

	sSkipLWS(iChanR, iChanU);

	// Suck up the remainder of the stream as the result message.
	for (;;)
		{
		byte readChar;
		if (not sQRead(readChar, iChanR))
			break;
		if (readChar == '\n')
			break;
		if (oResultMessage)
			*oResultMessage += readChar;
		}

	return true;
	}

bool sQReadHeaderNoParsing(const ChanR_Bin& iChanR, Map* oFields)
	{
	if (oFields)
		oFields->Clear();

	for (;;)
		{
		MIME::ChanR_Bin_Line theSIL(iChanR);
		Chan_XX_Unreader<byte> theChanU(theSIL);
		const bool gotOne = sQReadHeaderLineNoParsing(theChanU, theChanU, oFields);
		sSkipAll(theChanU);
		if (not gotOne)
			return true;
		}
	}

bool sQReadHeaderLineNoParsing(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	string fieldNameExact;
	if (not sQReadFieldName(iChanR, iChanU, nullptr, &fieldNameExact))
		return false;

	if (not fieldNameExact.size())
		return true;

	sSkipLWS(iChanR, iChanU);
	string fieldBody;
	sCopyAll(iChanR, ChanW_Bin_string(&fieldBody));
	if (ioFields)
		spAppend(*ioFields, fieldNameExact, fieldBody);

	return true;
	}

bool sQReadHeader(const ChanR_Bin& iChanR, Map* oFields)
	{
	if (oFields)
		oFields->Clear();

	for (;;)
		{
		MIME::ChanR_Bin_Line theSIL(iChanR);
		Chan_XX_Unreader<byte> theChanU(theSIL);
		const bool gotOne = sQReadHeaderLine(theChanU, theChanU, oFields);
		sSkipAll(theChanU);
		if (not gotOne)
			return true;
		}
	}

bool sQReadHeaderLine(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	string fieldName;
	if (not sQReadFieldName(iChanR, iChanU, &fieldName, nullptr))
		return false;

	if (fieldName.empty())
		return true;

	if (false)
		{}
	// -----------------
	// Request headers
	else if (fieldName == "accept") sQRead_accept(iChanR, iChanU, ioFields);
	else if (fieldName == "accept-charset")
		{
		for (;;)
			{
			sSkipLWS(iChanR, iChanU);

			string charset;
			if (not sQReadToken(iChanR, iChanU, &charset, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "accept-charset", charset);

			sSkipLWS(iChanR, iChanU);

			if (not sTryReadChar(iChanR, iChanU, ','))
				break;
			}
		}
	else if (fieldName == "accept-encoding")
		{
		for (;;)
			{
			sSkipLWS(iChanR, iChanU);

			string encoding;
			if (not sQReadToken(iChanR, iChanU, &encoding, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "accept-encoding", encoding);

			sSkipLWS(iChanR, iChanU);

			if (not sTryReadChar(iChanR, iChanU, ','))
				break;
			}
		}
	else if (fieldName == "accept-language") sQRead_accept_language(iChanR, iChanU, ioFields);
//	else if (fieldName == "authorization")
//	else if (fieldName == "from")
//	else if (fieldName == "host")
//	else if (fieldName == "if-modified-since")
//	else if (fieldName == "if-match")
//	else if (fieldName == "if-none-match")
//	else if (fieldName == "if-range")
//	else if (fieldName == "if-unmodified-since")
//	else if (fieldName == "max-forwards")
//	else if (fieldName == "proxy-authorization")
	else if (fieldName == "range") sQRead_range(iChanR, iChanU, ioFields);
//	else if (fieldName == "referer")
//	else if (fieldName == "user-agent")
	else if (fieldName == "cookie")
		{
		for (;;)
			{
			string cookieName, cookieValue;
			if (not sQReadParameter_Cookie(iChanR, iChanU, nullptr, &cookieValue, &cookieName))
				break;

			if (ioFields)
				{
				Map cookieMap = ioFields->Get("cookie").Get<Map>();
				cookieMap.Set(cookieName, cookieValue);
				ioFields->Set("cookie", cookieMap);
				}

			sSkipLWS(iChanR, iChanU);

			if (not sTryReadChar(iChanR, iChanU, ';'))
				break;
			}
		}
	// -----------------
	// Response headers
//	else if (fieldName == "age")
//	else if (fieldName == "location")
//	else if (fieldName == "proxy-authenticate")
//	else if (fieldName == "public")
//	else if (fieldName == "retry-after")
//	else if (fieldName == "server")
//	else if (fieldName == "vary")
//	else if (fieldName == "warning")
//	else if (fieldName == "www-authenticate")
	// -----------------
	// Request/Response headers
	else if (fieldName == "connection")
		{
		sSkipLWS(iChanR, iChanU);
		string body;
		if (sQReadToken(iChanR, iChanU, &body, nullptr))
			{
			if (ioFields)
				ioFields->Set("connection", body);
			}
		}
//	else if (fieldName == "transfer-encoding")
	// -----------------
	// Entity headers
//	else if (fieldName == "allow")
//	else if (fieldName == "content-base")
	else if (fieldName == "content-encoding")
		{
		for (;;)
			{
			sSkipLWS(iChanR, iChanU);

			string encoding;
			if (not sQReadToken(iChanR, iChanU, &encoding, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "content-encoding", encoding);

			sSkipLWS(iChanR, iChanU);

			if (not sTryReadChar(iChanR, iChanU, ','))
				break;
			}
		}
	else if (fieldName == "content-language")
		{
		for (;;)
			{
			sSkipLWS(iChanR, iChanU);

			string language;
			if (not sQReadToken(iChanR, iChanU, &language, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "content-language", language);

			sSkipLWS(iChanR, iChanU);

			if (not sTryReadChar(iChanR, iChanU, ','))
				break;
			}
		}
	else if (fieldName == "content-length") sQRead_content_length(iChanR, iChanU, ioFields);
//	else if (fieldName == "content-location")
//	else if (fieldName == "content-md5")
	else if (fieldName == "content-range") sQRead_content_range(iChanR, iChanU, ioFields);
	else if (fieldName == "content-type") sQRead_content_type(iChanR, iChanU, ioFields);
//	else if (fieldName == "etag")
//	else if (fieldName == "expires")
//	else if (fieldName == "last-modified")
//---------------
	else if (fieldName == "content-disposition") sQRead_content_disposition(iChanR, iChanU, ioFields);
	else
		{
		sSkipLWS(iChanR, iChanU);
		string fieldBody;
		sCopyAll(iChanR, ChanW_Bin_string(&fieldBody));
		if (ioFields)
			spAppend(*ioFields, fieldName, fieldBody);
		}

	return true;
	}

bool sParseQuery(const string& iString, Map& oTuple)
	{
	ChanRPos_Bin_string theChanRPos(iString);
	
	return sParseQuery(theChanRPos, theChanRPos, oTuple);
	}

bool sParseQuery(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map& oTuple)
	{
	for (;;)
		{
		byte readChar;
		string name;
		for (;;)
			{
			if (not sQRead(readChar, iChanR))
				break;

			if (readChar == '=')
				{
				sUnread(readChar, iChanU);
				break;
				}
			name.append(1, readChar);
			}

		if (not sTryReadChar(iChanR, iChanU, '='))
			break;

		string value;
		for (;;)
			{
			if (not sQRead(readChar, iChanR))
				break;

			if (readChar == '&')
				{
				sUnread(readChar, iChanU);
				break;
				}
			value.append(1, readChar);
			}
		oTuple.Set(name, value);
		if (not sTryReadChar(iChanR, iChanU, '&'))
			break;
		}
	return true;
	}

bool sDecodeComponent(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string& oComponent)
	{
	bool gotAny = false;
	for (;;)
		{
		byte readChar;
		if (not sQRead(readChar, iChanR))
			break;
		gotAny = true;
		if (readChar == '/')
			break;
		if (readChar == '%')
			{
			string decodedChars;
			if (not sQReadDecodedChars(iChanR, iChanU, oComponent))
				break;
			}
		else
			{
			oComponent += readChar;
			}
		}
	return gotAny;
	}

ZTrail sDecodeTrail(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU)
	{
	ZTrail result;
	for (;;)
		{
		string component;
		if (not sDecodeComponent(iChanR, iChanU, component))
			break;
		if (component.empty())
			continue;
		if (component[0] == '.')
			{
			if (component.size() == 1)
				continue;
			if (component[1] == '.')
				{
				if (component.size() == 2)
					component.clear();
				}
			}
		result.AppendComp(component);
		}
	return result;
	}

ZTrail sDecodeTrail(const string& iURL)
	{
	ChanRPos_Bin_string theChan(iURL);
	return sDecodeTrail(theChan, theChan);
	}

string sEncodeComponent(const string& iString)
	{
	static const char spValidChars[] =
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static const size_t spValidCharsLen = sizeof(spValidChars) - 1;

	string result;
	// The result is going to be at least as long as the source
	result.reserve(iString.size());

	string::size_type lastGood = 0;
	for (;;)
		{
		string::size_type nextProb =
			iString.find_first_not_of(spValidChars, lastGood, spValidCharsLen);

		if (nextProb == string::npos)
			{
			if (iString.size() > lastGood)
				result.append(iString, lastGood, iString.size() - lastGood);
			break;
			}
		if (nextProb > lastGood)
			result.append(iString, lastGood, nextProb - lastGood);
		lastGood = nextProb + 1;

		result += sStringf("%%%02hhX", iString[nextProb]);
		}
	return result;
	}

string sEncodeTrail(const ZTrail& iTrail)
	{
	string result;
	for (size_t xx = 0; xx < iTrail.Count(); ++xx)
		{
		result += '/';
		result += sEncodeComponent(iTrail.At(xx));
		}
	return result;
	}

string sGetString0(const Val& iVal)
	{
	if (ZQ<string> result = iVal.QGet<string>())
		return result.Get();

	const Seq& theSeq = iVal.Get<Seq>();
	if (theSeq.Count())
		return theSeq.Get<string>(0);

	return string();
	}

#if 0
static ZRef<ZStreamerR> spMakeStreamer_Transfer(
	const Map& iHeader, ZRef<ZStreamerR> iStreamerR)
	{
	// According to the spec, if content is chunked, content-length must be ignored.
	// I've seen some pages being returned with transfer-encoding "chunked, chunked", which
	// is either a mistake, or is nested chunking. I'm assuming the former for now.

	if (ZUtil_string::sContainsi("chunked", sGetString0(iHeader.Get("transfer-encoding"))))
		return new ZStreamerR_FT<ChanR_Bin_Chunked>(iChanR, iChanUerR);

	if (ZQ<int64> contentLength = iHeader.QGet<int64>("content-length"))
		return new ZStreamerR_FT<ZStreamR_Limited>(contentLength.Get(), iStreamerR);

	return iStreamerR;
	}

ZRef<ZStreamerR> sMakeContentStreamer(const Map& iHeader, ZRef<ZStreamerR> iStreamerR)
	{
	iStreamerR = spMakeStreamer_Transfer(iHeader, iStreamerR);

	// We could/should look for gzip Content-Encoding, and wrap a decoding filter around it.

	return iStreamerR;
	}
#endif

// =================================================================================================
// MARK: - HTTP, request headers

/// Extracts type and subtype into properties named 'type' and 'subtype' of ioFields.
/**
For each type/subtype entry, add a tuple containing string properties named
"type" and "subtype". If there are parameters for any they are added to the tuple
yadda yadda yadda

<code>
// This is pseudo-tuple source
{
type = string(); // the type eg 'text'
subtype = string(); // the subtype
parameters = {
  parameterName1 = string(parameterValue1);
  parameterName2 = string(parameterValue2);
  // ...
  parameterNameX = string(parameterValueX);
  }
}
<\endcode>
*/
bool sQRead_accept(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	for (;;)
		{
		Map parameters;
		string type, subtype;
		if (not sQReadMediaType(iChanR, iChanU, &type, &subtype, &parameters, nullptr, nullptr))
			break;
		Map temp;
		temp.Set("type", type);
		temp.Set("subtype", subtype);
		if (not parameters.IsEmpty())
			temp.Set("parameters", parameters);
		if (ioFields)
			spAppend(*ioFields, "accept", temp);

		sSkipLWS(iChanR, iChanU);

		if (not sTryReadChar(iChanR, iChanU, ','))
			break;
		}
	return true;
	}

//bool sQRead_accept_charset(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_accept_encoding(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

bool sQRead_accept_language(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	for (;;)
		{
		sSkipLWS(iChanR, iChanU);

		string languageTag;
		if (not sQReadLanguageTag(iChanR, iChanU, &languageTag))
			break;

		Map temp;
		temp.Set("tag", languageTag);

		Map parameters;
		for (;;)
			{
			sSkipLWS(iChanR, iChanU);

			if (not sTryReadChar(iChanR, iChanU, ';'))
				break;

			string name, value;
			if (not sQReadParameter(iChanR, iChanU, &name, &value, nullptr))
				break;
			parameters.Set(name, value);
			}

		if (not parameters.IsEmpty())
			temp.Set("parameters", parameters);

		if (ioFields)
			spAppend(*ioFields, "accept-language", temp);

		sSkipLWS(iChanR, iChanU);

		if (not sTryReadChar(iChanR, iChanU, ','))
			break;
		}
	return true;
	}

//bool sQRead_authorization(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_from(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_host(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

bool sQRead_range(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	Map theRange;
	if (not sQRead_range(iChanR, iChanU, theRange))
		return false;

	if (ioFields)
		ioFields->Set("range", theRange);

	return true;
	}

/*
There are three basic forms for a range request, which
we encode as a tuple with a form as follows:
bytes=x-y	{ begin = int64(x); end = int64(y); } // (x to y inclusive)
bytes=x-	{ begin = int64(x); } // (x to the end)
bytes=-y	{ last = int64(y); } // (last y)
*/
bool sQRead_range(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map& oRange)
	{
	sSkipLWS(iChanR, iChanU);

	if (not sQReadChars(iChanR, "bytes"))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not sTryReadChar(iChanR, iChanU, '='))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (sTryReadChar(iChanR, iChanU, '-'))
		{
		sSkipLWS(iChanR, iChanU);

		int64 lastBytes;
		if (not spQReadInt64(iChanR, iChanU, &lastBytes))
			return false;
		oRange.Set("last", lastBytes);
		return true;
		}
	else
		{
		sSkipLWS(iChanR, iChanU);

		int64 begin;
		if (not spQReadInt64(iChanR, iChanU, &begin))
			return false;

		sSkipLWS(iChanR, iChanU);

		oRange.Set("begin", begin);

		if (not sTryReadChar(iChanR, iChanU, '-'))
			return false;

		sSkipLWS(iChanR, iChanU);

		int64 end;
		if (spQReadInt64(iChanR, iChanU, &end))
			oRange.Set("end", end);
		return true;
		}
	}

//bool sQRead_referer(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

// =================================================================================================
// MARK: - HTTP, response headers

bool sQRead_www_authenticate(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

// =================================================================================================
// MARK: - HTTP, request or response headers

bool sQRead_transfer_encoding(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	string encoding;
	if (not sQRead_transfer_encoding(iChanR, iChanU, encoding))
		return false;

	if (ioFields)
		ioFields->Set("transfer-encoding", encoding);

	return true;
	}

bool sQRead_transfer_encoding(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string& oEncoding)
	{
	sSkipLWS(iChanR, iChanU);

	if (not sQReadToken(iChanR, iChanU, &oEncoding, nullptr))
		return false;

	return true;
	}

// =================================================================================================
// MARK: - HTTP, entity headers

bool sQRead_content_disposition(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	Map dispositionTuple;
	if (not sQRead_content_disposition(iChanR, iChanU, dispositionTuple))
		return false;

	if (ioFields)
		ioFields->Set("content-disposition", dispositionTuple);
	return true;
	}

bool sQRead_content_disposition(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map& oTuple)
	{
	sSkipLWS(iChanR, iChanU);

	string disposition;
	if (sQReadToken(iChanR, iChanU, &disposition, nullptr))
		{
		Map dispositionTuple;
		oTuple.Set("value", disposition);

		Map parameters;
		for (;;)
			{
			sSkipLWS(iChanR, iChanU);

			if (not sTryReadChar(iChanR, iChanU, ';'))
				break;

			string name, value;
			if (not sQReadParameter(iChanR, iChanU, &name, &value, nullptr))
				break;
			parameters.Set(name, value);
			}

		sSkipLWS(iChanR, iChanU);

		if (not parameters.IsEmpty())
			oTuple.Set("parameters", parameters);
		return true;
		}
	return false;
	}

//bool sQRead_content_encoding(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_content_language(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

bool sQRead_content_length(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	int64 theLength;
	if (sQRead_content_length(iChanR, iChanU, theLength))
		{
		if (ioFields)
			ioFields->Set("content-length", theLength);
		return true;
		}
	return false;
	}

bool sQRead_content_length(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, int64& oLength)
	{
	sSkipLWS(iChanR, iChanU);
	return spQReadInt64(iChanR, iChanU, &oLength);
	}

bool sQRead_content_location(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
bool sQRead_content_md5(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

bool sQRead_content_range(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	int64 begin, end, maxLength;
	if (not sQRead_content_range(iChanR, iChanU, begin, end, maxLength))
		return false;

	if (ioFields)
		{
		Map temp;
		temp.Set("begin", begin);
		temp.Set("end", end);
		temp.Set("maxlength", maxLength);
		ioFields->Set("content-range", temp);
		}

	return true;
	}

bool sQRead_content_range(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	int64& oBegin, int64& oEnd, int64& oMaxLength)
	{
	sSkipLWS(iChanR, iChanU);

	if (not sQReadChars(iChanR, "bytes"))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not spQReadInt64(iChanR, iChanU, &oBegin))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not sTryReadChar(iChanR, iChanU, '-'))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not spQReadInt64(iChanR, iChanU, &oEnd))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not sTryReadChar(iChanR, iChanU, '/'))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not spQReadInt64(iChanR, iChanU, &oMaxLength))
		return false;

	return true;
	}

bool sQRead_content_type(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields)
	{
	string type, subType;
	Map parameters;
	if (not sQRead_content_type(iChanR, iChanU, type, subType, parameters))
		return false;

	if (ioFields)
		{
		Map temp;
		temp.Set("type", type);
		temp.Set("subtype", subType);
		if (not parameters.IsEmpty())
			temp.Set("parameters", parameters);
		ioFields->Set("content-type", temp);
		}
	return true;
	}

bool sQRead_content_type(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string& oType, string& oSubType, Map& oParameters)
	{
	if (not sQReadMediaType(iChanR, iChanU, &oType, &oSubType, &oParameters, nullptr, nullptr))
		return false;
	return true;
	}

// =================================================================================================

bool sQReadHTTPVersion(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, int32* oVersionMajor, int32* oVersionMinor)
	{
	if (not sQReadChars(iChanR, "HTTP/"))
		return false;

	if (not spQReadInt32(iChanR, iChanU, oVersionMajor))
		return false;

	if (not sTryReadChar(iChanR, iChanU, '.'))
		return false;

	if (not spQReadInt32(iChanR, iChanU, oVersionMinor))
		return false;
	return true;
	}

bool sQReadURI(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string* oURI)
	{
	if (oURI)
		oURI->resize(0);

	for (;;)
		{
		byte readChar;
		if (not sQRead(readChar, iChanR))
			break;

		if (sIs_LWS(readChar))
			{
			sUnread(readChar, iChanU);
			break;
			}

		if (oURI)
			oURI->append(1, readChar);
		}
	return true;
	}

bool sQReadFieldName(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string* oName, string* oNameExact)
	{
	if (oName)
		oName->resize(0);
	if (oNameExact)
		oNameExact->resize(0);

	sSkipLWS(iChanR, iChanU);

	if (not sQReadToken(iChanR, iChanU, oName, oNameExact))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not sTryReadChar(iChanR, iChanU, ':'))
		return false;

	return true;
	}

bool sQReadParameter(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oName, string* oValue, string* oNameExact)
	{
	if (oName)
		oName->resize(0);
	if (oValue)
		oValue->resize(0);
	if (oNameExact)
		oNameExact->resize(0);

	sSkipLWS(iChanR, iChanU);

	if (not sQReadToken(iChanR, iChanU, oName, oNameExact))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not sTryReadChar(iChanR, iChanU, '='))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (sQReadToken(iChanR, iChanU, nullptr, oValue))
		return true;
	else if (sQReadQuotedString(iChanR, iChanU, nullptr, oValue))
		return true;

	return false;
	}

bool sQReadParameter_Cookie(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oName, string* oValue, string* oNameExact)
	{
	if (oName)
		oName->resize(0);
	if (oValue)
		oValue->resize(0);
	if (oNameExact)
		oNameExact->resize(0);

	sSkipLWS(iChanR, iChanU);

	if (not sQReadToken(iChanR, iChanU, oName, oNameExact))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not sTryReadChar(iChanR, iChanU, '='))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (sQReadToken_Cookie(iChanR, iChanU, nullptr, oValue))
		return true;
	else if (sQReadQuotedString(iChanR, iChanU, nullptr, oValue))
		return true;

	return false;
	}

bool sQReadMediaType(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oType, string* oSubtype, Map* oParameters,
	string* oTypeExact, string* oSubtypeExact)
	{
	if (oType)
		oType->resize(0);
	if (oSubtype)
		oSubtype->resize(0);
	if (oTypeExact)
		oTypeExact->resize(0);
	if (oSubtypeExact)
		oSubtypeExact->resize(0);
	if (oParameters)
		oParameters->Clear();

	sSkipLWS(iChanR, iChanU);

	if (not sQReadToken(iChanR, iChanU, oType, oTypeExact))
		return false;

	sSkipLWS(iChanR, iChanU);

	if (not sTryReadChar(iChanR, iChanU, '/'))
		return true;

	sSkipLWS(iChanR, iChanU);

	if (not sQReadToken(iChanR, iChanU, oSubtype, oSubtypeExact))
		return true;

	for (;;)
		{
		sSkipLWS(iChanR, iChanU);

		if (not sTryReadChar(iChanR, iChanU, ';'))
			break;

		string name, value;
		if (not sQReadParameter(iChanR, iChanU, &name, &value, nullptr))
			break;
		if (oParameters)
			oParameters->Set(name, value);
		}

	return true;
	}

bool sQReadLanguageTag(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string* oLanguageTag)
	{
	if (oLanguageTag)
		oLanguageTag->resize(0);

	byte readChar;
	if (not sQRead(readChar, iChanR))
		return false;

	if (not sIs_ALPHA(readChar))
		{
		sUnread(readChar, iChanU);
		return false;
		}

	if (oLanguageTag)
		oLanguageTag->append(1, readChar);

	for (;;)
		{
		if (not sQRead(readChar, iChanR))
			return true;

		if (not sIs_ALPHA(readChar) && readChar != '-')
			{
			sUnread(readChar, iChanU);
			return true;
			}

		if (oLanguageTag)
			oLanguageTag->append(1, readChar);
		}
	}

// =================================================================================================
// MARK: - HTTP, Lower level parsing

bool sParseURL(const string& iURL,
	string* oScheme, string* oHost, uint16* oPort, string* oPath)
	{
	if (oScheme)
		oScheme->clear();
	if (oHost)
		oHost->clear();
	if (oPort)
		*oPort = 0;
	if (oPath)
		oPath->clear();

	size_t start = 0;
	const char schemeDivider[] = "://";
	const size_t dividerOffset = iURL.find(schemeDivider);
	if (string::npos != dividerOffset)
		{
		start = dividerOffset + std::strlen(schemeDivider);
		if (oScheme)
			*oScheme = iURL.substr(0, dividerOffset);
		}

	string hostAndPort;
	size_t slashOffset = iURL.find('/', start);
	if (string::npos != slashOffset)
		{
		if (start == 0)
			slashOffset = 0;
		hostAndPort = iURL.substr(start, slashOffset - start);
		if (oPath)
			*oPath = iURL.substr(min(iURL.size(), slashOffset));
		start = slashOffset;
		}
	else
		{
		hostAndPort = iURL.substr(start);
		if (oPath)
			*oPath = "/";
		}

	const size_t colonOffset = hostAndPort.find(':');
	if (string::npos != colonOffset)
		{
		if (ZQ<int64> theQ = ZUtil_string::sQInt64(hostAndPort.substr(colonOffset + 1)))
			{
			if (oPort)
				*oPort = *theQ;
			if (oHost)
				*oHost = hostAndPort.substr(0, colonOffset);
			}
		else
			{
			if (oHost)
				*oHost = hostAndPort;
			}
		}
	else
		{
		if (oHost)
			*oHost = hostAndPort;
		}

	return true;
	}

bool sQReadToken(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oTokenLC, string* oTokenExact)
	{
	if (oTokenLC)
		oTokenLC->resize(0);
	if (oTokenExact)
		oTokenExact->resize(0);

	bool gotAny = false;

	for (;;)
		{
		byte readChar;
		if (not sQRead(readChar, iChanR))
			break;

		if (not sIs_token(readChar))
			{
			sUnread(readChar, iChanU);
			break;
			}

		if (readChar == '%')
			{
			string decodedChars;
			if (not sQReadDecodedChars(iChanR, iChanU, decodedChars))
				break;

			gotAny = true;

			if (oTokenLC)
				oTokenLC->append(decodedChars);
			if (oTokenExact)
				oTokenExact->append(decodedChars);
			}
		else
			{
			gotAny = true;

			if (oTokenLC)
				oTokenLC->append(1, char(tolower(readChar)));
			if (oTokenExact)
				oTokenExact->append(1, readChar);
			}
		}

	return gotAny;
	}

bool sQReadToken_Cookie(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string* oTokenLC, string* oTokenExact)
	{
	if (oTokenLC)
		oTokenLC->resize(0);
	if (oTokenExact)
		oTokenExact->resize(0);

	bool gotAny = false;

	for (;;)
		{
		byte readChar;
		if (not sQRead(readChar, iChanR))
			break;

		if (not sIs_token(readChar))
			{
			// Workaround for Google Analytics' illegal use of '(', ')'
			// and '=' in its cookie values (see RFC 2109, section 4.1
			// and RFC 2068, section 2.2)
			if (readChar != '('
				&& readChar != ')'
				&& readChar != '=')
				{
				sUnread(readChar, iChanU);
				break;
				}
			}

		if (readChar == '%')
			{
			string decodedChars;
			if (not sQReadDecodedChars(iChanR, iChanU, decodedChars))
				break;

			gotAny = true;

			if (oTokenLC)
				oTokenLC->append(decodedChars);
			if (oTokenExact)
				oTokenExact->append(decodedChars);
			}
		else
			{
			gotAny = true;

			if (oTokenLC)
				oTokenLC->append(1, char(tolower(readChar)));
			if (oTokenExact)
				oTokenExact->append(1, readChar);
			}
		}

	return gotAny;
	}

bool sQReadQuotedString(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oString, string* oStringExact)
	{
	if (oString)
		oString->resize(0);
	if (oStringExact)
		oStringExact->resize(0);

	if (not sTryReadChar(iChanR, iChanU, '"'))
		return false;

	for (;;)
		{
		byte readChar;
		if (not sQRead(readChar, iChanR))
			break;

		if (not sIs_qdtext(readChar))
			{
			sUnread(readChar, iChanU);
			break;
			}

		if (oString)
			oString->append(1, char(tolower(readChar)));
		if (oStringExact)
			oStringExact->append(1, readChar);
		}

	if (not sTryReadChar(iChanR, iChanU, '"'))
		return false;

	return true;
	}

bool sTryReadChar(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, char iChar)
	{
	byte readChar;
	if (not sQRead(readChar, iChanR))
		return false;

	if (readChar != iChar)
		{
		sUnread(readChar, iChanU);
		return false;
		}

	return true;
	}

bool sQReadChars(const ChanR_Bin& iChanR, const char* iString)
	{
	while (*iString)
		{
		byte readChar;
		if (not sQRead(readChar, iChanR))
			return false;
		if (*iString != readChar)
			return false;
		++iString;
		}
	return true;
	}

void sSkipLWS(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU)
	{
	for (;;)
		{
		byte readChar;
		if (not sQRead(readChar, iChanR))
			break;

		if (not sIs_LWS(readChar))
			{
			sUnread(readChar, iChanU);
			break;
			}
		}
	}

bool sQReadDecodedChars(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string& ioString)
	{
	byte readChar;
	if (not sQRead(readChar, iChanR))
		return false;

	if (not isxdigit(readChar))
		{
		sUnread(readChar, iChanU);
		ioString.append(1, '%');
		}
	else
		{
		byte readChar2;
		if (not sQRead(readChar2, iChanR))
			return false;

		if (not isxdigit(readChar2))
			{
			sUnread(readChar2, iChanU);

			ioString.append(1, '%');
			ioString.append(1, readChar);
			}
		else
			{
			ioString.append(1, char((spHexCharToUInt(readChar) << 4) + spHexCharToUInt(readChar2)));
			}
		}
	return true;
	}

// =================================================================================================
// MARK: - HTTP, Lexical classification

bool sIs_CHAR(char iChar)
	{
	// The following line:
	// return iChar >= 0 && iChar <= 127;
	// generates the following warning under gcc:
	// "comparison is always true due to limited range of data type"
	// and I'm sick of seeing it. So we check for
	// a set top bit, which is a valid check regardless of whether
	// char is a signed type or not.
	return 0 == (iChar & 0x80);
	}

bool sIs_UPALPHA(char iChar)
	{ return iChar >= 'A' && iChar <= 'Z'; }

bool sIs_LOALPHA(char iChar)
	{ return iChar >= 'a' && iChar <= 'z'; }

bool sIs_ALPHA(char iChar)
	{ return sIs_LOALPHA(iChar) || sIs_UPALPHA(iChar); }

bool sIs_DIGIT(char iChar)
	{ return iChar >= '0' && iChar <= '9'; }

bool sIs_CTL(char iChar)
	{
	if (iChar >= 0 && iChar <= 31)
		return true;
	if (iChar == 127)
		return true;
	return false;
	}

bool sIs_CR(char iChar)
	{ return iChar == '\r'; }

bool sIs_LF(char iChar)
	{ return iChar == '\n'; }

bool sIs_SP(char iChar)
	{ return iChar == ' '; }

bool sIs_HT(char iChar)
	{ return iChar == '\t'; }

bool sIs_QUOTE(char iChar)
	{ return iChar == '\"'; }

bool sIs_LWS(char iChar)
	{
	return sIs_HT(iChar) || sIs_SP(iChar);
	}

bool sIs_TEXT(char iChar)
	{ return !sIs_CTL(iChar); }

bool sIs_HEX(char iChar)
	{
	if (sIs_DIGIT(iChar))
		return true;
	if (iChar >= 'A' && iChar <= 'F')
		return true;
	if (iChar >= 'a' && iChar <= 'f')
		return true;
	return false;
	}

bool sIs_tspecials(char iChar)
	{
	switch (iChar)
		{
		case '(': case ')': case '<': case '>': case '@':
		case ',': case ';': case ':': case '\\': case '\"':
		case '/': case '[': case ']': case '?': case '=':
		case '{': case '}': case ' ': case '\t':
			return true;
		}
	return false;
	}

bool sIs_token(char iChar)
	{
	return !sIs_CTL(iChar) && !sIs_tspecials(iChar);
	}

bool sIs_ctext(char iChar)
	{
	return sIs_TEXT(iChar) && iChar != '(' && iChar != ')';
	}

bool sIs_qdtext(char iChar)
	{
	return sIs_TEXT(iChar) && iChar != '\"';
	}

// =================================================================================================
// MARK: - Writing

void sWrite_HeaderLine(const string& iName, const string& iBody, const ChanW_Bin& iChanW)
	{
	sWrite(iName, iChanW);
	sWrite(": ", iChanW);
	sWrite(iBody, iChanW);
	sWrite("\r\n", iChanW);
	}

void sWrite_Header(const Map& iHeader, const ChanW_Bin& iChanW)
	{
	for (Map::Index_t ii = iHeader.Begin(); ii != iHeader.End(); ++ii)
		{
		const string name = iHeader.NameOf(ii);
		const Val& theVal = *iHeader.PGet(ii);

		if (ZQ<Seq> asSeqQ = theVal.QGet<Seq>())
			{
			const Seq asSeq = *asSeqQ;
			for (size_t xx = 0, count = asSeq.Count(); xx < count; ++xx)
				{
				if (ZQ<string> bodyQ = asSeq.QGet<string>(xx))
					sWrite_HeaderLine(name, *bodyQ, iChanW);
				}
			}
		else if (ZQ<string> asStringQ = theVal.QGet<string>())
			{
			sWrite_HeaderLine(name, *asStringQ, iChanW);
			}
		}
	}

void sWrite_MinimalResponse(int iResult, const ChanW_Bin& iChanW)
	{ sWritef(iChanW, "HTTP/1.1 %d OK\r\n\r\n", iResult); }

void sWrite_MinimalResponse_ErrorInBody(int iError, const ChanW_Bin& iChanW)
	{
	sWritef(iChanW, "HTTP/1.1 %d ERROR\r\n", iError);
	sWrite("Content-Type: text/plain\r\n", iChanW);
	sWrite("\r\n", iChanW);
	sWritef(iChanW, "Error %d", iError);
	}

// =================================================================================================
// MARK: - HTTP::ChanR_Bin_Chunked

static uint64 spReadChunkSize(const ChanR_Bin& iChanR)
	{
	uint64 result = 0;
	for (;;)
		{
		byte theChar;
		if (not sQRead(theChar, iChanR))
			return 0;

		int theXDigit = 0;
		if (theChar >= '0' && theChar <= '9')
			{
			theXDigit = theChar - '0';
			}
		else if (theChar >= 'a' && theChar <= 'f')
			{
			theXDigit = theChar - 'a' + 10;
			}
		else if (theChar >= 'A' && theChar <= 'F')
			{
			theXDigit = theChar - 'A' + 10;
			}
		else
			{
			if (theChar != '\n')
				{
				// Skip everything till we hit a LF
				sSkip_Until<byte>(iChanR, byte('\n'));
				}
			break;
			}
		result = (result << 4) + theXDigit;
		}

	return result;
	}

ChanR_Bin_Chunked::ChanR_Bin_Chunked(const ChanR_Bin& iChanR)
:	fChanR(iChanR)
	{
	fChunkSize = spReadChunkSize(fChanR);
	fHitEnd = fChunkSize == 0;
	}

ChanR_Bin_Chunked::~ChanR_Bin_Chunked()
	{}

size_t ChanR_Bin_Chunked::Read(byte* oDest, size_t iCount)
	{
	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	while (iCount && !fHitEnd)
		{
		if (fChunkSize == 0)
			{
			// Read and discard the CRLF at the end of the chunk.
			uint64 countSkipped = sSkip(2, fChanR);
			if (countSkipped == 2)
				fChunkSize = spReadChunkSize(fChanR);
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			size_t countRead = sRead(
				localDest, std::min<size_t>(iCount, fChunkSize), fChanR);

			if (countRead == 0)
				fHitEnd = true;
			localDest += countRead;
			iCount -= countRead;
			fChunkSize -= countRead;
			}
		}
	return localDest - reinterpret_cast<uint8*>(oDest);
	}

size_t ChanR_Bin_Chunked::Readable()
	{ return min<size_t>(fChunkSize, sReadable(fChanR)); }

//bool ChanR_Bin_Chunked::Imp_WaitReadable(double iTimeout)
//	{ return fStreamSource.WaitReadable(iTimeout); }

// =================================================================================================
// MARK: - HTTP::ChanW_Bin_Chunked

ChanW_Bin_Chunked::ChanW_Bin_Chunked(size_t iBufferSize, const ChanW_Bin& iChanW)
:	fChanW(iChanW),
	fBuffer(max(size_t(64), iBufferSize), 0),
	fBufferUsed(0)
	{}

ChanW_Bin_Chunked::ChanW_Bin_Chunked(const ChanW_Bin& iChanW)
:	fChanW(iChanW),
	fBuffer(1024, 0),
	fBufferUsed(0)
	{}

ChanW_Bin_Chunked::~ChanW_Bin_Chunked()
	{
	try
		{
		this->pFlush();

		// Terminating zero-length chunk
		sWrite("0\r\n", fChanW);

		// There's supposed to be an additional CRLF at the end of all the data,
		// after any trailer entity headers.
		sWrite("\r\n", fChanW);
		}
	catch (...)
		{}
	}

size_t ChanW_Bin_Chunked::Write(const byte* iSource, size_t iCount)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	while (iCount)
		{
		if (fBufferUsed + iCount >= fBuffer.size())
			{
			// The data would overflow the buffer, so we can write the
			// buffer content (if any) plus this new stuff.
			sWritef(fChanW, "%X\r\n", fBufferUsed + iCount);
			// Hmmm. Do we allow an end of stream exception to propogate?
			sWrite(&fBuffer[0], fBufferUsed, fChanW);
			fBufferUsed = 0;
			sWrite(localSource, iCount, fChanW);
			sWrite("\r\n", fChanW);
			localSource += iCount;
			iCount = 0;
			}
		else
			{
			size_t countToCopy = min(iCount, size_t(fBuffer.size() - fBufferUsed));
			sMemCopy(&fBuffer[0] + fBufferUsed, localSource, countToCopy);
			fBufferUsed += countToCopy;
			iCount -= countToCopy;
			localSource += countToCopy;
			}
		}
	return localSource - reinterpret_cast<const uint8*>(iSource);
	}

void ChanW_Bin_Chunked::Flush()
	{
	this->pFlush();
	sFlush(fChanW);
	}

void ChanW_Bin_Chunked::pFlush()
	{
	if (const size_t bufferUsed = fBufferUsed)
		{
		fBufferUsed = 0;
		sWritef(fChanW, "%X\r\n", bufferUsed);
		sWrite(&fBuffer[0], bufferUsed, fChanW);
		sWrite("\r\n", fChanW);
		}
	}

} // namespace HTTP
} // namespace ZooLib
