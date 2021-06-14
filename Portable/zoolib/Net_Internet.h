// Copyright (c) 2001-2007 Andrew Green and Learning in Motion, Inc.
// Copyright (c) 2008-2020 Andrew Green.
// MIT License. http://www.zoolib.org/

#ifndef __ZooLib_Net_Internet_h__
#define __ZooLib_Net_Internet_h__ 1
#include "zconfig.h"

#include "zoolib/FunctionChain.h"
#include "zoolib/Multi.h"
#include "zoolib/Net.h"
#include "zoolib/StdInt.h" // For uint16, uint32, size_t

#include "zoolib/ZQ.h"

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
#pragma mark - sConnect_TCP

ZP<ChannerRWClose_Bin> sConnect_TCP(ip4_addr iRemoteAddr, ip_port iRemotePort);
ZP<ChannerRWClose_Bin> sConnect_TCP(ip6_addr iRemoteAddr, ip_port iRemotePort);

} // namespace ZooLib

#endif // __ZooLib_Net_Internet_h__
