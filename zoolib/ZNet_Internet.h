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

#ifndef __ZNet_Internet__
#define __ZNet_Internet__ 1
#include "zconfig.h"

#include "zoolib/ZMulti_T.h"
#include "zoolib/ZNet.h"
#include "zoolib/ZStdInt.h"

#include <string>

namespace ZooLib {

// =================================================================================================

typedef uint16 ip_port;

typedef uint32 ip4_addr;
typedef ip4_addr ip_addr;

struct ip6_addr
	{
	union
		{
		uint8 fAs8[16];
		uint16 fAs16[8];
		uint32 fAs32[4];
		uint64 fAs64[2];
		};
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress_Internet

class ZNetAddress_Internet
:	public ZNetAddress
	{
protected:
	ZNetAddress_Internet(ip_port iPort);

// Our protocol
	ip_port GetPort() const;

protected:
	const ip_port fPort;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress_IP4

class ZNetAddress_IP4
:	public ZNetAddress_Internet
	{
public:
	ZNetAddress_IP4(ip4_addr iAddr, ip_port iPort);
	ZNetAddress_IP4(uint8 iAddr0, uint8 iAddr1, uint8 iAddr2, uint8 iAddr3, ip_port iPort);

// From ZNetAddress
	virtual ZRef<ZNetEndpoint> Connect() const;

// Our protocol
	ip4_addr GetAddr() const;

	static const ip4_addr sLoopback = 0x7F000001u;
	static const ip4_addr sAny = 0;

	static std::string sAsString(ip4_addr iAddr);
	static ip4_addr sAddr(uint8 iAddr0, uint8 iAddr1, uint8 iAddr2, uint8 iAddr3);

private:
	const ip4_addr fAddr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress_IP6

class ZNetAddress_IP6
:	public ZNetAddress_Internet
	{
public:
	ZNetAddress_IP6(ip_port iPort, ip6_addr iAddr);

// From ZNetAddress
	virtual ZRef<ZNetEndpoint> Connect() const;

// Our protocol
	const ip6_addr& GetAddr() const;

	static const ip6_addr sLoopback;
	static const ip6_addr sAny;

private:
	const ip6_addr fAddr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetName_Internet

class ZNetName_Internet
:	public ZNetName
	{
public:
	typedef ZMulti_T3<std::string, ip_port, size_t> LookupParam_t;

	ZNetName_Internet(const std::string& iName, ip_port iPort);
	virtual ~ZNetName_Internet();

// From ZNetName
	virtual std::string AsString() const;
	virtual ZRef<ZNetNameLookup> CreateLookup(size_t iMaxAddresses) const;

// Our protocol
	const std::string& GetName() const;
	ip_port GetPort() const;

private:
	const std::string fName;
	const ip_port fPort;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP

class ZNetListener_TCP
:	public virtual ZNetListener
	{
public:
	typedef ZMulti_T1<ip_port> MakeParam_t;
	typedef ZMulti_T2<ip4_addr, ip_port> MakeParam4_t;
	typedef ZMulti_T2<ip6_addr, ip_port> MakeParam6_t;

	virtual ip_port GetPort() = 0;

	static ZRef<ZNetListener_TCP> sCreate(ip_port iPort);
	static ZRef<ZNetListener_TCP> sCreate(ip4_addr iAddress, ip_port iPort);
	static ZRef<ZNetListener_TCP> sCreate(ip6_addr iAddress, ip_port iPort);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP

class ZNetEndpoint_TCP
:	public virtual ZNetEndpoint
	{
public:
	typedef ZMulti_T2<ip4_addr, ip_port> MakeParam4_t;
	typedef ZMulti_T2<ip6_addr, ip_port> MakeParam6_t;

	static ZRef<ZNetEndpoint_TCP> sCreateConnected(ip4_addr iRemoteAddr, ip_port iRemotePort);
	static ZRef<ZNetEndpoint_TCP> sCreateConnected(ip6_addr iRemoteAddr, ip_port iRemotePort);
	};

} // namespace ZooLib

#endif // __ZNet_Internet__
