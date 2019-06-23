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

#ifndef __ZooLib_POSIX_Net_Internet_Socket_h__
#define __ZooLib_POSIX_Net_Internet_Socket_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/Net_Internet.h"
#include "zoolib/POSIX/Net_Socket.h"

#ifndef ZCONFIG_API_Avail__Net_Internet_Socket
	#if ZCONFIG_API_Enabled(Net_Socket)
		#define ZCONFIG_API_Avail__Net_Internet_Socket 1
	#endif
#endif

#ifndef ZCONFIG_API_Avail__Net_Internet_Socket
	#define ZCONFIG_API_Avail__Net_Internet_Socket 0
#endif

#ifndef ZCONFIG_API_Desired__Net_Internet_Socket
	#define ZCONFIG_API_Desired__Net_Internet_Socket 1
#endif

#if ZCONFIG_API_Enabled(Net_Internet_Socket)

ZMACRO_MSVCStaticLib_Reference(Net_Internet_Socket)

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - Net_TCP_Socket

namespace Net_TCP_Socket {

int sListen(ip4_addr iLocalAddress, ip_port iLocalPort);
int sListen(ip6_addr iLocalAddress, ip_port iLocalPort);

} // namespace Net_TCP_Socket

// =================================================================================================
#pragma mark - NetNameLookup_Internet_Socket

class NetNameLookup_Internet_Socket
:	public NetNameLookup
	{
public:
	NetNameLookup_Internet_Socket(const std::string& iName, ip_port iPort, size_t iMaxAddresses);
	virtual ~NetNameLookup_Internet_Socket();

// From NetNameLookup
	virtual void Start();
	virtual bool Finished();
	virtual void Advance();

	virtual ZP<NetAddress> CurrentAddress();
	virtual ZP<NetName> CurrentName();

protected:
	const std::string fName;
	const ip_port fPort;
	const size_t fCountAddressesToReturn;
	bool fStarted;
	size_t fCurrentIndex;
	std::vector<ZP<NetAddress_Internet>> fAddresses;
	};

// =================================================================================================
#pragma mark - NetListener_TCP_Socket

class NetListener_TCP_Socket
:	public virtual NetListener_TCP
,	public virtual NetListener_Socket
	{
	NetListener_TCP_Socket(int iFD, const IKnowWhatIAmDoing_t&);
public:
	NetListener_TCP_Socket(ip_port iLocalPort);
	NetListener_TCP_Socket(ip4_addr iLocalAddress, ip_port iLocalPort);
	NetListener_TCP_Socket(ip6_addr iLocalAddress, ip_port iLocalPort);

	virtual ~NetListener_TCP_Socket();

// From NetListener via NetListener_TCP
	virtual ZP<NetAddress> GetAddress();

// From NetListener_TCP
	virtual ip_port GetPort();
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Internet_Socket)

#endif // __ZooLib_POSIX_Net_Internet_Socket_h__
