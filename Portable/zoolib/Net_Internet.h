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

#ifndef __ZooLib_Net_Internet_h__
#define __ZooLib_Net_Internet_h__ 1
#include "zconfig.h"

#include "zoolib/FunctionChain.h"
#include "zoolib/Multi.h"
#include "zoolib/Net.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZStdInt.h" // For uint16, uint32, size_t

#include <string>

namespace ZooLib {

// =================================================================================================

typedef uint16 ip_port;

typedef uint32 ip4_addr;

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

typedef Multi1<ip_port> MakeParam_t;
typedef Multi2<ip4_addr, ip_port> MakeParam4_t;
typedef Multi2<ip6_addr, ip_port> MakeParam6_t;

// =================================================================================================
#pragma mark - NetAddress_Internet

class NetAddress_Internet
:	public NetAddress
	{
protected:
	NetAddress_Internet(ip_port iPort);

public:
// Our protocol
	ip_port GetPort();

protected:
	const ip_port fPort;
	};

// =================================================================================================
#pragma mark - NetAddress_IP4

class NetAddress_IP4
:	public NetAddress_Internet
	{
public:
	NetAddress_IP4(ip4_addr iAddr, ip_port iPort);
	NetAddress_IP4(uint8 iAddr0, uint8 iAddr1, uint8 iAddr2, uint8 iAddr3, ip_port iPort);

// From NetAddress
	virtual ZP<ChannerRWClose_Bin> Connect();

// Our protocol
	ip4_addr GetAddr();

	static const ip4_addr sLoopback = 0x7F000001u;
	static const ip4_addr sAny = 0;
	static const ip4_addr sNone = 0xFFFFFFFFu;

	static ZQ<ip4_addr> sQFromString(const std::string& iString);
	static std::string sAsString(ip4_addr iAddr);
	static ip4_addr sAddr(uint8 iAddr0, uint8 iAddr1, uint8 iAddr2, uint8 iAddr3);

private:
	const ip4_addr fAddr;
	};

// =================================================================================================
#pragma mark - NetAddress_IP6

class NetAddress_IP6
:	public NetAddress_Internet
	{
public:
	NetAddress_IP6(ip_port iPort, ip6_addr iAddr);

// From NetAddress
	virtual ZP<ChannerRWClose_Bin> Connect();

// Our protocol
	ip6_addr GetAddr();

	static const ip6_addr sLoopback;
	static const ip6_addr sAny;

private:
	const ip6_addr fAddr;
	};

// =================================================================================================
#pragma mark - NetName_Internet

class NetName_Internet
:	public NetName
	{
public:
	typedef Multi3<std::string, ip_port, size_t> LookupParam_t;

	NetName_Internet(const std::string& iName, ip_port iPort);
	virtual ~NetName_Internet();

// From NetName
	virtual std::string AsString();
	virtual ZP<NetNameLookup> MakeLookup(size_t iMaxAddresses);

// Our protocol
	std::string GetName() const;
	ip_port GetPort() const;

private:
	const std::string fName;
	const ip_port fPort;
	};

// =================================================================================================
#pragma mark - NetListener_TCP

class NetListener_TCP
:	public virtual NetListener
	{
public:
	virtual ip_port GetPort() = 0;
	};

ZP<NetListener_TCP> sNetListener_TCP(ip_port iPort);
ZP<NetListener_TCP> sNetListener_TCP(ip4_addr iAddress, ip_port iPort);
ZP<NetListener_TCP> sNetListener_TCP(ip6_addr iAddress, ip_port iPort);

// =================================================================================================
#pragma mark - sQConnect_TCP

ZP<ChannerRWClose_Bin> sQConnect_TCP(ip4_addr iRemoteAddr, ip_port iRemotePort);
ZP<ChannerRWClose_Bin> sQConnect_TCP(ip6_addr iRemoteAddr, ip_port iRemotePort);

} // namespace ZooLib

#endif // __ZooLib_Net_Internet_h__
