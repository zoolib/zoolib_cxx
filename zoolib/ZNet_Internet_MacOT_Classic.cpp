/* ------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZNet_Internet_MacOT_Classic.h"

#if ZCONFIG_API_Enabled(Internet_MacOT_Classic)

#include "ZMemory.h"
#include "ZThreadTM.h"

#ifndef kDebug_OT
#	define kDebug_OT 1
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static ZNet::Error sTranslateError(OTResult theOTResult)
	{
	if (theOTResult == kOTNoError)
		return ZNet::errorNone;
	return ZNet::errorGeneric;
	}

static OTResult sSetFourByteOption(EndpointRef ep, OTXTILevel level, OTXTIName name, UInt32 value)
	{
	// Set up the option buffer to specify the option and value to set.
	TOption option;
	option.len = kOTFourByteOptionSize;
	option.level = level;
	option.name = name;
	option.status = 0;
	option.value[0] = value;

	// Set up request parameter for OTOptionManagement
	TOptMgmt request;
	request.opt.buf = (UInt8 *) &option;
	request.opt.len = sizeof(option);
	request.flags = T_NEGOTIATE;

	// Set up reply parameter for OTOptionManagement.
	TOptMgmt result;
	result.opt.buf = (UInt8 *) &option;
	result.opt.maxlen = sizeof(option);
	OTResult err = ::OTOptionManagement(ep, &request, &result);
	if (err == noErr)
		{
		if (option.status != T_SUCCESS)
			err = option.status;
		}

	return err;
	}

class EnterLeaveNotifier
	{
public:
	EnterLeaveNotifier(ProviderRef iProviderRef);
	~EnterLeaveNotifier();
private:
	ProviderRef fProviderRef;
	bool fNeedsLeave;
	};

EnterLeaveNotifier::EnterLeaveNotifier(ProviderRef iProviderRef)
:	fProviderRef(iProviderRef)
	{
	if (fProviderRef)
		{
		fNeedsLeave = ::OTEnterNotifier(fProviderRef);
		if (!fNeedsLeave)
			ZDebugStop(0);
		}
	else
		{
		fNeedsLeave = false;
		}
	}

EnterLeaveNotifier::~EnterLeaveNotifier()
	{
	if (fNeedsLeave)
		::OTLeaveNotifier(fProviderRef);
	}

// =================================================================================================
#pragma mark -
#pragma mark * InitHelper__

typedef Boolean (*OTCanLoadLibrariesProc)(void); // Note that this is a "C" function, not a "pascal" function!

static OTCanLoadLibrariesProc sOTCanLoadLibrariesProc;

static void InitOTCanLoadLibrariesProc(void)
	{
#if TARGET_API_MAC_CARBON
#if TARGET_RT_MAC_CFM
	CFragConnectionID connID;
	Ptr junkMain;
	Str255 junkMsg;
	Ptr symAddr;
	CFragSymbolClass junkSymClass;
	
	if (::GetSharedLibrary("\pOTUtilityLib", kPowerPCCFragArch, kFindCFrag, &connID, &junkMain, junkMsg) == noErr
			&& FindSymbol(connID, "\pOTCanLoadLibraries", &symAddr, &junkSymClass) == noErr)
		{
		sOTCanLoadLibrariesProc = (OTCanLoadLibrariesProc) symAddr;
		}
#endif
#else
	sOTCanLoadLibrariesProc = OTCanLoadLibraries;
#endif
	}

namespace ZANONYMOUS {
class InitHelper__
	{
public:
	InitHelper__();
	~InitHelper__();
private:
	bool fInited;
	static InitHelper__ sInitHelper__;
	};

InitHelper__ InitHelper__::sInitHelper__;

InitHelper__::InitHelper__()
	{
	fInited = false;

	// If we're running on MacOSX we'll need to ensure the MP library is loaded.
	UInt32 systemVersion;
	OSErr err = ::Gestalt(gestaltSystemVersion, (SInt32*)&systemVersion);
	if (err == noErr && systemVersion >= 0x01000)
		{
		if (!MPLibraryIsLoaded())
			return;
		}

	#if ZCONFIG(OS, Carbon)
		if (noErr == ::InitOpenTransportInContext(kInitOTForApplicationMask, nil))
	#else
		if (noErr == ::InitOpenTransport())
	#endif
		{
		fInited = true;
		}

// AG 2005-08-30. I'm not sure why this line got commented out. It happened in the
// big checkin of 2002-05-14. It seems like it ought to be needed.
//	::InitOTCanLoadLibrariesProc();
	}

InitHelper__::~InitHelper__()
	{
	if (fInited)
		{
		#if ZCONFIG(OS, Carbon)
			::CloseOpenTransportInContext(nil);
		#else
			::CloseOpenTransport();
		#endif
		}
	}
} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZOTProviderWrapper

ZOTProviderWrapper::ZOTProviderWrapper()
	{
	fWaiters = nil;
	fProviderRef = nil;
	}

ZOTProviderWrapper::~ZOTProviderWrapper()
	{
	ZAssertStopf(kDebug_OT, fWaiters == nil, ("fWaiters == %08X", fWaiters));
	if (fProviderRef)
		{
		OTResult theResult = ::OTCloseProvider(fProviderRef);
		ZAssertStopf(kDebug_OT, theResult == kOTNoError, ("theResult == %d", theResult));
		}
	}

void ZOTProviderWrapper::SetProviderRef(ProviderRef iProviderRef)
	{
	ZAssertStop(kDebug_OT, !fProviderRef);

	fProviderRef = iProviderRef;

	// The provider as passed to use should be synchronous.
	ZAssertStop(kDebug_OT, ::OTIsSynchronous(fProviderRef));

	// But we need it to be asynchronous.
	::OTSetAsynchronous(fProviderRef);

	::OTInstallNotifier(fProviderRef, sNotifierUPP, this);
	}

void ZOTProviderWrapper::WakeAllWaiters()
	{
	EnterLeaveNotifier theELN(fProviderRef);
	ZOTProviderWaiter* currentWaiter = fWaiters;
	fWaiters = nil;

	::OTCancelSynchronousCalls(fProviderRef, kOTCanceledErr);

	while (currentWaiter)
		{
		currentWaiter->fEventCode = 0;
		currentWaiter->fResult = kOTCanceledErr;
		currentWaiter->fCookie = 0;
		ZOTProviderWaiter* nextWaiter = currentWaiter->fNext;
		currentWaiter->fNext = nil;
		ZAssertStop(kDebug_OT, currentWaiter->fThread);
		ZThreadTM_Resume(currentWaiter->fThread);
		currentWaiter = nextWaiter;
		}
	}

extern bool sDebug_InvokeLowLevelDebugger;

void ZOTProviderWrapper::RealNotifier(OTEventCode iEventCode, OTResult iResult, void* iCookie)
	{
	bool priorInvokeLoweLevelDebugger = sDebug_InvokeLowLevelDebugger;
	sDebug_InvokeLowLevelDebugger = true;

	if (iEventCode == T_OPENCOMPLETE)
		fProviderRef = reinterpret_cast<ProviderRef>(iCookie);

	ZOTProviderWaiter* priorWaiter = nil;
	ZOTProviderWaiter* currentWaiter = fWaiters;
	while (currentWaiter)
		{
		if (iEventCode == currentWaiter->fWaitForEventCode[0]
			|| iEventCode == currentWaiter->fWaitForEventCode[1]
			|| iEventCode == currentWaiter->fWaitForEventCode[2])
			{
			ZOTProviderWaiter* nextWaiter = currentWaiter->fNext;
			currentWaiter->fNext = nil;
			if (priorWaiter)
				priorWaiter->fNext = nextWaiter;
			else
				fWaiters = nextWaiter;

			currentWaiter->fEventCode = iEventCode;
			currentWaiter->fResult = iResult;
			currentWaiter->fCookie = iCookie;
			currentWaiter->fNext = nil;
			ZAssertStop(kDebug_OT, currentWaiter->fThread);
			ZThreadTM_Resume(currentWaiter->fThread);
			currentWaiter = nextWaiter;
			}
		else
			{
			priorWaiter = currentWaiter;
			currentWaiter = currentWaiter->fNext;
			}
		}

	sDebug_InvokeLowLevelDebugger = priorInvokeLoweLevelDebugger;
	}

struct DeferredNotifier_t
	{
	ZOTProviderWrapper* fProviderWrapper;
	OTEventCode fEventCode;
	OTResult fResult;
	void* fCookie;
	bool fFired;
	};

pascal void ZOTProviderWrapper::sDeferredNotifier(long iDeferredTask ZPARAM_A1)
	{
	DeferredNotifier_t* theDN = reinterpret_cast<DeferredNotifier_t*>(iDeferredTask);
	theDN->fProviderWrapper->RealNotifier(theDN->fEventCode, theDN->fResult, theDN->fCookie);
	theDN->fFired = true;
	}

DeferredTaskUPP ZOTProviderWrapper::sDeferredNotifierUPP = NewDeferredTaskUPP(sDeferredNotifier);

void ZOTProviderWrapper::Notifier(OTEventCode iEventCode, OTResult iResult, void* iCookie)
	{
	if (sOTCanLoadLibrariesProc && sOTCanLoadLibrariesProc())
		{
		// We can load libraries, which means we're running at system task level, and
		// so we need to invoke our notifier via a deferred task. cf OTMP which operates
		// in a reverse fashion from us -- it aims to support blocking calls from MP
		// threads under all circumstances and it uses the deferred task's non-reentrant
		// guarantee to protect stuff.
		DeferredNotifier_t theDN;
		theDN.fProviderWrapper = this;
		theDN.fEventCode = iEventCode;
		theDN.fResult = iResult;
		theDN.fCookie = iCookie;
		theDN.fFired = false;

		DeferredTask theDT;
		theDT.qLink = 0;
		theDT.qType = dtQType;
		theDT.dtFlags = 0;
		theDT.dtAddr = sDeferredNotifierUPP;
		theDT.dtParam = reinterpret_cast<long>(&theDN);
		theDT.dtReserved = 0;

		::DTInstall(&theDT);

		ZAssertStop(kDebug_OT, theDN.fFired);
		}
	else
		{
		this->RealNotifier(iEventCode, iResult, iCookie);
		}
	}

pascal void ZOTProviderWrapper::sNotifier(void* iContextPtr, OTEventCode iEventCode, OTResult iResult, void* iCookie)
	{ static_cast<ZOTProviderWrapper*>(iContextPtr)->Notifier(iEventCode, iResult, iCookie); }

OTNotifyUPP ZOTProviderWrapper::sNotifierUPP = NewOTNotifyUPP(sNotifier);

// =================================================================================================
#pragma mark -
#pragma mark * ZOTProviderWaiter

ZOTProviderWaiter::ZOTProviderWaiter(ZOTProviderWrapper* iProvider, OTEventCode iWaitForEventCode)
	{
	fProvider = iProvider;
	fNext = nil;
	fEventCode = 0;
	fResult = kOTNoError;
	fCookie = nil;
	fWaitForEventCode[0] = iWaitForEventCode;
	fWaitForEventCode[1] = 0;
	fWaitForEventCode[2] = 0;
	fThread = ZThreadTM_Current();
	fWaitOrDontWaitCalled = false;
	EnterLeaveNotifier theELN(fProvider->fProviderRef);
	fNext = fProvider->fWaiters;
	fProvider->fWaiters = this;
	}

ZOTProviderWaiter::ZOTProviderWaiter(ZOTProviderWrapper* iProvider, OTEventCode iWaitForEventCode0, OTEventCode iWaitForEventCode1)
	{
	fProvider = iProvider;
	fNext = nil;
	fEventCode = 0;
	fResult = kOTNoError;
	fCookie = nil;
	fWaitForEventCode[0] = iWaitForEventCode0;
	fWaitForEventCode[1] = iWaitForEventCode1;
	fWaitForEventCode[2] = 0;
	fThread = ZThreadTM_Current();
	fWaitOrDontWaitCalled = false;
	EnterLeaveNotifier theELN(fProvider->fProviderRef);
	fNext = fProvider->fWaiters;
	fProvider->fWaiters = this;
	}

ZOTProviderWaiter::ZOTProviderWaiter(ZOTProviderWrapper* iProvider, OTEventCode iWaitForEventCode0, OTEventCode iWaitForEventCode1, OTEventCode iWaitForEventCode2)
	{
	fProvider = iProvider;
	fNext = nil;
	fEventCode = 0;
	fResult = kOTNoError;
	fCookie = nil;
	fWaitForEventCode[0] = iWaitForEventCode0;
	fWaitForEventCode[1] = iWaitForEventCode1;
	fWaitForEventCode[2] = iWaitForEventCode2;
	fThread = ZThreadTM_Current();
	fWaitOrDontWaitCalled = false;
	EnterLeaveNotifier theELN(fProvider->fProviderRef);
	fNext = fProvider->fWaiters;
	fProvider->fWaiters = this;
	}

ZOTProviderWaiter::~ZOTProviderWaiter()
	{
	EnterLeaveNotifier theELN(fProvider->fProviderRef);
	ZAssertStop(kDebug_OT, fWaitOrDontWaitCalled);
	ZAssertStopf(kDebug_OT, fNext == nil, ("%08X", fNext));
	}

OTResult ZOTProviderWaiter::Wait(OTEventCode& oEventCode, void*& oCookie)
	{
	ZAssertStop(kDebug_OT, !fWaitOrDontWaitCalled);
	ZAssertStop(kDebug_OT, fThread == nil || fThread == ZThreadTM_Current());
	ZThreadTM_Suspend();
	fWaitOrDontWaitCalled = true;
	oEventCode = fEventCode;
	oCookie = fCookie;
	return fResult;
	}

OTResult ZOTProviderWaiter::Wait()
	{
	ZAssertStop(kDebug_OT, !fWaitOrDontWaitCalled);
	ZAssertStop(kDebug_OT, fThread == nil || fThread == ZThreadTM_Current());
	ZThreadTM_Suspend();
	fWaitOrDontWaitCalled = true;
	return fResult;
	}

void ZOTProviderWaiter::DontWait()
	{
	EnterLeaveNotifier theELN(fProvider->fProviderRef);
	ZAssertStop(kDebug_OT, !fWaitOrDontWaitCalled);
	ZAssertStop(kDebug_OT, fThread == nil || fThread == ZThreadTM_Current());

	ZOTProviderWaiter* priorWaiter = nil;
	ZOTProviderWaiter* currentWaiter = fProvider->fWaiters;

	while (currentWaiter)
		{
		if (currentWaiter == this)
			{
			if (priorWaiter)
				priorWaiter->fNext = fNext;
			else
				fProvider->fWaiters = fNext;
			fNext = nil;
			break;
			}
		priorWaiter = currentWaiter;
		currentWaiter = currentWaiter->fNext;
		}

	fWaitOrDontWaitCalled = true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Internet_MacOT_Classic

ZNetNameLookup_Internet_MacOT_Classic::ZNetNameLookup_Internet_MacOT_Classic(const string& iName, ip_port iPort, size_t iMaxAddresses)
:	fNetName(iName, iPort),
	fStarted(false),
	fCountAddressesToReturn(iMaxAddresses),
	fCurrentAddress(nil),
	fNextIndex(0)
	{
	if (0 == iName.size())
		{
		fStarted = true;
		fCountAddressesToReturn = 0;
		}
	}

ZNetNameLookup_Internet_MacOT_Classic::~ZNetNameLookup_Internet_MacOT_Classic()
	{
	delete fCurrentAddress;
	}

void ZNetNameLookup_Internet_MacOT_Classic::Start()
	{
	if (fStarted)
		return;

	fStarted = true;

	if (fCountAddressesToReturn == 0)
		return;

	ZOTProviderWrapper theProvider;
	ZOTProviderWaiter theWaiter1(&theProvider, T_OPENCOMPLETE);
#if ZCONFIG(OS, Carbon)
	OSStatus theResult = ::OTAsyncOpenInternetServicesInContext(kDefaultInternetServicesPath, 0, ZOTProviderWrapper::sNotifierUPP, &theProvider, nil);
#else // ZCONFIG(OS, Carbon)
	OSStatus theResult = ::OTAsyncOpenInternetServices(kDefaultInternetServicesPath, 0, ZOTProviderWrapper::sNotifierUPP, &theProvider);
#endif // ZCONFIG(OS, Carbon)
	if (theResult)
		{
		theWaiter1.DontWait();
		fCountAddressesToReturn = 0;
		return;
		}

	theResult = theWaiter1.Wait();
	if (theResult)
		{
		fCountAddressesToReturn = 0;
		return;
		}

	InetSvcRef theInetSvcRef = reinterpret_cast<InetSvcRef>(theProvider.GetProviderRef());

	ZOTProviderWaiter theWaiter2(&theProvider, T_DNRSTRINGTOADDRCOMPLETE);
	theResult = ::OTInetStringToAddress(theInetSvcRef, const_cast<char*>(fNetName.GetName().c_str()), &fInetHostInfo);
	if (theResult)
		{
		theWaiter2.DontWait();
		fCountAddressesToReturn = 0;
		}
	else
		{
		theWaiter2.Wait();
		}
	}

bool ZNetNameLookup_Internet_MacOT_Classic::Finished()
	{
	if (!fStarted)
		return true;

	if (0 == fCountAddressesToReturn)
		return true;

	while (fNextIndex < kMaxHostAddrs)
		{
		if (fInetHostInfo.addrs[fNextIndex])
			return false;
		++fNextIndex;
		}
	return true;
	}

void ZNetNameLookup_Internet_MacOT_Classic::Next()
	{
	ZAssertStop(kDebug_OT, fStarted);

	this->Internal_GrabNextResult();
	}

const ZNetAddress& ZNetNameLookup_Internet_MacOT_Classic::CurrentAddress()
	{
	if (!fCurrentAddress)
		this->Internal_GrabNextResult();

	ZAssertStop(kDebug_OT, fCurrentAddress);
	return *fCurrentAddress;
	}

const ZNetName& ZNetNameLookup_Internet_MacOT_Classic::CurrentName()
	{
	if (!fCurrentAddress)
		this->Internal_GrabNextResult();

	ZAssertStop(kDebug_OT, fCurrentAddress);
	return fNetName;
	}

void ZNetNameLookup_Internet_MacOT_Classic::Internal_GrabNextResult()
	{
	while (fNextIndex < kMaxHostAddrs)
		{
		if (fInetHostInfo.addrs[fNextIndex])
			{
			delete fCurrentAddress;
			fCurrentAddress = new ZNetAddress_Internet(fInetHostInfo.addrs[fNextIndex], fNetName.GetPort());
			++fNextIndex;
			--fCountAddressesToReturn;
			break;
			}
		++fNextIndex;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP_MacOT_Classic

ZNetListener_TCP_MacOT_Classic::ZNetListener_TCP_MacOT_Classic(ip_port iLocalPort, size_t iListenQueueSize)
	{
	if (iListenQueueSize == 0)
		iListenQueueSize = 5;

	if (iListenQueueSize > 10)
		iListenQueueSize = 10; // Let's not max out OT *too* much.

	fLocalPort = iLocalPort;

	// Open the endpoint.
	OTConfiguration* theConfiguration = ::OTCreateConfiguration("tilisten,tcp");

	OTResult theResult;

	// There are issues with doing async opens -- as we don't create listeners very often
	// I'm doing a sync open here to side-step the complexities.
#if ZCONFIG(OS, Carbon)
	EndpointRef theEPR = ::OTOpenEndpointInContext(theConfiguration, 0, nil, &theResult, nil);
#else
	EndpointRef theEPR = ::OTOpenEndpoint(theConfiguration, 0, nil, &theResult);
#endif
	if (theResult != noErr)
		throw ZNetEx(sTranslateError(theResult));

	// The endpoint should be synchronous
	ZAssertStop(kDebug_OT, ::OTIsSynchronous(theEPR));

	// We need it to be blocking.
	::OTSetBlocking(theEPR);

	// And the provider will set it to be async.
	fProvider.SetProviderRef(theEPR);

	::sSetFourByteOption(theEPR, INET_IP, kIP_REUSEADDR, 1);

	// Bind the endpoint
	InetAddress theInetAddress;
	theInetAddress.fAddressType = AF_INET;
	theInetAddress.fPort = iLocalPort;
	theInetAddress.fHost = kOTAnyInetAddress;
	TBind theTBind;
	theTBind.addr.len = sizeof(InetAddress);
	theTBind.addr.maxlen = sizeof(InetAddress);
	theTBind.addr.buf = (UInt8*)&theInetAddress;
	theTBind.qlen = iListenQueueSize;

	ZOTProviderWaiter theWaiter2(&fProvider, T_BINDCOMPLETE);
	theResult = ::OTBind(theEPR, &theTBind, nil);
	if (theResult == noErr)
		theResult = theWaiter2.Wait();
	else
		theWaiter2.DontWait();

	if (theResult != noErr)
		throw ZNetEx(sTranslateError(theResult));
	}

ZNetListener_TCP_MacOT_Classic::~ZNetListener_TCP_MacOT_Classic()
	{
	// Unbind our endpoint.
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());
	ZOTProviderWaiter theWaiter(&fProvider, T_UNBINDCOMPLETE);

	OTResult theResult = ::OTUnbind(theEndpointRef);
	if (theResult == kOTNoError)
		theWaiter.Wait();
	else
		theWaiter.DontWait();
	}

ZRef<ZNetEndpoint> ZNetListener_TCP_MacOT_Classic::Listen()
	{
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());

	InetAddress remoteInetAddress;
	TCall theTCall;
	ZBlockZero(&theTCall, sizeof(theTCall));
	theTCall.addr.buf = reinterpret_cast<UInt8*>(&remoteInetAddress);
	theTCall.addr.maxlen = sizeof(remoteInetAddress);


	while (true)
		{
		ZOTProviderWaiter theWaiter(&fProvider, T_LISTEN, T_DISCONNECT);
		OTResult theResult = ::OTListen(theEndpointRef, &theTCall);

		if (theResult == kOTNoDataErr)
			{
			// No pending connections, wait till a T_LISTEN or T_DISCONNECT show up or theWaiter is woken by our destructor.
			theResult = theWaiter.Wait();
			if (theResult == kOTCanceledErr)
				return ZRef<ZNetEndpoint>();
			}
		else
			{
			theWaiter.DontWait();

			if (theResult == noErr)
				break;

			if (theResult == kOTCanceledErr)
				return ZRef<ZNetEndpoint>();
			::OTSndDisconnect(theEndpointRef, &theTCall);
			}
		}
		

	ZRef<ZNetEndpoint_TCP_MacOT_Classic> theEndpoint = new ZNetEndpoint_TCP_MacOT_Classic(remoteInetAddress);
	while (true)
		{
		ZOTProviderWaiter theWaiter(&fProvider, T_ACCEPTCOMPLETE, T_LISTEN);
		OTResult theResult = ::OTAccept(theEndpointRef, theEndpoint->GetEndpointRef(), const_cast<TCall*>(&theTCall));
		if (theResult == noErr)
			{
			theResult = theWaiter.Wait();
			return theEndpoint;
			}
		else
			{
			theWaiter.DontWait();
			if (theResult == kOTLookErr)
				{
				OTResult lookResult = ::OTLook(theEndpointRef);
				if (lookResult != T_DISCONNECT)
					break;
				TDiscon discon;
				ZBlockZero(&discon, sizeof(discon));
				OSStatus junk = ::OTRcvDisconnect(theEndpointRef, &discon);
				ZAssertStop(kDebug_OT, junk == noErr);
				ZAssertStop(kDebug_OT, discon.sequence == theTCall.sequence);
				}
			}		
		}
	return ZRef<ZNetEndpoint>();
	}

void ZNetListener_TCP_MacOT_Classic::CancelListen()
	{
	fProvider.WakeAllWaiters();
	}

ip_port ZNetListener_TCP_MacOT_Classic::GetPort()
	{ return fLocalPort; }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP_MacOT_Classic_Classic

ZNetEndpoint_TCP_MacOT_Classic::ZNetEndpoint_TCP_MacOT_Classic(const InetAddress& iRemoteInetAddress)
	{
	this->Internal_Initialize();
	fRemoteHost = iRemoteInetAddress.fHost;
	fRemotePort = iRemoteInetAddress.fPort;
	}

ZNetEndpoint_TCP_MacOT_Classic::ZNetEndpoint_TCP_MacOT_Classic(ip_addr iRemoteHost, ip_port iRemotePort)
	{
	this->Internal_Initialize();
	fRemoteHost = iRemoteHost;
	fRemotePort = iRemotePort;
	
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());

	InetAddress theInetAddress;
	theInetAddress.fAddressType = AF_INET;
	theInetAddress.fPort = fRemotePort;
	theInetAddress.fHost = fRemoteHost;

	TCall theSndCall;
	theSndCall.addr.maxlen = sizeof(InetAddress);
	theSndCall.addr.len = sizeof(InetAddress);
	theSndCall.addr.buf = (unsigned char*)(&theInetAddress);

	theSndCall.opt.maxlen = 0;
	theSndCall.opt.len = 0;
	theSndCall.opt.buf = nil; // no connection options

	theSndCall.udata.maxlen = 0;
	theSndCall.udata.len = 0;
	theSndCall.udata.buf = nil; // no connection data

	theSndCall.sequence = 0; // ignored by OTConnect

	ZOTProviderWaiter theWaiter1(&fProvider, T_CONNECT, T_DISCONNECT);

	OTResult theResult = ::OTConnect(theEndpointRef, &theSndCall, nil);
	if (theResult == kOTNoDataErr)
		{
		OTEventCode theEventCode;
		void* theCookie;
		theResult = theWaiter1.Wait(theEventCode, theCookie);
		ZAssertStop(kDebug_OT, theResult == noErr);
		theResult = ::OTRcvConnect(theEndpointRef, nil);
		if (theResult == kOTLookErr)
			{
			OTResult lookResult = ::OTLook(theEndpointRef);
			ZAssertStop(kDebug_OT, lookResult == T_DISCONNECT);
			::OTRcvDisconnect(theEndpointRef, nil);
			this->Internal_TearDown();
			throw ZNetEx(ZNet::errorGeneric);
			}
		}
	else
		{
		theWaiter1.DontWait();
		ZUnimplemented();
		}
	}

void ZNetEndpoint_TCP_MacOT_Classic::Internal_Initialize()
	{
	OTConfiguration* theConfiguration = ::OTCreateConfiguration(kTCPName);

	ZOTProviderWaiter theWaiter1(&fProvider, T_OPENCOMPLETE);

#if ZCONFIG(OS, Carbon)
	OTResult theResult = ::OTAsyncOpenEndpointInContext(theConfiguration, 0, nil, ZOTProviderWrapper::sNotifierUPP, &fProvider, nil);
#else
	OTResult theResult = ::OTAsyncOpenEndpoint(theConfiguration, 0, nil, ZOTProviderWrapper::sNotifierUPP, &fProvider);
#endif

	if (theResult == noErr)
		theResult = theWaiter1.Wait();
	else
		theWaiter1.DontWait();

	if (theResult != noErr)
		throw ZNetEx(sTranslateError(theResult));

	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());

	ZAssertStop(kDebug_OT, !::OTIsSynchronous(theEndpointRef));

	::OTSetBlocking(theEndpointRef);

	ZOTProviderWaiter theWaiter2(&fProvider, T_BINDCOMPLETE);
	// Don't bind to any specific address.
	theResult = ::OTBind(theEndpointRef, nil, nil);

	if (theResult == noErr)
		theResult = theWaiter2.Wait();
	else
		theWaiter2.DontWait();

	if (theResult != noErr)
		throw ZNetEx(sTranslateError(theResult));
	}

void ZNetEndpoint_TCP_MacOT_Classic::Internal_TearDown()
	{
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());
	ZOTProviderWaiter theWaiter(&fProvider, T_UNBINDCOMPLETE);

	OTResult theResult = ::OTUnbind(theEndpointRef);
	if (theResult == kOTNoError)
		theWaiter.Wait();
	else
		theWaiter.DontWait();
	}

ZNetEndpoint_TCP_MacOT_Classic::~ZNetEndpoint_TCP_MacOT_Classic()
	{
	this->Internal_TearDown();
	}

void ZNetEndpoint_TCP_MacOT_Classic::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());
	if (oCountRead)
		*oCountRead = 0;
	char* localDest = static_cast<char*>(iDest);
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		ZOTProviderWaiter theWaiter(&fProvider, T_DATA, T_DISCONNECT, T_ORDREL);
		OTFlags theFlags = 0;
		OTResult theResult = ::OTRcv(theEndpointRef, localDest, countRemaining, &theFlags);
		if (theResult == kOTNoDataErr)
			{
			theWaiter.Wait();
			}
		else if (theResult > 0)
			{
			theWaiter.DontWait();
			if (oCountRead)
				*oCountRead += theResult;
			localDest += theResult;
			countRemaining -= theResult;
			break;
			}
		else
			{
			theWaiter.DontWait();
			if (theResult != kOTLookErr)
				break;
			OTResult lookResult = ::OTLook(theEndpointRef);
			if (lookResult == T_DISCONNECT)
				::OTRcvDisconnect(theEndpointRef, nil);
			else if (lookResult == T_ORDREL)
				::OTRcvOrderlyDisconnect(theEndpointRef);
			else
				ZDebugLogf(kDebug_OT, ("Unexpected OTLook result %d", lookResult));
			}
		}
	}

size_t ZNetEndpoint_TCP_MacOT_Classic::Imp_CountReadable()
	{
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());

	OTByteCount theCount;
	if (kOTNoError == ::OTCountDataBytes(theEndpointRef, &theCount))
		return theCount;

	return 0;
	}

void ZNetEndpoint_TCP_MacOT_Classic::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());
	if (oCountWritten)
		*oCountWritten = 0;
	const char* localSource = static_cast<const char*>(iSource);
	size_t countRemaining = iCount;
	int delay = 1;
	int delayCount = 10;
	while (countRemaining)
		{
		ZOTProviderWaiter theWaiter(&fProvider, T_GODATA, T_DISCONNECT, T_ORDREL);
		OTResult theResult = ::OTSnd(theEndpointRef, const_cast<char*>(localSource), countRemaining, 0);
		if (theResult == kOTFlowErr)
			{
			theResult = theWaiter.Wait();
			}
		else
			{
			theWaiter.DontWait();

			if (theResult == kOTOutOfMemoryErr)
				{
				// We need to delay for a bit here.
				if (delayCount-- == 0)
					break;
				ZThread::sSleep(delay);
				delay *= 2;
				}
			else
				{
				// Reset the delay.
				delay = 1;
				delayCount = 10;
				if (theResult > 0)
					{
					if (oCountWritten)
						*oCountWritten += theResult;
					localSource += theResult;
					countRemaining -= theResult;
					}
				else
					{
					if (theResult != kOTLookErr)
						break;
					OTResult lookResult = ::OTLook(theEndpointRef);
					if (lookResult == T_DISCONNECT)
						::OTRcvDisconnect(theEndpointRef, nil);
					else if (lookResult == T_ORDREL)
						::OTRcvOrderlyDisconnect(theEndpointRef);
					else
						ZDebugLogf(kDebug_OT, ("Unexpected OTLook result %d", lookResult));
					}
				}
			}
		}
	}

const ZStreamR& ZNetEndpoint_TCP_MacOT_Classic::GetStreamR()
	{ return *this; }

const ZStreamW& ZNetEndpoint_TCP_MacOT_Classic::GetStreamW()
	{ return *this; }

bool ZNetEndpoint_TCP_MacOT_Classic::WaitTillReadable(int iMilliseconds)
	{
	#warning AG NDY
	return false;
	}

void ZNetEndpoint_TCP_MacOT_Classic::SendDisconnect()
	{
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());
	while (true)
		{
		OTResult theResult = ::OTSndOrderlyDisconnect(theEndpointRef);
		if (theResult != kOTLookErr)
			break;

		OTResult lookResult = ::OTLook(theEndpointRef);
		if (lookResult == T_DISCONNECT)
			::OTRcvDisconnect(theEndpointRef, nil);
		else if (lookResult == T_ORDREL)
			::OTRcvOrderlyDisconnect(theEndpointRef);
		else
			ZDebugLogf(kDebug_OT, ("Unexpected OTLook result %d", lookResult));
		}
	}

bool ZNetEndpoint_TCP_MacOT_Classic::ReceiveDisconnect(int iMilliseconds)
	{
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());
	while (true)
		{
		ZOTProviderWaiter theWaiter(&fProvider, T_DATA, T_DISCONNECT, T_ORDREL);
		OTFlags theFlags = 0;
		OTResult theResult = ::OTRcv(theEndpointRef, ZooLib::sGarbageBuffer, sizeof(ZooLib::sGarbageBuffer), &theFlags);
		if (theResult == kOTNoDataErr)
			{
			theWaiter.Wait();
			}
		else if (theResult > 0)
			{
			theWaiter.DontWait();
			}
		else
			{
			theWaiter.DontWait();
			if (theResult != kOTLookErr)
				break;
			OTResult lookResult = ::OTLook(theEndpointRef);
			if (lookResult == T_DISCONNECT)
				::OTRcvDisconnect(theEndpointRef, nil);
			else if (lookResult == T_ORDREL)
				::OTRcvOrderlyDisconnect(theEndpointRef);
			else
				ZDebugLogf(kDebug_OT, ("Unexpected OTLook result %d", lookResult));
			break;
			}
		}
	#warning "Not done yet. We're not really timing out here"
	return true;
	}

void ZNetEndpoint_TCP_MacOT_Classic::Abort()
	{
	EndpointRef theEndpointRef = static_cast<EndpointRef>(fProvider.GetProviderRef());	
	::OTCancelSynchronousCalls(theEndpointRef, kOTCanceledErr);
	::OTSndDisconnect(theEndpointRef, nil);
	fProvider.WakeAllWaiters();
	}

ZNetAddress* ZNetEndpoint_TCP_MacOT_Classic::GetRemoteAddress()
	{ return new ZNetAddress_Internet(fRemoteHost, fRemotePort); }

EndpointRef ZNetEndpoint_TCP_MacOT_Classic::GetEndpointRef()
	{
	return static_cast<EndpointRef>(fProvider.GetProviderRef());
	}


#endif // ZCONFIG_API_Enabled(Internet_MacOT_Classic)
