/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZStreamerRWConFactory_Proxy_HTTPConnect.h"

#include "zoolib/ZHTTP.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWConFactory_Proxy_HTTPConnect

ZStreamerRWConFactory_Proxy_HTTPConnect::ZStreamerRWConFactory_Proxy_HTTPConnect(
	ZRef<ZStreamerRWConFactory> iProxyFactory,
	const std::string& iHost, uint16 iPort)
:	fProxyFactory(iProxyFactory)
,	fHost(iHost)
,	fPort(iPort)
	{}

ZStreamerRWConFactory_Proxy_HTTPConnect::~ZStreamerRWConFactory_Proxy_HTTPConnect()
	{}

ZRef<ZStreamerRWCon> ZStreamerRWConFactory_Proxy_HTTPConnect::MakeStreamerRWCon()
	{
	if (ZRef<ZStreamerRWCon> theSRWCon = fProxyFactory->MakeStreamerRWCon())
		{
		const ZStreamW& theStreamW = theSRWCon->GetStreamW();
		theStreamW.WriteString("CONNECT ");
		theStreamW.WriteString(fHost);
		theStreamW.WriteString(":");
		theStreamW.Writef("%d", fPort);
		theStreamW.WriteString(" HTTP/1.0\r\n");

//		theStreamW.WriteString("User-Agent: ZStreamerRWConFactory_Proxy_HTTPConnect\r\n");

//		theStreamW.WriteString("Host:");
//		theStreamW.WriteString(fHost);
//		theStreamW.WriteString("\r\n");
		
//		theStreamW.WriteString("Content-Length: 0\r\n");
//		theStreamW.WriteString("Proxy-Connection: Keep-Alive\r\n");
//		theStreamW.WriteString("Pragma: no-cache\r\n");

		theStreamW.WriteString("\r\n");

		theStreamW.Flush();

		const ZStreamR& theStreamR = theSRWCon->GetStreamR();
		int32 serverResultCode;
		if (ZHTTP::sReadResponse(ZStreamU_Unreader(theStreamR), &serverResultCode, nullptr))
			{
			if (serverResultCode == 200)
				{
				if (ZHTTP::sReadHeader(theStreamR, nullptr))
					return theSRWCon;
				}
			}

		theSRWCon->Disconnect();
		}
	return null;
	}

} // namespace ZooLib
