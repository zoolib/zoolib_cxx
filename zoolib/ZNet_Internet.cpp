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

#include "ZNet_Internet.h"
#include "ZFactoryChain.h"
#include "ZString.h"

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Factories

ZOOLIB_FACTORYCHAIN_HEAD(ZRef<ZNetNameLookup>, ZNetName_Internet::LookupParam_t);
ZOOLIB_FACTORYCHAIN_HEAD(ZRef<ZNetListener_TCP>, ZNetListener_TCP::MakeParam_t);
ZOOLIB_FACTORYCHAIN_HEAD(ZRef<ZNetEndpoint_TCP>, ZNetEndpoint_TCP::MakeParam_t);

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress_Internet

ZNetAddress_Internet::ZNetAddress_Internet()
	{}

ZNetAddress_Internet::ZNetAddress_Internet(const ZNetAddress_Internet& other)
:	fHost(other.fHost),
	fPort(other.fPort)
	{}

ZNetAddress_Internet::ZNetAddress_Internet(ip_addr iHost, ip_port iPort)
:	fHost(iHost),
	fPort(iPort)
	{}

ZNetAddress_Internet::ZNetAddress_Internet(
	uint8 iHost1, uint8 iHost2, uint8 iHost3, uint8 iHost4, ip_port iPort)
:	fHost((iHost1 << 24) | (iHost2 << 16) | (iHost3 << 8) | iHost4),
	fPort(iPort)
	{}

ZNetAddress_Internet::~ZNetAddress_Internet()
	{}

ZRef<ZNetEndpoint> ZNetAddress_Internet::Connect() const
	{ return ZNetEndpoint_TCP::sCreateConnectedEndpoint(fHost, fPort); }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetName_Internet

ZNetName_Internet::ZNetName_Internet()
:	fPort(0)
	{}

ZNetName_Internet::ZNetName_Internet(const ZNetName_Internet& other)
:	fName(other.fName),
	fPort(other.fPort)
	{}

ZNetName_Internet::ZNetName_Internet(const string& iName, ip_port iPort)
:	fName(iName),
	fPort(iPort)
	{}

ZNetName_Internet::~ZNetName_Internet()
	{}

string ZNetName_Internet::AsString() const
	{ return fName + ZString::sFormat(":%d", fPort); }

ZRef<ZNetNameLookup> ZNetName_Internet::CreateLookup(size_t iMaxAddresses) const
	{
	return ZFactoryChain_T<ZRef<ZNetNameLookup>, LookupParam_t>
		::sMake(LookupParam_t(fName, fPort, iMaxAddresses));
	}

const string& ZNetName_Internet::GetName() const
	{ return fName; }

ip_port ZNetName_Internet::GetPort() const
	{ return fPort; }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP

ZRef<ZNetListener_TCP> ZNetListener_TCP::sCreateListener(ip_port iPort, size_t iListenQueueSize)
	{
	return ZFactoryChain_T<ZRef<ZNetListener_TCP>, MakeParam_t>
		::sMake(MakeParam_t(0, iPort, iListenQueueSize));
	}

ZRef<ZNetListener_TCP> ZNetListener_TCP::sCreateListener(
	ip_addr iAddress, ip_port iPort, size_t iListenQueueSize)
	{
	return ZFactoryChain_T<ZRef<ZNetListener_TCP>, MakeParam_t>
		::sMake(MakeParam_t(iAddress, iPort, iListenQueueSize));
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP

ZRef<ZNetEndpoint_TCP> ZNetEndpoint_TCP::sCreateConnectedEndpoint(
	ip_addr iRemoteHost, ip_port iRemotePort)
	{
	return ZFactoryChain_T<ZRef<ZNetEndpoint_TCP>, MakeParam_t>
		::sMake(MakeParam_t(iRemoteHost, iRemotePort));
	}
