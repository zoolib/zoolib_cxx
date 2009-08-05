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
#include "zoolib/ZStreamR_Boundary.h"
#include "zoolib/ZStreamR_SkipAllOnDestroy.h"
#include "zoolib/ZStream_String.h"
#include "zoolib/ZStream_Tee.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimmer.h"
#include "zoolib/ZStrimmer_Stream.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZTextCoder.h"

#include <ctype.h> // For isalnum

NAMESPACE_ZOOLIB_BEGIN

namespace ZHTTP {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP

static bool spReadResponse(const ZStreamR& r, int32* oResponseCode, Map* oHeader)
	{
	ZMIME::StreamR_Header theSIH_Server(r);

	string serverResultMessage;
	if (!sReadResponse(ZStreamU_Unreader(theSIH_Server),
		oResponseCode, nullptr))
		{
		return false;
		}

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

ZRef<ZStreamerR> sRequest(
	const string& iMethod, string& ioURL,
	int32* oResultCode, Map* oFields, Data* oRawHeader)
	{
	for (bool keepGoing = true; keepGoing; /*no inc*/)
		{
		if (oRawHeader)
			oRawHeader->SetSize(0);

		string theHost;
		ip_port thePort = 80;
		string thePath;
		if (sParseURL(ioURL, nullptr, &theHost, &thePort, &thePath))
			{
			ZRef<ZNetName_Internet> theNN = new ZNetName_Internet(theHost, thePort);
			ZRef<ZNetEndpoint> theEndpoint = theNN->Connect(10);
			if (!theEndpoint)
				break;

			int32 theResponseCode;
			Map theHeaders;
			if (!spRequest(
				theEndpoint->GetStreamW(), theEndpoint->GetStreamR(),
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
						return new ZStreamerR_T<ZStreamR_Null>();

					ZRef<ZStreamerR> theStreamerR
						= sMakeContentStreamer(theHeaders, theEndpoint);

					if (!theStreamerR)
						theStreamerR = theEndpoint;

					return theStreamerR;
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
	return ZRef<ZStreamerR>();
	}

static void spPost_Prefix(const ZStreamW& w,
	const string& iHost, const string& iPath, bool iSendConnectionClose)
	{
	w.WriteString("POST ");
	w.WriteString(iPath);
	w.WriteString(" HTTP/1.1\r\n");
	w.WriteString("Host: ");
	w.WriteString(iHost);
	w.WriteString("\r\n");
	if (iSendConnectionClose)
		w.WriteString("Connection: close\r\n");
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

ZRef<ZStreamerR> sPost(
	const std::string& iURL, const ZStreamR& iBody,
	int32* oResultCode, Map* oFields, Data* oRawHeader)
	{
	string theHost;
	ip_port thePort;
	string thePath;
	if (sParseURL(iURL, nullptr, &theHost, &thePort, &thePath))
		{
		ZRef<ZNetName_Internet> theNN = new ZNetName_Internet(theHost, thePort);
		if (ZRef<ZNetEndpoint> theEndpoint = theNN->Connect(10))
			{
			const ZStreamR& r = theEndpoint->GetStreamR();
			const ZStreamW& w = theEndpoint->GetStreamW();

			spPost_Prefix(w, theHost, thePath, true);

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

					ZRef<ZStreamerR> theStreamerR
						= sMakeContentStreamer(theHeaders, theEndpoint);

					if (!theStreamerR)
						theStreamerR = theEndpoint;

					return theStreamerR;
					}
				}
			}
		}
	return ZRef<ZStreamerR>();
	}

ZRef<ZStreamerR> sPostRaw(
	const std::string& iURL, const ZStreamR& iBody,
	int32* oResultCode, Map* oFields, Data* oRawHeader)
	{
	string theHost;
	ip_port thePort;
	string thePath;
	if (sParseURL(iURL, nullptr, &theHost, &thePort, &thePath))
		{
		ZRef<ZNetName_Internet> theNN = new ZNetName_Internet(theHost, thePort);
		if (ZRef<ZNetEndpoint> theEndpoint = theNN->Connect(10))
			{
			const ZStreamR& r = theEndpoint->GetStreamR();
			const ZStreamW& w = theEndpoint->GetStreamW();

			spPost_Prefix(w, theHost, thePath, true);
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

					ZRef<ZStreamerR> theStreamerR
						= sMakeContentStreamer(theHeaders, theEndpoint);

					if (!theStreamerR)
						theStreamerR = theEndpoint;

					return theStreamerR;
					}
				}
			}
		}
	return ZRef<ZStreamerR>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP, read a POST into a Map

static ZRef<ZStrimmerR> spCreateStrimmerR(const Map& iHeader, const ZStreamR& iStreamR)
	{
	const string charset = iHeader
		.Get("content-type").GetMap()
		.Get("parameters").GetMap()
		.Get("charset").GetString();

	if (ZTextDecoder* theDecoder = ZTextDecoder::sMake(charset))
		return new ZStrimmerR_StreamR_T<ZStrimR_StreamDecoder>(theDecoder, iStreamR);
	else
		return new ZStrimmerR_StreamR_T<ZStrimR_StreamUTF8>(iStreamR);
	}

static bool spReadName(const ZStreamU& iStreamU, string& oName)
	{
	bool gotAny = false;
	for (;;)
		{
		char curChar;
		if (!iStreamU.ReadChar(curChar))
			break;
		if (!isalnum(curChar) && curChar != '_')
			{
			iStreamU.Unread();
			break;
			}
		oName += curChar;
		gotAny = true;
		}
	return gotAny;
	}

static bool spReadPOST(const ZStreamR& iStreamR, const Map& iHeader, Val& oVal)
	{
	const Map content_type = iHeader.Get("content-type").GetMap();
	if (content_type.Get("type").GetString() == "application"
		&& content_type.Get("subtype").GetString() == "x-www-url-encoded")
		{
		// It's application/x-www-url-encoded. So we're going to unpack it into a Map.
		// Map& theTuple = oVal.SetMutableTuple();
		// yadda yadda.
		//#warning "not done yet"
		return true;
		}
	else if (content_type.Get("type").GetString() == "multipart"
		&& content_type.Get("subtype").GetString() == "form-data")
		{
		const string baseBoundary = content_type
			.Get("parameters").GetMap()
			.Get("boundary").GetString();

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
			sReadHeader(
				ZStreamR_SkipAllOnDestroy(ZMIME::StreamR_Header(streamPart)), &header);

			Map contentDisposition = header.Get("content-disposition").GetMap();
			if (contentDisposition.Get("value").GetString() == "form-data")
				{
				const Map parameters = contentDisposition.Get("parameters").GetMap();
				const string name = parameters.Get("name").GetString();
				if (!name.empty())
					{
					Val theVal;
					if (spReadPOST(streamPart, header, theVal))
						theMap.Set(name, theVal);
					}
				}
			streamPart.SkipAll();
			}
		oVal = theMap;
		}
	else if (content_type.Get("type").GetString() == "text")
		{
		// It's explicitly some kind of text. Use sCreateStrimmerR to create an appropriate
		// strimmer, which it does by examining values in iHeader.
		string theString;
		spCreateStrimmerR(iHeader, iStreamR)->GetStrimR().CopyAllTo(ZStrimW_String(theString));
		oVal.SetString(theString);
		return true;		
		}
	else if (!content_type)
		{
		// There was no content type specified, so assume text.
		string theString;
		ZStreamWPos_String(theString).CopyAllFrom(iStreamR);
		oVal.SetString(theString);
		return true;
		}

	// It's some other kind of content. Put it into a raw.
	oVal = sReadAll_T<Data>(iStreamR);
	return true;
	}

} // namespace ZHTTP

NAMESPACE_ZOOLIB_END
