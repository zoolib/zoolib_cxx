/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZNet_Internet_MacOT_Classic__
#define __ZNet_Internet_MacOT_Classic__ 1
#include "zconfig.h"
#include "ZCONFIG_API.h"
#include "ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Net_Internet_MacOT_Classic
#	define ZCONFIG_API_Avail__Net_Internet_MacOT_Classic \
	(ZCONFIG(API_Thread, Mac) && (ZCONFIG_SPI_Enabled(Carbon) || ZCONFIG_SPI_Enabled(MacClassic)))
#endif

#ifndef ZCONFIG_API_Desired__Net_Internet_MacOT_Classic
#	define ZCONFIG_API_Desired__Net_Internet_MacOT_Classic 1
#endif

#include "ZCompat_NonCopyable.h"
#include "ZNet_Internet.h"

#if ZCONFIG_API_Enabled(Net_Internet_MacOT_Classic)

#include "ZThreadTM.h"
#include <OSUtils.h>
#include <OpenTransportProviders.h>

// =================================================================================================
#pragma mark -
#pragma mark * ZOTProviderWrapper

class ZOTProviderWaiter;

/*!
ZOTProviderWrapper encapsulates a ProviderRef and a list of ZOTProviderWaiters that are each
looking for particular notifications.
*/

class ZOTProviderWrapper : ZooLib::NonCopyable
	{
public:
	ZOTProviderWrapper();
	~ZOTProviderWrapper();

	ProviderRef GetProviderRef()
		{ return fProviderRef; }
	void SetProviderRef(ProviderRef iProviderRef);

	void WakeAllWaiters();

	static OTNotifyUPP sNotifierUPP;

private:
	void RealNotifier(OTEventCode iEventCode, OTResult iResult, void* iCookie);

	void Notifier(OTEventCode iEventCode, OTResult iResult, void* iCookie);
	static pascal void sNotifier(void* iContextPtr,
		OTEventCode iEventCode, OTResult iResult, void* iCookie);

	static pascal void sDeferredNotifier(long iDeferredTask ZPARAM_A1);
	static DeferredTaskUPP sDeferredNotifierUPP;

	friend class ZOTProviderWaiter;

	ZOTProviderWaiter* fWaiters;

	ProviderRef fProviderRef;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZOTProviderWaiter

/*!
ZOTProviderWaiter encapsulates one or two notification messages that are being waited for.
*/

class ZOTProviderWaiter
	{
public:
	ZOTProviderWaiter(ZOTProviderWrapper* iProvider, OTEventCode iWaitForEventCode);

	ZOTProviderWaiter(ZOTProviderWrapper* iProvider,
		OTEventCode iWaitForEventCode0,
		OTEventCode iWaitForEventCode1);

	ZOTProviderWaiter(ZOTProviderWrapper* iProvider,
		OTEventCode iWaitForEventCode0,
		OTEventCode iWaitForEventCode1,
		OTEventCode iWaitForEventCode2);

	~ZOTProviderWaiter();

	void AddEventCode(OTEventCode iEventCode);
	OTResult Wait(OTEventCode& outEventCode, void*& oCookie);
	OTResult Wait();
	void DontWait();

private:
	friend class ZOTProviderWrapper;

	ZOTProviderWaiter* fNext;
	ZOTProviderWrapper* fProvider;
	OTEventCode fEventCode;
	OTResult fResult;
	void* fCookie;
	OTEventCode fWaitForEventCode[3];
	ZThreadTM_State* fThread;
	bool fWaitOrDontWaitCalled;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Internet_MacOT_Classic

class ZNetNameLookup_Internet_MacOT_Classic : public ZNetNameLookup
	{
public:
	ZNetNameLookup_Internet_MacOT_Classic(const string& iName, ip_port iPort, size_t iMaxAddresses);
	virtual ~ZNetNameLookup_Internet_MacOT_Classic();

// From ZNetNameLookup
	virtual void Start();
	virtual bool Finished();
	virtual void Next();
	virtual const ZNetAddress& CurrentAddress();
	virtual const ZNetName& CurrentName();

protected:
	void Internal_GrabNextResult();

	ZNetName_Internet fNetName;
	bool fStarted;
	size_t fCountAddressesToReturn;
	ZNetAddress* fCurrentAddress;
	size_t fNextIndex;
	InetHostInfo fInetHostInfo;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP_MacOT_Classic

class ZNetListener_TCP_MacOT_Classic : public ZNetListener_TCP
	{
public:
	ZNetListener_TCP_MacOT_Classic(ip_port iLocalPort, size_t iListenQueueSize);
	virtual ~ZNetListener_TCP_MacOT_Classic();

// From ZNetListener_TCP
	virtual ZRef<ZNetEndpoint> Listen();
	virtual void CancelListen();

// From ZNetListener_TCP
	virtual ip_port GetPort();

private:
	ZOTProviderWrapper fProvider;
	ip_port fLocalPort;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP_MacOT_Classic

class ZNetEndpoint_TCP_MacOT_Classic
:	protected ZStreamR,
	protected ZStreamW,
	public ZNetEndpoint_TCP
	{
private:
	friend class ZNetListener_TCP_MacOT_Classic;
	ZNetEndpoint_TCP_MacOT_Classic(const InetAddress& iRemoteInetAddress);

public:
	ZNetEndpoint_TCP_MacOT_Classic(ip_addr iRemoteHost, ip_port iRemotePort);
	virtual ~ZNetEndpoint_TCP_MacOT_Classic();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamerR via ZNetEndpoint_TCP
	virtual const ZStreamR& GetStreamR();

// From ZStreamerW via ZNetEndpoint_TCP
	virtual const ZStreamW& GetStreamW();

// From ZNetEndpoint via ZNetEndpoint_TCP
	virtual bool WaitTillReadable(int iMilliseconds);
	virtual void SendDisconnect();
	virtual bool ReceiveDisconnect(int iMilliseconds);
	virtual void Abort();

	virtual ZNetAddress* GetRemoteAddress();

	EndpointRef GetEndpointRef();

private:
	void Internal_Initialize();
	void Internal_TearDown();

	ZOTProviderWrapper fProvider;
	ip_addr fRemoteHost;
	ip_port fRemotePort;
	};

// =================================================================================================

#endif // ZCONFIG_API_Enabled(Net_Internet_MacOT_Classic)

#endif // __ZNet_Internet_MacOT_Classic__
