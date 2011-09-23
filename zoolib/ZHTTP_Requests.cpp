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

#include "zoolib/ZHTTP_Requests.h"

#include "zoolib/ZMIME.h"
#include "zoolib/ZNet_Internet.h"
#include "zoolib/ZStream_String.h"
#include "zoolib/ZStream_Tee.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStreamR_Boundary.h"
#include "zoolib/ZStreamR_SkipAllOnDestroy.h"
#include "zoolib/ZStreamerRWCon_SSL.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimmer.h"
#include "zoolib/ZStrimmer_Stream.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZTextCoder.h"
#include "zoolib/ZUtil_string.h"

#include <ctype.h> // For isalnum

namespace ZooLib {
namespace ZHTTP {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP

static bool spReadResponse(const ZStreamR& r, int32* oResponseCode, Map* oHeader)
	{
	ZMIME::StreamR_Header theSIH_Server(r);

	string serverResultMessage;
	if (!sReadResponse(ZStreamU_Unreader(theSIH_Server), oResponseCode, nullptr))
		return false;

	if (!sReadHeader(theSIH_Server, oHeader))
		return false;

	return true;
	}

static bool spRequest(const ZStreamW& w, const ZStreamR& r,
	const string& iMethod, const string& iHost, const string& iPath,
	bool iSendConnectionClose,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	w.WriteString(iMethod);
	w.WriteString(" ");
	w.WriteString(iPath);
	w.WriteString(" HTTP/1.1\r\n");
	w.WriteString("Host: ");
	w.WriteString(iHost);
	w.WriteString("\r\n");
	if (iSendConnectionClose)
		w.WriteString("Connection: close\r\n");
	w.WriteString("\r\n");
	w.Flush();

	if (oRawHeader)
		{
		ZStreamRWPos_Data_T<Data> theSRWP_Data(*oRawHeader);
		ZStreamR_Tee theStream_Tee(r, theSRWP_Data);
		return spReadResponse(theStream_Tee, oResponseCode, oHeader);
		}
	else
		{
		return spReadResponse(r, oResponseCode, oHeader);
		}
	}

static ZRef<ZStreamerRWCon> spConnect(ZRef<Callable_Connect> iCallable_Connect,
	const string& iScheme, const string& iHost, ip_port iPort)
	{
	if (false)
		{}
	else if (ZUtil_string::sEquali("http", iScheme))
		{
		if (not iPort)
			iPort = 80;
		}
	else if (ZUtil_string::sEquali("https", iScheme))
		{
		if (not iPort)
			iPort = 443;
		}
	else
		{
		return null;
		}

	const bool useSSL = ZUtil_string::sEquali("https", iScheme);

	if (iCallable_Connect)
		{
		return iCallable_Connect->Call(iHost, iPort, useSSL);
		}
	else if (ZRef<ZStreamerRWCon> theEP = ZNetName_Internet(iHost, iPort).Connect(10))
		{
		if (useSSL)
			return sStreamerRWCon_SSL(theEP, theEP);
		return theEP;
		}
	else
		{
		return null;
		}
	}

ZRef<ZStreamerR> sRequest(ZRef<Callable_Connect> iCallable_Connect,
	const string& iMethod, string& ioURL,
	int32* oResultCode, Map* oFields, Data* oRawHeader)
	{
	for (bool keepGoing = true; keepGoing; /*no inc*/)
		{
		if (oRawHeader)
			oRawHeader->SetSize(0);

		string theScheme;
		string theHost;
		ip_port thePort;
		string thePath;
		if (sParseURL(ioURL, &theScheme, &theHost, &thePort, &thePath))
			{
			ZRef<ZStreamerRWCon> theEP = spConnect(iCallable_Connect, theScheme, theHost, thePort);
			if (!theEP)
				break;

			int32 theResponseCode;
			Map theHeaders;
			if (!spRequest
				(theEP->GetStreamW(), theEP->GetStreamR(),
				iMethod, theHost, thePath,
				true,
				&theResponseCode, &theHeaders, oRawHeader))
				{
				break;
				}

			if (oResultCode)
				*oResultCode = theResponseCode;

			switch (theResponseCode)
				{
				case 200:
					{
					if (oFields)
						*oFields = theHeaders;

					if ("HEAD" == iMethod)
						return new ZStreamerR_T<ZStreamR_Null>;

					if (ZRef<ZStreamerR> theStreamerR = sMakeContentStreamer(theHeaders, theEP))
						return theStreamerR;

					return theEP;
					}
				case 301:
				case 302:
				case 303:
					{
					ioURL = sGetString0(theHeaders.Get("location"));
					break;
					}
				default:
					{
					keepGoing = false;
					break;
					}
				}
			}
		}
	return null;
	}

static void spPost_Prefix(const ZStreamW& w,
	const string& iHost, const string& iPath, const Map* iFields, bool iSendConnectionClose)
	{
	w.WriteString("POST ");
	w.WriteString(iPath);
	w.WriteString(" HTTP/1.1\r\n");
	w.WriteString("Host: ");
	w.WriteString(iHost);
	w.WriteString("\r\n");
	if (iSendConnectionClose)
		w.WriteString("Connection: close\r\n");
	if (iFields)
		{
		for (Map::Index_t ii = iFields->Begin(); ii != iFields->End(); ++ii)
			{
			w.WriteString(iFields->NameOf(ii));
			w.WriteString(": ");
			w.WriteString(iFields->Get<string>(ii));
			w.WriteString("\r\n");
			}
		}
	}

static bool spPost_Suffix(const ZStreamR& r,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (oRawHeader)
		{
		ZStreamRWPos_Data_T<Data> theSRWP_Data(*oRawHeader);
		ZStreamR_Tee theStream_Tee(r, theSRWP_Data);
		return spReadResponse(theStream_Tee, oResponseCode, oHeader);
		}
	else
		{
		return spReadResponse(r, oResponseCode, oHeader);
		}
	}

ZRef<ZStreamerR> sPost(ZRef<Callable_Connect> iCallable_Connect,
	const std::string& iURL, const ZStreamR& iBody,
	int32* oResultCode, Map* oFields, Data* oRawHeader)
	{ return sPost(iCallable_Connect, iURL, nullptr, iBody, oResultCode, oFields, oRawHeader); }

ZRef<ZStreamerR> sPost(ZRef<Callable_Connect> iCallable_Connect,
	const std::string& iURL, const Map* iFields, const ZStreamR& iBody,
	int32* oResultCode, Map* oFields, Data* oRawHeader)
	{
	string theScheme;
	string theHost;
	ip_port thePort;
	string thePath;
	if (sParseURL(iURL, &theScheme, &theHost, &thePort, &thePath))
		{
		if (ZRef<ZStreamerRWCon> theEP = spConnect(iCallable_Connect, theScheme, theHost, thePort))
			{
			const ZStreamR& r = theEP->GetStreamR();
			const ZStreamW& w = theEP->GetStreamW();

			spPost_Prefix(w, theHost, thePath, iFields, true);

			if (const ZStreamRPos* bodyRPos = dynamic_cast<const ZStreamRPos*>(&iBody))
				{
				const uint64 theLength = bodyRPos->GetSize() - bodyRPos->GetPosition();
				w.Writef("Content-Length: %lld\r\n", theLength);
				w.WriteString("\r\n");

				w.CopyFrom(*bodyRPos, theLength);
				}
			else
				{
				w.WriteString("Transfer-Encoding: chunked\r\n");
				w.WriteString("\r\n");
				StreamW_Chunked(16 *1024, w).CopyAllFrom(iBody);
				}
			w.Flush();

			int32 theResponseCode;
			Map theHeaders;
			if (spPost_Suffix(r, &theResponseCode, &theHeaders, oRawHeader))
				{
				if (200 == theResponseCode)
					{
					if (oFields)
						*oFields = theHeaders;

					if (ZRef<ZStreamerR> theStreamerR = sMakeContentStreamer(theHeaders, theEP))
						return theStreamerR;

					return theEP;
					}
				}
			}
		}
	return null;
	}

ZRef<ZStreamerR> sPostRaw(ZRef<Callable_Connect> iCallable_Connect,
	const std::string& iURL, const ZStreamR& iBody,
	int32* oResultCode, Map* oFields, Data* oRawHeader)
	{
	string theScheme;
	string theHost;
	ip_port thePort;
	string thePath;
	if (sParseURL(iURL, &theScheme, &theHost, &thePort, &thePath))
		{
		if (ZRef<ZStreamerRWCon> theEP = spConnect(iCallable_Connect, theScheme, theHost, thePort))
			{
			const ZStreamR& r = theEP->GetStreamR();
			const ZStreamW& w = theEP->GetStreamW();

			spPost_Prefix(w, theHost, thePath, nullptr, true);
			w.CopyAllFrom(iBody);
			w.Flush();

			int32 theResponseCode;
			Map theHeaders;
			if (spPost_Suffix(r, &theResponseCode, &theHeaders, oRawHeader))
				{
				if (200 == theResponseCode)
					{
					if (oFields)
						*oFields = theHeaders;

					if (ZRef<ZStreamerR> theStreamerR = sMakeContentStreamer(theHeaders, theEP))
						return theStreamerR;

					return theEP;
					}
				}
			}
		}
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP, read a POST into a Map

static ZRef<ZStrimmerR> spCreateStrimmerR(const Map& iHeader, const ZStreamR& iStreamR)
	{
	const string charset =
		iHeader.Get("content-type").Get("parameters").Get<string>("charset");

	if (ZTextDecoder* theDecoder = ZTextDecoder::sMake(charset))
		return new ZStrimmerR_StreamR_T<ZStrimR_StreamDecoder>(theDecoder, iStreamR);
	else
		return new ZStrimmerR_StreamR_T<ZStrimR_StreamUTF8>(iStreamR);
	}

#if 0

static bool spReadName(const ZStreamU& iStreamU, string& oName)
	{
	bool gotAny = false;
	for (;;)
		{
		char curChar;
		if (not iStreamU.ReadChar(curChar))
			break;
		if (not isalnum(curChar) && curChar != '_')
			{
			iStreamU.Unread();
			break;
			}
		oName += curChar;
		gotAny = true;
		}
	return gotAny;
	}

#endif

static ZQ<Val> spReadPOST(const ZStreamR& iStreamR, const Map& iHeader)
	{
	const Map content_type = iHeader.Get<Map>("content-type");
	if (content_type.Get<string>("type") == "application"
		&& content_type.Get<string>("subtype") == "x-www-url-encoded")
		{
		// It's application/x-www-url-encoded. So we're going to unpack it into a Map.
		// Map& theTuple = oVal.SetMutableTuple();
		// yadda yadda.
		//#warning "not done yet"
		return null;
		}
	else if (content_type.Get<string>("type") == "multipart"
		&& content_type.Get<string>("subtype") == "form-data")
		{
		const string baseBoundary = content_type.Get("parameters").Get<string>("boundary");

		// Skip optional unheadered data section before the first boundary.
		ZStreamR_Boundary(baseBoundary, iStreamR).SkipAll();

		const string boundary = "\r\n--" + baseBoundary;
		Map theMap;
		for (;;)
			{
			bool done = false;
			// At this point we're sitting right after a boundary. We skip all subsequent
			// characters, although there's supposed to be only white space, until
			// we see "--", in which case we've hit the end, or we see CR LF, in which
			// case there's another part following.
			char prior = 0;
			for (;;)
				{
				char current = iStreamR.ReadInt8();
				if (prior == '-' && current == '-')
					{
					done = true;
					break;
					}

				if (prior == '\r' && current =='\n')
					break;

				prior = current;
				}
			if (done)
				break;

			// We're now sitting at the beginning of the part's header.
			ZStreamR_Boundary streamPart(boundary, iStreamR);

			// We parse it into the Map called 'header'.
			Map header;
			sReadHeader
				(ZStreamR_SkipAllOnDestroy(ZMIME::StreamR_Header(streamPart)), &header);

			Map contentDisposition = header.Get<Map>("content-disposition");
			if (contentDisposition.Get<string>("value") == "form-data")
				{
				const string name = contentDisposition.Get("parameters").Get<string>("name");
				if (!name.empty())
					{
					if (ZQ<Val> theQVal = spReadPOST(streamPart, header))
						theMap.Set(name, theQVal.Get());
					}
				}
			streamPart.SkipAll();
			}
		return theMap;
		}
	else if (content_type.Get<string>("type") == "text")
		{
		// It's explicitly some kind of text. Use spCreateStrimmerR to create an appropriate
		// strimmer, which it does by examining values in iHeader.
		return spCreateStrimmerR(iHeader, iStreamR)->GetStrimR().ReadAll8();
		}
	else if (!content_type)
		{
		// There was no content type specified, so assume text.
		string theString;
		ZStreamWPos_String(theString).CopyAllFrom(iStreamR);
		return theString;
		}

	// It's some other kind of content. Put it into a raw.
	return sReadAll_T<Data>(iStreamR);
	}

bool sCONNECT(const ZStreamR& r, const ZStreamW& w,
	const string& iAddress,
	const Map* iHeader,
	int32* oResultCode, Map* oHeader)
	{
	if (oResultCode)
		*oResultCode = 0;

	w.WriteString("CONNECT ");
	w.WriteString(iAddress);
	w.WriteString(" HTTP/1.0\r\n");

	if (iHeader)
		sWrite_Header(w, *iHeader);

	w.WriteString("\r\n");

	w.Flush();

	int32 serverResultCode;
	if (ZHTTP::sReadResponse(ZStreamU_Unreader(r), &serverResultCode, nullptr))
		{
		if (oResultCode)
			*oResultCode = serverResultCode;

		if (ZHTTP::sReadHeader(r, oHeader))
			return serverResultCode == 200;
		}

	return false;
	}

} // namespace ZHTTP
} // namespace ZooLib
