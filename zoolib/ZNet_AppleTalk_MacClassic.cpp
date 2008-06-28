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

#include "zoolib/ZNet_AppleTalk_MacClassic.h"

// =================================================================================================
#if ZCONFIG(API_Net, MacClassic)

#include "zoolib/ZMemory.h"
#include "zoolib/ZString.h"

#include <Devices.h>

// ==================================================
#pragma mark -
#pragma mark * ThreadedDSPParamBlock

struct ThreadedDSPParamBlock : public ZThreadTM_PBHeader
	{
	DSPParamBlock fParamBlock;

	ThreadedDSPParamBlock();
	void ZeroContents();
	};

ThreadedDSPParamBlock::ThreadedDSPParamBlock()
	{
	ZBlockSet(&fParamBlock, sizeof(fParamBlock), 0);
	}

void ThreadedDSPParamBlock::ZeroContents()
	{
	ZBlockSet(&fParamBlock, sizeof(fParamBlock), 0);
	}

// ==================================================
#pragma mark -
#pragma mark * ThreadedMPPParamBlock

ThreadedMPPParamBlock::ThreadedMPPParamBlock()
	{
	ZBlockSet(&fParamBlock, sizeof(fParamBlock), 0);
	}

void ThreadedMPPParamBlock::ZeroContents()
	{
	ZBlockSet(&fParamBlock, sizeof(fParamBlock), 0);
	}

ZNetNameLookup_AppleTalk_MacClassic::ZNetNameLookup_AppleTalk_MacClassic(const string& inName, const string& inType, const string& inZone, size_t inMaxAddresses)
	{
	fStarted = false;
	fNextIndex = 0;
	fCurrentAddress = nil;
	fCurrentName = nil;
	fInfoBuffer = nil;

	Str32 pascalName, pascalType, pascalZone;
	ZString::sToPString(inName, pascalName, 32);
	ZString::sToPString(inType, pascalType, 32);
	ZString::sToPString(inZone, pascalZone, 32);
	::NBPSetEntity((char*)&fEntityName, pascalName, pascalType, pascalZone);

	// Zero max addresses means get as many as possible.
	fMaxAddresses = inMaxAddresses;
	if (fMaxAddresses == 0)
		fMaxAddresses = 0x10000/sizeof(NTElement);
	// We can't get more than 64K's worth though.
	fMaxAddresses = min(fMaxAddresses, 0x10000/sizeof(NTElement));
	}

ZNetNameLookup_AppleTalk_MacClassic::~ZNetNameLookup_AppleTalk_MacClassic()
	{
	if (fStarted)
		{
		while (fMPPParamBlock.fParamBlock.NBP.ioResult == 1)
			ZThread::sSleep(100);
		delete[] fInfoBuffer;
		delete fCurrentAddress;
		delete fCurrentName;
		}
	}

void ZNetNameLookup_AppleTalk_MacClassic::Start()
	{
	if (fStarted)
		return;

	fStarted = true;

	// 1 interval is 8 ticks
	fMPPParamBlock.fParamBlock.NBP.interval = 7;
	// number of times to try
	fMPPParamBlock.fParamBlock.NBP.count = 5;

	fMPPParamBlock.fParamBlock.NBP.nbpPtrs.entityPtr = (char*)&fEntityName;

	// Allocate some space for the return info
	fInfoBuffer = new char[fMaxAddresses * sizeof(NTElement)];
	fMPPParamBlock.fParamBlock.NBP.parm.Lookup.retBuffPtr = fInfoBuffer;
	fMPPParamBlock.fParamBlock.NBP.parm.Lookup.retBuffSize = fMaxAddresses * sizeof(NTElement);
	fMPPParamBlock.fParamBlock.NBP.parm.Lookup.maxToGet = fMaxAddresses;
	fMPPParamBlock.fParamBlock.NBP.parm.Lookup.numGotten = 0;

	// Fire off the lookup
	::PLookupName(&(fMPPParamBlock.fParamBlock), true);
	}

bool ZNetNameLookup_AppleTalk_MacClassic::Finished()
	{
	if (!fStarted)
		return true;

	// We're done when the async lookup has completed and numGotten matches fNextIndex.
	return fMPPParamBlock.fParamBlock.NBP.ioResult != 1 && fMPPParamBlock.fParamBlock.NBP.parm.Lookup.numGotten == fNextIndex;
	}

void ZNetNameLookup_AppleTalk_MacClassic::Next()
	{
	ZAssertStop(2, fStarted);

	this->Internal_GrabNextResult();
	}

const ZNetAddress& ZNetNameLookup_AppleTalk_MacClassic::CurrentAddress()
	{
	if (!fCurrentAddress)
		this->Internal_GrabNextResult();

	ZAssertStop(2, fCurrentAddress);
	return *fCurrentAddress;
	}

const ZNetName& ZNetNameLookup_AppleTalk_MacClassic::CurrentName()
	{
	if (!fCurrentName)
		this->Internal_GrabNextResult();

	ZAssertStop(2, fCurrentName);
	return *fCurrentName;
	}

void ZNetNameLookup_AppleTalk_MacClassic::Internal_GrabNextResult()
	{
	// Wait till either the async lookup completes or numGotten no longer matches fNextIndex.
	while (fMPPParamBlock.fParamBlock.NBP.ioResult == 1 && fMPPParamBlock.fParamBlock.NBP.parm.Lookup.numGotten == fNextIndex)
		ZThread::sSleep(100);

	if (fMPPParamBlock.fParamBlock.NBP.parm.Lookup.numGotten != fNextIndex)
		{
		delete fCurrentAddress;
		delete fCurrentName;

		EntityName foundEntityName;
		AddrBlock foundAddrBlock;
		::NBPExtract(fInfoBuffer, fMPPParamBlock.fParamBlock.NBP.parm.Lookup.numGotten, fNextIndex, &foundEntityName, &foundAddrBlock);

		string foundName(reinterpret_cast<char*>(&foundEntityName.objStr[1]), foundEntityName.objStr[0]);
		string foundType(reinterpret_cast<char*>(&foundEntityName.typeStr[1]), foundEntityName.typeStr[0]);
		string foundZone(reinterpret_cast<char*>(&foundEntityName.zoneStr[1]), foundEntityName.zoneStr[0]);

		fCurrentName = new ZNetName_AppleTalk(foundName, foundType, foundZone);

		fCurrentAddress = new ZNetAddress_AppleTalk(foundAddrBlock.aNet, foundAddrBlock.aNode, foundAddrBlock.aSocket);
		++fNextIndex;
		}
	}

// ==================================================
#pragma mark -
#pragma mark * ZNet_AppleTalk_MacClassic

short ZNet_AppleTalk_MacClassic::sADSPRefNum = 0;
static const AddrBlock kNullAddrBlock = { 0, 0, 0 };

OSErr ZNet_AppleTalk_MacClassic::sGetADSPRefNum(short& theRefNum)
	{
	if (sADSPRefNum == 0)
		{
		ZThread::NormalTime normal;
		// Get the MPP driver open
		if (!::IsMPPOpen())
			{
			OSErr err = ::MPPOpen();
			if (err != noErr)
				return err;
			}
		OSErr err = ::OpenDriver("\p.DSP", &sADSPRefNum);
		if (err != noErr)
			{
			sADSPRefNum = 0;
			return err;
			}
		}
	theRefNum = sADSPRefNum;
	return noErr;
	}

ZNet::Error ZNet_AppleTalk_MacClassic::sTranslateError(OSErr nativeError)
	{
	ZNet::Error theErr = ZNet::errorGeneric;
	switch (nativeError)
		{
		case noErr: theErr = ZNet::errorNone; break;
		default: break;
		}
	return theErr;
	}

// ==================================================
#pragma mark -
#pragma mark * ZNetNameRegistered_AppleTalk_MacClassic

ZNetNameRegistered_AppleTalk_MacClassic::ZNetNameRegistered_AppleTalk_MacClassic(const string& inName, const string& inType, const string& inZone, uint8 inSocket)
	{
	// Build our name table entry
	Str32 pascalName, pascalType, pascalZone;
	ZString::sToPString(inName, pascalName, 32);
	ZString::sToPString(inType, pascalType, 32);
	ZString::sToPString(inZone, pascalZone, 32);

	::NBPSetNTE((char*)&fNamesTableEntry, pascalName, pascalType, pascalZone, inSocket);

	fRegisteredOK = false;
	ThreadedMPPParamBlock paramBlock;
	paramBlock.fParamBlock.NBP.interval = 7;
	paramBlock.fParamBlock.NBP.count = 3;
	paramBlock.fParamBlock.NBP.nbpPtrs.entityPtr = (Ptr)&fNamesTableEntry;
	paramBlock.fParamBlock.NBP.parm.verifyFlag = 1;

	ZThreadTM_SetupForAsyncCompletion(&paramBlock);
	OSErr err = ::PRegisterName(&paramBlock.fParamBlock, true);
	ZThreadTM_WaitForAsyncCompletion(&paramBlock, err);
	if (err == noErr)
		fRegisteredOK = true;
	}

ZNetNameRegistered_AppleTalk_MacClassic::~ZNetNameRegistered_AppleTalk_MacClassic()
	{
	if (!fRegisteredOK)
		return;

	// Unregister this name
	ThreadedMPPParamBlock paramBlock;
	paramBlock.fParamBlock.NBP.interval = 7;
	paramBlock.fParamBlock.NBP.count = 3;
	paramBlock.fParamBlock.NBP.nbpPtrs.entityPtr = (Ptr)&fNamesTableEntry.nt.entityData;
	ZThreadTM_SetupForAsyncCompletion(&paramBlock);
	OSErr err = ::PRemoveName(&(paramBlock.fParamBlock), true);
	err = ZThreadTM_WaitForAsyncCompletion(&paramBlock, err);
	// Ignore the error -- nothing we can do about it anyway
	}

string ZNetNameRegistered_AppleTalk_MacClassic::GetName() const
	{
	EntityName* entityName = (EntityName*)(&fNamesTableEntry.nt.entityData);
	return string((char*)(&(entityName->objStr[1])), size_t(entityName->objStr[0]));
	}

string ZNetNameRegistered_AppleTalk_MacClassic::GetType() const
	{
	EntityName* entityName = (EntityName*)(&fNamesTableEntry.nt.entityData);
	return string((char*)(&(entityName->typeStr[1])), size_t(entityName->typeStr[0]));
	}

string ZNetNameRegistered_AppleTalk_MacClassic::GetZone() const
	{
	EntityName* entityName = (EntityName*)(&fNamesTableEntry.nt.entityData);
	return string((char*)(&(entityName->zoneStr[1])), size_t(entityName->zoneStr[0]));
	}

uint8 ZNetNameRegistered_AppleTalk_MacClassic::GetSocket() const
	{ return fNamesTableEntry.nt.nteAddress.aSocket; }

bool ZNetNameRegistered_AppleTalk_MacClassic::IsRegistrationGood() const
	{ return fRegisteredOK; }

// ==================================================
#pragma mark -
#pragma mark * ZNetListener_ADSP_MacClassic

ZNetListener_ADSP_MacClassic::ZNetListener_ADSP_MacClassic(size_t inSendBufferSize, size_t inReceiveBufferSize)
:	fCancelling(false),
	fSendBufferSize(inSendBufferSize),
	fReceiveBufferSize(inReceiveBufferSize),
	fLocalSocket(0)
	{
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);
	if (err != noErr)
		throw ZNet::Ex(ZNet_AppleTalk_MacClassic::sTranslateError(err));

	// We use an extended paramBlock, which has some crud on the
	// front for LThread to use when blocking us
	ThreadedDSPParamBlock paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
	paramBlock.fParamBlock.u.initParams.ccbPtr = &fCCB;
	paramBlock.fParamBlock.u.initParams.userRoutine = nil;
	paramBlock.fParamBlock.u.initParams.localSocket = 0;
	paramBlock.fParamBlock.csCode = dspCLInit;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	if (err != noErr)
		throw ZNet::Ex(ZNet_AppleTalk_MacClassic::sTranslateError(err));
	fLocalSocket = paramBlock.fParamBlock.u.initParams.localSocket;
	}

ZNetListener_ADSP_MacClassic::~ZNetListener_ADSP_MacClassic()
	{
	fCancelling = true;
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);
	ThreadedDSPParamBlock paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
	paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
	paramBlock.fParamBlock.u.closeParams.abort = 1;
	paramBlock.fParamBlock.csCode = dspCLRemove;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);

	// Wait till the listen has exited
	ZMutexLocker listenLocker(fListenMutex);
	}

ZNetEndpoint* ZNetListener_ADSP_MacClassic::Listen()
	{
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);

	ZMutexLocker listenLocker(fListenMutex);
	ZNetEndpoint_ADSP_MacClassic* theEndpoint = nil;
	while (!fCancelling)
		{
		ThreadedDSPParamBlock paramBlock;
		paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
		paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
		paramBlock.fParamBlock.u.openParams.filterAddress = kNullAddrBlock;
		paramBlock.fParamBlock.csCode = dspCLListen;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (fCancelling)
			break;
		if (err != noErr)
			continue; //??? Could get into a tight loop -- maybe need to yield

		ListenInfo theInfo;
		theInfo.fRemoteCID = paramBlock.fParamBlock.u.openParams.remoteCID;
		theInfo.fRemoteAddress = paramBlock.fParamBlock.u.openParams.remoteAddress;
		theInfo.fSendSeq = paramBlock.fParamBlock.u.openParams.sendSeq;
		theInfo.fSendWindow = paramBlock.fParamBlock.u.openParams.sendWindow;
		theInfo.fAttnSendSeq = paramBlock.fParamBlock.u.openParams.attnSendSeq;

		try
			{
			theEndpoint = new ZNetEndpoint_ADSP_MacClassic(fSendBufferSize, fReceiveBufferSize, &theInfo);
			}
		catch (...)
			{}
		break;
		}
	return theEndpoint;
	}

uint8 ZNetListener_ADSP_MacClassic::GetSocket()
	{ return fLocalSocket; }

// ==================================================
#pragma mark -
#pragma mark * ZNetEndpoint_ADSP_MacClassic

ZNetEndpoint_ADSP_MacClassic::ZNetEndpoint_ADSP_MacClassic(size_t inSendBufferSize, size_t inReceiveBufferSize, ZNetListener_ADSP_MacClassic::ListenInfo* inInfo)
:	fAttentionBuffer(nil),
	fSendBuffer(nil),
	fReceiveBuffer(nil),
	fSendBufferSize(inSendBufferSize),
	fReceiveBufferSize(inReceiveBufferSize)
	{
	// This constructor is used when creating a socket to handle an incoming connection
	ZAssertStop(2, fSendBufferSize >= minDSPQueueSize);
	ZAssertStop(2, fReceiveBufferSize >= minDSPQueueSize);

	bool inited = false;

	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);
	if (err != noErr)
		throw ZNet::Ex(ZNet_AppleTalk_MacClassic::sTranslateError(err));

	// We're going to connect to the other side, so we need buffer space
	// Allocate the buffers
	try
		{
		fSendBuffer = new char[fSendBufferSize];
		fReceiveBuffer = new char[fReceiveBufferSize];
		fAttentionBuffer = new char[attnBufSize];

		// Initialize the socket
		ThreadedDSPParamBlock paramBlock;
		paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
		paramBlock.fParamBlock.u.initParams.ccbPtr = &fCCB;
		paramBlock.fParamBlock.u.initParams.userRoutine = nil;
		paramBlock.fParamBlock.u.initParams.sendQSize = fSendBufferSize;
		paramBlock.fParamBlock.u.initParams.sendQueue = (unsigned char*)fSendBuffer;
		paramBlock.fParamBlock.u.initParams.recvQSize = fReceiveBufferSize;
		paramBlock.fParamBlock.u.initParams.recvQueue = (unsigned char*)fReceiveBuffer;
		paramBlock.fParamBlock.u.initParams.attnPtr = (unsigned char*)fAttentionBuffer;
		paramBlock.fParamBlock.u.initParams.localSocket = 0;
		paramBlock.fParamBlock.csCode = dspInit;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			throw ZNet::Ex(ZNet_AppleTalk_MacClassic::sTranslateError(err));
		// Grab our local socket
		fLocalSocket = paramBlock.fParamBlock.u.initParams.localSocket;

		inited = true;

		// Setup our options
		paramBlock.ZeroContents();
		paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
		paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
		paramBlock.fParamBlock.u.optionParams.sendBlocking = max(size_t(572), fSendBufferSize);
		paramBlock.fParamBlock.u.optionParams.sendTimer = 0;
		paramBlock.fParamBlock.u.optionParams.rtmtTimer = 0;
		paramBlock.fParamBlock.u.optionParams.badSeqMax = 0;
		paramBlock.fParamBlock.u.optionParams.useCheckSum = 0;
		paramBlock.fParamBlock.csCode = dspOptions;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			throw ZNet::Ex(ZNet_AppleTalk_MacClassic::sTranslateError(err));

		// Accept the incoming connection
		paramBlock.ZeroContents();
		paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
		paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
		paramBlock.fParamBlock.u.openParams.remoteAddress = inInfo->fRemoteAddress;
		paramBlock.fParamBlock.u.openParams.filterAddress = kNullAddrBlock;
		paramBlock.fParamBlock.u.openParams.remoteCID = inInfo->fRemoteCID;
		paramBlock.fParamBlock.u.openParams.sendSeq = inInfo->fSendSeq;
		paramBlock.fParamBlock.u.openParams.sendWindow = inInfo->fSendWindow;
		paramBlock.fParamBlock.u.openParams.attnSendSeq = inInfo->fAttnSendSeq;
		paramBlock.fParamBlock.u.openParams.ocMode = ocAccept;
		paramBlock.fParamBlock.u.openParams.ocInterval = 3; // Every half a second
		paramBlock.fParamBlock.u.openParams.ocMaximum = 20; // For ten seconds
		paramBlock.fParamBlock.csCode = dspOpen;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			throw ZNet::Ex(ZNet_AppleTalk_MacClassic::sTranslateError(err));
		}
	catch (...)
		{
		if (inited)
			{
			short theADSPRefNum;
			OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);
	
			ThreadedDSPParamBlock paramBlock;
			paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
			paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
			paramBlock.fParamBlock.u.closeParams.abort = 1;
			paramBlock.fParamBlock.csCode = dspRemove;
			err = ZThreadTM_PBControlAndBlock(&paramBlock);
			}
		delete[] fSendBuffer;
		delete[] fReceiveBuffer;
		delete[] fAttentionBuffer;
		throw;
		}
	}

ZNetEndpoint_ADSP_MacClassic::ZNetEndpoint_ADSP_MacClassic(size_t inSendBufferSize, size_t inReceiveBufferSize)
:	fAttentionBuffer(nil),
	fSendBuffer(nil),
	fReceiveBuffer(nil),
	fSendBufferSize(inSendBufferSize),
	fReceiveBufferSize(inReceiveBufferSize)
	{
	// This constructor is used when creating a socket that will connect to
	// a listening socket
	ZAssertStop(2, fSendBufferSize >= minDSPQueueSize);
	ZAssertStop(2, fReceiveBufferSize >= minDSPQueueSize);

	bool inited = false;

	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);
	if (err != noErr)
		throw ZNet::Ex(ZNet_AppleTalk_MacClassic::sTranslateError(err));

	// We're going to connect to the other side, so we need buffer space
	// Allocate the buffers
	try
		{
		fSendBuffer = new char[fSendBufferSize];
		fReceiveBuffer = new char[fReceiveBufferSize];
		fAttentionBuffer = new char[attnBufSize];

		ThreadedDSPParamBlock paramBlock;
		paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
		paramBlock.fParamBlock.u.initParams.ccbPtr = &fCCB;
		paramBlock.fParamBlock.u.initParams.userRoutine = nil;
		paramBlock.fParamBlock.u.initParams.sendQSize = fSendBufferSize;
		paramBlock.fParamBlock.u.initParams.sendQueue = (unsigned char*)fSendBuffer;
		paramBlock.fParamBlock.u.initParams.recvQSize = fReceiveBufferSize;
		paramBlock.fParamBlock.u.initParams.recvQueue = (unsigned char*)fReceiveBuffer;
		paramBlock.fParamBlock.u.initParams.attnPtr = (unsigned char*)fAttentionBuffer;
		paramBlock.fParamBlock.u.initParams.localSocket = 0;
		paramBlock.fParamBlock.csCode = dspInit;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			throw ZNet::Ex(ZNet_AppleTalk_MacClassic::sTranslateError(err));
		// Grab our local socket
		fLocalSocket = paramBlock.fParamBlock.u.initParams.localSocket;

		inited = true;

		// Setup our options
		paramBlock.ZeroContents();
		paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
		paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
		paramBlock.fParamBlock.u.optionParams.sendBlocking = max(size_t(572), fSendBufferSize);
		paramBlock.fParamBlock.u.optionParams.sendTimer = 0;
		paramBlock.fParamBlock.u.optionParams.rtmtTimer = 0;
		paramBlock.fParamBlock.u.optionParams.badSeqMax = 0;
		paramBlock.fParamBlock.u.optionParams.useCheckSum = 0;
		paramBlock.fParamBlock.csCode = dspOptions;
		err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			throw ZNet::Ex(ZNet_AppleTalk_MacClassic::sTranslateError(err));
		}
	catch (...)
		{
		if (inited)
			{
			ThreadedDSPParamBlock paramBlock;
			paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
			paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
			paramBlock.fParamBlock.u.closeParams.abort = 1;
			paramBlock.fParamBlock.csCode = dspRemove;
			err = ZThreadTM_PBControlAndBlock(&paramBlock);
			}
		delete[] fSendBuffer;
		delete[] fReceiveBuffer;
		delete[] fAttentionBuffer;
		throw;
		}
	}

ZNetEndpoint_ADSP_MacClassic::~ZNetEndpoint_ADSP_MacClassic()
	{
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);

	ThreadedDSPParamBlock paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
	paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
	paramBlock.fParamBlock.u.closeParams.abort = 1;
	paramBlock.fParamBlock.csCode = dspRemove;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);

	delete[] fSendBuffer;
	delete[] fReceiveBuffer;
	delete[] fAttentionBuffer;
	}

ZNet::State ZNetEndpoint_ADSP_MacClassic::GetState()
	{
	// As we own the CCB we can pull the connection state straight out of there
	ZNet::State theState = ZNet::stateUnknown;
	switch (fCCB.state)
		{
		case 1: theState = ZNet::stateListening; break; // As a listener
		case 2: theState = ZNet::stateListening; break; // As a passive socket
		case 3: theState = ZNet::stateOutConnect; break; // We've sent a connection request
		case 4: theState = ZNet::stateConnected; break;
		case 5: theState = ZNet::stateInDisconnect; break;// Close has been requested (by us or the other side)
		case 6: theState = ZNet::stateClosed; break;
		default: break;
		}
	return theState;
	}


ZNet::Error ZNetEndpoint_ADSP_MacClassic::Imp_CountReadable(size_t& outCount)
	{
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);

	// Setup our options
	ThreadedDSPParamBlock paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
	paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
	paramBlock.fParamBlock.csCode = dspStatus;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	outCount = paramBlock.fParamBlock.u.statusParams.recvQPending;
	return ZNet_AppleTalk_MacClassic::sTranslateError(err);
	}

ZNet::Error ZNetEndpoint_ADSP_MacClassic::Connect(int16 inNet, uint8 inNode, int8 inSocket)
	{
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);

	ThreadedDSPParamBlock paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
	paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
	paramBlock.fParamBlock.u.openParams.remoteAddress.aNet = inNet;
	paramBlock.fParamBlock.u.openParams.remoteAddress.aNode = inNode;
	paramBlock.fParamBlock.u.openParams.remoteAddress.aSocket = inSocket;
	paramBlock.fParamBlock.u.openParams.filterAddress = kNullAddrBlock;
	paramBlock.fParamBlock.u.openParams.ocMode = ocRequest;
	paramBlock.fParamBlock.u.openParams.ocInterval = 3; // Every half a second
	paramBlock.fParamBlock.u.openParams.ocMaximum = 20; // For ten seconds
	paramBlock.fParamBlock.csCode = dspOpen;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	return ZNet_AppleTalk_MacClassic::sTranslateError(err);
	}

ZNet::Error ZNetEndpoint_ADSP_MacClassic::Disconnect()
	{
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);

	ThreadedDSPParamBlock paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
	paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
	paramBlock.fParamBlock.u.closeParams.abort = 0;
	paramBlock.fParamBlock.csCode = dspClose;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	return ZNet_AppleTalk_MacClassic::sTranslateError(err);
	}


ZNet::Error ZNetEndpoint_ADSP_MacClassic::Abort()
	{
	// AG 98-01-08. Not sure if there's a clean way to abort a connection
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);

	ThreadedDSPParamBlock paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
	paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
	paramBlock.fParamBlock.u.closeParams.abort = 0;
	paramBlock.fParamBlock.csCode = dspClose;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	return ZNet_AppleTalk_MacClassic::sTranslateError(err);
	}


ZNet::Error ZNetEndpoint_ADSP_MacClassic::Receive(void* inDest, size_t inCount, size_t* outCountReceived)
	{
	if (outCountReceived)
		*outCountReceived = 0;
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);

	unsigned char* localDest = reinterpret_cast<unsigned char*>(inDest);
	size_t countRemaining = inCount;
	while (countRemaining)
		{
		ThreadedDSPParamBlock paramBlock;
		paramBlock.fParamBlock.ioCompletion = nil;
		paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
		paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
		paramBlock.fParamBlock.u.ioParams.reqCount = countRemaining;
		paramBlock.fParamBlock.u.ioParams.dataPtr = localDest;
		paramBlock.fParamBlock.csCode = dspRead;
		OSErr err = ZThreadTM_PBControlAndBlock(&paramBlock);
		if (err != noErr)
			return ZNet_AppleTalk_MacClassic::sTranslateError(err);
		if (paramBlock.fParamBlock.u.ioParams.actCount == 0)
			return ZNet::errorBadState;
		if (outCountReceived)
			*outCountReceived +=paramBlock.fParamBlock.u.ioParams.actCount;
		countRemaining -= paramBlock.fParamBlock.u.ioParams.actCount;
		localDest += paramBlock.fParamBlock.u.ioParams.actCount;
		}
	return ZNet::errorNone;
	}

ZNet::Error ZNetEndpoint_ADSP_MacClassic::Send(const void* inSource, size_t inCount, size_t* outCountSent, bool inPush)
	{
	if (outCountSent)
		*outCountSent = 0;
	short theADSPRefNum;
	OSErr err = ZNet_AppleTalk_MacClassic::sGetADSPRefNum(theADSPRefNum);

	ThreadedDSPParamBlock paramBlock;
	paramBlock.fParamBlock.ioCRefNum = theADSPRefNum;
	paramBlock.fParamBlock.ioCompletion = nil;
	paramBlock.fParamBlock.ccbRefNum = fCCB.refNum;
	paramBlock.fParamBlock.u.ioParams.reqCount = inCount;
	paramBlock.fParamBlock.u.ioParams.dataPtr = const_cast<void*>(inSource);
	paramBlock.fParamBlock.u.ioParams.eom = 0;
	paramBlock.fParamBlock.u.ioParams.flush = inPush ? 1 : 0;
	paramBlock.fParamBlock.csCode = dspWrite;
	err = ZThreadTM_PBControlAndBlock(&paramBlock);
	if (outCountSent)
		*outCountSent = paramBlock.fParamBlock.u.ioParams.actCount;
	return ZNet_AppleTalk_MacClassic::sTranslateError(err);
	}

ZNetAddress* ZNetEndpoint_ADSP_MacClassic::GetRemoteAddress()
	{
	// Only return an address object if we're connected
	if (fCCB.state != 4)
		return nil;
	return new ZNetAddress_AppleTalk(fCCB.remoteAddress.aNet, fCCB.remoteAddress.aNode, fCCB.remoteAddress.aSocket);
	}

#endif // ZCONFIG(API_Net, MacClassic)
// =================================================================================================
