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
#ifndef __ZNet_Internet_Socket__
#define __ZNet_Internet_Socket__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZNet_Internet.h"
#include "zoolib/ZNet_Socket.h"

#ifndef ZCONFIG_API_Avail__Net_Internet_Socket
#	if ZCONFIG_API_Enabled(Net_Socket)
#		define ZCONFIG_API_Avail__Net_Internet_Socket 1
#	endif
#endif

#ifndef ZCONFIG_API_Avail__Net_Internet_Socket
#	define ZCONFIG_API_Avail__Net_Internet_Socket 0
#endif

#ifndef ZCONFIG_API_Desired__Net_Internet_Socket
#	define ZCONFIG_API_Desired__Net_Internet_Socket 1
#endif

#if ZCONFIG_API_Enabled(Net_Internet_Socket)

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZNet_TCP_Socket

namespace ZNet_TCP_Socket {

int sListen(ip4_addr iLocalAddress, ip_port iLocalPort);
int sListen(ip6_addr iLocalAddress, ip_port iLocalPort);

} // namespace ZNet_TCP_Socket

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Internet_Socket

class ZNetNameLookup_Internet_Socket : public ZNetNameLookup
	{
public:
	ZNetNameLookup_Internet_Socket(const std::string& iName, ip_port iPort, size_t iMaxAddresses);
	virtual ~ZNetNameLookup_Internet_Socket();

// From ZNetNameLookup
	virtual void Start();
	virtual bool Finished();
	virtual void Advance();

	virtual ZRef<ZNetAddress> CurrentAddress();
	virtual ZRef<ZNetName> CurrentName();

protected:
	const std::string fName;
	const ip_port fPort;
	const size_t fCountAddressesToReturn;
	bool fStarted;
	size_t fCurrentIndex;
	std::vector<ZRef<ZNetAddress_Internet> > fAddresses;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP_Socket

class ZNetListener_TCP_Socket
:	public ZNetListener_TCP,
	public ZNetListener_Socket
	{
	ZNetListener_TCP_Socket(int iFD, const IKnowWhatIAmDoing_t&);
public:
	static ZRef<ZNetListener_TCP_Socket> sCreateWithFD(int iFD);

	ZNetListener_TCP_Socket(ip_port iLocalPort);
	ZNetListener_TCP_Socket(ip4_addr iLocalAddress, ip_port iLocalPort);
	ZNetListener_TCP_Socket(ip6_addr iLocalAddress, ip_port iLocalPort);

	virtual ~ZNetListener_TCP_Socket();

// From ZNetListener via ZNetListener_TCP
	virtual ZRef<ZNetAddress> GetAddress();

// From ZNetListener_TCP
	virtual ip_port GetPort();

// From ZNetListener_Socket
	virtual ZRef<ZNetEndpoint> Imp_MakeEndpoint(int iSocketFD);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP_Socket

class ZNetEndpoint_TCP_Socket
:	public ZNetEndpoint_TCP,
	public ZNetEndpoint_Socket
	{
public:
	ZNetEndpoint_TCP_Socket(int iSocketFD);

	ZNetEndpoint_TCP_Socket
		(ip4_addr iLocalHost, ip_port iLocalPort, ip4_addr iRemoteHost, ip_port iRemotePort);

	ZNetEndpoint_TCP_Socket(ip4_addr iRemoteHost, ip_port iRemotePort);
	ZNetEndpoint_TCP_Socket(ip6_addr iRemoteHost, ip_port iRemotePort);

	virtual ~ZNetEndpoint_TCP_Socket();

// From ZNetEndpoint via ZNetEndpoint_TCP
	virtual ZRef<ZNetAddress> GetLocalAddress();
	virtual ZRef<ZNetAddress> GetRemoteAddress();
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Internet_Socket)

#endif // __ZNet_Internet_Socket__
