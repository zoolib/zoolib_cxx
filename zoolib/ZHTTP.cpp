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
#include "zoolib/ZString.h"

#include <ctype.h>
#include <stdio.h> // For sprintf

NAMESPACE_ZOOLIB_BEGIN

using std::max;
using std::min;
using std::pair;
using std::replace;
using std::string;
using std::vector;

#define kDebug_HTTP 2

namespace ZHTTP {

static const char CR = '\r';
static const char LF = '\n';

// =================================================================================================
#pragma mark -
#pragma mark * Utility stuff

static void spAppend(Map& ioFields, const string& iName, const Val& iValue)
	{
	Seq theSeq = ioFields.Get(iName).Get_T<Seq>();
	theSeq.Append(iValue);
	ioFields.Set(iName, theSeq);
//	ioFields.Mutable(iName).MutableSeq().Append(iValue);
	}

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
	if (!iStream.ReadChar(readChar))
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
	if (!spReadDigit(iStream, digit))
		return false;
	for (;;)
		{
		if (oInt32)
			{
			*oInt32 *= 10;
			*oInt32 += digit;
			}
		if (!spReadDigit(iStream, digit))
			break;
		}
	return true;
	}

static bool spReadInt64(const ZStreamU& iStream, int64* oInt64)
	{
	if (oInt64)
		*oInt64 = 0;

	int digit;
	if (!spReadDigit(iStream, digit))
		return false;
	for (;;)
		{
		if (oInt64)
			{
			*oInt64 *= 10;
			*oInt64 += digit;
			}
		if (!spReadDigit(iStream, digit))
			break;
		}
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP::Response

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
	fHeaders.push_back(pair<string, string>(iName, ZString::sFormat("%d", iValue)));
	}

void Response::Set(const string& iName, uint64 iValue)
	{
	fHeaders.push_back(pair<string, string>(iName, ZString::sFormat("%lld", iValue)));
	}

void Response::Send(const ZStreamW& s) const
	{
	ZAssert(fResult >= 100 && fResult <= 999);
	if (fIsVersion11)
		s.WriteString("HTTP/1.1 ");
	else
		s.WriteString("HTTP/1.0 ");
	s.WriteString(ZString::sFromInt(fResult));
	if (!fMessage.empty())
		{
		s.WriteString(" ");
		s.WriteString(fMessage);
		}
	s.WriteString("\r\n");
	for (vector<pair<string, string> >::const_iterator i = fHeaders.begin();
		i != fHeaders.end(); ++i)
		{
		s.WriteString((*i).first);
		s.WriteString(": ");
		s.WriteString((*i).second);
		s.WriteString("\r\n");
		}
	s.WriteString("\r\n");	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP, high level parsing

// This method should look at all the range entries in iRangeParam
// and turn them into a list of ascending, non overlapping start/finish
// pairs in oRanges.
bool sOrganizeRanges(size_t iSourceSize, const Val& iRangeParam,
	vector<pair<size_t, size_t> >& oRanges)
	{
	Map asMap = iRangeParam.GetMap();
	int64 reqBegin;
	if (asMap.Get("begin").QGet_T(reqBegin))
		{
		int64 reqEnd;
		if (asMap.Get("end").QGet_T(reqEnd))
			{
			if (reqEnd < reqBegin)
				return false;
			if (reqEnd > iSourceSize)
				return false;
			}
		if (reqBegin > iSourceSize)
			return false;
		oRanges.push_back(pair<size_t, size_t>(reqBegin, reqEnd + 1));
		return true;
		}
	else
		{
		int64 reqLast;
		if (asMap.Get("last").QGet_T(reqLast))
			{
			if (reqLast > iSourceSize)
				return false;
			oRanges.push_back(pair<size_t, size_t>(iSourceSize - reqLast, iSourceSize));
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

	if (!sReadToken(theStreamU, nullptr, oMethod))
		{
		if (oError)
			*oError = "Failed to read method";
		return false;
		}

	sSkipLWS(theStreamU);

	if (!sReadURI(theStreamU, oURL))
		{
		if (oError)
			*oError = "Failed to read URI";
		return false;
		}

	sSkipLWS(theStreamU);

	int32 major, minor;
	if (!sReadHTTPVersion(theStreamU, &major, &minor))
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
	if (!sReadHTTPVersion(iStream, &major, &minor))
		return false;

	sSkipLWS(iStream);

	if (!spReadInt32(iStream, oResultCode))
		return false;

	sSkipLWS(iStream);

	// Suck up the remainder of the stream as the result message.
	for (;;)
		{
		char readChar;
		if (!iStream.ReadChar(readChar))
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
		if (!gotOne)
			return true;
		}
	}

bool sReadHeaderLineNoParsing(const ZStreamU& iStream, Map* ioFields)
	{
	string fieldNameExact;
	if (!sReadFieldName(iStream, nullptr, &fieldNameExact))
		return false;

	if (!fieldNameExact.size())
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
		if (!gotOne)
			return true;
		}
	}

bool sReadHeaderLine(const ZStreamU& iStream, Map* ioFields)
	{
	string fieldName;
	if (!sReadFieldName(iStream, &fieldName, nullptr))
		return false;

	if (fieldName.empty())
		return true;

	if (false)
		{}
	// ----------------------------------------
	// Request headers
	else if (fieldName == "accept") sRead_accept(iStream, ioFields);
	else if (fieldName == "accept-charset")
		{
		for (;;)
			{
			sSkipLWS(iStream);

			string charset;
			if (!sReadToken(iStream, &charset, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "accept-charset", charset);

			sSkipLWS(iStream);

			if (!sReadChar(iStream, ','))
				break;
			}
		}
	else if (fieldName == "accept-encoding")
		{
		for (;;)
			{
			sSkipLWS(iStream);

			string encoding;
			if (!sReadToken(iStream, &encoding, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "accept-encoding", encoding);

			sSkipLWS(iStream);

			if (!sReadChar(iStream, ','))
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
			if (!sReadParameter_Cookie(iStream, nullptr, &cookieValue, &cookieName))
				break;

			if (ioFields)
				{
				Map cookieMap = ioFields->Get("cookie").GetMap();
				cookieMap.Set(cookieName, cookieValue);
				ioFields->Set("cookie", cookieMap);
				}

			sSkipLWS(iStream);

			if (!sReadChar(iStream, ';'))
				break;
			}
		}
	// ----------------------------------------
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
	// ----------------------------------------
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
	// ----------------------------------------
	// Entity headers
//	else if (fieldName == "allow")
//	else if (fieldName == "content-base")
	else if (fieldName == "content-encoding")
		{
		for (;;)
			{
			sSkipLWS(iStream);

			string encoding;
			if (!sReadToken(iStream, &encoding, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "content-encoding", encoding);

			sSkipLWS(iStream);

			if (!sReadChar(iStream, ','))
				break;
			}
		}
	else if (fieldName == "content-language")
		{
		for (;;)
			{
			sSkipLWS(iStream);

			string language;
			if (!sReadToken(iStream, &language, nullptr))
				break;

			if (ioFields)
				spAppend(*ioFields, "content-language", language);

			sSkipLWS(iStream);

			if (!sReadChar(iStream, ','))
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

static string spDecode_URI(const string& iString)
	{
	string result = iString;
	replace(result.begin(), result.end(),'+',' '); // replace + with space
	string::size_type pos=0;
	while ((pos = result.find_first_of('%',pos)) != string::npos)
		{
		char hexVal = 0;
		for (size_t k = 0; (k < 2) && (isxdigit(result[pos + k + 1])); ++k)
			{
			if (isdigit(result[pos + k + 1]))
				hexVal = 16 * hexVal + (result[pos + k + 1] - '0');
			else
				hexVal = 16 * hexVal + (10 + toupper(result[pos + k + 1]) - 'A');
			}
		result.erase(pos, 3);
		result.insert(pos, 1, hexVal);
		++pos;
		}

	return result;
	}

void sParseParam(const string& iString, Map& oParam)
	{
	string::size_type prevPos = 0;
	string::size_type pos;

	while ((pos = iString.find_first_of('&', prevPos)) != string::npos)
		{
		string theData = iString.substr(prevPos, pos - prevPos);
		string::size_type epos = theData.find_first_of('=');
		if (epos != string::npos)
			{
			oParam.Set(theData.substr(0, epos),
				spDecode_URI(theData.substr(epos + 1, pos - epos - 1)));
			}
		prevPos = ++pos;
		}

	// now handle the last element (or the only element, in the case of no &
	string theData = iString.substr(prevPos, pos - prevPos);
	string::size_type epos = theData.find_first_of('=');
	if (epos != string::npos)
		{
		oParam.Set(theData.substr(0, epos),
			spDecode_URI(theData.substr(epos + 1, pos - epos - 1)));
		}
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
			if (!iStream.ReadChar(readChar))
				break;

			if (readChar == '=')
				{
				iStream.Unread();
				break;
				}
			name.append(1, readChar);
			}

		if (!sReadChar(iStream, '='))
			break;

		string value;
		for (;;)
			{
			if (!iStream.ReadChar(readChar))
				break;

			if (readChar == '&')
				{
				iStream.Unread();
				break;
				}
			value.append(1, readChar);
			}
		oTuple.Set(name, value);
		if (!sReadChar(iStream, '&'))
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
		if (!s.ReadChar(readChar))
			break;
		gotAny = true;
		if (readChar == '/')
			break;
		if (readChar == '%')
			{
			string decodedChars;
			if (!sReadDecodedChars(s, oComponent))
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
		if (!sDecodeComponent(s, component))
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

		char probChar = iString[nextProb];
		char p[4];
		sprintf(p,"%%%02hhX", probChar);
		result.append(p);
		}
	return result;
	}

string sEncodeTrail(const ZTrail& iTrail)
	{
	string result;
	for (size_t x = 0; x < iTrail.Count(); ++x)
		{
		result += '/';
		result += sEncodeComponent(iTrail.At(x));
		}
	return result;
	}

string sGetString0(const Val& iVal)
	{
	if (ZQ_T<string> result = iVal.QGet_T<string>())
		return result.Get();

	const Seq& theSeq = iVal.GetSeq();
	if (theSeq.Count())
		return theSeq.Get(0).Get_T<string>();

	return string();
	}

static ZRef<ZStreamerR> spMakeStreamer_Transfer(
	const Map& iHeader, ZRef<ZStreamerR> iStreamerR)
	{
	// According to the spec, if content is chunked, content-length must be ignored.
	// I've seen some pages being returned with transfer-encoding "chunked, chunked", which
	// is either a mistake, or is nested chunking. I'm assuming the former for now.
	if (ZString::sContainsi(sGetString0(iHeader.Get("transfer-encoding")), "chunked"))
		{
		return new ZStreamerR_FT<StreamR_Chunked>(iStreamerR);
		}

	if (ZQ_T<int64> contentLength = iHeader.Get("content-length").QGet_T<int64>())
		return new ZStreamerR_FT<ZStreamR_Limited>(contentLength.Get(), iStreamerR);

	return iStreamerR;
	}

ZRef<ZStreamerR> sMakeContentStreamer(const Map& iHeader, ZRef<ZStreamerR> iStreamerR)
	{
	iStreamerR = spMakeStreamer_Transfer(iHeader, iStreamerR);

	// We could/should look for gzip Content-Encoding, and wrap a decoding filter around it.

	return iStreamerR;
	}

ZRef<ZStreamerR> sMakeContentStreamer(const Map& iHeader, const ZStreamR& iStreamR)
	{
	return sMakeContentStreamer(iHeader, new ZStreamerR_Stream(iStreamR));
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP, request headers

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
		if (!sReadMediaType(iStream, &type, &subtype, &parameters, nullptr, nullptr))
			break;
		Map temp;
		temp.Set("type", type);
		temp.Set("subtype", subtype);
		if (parameters)
			temp.Set("parameters", parameters);
		if (ioFields)
			spAppend(*ioFields, "accept", temp);

		sSkipLWS(iStream);

		if (!sReadChar(iStream, ','))
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
		if (!sReadLanguageTag(iStream, &languageTag))
			break;

		Map temp;
		temp.Set("tag", languageTag);
		
		Map parameters;
		for (;;)
			{
			sSkipLWS(iStream);
	
			if (!sReadChar(iStream, ';'))
				break;
	
			string name, value;
			if (!sReadParameter(iStream, &name, &value, nullptr))
				break;
			parameters.Set(name, value); 
			}
	
		if (parameters)
			temp.Set("parameters", parameters);

		if (ioFields)
			spAppend(*ioFields, "accept-language", temp);

		sSkipLWS(iStream);

		if (!sReadChar(iStream, ','))
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
	if (!sRead_range(iStream, theRange))
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

	if (!sReadChars(iStream, "bytes"))
		return false;

	sSkipLWS(iStream);

	if (!sReadChar(iStream, '='))
		return false;

	sSkipLWS(iStream);

	if (sReadChar(iStream, '-'))
		{
		sSkipLWS(iStream);

		int64 lastBytes;
		if (!spReadInt64(iStream, &lastBytes))
			return false;
		oRange.Set("last", lastBytes);
		return true;
		}
	else
		{
		sSkipLWS(iStream);

		int64 begin;
		if (!spReadInt64(iStream, &begin))
			return false;

		sSkipLWS(iStream);

		oRange.Set("begin", begin);

		if (!sReadChar(iStream, '-'))
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
#pragma mark -
#pragma mark * ZHTTP, response headers

bool sRead_www_authenticate(const ZStreamU& iStream, Map* ioFields);

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP, request or response headers

bool sRead_transfer_encoding(const ZStreamU& iStream, Map* ioFields)
	{
	string encoding;
	if (!sRead_transfer_encoding(iStream, encoding))
		return false;
	
	if (ioFields)
		ioFields->Set("transfer-encoding", encoding);

	return true;
	}

bool sRead_transfer_encoding(const ZStreamU& iStream, string& oEncoding)
	{
	sSkipLWS(iStream);

	if (!sReadToken(iStream, &oEncoding, nullptr))
		return false;

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP, entity headers

bool sRead_content_disposition(const ZStreamU& iStream, Map* ioFields)
	{
	Map dispositionTuple;
	if (!sRead_content_disposition(iStream, dispositionTuple))
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
	
			if (!sReadChar(iStream, ';'))
				break;
	
			string name, value;
			if (!sReadParameter(iStream, &name, &value, nullptr))
				break;
			parameters.Set(name, value);
			}

		sSkipLWS(iStream);

		if (parameters)
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
	if (!sRead_content_range(iStream, begin, end, maxLength))
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

	if (!sReadChars(iStream, "bytes"))
		return false;

	sSkipLWS(iStream);

	if (!spReadInt64(iStream, &oBegin))
		return false;

	sSkipLWS(iStream);

	if (!sReadChar(iStream, '-'))
		return false;

	sSkipLWS(iStream);

	if (!spReadInt64(iStream, &oEnd))
		return false;

	sSkipLWS(iStream);

	if (!sReadChar(iStream, '/'))
		return false;

	sSkipLWS(iStream);

	if (!spReadInt64(iStream, &oMaxLength))
		return false;

	return true;
	}

bool sRead_content_type(const ZStreamU& iStream, Map* ioFields)
	{
	string type, subType;
	Map parameters;
	if (!sRead_content_type(iStream, type, subType, parameters))
		return false;

	if (ioFields)
		{
		Map temp;
		temp.Set("type", type);
		temp.Set("subtype", subType);
		if (parameters)
			temp.Set("parameters", parameters);
		ioFields->Set("content-type", temp);
		}
	return true;
	}

bool sRead_content_type(const ZStreamU& iStream,
	string& oType, string& oSubType, Map& oParameters)
	{
	if (!sReadMediaType(iStream, &oType, &oSubType, &oParameters, nullptr, nullptr))
		return false;
	return true;
	}

// =================================================================================================

bool sReadHTTPVersion(const ZStreamU& iStream, int32* oVersionMajor, int32* oVersionMinor)
	{
	if (!sReadChars(iStream, "HTTP/"))
		return false;

	if (!spReadInt32(iStream, oVersionMajor))
		return false;

	if (!sReadChar(iStream, '.'))
		return false;

	if (!spReadInt32(iStream, oVersionMinor))
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
		if (!iStream.ReadChar(readChar))
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

	if (!sReadToken(iStream, oName, oNameExact))
		return false;

	sSkipLWS(iStream);

	if (!sReadChar(iStream, ':'))
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

	if (!sReadToken(iStream, oName, oNameExact))
		return false;

	sSkipLWS(iStream);

	if (!sReadChar(iStream, '='))
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

	if (!sReadToken(iStream, oName, oNameExact))
		return false;

	sSkipLWS(iStream);

	if (!sReadChar(iStream, '='))
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

	if (!sReadToken(iStream, oType, oTypeExact))
		return false;

	sSkipLWS(iStream);

	if (!sReadChar(iStream, '/'))
		return true;

	sSkipLWS(iStream);

	if (!sReadToken(iStream, oSubtype, oSubtypeExact))
		return true;

	for (;;)
		{
		sSkipLWS(iStream);

		if (!sReadChar(iStream, ';'))
			break;

		string name, value;
		if (!sReadParameter(iStream, &name, &value, nullptr))
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
	if (!iStream.ReadChar(readChar))
		return false;

	if (!sIs_ALPHA(readChar))
		{
		iStream.Unread();
		return false;
		}

	if (oLanguageTag)
		oLanguageTag->append(1, readChar);

	for (;;)
		{
		if (!iStream.ReadChar(readChar))
			return true;

		if (!sIs_ALPHA(readChar) && readChar != '-')
			{
			iStream.Unread();
			return true;
			}

		if (oLanguageTag)
			oLanguageTag->append(1, readChar);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP, Lower level parsing

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
	if (string::npos == dividerOffset)
		{
		if (oPath)
			*oPath = iURL;
		return true;
		}
	else
		{
		start = dividerOffset + std::strlen(schemeDivider);
		if (oScheme)
			*oScheme = iURL.substr(0, dividerOffset);
		}

	string hostAndPort;
	const size_t slashOffset = iURL.find('/', start);
	if (string::npos != slashOffset)
		{
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
		if (oPort)
			*oPort = ZString::sAsInt(hostAndPort.substr(colonOffset + 1));
		if (oHost)
			*oHost = hostAndPort.substr(0, colonOffset);
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
		if (!iStream.ReadChar(readChar))
			break;

		if (!sIs_token(readChar))
			{
			iStream.Unread();
			break;
			}

		if (readChar == '%')
			{
			string decodedChars;
			if (!sReadDecodedChars(iStream, decodedChars))
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
		if (!iStream.ReadChar(readChar))
			break;

		if (!sIs_token(readChar))
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
			if (!sReadDecodedChars(iStream, decodedChars))
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

	if (!sReadChar(iStream, '"'))
		return false;

	for (;;)
		{
		char readChar;
		if (!iStream.ReadChar(readChar))
			break;

		if (!sIs_qdtext(readChar))
			{
			iStream.Unread();
			break;
			}

		if (oString)
			oString->append(1, char(tolower(readChar)));
		if (oStringExact)
			oStringExact->append(1, readChar);
		}

	if (!sReadChar(iStream, '"'))
		return false;

	return true;
	}

bool sReadChar(const ZStreamU& iStream, char iChar)
	{
	char readChar;
	if (!iStream.ReadChar(readChar))
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
		if (!iStream.ReadChar(readChar))
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
		if (!iStream.ReadChar(readChar))
			break;

		if (!sIs_LWS(readChar))
			{
			iStream.Unread();
			break;
			}
		}
	}

bool sReadDecodedChars(const ZStreamU& iStream, string& ioString)
	{
	char readChar;
	if (!iStream.ReadChar(readChar))
		return false;

	if (!isxdigit(readChar))
		{
		iStream.Unread();
		ioString.append(1, '%');
		}
	else
		{
		char readChar2;
		if (!iStream.ReadChar(readChar2))
			return false;

		if (!isxdigit(readChar2))
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
#pragma mark -
#pragma mark * ZHTTP, Lexical classification

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
#pragma mark -
#pragma mark * ZHTTP::StreamR_Chunked

static uint64 spReadChunkSize(const ZStreamR& s)
	{
	uint64 result = 0;
	for (;;)
		{
		char theChar;
		if (!s.ReadChar(theChar))
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
#pragma mark -
#pragma mark * ZHTTP::StreamW_Chunked

StreamW_Chunked::StreamW_Chunked(size_t iBufferSize, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fBufferSize(max(size_t(64), iBufferSize))
	{
	fBuffer = new uint8[fBufferSize];
	fBufferUsed = 0;
	}

StreamW_Chunked::StreamW_Chunked(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fBufferSize(1024)
	{
	fBuffer = new uint8[fBufferSize];
	fBufferUsed = 0;
	}

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

	delete[] fBuffer;
	}

void StreamW_Chunked::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	while (iCount)
		{
		if (fBufferUsed + iCount >= fBufferSize)
			{
			// The data would overflow the buffer, so we can write the
			// buffer content (if any) plus this new stuff.
			fStreamSink.Writef("%X\r\n", fBufferUsed + iCount);
			// Hmmm. Do we allow an end of stream exception to propogate?
			fStreamSink.Write(fBuffer, fBufferUsed);
			fBufferUsed = 0;
			fStreamSink.Write(localSource, iCount);
			fStreamSink.WriteString("\r\n");
			localSource += iCount;
			iCount = 0;
			}
		else
			{
			size_t countToCopy = min(iCount, size_t(fBufferSize - fBufferUsed));
			ZMemCopy(fBuffer + fBufferUsed, localSource, countToCopy);
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
		fStreamSink.Write(fBuffer, bufferUsed);
		fStreamSink.WriteString("\r\n");
		}
	}

} // namespace ZHTTP

NAMESPACE_ZOOLIB_END
