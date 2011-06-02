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

#include "zoolib/ZStreamerRWConFactory_Proxy_SOCKS.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWConFactory_Proxy_SOCKS

ZStreamerRWConFactory_Proxy_SOCKS::ZStreamerRWConFactory_Proxy_SOCKS(
	ZRef<ZStreamerRWConFactory> iProxyFactory,
	const std::string& iHost, uint16 iPort)
:	fProxyFactory(iProxyFactory)
,	fHost(iHost)
,	fPort(iPort)
	{}

ZStreamerRWConFactory_Proxy_SOCKS::~ZStreamerRWConFactory_Proxy_SOCKS()
	{}

ZRef<ZStreamerRWCon> ZStreamerRWConFactory_Proxy_SOCKS::MakeStreamerRWCon()
	{
	if (ZRef<ZStreamerRWCon> theSRWCon = fProxyFactory->MakeStreamerRWCon())
		{
		const ZStreamW& theStreamW = theSRWCon->GetStreamW();
		theStreamW.WriteUInt8(4); // Version, use 4 for now
		theStreamW.WriteUInt8(1); // Open TCP connection
		theStreamW.WriteUInt16(fPort);
		theStreamW.WriteUInt32(1); // 0.0.0.1, indicates that host name will follow

		// user ID, terminated by zero
		theStreamW.WriteUInt8(0);

		theStreamW.WriteString(fHost); // Host name
		theStreamW.WriteUInt8(0);

		theStreamW.Flush();

		const ZStreamR& theStreamR = theSRWCon->GetStreamR();
		theStreamR.ReadUInt8(); // null byte
		const uint8 status = theStreamR.ReadUInt8();
		/*const uint16 port = */theStreamR.ReadUInt16();
		/*const uint16 address = */theStreamR.ReadUInt32();
		
		if (status == 0x5a)
			return theSRWCon;

		theSRWCon->Disconnect();
		}
	return null;
	}

} // namespace ZooLib
