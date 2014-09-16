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

#include "zoolib/ChanR_XX_More.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Chan_XX_Unreader.h"
#include "zoolib/Chan_XX_Tee.h"
#include "zoolib/HTTP_Requests.h"
#include "zoolib/MIME.h"

#include "zoolib/ZNet_Internet.h"
#include "zoolib/ZStreamerRWCon_SSL.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_string.h"

//#include "zoolib/ZStream_String.h"
//#include "zoolib/ZStreamR_Boundary.h"
//#include "zoolib/ZStreamR_SkipAllOnDestroy.h"
//#include "zoolib/ZStrim_Stream.h"
//#include "zoolib/ZStrimmer.h"
//#include "zoolib/ZStrimmer_Stream.h"
//#include "zoolib/ZStrimmer_Streamer.h"
//#include "zoolib/ZTextCoder.h"
//#include "zoolib/ZUtil_Stream_Operators.h"
//
//#include <ctype.h> // For isalnum

namespace ZooLib {
namespace HTTP {

// =================================================================================================
// MARK: - Helpers (anonymous)

namespace { // anonymous

static bool spQReadResponse(const ChanR_Bin& iChanR, int32* oResponseCode, Map* oHeader)
	{
	MIME::ChanR_Bin_Header theSIH_Server(iChanR);
	Chan_XX_Unreader<byte> theChanU(theSIH_Server);

	string serverResultMessage;
	if (not sQReadResponse(theChanU, theChanU, oResponseCode, nullptr))
		return false;

	return sQReadHeader(theChanU, oHeader);
	}

static
ZQ<Connection_t> spQConnect(ZRef<Callable_Connect> iCallable_Connect,
	const string& iScheme, const string& iHost, ip_port iPort)
	{
	const bool useSSL = ZUtil_string::sEquali("https", iScheme)
		|| ZUtil_string::sEquali("wss", iScheme);

	ip_port thePort = iPort;
	if (not thePort)
		{
		if (useSSL)
			thePort = 443;
		else
			thePort = 80;
		}

	if (iCallable_Connect)
		return iCallable_Connect->Call(iHost, thePort, useSSL);

	return sQChanner(iHost, thePort, useSSL);
	}

} // anonymous namespace

// =================================================================================================
// MARK: - HTTP::sChanner

class ChannerClose_RWCon
:	public ChannerClose
,	public ChanClose
	{
public:
	ChannerClose_RWCon(const ZRef<ZStreamerWCon>& iSWCon)
	:	fSWCon(iSWCon)
		{}

	virtual void GetChan(const ChanClose*& oChanPtr)
		{ oChanPtr = this; }

	virtual void Close()
		{ fSWCon->GetStreamWCon().SendDisconnect(); }

	const ZRef<ZStreamerWCon> fSWCon;
	};

static
ZRef<ChannerClose> spChannerClose_RWCon(const ZRef<ZStreamerWCon>& iSWCon)
	{ return new ChannerClose_RWCon(iSWCon); }

ZQ<Connection_t> sQChanner(const string& iHost, uint16 iPort, bool iUseSSL)
	{
	if (ZRef<ZStreamerRWCon> theEP = ZNetName_Internet(iHost, iPort).Connect(10))
		{
		if (iUseSSL)
			theEP = sStreamerRWCon_SSL(theEP, theEP);

		Connection_t theC(ZRef<ChannerR_Bin>(theEP), ZRef<ChannerW_Bin>(theEP), spChannerClose_RWCon(theEP));
		return theC;
		}
	return null;
	}

// =================================================================================================
// MARK: - HTTP::sRequest

static bool spQRequest(const ChanR_Bin& iChanR, const ChanW_Bin& iChanW,
	const string& iMethod, const string& iHost, const string& iPath, const Map* iHeader,
	bool iSendConnectionClose,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	iChanW << iMethod << " " << iPath << " HTTP/1.1\r\n" << "Host: " << iHost << "\r\n";

	if (iHeader)
		sWrite_Header(*iHeader, iChanW);

	if (iSendConnectionClose)
		iChanW << "Connection: close\r\n";

	iChanW << "\r\n";

	sFlush(iChanW);

	if (oRawHeader)
		{
		ChanRWPos_Bin_Data<Data> theSRWP_Data(oRawHeader);
		ChanR_XX_Tee<byte> theTee(iChanR, theSRWP_Data);
		return spQReadResponse(theTee, oResponseCode, oHeader);
		}
	else
		{
		return spQReadResponse(iChanR, oResponseCode, oHeader);
		}
	}


ZQ<Connection_t> sQRequest(const ZRef<Callable_Connect>& iCallable_Connect,
	const string& iMethod, const string& iURL, const Map* iHeader,
	bool iConnectionClose,
	string* oURL, int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	string theURL = iURL;

	for (;;)
		{
		if (oURL)
			*oURL = theURL;

		if (oRawHeader)
			oRawHeader->SetSize(0);

		string theScheme;
		string theHost;
		ip_port thePort;
		string thePath;
		if (sParseURL(theURL, &theScheme, &theHost, &thePort, &thePath))
			{
			ZQ<Connection_t> theConnQ = spQConnect(iCallable_Connect, theScheme, theHost, thePort);
			if (not theConnQ)
				return null;

			int32 theResponseCode;
			Map theResponseHeader;
			if (not spQRequest(
				sGetChan(theConnQ->f0), sGetChan(theConnQ->f1),
				iMethod, theHost + sStringf(":%d", thePort), thePath, iHeader,
				iConnectionClose,
				&theResponseCode, &theResponseHeader, oRawHeader))
				{ return null; }

			if (oResponseCode)
				*oResponseCode = theResponseCode;

			if (oHeader)
				*oHeader = theResponseHeader;

			switch (theResponseCode)
				{
				case 101:
					{
					return theConnQ;
					}
				case 200:
					{
					if ("HEAD" == iMethod)
						theConnQ->f0 = sChanner_T<ChanR_XX_Null<byte> >();

					theConnQ->f0 = sMakeContentChanner(theResponseHeader, theConnQ->f0);

					return theConnQ;
					}
				case 301:
				case 302:
				case 303:
					{
					theURL = sGetString0(theResponseHeader.Get("location"));
					break;
					}
				default:
					{
					return null;
					}
				}
			}
		}
	}

// =================================================================================================
// MARK: - HTTP::sPOST

static void spPOST_Prefix(const ChanW_Bin& w,
	const string& iMethod,
	const string& iHost, const string& iPath, const Map* iHeader, bool iSendConnectionClose)
	{
	w << iMethod << " " << iPath << " HTTP/1.1\r\n" << "Host: " << iHost << "\r\n";
	if (iSendConnectionClose)
		w << "Connection: close\r\n";
	if (iHeader)
		sWrite_Header(*iHeader, w);
	}

ZQ<Connection_t> sQPOST_Send(ZRef<Callable_Connect> iCallable_Connect,
	const string& iMethod,
	const string& iURL, const Map* iHeader, const ChanR_Bin& iBody, ZQ<uint64> iBodyCountQ)
	{
	string theScheme;
	string theHost;
	ip_port thePort;
	string thePath;
	if (sParseURL(iURL, &theScheme, &theHost, &thePort, &thePath))
		{
		if (ZQ<Connection_t> theConnQ = spQConnect(iCallable_Connect, theScheme, theHost, thePort))
			{
			const ChanW_Bin& theChanW = sGetChan(theConnQ->f1);

			spPOST_Prefix(theChanW, iMethod, theHost, thePath, iHeader, true);

			if (iBodyCountQ)
				{
				sWritef(theChanW, "Content-Length: %lld\r\n", *iBodyCountQ);
				theChanW << "\r\n";
				sCopyFully(iBody, theChanW, *iBodyCountQ);
				}
			else
				{
				theChanW << "Transfer-Encoding: chunked\r\n";
				theChanW << "\r\n";
				sCopyAll(iBody, ChanW_Bin_Chunked(16 *1024, theChanW));
				}
			sFlush(theChanW);
			return theConnQ;
			}
		}
	return null;
	}

static bool spQPOST_Suffix(const ChanR_Bin& iChanR,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (oRawHeader)
		{
		ChanRWPos_Bin_Data<Data> theSRWP_Data(oRawHeader);
		ChanR_XX_Tee<byte> theTee(iChanR, theSRWP_Data);
		return spQReadResponse(theTee, oResponseCode, oHeader);
		}
	else
		{
		return spQReadResponse(iChanR, oResponseCode, oHeader);
		}
	}

ZQ<Connection_t> sQPOST_Receive(const ZQ<Connection_t>& iConnQ,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (iConnQ)
		{
		int32 theResponseCode;
		Map theResponseHeader;
		if (spQPOST_Suffix(sGetChan(iConnQ->f0),
			&theResponseCode, &theResponseHeader, oRawHeader))
			{
			if (oResponseCode)
				*oResponseCode = theResponseCode;

			if (200 == theResponseCode)
				{
				if (oHeader)
					*oHeader = theResponseHeader;


				ZQ<Connection_t> theConnQ = iConnQ;
				if (ZRef<ChannerR_Bin> theChanner =
					sMakeContentChanner(theResponseHeader, theConnQ->f0))
					{ theConnQ->f0 = theChanner; }

				return theConnQ;
				}
			}
		}
	return null;
	}

ZQ<Connection_t> sQPOST(ZRef<Callable_Connect> iCallable_Connect,
	const string& iURL, const Map* iHeader, const ChanR_Bin& iBody, ZQ<uint64> iBodyCountQ,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (ZQ<Connection_t> theConnQ = sQPOST_Send(iCallable_Connect,
		"POST", iURL, iHeader, iBody, iBodyCountQ))
		{
		return sQPOST_Receive(theConnQ, oResponseCode, oHeader, oRawHeader);
		}
	return null;
	}

// =================================================================================================
// MARK: - sCONNECT

bool sCONNECT(const ChanR_Bin& r, const ChanW_Bin& w,
	const std::string& iAddress, const Map* iHeader,
	int32* oResponseCode, Map* oHeader)
	{
	if (oResponseCode)
		*oResponseCode = 0;

	w << "CONNECT " << iAddress << " HTTP/1.0\r\n";

	if (iHeader)
		sWrite_Header(*iHeader, w);

	w << "\r\n";

	sFlush(w);

	int32 serverResponseCode;
	Chan_XX_Unreader<byte> theChanU(r);
	if (sQReadResponse(theChanU, theChanU, &serverResponseCode, nullptr))
		{
		if (oResponseCode)
			*oResponseCode = serverResponseCode;

		if (sQReadHeader(r, oHeader))
			return serverResponseCode == 200;
		}

	return false;
	}

} // namespace HTTP
} // namespace ZooLib
