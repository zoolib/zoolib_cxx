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
#include "zoolib/ZTypes.h"

#include <string>

// =================================================================================================

typedef uint32 ip_addr;
typedef uint16 ip_port;

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress_Internet

class ZNetAddress_Internet : public ZNetAddress
	{
public:
	ZNetAddress_Internet();
	ZNetAddress_Internet(const ZNetAddress_Internet& other);
	ZNetAddress_Internet(ip_addr iHost, ip_port inPort);
	ZNetAddress_Internet(uint8 iHost1, uint8 iHost2, uint8 iHost3, uint8 iHost4, ip_port iPort);
	virtual ~ZNetAddress_Internet();

// From ZNetAddress
	virtual ZRef<ZNetEndpoint> Connect() const;

// Our protocol
	ip_addr GetHost() const { return fHost; }
	ip_port GetPort() const { return fPort; }

private:
	ip_addr fHost;
	ip_port fPort;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetName_Internet

class ZNetName_Internet : public ZNetName
	{
public:
	typedef ZMulti_T3<std::string, ip_port, size_t> LookupParam_t;

	ZNetName_Internet();
	ZNetName_Internet(const ZNetName_Internet& other);
	ZNetName_Internet(const std::string& iName, ip_port iPort);
	virtual ~ZNetName_Internet();

// From ZNetName
	virtual std::string AsString() const;
	virtual ZRef<ZNetNameLookup> CreateLookup(size_t iMaxAddresses) const;

// Our protocol
	const std::string& GetName() const;
	ip_port GetPort() const;

private:
	std::string fName;
	ip_port fPort;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP

class ZNetListener_TCP : public ZNetListener
	{
public:
	typedef ZMulti_T3<ip_addr, ip_port, size_t> MakeParam_t;

	virtual ip_port GetPort() = 0;

	static ZRef<ZNetListener_TCP> sCreateListener(
		ip_port iPort, size_t iListenQueueSize);

	static ZRef<ZNetListener_TCP> sCreateListener(
		ip_addr iAddress, ip_port iPort, size_t iListenQueueSize);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP

class ZNetEndpoint_TCP : public ZNetEndpoint
	{
public:
	typedef ZMulti_T2<ip_addr, ip_port> MakeParam_t;

	static ZRef<ZNetEndpoint_TCP> sCreateConnectedEndpoint(
		ip_addr iRemoteHost, ip_port iRemotePort);
	};

#endif // __ZNet_Internet__
