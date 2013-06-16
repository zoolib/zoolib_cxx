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

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZNet_Internet.h"
#include "zoolib/ZStringf.h"

#if ZCONFIG_SPI_Enabled(Win)
	#include <winsock.h>
#else
	#include <arpa/inet.h> // For inet_addr
#endif

using std::string;

namespace ZooLib {

// =================================================================================================
// MARK: - ZNetAddress_Internet

ZNetAddress_Internet::ZNetAddress_Internet(ip_port iPort)
:	fPort(iPort)
	{}

ip_port ZNetAddress_Internet::GetPort() const
	{ return fPort; }

// =================================================================================================
// MARK: - ZNetAddress_IP4

ZNetAddress_IP4::ZNetAddress_IP4(ip4_addr iAddr, ip_port iPort)
:	ZNetAddress_Internet(iPort)
,	fAddr(iAddr)
	{}

ZNetAddress_IP4::ZNetAddress_IP4(
	uint8 iAddr0, uint8 iAddr1, uint8 iAddr2, uint8 iAddr3, ip_port iPort)
:	ZNetAddress_Internet(iPort)
,	fAddr(sAddr(iAddr0, iAddr1, iAddr2, iAddr3))
	{}

ZRef<ZNetEndpoint> ZNetAddress_IP4::Connect() const
	{ return ZNetEndpoint_TCP::sCreateConnected(fAddr, fPort); }

ip4_addr ZNetAddress_IP4::GetAddr() const
	{ return fAddr; }

ZQ<ip4_addr> ZNetAddress_IP4::sFromString(const std::string& iString)
	{
	const ip4_addr theAddr = ntohl(inet_addr(iString.c_str()));
	if (theAddr == INADDR_NONE)
		return null;
	return theAddr;
	}

std::string ZNetAddress_IP4::sAsString(ip4_addr iAddr)
	{
	return sStringf("%d.%d.%d.%d",
		int((iAddr >> 24) & 0xFF),
		int((iAddr >> 16) & 0xFF),
		int((iAddr >> 8) & 0xFF),
		int(iAddr & 0xFF));
	}

ip4_addr ZNetAddress_IP4::sAddr(uint8 iAddr0, uint8 iAddr1, uint8 iAddr2, uint8 iAddr3)
	{ return (iAddr0 << 24) | (iAddr1 << 16) | (iAddr2 << 8) | iAddr3; }

// =================================================================================================
// MARK: - ZNetAddress_IP6

ZNetAddress_IP6::ZNetAddress_IP6(ip_port iPort, ip6_addr iAddr)
:	ZNetAddress_Internet(iPort)
,	fAddr(iAddr)
	{}

ZRef<ZNetEndpoint> ZNetAddress_IP6::Connect() const
	{ return ZNetEndpoint_TCP::sCreateConnected(fAddr, fPort); }

const ip6_addr& ZNetAddress_IP6::GetAddr() const
	{ return fAddr; }

const ip6_addr ZNetAddress_IP6::sLoopback = {{{0}}};

const ip6_addr ZNetAddress_IP6::sAny =
	{{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}};

// =================================================================================================
// MARK: - ZNetName_Internet

ZNetName_Internet::ZNetName_Internet(const string& iName, ip_port iPort)
:	fName(iName)
,	fPort(iPort)
	{}

ZNetName_Internet::~ZNetName_Internet()
	{}

string ZNetName_Internet::AsString() const
	{ return fName + sStringf(":%d", fPort); }

ZRef<ZNetNameLookup> ZNetName_Internet::CreateLookup(size_t iMaxAddresses) const
	{
	return ZFunctionChain_T<ZRef<ZNetNameLookup>, LookupParam_t>
		::sInvoke(LookupParam_t(fName, fPort, iMaxAddresses));
	}

const string& ZNetName_Internet::GetName() const
	{ return fName; }

ip_port ZNetName_Internet::GetPort() const
	{ return fPort; }

// =================================================================================================
// MARK: - ZNetListener_TCP

ZRef<ZNetListener_TCP> ZNetListener_TCP::sCreate(ip_port iPort)
	{
	return ZFunctionChain_T<ZRef<ZNetListener_TCP>, MakeParam_t>
		::sInvoke(MakeParam_t(iPort));
	}

ZRef<ZNetListener_TCP> ZNetListener_TCP::sCreate(
	ip4_addr iAddress, ip_port iPort)
	{
	return ZFunctionChain_T<ZRef<ZNetListener_TCP>, MakeParam4_t>
		::sInvoke(MakeParam4_t(iAddress, iPort));
	}

ZRef<ZNetListener_TCP> ZNetListener_TCP::sCreate(
	ip6_addr iAddress, ip_port iPort)
	{
	return ZFunctionChain_T<ZRef<ZNetListener_TCP>, MakeParam6_t>
		::sInvoke(MakeParam6_t(iAddress, iPort));
	}

// =================================================================================================
// MARK: - ZNetEndpoint_TCP

ZRef<ZNetEndpoint_TCP> ZNetEndpoint_TCP::sCreateConnected(
	ip4_addr iRemoteAddr, ip_port iRemotePort)
	{
	return ZFunctionChain_T<ZRef<ZNetEndpoint_TCP>, MakeParam4_t>
		::sInvoke(MakeParam4_t(iRemoteAddr, iRemotePort));
	}

ZRef<ZNetEndpoint_TCP> ZNetEndpoint_TCP::sCreateConnected(
	ip6_addr iRemoteAddr, ip_port iRemotePort)
	{
	return ZFunctionChain_T<ZRef<ZNetEndpoint_TCP>, MakeParam6_t>
		::sInvoke(MakeParam6_t(iRemoteAddr, iRemotePort));
	}

} // namespace ZooLib
