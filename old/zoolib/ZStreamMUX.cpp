/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#include "zoolib/ZStreamMUX.h"

#include "zoolib/ZCommer.h"
#include "zoolib/ZDList.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_vector.h"

using std::deque;
using std::map;
using std::min;
using std::pair;
using std::string;
using std::vector;

namespace ZooLib {

#define kDebug 1

// =================================================================================================
// MARK: - ZStreamMUX, definitions and helper functions

const size_t kDefaultMaxFragmentSize = 1024;
const size_t kDefaultReceiveBufferSize = 16 * 1024;

enum EStateReceive
	{
	eStateReceive_Closed,
	eStateReceive_CloseReceived,
	eStateReceive_ClosePending,
	eStateReceive_CloseSent,
	eStateReceive_Open
	};

enum EStateSend
	{
	eStateSend_Closed,
	eStateSend_CloseReceived,
	eStateSend_ClosePending,
	eStateSend_CloseSent,
	eStateSend_Open
	};

enum EStateEP
	{
	eStateEP_Accepted,
	eStateEP_ConnectPending,
	eStateEP_ConnectSent,
	eStateEP_Connected
	};

enum EMsg
	{
	eMsg_Open = 0x10,
	eMsg_OpenAck = 0x11,
	eMsg_OpenNack = 0x12,

	eMsg_CloseYourReceive = 0x20,

	eMsg_CloseYourSend = 0x30,

	eMsg_Data = 0x40,

	eMsg_AddCredit = 0x50,

	eMsg_Ping = 0x60,
	eMsg_Pong = 0x61,

	eMsg_Goodbye = 0x80
	};

// These functions will ultimately pack small values into a byte, whilst
// still being able to handle larger values.
static uint32 spReceivePackedInt(const ZStreamR& iStreamR)
	{
	return iStreamR.ReadCount();
	}

static void spSendPackedInt(const ZStreamW& iStreamW, uint32 iInt)
	{
	iStreamW.WriteCount(iInt);
	}

static uint32 spReceivePackedID(const ZStreamR& iStreamR)
	{
	return spReceivePackedInt(iStreamR);
	}

static void spSendPackedID(const ZStreamW& iStreamW, uint32 iInt)
	{
	spSendPackedInt(iStreamW, iInt ^ 0x1);
	}

static string spReceivePackedString(const ZStreamR& iStreamR)
	{
	if (size_t theLength = spReceivePackedInt(iStreamR))
		return iStreamR.ReadString(theLength);

	return string();
	}

static void spSendPackedString(const ZStreamW& iStreamW, const string& iString)
	{
	size_t theLength = iString.length();
	spSendPackedInt(iStreamW, theLength);
	iStreamW.WriteString(iString);
	}

static bool spIsLocal(uint32 iID)
	{ return iID & 0x1; }

static bool spIsRemote(uint32 iID)
	{ return !spIsLocal(iID); }

// =================================================================================================
// MARK: - ZStreamMUX::Commer

class ZStreamMUX::Commer : public ZCommer
	{
public:
	Commer(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW, ZRef<ZStreamMUX> iStreamMUX);

// From ZStreamerReader via ZCommer
	virtual bool Read(const ZStreamR& iStreamR);

// From ZStreamerWriter via ZCommer
	virtual bool Write(const ZStreamW& iStreamW);

// From ZCommer
	virtual void Started();
	virtual void Finished();

private:
	ZWeakRef<ZStreamMUX> fStreamMUX;
	};

ZStreamMUX::Commer::Commer(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
	ZRef<ZStreamMUX> iStreamMUX)
:	ZCommer(iStreamerR, iStreamerW),
	fStreamMUX(iStreamMUX)
	{}

bool ZStreamMUX::Commer::Read(const ZStreamR& iStreamR)
	{
	if (ZRef<ZStreamMUX> theStreamMUX = fStreamMUX)
		return theStreamMUX->pRead(iStreamR);
	return false;
	}

bool ZStreamMUX::Commer::Write(const ZStreamW& iStreamW)
	{
	if (ZRef<ZStreamMUX> theStreamMUX = fStreamMUX)
		return theStreamMUX->pWrite(iStreamW);
	return false;
	}

void ZStreamMUX::Commer::Started()
	{}

void ZStreamMUX::Commer::Finished()
	{
	if (ZRef<ZStreamMUX> theStreamMUX = fStreamMUX)
		theStreamMUX->Finished();
	}

// =================================================================================================
// MARK: - ZStreamMUX::Endpoint

class ZStreamMUX::DLink_Endpoint_Pending
:	public DListLink<ZStreamMUX::Endpoint, ZStreamMUX::DLink_Endpoint_Pending, kDebug>
	{};

class ZStreamMUX::Endpoint
:	public ZStreamerRWCon,
	public ZStreamMUX::DLink_Endpoint_Pending,
	private ZStreamRCon,
	private ZStreamWCon
	{
public:
	friend class ZStreamMUX;

	Endpoint(ZRef<ZStreamMUX> iMUX, uint32 iEPID, size_t iReceiveBufferSize,
		Listener* iListener, size_t iCreditLimit);

	Endpoint(ZRef<ZStreamMUX> iMUX, uint32 iEPID, size_t iReceiveBufferSize,
		const string* iOpenName);

	virtual ~Endpoint();

// From ZCounted via ZStreamerRWCon
	virtual void Finalize();

// From ZStreamerRCon via ZStreamerRWCon
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZStreamerRWCon
	virtual const ZStreamWCon& GetStreamWCon();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(double iTimeout);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	ZWeakRef<ZStreamMUX> fMUX;

	ZCnd fCondition_Receive;
	ZCnd fCondition_Send;
	uint32 fEPID;

	Listener* fListener;

	const string* fOpenName;
	const size_t fReceiveBufferSize;

	EStateEP fStateEP;
	EStateReceive fStateReceive;
	EStateSend fStateSend;

	deque<uint8> fBuffer_Receive;
	size_t fCount_Received;
	size_t fCount_Acked;

	deque<uint8> fBuffer_Send;
	size_t fCreditRemaining;
	bool fReceivedSinceLastCredit;
	};

// Called by ZStreamMUX when accepting a connection.
ZStreamMUX::Endpoint::Endpoint(ZRef<ZStreamMUX> iMUX, uint32 iEPID, size_t iReceiveBufferSize,
	Listener* iListener, size_t iCreditLimit)
:	fMUX(iMUX),
	fEPID(iEPID),
	fReceiveBufferSize(iReceiveBufferSize),
	fListener(iListener),
	fOpenName(nullptr),
	fStateEP(eStateEP_Accepted),
	fStateReceive(eStateReceive_Open),
	fStateSend(eStateSend_Open),
	fCount_Received(0),
	fCount_Acked(0),
	fCreditRemaining(iCreditLimit),
	fReceivedSinceLastCredit(true)
	{
	ZAssert(spIsRemote(fEPID));
	}

// Called by ZStreamMUX when opening a connection
ZStreamMUX::Endpoint::Endpoint(ZRef<ZStreamMUX> iMUX, uint32 iEPID, size_t iReceiveBufferSize,
	const string* iOpenName)
:	fMUX(iMUX),
	fEPID(iEPID),
	fReceiveBufferSize(iReceiveBufferSize),
	fListener(nullptr),
	fOpenName(iOpenName),
	fStateEP(eStateEP_ConnectPending),
	fStateReceive(eStateReceive_Open),
	fStateSend(eStateSend_Open),
	fCount_Received(0),
	fCount_Acked(0),
	fCreditRemaining(0),
	fReceivedSinceLastCredit(true)
	{
	ZAssert(spIsLocal(fEPID));
	}

ZStreamMUX::Endpoint::~Endpoint()
	{
	ZAssert(fStateReceive == eStateReceive_CloseReceived
		|| fStateReceive == eStateReceive_Closed);
	ZAssert(fStateSend == eStateSend_Closed);

	if (ZLOG(s, eDebug + 1, "ZStreamMUX::Endpoint"))
		s.Writef("~Endpoint, this: %p", this);
	}

void ZStreamMUX::Endpoint::Finalize()
	{
	bool hasMUX = false;
	bool needsDelete = false;

	if (ZRef<ZStreamMUX> theMUX = fMUX)
		{
		hasMUX = true;
		// Endpoint_Finalize calls our FinalizationComplete, we don't do it here.
		if (theMUX->Endpoint_Finalize(this))
			needsDelete = true;
		}

	if (hasMUX)
		{
		if (needsDelete)
			delete this;
		return;
		}

	if (this->FinishFinalize())
		delete this;
	}

const ZStreamRCon& ZStreamMUX::Endpoint::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& ZStreamMUX::Endpoint::GetStreamWCon()
	{ return *this; }

void ZStreamMUX::Endpoint::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		{
		theMUX->Endpoint_Read(this, oDest, iCount, oCountRead);
		}
	else
		{
		if (oCountRead)
			*oCountRead = 0;
		}
	}

size_t ZStreamMUX::Endpoint::Imp_CountReadable()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		return theMUX->Endpoint_CountReadable(this);
	else
		return 0;
	}

bool ZStreamMUX::Endpoint::Imp_WaitReadable(double iTimeout)
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		return theMUX->Endpoint_WaitReadable(this, iTimeout);
	return true;
	}

void ZStreamMUX::Endpoint::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		{
		theMUX->Endpoint_Write(this, iSource, iCount, oCountWritten);
		}
	else
		{
		if (oCountWritten)
			*oCountWritten = 0;
		}
	}

bool ZStreamMUX::Endpoint::Imp_ReceiveDisconnect(double iTimeout)
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		theMUX->Endpoint_ReceiveDisconnect(this, iTimeout);
	return true;
	}

void ZStreamMUX::Endpoint::Imp_SendDisconnect()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		theMUX->Endpoint_SendDisconnect(this);
	}

void ZStreamMUX::Endpoint::Imp_Abort()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		theMUX->Endpoint_Abort(this);
	}

// =================================================================================================
// MARK: - ZStreamMUX::Listener

class ZStreamMUX::Listener : public ZStreamerRWConFactory
	{
public:
	friend class ZStreamMUX;

	Listener(ZRef<ZStreamMUX> iMUX, const string& iName, size_t iReceiveBufferSize);
	virtual ~Listener();

// From ZCounted via ZStreamerRWConFactory
	virtual void Finalize();

// From ZStreamerRWConFactory
	virtual ZRef<ZStreamerRWCon> MakeStreamerRWCon();
	virtual void Cancel();

private:
	ZWeakRef<ZStreamMUX> fMUX;
	ZCnd fCondition;
	const string fName;
	const size_t fReceiveBufferSize;
	DListHead<DLink_Endpoint_Pending> fEndpoints_Pending;
	bool fCancelled;
	};

ZStreamMUX::Listener::Listener(ZRef<ZStreamMUX> iMUX,
	const string& iName, size_t iReceiveBufferSize)
:	fMUX(iMUX),
	fName(iName),
	fReceiveBufferSize(iReceiveBufferSize),
	fCancelled(false)
	{}

ZStreamMUX::Listener::~Listener()
	{
	ZAssert(fEndpoints_Pending.Empty());
	if (ZLOG(s, eDebug + 1, "ZStreamMUX::"))
		s.Writef("~Listener, this: %p", this);
	}

void ZStreamMUX::Listener::Finalize()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		{
		theMUX->Listener_Finalize(this);
		return;
		}

	if (this->FinishFinalize())
		delete this;
	}

ZRef<ZStreamerRWCon> ZStreamMUX::Listener::MakeStreamerRWCon()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		return theMUX->Listener_Listen(this);
	else
		return null;
	}

void ZStreamMUX::Listener::Cancel()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX)
		theMUX->Listener_Cancel(this);
	}

// =================================================================================================
// MARK: - ZStreamMUX::Options

ZStreamMUX::Options::Options()
:	fMaxFragmentSize(kDefaultMaxFragmentSize),
	fDefaultReceiveBufferSize(kDefaultReceiveBufferSize)
	{}

ZStreamMUX::Options::Options(size_t iMaxFragmentSize, size_t iDefaultReceiveBufferSize)
:	fMaxFragmentSize(iMaxFragmentSize),
	fDefaultReceiveBufferSize(iDefaultReceiveBufferSize)
	{}

// =================================================================================================
// MARK: - ZStreamMUX

ZStreamMUX::ZStreamMUX()
:	fMaxFragmentSize(kDefaultMaxFragmentSize),
	fDefaultReceiveBufferSize(kDefaultReceiveBufferSize),
	fLifecycle(eLifecycle_Running),
	fTime_LastRead(0),
	fPingInterval(0),
	fPingReceived(false),
	fPingSent(false)
	{}

ZStreamMUX::ZStreamMUX(const Options& iOptions)
:	fMaxFragmentSize(iOptions.fMaxFragmentSize),
	fDefaultReceiveBufferSize(iOptions.fDefaultReceiveBufferSize),
	fLifecycle(eLifecycle_Running),
	fTime_LastRead(0),
	fPingInterval(0),
	fPingReceived(false),
	fPingSent(false)
	{}

ZStreamMUX::~ZStreamMUX()
	{
	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		s.Writef("~ZStreamMUX, this: %p", this);

	ZAssert(fMap_IDToEndpoint.empty());
	ZAssert(fMap_NameToListener.empty());

	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		s.Writef("~ZStreamMUX, this: %p, Exiting", this);
	}

void ZStreamMUX::Finalize()
	{
	ZGuardRMtxR locker(fMutex);
	if (this->IsShared())
		{
		this->FinishFinalize();
		return;
		}

	for (map<string, Listener*>::iterator i = fMap_NameToListener.begin();
		i != fMap_NameToListener.end(); ++i)
		{
		Listener* theListener = i->second;
		theListener->fCancelled = true;
		theListener->fCondition.Broadcast();
		}
	fMap_NameToListener.clear();

	this->Stop();
	if (fCommer)
		{
		fCommer->WaitTillFinished();
		fCommer.Clear();
		}

	fMap_IDToEndpoint.clear();

	if (this->FinishFinalize())
		{
		locker.Release();
		delete this;
		}
	}

void ZStreamMUX::Finished()
	{

	}

void ZStreamMUX::Start(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
	{
	ZAssert(!fCommer);
	fCommer = new Commer(iStreamerR, iStreamerW, this);
	sStartCommerRunners(fCommer);
	}

void ZStreamMUX::Stop()
	{
	ZGuardRMtxR locker(fMutex);

	if (fLifecycle == eLifecycle_Running)
		{
		fLifecycle = eLifecycle_StoppingRun;

		// Abort any endpoints that are connected.
		for (map<uint32, Endpoint*>::iterator i = fMap_IDToEndpoint.begin();
			i != fMap_IDToEndpoint.end(); ++i)
			{
			this->pAbort(i->second);
			}
		fCommer->WakeWrite();
		}
	else
		{
		if (ZLOG(s, eDebug, "ZStreamMUX"))
			s << "Stop, already stopping/stopped";
		}
	}

ZRef<ZStreamerRWConFactory> ZStreamMUX::Listen(const std::string& iName)
	{ return this->Listen(iName, fDefaultReceiveBufferSize); }

ZRef<ZStreamerRWConFactory> ZStreamMUX::Listen
	(const std::string& iName, size_t iReceiveBufferSize)
	{
	ZGuardRMtxR locker(fMutex);
	map<std::string, Listener*>::iterator i = fMap_NameToListener.find(iName);
	if (i == fMap_NameToListener.end())
		{
		ZRef<Listener> theListener = new Listener(this, iName, iReceiveBufferSize);
		fMap_NameToListener.insert(pair<string, Listener*>(iName, theListener.Get()));
		return theListener;
		}
	return null;
	}

ZRef<ZStreamerRWCon> ZStreamMUX::Connect(const std::string& iName)
	{ return this->Connect(iName, fDefaultReceiveBufferSize); }

ZRef<ZStreamerRWCon> ZStreamMUX::Connect(const std::string& iName, size_t iReceiveBufferSize)
	{
	ZGuardRMtxR locker(fMutex);

	if (fLifecycle == eLifecycle_Running)
		{
		const uint32 theEPID = this->pGetUnusedID();
		ZRef<Endpoint> theEndpoint = new Endpoint(this, theEPID, iReceiveBufferSize, &iName);

		if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
			{
			s.Writef("Connect, this: %p, Connect starting , theID: %d, theState: %d",
				this, theEPID, theEndpoint->fStateEP);
			}

		fMap_IDToEndpoint.insert(pair<uint32, Endpoint*>(theEPID, theEndpoint.Get()));
		fCommer->WakeWrite();

		while (theEndpoint->fStateEP != eStateEP_Connected)
			theEndpoint->fCondition_Receive.Wait(fMutex);

		if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
			{
			s.Writef("Connect, this: %p, Connect done, theID: %d, theState: %d",
				this, theEPID, theEndpoint->fStateEP);
			}

		if (theEndpoint->fStateSend == eStateSend_Open)
			return theEndpoint;
		}

	return null;
	}

uint32 ZStreamMUX::sGetConID(ZRef<ZStreamerRWCon> iCon)
	{
	if (ZRef<Endpoint> theEP = iCon.DynamicCast<Endpoint>())
		return theEP->fEPID;
	return 0;
	}

void ZStreamMUX::SetPingInterval(double iInterval)
	{
	ZGuardRMtxR locker(fMutex);
	fPingInterval = iInterval;
	if (fCommer)
		fCommer->WakeWrite();
	}

bool ZStreamMUX::pRead(const ZStreamR& iStreamR)
	{
	ZGuardRMtxR locker(fMutex);
	if (fLifecycle == eLifecycle_Running
		|| fLifecycle == eLifecycle_StoppingRun)
		{
		locker.Release();
		try
			{
			this->pReadOne(iStreamR);
			locker.Acquire();
			}
		catch (...)
			{
			locker.Acquire();
			fLifecycle = eLifecycle_StreamsDead;
			fCommer->WakeWrite();
			}
		}

	return (fLifecycle == eLifecycle_Running
		|| fLifecycle == eLifecycle_StoppingRun);
	}

bool ZStreamMUX::pWrite(const ZStreamW& iStreamW)
	{
	ZGuardRMtxR locker(fMutex);

	uint32 nextEPID = 1;
	bool anEndpointWroteSinceReset = false;
	bool writtenSinceFlush = false;
	while (fLifecycle == eLifecycle_Running
		|| fLifecycle == eLifecycle_StoppingRun)
		{
		if (fPingReceived)
			{
			fPingReceived = false;
			if (this->pSendMessage(iStreamW, eMsg_Pong))
				writtenSinceFlush = true;
			}

		// Interleave nacks and endpoint handling, to alleviate DOS.
		if (!fOpenNacksToSend.empty())
			{
			const uint32 theID = fOpenNacksToSend.front();
			fOpenNacksToSend.pop_front();
			if (this->pSendMessage(iStreamW, eMsg_OpenNack, theID))
				writtenSinceFlush = true;
			}

		if (!fPingSent
			&& fPingInterval > 0
			&& fTime_LastRead + fPingInterval < ZTime::sSystem())
			{
			fPingSent = true;
			if (this->pSendMessage(iStreamW, eMsg_Ping))
				writtenSinceFlush = true;
			}

		bool anEndpointWrote = false;
		while (!anEndpointWrote)
			{
			map<uint32, Endpoint*>::iterator iter = fMap_IDToEndpoint.lower_bound(nextEPID);
			++nextEPID;
			if (iter == fMap_IDToEndpoint.end())
				break;

			Endpoint* theEP = iter->second;
			if (this->pWriteOne(iStreamW, theEP))
				{
				writtenSinceFlush = true;
				anEndpointWrote = true;
				anEndpointWroteSinceReset = true;
				}
			}

		if (!anEndpointWrote)
			{
			if (!anEndpointWroteSinceReset)
				{
				if (writtenSinceFlush)
					{
					writtenSinceFlush = false;
					this->pFlush(iStreamW);
					}
				else
					{
					if (fLifecycle == eLifecycle_StoppingRun && fMap_IDToEndpoint.empty())
						{
						break;
						}
					else
						{
						if (fPingInterval > 0)
							{
							fCommer.StaticCast<ZStreamerWriter>()->
								WakeAt(ZTime::sSystem() + fPingInterval);
							}
						return true;
						}
					}
				}
			nextEPID = 1;
			anEndpointWroteSinceReset = false;
			}
		}

	if (fLifecycle == eLifecycle_StreamsDead || eLifecycle_ReadDead)
		{
//		vector<ZRef<Endpoint> > localEPs;
		for (map<uint32, Endpoint*>::iterator i = fMap_IDToEndpoint.begin();
			i != fMap_IDToEndpoint.end(); ++i)
			{
			Endpoint* theEP = i->second;
			theEP->fStateEP = eStateEP_Connected; // Really means it's not connectING.
			theEP->fStateReceive = eStateReceive_Closed;
			theEP->fStateSend = eStateSend_Closed;
			theEP->fCondition_Receive.Broadcast();
			theEP->fCondition_Send.Broadcast();
			if (theEP->fListener)
				{
				theEP->fListener->fEndpoints_Pending.Erase(theEP);
				theEP->fListener = nullptr;
				}
//			localEPs.push_back(theEP);
			}
		fMap_IDToEndpoint.clear();

		locker.Release();

		// Clear their references to us.
//		for (vector<ZRef<Endpoint> >::iterator i = localEPs.begin();
//			i != localEPs.end(); ++i)
//			{ (*i)->fPUC_MUX.Clear(); }

		locker.Acquire();
		}

	if (fLifecycle != eLifecycle_StreamsDead)
		{
		ZAssert(fMap_IDToEndpoint.empty());

		if (ZLOG(s, eDebug, "ZStreamMUX"))
			s.Writef("Write, this: %p, send goodbye", this);
		this->pSendMessage(iStreamW, eMsg_Goodbye);
		this->pFlush(iStreamW);
		}
	return false;
	}

bool ZStreamMUX::Endpoint_Finalize(Endpoint* iEP)
	{
	ZGuardRMtxR locker(fMutex);

	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		{
		s.Writef("Endpoint_Finalize, this: %p, theID: %d",
			this, iEP->fEPID);
		}

	if (not iEP->FinishFinalize())
		return false;

	if (iEP->fStateSend == eStateSend_Open)
		{
		iEP->fStateSend = eStateSend_ClosePending;
		}

	if (iEP->fStateReceive == eStateReceive_Open)
		{
		iEP->fStateReceive = eStateReceive_ClosePending;
		iEP->fBuffer_Receive.clear();
		}

	iEP->fCondition_Receive.Broadcast();
	fCommer->WakeWrite();

	return this->pDetachIfUnused(iEP);
	}

void ZStreamMUX::Endpoint_Read(Endpoint* iEP,
	void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(oDest);
	deque<uint8>& theBR = iEP->fBuffer_Receive;

	ZGuardRMtxR locker(fMutex);
	while (iCount)
		{
		if (const size_t countToRead = min(iCount, theBR.size()))
			{
			copy(theBR.begin(), theBR.begin() + countToRead, localDest);
			localDest += countToRead;
			iCount -= countToRead;
			theBR.erase(theBR.begin(), theBR.begin() + countToRead);
			iEP->fCount_Received += countToRead;
			fCommer->WakeWrite();
			break;
			}
		else if (iEP->fStateReceive == eStateReceive_CloseReceived
			|| iEP->fStateReceive == eStateReceive_Closed)
			{
			break;
			}
		iEP->fCondition_Receive.Wait(fMutex);
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(oDest);
	}

size_t ZStreamMUX::Endpoint_CountReadable(Endpoint* iEP)
	{
	ZGuardRMtxR locker(fMutex);
//	if (iEP->fStateEP != eStateEP_Connected
//		|| iEP->fStateReceive == eStateReceive_Closed)
//		{
//		return 0;
//		}

	return iEP->fBuffer_Receive.size();
	}

bool ZStreamMUX::Endpoint_WaitReadable(Endpoint* iEP, double iTimeout)
	{
	const ZTime deadline = ZTime::sSystem() + iTimeout;
	ZGuardRMtxR locker(fMutex);
	for (;;)
		{
		if (iEP->fStateEP != eStateEP_Connected
			|| iEP->fStateReceive == eStateReceive_Closed
			|| iEP->fStateReceive == eStateReceive_CloseReceived
			|| iEP->fBuffer_Receive.size())
			{
			return true;
			}

		if (not iEP->fCondition_Receive.WaitUntil(fMutex, deadline))
			return false;
		}
	}

void ZStreamMUX::Endpoint_Write(Endpoint* iEP,
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	ZGuardRMtxR locker(fMutex);
	const uint8* localSource = static_cast<const uint8*>(iSource);
	while (iCount)
		{
		if (iEP->fStateSend != eStateSend_Open)
			break;

		if (const size_t countToWrite = min(iCount, iEP->fCreditRemaining))
			{
			deque<uint8>& theBS = iEP->fBuffer_Send;
			theBS.insert(theBS.end(), localSource, localSource + countToWrite);
			localSource += countToWrite;
			iCount -= countToWrite;
			iEP->fCreditRemaining -= countToWrite;
			fCommer->WakeWrite();
			break;
			}
		else
			{
			iEP->fCondition_Send.Wait(fMutex);
			}
		}

	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const uint8*>(iSource);
	}

bool ZStreamMUX::Endpoint_ReceiveDisconnect(Endpoint* iEP, double iTimeout)
	{
	const ZTime deadline = ZTime::sSystem() + iTimeout;
	ZGuardRMtxR locker(fMutex);

	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		{
		s.Writef("Endpoint_ReceiveDisconnect, this: %p, theID: %d",
			this, iEP->fEPID);
		}

	for (;;)
		{
		if (size_t count = iEP->fBuffer_Receive.size())
			{
			iEP->fCount_Received += count;
			iEP->fBuffer_Receive.clear();
			fCommer->WakeWrite();
			}
		else if (iEP->fStateEP != eStateEP_Connected
			|| iEP->fStateReceive == eStateReceive_CloseReceived
			|| iEP->fStateReceive == eStateReceive_Closed)
			{
			return true;
			}

		if (not iEP->fCondition_Receive.WaitUntil(fMutex, deadline))
			return false;
		}
	}

void ZStreamMUX::Endpoint_SendDisconnect(Endpoint* iEP)
	{
	ZGuardRMtxR locker(fMutex);

	if (iEP->fStateEP == eStateEP_Connected && iEP->fStateSend == eStateSend_Open)
		{
		if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
			{
			s.Writef("Endpoint_SendDisconnect, this: %p, theID: %d",
				this, iEP->fEPID);
			}
		iEP->fStateSend = eStateSend_ClosePending;
		fCommer->WakeWrite();
		}
	else
		{
		if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
			{
			s.Writef("Endpoint_SendDisconnect, no effect, this: %p, theID: %d",
				this, iEP->fEPID);
			}
		}
	}

void ZStreamMUX::Endpoint_Abort(Endpoint* iEP)
	{
	ZGuardRMtxR locker(fMutex);

	this->pAbort(iEP);

	fCommer->WakeWrite();
	}

void ZStreamMUX::Listener_Finalize(Listener* iListener)
	{
	ZGuardRMtxR locker(fMutex);
	if (not iListener->FinishFinalize())
		return;

	map<std::string, Listener*>::iterator i = fMap_NameToListener.find(iListener->fName);
	ZAssert(i != fMap_NameToListener.end());
	fMap_NameToListener.erase(i);

	for (DListEraser<Endpoint, DLink_Endpoint_Pending>
		i = iListener->fEndpoints_Pending; i; i.Advance())
		{
		Endpoint* theEndpoint = i.Current();
		this->Endpoint_Abort(theEndpoint);
		theEndpoint->fListener = nullptr;
		}

	delete iListener;
	}

ZRef<ZStreamerRWCon> ZStreamMUX::Listener_Listen(Listener* iListener)
	{
	ZGuardRMtxR locker(fMutex);

	if (not iListener->fCancelled && iListener->fEndpoints_Pending.Empty())
		iListener->fCondition.Wait(fMutex);

	if (iListener->fEndpoints_Pending.Empty())
		return null;

	Endpoint* theEndpoint = iListener->fEndpoints_Pending.PopFront<Endpoint>();
	theEndpoint->fListener = nullptr;
	return theEndpoint;
	}

void ZStreamMUX::Listener_Cancel(Listener* iListener)
	{
	ZGuardRMtxR locker(fMutex);

	// Wake everything, and let any call to Listener_Listen complete.
	iListener->fCondition.Broadcast();
	}

uint32 ZStreamMUX::pGetUnusedID()
	{
	// The first candidate (local) ID is 3. If the current entry is at least
	// two greater than the candidate ID then the candidate is
	// not in use and can be returned. Otherwise the next candidate
	// is the odd number greater than the current entry's.

	uint32 nextPossibleID = 3;

	for (map<uint32, Endpoint*>::iterator i = fMap_IDToEndpoint.begin();
		i != fMap_IDToEndpoint.end(); ++i)
		{
		Endpoint* theEP = i->second;
		uint32 currentID = i->first;
		ZAssert(currentID == theEP->fEPID);
		if (currentID > nextPossibleID && currentID > nextPossibleID + 1)
			break;
		nextPossibleID = 1 | (currentID + 1);
		}

	return nextPossibleID;
	}

ZStreamMUX::Endpoint* ZStreamMUX::pGetEndpointNilOkay(uint32 iGlobalID)
	{
	map<uint32, Endpoint*>::iterator i = fMap_IDToEndpoint.find(iGlobalID);
	if (i != fMap_IDToEndpoint.end())
		return i->second;
	return nullptr;
	}

ZStreamMUX::Endpoint* ZStreamMUX::pGetEndpoint(uint32 iGlobalID)
	{
	map<uint32, Endpoint*>::iterator i = fMap_IDToEndpoint.find(iGlobalID);
	ZAssert(i != fMap_IDToEndpoint.end());
	return i->second;
	}

bool ZStreamMUX::pDetachIfUnused(Endpoint* iEP)
	{
	if (iEP->fStateSend != eStateSend_Closed)
		return false;

	if (iEP->fStateReceive != eStateReceive_Closed)
		return false;

	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		s.Writef("pDetachIfUnused, this: %p, detaching theID: %d", this, iEP->fEPID);

	// It's dead.
	if (iEP->fEPID)
		{
		ZUtil_STL::sEraseMustContain(kDebug, fMap_IDToEndpoint, iEP->fEPID);
		iEP->fEPID = 0;
		fCommer->WakeWrite();
		}

	return !iEP->IsReferenced();
	}

void ZStreamMUX::pAbort(Endpoint* iEP)
	{
	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		{
		s.Writef("pAbort, this: %p, theID: %d",
			this, iEP->fEPID);
		}

	if (iEP->fStateSend == eStateSend_Open)
		{
		iEP->fStateSend = eStateSend_ClosePending;
		iEP->fBuffer_Send.clear();

		if (iEP->fListener)
			{
			iEP->fListener->fEndpoints_Pending.Erase(iEP);
			iEP->fListener = nullptr;
			}
		}

	if (iEP->fStateReceive == eStateReceive_Open)
		{
		iEP->fStateReceive = eStateReceive_ClosePending;
		iEP->fBuffer_Receive.clear();
		}

	iEP->fCondition_Receive.Broadcast();
	iEP->fCondition_Send.Broadcast();
	fCommer->WakeWrite();
	}

void ZStreamMUX::pReadOne(const ZStreamR& iStreamR)
	{
	const uint8 theMessage = iStreamR.ReadUInt8();

	ZGuardRMtxR locker(fMutex);

	fTime_LastRead = ZTime::sSystem();

	switch (theMessage)
		{
		case eMsg_Ping:
			{
			fPingReceived = true;
			locker.Release();
			fCommer->WakeWrite();
			break;
			}
		case eMsg_Pong:
			{
			fPingSent = false;
			break;
			}
		case eMsg_Open:
			{
			locker.Release();

			// Open contains the remote sessionID, initial credit,
			// and the listener name against which to connect.
			const uint32 theID = spReceivePackedID(iStreamR);
			ZAssert(spIsRemote(theID));
			const uint32 creditLimit = spReceivePackedInt(iStreamR);
			const string listenerName = spReceivePackedString(iStreamR);

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %p, Open, theID: %d, creditLimit: %d, name: %s",
					this, theID, creditLimit, listenerName.c_str());
				}

			locker.Acquire();

			// Find the listener
			map<string, Listener*>::iterator i = fMap_NameToListener.find(listenerName);
			if (fLifecycle != eLifecycle_Running || i == fMap_NameToListener.end())
				{
				// Disallow the open if we're stopping, or of course if we're not
				// listening for that name.
				fOpenNacksToSend.push_back(theID);
				}
			else
				{
				Listener* theListener = i->second;
				Endpoint* theEP = new Endpoint(this, theID, theListener->fReceiveBufferSize,
					theListener, creditLimit);

				fMap_IDToEndpoint.insert(pair<uint32, Endpoint*>(theID, theEP));
				theListener->fEndpoints_Pending.PushBack(theEP);
				theListener->fCondition.Broadcast();
				}
			locker.Release();
			fCommer->WakeWrite();
			break;
			}
		case eMsg_OpenAck:
			{
			locker.Release();

			const uint32 theID = spReceivePackedID(iStreamR);
			ZAssert(spIsLocal(theID));
			const uint32 creditLimit = spReceivePackedInt(iStreamR);

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %p, OpenAck, theID: %d, creditLimit: %d",
					this, theID, creditLimit);
				}

			locker.Acquire();

			Endpoint* theEP = this->pGetEndpoint(theID);
			// We can be in another state, but only if we return the endpoint
			// from Connect before the connect completes.
			ZAssert(theEP->fStateEP == eStateEP_ConnectSent);
			theEP->fStateEP = eStateEP_Connected;
			theEP->fCreditRemaining = creditLimit;
			theEP->fCondition_Receive.Broadcast();
			break;
			}
		case eMsg_OpenNack:
			{
			locker.Release();

			// An attempt to contact a Listener failed.
			const uint32 theID = spReceivePackedID(iStreamR);
			ZAssert(spIsLocal(theID));

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %p, OpenNack, theID: %d",
					this, theID);
				}

			locker.Acquire();

			Endpoint* theEP = this->pGetEndpoint(theID);
			ZAssert(theEP->fStateEP == eStateEP_ConnectSent);
			theEP->fStateEP = eStateEP_Connected;
			theEP->fStateReceive = eStateReceive_Closed;
			theEP->fStateSend = eStateSend_Closed;
			theEP->fCondition_Receive.Broadcast();
			break;
			}
		case eMsg_CloseYourReceive:
			{
			locker.Release();

			const uint32 theID = spReceivePackedID(iStreamR);

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %p, CloseYourReceive, theID: %d",
					this, theID);
				}

			locker.Acquire();

			Endpoint* theEP = this->pGetEndpoint(theID);
			ZAssert(theEP->fStateEP == eStateEP_Connected);

			theEP->fReceivedSinceLastCredit = true;

			switch (theEP->fStateReceive)
				{
				case eStateReceive_Open:
					{
					theEP->fStateReceive = eStateReceive_CloseReceived;
					break;
					}
				case eStateReceive_ClosePending:
					{
					theEP->fStateReceive = eStateReceive_CloseReceived;
					break;
					}
				case eStateReceive_CloseSent:
					{
					theEP->fStateReceive = eStateReceive_Closed;
					break;
					}
				default:
					{
					ZUnimplemented();
					}
				}

			theEP->fCondition_Receive.Broadcast();
			if (this->pDetachIfUnused(theEP))
				delete theEP;

			locker.Release();
			fCommer->WakeWrite();
			break;
			}
		case eMsg_CloseYourSend:
			{
			locker.Release();

			const uint32 theID = spReceivePackedID(iStreamR);

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %p, CloseYourSend, theID: %d",
					this, theID);
				}

			locker.Acquire();

			Endpoint* theEP = this->pGetEndpoint(theID);
			ZAssert(theEP->fStateEP == eStateEP_Connected);

			theEP->fReceivedSinceLastCredit = true;

			switch (theEP->fStateSend)
				{
				case eStateSend_Open:
					{
					theEP->fStateSend = eStateSend_CloseReceived;
					theEP->fBuffer_Send.clear();
					break;
					}
				case eStateSend_ClosePending:
					{
					theEP->fStateSend = eStateSend_CloseReceived;
					break;
					}
				case eStateSend_CloseSent:
					{
					theEP->fStateSend = eStateSend_Closed;
					break;
					}
				default:
					{
					ZUnimplemented();
					}
				}

			theEP->fCondition_Send.Broadcast();
			if (this->pDetachIfUnused(theEP))
				delete theEP;

			locker.Release();
			fCommer->WakeWrite();
			break;
			}
		case eMsg_Data:
			{
			locker.Release();

			const uint32 theID = spReceivePackedID(iStreamR);
			const uint32 theCount = spReceivePackedInt(iStreamR);
			if (true || spIsLocal(theID))
				{
				if (ZLOG(s, eDebug + 2, "ZStreamMUX"))
					{
					s.Writef("pReadOne, this: %p, Data, theID: %d, size: %d",
						this, theID, theCount);
					}
				}

			vector<uint8> buffer(theCount, 0);
			if (theCount)
				iStreamR.Read(&buffer[0], theCount);

			locker.Acquire();

			Endpoint* theEP = this->pGetEndpoint(theID);
			ZAssert(theEP->fStateReceive != eStateReceive_CloseReceived);
			ZAssert(theEP->fStateReceive != eStateReceive_Closed);

			theEP->fReceivedSinceLastCredit = true;

			if (theEP->fStateReceive == eStateReceive_Open)
				{
				deque<uint8>& theBR = theEP->fBuffer_Receive;
				theBR.insert(theBR.end(), buffer.begin(), buffer.end());
				theEP->fCondition_Receive.Broadcast();
				}
			else
				{
				theEP->fCount_Received += theCount;
				locker.Release();
				fCommer->WakeWrite();
				}
			break;
			}
		case eMsg_AddCredit:
			{
			locker.Release();

			const uint32 theID = spReceivePackedID(iStreamR);
			const uint32 countAcked = spReceivePackedInt(iStreamR);

			if (true || spIsLocal(theID))
				{
				if (ZLOG(s, eDebug + 2, "ZStreamMUX"))
					{
					s.Writef("pReadOne, this: %p, AddCredit, theID: %d, countAcked: %d",
						this, theID, countAcked);
					}
				}

			locker.Acquire();

			Endpoint* theEP = this->pGetEndpoint(theID);
			ZAssert(theEP->fStateSend != eStateSend_CloseReceived);
			ZAssert(theEP->fStateSend != eStateSend_Closed);

			theEP->fReceivedSinceLastCredit = true;

			theEP->fCreditRemaining += countAcked;
			theEP->fCondition_Send.Broadcast();
			break;
			}
		case eMsg_Goodbye:
			{
			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				s.Writef("pReadOne, this: %p, goodbye", this);

			if (fLifecycle == eLifecycle_Running || fLifecycle == eLifecycle_StoppingRun)
				{
				fLifecycle = eLifecycle_ReadDead;
				locker.Release();
				fCommer->WakeWrite();
				}
			break;
			}
		}
	}

bool ZStreamMUX::pSendMessage
	(const ZStreamW& iStreamW, uint8 iMessage, uint32 iEPID)
	{
	fMutex.Release();
	try
		{
		iStreamW.WriteUInt8(iMessage);
		spSendPackedID(iStreamW, iEPID);
		fMutex.Acquire();
		return true;
		}
	catch (...)
		{
		fMutex.Acquire();
		fLifecycle = eLifecycle_StreamsDead;
		return false;
		}
	}

bool ZStreamMUX::pSendMessage
	(const ZStreamW& iStreamW, uint8 iMessage)
	{
	fMutex.Release();
	try
		{
		iStreamW.WriteUInt8(iMessage);
		fMutex.Acquire();
		return true;
		}
	catch (...)
		{
		fMutex.Acquire();
		fLifecycle = eLifecycle_StreamsDead;
		return false;
		}
	}

bool ZStreamMUX::pSendMessage_Param
	(const ZStreamW& iStreamW, uint8 iMessage, uint32 iEPID, uint32 iParam)
	{
	fMutex.Release();
	try
		{
		iStreamW.WriteUInt8(iMessage);
		spSendPackedID(iStreamW, iEPID);
		spSendPackedInt(iStreamW, iParam);
		fMutex.Acquire();
		return true;
		}
	catch (...)
		{
		fMutex.Acquire();
		fLifecycle = eLifecycle_StreamsDead;
		return false;
		}
	}

bool ZStreamMUX::pSendMessage_Open
	(const ZStreamW& iStreamW, Endpoint* iEP)
	{
	const uint32 theEPID = iEP->fEPID;
	ZAssert(spIsLocal(theEPID));
	const uint32 creditLimit = iEP->fReceiveBufferSize;
	const string& theName = *iEP->fOpenName;
	fMutex.Release();
	try
		{
		iStreamW.WriteUInt8(eMsg_Open);
		spSendPackedID(iStreamW, theEPID);
		spSendPackedInt(iStreamW, creditLimit);
		spSendPackedString(iStreamW, theName);
		fMutex.Acquire();
		return true;
		}
	catch (...)
		{
		fMutex.Acquire();
		fLifecycle = eLifecycle_StreamsDead;
		return false;
		}
	}

bool ZStreamMUX::pSendMessage_Data
	(const ZStreamW& iStreamW, uint32 iEPID, const void* iSource, size_t iCount)
	{
	fMutex.Release();
	try
		{
		iStreamW.WriteUInt8(eMsg_Data);
		spSendPackedID(iStreamW, iEPID);
		spSendPackedInt(iStreamW, iCount);
		iStreamW.Write(iSource, iCount);
		fMutex.Acquire();
		return true;
		}
	catch (...)
		{
		fMutex.Acquire();
		fLifecycle = eLifecycle_StreamsDead;
		return false;
		}
	}

void ZStreamMUX::pFlush(const ZStreamW& iStreamW)
	{
	fMutex.Release();
	try
		{
		iStreamW.Flush();
		fMutex.Acquire();
		}
	catch (...)
		{
		fMutex.Acquire();
		fLifecycle = eLifecycle_StreamsDead;
		}
	}

bool ZStreamMUX::pWriteOne(const ZStreamW& iStreamW, Endpoint* iEP)
	{
	const uint32 theEPID = iEP->fEPID;

	switch (iEP->fStateEP)
		{
		case eStateEP_Accepted:
			{
			const uint32 creditLimit = iEP->fReceiveBufferSize;
			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pWriteOne, this: %p, send OpenAck, theID: %d, credit: %d",
					this, theEPID, creditLimit);
				}

			iEP->fStateEP = eStateEP_Connected;
			iEP->fCondition_Receive.Broadcast();
			return this->pSendMessage_Param(iStreamW, eMsg_OpenAck, theEPID, creditLimit);
			}
		case eStateEP_ConnectPending:
			{
			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pWriteOne, this: %p, send Open, theID: %d",
					this, theEPID);
				}

			iEP->fStateEP = eStateEP_ConnectSent;
			return this->pSendMessage_Open(iStreamW, iEP);
			}
		case eStateEP_Connected:
			{
			if (iEP->fStateReceive == eStateReceive_CloseReceived)
				{
				if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
					{
					s.Writef
						("pWriteOne, this: %p, eStateReceive_CloseReceived, "
						"send CloseYourSend, theID: %d",
						this, theEPID);
					}
				iEP->fStateReceive = eStateReceive_Closed;
				if (this->pDetachIfUnused(iEP))
					delete iEP;
				return this->pSendMessage(iStreamW, eMsg_CloseYourSend, theEPID);
				}

			if (iEP->fStateReceive == eStateReceive_ClosePending)
				{
				if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
					{
					s.Writef
						("pWriteOne, this: %p, eStateReceive_ClosePending, "
						"send CloseYourSend, theID: %d",
						this, theEPID);
					}
				iEP->fStateReceive = eStateReceive_CloseSent;
				return this->pSendMessage(iStreamW, eMsg_CloseYourSend, theEPID);
				}

			if (iEP->fStateReceive == eStateReceive_Open)
				{
				// Read has not been closed.
				if (const size_t ackDelta = iEP->fCount_Received - iEP->fCount_Acked)
					{
					// Some sent data has been consumed.
					if (iEP->fReceivedSinceLastCredit)
						{
						iEP->fReceivedSinceLastCredit = false;
						iEP->fCount_Acked = iEP->fCount_Received;
						if (ZLOG(s, eDebug + 2, "ZStreamMUX"))
							{
							s.Writef
								("pWriteOne, this: %p, send AddCredit, theID: %d, credit: %zu",
								this, theEPID, ackDelta);
							}
						return this->pSendMessage_Param(iStreamW,
							eMsg_AddCredit, theEPID, ackDelta);
						}
					}
				}

			if (iEP->fStateSend == eStateSend_CloseReceived)
				{
				if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
					{
					s.Writef
						("pWriteOne, this: %p, eStateSend_CloseReceived, "
						"send CloseYourReceive, theID: %d",
						this, theEPID);
					}
				iEP->fStateSend = eStateSend_Closed;
				if (this->pDetachIfUnused(iEP))
					delete iEP;
				return this->pSendMessage(iStreamW, eMsg_CloseYourReceive, theEPID);
				}

			if (iEP->fStateSend != eStateSend_Closed)
				{
				if (size_t countToSend = iEP->fBuffer_Send.size())
					{
					// We have data in our buffer.
					countToSend = min(countToSend, fMaxFragmentSize);
					if (countToSend)
						{
						// And we can send it.
						deque<uint8>& theBS = iEP->fBuffer_Send;
						deque<uint8>::iterator begin = theBS.begin();
						deque<uint8>::iterator end = begin + countToSend;
						vector<uint8> buffer(begin, end);
						theBS.erase(begin, end);
						if (true || spIsLocal(iEP->fEPID))
							{
							if (ZLOG(s, eDebug + 2, "ZStreamMUX"))
								{
								s.Writef("pWriteOne, this: %p, send Data, theID: %d, size: %zu",
									this, theEPID, buffer.size());
								}
							}
						return this->pSendMessage_Data
							(iStreamW, theEPID, &buffer[0], buffer.size());
						}
					// Couldn't do any work right now.
					return false;
					}

				if (iEP->fStateSend == eStateSend_ClosePending)
					{
					// Buffer is empty and local close has occurred.
					if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
						{
						s.Writef
							("pWriteOne, this: %p, eStateSend_ClosePending with empty buffer, "
							"send CloseYourReceive, theID: %d",
							this, theEPID);
						}
					iEP->fStateSend = eStateSend_CloseSent;
					return this->pSendMessage(iStreamW, eMsg_CloseYourReceive, theEPID);
					}
				}
			break;
			}
		}
	return false;
	}

} // namespace ZooLib
