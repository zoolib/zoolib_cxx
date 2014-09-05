/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZMacFixup.h"

#include "zoolib/ZNet_Internet_MacOT_OSX.h"

#if ZCONFIG_API_Enabled(Net_Internet_MacOT_OSX)

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZMemory.h" // For ZBlockZero

#include ZMACINCLUDE3(CoreServices,CarbonCore,Multiprocessing.h)

#ifndef kDebug_OT
	#define kDebug_OT 1
#endif

using std::string;

namespace ZooLib {

// =================================================================================================
// MARK: - ZMacMP

namespace ZMacMP {

typedef void (*EntryProc)(void*);

void sInvokeInMP(EntryProc iProc, void* iParam);

} // namespace ZMacMP

#if !ZCONFIG(Thread_API, Mac)
void ZMacMP::sInvokeInMP(EntryProc iProc, void* iParam)
	{
	try
		{
		iProc(iParam);
		}
	catch (...)
		{
		ZDebugStopf(1, ("spMPTaskEntry, caught an exception"));
		}
	}
#endif // !ZCONFIG(Thread_API, Mac)


#if ZCONFIG(Thread_API, Mac)

#include "zoolib/ZAtomic.h"
#include "zoolib/ZThreadTM.h"

static MPQueueID spQueue;

namespace { // anonymous

static int spInitCount;
class InitHelper
	{
public:
	InitHelper()
		{
		if (spInitCount++ == 0)
			{
			MPLibraryIsLoaded();
			::MPCreateQueue(&spQueue);
			}
		}

	~InitHelper()
		{
		if (--spInitCount == 0)
			::MPDeleteQueue(spQueue);
		}
	};

InitHelper sInitHelper;

} // anonymous namespace

static ZAtomic_t spMPTaskCount;

static OSStatus spMPTaskEntry(void* iArg)
	{
	while (true)
		{
		void* param1;
		void* param2;
		void* param3;

		if (noErr != ::MPWaitOnQueue(spQueue, &param1, &param2, &param3, kDurationForever))
			break;

		try
			{
			((ZMacMP::EntryProc)param1)(param2);
			}
		catch (...)
			{
			ZDebugStopf(1, ("spMPTaskEntry, caught an exception"));
			}

		::ZThreadTM_Resume(static_cast<ZThreadTM_State*>(param3));

		if (sAtomic_Add(&spMPTaskCount, 1) >= 5)
			{
			sAtomic_Add(&spMPTaskCount, -1);
			break;
			}
		}
	return 0;
	}

void ZMacMP::sInvokeInMP(EntryProc iProc, void* iParam)
	{
	if (::MPTaskIsPreemptive(kInvalidID))
		{
		try
			{
			iProc(iParam);
			}
		catch (...)
			{
			ZDebugStopf(1, ("spMPTaskEntry, caught an exception"));
			}
		}
	else
		{
		ZThreadTM_State* theThread = ZThreadTM_Current();
		if (sAtomic_Add(&spMPTaskCount, -1) <= 0)
			{
			sAtomic_Add(&spMPTaskCount, 1);
			MPTaskID theTaskID;
			OSStatus err = ::MPCreateTask(spMPTaskEntry, nullptr, 64 * 1024, 0, 0, 0, 0, &theTaskID);
			ZAssertStop(1, err == noErr);
			}

		::MPNotifyQueue(spQueue, iProc, iParam, theThread);
		::ZThreadTM_Suspend();
		}
	}
#endif // ZCONFIG(Thread_API, Mac)

// =================================================================================================
// MARK: - Initialize OpenTransport

static OTClientContextPtr spOTClientContextPtr;

namespace { // anonymous

static int spInitCount;
class InitHelper
	{
public:
	InitHelper()
		{
		if (spInitCount++ == 0)
			{
			::InitOpenTransportInContext(kInitOTForExtensionMask, &spOTClientContextPtr);
			(void)MPLibraryIsLoaded();
			}
		}

	~InitHelper()
		{
		if (--spInitCount == 0)
			::CloseOpenTransportInContext(spOTClientContextPtr);
		}
	};

InitHelper sInitHelper;

} // anonymous namespace

// =================================================================================================
// MARK: - Factory functions

namespace { // anonymous

class Make_NameLookup
:	public ZFunctionChain_T<ZRef<ZNetNameLookup>, ZNetName_Internet::LookupParam_t>
	{
public:
	Make_NameLookup() : Base_t(false) {}

	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetNameLookup_Internet_MacOT_OSX(iParam.f0, iParam.f1, iParam.f2);
		return true;
		}
	} sMaker0;


class Make_Listener
:	public ZFunctionChain_T<ZRef<ZNetListener_TCP>, ZNetListener_TCP::MakeParam_t>
	{
public:
	Make_Listener() : Base_t(false) {}

	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetListener_TCP_MacOT_OSX(iParam.f0);
		return true;
		}
	} sMaker1;


class Make_Endpoint
:	public ZFunctionChain_T<ZRef<ZNetEndpoint_TCP>, ZNetEndpoint_TCP::MakeParam4_t>
	{
public:
	Make_Endpoint() : Base_t(false) {}

	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetEndpoint_TCP_MacOT_OSX(iParam.f0, iParam.f1);
		return true;
		}
	} sMaker2;

} // anonymous namespace

// =================================================================================================
// MARK: - Helper functions

static ZNet::Error spTranslateError(OTResult theOTResult)
	{
	if (theOTResult == kOTNoError)
		return ZNet::errorNone;
	return ZNet::errorGeneric;
	}

static OTResult spSetFourByteOption(EndpointRef ep, OTXTILevel level, OTXTIName name, UInt32 value)
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

// =================================================================================================
// MARK: - ZNetNameLookup_Internet_MacOT_OSX

ZNetNameLookup_Internet_MacOT_OSX::ZNetNameLookup_Internet_MacOT_OSX(
	const string& iName, ip_port iPort, size_t iMaxAddresses)
:	fName(iName),
	fPort(iPort),
	fStarted(false),
	fCountAddressesToReturn(iMaxAddresses),
	fCurrentIndex(0)
	{
	if (iName.empty())
		{
		fStarted = true;
		fCountAddressesToReturn = 0;
		}
	}

ZNetNameLookup_Internet_MacOT_OSX::~ZNetNameLookup_Internet_MacOT_OSX()
	{}

struct MPLookup_t
	{
	char* fName;
	InetHostInfo* fInetHostInfo;
	};

void ZNetNameLookup_Internet_MacOT_OSX::Start()
	{
	if (fStarted)
		return;

	fStarted = true;

	if (fCountAddressesToReturn == 0)
		return;

	MPLookup_t theStruct;
	theStruct.fName = const_cast<char*>(fName.c_str());
	theStruct.fInetHostInfo = &fInetHostInfo;
	ZMacMP::sInvokeInMP(sMP_Lookup, &theStruct);
	}

bool ZNetNameLookup_Internet_MacOT_OSX::Finished()
	{
	if (!fStarted)
		return true;

	if (0 == fCountAddressesToReturn)
		return true;

	for (/*no init*/; fCurrentIndex < kMaxHostAddrs; ++fCurrentIndex)
		{
		if (fInetHostInfo.addrs[fCurrentIndex])
			return false;
		}

	return true;
	}

void ZNetNameLookup_Internet_MacOT_OSX::Advance()
	{
	ZAssertStop(kDebug_OT, fStarted);

	if (0 == fCountAddressesToReturn)
		return;

	for (/*no init*/; fCurrentIndex < kMaxHostAddrs; ++fCurrentIndex)
		{
		if (fInetHostInfo.addrs[fCurrentIndex])
			{
			--fCountAddressesToReturn;
			return;
			}
		}
	}

ZRef<ZNetAddress> ZNetNameLookup_Internet_MacOT_OSX::CurrentAddress()
	{
	if (fCurrentIndex < kMaxHostAddrs && fInetHostInfo.addrs[fCurrentIndex])
		return new ZNetAddress_IP4(fInetHostInfo.addrs[fCurrentIndex], fPort);

	return null;
	}

ZRef<ZNetName> ZNetNameLookup_Internet_MacOT_OSX::CurrentName()
	{
	return new ZNetName_Internet(fName, fPort);
	}

void ZNetNameLookup_Internet_MacOT_OSX::sMP_Lookup(void* iParam)
	{
	MPLookup_t* theStruct = static_cast<MPLookup_t*>(iParam);

	OSStatus theErr;
	InetSvcRef theInetSvcRef = ::OTOpenInternetServicesInContext(
		kDefaultInternetServicesPath, 0, &theErr, spOTClientContextPtr);

	if (noErr != theErr)
		return;

	/*OTResult theResult = */::OTInetStringToAddress(
		theInetSvcRef, theStruct->fName, theStruct->fInetHostInfo);

	::OTCloseProvider(theInetSvcRef);
	}

// =================================================================================================
// MARK: - ZNetListener_TCP_MacOT_OSX

struct ListenerConstruct_t
	{
	ZNetListener_TCP_MacOT_OSX* fListener;
	ip_port fLocalPort;
	size_t fListenQueueSize;
	OTResult fResult;
	};

ZNetListener_TCP_MacOT_OSX::ZNetListener_TCP_MacOT_OSX(ip_port iLocalPort)
	{
	// For the moment, remember the port in an instance variable
	// rather than pulling it from fEndpointRef
	fLocalPort = iLocalPort;

	fEndpointRef = 0;

	ListenerConstruct_t theStruct;
	theStruct.fListener = this;
	theStruct.fLocalPort = iLocalPort;
	theStruct.fListenQueueSize = 5;
	ZMacMP::sInvokeInMP(sMP_Constructor, &theStruct);

	if (theStruct.fResult != noErr)
		throw ZNetEx(spTranslateError(theStruct.fResult));
	}

void ZNetListener_TCP_MacOT_OSX::sMP_Constructor(void* iParam)
	{
	ListenerConstruct_t* theStruct = static_cast<ListenerConstruct_t*>(iParam);

	theStruct->fListener->fEndpointRef = ::OTOpenEndpointInContext(
		::OTCreateConfiguration("tilisten, tcp"),
		0, nullptr, &theStruct->fResult, spOTClientContextPtr);

	if (theStruct->fResult == noErr)
		theStruct->fResult = ::OTSetBlocking(theStruct->fListener->fEndpointRef);

	if (theStruct->fResult == noErr)
		{
		theStruct->fResult = spSetFourByteOption(theStruct->fListener->fEndpointRef,
			INET_IP, kIP_REUSEADDR, 1);
		}

	if (theStruct->fResult == noErr)
		{
		InetAddress theInetAddress;
		::OTInitInetAddress(&theInetAddress, theStruct->fLocalPort, kOTAnyInetAddress);
		TBind bindReq;
		bindReq.addr.buf = (UInt8*)&theInetAddress;
		bindReq.addr.len = sizeof(theInetAddress);
		bindReq.qlen = theStruct->fListenQueueSize;
		theStruct->fResult =
			::OTBind(theStruct->fListener->fEndpointRef, &bindReq, nullptr);
		}

	if (theStruct->fResult != noErr)
		{
		if (theStruct->fListener->fEndpointRef)
			::OTCloseProvider(theStruct->fListener->fEndpointRef);
		theStruct->fListener->fEndpointRef = nullptr;
		}
	}

struct ListenerGeneric_t
	{
	EndpointRef fEndpointRef;
	};

ZNetListener_TCP_MacOT_OSX::~ZNetListener_TCP_MacOT_OSX()
	{
	ListenerGeneric_t theStruct;
	theStruct.fEndpointRef = fEndpointRef;
	ZMacMP::sInvokeInMP(sMP_Destructor, &theStruct);
	fEndpointRef = nullptr;
 	}

void ZNetListener_TCP_MacOT_OSX::sMP_Destructor(void* iParam)
	{
	ListenerGeneric_t* theStruct = static_cast<ListenerGeneric_t*>(iParam);
	if (theStruct->fEndpointRef)
		::OTCloseProvider(theStruct->fEndpointRef);
	}

static OSStatus spOTAcceptQ(EndpointRef listener, EndpointRef worker, TCall* call)
	// My own personal wrapper around the OTAccept routine that handles
	// the connection attempt disappearing cleanly.
	{
	// First try the Accept.
	OSStatus err = ::OTAccept(listener, worker, call);

	// If that fails with a look error, let's see what the problem is.
	if (err == kOTLookErr)
		{
		OTResult look = ::OTLook(listener);

		// Only two async events should be able to cause Accept to "look", namely
		// T_LISTEN and T_DISCONNECT. However, the "tilisten" module prevents
		// further connection attempts coming up while we're still thinking about
		// this one, so the only event that should come up is T_DISCONNECT.

		ZAssertStop(kDebug_OT, look == T_DISCONNECT);
		if (look == T_DISCONNECT)
			{
			// If we get a T_DISCONNECT, it should be for the current pending
			// connection attempt. We receive the disconnect info and check
			// the sequence number against that in the call. If they match,
			// the connection attempt disappeared and we return kOTNoDataErr.
			// If they don't match, that's weird.

			TDiscon discon;
			::OTMemzero(&discon, sizeof(discon));

			OSStatus junk = ::OTRcvDisconnect(listener, &discon);
			ZAssertStop(kDebug_OT, junk == noErr);

			if (discon.sequence == call->sequence)
				{
				err = kOTNoDataErr;
				}
			else
				{
				ZAssertStop(kDebug_OT, false);
				// Leave err set to kOTLookErr.
				}
			}
		}
	return err;
	}

struct Listen_t
	{
	ZNetListener_TCP_MacOT_OSX* fListener;
	EndpointRef fAcceptedEndpointRef;
	InetAddress fInetAddress;
	};

ZRef<ZNetEndpoint> ZNetListener_TCP_MacOT_OSX::Listen()
	{
	Listen_t theStruct;
	theStruct.fListener = this;
	ZMacMP::sInvokeInMP(sMP_Listen, &theStruct);
	if (theStruct.fAcceptedEndpointRef)
		{
		return new ZNetEndpoint_TCP_MacOT_OSX(
			theStruct.fAcceptedEndpointRef, theStruct.fInetAddress);
		}

	return null;
	}

void ZNetListener_TCP_MacOT_OSX::sMP_Listen(void* iParam)
	{
	Listen_t* theStruct = static_cast<Listen_t*>(iParam);

	TCall theTCall;
	ZMemZero_T(theTCall);
	theTCall.addr.buf = reinterpret_cast<UInt8*>(&theStruct->fInetAddress);
	theTCall.addr.maxlen = sizeof(theStruct->fInetAddress);

	OTResult theOTResult = ::OTListen(theStruct->fListener->fEndpointRef, &theTCall);
	if (theOTResult == noErr)
		{
		EndpointRef acceptedEndpointRef = ::OTOpenEndpointInContext(
			::OTCreateConfiguration("tcp"),
			0, nullptr, &theOTResult, spOTClientContextPtr);

		if (acceptedEndpointRef)
			{
			theOTResult = ::OTSetBlocking(acceptedEndpointRef);
			if (theOTResult == noErr)
				theOTResult = spOTAcceptQ(
					theStruct->fListener->fEndpointRef, acceptedEndpointRef, &theTCall);

			if (theOTResult == noErr)
				{
				theStruct->fAcceptedEndpointRef = acceptedEndpointRef;
				return;
				}
			else if (theOTResult == kOTNoDataErr)
				{
				ZAssertStop(kDebug_OT, acceptedEndpointRef);
				::OTCloseProvider(acceptedEndpointRef);
				theOTResult = noErr;
				}
			}
		if (theOTResult)
			::OTSndDisconnect(theStruct->fListener->fEndpointRef, &theTCall);
		}
	theStruct->fAcceptedEndpointRef = nullptr;
	}

void ZNetListener_TCP_MacOT_OSX::CancelListen()
	{
	ListenerGeneric_t theStruct;
	theStruct.fEndpointRef = fEndpointRef;
	ZMacMP::sInvokeInMP(sMP_CancelListen, &theStruct);
	}

void ZNetListener_TCP_MacOT_OSX::sMP_CancelListen(void* iParam)
	{
	ListenerGeneric_t* theStruct = static_cast<ListenerGeneric_t*>(iParam);
	::OTCancelSynchronousCalls(theStruct->fEndpointRef, kOTCanceledErr);
	}

ZRef<ZNetAddress> ZNetListener_TCP_MacOT_OSX::GetAddress()
	{ return null; }

ip_port ZNetListener_TCP_MacOT_OSX::GetPort()
	{ return fLocalPort; }

// =================================================================================================
// MARK: - ZNetEndpoint_TCP_MacOT_OSX

ZNetEndpoint_TCP_MacOT_OSX::ZNetEndpoint_TCP_MacOT_OSX(
	EndpointRef iEndpointRef, InetAddress& iRemoteInetAddress)
	{
	fEndpointRef = iEndpointRef;
	fRemoteHost = iRemoteInetAddress.fHost;
	fRemotePort = iRemoteInetAddress.fPort;
	}

struct EndpointConstruct_t
	{
	ZNetEndpoint_TCP_MacOT_OSX* fEndpoint;
	ip_addr fRemoteHost;
	ip_port fRemotePort;
	OTResult fResult;
	};

ZNetEndpoint_TCP_MacOT_OSX::ZNetEndpoint_TCP_MacOT_OSX(ip_addr iRemoteHost, ip_port iRemotePort)
	{
	fEndpointRef = nullptr;
	fRemoteHost = iRemoteHost;
	fRemotePort = iRemotePort;

	EndpointConstruct_t theStruct;
	theStruct.fEndpoint = this;
	theStruct.fRemoteHost = fRemoteHost;
	theStruct.fRemotePort = fRemotePort;
	ZMacMP::sInvokeInMP(sMP_Constructor, &theStruct);

	if (theStruct.fResult != noErr)
		throw ZNetEx(spTranslateError(theStruct.fResult));
	}

void ZNetEndpoint_TCP_MacOT_OSX::sMP_Constructor(void* iParam)
	{
	EndpointConstruct_t* theStruct = static_cast<EndpointConstruct_t*>(iParam);

	theStruct->fEndpoint->fEndpointRef = ::OTOpenEndpointInContext(
		::OTCreateConfiguration(kTCPName),
		0, nullptr, &theStruct->fResult, spOTClientContextPtr);

	if (theStruct->fResult == noErr)
		{
		ZAssertStop(kDebug_OT, ::OTIsSynchronous(theStruct->fEndpoint->fEndpointRef));
		theStruct->fResult = ::OTSetBlocking(theStruct->fEndpoint->fEndpointRef);
		}

	if (theStruct->fResult == noErr)
		theStruct->fResult = ::OTBind(theStruct->fEndpoint->fEndpointRef, nullptr, nullptr);

	if (theStruct->fResult == noErr)
		{
		InetAddress theInetAddress;
		theInetAddress.fAddressType = AF_INET;
		theInetAddress.fPort = theStruct->fRemotePort;
		theInetAddress.fHost = theStruct->fRemoteHost;

		TCall theSndCall;
		theSndCall.addr.buf = reinterpret_cast<UInt8*>(&theInetAddress);
		theSndCall.addr.len = sizeof(InetAddress);
		theSndCall.addr.maxlen = sizeof(InetAddress);

		theSndCall.opt.buf = nullptr; // no connection options
		theSndCall.opt.len = 0;
		theSndCall.opt.maxlen = 0;

		theSndCall.udata.buf = nullptr; // no connection data
		theSndCall.udata.len = 0;
		theSndCall.udata.maxlen = 0;

		OTResult theResult = ::OTConnect(
			theStruct->fEndpoint->fEndpointRef, &theSndCall, nullptr);

		if (theResult == kOTLookErr)
			theResult = ::OTLook(theStruct->fEndpoint->fEndpointRef);

		if (theResult == T_DISCONNECT)
			{
			::OTRcvDisconnect(theStruct->fEndpoint->fEndpointRef, nullptr);
			theStruct->fResult = kEAGAINErr; //??
			}
		}

	if (theStruct->fResult != noErr)
		{
		if (theStruct->fEndpoint->fEndpointRef)
			::OTCloseProvider(theStruct->fEndpoint->fEndpointRef);
		theStruct->fEndpoint->fEndpointRef = nullptr;
		}
	}

struct EndpointGeneric_t
	{
	EndpointRef fEndpointRef;
	};

ZNetEndpoint_TCP_MacOT_OSX::~ZNetEndpoint_TCP_MacOT_OSX()
	{
	if (fEndpointRef)
		{
		EndpointGeneric_t theStruct;
		theStruct.fEndpointRef = fEndpointRef;
		ZMacMP::sInvokeInMP(sMP_Destructor, &theStruct);
		fEndpointRef = 0;
		}
	}

void ZNetEndpoint_TCP_MacOT_OSX::sMP_Destructor(void* iParam)
	{
	EndpointGeneric_t* theStruct = static_cast<EndpointGeneric_t*>(iParam);
	::OTCloseProvider(theStruct->fEndpointRef);
	}

const ZStreamRCon& ZNetEndpoint_TCP_MacOT_OSX::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& ZNetEndpoint_TCP_MacOT_OSX::GetStreamWCon()
	{ return *this; }

ZRef<ZNetAddress> ZNetEndpoint_TCP_MacOT_OSX::GetRemoteAddress()
	{ return new ZNetAddress_IP4(fRemoteHost, fRemotePort); }

struct Imp_Read_t
	{
	EndpointRef fEndpointRef;
	void* fDest;
	size_t fCount;
	size_t* fCountRead;
	};

void ZNetEndpoint_TCP_MacOT_OSX::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	Imp_Read_t theStruct;
	theStruct.fEndpointRef = fEndpointRef;
	theStruct.fDest = oDest;
	theStruct.fCount = iCount;
	theStruct.fCountRead = oCountRead;
	ZMacMP::sInvokeInMP(sMP_Imp_Read, &theStruct);
	}

void ZNetEndpoint_TCP_MacOT_OSX::sMP_Imp_Read(void* iParam)
	{
	Imp_Read_t* theStruct = static_cast<Imp_Read_t*>(iParam);

	if (theStruct->fCountRead)
		*theStruct->fCountRead = 0;
	char* localDest = static_cast<char*>(theStruct->fDest);
	size_t countRemaining = theStruct->fCount;
	while (countRemaining)
		{
		OTFlags theFlags = 0;
		OTResult theResult = ::OTRcv(theStruct->fEndpointRef, localDest, countRemaining, &theFlags);
		if (theResult > 0)
			{
			if (theStruct->fCountRead)
				*theStruct->fCountRead += theResult;
			localDest += theResult;
			countRemaining -= theResult;
			break;
			}
		else
			{
			if (theResult != kOTLookErr)
				break;
			OTResult lookResult = ::OTLook(theStruct->fEndpointRef);
			if (lookResult == T_DISCONNECT)
				::OTRcvDisconnect(theStruct->fEndpointRef, nullptr);
			else if (lookResult == T_ORDREL)
				::OTRcvOrderlyDisconnect(theStruct->fEndpointRef);
			else
				ZDebugLogf(kDebug_OT, ("Unexpected OTLook result %d", lookResult));
			}
		}
	}

struct GetCountReadable_t
	{
	EndpointRef fEndpointRef;
	OTByteCount fCountReadable;
	};

size_t ZNetEndpoint_TCP_MacOT_OSX::Imp_CountReadable()
	{
	GetCountReadable_t theStruct;
	theStruct.fEndpointRef = fEndpointRef;
	ZMacMP::sInvokeInMP(sMP_GetCountReadable, &theStruct);
	return theStruct.fCountReadable;
	}

void ZNetEndpoint_TCP_MacOT_OSX::sMP_GetCountReadable(void* iParam)
	{
	GetCountReadable_t* theStruct = static_cast<GetCountReadable_t*>(iParam);
	if (kOTNoError != ::OTCountDataBytes(theStruct->fEndpointRef, &theStruct->fCountReadable))
		theStruct->fCountReadable = 0;
	}

bool ZNetEndpoint_TCP_MacOT_OSX::Imp_WaitReadable(double iTimeout)
	{
	//#warning "Not done yet. We're not really timing out here"
	return this->Imp_CountReadable();
	}

struct Imp_Write_t
	{
	EndpointRef fEndpointRef;
	const void* fSource;
	size_t fCount;
	size_t* fCountWritten;
	};

void ZNetEndpoint_TCP_MacOT_OSX::Imp_Write(
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	Imp_Write_t theStruct;
	theStruct.fEndpointRef = fEndpointRef;
	theStruct.fSource = iSource;
	theStruct.fCount = iCount;
	theStruct.fCountWritten = oCountWritten;
	ZMacMP::sInvokeInMP(sMP_Imp_Write, &theStruct);
	}

void ZNetEndpoint_TCP_MacOT_OSX::sMP_Imp_Write(void* iParam)
	{
	Imp_Write_t* theStruct = static_cast<Imp_Write_t*>(iParam);

	if (theStruct->fCountWritten)
		*theStruct->fCountWritten = 0;
	const char* localSource = static_cast<const char*>(theStruct->fSource);
	size_t countRemaining = theStruct->fCount;
	while (countRemaining)
		{
		OTResult theResult = ::OTSnd(theStruct->fEndpointRef,
			const_cast<char*>(localSource), countRemaining, 0);

		if (theResult > 0)
			{
			if (theStruct->fCountWritten)
				*theStruct->fCountWritten += theResult;
			localSource += theResult;
			countRemaining -= theResult;
			}
		else
			{
			if (theResult != kOTLookErr)
				break;
			OTResult lookResult = ::OTLook(theStruct->fEndpointRef);
			if (lookResult == T_DISCONNECT)
				::OTRcvDisconnect(theStruct->fEndpointRef, nullptr);
			else if (lookResult == T_ORDREL)
				::OTRcvOrderlyDisconnect(theStruct->fEndpointRef);
			else
				ZDebugLogf(kDebug_OT, ("Unexpected OTLook result %d", lookResult));
			}
		}
	}

struct ReceiveDisconnect_t
	{
	EndpointRef fEndpointRef;
	int fMilliseconds;
	bool fResult;
	};

bool ZNetEndpoint_TCP_MacOT_OSX::Imp_ReceiveDisconnect(double iTimeout)
	{
	ReceiveDisconnect_t theStruct;
	theStruct.fEndpointRef = fEndpointRef;
	theStruct.fMilliseconds = iTimeout * 1e3;
	ZMacMP::sInvokeInMP(sMP_ReceiveDisconnect, &theStruct);
	return theStruct.fResult;
	}

void ZNetEndpoint_TCP_MacOT_OSX::sMP_ReceiveDisconnect(void* iParam)
	{
	ReceiveDisconnect_t* theStruct = static_cast<ReceiveDisconnect_t*>(iParam);

	while (true)
		{
		OTFlags theFlags = 0;
		OTResult theResult = ::OTRcv(theStruct->fEndpointRef,
			sGarbageBuffer, sizeof(sGarbageBuffer), &theFlags);

		if (theResult < 0)
			{
			if (theResult != kOTLookErr)
				break;
			OTResult lookResult = ::OTLook(theStruct->fEndpointRef);
			if (lookResult == T_DISCONNECT)
				{
				::OTRcvDisconnect(theStruct->fEndpointRef, nullptr);
				break;
				}
			else if (lookResult == T_ORDREL)
				{
				::OTRcvOrderlyDisconnect(theStruct->fEndpointRef);
				break;
				}
			else
				{
				ZDebugLogf(kDebug_OT, ("Unexpected OTLook result %d", lookResult));
				}
			}
		}
	theStruct->fResult = true;
	//#warning "Not done yet. We're not really timing out here"
	}

void ZNetEndpoint_TCP_MacOT_OSX::Imp_SendDisconnect()
	{
	EndpointGeneric_t theStruct;
	theStruct.fEndpointRef = fEndpointRef;
	ZMacMP::sInvokeInMP(sMP_SendDisconnect, &theStruct);
	}

void ZNetEndpoint_TCP_MacOT_OSX::sMP_SendDisconnect(void* iParam)
	{
	EndpointGeneric_t* theStruct = static_cast<EndpointGeneric_t*>(iParam);

	while (true)
		{
		OTResult theResult = ::OTSndOrderlyDisconnect(theStruct->fEndpointRef);
		if (theResult != kOTLookErr)
			break;

		OTResult lookResult = ::OTLook(theStruct->fEndpointRef);
		if (lookResult == T_DISCONNECT)
			::OTRcvDisconnect(theStruct->fEndpointRef, nullptr);
		else if (lookResult == T_ORDREL)
			::OTRcvOrderlyDisconnect(theStruct->fEndpointRef);
		else
			ZDebugLogf(kDebug_OT, ("Unexpected OTLook result %d", lookResult));
		}
	}

void ZNetEndpoint_TCP_MacOT_OSX::Imp_Abort()
	{
	EndpointGeneric_t theStruct;
	theStruct.fEndpointRef = fEndpointRef;
	ZMacMP::sInvokeInMP(sMP_Abort, &theStruct);
	}

void ZNetEndpoint_TCP_MacOT_OSX::sMP_Abort(void* iParam)
	{
	EndpointGeneric_t* theStruct = static_cast<EndpointGeneric_t*>(iParam);
	::OTCancelSynchronousCalls(theStruct->fEndpointRef, kOTCanceledErr);
	::OTSndDisconnect(theStruct->fEndpointRef, nullptr);
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Internet_MacOT_OSX)
