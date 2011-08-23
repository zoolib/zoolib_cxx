/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZNet_Internet_WinSock__
#define __ZNet_Internet_WinSock__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Net_Internet_WinSock
#	define ZCONFIG_API_Avail__Net_Internet_WinSock ZCONFIG_SPI_Enabled(Win)
#endif

#ifndef ZCONFIG_API_Desired__Net_Internet_WinSock
#	define ZCONFIG_API_Desired__Net_Internet_WinSock 1
#endif

#include "zoolib/ZNet_Internet.h"

#if ZCONFIG_API_Enabled(Net_Internet_WinSock)

#include "zoolib/ZCompat_MSVCStaticLib.h"
ZMACRO_MSVCStaticLib_Reference(Net_Internet_WinSock)

#include <vector>

#include <winsock.h>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZNet_Internet_WinSock

class ZNet_Internet_WinSock
	{
public:
	static ZNet::Error sTranslateError(int inNativeError);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Internet_WinSock

class ZNetNameLookup_Internet_WinSock : public ZNetNameLookup
	{
public:
	ZNetNameLookup_Internet_WinSock
		(const std::string& inName, ip_port inPort, size_t inMaxAddresses);
	virtual ~ZNetNameLookup_Internet_WinSock();

// From ZNetNameLookup
	virtual void Start();
	virtual bool Finished();
	virtual void Advance();
	virtual ZRef<ZNetAddress> CurrentAddress();
	virtual ZRef<ZNetName> CurrentName();

protected:
	std::string fName;
	ip_port fPort;
	bool fStarted;
	size_t fCountAddressesToReturn;
	size_t fCurrentIndex;
	std::vector<ip4_addr> fAddresses;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP_WinSock

class ZNetListener_TCP_WinSock : public ZNetListener_TCP, private ZNet_Internet_WinSock
	{
public:
	ZNetListener_TCP_WinSock(ip_port iLocalPort);
	ZNetListener_TCP_WinSock(ip4_addr iLocalAddress, ip_port iLocalPort);
	virtual ~ZNetListener_TCP_WinSock();

// From ZNetListener via ZNetListener_TCP
	virtual ZRef<ZNetAddress> GetAddress();
	virtual ZRef<ZNetEndpoint> Listen();
	virtual void CancelListen();

// From ZNetListener_TCP
	virtual ip_port GetPort();

protected:
	void pInit(ip4_addr iLocalAddress, ip_port iLocalPort);

	SOCKET fSOCKET;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP_WinSock

class ZNetEndpoint_TCP_WinSock
:	public ZNetEndpoint_TCP,
	private ZStreamRCon,
	private ZStreamWCon,
	private ZNet_Internet_WinSock
	{
public:
	ZNetEndpoint_TCP_WinSock(SOCKET iSOCKET);
	ZNetEndpoint_TCP_WinSock(ip4_addr iRemoteHost, ip_port iRemotePort);
	ZNetEndpoint_TCP_WinSock
		(ip4_addr iLocalHost, ip_port iLocalPort, ip4_addr iRemoteHost, ip_port iRemotePort);
	virtual ~ZNetEndpoint_TCP_WinSock();

// From ZStreamerRCon via ZNetEndpoint_TCP
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZNetEndpoint_TCP
	virtual const ZStreamWCon& GetStreamWCon();

// From ZNetEndpoint via ZNetEndpoint_TCP
	virtual ZRef<ZNetAddress> GetLocalAddress();
	virtual ZRef<ZNetAddress> GetRemoteAddress();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(double iTimeout);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	SOCKET fSOCKET;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Internet_WinSock)

#endif // __ZNet_Internet_WinSock__
