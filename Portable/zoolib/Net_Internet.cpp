// Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/Net_Internet.h"

#include "zoolib/Stringf.h"

#if ZCONFIG_SPI_Enabled(Win)
	#include <winsock.h>
#else
	#include <arpa/inet.h> // For inet_addr
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - NetAddress_Internet

NetAddress_Internet::NetAddress_Internet(ip_port iPort)
:	fPort(iPort)
	{}

ip_port NetAddress_Internet::GetPort()
	{ return fPort; }

// =================================================================================================
#pragma mark - NetAddress_IP4

NetAddress_IP4::NetAddress_IP4(ip4_addr iAddr, ip_port iPort)
:	NetAddress_Internet(iPort)
,	fAddr(iAddr)
	{}

NetAddress_IP4::NetAddress_IP4(
	uint8 iAddr0, uint8 iAddr1, uint8 iAddr2, uint8 iAddr3, ip_port iPort)
:	NetAddress_Internet(iPort)
,	fAddr(sAddr(iAddr0, iAddr1, iAddr2, iAddr3))
	{}

ZP<ChannerRWCon_Bin> NetAddress_IP4::Connect()
	{ return sConnect_TCP(fAddr, fPort); }

ip4_addr NetAddress_IP4::GetAddr()
	{ return fAddr; }

ZQ<ip4_addr> NetAddress_IP4::sQFromString(const std::string& iString)
	{
	const ip4_addr theAddr = ntohl(inet_addr(iString.c_str()));
	if (theAddr == INADDR_NONE)
		return null;
	return theAddr;
	}

std::string NetAddress_IP4::sAsString(ip4_addr iAddr)
	{
	return sStringf("%d.%d.%d.%d",
		int((iAddr >> 24) & 0xFF),
		int((iAddr >> 16) & 0xFF),
		int((iAddr >> 8) & 0xFF),
		int(iAddr & 0xFF));
	}

ip4_addr NetAddress_IP4::sAddr(uint8 iAddr0, uint8 iAddr1, uint8 iAddr2, uint8 iAddr3)
	{ return (iAddr0 << 24) | (iAddr1 << 16) | (iAddr2 << 8) | iAddr3; }

// =================================================================================================
#pragma mark - NetAddress_IP6

NetAddress_IP6::NetAddress_IP6(ip_port iPort, ip6_addr iAddr)
:	NetAddress_Internet(iPort)
,	fAddr(iAddr)
	{}

ZP<ChannerRWCon_Bin> NetAddress_IP6::Connect()
	{ return sConnect_TCP(fAddr, fPort); }

ip6_addr NetAddress_IP6::GetAddr()
	{ return fAddr; }

const ip6_addr NetAddress_IP6::sLoopback = {};

const ip6_addr NetAddress_IP6::sAny =
	{{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}};

// =================================================================================================
#pragma mark - NetName_Internet

NetName_Internet::NetName_Internet(const std::string& iName, ip_port iPort)
:	fName(iName)
,	fPort(iPort)
	{}

NetName_Internet::~NetName_Internet()
	{}

std::string NetName_Internet::AsString()
	{ return fName + sStringf(":%d", fPort); }

ZP<NetNameLookup> NetName_Internet::MakeLookup(size_t iMaxAddresses)
	{
	return FunctionChain<ZP<NetNameLookup>, LookupParam_t>
		::sInvoke(LookupParam_t(fName, fPort, iMaxAddresses));
	}

std::string NetName_Internet::GetName() const
	{ return fName; }

ip_port NetName_Internet::GetPort() const
	{ return fPort; }

// =================================================================================================
#pragma mark - NetListener_TCP

ZP<NetListener_TCP> sNetListener_TCP(ip_port iPort)
	{
	return FunctionChain<ZP<NetListener_TCP>, MakeParam_t>
		::sInvoke(MakeParam_t(iPort));
	}

ZP<NetListener_TCP> sNetListener_TCP(ip4_addr iAddress, ip_port iPort)
	{
	return FunctionChain<ZP<NetListener_TCP>, MakeParam4_t>
		::sInvoke(MakeParam4_t(iAddress, iPort));
	}

ZP<NetListener_TCP> sNetListener_TCP(ip6_addr iAddress, ip_port iPort)
	{
	return FunctionChain<ZP<NetListener_TCP>, MakeParam6_t>
		::sInvoke(MakeParam6_t(iAddress, iPort));
	}

// =================================================================================================
#pragma mark - NetEndpoint_TCP

ZP<ChannerRWCon_Bin> sConnect_TCP(ip4_addr iRemoteAddr, ip_port iRemotePort)
	{
	return FunctionChain<ZP<ChannerRWCon_Bin>, MakeParam4_t>
		::sInvoke(MakeParam4_t(iRemoteAddr, iRemotePort));
	}

ZP<ChannerRWCon_Bin> sConnect_TCP(ip6_addr iRemoteAddr, ip_port iRemotePort)
	{
	return FunctionChain<ZP<ChannerRWCon_Bin>, MakeParam6_t>
		::sInvoke(MakeParam6_t(iRemoteAddr, iRemotePort));
	}

} // namespace ZooLib
