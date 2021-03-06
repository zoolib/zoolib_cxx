// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/HTTP/Requests.h"

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Chan_XX_Tee.h"
#include "zoolib/ChanRU_XX_Unreader.h"
#include "zoolib/Channer_XX_Wrapper.h"
#include "zoolib/HTTP.h"
#include "zoolib/HTTP_Content.h" // For ChanW_Bin_Chunked.
#include "zoolib/Log.h"
#include "zoolib/MIME.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_string.h"

namespace ZooLib {
namespace HTTP {

using std::pair;

// =================================================================================================
#pragma mark - Helpers (anonymous)

namespace { // anonymous

bool spQReadResponse(const ChanR_Bin& iChanR, int32* oResponseCode, Map* oHeader)
	{
	MIME::ChanR_Bin_Header theSIH_Server(iChanR);
	ChanRU_XX_Unreader<byte> theChanRU(theSIH_Server);

	if (not sQReadResponse(theChanRU, oResponseCode, nullptr))
		return false;

	return sQReadHeader(theChanRU, oHeader);
	}

pair<uint16,bool> spGetPortAndSSL(const string& iScheme, const string& iHost, uint16 iPort)
	{
	const bool useSSL = Util_string::sEquali("https", iScheme)
		|| Util_string::sEquali("wss", iScheme);

	uint16 thePort = iPort;
	if (not thePort)
		{
		if (useSSL)
			thePort = 443;
		else
			thePort = 80;
		}
	return pair<uint16,bool>(thePort, useSSL);
	}

ZP<ChannerRWClose_Bin> spConnect(ZP<Callable_Connect> iCallable_Connect,
	const string& iScheme, const string& iHost, uint16 iPort)
	{
	const pair<uint16,bool> thePortAndSSL = spGetPortAndSSL(iScheme, iHost, iPort);

	if (iCallable_Connect)
		return iCallable_Connect->Call(iHost, thePortAndSSL.first, thePortAndSSL.second);

	return sConnect(iHost, thePortAndSSL.first, thePortAndSSL.second);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - HTTP::sRequest

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
		ChanR_XX_Tee<ChanR_Bin> theTee(iChanR, theSRWP_Data);
		return spQReadResponse(theTee, oResponseCode, oHeader);
		}
	else
		{
		return spQReadResponse(iChanR, oResponseCode, oHeader);
		}
	}

bool sQRequest(ZP<ChannerRWClose_Bin>& ioConnection,
	ZP<Callable_Connect> iCallable_Connect,
	const string& iMethod, const string& iURL, const Map* iHeader,
	bool iConnectionClose,
	string* oURL, int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	string theURL = iURL;

	for (;;)
		{
		if (oURL)
			*oURL = theURL;

		if (oResponseCode)
			*oResponseCode = 0;

		if (oHeader)
			oHeader->Clear();

		if (oRawHeader)
			oRawHeader->SetSize(0);

		string theScheme;
		string theHost;
		uint16 thePort;
		string thePath;
		if (sParseURL(theURL, &theScheme, &theHost, &thePort, &thePath))
			{
			// If ioConnection is valid, we *assume* that it is a live connection
			// to the server for theURL
			const pair<uint16,bool> thePortAndSSL = spGetPortAndSSL(theScheme, theHost, thePort);
			thePort = thePortAndSSL.first;

			const bool wasExtantConnection = bool(ioConnection);
			if (not wasExtantConnection)
				{
				if (iCallable_Connect)
					ioConnection = iCallable_Connect->Call(theHost, thePort, thePortAndSSL.second);
				else
					ioConnection = sConnect(theHost, thePort, thePortAndSSL.second);
				}

			if (not ioConnection)
				{
				// We have no physical connection, our caller can do any retry it desires.
				return false;
				}

			int32 theResponseCode;
			Map theResponseHeader;
			if (not spQRequest(
				*ioConnection, *ioConnection,
				iMethod, theHost + sStringf(":%d", thePort), thePath, iHeader,
				iConnectionClose,
				&theResponseCode, &theResponseHeader, oRawHeader))
				{
				// We failed to interpret the response. We've thus lost track of our position in any
				// request/response sequence and the connection cannot (continue to) be reused.
				sClear(ioConnection);

				if (wasExtantConnection)
					{
					// The connection was reused, so a prior request had worked. Assume something
					// minor messed up and go around. If the same error recurs wasExtantConnection
					// will be false and we won't enter this branch, so we get one retry for
					// each physical connection.
					if (ZLOGF(w, eDebug))
						w << "Request failed on reused connection, retrying once: " << theURL;
					continue;
					}
				// The connection was fresh, assume the problem is real.
				return false;
				}

			if (oResponseCode)
				*oResponseCode = theResponseCode;

			if (oHeader)
				*oHeader = theResponseHeader;

			switch (theResponseCode)
				{
				case 301:
				case 302:
				case 303:
					{
					theURL = sAbsoluteURI(theURL, sGetString0(theResponseHeader.Get("location")));
					// Read and discard the body
					if (ZP<ChannerR_Bin> theChannerR_Content = sMakeContentChanner(
						iMethod, theResponseCode, theResponseHeader,
						ioConnection))
						{ sSkipAll(*theChannerR_Content); }

					string newScheme;
					string newHost;
					uint16 newPort;
					string newPath;
					if (sParseURL(theURL, &newScheme, &newHost, &newPort, &newPath))
						{
						// If newScheme, newHost and newPort match the prior values, then assume
						// we can keep the connection alive, otherwise we drop it.
						if (newScheme == theScheme and newHost == theHost and newPort == thePort)
							break;
						}
					sClear(ioConnection);
					break;
					}
				default:
					{
					return true;
					}
				}
			}
		}
	}

// =================================================================================================
#pragma mark - HTTP::sPOST

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

ZP<ChannerRWClose_Bin> sPOST_Send(ZP<Callable_Connect> iCallable_Connect,
	const string& iMethod,
	const string& iURL, const Map* iHeader, const ChanR_Bin& iBody, ZQ<uint64> iBodyCountQ)
	{
	string theScheme;
	string theHost;
	uint16 thePort;
	string thePath;
	if (sParseURL(iURL, &theScheme, &theHost, &thePort, &thePath))
		{
		if (ZP<ChannerRWClose_Bin> theConn = spConnect(iCallable_Connect, theScheme, theHost, thePort))
			{
			const ChanW_Bin& theChanW = *theConn;

			spPOST_Prefix(theChanW, iMethod, theHost, thePath, iHeader, true);

			if (iBodyCountQ)
				{
				sEWritef(theChanW, "Content-Length: %lld\r\n", *iBodyCountQ);
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
			return theConn;
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
		ChanR_XX_Tee<ChanR_Bin> theTee(iChanR, theSRWP_Data);
		return spQReadResponse(theTee, oResponseCode, oHeader);
		}
	else
		{
		return spQReadResponse(iChanR, oResponseCode, oHeader);
		}
	}

ZP<ChannerRWClose_Bin> sPOST_Receive(ZP<ChannerRWClose_Bin> iConn,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (iConn)
		{
		int32 theResponseCode;
		Map theResponseHeader;
		if (spQPOST_Suffix(*iConn,
			&theResponseCode, &theResponseHeader, oRawHeader))
			{
			if (oResponseCode)
				*oResponseCode = theResponseCode;

			if (theResponseCode >= 200 && theResponseCode < 300)
				{
				if (oHeader)
					*oHeader = theResponseHeader;

				if (ZP<ChannerR_Bin> theChanner =
					sMakeContentChanner(theResponseHeader, iConn))
					{
					return sChanner_Wrapper<byte>(theChanner, iConn, iConn);
					}

				return iConn;
				}
			}
		}
	return null;
	}

ZP<ChannerRWClose_Bin> sPOST(ZP<Callable_Connect> iCallable_Connect,
	const string& iURL, const Map* iHeader, const ChanR_Bin& iBody, ZQ<uint64> iBodyCountQ,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (ZP<ChannerRWClose_Bin> theConn = sPOST_Send(iCallable_Connect,
		"POST", iURL, iHeader, iBody, iBodyCountQ))
		{
		return sPOST_Receive(theConn, oResponseCode, oHeader, oRawHeader);
		}
	return null;
	}

// =================================================================================================
#pragma mark - sConnect

bool sQCONNECT(const ChanR_Bin& r, const ChanW_Bin& w,
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
	ChanRU_XX_Unreader<byte> theChanRU(r);
	if (sQReadResponse(theChanRU, &serverResponseCode, nullptr))
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
