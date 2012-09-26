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
// MARK: -

ZRef<ZStreamerRWCon> sConnect(const string& iHost, uint16 iPort, bool iUseSSL)
	{
	if (ZRef<ZStreamerRWCon> theEP = ZNetName_Internet(iHost, iPort).Connect(10))
		{
		if (iUseSSL)
			return sStreamerRWCon_SSL(theEP, theEP);
		return theEP;
		}
	return null;
	}

// =================================================================================================
// MARK: -

static bool spReadResponse(const ZStreamR& r, int32* oResponseCode, Map* oHeader)
	{
	ZMIME::StreamR_Header theSIH_Server(r);

	string serverResultMessage;
	if (not sReadResponse(ZStreamU_Unreader(theSIH_Server), oResponseCode, nullptr))
		return false;

	if (not sReadHeader(theSIH_Server, oHeader))
		return false;

	return true;
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
		return iCallable_Connect->Call(iHost, iPort, useSSL);
	else
		return sConnect(iHost, iPort, useSSL);
	}

// =================================================================================================
// MARK: -

static bool spRequest(const ZStreamW& w, const ZStreamR& r,
	const string& iMethod, const string& iHost, const string& iPath, const Map* iHeader,
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

	if (iHeader)
		sWrite_Header(w, *iHeader);

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

ZRef<ZStreamerR> sRequest(ZRef<Callable_Connect> iCallable_Connect,
	const string& iMethod, string& ioURL, const Map* iHeader,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
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
			if (not theEP)
				break;

			int32 theResponseCode;
			Map theHeader;
			if (not spRequest
				(theEP->GetStreamW(), theEP->GetStreamR(),
				iMethod, theHost, thePath, iHeader,
				true,
				&theResponseCode, &theHeader, oRawHeader))
				{
				break;
				}

			if (oResponseCode)
				*oResponseCode = theResponseCode;

			switch (theResponseCode)
				{
				case 200:
					{
					if (oHeader)
						*oHeader = theHeader;

					if ("HEAD" == iMethod)
						return new ZStreamerR_T<ZStreamR_Null>;

					if (ZRef<ZStreamerR> theStreamerR = sMakeContentStreamer(theHeader, theEP))
						return theStreamerR;

					return theEP;
					}
				case 301:
				case 302:
				case 303:
					{
					ioURL = sGetString0(theHeader.Get("location"));
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

// =================================================================================================
// MARK: -

static void spPost_Prefix(const ZStreamW& w,
	const std::string& iMethod,
	const string& iHost, const string& iPath, const Map* iHeader, bool iSendConnectionClose)
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
	if (iHeader)
		sWrite_Header(w, *iHeader);
	}

ZRef<ZStreamerR> sPost_Send(ZRef<Callable_Connect> iCallable_Connect,
	const std::string& iMethod,
	const std::string& iURL, const Map* iHeader, const ZStreamR& iBody)
	{
	string theScheme;
	string theHost;
	ip_port thePort;
	string thePath;
	if (sParseURL(iURL, &theScheme, &theHost, &thePort, &thePath))
		{
		if (ZRef<ZStreamerRWCon> theEP = spConnect(iCallable_Connect, theScheme, theHost, thePort))
			{
			const ZStreamW& w = theEP->GetStreamW();

			spPost_Prefix(w, iMethod, theHost, thePath, iHeader, true);

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
			return theEP;
			}
		}
	return null;
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

ZRef<ZStreamerR> sPost_Receive(const ZRef<ZStreamerR>& iStreamerR,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (iStreamerR)
		{
		int32 theResponseCode;
		Map theHeader;
		if (spPost_Suffix(iStreamerR->GetStreamR(), &theResponseCode, &theHeader, oRawHeader))
			{
			if (oResponseCode)
				*oResponseCode = theResponseCode;

			if (200 == theResponseCode)
				{
				if (oHeader)
					*oHeader = theHeader;

				if (ZRef<ZStreamerR> theStreamerR = sMakeContentStreamer(theHeader, iStreamerR))
					return theStreamerR;

				return iStreamerR;
				}
			}
		}
	return null;
	}

ZRef<ZStreamerR> sPost(ZRef<Callable_Connect> iCallable_Connect,
	const std::string& iURL, const Map* iHeader, const ZStreamR& iBody,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (ZRef<ZStreamerR> theStreamerR = sPost_Send(iCallable_Connect, "POST", iURL, iHeader, iBody))
		return sPost_Receive(theStreamerR, oResponseCode, oHeader, oRawHeader);
	return null;
	}

// =================================================================================================
// MARK: -

bool sCONNECT(const ZStreamR& r, const ZStreamW& w,
	const string& iAddress, const Map* iHeader,
	int32* oResponseCode, Map* oHeader)
	{
	if (oResponseCode)
		*oResponseCode = 0;

	w.WriteString("CONNECT ");
	w.WriteString(iAddress);
	w.WriteString(" HTTP/1.0\r\n");

	if (iHeader)
		sWrite_Header(w, *iHeader);

	w.WriteString("\r\n");

	w.Flush();

	int32 serverResponseCode;
	if (ZHTTP::sReadResponse(ZStreamU_Unreader(r), &serverResponseCode, nullptr))
		{
		if (oResponseCode)
			*oResponseCode = serverResponseCode;

		if (ZHTTP::sReadHeader(r, oHeader))
			return serverResponseCode == 200;
		}

	return false;
	}

} // namespace ZHTTP
} // namespace ZooLib
