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

#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZMIME.h"
#include "zoolib/ZNet_Internet.h"
#include "zoolib/ZStreamR_Boundary.h"
#include "zoolib/ZStreamR_SkipAllOnDestroy.h"
#include "zoolib/ZStream_String.h"
#include "zoolib/ZStream_Tee.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimmer.h"
#include "zoolib/ZStrimmer_Stream.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZTextCoder.h"

#include <ctype.h> // For isalnum

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZHTTP

static bool sReadResponse(const ZStreamR& r, int32* oResponseCode, ZTuple* oHeader)
	{
	ZMIME::StreamR_Header theSIH_Server(r);

	string serverResultMessage;
	if (!ZHTTP::sReadResponse(ZStreamU_Unreader(theSIH_Server),
		oResponseCode, nullptr))
		{
		return false;
		}

	if (!ZHTTP::sReadHeader(theSIH_Server, oHeader))
		return false;

	return true;
	}

static bool sRequest(const ZStreamW& w, const ZStreamR& r,
	const string& iMethod, const string& iHost, const string& iPath,
	bool iSendConnectionClose,
	int32* oResponseCode, ZTuple* oHeader, ZMemoryBlock* oRawHeader)
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
		ZStreamRWPos_MemoryBlock theSRWP_MB(*oRawHeader);
		ZStreamR_Tee theStream_Tee(r, theSRWP_MB);
		return sReadResponse(theStream_Tee, oResponseCode, oHeader);
		}
	else
		{
		return sReadResponse(r, oResponseCode, oHeader);
		}
	}

ZRef<ZStreamerR> ZHTTP::sRequest(
	const string& iMethod, string& ioURL,
	int32* oResultCode, ZTuple* oFields, ZMemoryBlock* oRawHeader)
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
			ZTuple theHeaders;
			if (!sRequest(
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
						= ZHTTP::sMakeContentStreamer(theHeaders, theEndpoint);

					if (!theStreamerR)
						theStreamerR = theEndpoint;

					return theStreamerR;
					}
				case 301:
				case 302:
				case 303:
					{
					ioURL = ZHTTP::sGetString0(theHeaders.GetValue("location"));
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

static void sPOSTPrefix(const ZStreamW& w,
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

static bool sPOSTSuffix(const ZStreamR& r,
	int32* oResponseCode, ZTuple* oHeader, ZMemoryBlock* oRawHeader)
	{
	if (oRawHeader)
		{
		ZStreamRWPos_MemoryBlock theSRWP_MB(*oRawHeader);
		ZStreamR_Tee theStream_Tee(r, theSRWP_MB);
		return sReadResponse(theStream_Tee, oResponseCode, oHeader);
		}
	else
		{
		return sReadResponse(r, oResponseCode, oHeader);
		}
	}

ZRef<ZStreamerR> ZHTTP::sPost(
	const std::string& iURL, const ZStreamR& iBody,
	int32* oResultCode, ZTuple* oFields, ZMemoryBlock* oRawHeader)
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

			sPOSTPrefix(w, theHost, thePath, true);

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
				ZHTTP::StreamW_Chunked(16 *1024, w).CopyAllFrom(iBody);
				}
			w.Flush();

			int32 theResponseCode;
			ZTuple theHeaders;
			if (sPOSTSuffix(r, &theResponseCode, &theHeaders, oRawHeader))
				{
				if (200 == theResponseCode)
					{
					if (oFields)
						*oFields = theHeaders;

					ZRef<ZStreamerR> theStreamerR
						= ZHTTP::sMakeContentStreamer(theHeaders, theEndpoint);

					if (!theStreamerR)
						theStreamerR = theEndpoint;

					return theStreamerR;
					}
				}
			}
		}
	return ZRef<ZStreamerR>();
	}

ZRef<ZStreamerR> ZHTTP::sPostRaw(
	const std::string& iURL, const ZStreamR& iBody,
	int32* oResultCode, ZTuple* oFields, ZMemoryBlock* oRawHeader)
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

			sPOSTPrefix(w, theHost, thePath, true);
			w.CopyAllFrom(iBody);
			w.Flush();

			int32 theResponseCode;
			ZTuple theHeaders;
			if (sPOSTSuffix(r, &theResponseCode, &theHeaders, oRawHeader))
				{
				if (200 == theResponseCode)
					{
					if (oFields)
						*oFields = theHeaders;

					ZRef<ZStreamerR> theStreamerR
						= ZHTTP::sMakeContentStreamer(theHeaders, theEndpoint);

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
#pragma mark * ZHTTP, read a POST into a tuple

static ZRef<ZStrimmerR> sCreateStrimmerR(const ZTuple& iHeader, const ZStreamR& iStreamR)
	{
	string charset = iHeader.GetTuple("content-type").GetTuple("parameters").GetString("charset");

	if (ZTextDecoder* theDecoder = ZTextDecoder::sMake(charset))
		return new ZStrimmerR_StreamR_T<ZStrimR_StreamDecoder>(theDecoder, iStreamR);
	else
		return new ZStrimmerR_StreamR_T<ZStrimR_StreamUTF8>(iStreamR);
	}


static bool sReadName(const ZStreamU& iStreamU, string& oName)
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

#if 0
static bool sReadValue(const ZStreamU& iStreamU, string& oValue)
	{
	//#warning "not done yet"
	bool gotAny = false;
	for (;;)
		{
		char curChar;
		if (!iStreamU.ReadChar(curChar))
			break;

		if (curChar == '%')
			{
			
			}
		else
			{
			oValue += curChar;
			}
		gotAny = true;
		}
	return gotAny;
	}

static void sReadParams(const ZStreamU& iStreamU, ZTuple& ioTuple)
	{
	for (;;)
		{
		string name;
		if (!sReadName(iStreamU, name))
			break;

		if (!ZHTTP::sReadChar(iStreamU, '='))
			{
			// It's a param with no value.
			ioTuple.SetNull(name);
			}
		else
			{
			string value;
			if (!sReadValue(iStreamU, value))
				break;
			ioTuple.SetString(name, value);
			}
		}
	}
#endif

static bool sReadPOST(const ZStreamR& iStreamR, const ZTuple& iHeader, ZTValue& oTV)
	{
	ZTuple content_type = iHeader.GetTuple("content-type");
	if (content_type.GetString("type") == "application"
		&& content_type.GetString("subtype") == "x-www-url-encoded")
		{
		// It's application/x-www-url-encoded. So we're going to unpack it into a tuple.
		ZTuple& theTuple = oTV.SetMutableTuple();
		// yadda yadda.
		//#warning "not done yet"
		return true;
		}
	else if (content_type.GetString("type") == "multipart"
		&& content_type.GetString("subtype") == "form-data")
		{
		ZTuple& theTuple = oTV.SetMutableTuple();

		const string baseBoundary = content_type.GetTuple("parameters").GetString("boundary");

		// Skip optional unheadered data section before the first boundary.
		ZStreamR_Boundary(baseBoundary, iStreamR).SkipAll();

		const string boundary = "\r\n--" + baseBoundary;
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

			// We parse it into the tuple called 'header'.
			ZTuple header;
			ZHTTP::sReadHeader(
				ZStreamR_SkipAllOnDestroy(ZMIME::StreamR_Header(streamPart)), &header);

			ZTuple contentDisposition = header.GetTuple("content-disposition");
			if (contentDisposition.GetString("value") == "form-data")
				{
				ZTuple parameters = contentDisposition.GetTuple("parameters");
				ZTName name(parameters.GetString("name"));
				if (!name.Empty())
					{
					if (!sReadPOST(streamPart, header, theTuple.SetMutableNull(name)))
						theTuple.Erase(name);
					}
				}
			streamPart.SkipAll();
			}
		}
	else if (content_type.GetString("type") == "text")
		{
		// It's explicitly some kind of text. Use sCreateStrimmerR to create an appropriate
		// strimmer, which it does by examining values in iHeader.
		string theString;
		sCreateStrimmerR(iHeader, iStreamR)->GetStrimR().CopyAllTo(ZStrimW_String(theString));
		oTV.SetString(theString);
		return true;		
		}
	else if (!content_type)
		{
		// There was no content type specified, so assume text.
		string theString;
		ZStreamWPos_String(theString).CopyAllFrom(iStreamR);
		oTV.SetString(theString);
		return true;
		}

	// It's some other kind of content. Put it into a raw.
	oTV.SetRaw(iStreamR);
	return true;
	}

NAMESPACE_ZOOLIB_END
