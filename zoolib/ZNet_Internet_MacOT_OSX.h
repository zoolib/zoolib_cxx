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

#ifndef __ZNet_Internet_MacOT_OSX__
#define __ZNet_Internet_MacOT_OSX__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Net_Internet_MacOT_OSX
#	define ZCONFIG_API_Avail__Net_Internet_MacOT_OSX ZCONFIG_SPI_Enabled(Carbon)
#endif

#ifndef ZCONFIG_API_Desired__Net_Internet_MacOT_OSX
#	define ZCONFIG_API_Desired__Net_Internet_MacOT_OSX 1
#endif

#include "zoolib/ZNet_Internet.h"

#if ZCONFIG_API_Enabled(Net_Internet_MacOT_OSX)

#if ZCONFIG_SPI_Enabled(MacOSX)
// Uggh. OpenTransportProviders.h is now part of the
// OSServices subframework of CoreServices.
#	include <CoreServices/CoreServices.h>
#else
#	include <OpenTransportProviders.h>
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Internet_MacOT_OSX

class ZNetNameLookup_Internet_MacOT_OSX : public ZNetNameLookup
	{
public:
	ZNetNameLookup_Internet_MacOT_OSX(const string& iName, ip_port iPort, size_t iMaxAddresses);
	virtual ~ZNetNameLookup_Internet_MacOT_OSX();

// From ZNetNameLookup
	virtual void Start();
	virtual bool Finished();
	virtual void Advance();

	virtual ZRef<ZNetAddress> CurrentAddress();
	virtual ZRef<ZNetName> CurrentName();

protected:
	static void sMP_Lookup(void* iParam);

	std::string fName;
	ip_port fPort;
	bool fStarted;
	size_t fCountAddressesToReturn;
	size_t fCurrentIndex;
	InetHostInfo fInetHostInfo;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP_MacOT_OSX

class ZNetListener_TCP_MacOT_OSX : public ZNetListener_TCP
	{
public:
	ZNetListener_TCP_MacOT_OSX(ip_port iLocalPort, size_t iListenQueueSize);
	virtual ~ZNetListener_TCP_MacOT_OSX();

// From ZNetListener via ZNetListener_TCP
	virtual ZRef<ZNetEndpoint> Listen();
	virtual void CancelListen();

// From ZNetListener_TCP
	virtual ip_port GetPort();

private:
	static void sMP_Constructor(void* iParam);
	static void sMP_Destructor(void* iParam);
	static void sMP_Listen(void* iParam);
	static void sMP_CancelListen(void* iParam);

	EndpointRef fEndpointRef;
	ip_port fLocalPort;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP_MacOT_OSX

class ZNetEndpoint_TCP_MacOT_OSX
:	public ZNetEndpoint_TCP,
	private ZStreamRCon,
	private ZStreamWCon
	{
	friend class ZNetListener_TCP_MacOT_OSX;
protected:
	ZNetEndpoint_TCP_MacOT_OSX(EndpointRef iEndpointRef, InetAddress& iRemoteInetAddress);

public:
	ZNetEndpoint_TCP_MacOT_OSX(ip_addr iRemoteHost, ip_port iRemotePort);
	void Internal_Init();

	virtual ~ZNetEndpoint_TCP_MacOT_OSX();

// From ZStreamerRCon via ZNetEndpoint_TCP
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZNetEndpoint_TCP
	virtual const ZStreamWCon& GetStreamWCon();

// From ZNetEndpoint via ZNetEndpoint_TCP
	virtual ZRef<ZNetAddress> GetRemoteAddress();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamRCon
	virtual bool Imp_WaitReadable(int iMilliseconds);
	virtual bool Imp_ReceiveDisconnect(int iMilliseconds);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	static void sMP_Constructor(void* iParam);
	static void sMP_Destructor(void* iParam);
	static void sMP_Imp_Read(void* iParam);
	static void sMP_GetCountReadable(void* iParam);

	static void sMP_Imp_Write(void* iParam);
	static void sMP_SendDisconnect(void* iParam);
	static void sMP_ReceiveDisconnect(void* iParam);
	static void sMP_Abort(void* iParam);

	EndpointRef fEndpointRef;
	ip_addr fRemoteHost;
	ip_port fRemotePort;
	};

// =================================================================================================

#endif // ZCONFIG_API_Enabled(Net_Internet_MacOT_OSX)

#endif // __ZNet_Internet_MacOT_OSX__
