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

#ifndef __ZNet_Internet_MacClassic__
#define __ZNet_Internet_MacClassic__ 1
#include "zconfig.h"

#include "zoolib/ZNet_Internet.h"

// =================================================================================================
#if 0
#include "zoolib/ZThread.h"

#include <AddressXlation.h>
#include <MacTCP.h>

#include <vector>

// ==================================================
#pragma mark -
#pragma mark * ZNet_Internet_MacClassic

//! ZNet_Internet_MacClassic is a holder for process-wide utilities and information regarding classic (MacTCP) internet stuff

class ZNet_Internet_MacClassic
	{
protected:
	struct Threaded_TCPiopb : public ZThreadTM_PBHeader
		{
		TCPiopb fParamBlock;

		Threaded_TCPiopb();
		void ZeroContents();
		};

	struct Threaded_UDPiopb;

	static OSErr sGetTCPRefNum(short& outRefNum);
	static ZNet::Error sTranslateError(OSErr inNativeError);

private:
	static short sMacTCPRefNum;
	static DEFINE_API(void) sNotificationProc(StreamPtr inTCPStream, unsigned short inEventCode, Ptr inUserDataPtr, unsigned short inTerminReason, struct ICMPReport* inICMPMessage);
	static TCPNotifyUPP sNotificationProcUPP;
	};

// ==================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Internet_MacClassic

class ZNetNameLookup_Internet_MacClassic : public ZNetNameLookup
	{
public:
	ZNetNameLookup_Internet_MacClassic(const string& inName, ip_port inPort, size_t inMaxAddresses);
	virtual ~ZNetNameLookup_Internet_MacClassic();

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
	hostInfo fHostInfo;

	static pascal void sDNRResultProc(struct hostInfo* inHostInfo, Ptr inUserDataPtr);
	static ResultUPP sDNRResultProcUPP;
	};

// ==================================================
#pragma mark -
#pragma mark * ZNetListener_TCP_MacClassic

class ZNetListener_TCP_MacClassic
:	public ZNetListener_TCP,
	protected ZNet_Internet_MacClassic
	{
	friend class ZNetEndpoint_TCP_MacClassic;
	struct MacTCPControlBlock
		{
		MacTCPControlBlock(size_t bufferSize);
		~MacTCPControlBlock();
		Threaded_TCPiopb fPB;
		StreamPtr fStreamPtr;
		char* fBuffer;
		};

public:
	ZNetListener_TCP_MacClassic(ip_port iLocalPort, size_t iListenQueueSize);
	virtual ~ZNetListener_TCP_MacClassic();

// From ZNetListener_TCP
	virtual ZRef<ZNetEndpoint> Listen();
	virtual void CancelListen();

protected:
	void ReplenishListeningSockets();

	ip_port fLocalPort;
	size_t fListenQueueSize;
	size_t fBufferSize;
	vector<MacTCPControlBlock*> fPendingControlBlocks;
	vector<MacTCPControlBlock*> fCompletedControlBlocks;
	ZMutex fListenMutex;
	ZMutex fAccessMutex;
	};

// ==================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP_MacClassic

class ZNetEndpoint_TCP_MacClassic
:	private ZStreamR,
	private ZStreamW,
	public ZNetEndpoint_TCP,
	protected ZNet_Internet_MacClassic
	{
private:
	friend class ZNetListener_TCP_MacClassic;
	ZNetEndpoint_TCP_MacClassic(ZNetListener_TCP_MacClassic::MacTCPControlBlock* iControlBlock);

public:
	ZNetEndpoint_TCP_MacClassic(ip_addr iRemoteHost, ip_port iRemotePort);
	virtual ~ZNetEndpoint_TCP_MacClassic();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamerR via ZNetEndpoint_TCP
	virtual ZStreamR& GetStreamR();

// From ZStreamerW via ZNetEndpoint_TCP
	virtual ZStreamW& GetStreamW();

// From ZNetEndpoint via ZNetEndpoint_TCP
	virtual bool WaitTillReadable(int iMilliseconds);
	virtual void SendDisconnect();
	virtual bool ReceiveDisconnect(int iMilliseconds);
	virtual void Disconnect();
	virtual void Abort();

	virtual ZNetAddress* GetRemoteAddress();

private:
	StreamPtr fStreamPtr;
	char* fBuffer;

	friend class ZNetListener_TCP_MacClassic;
	};

#if 0
// ==================================================
#pragma mark -
#pragma mark * ZNetEndpointDG_UDP_MacClassic

class ZNetEndpointDG_UDP_MacClassic : public ZNetEndpointDG_UDP, private ZNet_Internet_MacClassic
	{
public:
	ZNetEndpointDG_UDP_MacClassic(size_t inInternalBufferSize);
	ZNetEndpointDG_UDP_MacClassic(ip_port inLocalPort, size_t inInternalBufferSize);
	virtual ~ZNetEndpointDG_UDP_MacClassic();

	virtual ZNet::Error Receive(void* inBuffer, size_t inBufferSize, size_t& outCountReceived, ZNetAddress*& outSourceAddress);
	virtual ZNet::Error Send(const void* inBuffer, size_t inCount, ZNetAddress* inDestAddress);

private:
	StreamPtr fStreamPtr;
	char* fBuffer;
	};
#endif
#endif // 0
// =================================================================================================

#endif // __ZNet_Internet_MacClassic__
