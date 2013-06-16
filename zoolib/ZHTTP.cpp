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

#include "zoolib/ZHTTP.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZMIME.h"
#include "zoolib/ZStreamR_Boundary.h"
#include "zoolib/ZStream_Limited.h"
#include "zoolib/ZStream_String.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_string.h"

#include <ctype.h>

namespace ZooLib {
namespace ZHTTP {

using std::max;
using std::min;
using std::pair;
using std::replace;
using std::string;
using std::vector;

static const char CR = '\r';
static const char LF = '\n';

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

static bool spReadDigit(const ZStreamU& iStream, int& oDigit)
	{
	char readChar;
	if (not iStream.ReadChar(readChar))
		return false;

	if (readChar < '0' || readChar > '9')
		{
		iStream.Unread();
		return false;
		}

	oDigit = readChar - '0';
	return true;
	}

static bool spReadInt32(const ZStreamU& iStream, int32* oInt32)
	{
	if (oInt32)
		*oInt32 = 0;

	int digit;
	if (not spReadDigit(iStream, digit))
		return false;
	for (;;)
		{
		if (oInt32)
			{
			*oInt32 *= 10;
			*oInt32 += digit;
			}
		if (not spReadDigit(iStream, digit))
			break;
		}
	return true;
	}

static bool spReadInt64(const ZStreamU& iStream, int64* oInt64)
	{
	if (oInt64)
		*oInt64 = 0;

	int digit;
	if (not spReadDigit(iStream, digit))
		return false;
	for (;;)
		{
		if (oInt64)
			{
			*oInt64 *= 10;
			*oInt64 += digit;
			}
		if (not spReadDigit(iStream, digit))
			break;
		}
	return true;
	}

// =================================================================================================
// MARK: - ZHTTP::Response

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

void Response::Send(const ZStreamW& s) const
	{
	ZAssert(fResult >= 100 && fResult <= 999);
	if (fIsVersion11)
		s.WriteString("HTTP/1.1 ");
	else
		s.WriteString("HTTP/1.0 ");
	s.Writef("%d", fResult);
	if (not fMessage.empty())
		{
		s.WriteString(" ");
		s.WriteString(fMessage);
		}

	s.WriteString("\r\n");

	for (vector<pair<string, string> >::const_iterator ii = fHeaders.begin();
		ii != fHeaders.end(); ++ii)
		{ sWrite_HeaderLine(s, ii->first, ii->second); }

	s.WriteString("\r\n");
	}

// =================================================================================================
// MARK: - ZHTTP, high level parsing

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

bool sReadRequest(const ZStreamR& iStreamR, string* oMethod, string* oURL, string* oError)
	{
	ZMIME::StreamR_Line theSIL(iStreamR);
	ZStreamU_Unreader theStreamU(theSIL);

	if (oMethod)
		oMethod->resize(0);
	if (oURL)
		oURL->resize(0);
	if (oError)
		oError->resize(0);

	if (not sReadToken(theStreamU, nullptr, oMethod))
		{
		if (oError)
			*oError = "Failed to read method";
		return false;
		}

	sSkipLWS(theStreamU);

	if (not sReadURI(theStreamU, oURL))
		{
		if (oError)
			*oError = "Failed to read URI";
		return false;
		}

	sSkipLWS(theStreamU);

	int32 major, minor;
	if (not sReadHTTPVersion(theStreamU, &major, &minor))
		{
		if (oError)
			*oError = "Failed to read version";
		return false;
		}

	theStreamU.SkipAll();
	return true;
	}

bool sReadResponse(const ZStreamU& iStream, int32* oResultCode, string* oResultMessage)
	{
	if (oResultMessage)
		oResultMessage->resize(0);

	int32 major, minor;
	if (not sReadHTTPVersion(iStream, &major, &minor))
		return false;

	sSkipLWS(iStream);

	if (not spReadInt32(iStream, oResultCode))
		return false;

	sSkipLWS(iStream);

	// Suck up the remainder of the stream as the result message.
	for (;;)
		{
		char readChar;
		if (not iStream.ReadChar(readChar))
			break;
		if (readChar == LF)
			break;
		if (oResultMessage)
			*oResultMessage += readChar;
		}

	return true;
	}

bool sReadHeaderNoParsing(const ZStreamR& iStream, Map* oFields)
	{
	if (oFields)
		oFields->Clear();

	for (;;)
		{
		ZMIME::StreamR_Line theSIL(iStream);
		ZStreamU_Unreader theStream(theSIL);
		const bool gotOne = sReadHeaderLineNoParsing(theStream, oFields);
		theStream.SkipAll();
		if (not gotOne)
			return true;
		}
	}

bool sReadHeaderLineNoParsing(const ZStreamU& iStream, Map* ioFields)
	{
	string fieldNameExact;
	if (not sReadFieldName(iStream, nullptr, &fieldNameExact))
		return false;

	if (not fieldNameExact.size())
		return true;

	sSkipLWS(iStream);
	string fieldBody;
	iStream.CopyAllTo(ZStreamWPos_String(fieldBody));
	if (ioFields)
		spAppend(*ioFields, fieldNameExact, fieldBody);

	return true;
	}

bool sReadHeader(const ZStreamR& iStream, Map* oFields)
	{
	if (oFields)
		oFields->Clear();

	for (;;)
		{
		ZMIME::StreamR_Line theSIL(iStream);
		ZStreamU_Unreader theStream(theSIL);
		const bool gotOne = sReadHeaderLine(theStream, oFields);
		theStream.SkipAll();
		if (not gotOne)
			return true;
		}
	}

bool sReadHeaderLine(const ZStreamU& iStream, Map* ioFields)
	{
	string fieldName;
	if (not sReadFieldName(iStream, &fieldName, nullptr))
		return false;

	if (fieldName.empty())
		return true;

	if (false)
		{}
	// -----------------
	// Request headers
	else if (fieldName == "accept") sRead_accept(iStream, ioFields);
	else if (fieldName == "accept-charset")
		{
		for (;;)
			{
			sSkipLWS(iStream);

			string charset;
			if (not sReadToken(iStream, &charset, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "accept-charset", charset);

			sSkipLWS(iStream);

			if (not sReadChar(iStream, ','))
				break;
			}
		}
	else if (fieldName == "accept-encoding")
		{
		for (;;)
			{
			sSkipLWS(iStream);

			string encoding;
			if (not sReadToken(iStream, &encoding, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "accept-encoding", encoding);

			sSkipLWS(iStream);

			if (not sReadChar(iStream, ','))
				break;
			}
		}
	else if (fieldName == "accept-language") sRead_accept_language(iStream, ioFields);
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
	else if (fieldName == "range") sRead_range(iStream, ioFields);
//	else if (fieldName == "referer")
//	else if (fieldName == "user-agent")
	else if (fieldName == "cookie")
		{
		for (;;)
			{
			string cookieName, cookieValue;
			if (not sReadParameter_Cookie(iStream, nullptr, &cookieValue, &cookieName))
				break;

			if (ioFields)
				{
				Map cookieMap = ioFields->Get("cookie").Get<Map>();
				cookieMap.Set(cookieName, cookieValue);
				ioFields->Set("cookie", cookieMap);
				}

			sSkipLWS(iStream);

			if (not sReadChar(iStream, ';'))
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
		sSkipLWS(iStream);
		string body;
		if (sReadToken(iStream, &body, nullptr))
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
			sSkipLWS(iStream);

			string encoding;
			if (not sReadToken(iStream, &encoding, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "content-encoding", encoding);

			sSkipLWS(iStream);

			if (not sReadChar(iStream, ','))
				break;
			}
		}
	else if (fieldName == "content-language")
		{
		for (;;)
			{
			sSkipLWS(iStream);

			string language;
			if (not sReadToken(iStream, &language, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "content-language", language);

			sSkipLWS(iStream);

			if (not sReadChar(iStream, ','))
				break;
			}
		}
	else if (fieldName == "content-length") sRead_content_length(iStream, ioFields);
//	else if (fieldName == "content-location")
//	else if (fieldName == "content-md5")
	else if (fieldName == "content-range") sRead_content_range(iStream, ioFields);
	else if (fieldName == "content-type") sRead_content_type(iStream, ioFields);
//	else if (fieldName == "etag")
//	else if (fieldName == "expires")
//	else if (fieldName == "last-modified")
//---------------
	else if (fieldName == "content-disposition") sRead_content_disposition(iStream, ioFields);
	else
		{
		sSkipLWS(iStream);
		string fieldBody;
		iStream.CopyAllTo(ZStreamWPos_String(fieldBody));
		if (ioFields)
			spAppend(*ioFields, fieldName, fieldBody);
		}

	return true;
	}

bool sParseQuery(const string& iString, Map& oTuple)
	{ return sParseQuery(ZStreamRPos_String(iString), oTuple); }

bool sParseQuery(const ZStreamU& iStream, Map& oTuple)
	{
	for (;;)
		{
		char readChar;
		string name;
		for (;;)
			{
			if (not iStream.ReadChar(readChar))
				break;

			if (readChar == '=')
				{
				iStream.Unread();
				break;
				}
			name.append(1, readChar);
			}

		if (not sReadChar(iStream, '='))
			break;

		string value;
		for (;;)
			{
			if (not iStream.ReadChar(readChar))
				break;

			if (readChar == '&')
				{
				iStream.Unread();
				break;
				}
			value.append(1, readChar);
			}
		oTuple.Set(name, value);
		if (not sReadChar(iStream, '&'))
			break;
		}
	return true;
	}

bool sDecodeComponent(const ZStreamU& s, string& oComponent)
	{
	bool gotAny = false;
	for (;;)
		{
		char readChar;
		if (not s.ReadChar(readChar))
			break;
		gotAny = true;
		if (readChar == '/')
			break;
		if (readChar == '%')
			{
			string decodedChars;
			if (not sReadDecodedChars(s, oComponent))
				break;
			}
		else
			{
			oComponent += readChar;
			}
		}
	return gotAny;
	}

ZTrail sDecodeTrail(const ZStreamU& s)
	{
	ZTrail result;
	for (;;)
		{
		string component;
		if (not sDecodeComponent(s, component))
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
	{ return sDecodeTrail(ZStreamRPos_String(iURL)); }

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

static ZRef<ZStreamerR> spMakeStreamer_Transfer(
	const Map& iHeader, ZRef<ZStreamerR> iStreamerR)
	{
	// According to the spec, if content is chunked, content-length must be ignored.
	// I've seen some pages being returned with transfer-encoding "chunked, chunked", which
	// is either a mistake, or is nested chunking. I'm assuming the former for now.

	if (ZUtil_string::sContainsi("chunked", sGetString0(iHeader.Get("transfer-encoding"))))
		return new ZStreamerR_FT<StreamR_Chunked>(iStreamerR);

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

// =================================================================================================
// MARK: - ZHTTP, request headers

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
bool sRead_accept(const ZStreamU& iStream, Map* ioFields)
	{
	for (;;)
		{
		Map parameters;
		string type, subtype;
		if (not sReadMediaType(iStream, &type, &subtype, &parameters, nullptr, nullptr))
			break;
		Map temp;
		temp.Set("type", type);
		temp.Set("subtype", subtype);
		if (not parameters.IsEmpty())
			temp.Set("parameters", parameters);
		if (ioFields)
			spAppend(*ioFields, "accept", temp);

		sSkipLWS(iStream);

		if (not sReadChar(iStream, ','))
			break;
		}
	return true;
	}

//bool sRead_accept_charset(const ZStreamU& iStream, Map* ioFields);
//bool sRead_accept_encoding(const ZStreamU& iStream, Map* ioFields);

bool sRead_accept_language(const ZStreamU& iStream, Map* ioFields)
	{
	for (;;)
		{
		sSkipLWS(iStream);

		string languageTag;
		if (not sReadLanguageTag(iStream, &languageTag))
			break;

		Map temp;
		temp.Set("tag", languageTag);

		Map parameters;
		for (;;)
			{
			sSkipLWS(iStream);

			if (not sReadChar(iStream, ';'))
				break;

			string name, value;
			if (not sReadParameter(iStream, &name, &value, nullptr))
				break;
			parameters.Set(name, value);
			}

		if (not parameters.IsEmpty())
			temp.Set("parameters", parameters);

		if (ioFields)
			spAppend(*ioFields, "accept-language", temp);

		sSkipLWS(iStream);

		if (not sReadChar(iStream, ','))
			break;
		}
	return true;
	}

//bool sRead_authorization(const ZStreamU& iStream, Map* ioFields);
//bool sRead_from(const ZStreamU& iStream, Map* ioFields);
//bool sRead_host(const ZStreamU& iStream, Map* ioFields);

bool sRead_range(const ZStreamU& iStream, Map* ioFields)
	{
	Map theRange;
	if (not sRead_range(iStream, theRange))
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
bool sRead_range(const ZStreamU& iStream, Map& oRange)
	{
	sSkipLWS(iStream);

	if (not sReadChars(iStream, "bytes"))
		return false;

	sSkipLWS(iStream);

	if (not sReadChar(iStream, '='))
		return false;

	sSkipLWS(iStream);

	if (sReadChar(iStream, '-'))
		{
		sSkipLWS(iStream);

		int64 lastBytes;
		if (not spReadInt64(iStream, &lastBytes))
			return false;
		oRange.Set("last", lastBytes);
		return true;
		}
	else
		{
		sSkipLWS(iStream);

		int64 begin;
		if (not spReadInt64(iStream, &begin))
			return false;

		sSkipLWS(iStream);

		oRange.Set("begin", begin);

		if (not sReadChar(iStream, '-'))
			return false;

		sSkipLWS(iStream);

		int64 end;
		if (spReadInt64(iStream, &end))
			oRange.Set("end", end);
		return true;
		}
	}

//bool sRead_referer(const ZStreamU& iStream, Map* ioFields);

// =================================================================================================
// MARK: - ZHTTP, response headers

bool sRead_www_authenticate(const ZStreamU& iStream, Map* ioFields);

// =================================================================================================
// MARK: - ZHTTP, request or response headers

bool sRead_transfer_encoding(const ZStreamU& iStream, Map* ioFields)
	{
	string encoding;
	if (not sRead_transfer_encoding(iStream, encoding))
		return false;

	if (ioFields)
		ioFields->Set("transfer-encoding", encoding);

	return true;
	}

bool sRead_transfer_encoding(const ZStreamU& iStream, string& oEncoding)
	{
	sSkipLWS(iStream);

	if (not sReadToken(iStream, &oEncoding, nullptr))
		return false;

	return true;
	}

// =================================================================================================
// MARK: - ZHTTP, entity headers

bool sRead_content_disposition(const ZStreamU& iStream, Map* ioFields)
	{
	Map dispositionTuple;
	if (not sRead_content_disposition(iStream, dispositionTuple))
		return false;

	if (ioFields)
		ioFields->Set("content-disposition", dispositionTuple);
	return true;
	}

bool sRead_content_disposition(const ZStreamU& iStream, Map& oTuple)
	{
	sSkipLWS(iStream);

	string disposition;
	if (sReadToken(iStream, &disposition, nullptr))
		{
		Map dispositionTuple;
		oTuple.Set("value", disposition);

		Map parameters;
		for (;;)
			{
			sSkipLWS(iStream);

			if (not sReadChar(iStream, ';'))
				break;

			string name, value;
			if (not sReadParameter(iStream, &name, &value, nullptr))
				break;
			parameters.Set(name, value);
			}

		sSkipLWS(iStream);

		if (not parameters.IsEmpty())
			oTuple.Set("parameters", parameters);
		return true;
		}
	return false;
	}

bool sRead_content_encoding(const ZStreamU& iStream, Map* ioFields);
bool sRead_content_language(const ZStreamU& iStream, Map* ioFields);

bool sRead_content_length(const ZStreamU& iStream, Map* ioFields)
	{
	int64 theLength;
	if (sRead_content_length(iStream, theLength))
		{
		if (ioFields)
			ioFields->Set("content-length", theLength);
		return true;
		}
	return false;
	}

bool sRead_content_length(const ZStreamU& iStream, int64& oLength)
	{
	sSkipLWS(iStream);
	return spReadInt64(iStream, &oLength);
	}

bool sRead_content_location(const ZStreamU& iStream, Map* ioFields);
bool sRead_content_md5(const ZStreamU& iStream, Map* ioFields);

bool sRead_content_range(const ZStreamU& iStream, Map* ioFields)
	{
	int64 begin, end, maxLength;
	if (not sRead_content_range(iStream, begin, end, maxLength))
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

bool sRead_content_range(const ZStreamU& iStream,
	int64& oBegin, int64& oEnd, int64& oMaxLength)
	{
	sSkipLWS(iStream);

	if (not sReadChars(iStream, "bytes"))
		return false;

	sSkipLWS(iStream);

	if (not spReadInt64(iStream, &oBegin))
		return false;

	sSkipLWS(iStream);

	if (not sReadChar(iStream, '-'))
		return false;

	sSkipLWS(iStream);

	if (not spReadInt64(iStream, &oEnd))
		return false;

	sSkipLWS(iStream);

	if (not sReadChar(iStream, '/'))
		return false;

	sSkipLWS(iStream);

	if (not spReadInt64(iStream, &oMaxLength))
		return false;

	return true;
	}

bool sRead_content_type(const ZStreamU& iStream, Map* ioFields)
	{
	string type, subType;
	Map parameters;
	if (not sRead_content_type(iStream, type, subType, parameters))
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

bool sRead_content_type(const ZStreamU& iStream,
	string& oType, string& oSubType, Map& oParameters)
	{
	if (not sReadMediaType(iStream, &oType, &oSubType, &oParameters, nullptr, nullptr))
		return false;
	return true;
	}

// =================================================================================================

bool sReadHTTPVersion(const ZStreamU& iStream, int32* oVersionMajor, int32* oVersionMinor)
	{
	if (not sReadChars(iStream, "HTTP/"))
		return false;

	if (not spReadInt32(iStream, oVersionMajor))
		return false;

	if (not sReadChar(iStream, '.'))
		return false;

	if (not spReadInt32(iStream, oVersionMinor))
		return false;
	return true;
	}

bool sReadURI(const ZStreamU& iStream, string* oURI)
	{
	if (oURI)
		oURI->resize(0);

	for (;;)
		{
		char readChar;
		if (not iStream.ReadChar(readChar))
			break;

		if (sIs_LWS(readChar))
			{
			iStream.Unread();
			break;
			}

		if (oURI)
			oURI->append(1, readChar);
		}
	return true;
	}

bool sReadFieldName(const ZStreamU& iStream, string* oName, string* oNameExact)
	{
	if (oName)
		oName->resize(0);
	if (oNameExact)
		oNameExact->resize(0);

	sSkipLWS(iStream);

	if (not sReadToken(iStream, oName, oNameExact))
		return false;

	sSkipLWS(iStream);

	if (not sReadChar(iStream, ':'))
		return false;

	return true;
	}

bool sReadParameter(const ZStreamU& iStream,
	string* oName, string* oValue, string* oNameExact)
	{
	if (oName)
		oName->resize(0);
	if (oValue)
		oValue->resize(0);
	if (oNameExact)
		oNameExact->resize(0);

	sSkipLWS(iStream);

	if (not sReadToken(iStream, oName, oNameExact))
		return false;

	sSkipLWS(iStream);

	if (not sReadChar(iStream, '='))
		return false;

	sSkipLWS(iStream);

	if (sReadToken(iStream, nullptr, oValue))
		return true;
	else if (sReadQuotedString(iStream, nullptr, oValue))
		return true;

	return false;
	}

bool sReadParameter_Cookie(const ZStreamU& iStream,
	string* oName, string* oValue, string* oNameExact)
	{
	if (oName)
		oName->resize(0);
	if (oValue)
		oValue->resize(0);
	if (oNameExact)
		oNameExact->resize(0);

	sSkipLWS(iStream);

	if (not sReadToken(iStream, oName, oNameExact))
		return false;

	sSkipLWS(iStream);

	if (not sReadChar(iStream, '='))
		return false;

	sSkipLWS(iStream);

	if (sReadToken_Cookie(iStream, nullptr, oValue))
		return true;
	else if (sReadQuotedString(iStream, nullptr, oValue))
		return true;

	return false;
	}

bool sReadMediaType(const ZStreamU& iStream,
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

	sSkipLWS(iStream);

	if (not sReadToken(iStream, oType, oTypeExact))
		return false;

	sSkipLWS(iStream);

	if (not sReadChar(iStream, '/'))
		return true;

	sSkipLWS(iStream);

	if (not sReadToken(iStream, oSubtype, oSubtypeExact))
		return true;

	for (;;)
		{
		sSkipLWS(iStream);

		if (not sReadChar(iStream, ';'))
			break;

		string name, value;
		if (not sReadParameter(iStream, &name, &value, nullptr))
			break;
		if (oParameters)
			oParameters->Set(name, value);
		}

	return true;
	}

bool sReadLanguageTag(const ZStreamU& iStream, string* oLanguageTag)
	{
	if (oLanguageTag)
		oLanguageTag->resize(0);

	char readChar;
	if (not iStream.ReadChar(readChar))
		return false;

	if (not sIs_ALPHA(readChar))
		{
		iStream.Unread();
		return false;
		}

	if (oLanguageTag)
		oLanguageTag->append(1, readChar);

	for (;;)
		{
		if (not iStream.ReadChar(readChar))
			return true;

		if (not sIs_ALPHA(readChar) && readChar != '-')
			{
			iStream.Unread();
			return true;
			}

		if (oLanguageTag)
			oLanguageTag->append(1, readChar);
		}
	}

// =================================================================================================
// MARK: - ZHTTP, Lower level parsing

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

bool sReadToken(const ZStreamU& iStream, string* oTokenLC, string* oTokenExact)
	{
	if (oTokenLC)
		oTokenLC->resize(0);
	if (oTokenExact)
		oTokenExact->resize(0);

	bool gotAny = false;

	for (;;)
		{
		char readChar;
		if (not iStream.ReadChar(readChar))
			break;

		if (not sIs_token(readChar))
			{
			iStream.Unread();
			break;
			}

		if (readChar == '%')
			{
			string decodedChars;
			if (not sReadDecodedChars(iStream, decodedChars))
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

bool sReadToken_Cookie(const ZStreamU& iStream, string* oTokenLC, string* oTokenExact)
	{
	if (oTokenLC)
		oTokenLC->resize(0);
	if (oTokenExact)
		oTokenExact->resize(0);

	bool gotAny = false;

	for (;;)
		{
		char readChar;
		if (not iStream.ReadChar(readChar))
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
				iStream.Unread();
				break;
				}
			}

		if (readChar == '%')
			{
			string decodedChars;
			if (not sReadDecodedChars(iStream, decodedChars))
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

bool sReadQuotedString(const ZStreamU& iStream, string* oString, string* oStringExact)
	{
	if (oString)
		oString->resize(0);
	if (oStringExact)
		oStringExact->resize(0);

	if (not sReadChar(iStream, '"'))
		return false;

	for (;;)
		{
		char readChar;
		if (not iStream.ReadChar(readChar))
			break;

		if (not sIs_qdtext(readChar))
			{
			iStream.Unread();
			break;
			}

		if (oString)
			oString->append(1, char(tolower(readChar)));
		if (oStringExact)
			oStringExact->append(1, readChar);
		}

	if (not sReadChar(iStream, '"'))
		return false;

	return true;
	}

bool sReadChar(const ZStreamU& iStream, char iChar)
	{
	char readChar;
	if (not iStream.ReadChar(readChar))
		return false;

	if (readChar != iChar)
		{
		iStream.Unread();
		return false;
		}

	return true;
	}

bool sReadChars(const ZStreamU& iStream, const char* iString)
	{
	while (*iString)
		{
		char readChar;
		if (not iStream.ReadChar(readChar))
			return false;
		if (*iString != readChar)
			return false;
		++iString;
		}
	return true;
	}

void sSkipLWS(const ZStreamU& iStream)
	{
	for (;;)
		{
		char readChar;
		if (not iStream.ReadChar(readChar))
			break;

		if (not sIs_LWS(readChar))
			{
			iStream.Unread();
			break;
			}
		}
	}

bool sReadDecodedChars(const ZStreamU& iStream, string& ioString)
	{
	char readChar;
	if (not iStream.ReadChar(readChar))
		return false;

	if (not isxdigit(readChar))
		{
		iStream.Unread();
		ioString.append(1, '%');
		}
	else
		{
		char readChar2;
		if (not iStream.ReadChar(readChar2))
			return false;

		if (not isxdigit(readChar2))
			{
			iStream.Unread();

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
// MARK: - ZHTTP, Lexical classification

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

void sWrite_HeaderLine(const ZStreamW& w, const string& iName, const string& iBody)
	{
	w.WriteString(iName);
	w.WriteString(": ");
	w.WriteString(iBody);
	w.WriteString("\r\n");
	}

void sWrite_Header(const ZStreamW& w, const Map& iHeader)
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
					sWrite_HeaderLine(w, name, *bodyQ);
				}
			}
		else if (ZQ<string> asStringQ = theVal.QGet<string>())
			{
			sWrite_HeaderLine(w, name, *asStringQ);
			}
		}
	}

void sWrite_MinimalResponse(const ZStreamW& w, int iResult)
	{ w.Writef("HTTP/1.1 %d OK\r\n\r\n", iResult); }

void sWrite_MinimalResponse_ErrorInBody(const ZStreamW& w, int iError)
	{
	w.Writef("HTTP/1.1 %d ERROR\r\n", iError);
	w.WriteString("Content-Type: text/plain\r\n");
	w.WriteString("\r\n");
	w.Writef("Error %d", iError);
	}

// =================================================================================================
// MARK: - ZHTTP::StreamR_Chunked

static uint64 spReadChunkSize(const ZStreamR& s)
	{
	uint64 result = 0;
	for (;;)
		{
		char theChar;
		if (not s.ReadChar(theChar))
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
				ZStreamR_Boundary("\n", s).SkipAll();
				}
			break;
			}
		result = (result << 4) + theXDigit;
		}

	return result;
	}

StreamR_Chunked::StreamR_Chunked(const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	fChunkSize = spReadChunkSize(fStreamSource);
	fHitEnd = fChunkSize == 0;
	}

StreamR_Chunked::~StreamR_Chunked()
	{}

void StreamR_Chunked::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	while (iCount && !fHitEnd)
		{
		if (fChunkSize == 0)
			{
			// Read and discard the CRLF at the end of the chunk.
			uint64 countSkipped;
			fStreamSource.Skip(2, &countSkipped);
			if (countSkipped == 2)
				fChunkSize = spReadChunkSize(fStreamSource);
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			size_t countRead;
			fStreamSource.Read(
				localDest, min(iCount, ZStream::sClampedSize(fChunkSize)), &countRead);

			if (countRead == 0)
				fHitEnd = true;
			localDest += countRead;
			iCount -= countRead;
			fChunkSize -= countRead;
			}
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(oDest);
	}

size_t StreamR_Chunked::Imp_CountReadable()
	{ return min(ZStream::sClampedSize(fChunkSize), fStreamSource.CountReadable()); }

bool StreamR_Chunked::Imp_WaitReadable(double iTimeout)
	{ return fStreamSource.WaitReadable(iTimeout); }

// =================================================================================================
// MARK: - ZHTTP::StreamW_Chunked

StreamW_Chunked::StreamW_Chunked(size_t iBufferSize, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fBuffer(max(size_t(64), iBufferSize), 0),
	fBufferUsed(0)
	{}

StreamW_Chunked::StreamW_Chunked(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fBuffer(1024, 0),
	fBufferUsed(0)
	{}

StreamW_Chunked::~StreamW_Chunked()
	{
	try
		{
		this->pFlush();

		// Terminating zero-length chunk
		fStreamSink.WriteString("0\r\n");

		// There's supposed to be an additional CRLF at the end of all the data,
		// after any trailer entity headers.
		fStreamSink.WriteString("\r\n");
		}
	catch (...)
		{}
	}

void StreamW_Chunked::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	while (iCount)
		{
		if (fBufferUsed + iCount >= fBuffer.size())
			{
			// The data would overflow the buffer, so we can write the
			// buffer content (if any) plus this new stuff.
			fStreamSink.Writef("%X\r\n", fBufferUsed + iCount);
			// Hmmm. Do we allow an end of stream exception to propogate?
			fStreamSink.Write(&fBuffer[0], fBufferUsed);
			fBufferUsed = 0;
			fStreamSink.Write(localSource, iCount);
			fStreamSink.WriteString("\r\n");
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
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const uint8*>(iSource);
	}

void StreamW_Chunked::Imp_Flush()
	{
	this->pFlush();
	fStreamSink.Flush();
	}

void StreamW_Chunked::pFlush()
	{
	if (const size_t bufferUsed = fBufferUsed)
		{
		fBufferUsed = 0;
		fStreamSink.Writef("%X\r\n", bufferUsed);
		fStreamSink.Write(&fBuffer[0], bufferUsed);
		fStreamSink.WriteString("\r\n");
		}
	}

} // namespace ZHTTP
} // namespace ZooLib
