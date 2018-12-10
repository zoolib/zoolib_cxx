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
#include "zoolib/ChanRU_XX_Unreader.h"
#include "zoolib/Coerce_Any.h"
#include "zoolib/Compat_algorithm.h"
#include "zoolib/HTTP.h"
#include "zoolib/Memory.h"
#include "zoolib/MIME.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_string.h"

#include "zoolib/ZDebug.h"

namespace ZooLib {
namespace HTTP {

using std::max;
using std::min;
using std::pair;
using std::vector;

// =================================================================================================
#pragma mark - Utility stuff

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
	return 0;
	}

static bool spQReadDigit(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, int& oDigit)
	{
	byte readChar;
	if (not sQRead(iChanR, readChar))
		return false;

	if (readChar < '0' || readChar > '9')
		{
		sUnread(iChanU, readChar);
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
#pragma mark - HTTP::Response

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
	fMessage.clear();
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
	fHeaders.push_back(pair<string, string>(iName, sStringf("%lld", (long long)iValue)));
	}

void Response::Send(const ChanW_Bin& iChanW) const
	{
	ZAssert(fResult >= 100 && fResult <= 999);
	if (fIsVersion11)
		sEWrite(iChanW, "HTTP/1.1 ");
	else
		sEWrite(iChanW, "HTTP/1.0 ");

	sEWritef(iChanW, "%d", fResult);
	if (not fMessage.empty())
		{
		sEWrite(iChanW, " ");
		sEWrite(iChanW, fMessage);
		}

	sEWrite(iChanW, "\r\n");

	for (vector<pair<string, string> >::const_iterator ii = fHeaders.begin();
		ii != fHeaders.end(); ++ii)
		{ sWrite_HeaderLine(ii->first, ii->second, iChanW); }

	sEWrite(iChanW, "\r\n");
	}

// =================================================================================================
#pragma mark - HTTP, high level parsing

// This method should look at all the range entries in iRangeParam
// and turn them into a list of ascending, non overlapping start/finish
// pairs in oRanges.
bool sOrganizeRanges(int64 iSourceSize, const Val& iRangeParam,
	vector<pair<int64,int64> >& oRanges)
	{
	const Map asMap = iRangeParam.Get<Map>();
	if (ZQ<int64> reqBeginQ = sQCoerceInt(asMap.Get("begin")))
		{
		if (*reqBeginQ <= iSourceSize)
			{
			if (ZQ<int64> reqEndQ = sQCoerceInt(asMap.Get("end")))
				{
				if (*reqEndQ < *reqBeginQ)
					return false;
				if (*reqEndQ > iSourceSize)
					return false;
				oRanges.push_back(pair<int64,int64>(*reqBeginQ, *reqEndQ + 1));
				return true;
				}
			}
		}
	else if (ZQ<int64> reqLastQ = sQCoerceInt(asMap.Get("last")))
		{
		if (*reqLastQ <= iSourceSize)
			{
			oRanges.push_back(pair<int64,int64>(iSourceSize - *reqLastQ, iSourceSize));
			return true;
			}
		}
	return false;
	}

bool sQReadRequest(const ChanR_Bin& iChanR, string* oMethod, string* oURL, string* oError)
	{
	MIME::ChanR_Bin_Line theSIL(iChanR);
	ChanRU_XX_Unreader<byte> theChanRU(theSIL);

	if (oMethod)
		oMethod->resize(0);
	if (oURL)
		oURL->resize(0);
	if (oError)
		oError->resize(0);

	if (not sQReadToken(theChanRU, theChanRU, nullptr, oMethod))
		{
		if (oError)
			*oError = "Failed to read method";
		return false;
		}

	sSkipLWS(theChanRU, theChanRU);

	if (not sQReadURI(theChanRU, theChanRU, oURL))
		{
		if (oError)
			*oError = "Failed to read URI";
		return false;
		}

	sSkipLWS(theChanRU, theChanRU);

	int32 major, minor;
	if (not sQReadHTTPVersion(theChanRU, theChanRU, &major, &minor))
		{
		if (oError)
			*oError = "Failed to read version";
		return false;
		}

	sSkipAll(theChanRU);
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
		if (not sQRead(iChanR, readChar))
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
		ChanRU_XX_Unreader<byte> theChanRU(theSIL);
		const bool gotOne = sQReadHeaderLineNoParsing(theChanRU, theChanRU, oFields);
		sSkipAll(theChanRU);
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
		ChanRU_XX_Unreader<byte> theChanRU(theSIL);
		const bool gotOne = sQReadHeaderLine(theChanRU, theChanRU, oFields);
		sSkipAll(theChanRU);
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
	else if (fieldName == "content-disposition")
		{ sQRead_content_disposition(iChanR, iChanU, ioFields); }
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
			if (not sQRead(iChanR, readChar))
				break;

			if (readChar == '=')
				{
				sUnread(iChanU, readChar);
				break;
				}
			name.append(1, readChar);
			}

		if (not sTryReadChar(iChanR, iChanU, '='))
			break;

		string value;
		for (;;)
			{
			if (not sQRead(iChanR, readChar))
				break;

			if (readChar == '&')
				{
				sUnread(iChanU, readChar);
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
		if (not sQRead(iChanR, readChar))
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

Trail sDecodeTrail(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU)
	{
	Trail result;
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

Trail sDecodeTrail(const string& iURL)
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

string sEncodeTrail(const Trail& iTrail)
	{
	string result;
	for (size_t xx = 0; xx < iTrail.Count(); ++xx)
		{
		result += '/';
		result += sEncodeComponent(iTrail.At(xx));
		}
	return result;
	}

ZQ<string> sQGetString0(const Val& iVal)
	{
	if (ZQ<string> result = iVal.QGet<string>())
		return result;
	return iVal.QGet<string>(0);
	}

string sGetString0(const Val& iVal)
	{ return sQGetString0(iVal).Get(); }

// =================================================================================================
#pragma mark - HTTP, request headers

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
</code>
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
#pragma mark - HTTP, response headers

bool sQRead_www_authenticate(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

// =================================================================================================
#pragma mark - HTTP, request or response headers

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
#pragma mark - HTTP, entity headers

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
		if (not sQRead(iChanR, readChar))
			break;

		if (sIs_LWS(readChar))
			{
			sUnread(iChanU, readChar);
			break;
			}

		if (oURI)
			oURI->append(1, readChar);
		}
	return true;
	}

bool sQReadFieldName(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oName, string* oNameExact)
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
	if (not sQRead(iChanR, readChar))
		return false;

	if (not sIs_ALPHA(readChar))
		{
		sUnread(iChanU, readChar);
		return false;
		}

	if (oLanguageTag)
		oLanguageTag->append(1, readChar);

	for (;;)
		{
		if (not sQRead(iChanR, readChar))
			return true;

		if (not sIs_ALPHA(readChar) && readChar != '-')
			{
			sUnread(iChanU, readChar);
			return true;
			}

		if (oLanguageTag)
			oLanguageTag->append(1, readChar);
		}
	}

// =================================================================================================
#pragma mark - HTTP, Lower level parsing

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
	const char schemeSeparator[] = "://";
	const size_t separatorOffset = iURL.find(schemeSeparator);
	if (string::npos != separatorOffset)
		{
		start = separatorOffset + std::strlen(schemeSeparator);
		if (oScheme)
			*oScheme = iURL.substr(0, separatorOffset);
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
		// start = slashOffset;
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
		if (ZQ<int64> theQ = Util_string::sQInt64(hostAndPort.substr(colonOffset + 1)))
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

string sAbsoluteURI(const string& iBase, const string& iOther)
	{
	string baseScheme;
	string baseHost;
	uint16 basePort;
	string basePath;
	const bool baseResult = HTTP::sParseURL(iBase, &baseScheme, &baseHost, &basePort, &basePath);
	ZAssert(baseResult);

	string otherScheme;
	string otherHost;
	uint16 otherPort;
	string otherPath;
	const bool otherResult = HTTP::sParseURL(iOther, &otherScheme, &otherHost, &otherPort, &otherPath);
	ZAssert(otherResult);

	string result;
	if (sNotEmpty(otherScheme))
		{
		result += otherScheme;
		result += "://";
		}
	else if (sNotEmpty(baseScheme))
		{
		result += baseScheme;
		result += "://";
		}

	if (sNotEmpty(otherHost))
		{
		result += otherHost;
		if (0 != otherPort)
			result += sStringf(":%d", otherPort);
		}
	else if (sNotEmpty(baseScheme))
		{
		result += baseHost;
		if (0 != basePort)
			result += sStringf(":%d", basePort);
		}

	if (sNotEmpty(otherPath) && otherPath[0] == '/')
		{
		result += otherPath;
		}
	else
		{
		if (sNotEmpty(basePath))
			{
			if (basePath[0] != '/')
				result += '/';
			}
		result += basePath;
		if (sIsEmpty(basePath) || basePath.at(basePath.size() - 1) != '/')
			result += '/';
		result += otherPath;
		}
	return result;
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
		if (not sQRead(iChanR, readChar))
			break;

		if (not sIs_token(readChar))
			{
			sUnread(iChanU, readChar);
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

bool sQReadToken_Cookie(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
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
		if (not sQRead(iChanR, readChar))
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
				sUnread(iChanU, readChar);
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
		if (not sQRead(iChanR, readChar))
			break;

		if (not sIs_qdtext(readChar))
			{
			sUnread(iChanU, readChar);
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
	if (not sQRead(iChanR, readChar))
		return false;

	if (readChar != iChar)
		{
		sUnread(iChanU, readChar);
		return false;
		}

	return true;
	}

bool sQReadChars(const ChanR_Bin& iChanR, const char* iString)
	{
	while (*iString)
		{
		byte readChar;
		if (not sQRead(iChanR, readChar))
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
		if (not sQRead(iChanR, readChar))
			break;

		if (not sIs_LWS(readChar))
			{
			sUnread(iChanU, readChar);
			break;
			}
		}
	}

bool sQReadDecodedChars(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string& ioString)
	{
	byte readChar;
	if (not sQRead(iChanR, readChar))
		return false;

	if (not isxdigit(readChar))
		{
		sUnread(iChanU, readChar);
		ioString.append(1, '%');
		}
	else
		{
		byte readChar2;
		if (not sQRead(iChanR, readChar2))
			return false;

		if (not isxdigit(readChar2))
			{
			sUnread(iChanU, readChar2);

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
#pragma mark - HTTP, Lexical classification

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
#pragma mark - Writing

void sWrite_HeaderLine(const string& iName, const string& iBody, const ChanW_Bin& iChanW)
	{
	sEWrite(iChanW, iName);
	sEWrite(iChanW, ": ");
	sEWrite(iChanW, iBody);
	sEWrite(iChanW, "\r\n");
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
	{ sEWritef(iChanW, "HTTP/1.1 %d OK\r\n\r\n", iResult); }

void sWrite_MinimalResponse_ErrorInBody(int iError, const ChanW_Bin& iChanW)
	{
	sEWritef(iChanW, "HTTP/1.1 %d ERROR\r\n", iError);
	sEWrite(iChanW, "Content-Type: text/plain\r\n");
	sEWrite(iChanW, "\r\n");
	sEWritef(iChanW, "Error %d", iError);
	}

} // namespace HTTP
} // namespace ZooLib
