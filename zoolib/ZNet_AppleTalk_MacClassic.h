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

#ifndef __ZNet_AppleTalk_MacClassic__
#define __ZNet_AppleTalk_MacClassic__ 1
#include "zconfig.h"

#include "zoolib/ZNet_AppleTalk.h"

// =================================================================================================
#if ZCONFIG(API_Net, MacClassic)

#include "zoolib/ZThread.h"
#include <ADSP.h>

// ==================================================
#pragma mark -
#pragma mark * ThreadedMPPParamBlock

struct ThreadedMPPParamBlock : public ZThreadTM_PBHeader
	{
	MPPParamBlock fParamBlock;

	ThreadedMPPParamBlock();
	void ZeroContents();
	};

// ==================================================
#pragma mark -
#pragma mark * ZNet_AppleTalk_MacClassic

class ZNet_AppleTalk_MacClassic
	{
protected:
	static OSErr sGetADSPRefNum(short& theRefNum);
	static ZNet::Error sTranslateError(OSErr nativeError);

private:
	static short sADSPRefNum;
	};

// ==================================================
#pragma mark -
#pragma mark * ZNetNameRegistered_AppleTalk

class ZNetNameRegistered_AppleTalk_MacClassic : public ZNetNameRegistered_AppleTalk
	{
public:
	ZNetNameRegistered_AppleTalk_MacClassic(const string& inName, const string& inType, const string& inZone, uint8 InSocket);
	virtual ~ZNetNameRegistered_AppleTalk_MacClassic();

// From ZNetNameRegistered_AppleTalk
	virtual string GetName() const;
	virtual string GetType() const;
	virtual string GetZone() const;
	virtual uint8 GetSocket() const;

// Our protocol
	bool IsRegistrationGood() const;

private:
	NamesTableEntry fNamesTableEntry;
	bool fRegisteredOK;
	};

// ==================================================
#pragma mark -
#pragma mark * ZNetNameLookup_AppleTalk_MacClassic

class ZNetNameLookup_AppleTalk_MacClassic : public ZNetNameLookup
	{
public:
	ZNetNameLookup_AppleTalk_MacClassic(const string& inName, const string& inType, const string& inZone, size_t inMaxAddresses);
	virtual ~ZNetNameLookup_AppleTalk_MacClassic();

// From ZNetNameLookup
	virtual void Start();
	virtual bool Finished();
	virtual void Next();
	virtual const ZNetAddress& CurrentAddress();
	virtual const ZNetName& CurrentName();

protected:
	void Internal_GrabNextResult();

	bool fStarted;
	size_t fNextIndex;
	size_t fMaxAddresses;
	ZNetAddress* fCurrentAddress;
	ZNetName* fCurrentName;

	char* fInfoBuffer;
	EntityName fEntityName;
	ThreadedMPPParamBlock fMPPParamBlock;
	};

// ==================================================
#pragma mark -
#pragma mark * ZNetListener_ADSP_MacClassic

class ZNetListener_ADSP_MacClassic : public ZNetListener_ADSP,
							private ZNet_AppleTalk_MacClassic
	{
private:
	friend class ZNetEndpoint_ADSP_MacClassic;
	struct ListenInfo
		{
		short fRemoteCID;
		AddrBlock fRemoteAddress;
		long fSendSeq;
		short fSendWindow;
		long fAttnSendSeq;
		};

public:
	ZNetListener_ADSP_MacClassic();
	virtual ~ZNetListener_ADSP_MacClassic();

// From ZNetListener via ZNetListener_ADSP
	virtual ZRef<ZNetEndpoint> Listen();
	virtual void CancelListen();

// From ZNetListener_ADSP
	virtual uint8 GetSocket();

private:
	ZMutex fListenMutex;
	bool fCancelling;
	short fADSPRefNum;
	TRCCB fCCB;
	uint8 fLocalSocket;

	size_t fSendBufferSize;
	size_t fReceiveBufferSize;
	};

// ==================================================
#pragma mark -
#pragma mark * ZNetEndpoint_ADSP_MacClassic

class ZNetEndpoint_ADSP_MacClassic : private ZStreamR,
						private ZStreamW,
						public ZNetEndpoint_ADSP,
						private ZNet_AppleTalk_MacClassic
	{
private:
	friend class ZNetListener_ADSP_MacClassic;
	ZNetEndpoint_ADSP_MacClassic(size_t iSendBufferSize, size_t iReceiveBufferSize, const ZNetListener_ADSP_MacClassic::ListenInfo& iInfo);

public:
	ZNetEndpoint_ADSP_MacClassic(int16 iNet, uint8 iNode, int8 iSocket);
	virtual ~ZNetEndpoint_ADSP_MacClassic();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamerR via ZNetEndpoint_ADSP
	virtual ZStreamR& GetStreamR();

// From ZStreamerW via ZNetEndpoint_ADSP
	virtual ZStreamW& GetStreamW();

// From ZNetEndpoint via ZNetEndpoint_ADSP
	virtual void SendDisconnect();
	virtual void ReceiveDisconnect();
	virtual void Disconnect();
	virtual void Abort();

	virtual ZNetAddress* GetRemoteAddress();

private:
	TRCCB fCCB;
	uint8 fLocalSocket;

	char* fSendBuffer;
	size_t fSendBufferSize;
	char* fReceiveBuffer;
	size_t fReceiveBufferSize;
	char* fAttentionBuffer;
	};

#endif // ZCONFIG(API_Net, MacClassic)
// =================================================================================================


#endif // __ZNet_AppleTalk_MacClassic__
