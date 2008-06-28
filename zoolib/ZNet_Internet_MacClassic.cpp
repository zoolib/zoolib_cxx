/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZNet_Internet_MacClassic.h"

// =================================================================================================
#if 0//
#include "zoolib/ZMemory.h"

struct ZNet_Internet_MacClassic::Threaded_UDPiopb : public ZThreadTM_PBHeader
	{
	UDPiopb fParamBlock;

	Threaded_UDPiopb();
	void ZeroContents();
	};

// ==================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Internet_MacClassic

static bool sMacTCPResolverOpened = false;

ResultUPP ZNetNameLookup_Internet_MacClassic::sDNRResultProcUPP = NewResultProc(ZNetNameLookup_Internet_MacClassic::sDNRResultProc);

ZNetNameLookup_Internet_MacClassic::ZNetNameLookup_Internet_MacClassic(const string& inName, ip_port inPort, size_t inMaxAddresses)
:	fNetName(inName, inPort),
	fStarted(false),
	fCountAddressesToReturn(inMaxAddresses),
	fCurrentAddress(nil),
	fNextIndex(0)
	{
	if (0 == inName.size())
		{
		fStarted = true;
		fCountAddressesToReturn = 0;
		}
	}

ZNetNameLookup_Internet_MacClassic::~ZNetNameLookup_Internet_MacClassic()
	{
	delete fCurrentAddress;
	}

void ZNetNameLookup_Internet_MacClassic::Start()
	{
	if (fStarted)
		return;

	fStarted = true;

	if (fCountAddressesToReturn == 0)
		return;

	if (!sMacTCPResolverOpened)
		{
		if (::OpenResolver(nil) != noErr)
			return;
		sMacTCPResolverOpened = true;
		}

	ZThreadTM_State* theThread = ZThreadTM_Current();
	if (cacheFault == ::StrToAddr(const_cast<char*>(fNetName.GetName().c_str()), &fHostInfo, sDNRResultProcUPP, reinterpret_cast<char*>(theThread)))
		ZThreadTM_Suspend();
	}

bool ZNetNameLookup_Internet_MacClassic::Finished()
	{
	if (!fStarted)
		return true;

	if (0 == fCountAddressesToReturn)
		return true;

	if (fHostInfo.rtnCode != noErr)
		return true;

	while (fNextIndex < NUM_ALT_ADDRS)
		{
		if (fHostInfo.addr[fNextIndex])
			return false;
		++fNextIndex;
		}
	return true;
	}

void ZNetNameLookup_Internet_MacClassic::Next()
	{
	ZAssertStop(2, fStarted);

	this->Internal_GrabNextResult();
	}

const ZNetAddress& ZNetNameLookup_Internet_MacClassic::CurrentAddress()
	{
	if (!fCurrentAddress)
		this->Internal_GrabNextResult();

	ZAssertStop(2, fCurrentAddress);
	return *fCurrentAddress;
	}

const ZNetName& ZNetNameLookup_Internet_MacClassic::CurrentName()
	{
	if (!fCurrentAddress)
		this->Internal_GrabNextResult();

	ZAssertStop(2, fCurrentAddress);
	return fNetName;
	}

void ZNetNameLookup_Internet_MacClassic::Internal_GrabNextResult()
	{
	ZAssertStop(2, fHostInfo.rtnCode == noErr);
	while (fNextIndex < NUM_ALT_ADDRS)
		{
		if (fHostInfo.addr[fNextIndex])
			{
			delete fCurrentAddress;
			fCurrentAddress = new ZNetAddress_Internet(fHostInfo.addr[fNextIndex], fNetName.GetPort());
			++fNextIndex;
			--fCountAddressesToReturn;
			break;
			}
		++fNextIndex;
		}
	}

pascal void ZNetNameLookup_Internet_MacClassic::sDNRResultProc(struct hostInfo* inHostInfo, Ptr inUserDataPtr)
	{ ZThreadTM_Resume(reinterpret_cast<ZThreadTM_State*>(inUserDataPtr)); }

// ==================================================
#pragma mark -
#pragma mark * ZNet_Internet_MacClassic

ZNet_Internet_MacClassic::Threaded_TCPiopb::Threaded_TCPiopb()
	{
	ZBlockSet(&fParamBlock, sizeof(fParamBlock), 0);
	}

void ZNet_Internet_MacClassic::Threaded_TCPiopb::ZeroContents()
	{
	ZBlockSet(&fParamBlock, sizeof(fParamBlock), 0);
	}

ZNet_Internet_MacClassic::Threaded_UDPiopb::Threaded_UDPiopb()
	{
	ZBlockSet(&fParamBlock, sizeof(fParamBlock), 0);
	}

void ZNet_Internet_MacClassic::Threaded_UDPiopb::ZeroContents()
	{
	ZBlockSet(&fParamBlock, sizeof(fParamBlock), 0);
	}

OSErr ZNet_Internet_MacClassic::sGetTCPRefNum(short& theRefNum)
	{
	if (sMacTCPRefNum == 0)
		{
		OSErr err = OpenDriver("\p.IPP", &sMacTCPRefNum);
		if (err != noErr)
			return err;
		}
	theRefNum = sMacTCPRefNum;
	return noErr;
	}

ZNet::Error ZNet_Internet_MacClassic::sTranslateError(OSErr inNativeError)
	{
	ZNet::Error theError = ZNet::errorGeneric;
	switch (inNativeError)
		{
		case noErr:
			theError = ZNet::errorNone; break;
		case inProgress:
		case ipBadLapErr:
		case ipBadCnfgErr:
		case ipNoCnfgErr:
		case ipLoadErr:
		case ipBadAddr:
		case connectionClosing:
		case invalidLength:
		case connectionExists:
		case connectionDoesntExist:
		case insufficientResources:
		case invalidStreamPtr:
		case streamAlreadyOpen:
		case connectionTerminated:
		case invalidBufPtr:
		case invalidRDS:
//		case invalidWDS:
		case openFailed:
		case commandTimeout:
		case duplicateSocket:
		case ipDontFragErr:
		case ipDestDeadErr:
		case icmpEchoTimeoutErr:
		case ipNoFragMemErr:
		case ipRouteErr:
		case nameSyntaxErr:
		case cacheFault:
		case noResultProc:
		case noNameServer:
		case authNameErr:
		case noAnsErr:
		case dnrErr:
		case outOfMemory:
		default:
			break;
		}
	return theError;
	}

TCPNotifyUPP ZNet_Internet_MacClassic::sNotificationProcUPP = NewTCPNotifyProc(ZNet_Internet_MacClassic::sNotificationProc);
short ZNet_Internet_MacClassic::sMacTCPRefNum = 0;

#if 0
size_t ZNet_Internet_MacClassic::sLookup_FromAddress(const ZNetAddress* inNetAddress, ip_addr inIPAddress, ip_port inPort, ZNet::LookupHandler* inHandler, size_t inMaxAddresses)
	{
	ZAssertStop(2, inNetAddress);
	if (inIPAddress == 0)
		return 0;

	if (!sResolverOpened)
		{
		if (::OpenResolver(nil) != noErr)
			return 0;
		sResolverOpened = true;
		}
	hostInfo theHostInfo;
	ZThreadTM_State* theThreadTMState = ZThreadTM_SetupForAsyncCompletion();
	OSErr err = ::AddrToName(inIPAddress, &theHostInfo, sDNRResultProcUPP, reinterpret_cast<char*>(theThreadTMState));
	ZThreadTM_WaitForAsyncCompletion(err == cacheFault);
	err = theHostInfo.rtnCode;
	if (err == noErr)
		{
		ZNetName_Internet* theName = new ZNetName_Internet(theHostInfo.cname, inPort);
		inHandler->LookupCallback_HeresAName(*inNetAddress, theName, true);
		return 1;
		}
	return 0;
	}

size_t ZNet_Internet_MacClassic::sLookup_FromName(const ZNetName* inNetName, const string& inName, ip_port inPort, ZNet::LookupHandler* inHandler, size_t inMaxAddresses)
	{
	ZAssertStop(2, inNetName);
	if (inName.size() <= 0)
		return 0;

	if (!sResolverOpened)
		{
		if (::OpenResolver(nil) != noErr)
			return 0;
		sResolverOpened = true;
		}

	hostInfo theHostInfo;
	ZThreadTM_State* theThreadTMState = ZThreadTM_SetupForAsyncCompletion();
	OSErr err = ::StrToAddr(const_cast<char*>(inName.c_str()), &theHostInfo, sDNRResultProcUPP, reinterpret_cast<char*>(theThreadTMState));
	ZThreadTM_WaitForAsyncCompletion(err == cacheFault);
	err = theHostInfo.rtnCode;
	if (err == noErr)
		{
		size_t count = 0;
		for (long x = 0; x < NUM_ALT_ADDRS; ++x)
			{
			if (theHostInfo.addr[x] != 0)
				{
				ZNetAddress_Internet* theAddress = new ZNetAddress_Internet(theHostInfo.addr[x], inPort);
				inHandler->LookupCallback_HeresAnAddress(*inNetName, theAddress, x == 0);
				++count;
				}
			}
		return count;
		}
	return 0;
	}
#endif


DEFINE_API(void) ZNet_Internet_MacClassic::sNotificationProc(StreamPtr inTCPStream, unsigned short inEventCode, Ptr inUserDataPtr, unsigned short inTerminReason, struct ICMPReport* inICMPMessage)
	{}

// ==================================================
#pragma mark -
#pragma mark * ZNetListener_TCP_MacClassic

ZNetListener_TCP_MacClassic::MacTCPControlBlock::MacTCPControlBlock(size_t inBufferSize)
:	fBuffer(nil)
	{ fBuffer = new char[inBufferSize]; }

ZNetListener_TCP_MacClassic::MacTCPControlBlock::~MacTCPControlBlock()
	{
	delete[] fBuffer;
	}

ZNetListener_TCP_MacClassic::ZNetListener_TCP_MacClassic(ip_port iLocalPort, size_t iListenQueueSize)
:	fLocalPort(iLocalPort),
	fListenQueueSize(iListenQueueSize)
	{
	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);
	if (err != noErr)
		throw ZNetEx(ZNet_Internet_MacClassic::sTranslateError(err));

	fBufferSize = 4096;

	if (fListenQueueSize > 10)
		fListenQueueSize = 10; // Let's not max out MacTCP *too* much

	// Get our listening sockets up
	this->ReplenishListeningSockets();

	// If there are no pending control blocks, then we probably don't have a functioning stack
	if (fPendingControlBlocks.size() == 0 && fCompletedControlBlocks.size() == 0)
		throw ZNetEx(ZNet::errorGeneric);
	}

ZNetListener_TCP_MacClassic::~ZNetListener_TCP_MacClassic()
	{
	// Acquire the access mutex
	ZMutexLocker accessLocker(fAccessMutex);

	// Setting our queue size to zero is the indication that we're deleting
	fListenQueueSize = 0;

	// First terminate abort any outstanding listens
	short theMacTCPRefNum;
	if (ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum) == noErr)
		{
		for (vector<MacTCPControlBlock*>::iterator i = fPendingControlBlocks.begin(); i != fPendingControlBlocks.end(); ++i)
			{
			Threaded_TCPiopb paramBlock;
			paramBlock.fParamBlock.tcpStream = (*i)->fStreamPtr;
			paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
			paramBlock.fParamBlock.csCode = TCPAbort;
			OSErr err = ZThreadTM_PBControlAndBlock(&paramBlock);
			}
		}
	// Release the access mutex, which will allow any pending Listen to move on to
	// shift completed listens from the pending list to the completed list
	accessLocker.Release();

	// Wait till the listen exits
	ZMutexLocker listenLocker(fListenMutex);

	// Now release and delete the control blocks
	if (ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum) == noErr)
		{
		for (vector<MacTCPControlBlock*>::iterator i = fPendingControlBlocks.begin(); i != fPendingControlBlocks.end(); ++i)
			{
			Threaded_TCPiopb paramBlock;
			paramBlock.fParamBlock.tcpStream = (*i)->fStreamPtr;
			paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
			paramBlock.fParamBlock.csCode = TCPRelease;
			OSErr err = ZThreadTM_PBControlAndBlock(&paramBlock);
			delete *i;
			}
		for (vector<MacTCPControlBlock*>::iterator i = fCompletedControlBlocks.begin(); i != fCompletedControlBlocks.end(); ++i)
			{
			Threaded_TCPiopb paramBlock;
			paramBlock.fParamBlock.tcpStream = (*i)->fStreamPtr;
			paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
			paramBlock.fParamBlock.csCode = TCPRelease;
			OSErr err = ZThreadTM_PBControlAndBlock(&paramBlock);
			delete *i;
			}
		}
	}

ZRef<ZNetEndpoint> ZNetListener_TCP_MacClassic::Listen()
	{
	ZMutexLocker listenLocker(fListenMutex);

	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);
	if (err != noErr)
		return nil;

	ZRef<ZNetEndpoint> theEndpoint;

	ZMutexLocker accessLocker(fAccessMutex);
	while (fListenQueueSize > 0)
		{
		// Refill our listening sockets list
		this->ReplenishListeningSockets();

		// Build a list of our pending control block's param blocks, and get the thread infrastructure
		// to wait for any of them to complete
		if (fCompletedControlBlocks.size() == 0 && fPendingControlBlocks.size() != 0)
			{
			vector<ZThreadTM_PBHeader*> waitPtrs;
			waitPtrs.reserve(fPendingControlBlocks.size());
			for (vector<MacTCPControlBlock*>::iterator i = fPendingControlBlocks.begin(); i != fPendingControlBlocks.end(); ++i)
				waitPtrs.push_back(&(*i)->fPB);
			accessLocker.Release();
			ZThreadTM_WaitForAsyncCompletionMulti(&waitPtrs.at(0), waitPtrs.size());
			// When we get to here, one of the entries in waitForPtrs will have completed.
			accessLocker.Acquire();
			}

		// Move all completed pending listens onto our completed vector, and remove them from fPendingControlBlocks
		for (vector<MacTCPControlBlock*>::iterator i = fPendingControlBlocks.begin(); i != fPendingControlBlocks.end();)
			{
			if ((*i)->fPB.fParamBlock.ioResult != 1)
				{
				OSErr err = (*i)->fPB.fParamBlock.ioResult;
				if (err == noErr)
					{
					fCompletedControlBlocks.push_back(*i);
					fPendingControlBlocks.erase(i);
					}
				else
					{
					Threaded_TCPiopb paramBlock;
					paramBlock.fParamBlock.tcpStream = (*i)->fStreamPtr;
					paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
					paramBlock.fParamBlock.csCode = TCPRelease;
					OSErr err = ZThreadTM_PBControlAndBlock(&paramBlock);
					delete *i;
					fPendingControlBlocks.erase(i);
					}
				}
			else
				{
				++i;
				}
			}

		// Bail out if our listen queue has been zeroed (in our destructor)
		if (fListenQueueSize == 0)
			break;
		// Pull a completed control block off the list
		if (fCompletedControlBlocks.size() > 0)
			{
			MacTCPControlBlock* theControlBlock = fCompletedControlBlocks[0];
			fCompletedControlBlocks.erase(fCompletedControlBlocks.begin());
			try
				{
				theEndpoint = new ZNetEndpoint_TCP_MacClassic(theControlBlock);
				}
			catch (...)
				{}
			}
		if (theEndpoint)
			break;
		}
	return theEndpoint;
	}

void ZNetListener_TCP_MacClassic::ReplenishListeningSockets()
	{
	ZMutexLocker accessLocker(fAccessMutex);

	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);
	if (err != noErr)
		return;

	// Make sure we have our full complement of listen blocks
	vector<MacTCPControlBlock*> createdControlBlocks;
	while (createdControlBlocks.size() + fPendingControlBlocks.size() < fListenQueueSize)
		{
		MacTCPControlBlock* aMacTCPControlBlock = new MacTCPControlBlock(fBufferSize);
		aMacTCPControlBlock->fPB.fParamBlock.ioCRefNum = theMacTCPRefNum;
		aMacTCPControlBlock->fPB.fParamBlock.csParam.create.rcvBuff = aMacTCPControlBlock->fBuffer;
		aMacTCPControlBlock->fPB.fParamBlock.csParam.create.rcvBuffLen = fBufferSize;
		aMacTCPControlBlock->fPB.fParamBlock.csParam.create.notifyProc = nil; // sNotificationProcUPP;
		aMacTCPControlBlock->fPB.fParamBlock.csParam.create.userDataPtr = nil;
		aMacTCPControlBlock->fPB.fParamBlock.csCode = TCPCreate;
		OSErr err = ZThreadTM_PBControlAndBlock(&aMacTCPControlBlock->fPB);
		if (err != noErr)
			{
			delete aMacTCPControlBlock;
			break;
			}
		createdControlBlocks.push_back(aMacTCPControlBlock);
		aMacTCPControlBlock->fStreamPtr = aMacTCPControlBlock->fPB.fParamBlock.tcpStream;
		}

	// Now, do a passiveOpen on each newly created block, and move it into the pending list
	for (vector<MacTCPControlBlock*>::iterator i = createdControlBlocks.begin(); i != createdControlBlocks.end(); ++i)
		{
		ZThreadTM_SetupForAsyncCompletionMulti(&(*i)->fPB);
		(*i)->fPB.fParamBlock.ioCRefNum = theMacTCPRefNum;
		(*i)->fPB.fParamBlock.tcpStream = (*i)->fStreamPtr;
		(*i)->fPB.fParamBlock.csParam.open.ulpTimeoutValue = 0;
		(*i)->fPB.fParamBlock.csParam.open.ulpTimeoutAction = 0;
		(*i)->fPB.fParamBlock.csParam.open.validityFlags = 0;
		(*i)->fPB.fParamBlock.csParam.open.commandTimeoutValue = 0;
		(*i)->fPB.fParamBlock.csParam.open.remoteHost = 0;
		(*i)->fPB.fParamBlock.csParam.open.remotePort = 0;
		(*i)->fPB.fParamBlock.csParam.open.localHost = 0;
		(*i)->fPB.fParamBlock.csParam.open.localPort = fLocalPort;
		(*i)->fPB.fParamBlock.csParam.open.tosFlags = 0;
		(*i)->fPB.fParamBlock.csParam.open.precedence = 0;
		(*i)->fPB.fParamBlock.csParam.open.dontFrag = false;
		(*i)->fPB.fParamBlock.csParam.open.timeToLive = 0;
		(*i)->fPB.fParamBlock.csParam.open.security = 0;
		(*i)->fPB.fParamBlock.csParam.open.optionCnt = 0;
		(*i)->fPB.fParamBlock.csParam.open.userDataPtr = nil;
		(*i)->fPB.fParamBlock.csCode = TCPPassiveOpen;
		::PBControlAsync((ParmBlkPtr)&(*i)->fPB.fParamBlock);
		fPendingControlBlocks.push_back(*i);
		}
	}

// ==================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP_MacClassic

ZNetEndpoint_TCP_MacClassic::ZNetEndpoint_TCP_MacClassic(ZNetListener_TCP_MacClassic::MacTCPControlBlock* inControlBlock)
	{
	ZAssertStop(2, inControlBlock);
	fBuffer = inControlBlock->fBuffer;
	fStreamPtr = inControlBlock->fStreamPtr;
	inControlBlock->fBuffer = nil;
	inControlBlock->fStreamPtr = nil;
	delete inControlBlock;
	}

ZNetEndpoint_TCP_MacClassic::ZNetEndpoint_TCP_MacClassic(ip_addr iRemoteHost, ip_port iRemotePort)
:	fBuffer(nil), fStreamPtr(nil)
	{
	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);
	if (err != noErr)
		throw ZNetEx(ZNet_Internet_MacClassic::sTranslateError(err));

	try
		{
		ZAssertStop(2, inInternalBufferSize > 0);
		if (inInternalBufferSize < 4096)
			inInternalBufferSize = 4096;
		fBuffer = new char[inInternalBufferSize];

		Threaded_TCPiopb paramBlock;
		paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
		paramBlock.fParamBlock.csParam.create.rcvBuff = fBuffer;
		paramBlock.fParamBlock.csParam.create.rcvBuffLen = inInternalBufferSize;
		paramBlock.fParamBlock.csParam.create.notifyProc = nil; // sNotificationProcUPP;
		paramBlock.fParamBlock.csParam.create.userDataPtr = nil;
		paramBlock.fParamBlock.csCode = TCPCreate;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			throw ZNetEx(ZNet_Internet_MacClassic::sTranslateError(err));
		fStreamPtr = paramBlock.fParamBlock.tcpStream;

		paramBlock.fParamBlock.tcpStream = fStreamPtr;
		paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
		paramBlock.fParamBlock.csParam.open.ulpTimeoutValue = 0;
		paramBlock.fParamBlock.csParam.open.ulpTimeoutAction = 0;
		paramBlock.fParamBlock.csParam.open.validityFlags = 0;
		paramBlock.fParamBlock.csParam.open.commandTimeoutValue = 0;
		paramBlock.fParamBlock.csParam.open.remoteHost = iRemoteHost;
		paramBlock.fParamBlock.csParam.open.remotePort = iRemotePort;
		paramBlock.fParamBlock.csParam.open.localHost = 0;
		paramBlock.fParamBlock.csParam.open.localPort = 0; // We don't care which local port we're using
		paramBlock.fParamBlock.csParam.open.tosFlags = 0;
		paramBlock.fParamBlock.csParam.open.precedence = 0;
		paramBlock.fParamBlock.csParam.open.dontFrag = false;
		paramBlock.fParamBlock.csParam.open.timeToLive = 0;
		paramBlock.fParamBlock.csParam.open.security = 0;
		paramBlock.fParamBlock.csParam.open.optionCnt = 0;
		paramBlock.fParamBlock.csParam.open.userDataPtr = nil;
		paramBlock.fParamBlock.csCode = TCPActiveOpen;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		throw ZNetEx(ZNet_Internet_MacClassic::sTranslateError(err));
		}
	catch (...)
		{
		delete[] fBuffer;
		if (fStreamPtr)
			{
			Threaded_TCPiopb paramBlock;
			paramBlock.fParamBlock.tcpStream = fStreamPtr;
			paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
			paramBlock.fParamBlock.csCode = TCPRelease;
			// Ignore the error (what else can we do?)
			err = ZThreadTM_PBControlAndBlock(&paramBlock);
			}
		throw;
		}
	}

ZNetEndpoint_TCP_MacClassic::~ZNetEndpoint_TCP_MacClassic()
	{
	if (fStreamPtr)
		{
		short theMacTCPRefNum;
		OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);

		Threaded_TCPiopb paramBlock;
		paramBlock.fParamBlock.tcpStream = fStreamPtr;
		paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
		paramBlock.fParamBlock.csCode = TCPRelease;
		// Ignore the error (what else can we do?)
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		}
	delete[] fBuffer;
	}

ZNet::State ZNetEndpoint_TCP_MacClassic::GetState()
	{
	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);
	if (err != noErr)
		return ZNet::stateUnknown;

	Threaded_TCPiopb paramBlock;
	paramBlock.fParamBlock.tcpStream = fStreamPtr;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csParam.status.userDataPtr = nil;
	paramBlock.fParamBlock.csCode = TCPStatus;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	ZNet::State theState = ZNet::stateUnknown;
	if (err == noErr)
		{
		switch (paramBlock.fParamBlock.csParam.status.connectionState)
			{
			case 0: theState = ZNet::stateClosed; break; // CLOSED
			case 2: theState = ZNet::stateListening; break; // LISTEN
			case 4: theState = ZNet::stateInConnect; break; // SYN_RCVD
			case 6: theState = ZNet::stateOutConnect; break; // SYN_SENT
			case 8: theState = ZNet::stateConnected; break; // ESTABLISHED
			case 10: theState = ZNet::stateOutDisconnect; break; // FIN_WAIT_1
			case 12: theState = ZNet::stateOutDisconnect; break; // FIN_WAIT_2
			case 14: theState = ZNet::stateInDisconnect; break; // CLOSE_WAIT
			case 16: theState = ZNet::stateClosed; break; // CLOSING
			case 18: theState = ZNet::stateClosed; break; // LAST_ACK
			case 20: theState = ZNet::stateClosed; break; // TIME_WAIT
			default: break;
			}
		}
	else if (err == connectionDoesntExist)
		theState = ZNet::stateClosed;
	return theState;
	}


ZNet::Error ZNetEndpoint_TCP_MacClassic::Imp_CountReadable(size_t& outCount)
	{
	outCount = 0;
	if (fStreamPtr == nil)
		return ZNet::errorBadState;

	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);

	Threaded_TCPiopb paramBlock;
	paramBlock.fParamBlock.tcpStream = fStreamPtr;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csParam.status.userDataPtr = nil;
	paramBlock.fParamBlock.csCode = TCPStatus;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	outCount = paramBlock.fParamBlock.csParam.status.amtUnreadData;
	return ZNet_Internet_MacClassic::sTranslateError(err);
	}

ZNet::Error ZNetEndpoint_TCP_MacClassic::Disconnect()
	{
	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);

	Threaded_TCPiopb paramBlock;
	paramBlock.fParamBlock.tcpStream = fStreamPtr;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csParam.close.ulpTimeoutValue = 0;
	paramBlock.fParamBlock.csParam.close.ulpTimeoutAction = 0;
	paramBlock.fParamBlock.csParam.close.validityFlags = 0;
	paramBlock.fParamBlock.csParam.close.userDataPtr = nil;
	paramBlock.fParamBlock.csCode = TCPClose;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	if (err != noErr)
		return ZNet_Internet_MacClassic::sTranslateError(err);

	// Suck up (and obviously discard) any pending incoming data
	while (true)
		{
		paramBlock.ZeroContents();
		paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
		paramBlock.fParamBlock.tcpStream = fStreamPtr;
		paramBlock.fParamBlock.csParam.receive.commandTimeoutValue = 0;
		paramBlock.fParamBlock.csParam.receive.rcvBuff = ZooLib::sGarbageBuffer;
		paramBlock.fParamBlock.csParam.receive.rcvBuffLen = sizeof(ZooLib::sGarbageBuffer);
		paramBlock.fParamBlock.csParam.receive.userDataPtr = nil;
		paramBlock.fParamBlock.csCode = TCPRcv;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			break;
		}

	// Now busy wait for the state to change to closed
	paramBlock.ZeroContents();
	paramBlock.fParamBlock.tcpStream = fStreamPtr;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csParam.status.userDataPtr = nil;
	paramBlock.fParamBlock.csCode = TCPStatus;

	while (true)
		{
		err = ZThreadTM_PBControlAndBlock(&paramBlock);

		if (err != noErr)
			break;

		if (paramBlock.fParamBlock.csParam.status.connectionState == 0)
			break;
		if (paramBlock.fParamBlock.csParam.status.connectionState == 20)
			break;
		
		// AG 97-12-28. Status will probably return immediately without yielding,
		// but if the connection is not going down for some reason, then we'll be
		// stuck in this thread permanently, and the app will freeze up. So do a
		// yield just to let other threads get some work done.
		ZThreadTM_YieldIfTimeSliceExpired();
		}

	return ZNet_Internet_MacClassic::sTranslateError(err);
	}

ZNet::Error ZNetEndpoint_TCP_MacClassic::Abort()
	{
	// AG 98-01-08. Abort the connection, should send an RST to the other end if it's still there
	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);

	Threaded_TCPiopb paramBlock;
	paramBlock.fParamBlock.tcpStream = fStreamPtr;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csParam.close.ulpTimeoutValue = 0;
	paramBlock.fParamBlock.csParam.close.ulpTimeoutAction = 0;
	paramBlock.fParamBlock.csParam.close.validityFlags = 0;
	paramBlock.fParamBlock.csParam.close.userDataPtr = nil;
	paramBlock.fParamBlock.csCode = TCPAbort;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	if (err != noErr)
		return ZNet_Internet_MacClassic::sTranslateError(err);
	return ZNet::errorNone;
	}

ZNet::Error ZNetEndpoint_TCP_MacClassic::Receive(void* inDest, size_t inCount, size_t* outCountReceived)
	{
	if (outCountReceived)
		*outCountReceived = 0;
	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);

	char* localDest = reinterpret_cast<char*>(inDest);
	size_t countRemaining = inCount;
	while (countRemaining)
		{
		Threaded_TCPiopb paramBlock;
		paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
		paramBlock.fParamBlock.tcpStream = fStreamPtr;
		paramBlock.fParamBlock.csParam.receive.commandTimeoutValue = 0;
		paramBlock.fParamBlock.csParam.receive.rcvBuff = localDest;
		paramBlock.fParamBlock.csParam.receive.rcvBuffLen = min(size_t(32767), countRemaining);
		paramBlock.fParamBlock.csParam.receive.userDataPtr = nil;
		paramBlock.fParamBlock.csCode = TCPRcv;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			return ZNet_Internet_MacClassic::sTranslateError(err);
		if (outCountReceived)
			*outCountReceived += paramBlock.fParamBlock.csParam.receive.rcvBuffLen;
		countRemaining -= paramBlock.fParamBlock.csParam.receive.rcvBuffLen;
		localDest += paramBlock.fParamBlock.csParam.receive.rcvBuffLen;
		}
	return ZNet::errorNone;
	}

ZNet::Error ZNetEndpoint_TCP_MacClassic::Send(const void* inSource, size_t inCount, size_t* outCountSent, bool inPush)
	{
	if (outCountSent)
		*outCountSent = 0;

	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);

	const char* localSource = reinterpret_cast<const char*>(inSource);
	size_t countRemaining = inCount;
	while (countRemaining > 0)
		{
		wdsEntry theWDS[2];
		Threaded_TCPiopb paramBlock;
		paramBlock.fParamBlock.tcpStream = fStreamPtr;
		paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
		paramBlock.fParamBlock.csParam.send.ulpTimeoutValue = 0;
		paramBlock.fParamBlock.csParam.send.ulpTimeoutAction = 0;
		paramBlock.fParamBlock.csParam.send.validityFlags = 0;
		paramBlock.fParamBlock.csParam.send.pushFlag = inPush ? 1 : 0;
		paramBlock.fParamBlock.csParam.send.urgentFlag = false;

		size_t countToSend = min(size_t(32767), countRemaining);
		theWDS[0].ptr = const_cast<char*>(localSource);
		theWDS[0].length = countToSend;
		theWDS[1].ptr = 0;
		theWDS[1].length = 0;
		paramBlock.fParamBlock.csParam.send.wdsPtr = (Ptr)&theWDS;
		paramBlock.fParamBlock.csParam.send.userDataPtr = nil;
		paramBlock.fParamBlock.csCode = TCPSend;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			return ZNet_Internet_MacClassic::sTranslateError(err);
		if (outCountSent)
			*outCountSent += countToSend;
		countRemaining -= countToSend;
		localSource += countToSend;
		}
	return ZNet::errorNone;
	}

ZNetAddress* ZNetEndpoint_TCP_MacClassic::GetRemoteAddress()
	{
	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);
	if (err != noErr)
		return nil;

	Threaded_TCPiopb paramBlock;
	paramBlock.fParamBlock.tcpStream = fStreamPtr;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csParam.status.userDataPtr = nil;
	paramBlock.fParamBlock.csCode = TCPStatus;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	if (err != noErr || paramBlock.fParamBlock.csParam.status.connectionState != 8)
		return nil;
	return new ZNetAddress_Internet(paramBlock.fParamBlock.csParam.status.remoteHost, paramBlock.fParamBlock.csParam.status.remotePort);
	}
// ==================================================
#pragma mark -
#pragma mark * ZNetEndpointDG_UDP_MacClassic

ZNetEndpointDG_UDP_MacClassic::ZNetEndpointDG_UDP_MacClassic(size_t inInternalBufferSize)
:	fStreamPtr(nil), fBuffer(nil)
	{
	ZAssertStop(2, inInternalBufferSize > 0);
	if (inInternalBufferSize < 4096)
		inInternalBufferSize = 4096;
	fBuffer = new char[inInternalBufferSize];

	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);
	if (err != noErr)
		{
		delete[] fBuffer;
		throw ZNetEx(ZNet_Internet_MacClassic::sTranslateError(err));
		}

	Threaded_UDPiopb paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csCode = UDPCreate;
	paramBlock.fParamBlock.csParam.create.rcvBuff = fBuffer;
	paramBlock.fParamBlock.csParam.create.rcvBuffLen = inInternalBufferSize;
	paramBlock.fParamBlock.csParam.create.localPort = 0;
	paramBlock.fParamBlock.csParam.create.notifyProc = nil;
	paramBlock.fParamBlock.csParam.create.userDataPtr = nil;

	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	if (err != noErr)
		{
		delete[] fBuffer;
		throw ZNetEx(ZNet_Internet_MacClassic::sTranslateError(err));
		}
	fStreamPtr = paramBlock.fParamBlock.udpStream;
	}

ZNetEndpointDG_UDP_MacClassic::ZNetEndpointDG_UDP_MacClassic(ip_port inLocalPort, size_t inInternalBufferSize)
:	fStreamPtr(nil), fBuffer(nil)
	{
	ZAssertStop(2, inInternalBufferSize > 0);
	if (inInternalBufferSize < 4096)
		inInternalBufferSize = 4096;
	fBuffer = new char[inInternalBufferSize];

	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);
	if (err != noErr)
		{
		delete[] fBuffer;
		throw ZNetEx(ZNet_Internet_MacClassic::sTranslateError(err));
		}

	Threaded_UDPiopb paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csCode = UDPCreate;
	paramBlock.fParamBlock.ioCompletion = nil;
	paramBlock.fParamBlock.csParam.create.rcvBuff = fBuffer;
	paramBlock.fParamBlock.csParam.create.rcvBuffLen = inInternalBufferSize;
	paramBlock.fParamBlock.csParam.create.localPort = inLocalPort;
	paramBlock.fParamBlock.csParam.create.notifyProc = nil;
	paramBlock.fParamBlock.csParam.create.userDataPtr = nil;

	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	if (err != noErr)
		{
		delete[] fBuffer;
		throw ZNetEx(ZNet_Internet_MacClassic::sTranslateError(err));
		}
	fStreamPtr = paramBlock.fParamBlock.udpStream;
	}

ZNetEndpointDG_UDP_MacClassic::~ZNetEndpointDG_UDP_MacClassic()
	{
	if (fStreamPtr != nil)
		{
		short theMacTCPRefNum;
		OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);

		Threaded_UDPiopb paramBlock;
		paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
		paramBlock.fParamBlock.ioCompletion = nil;
		paramBlock.fParamBlock.csCode = UDPRelease;
		paramBlock.fParamBlock.udpStream = fStreamPtr;
	// Ignore the error (what else can we do?)
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		}
	delete[] fBuffer;
	}

ZNet::Error ZNetEndpointDG_UDP_MacClassic::Receive(void* inBuffer, size_t inBufferSize, size_t& outCountReceived, ZNetAddress*& outSourceAddress)
	{
	outCountReceived = 0;
	outSourceAddress = nil;

	ZAssertStop(2, inBuffer != nil);
	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);

	Threaded_UDPiopb paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csCode = UDPRead;
	paramBlock.fParamBlock.ioCompletion = nil;
	paramBlock.fParamBlock.udpStream = fStreamPtr;
	paramBlock.fParamBlock.csParam.receive.timeOut = 0;
	paramBlock.fParamBlock.csParam.receive.userDataPtr = nil;
	paramBlock.fParamBlock.csParam.receive.secondTimeStamp = 0;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	if (err != noErr)
		return ZNet_Internet_MacClassic::sTranslateError(err);
	outSourceAddress = new ZNetAddress_Internet(paramBlock.fParamBlock.csParam.receive.remoteHost,
											paramBlock.fParamBlock.csParam.receive.remotePort);
	ZNet::Error returnedError = ZNet::errorNone;

	size_t amountToMove = paramBlock.fParamBlock.csParam.receive.rcvBuffLen;
	if (amountToMove > inBufferSize)
		{
		returnedError = ZNet::errorBufferTooSmall;
		amountToMove = inBufferSize;
		}
	ZBlockCopy(paramBlock.fParamBlock.csParam.receive.rcvBuff, inBuffer, amountToMove);

	// Now return the buffer to the UDP driver (iff some data was actually delivered to us)
	if (paramBlock.fParamBlock.csParam.receive.rcvBuffLen > 0)
		{
		paramBlock.fParamBlock.csCode = UDPBfrReturn;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		}
	return returnedError;
	}

ZNet::Error ZNetEndpointDG_UDP_MacClassic::Send(const void* inBuffer, size_t inCount, ZNetAddress* inDestAddress)
	{
	short theMacTCPRefNum;
	OSErr err = ZNet_Internet_MacClassic::sGetTCPRefNum(theMacTCPRefNum);

	ZAssertStop(2, inDestAddress != nil);
	ZNetAddress_Internet* internetAddress = dynamic_cast<ZNetAddress_Internet*>(inDestAddress);
	ZAssertStop(2, internetAddress);
	wdsEntry theWDS[2];
	theWDS[0].ptr = const_cast<char*>(inBuffer);
	theWDS[0].length = inCount;
	theWDS[1].ptr = 0;
	theWDS[1].length = 0;
	Threaded_UDPiopb paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theMacTCPRefNum;
	paramBlock.fParamBlock.csCode = UDPWrite;
	paramBlock.fParamBlock.ioCompletion = nil;
	paramBlock.fParamBlock.udpStream = fStreamPtr;
	paramBlock.fParamBlock.csParam.send.reserved = 0;
	paramBlock.fParamBlock.csParam.send.remoteHost = internetAddress->GetAddress();
	paramBlock.fParamBlock.csParam.send.remotePort = internetAddress->GetPort();
	paramBlock.fParamBlock.csParam.send.checkSum = 1;
	paramBlock.fParamBlock.csParam.send.userDataPtr = nil;
	paramBlock.fParamBlock.csParam.send.wdsPtr = (Ptr)&theWDS;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	if (err != noErr)
		return ZNet_Internet_MacClassic::sTranslateError(err);
	return ZNet::errorNone;
	}

#endif // 0
// =================================================================================================
