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
#include "zoolib/ZLog.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZUtil_STL.h"

using std::deque;
using std::map;
using std::min;
using std::pair;
using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

#define kDebug 1

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamMUX, definitions and helper functions

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
static uint32 sReceivePackedInt(const ZStreamR& iStreamR)
	{
	return iStreamR.ReadCount();
	}

static void sSendPackedInt(const ZStreamW& iStreamW, uint32 iInt)
	{
	iStreamW.WriteCount(iInt);
	}

static uint32 sReceivePackedID(const ZStreamR& iStreamR)
	{
	return sReceivePackedInt(iStreamR);
	}

static void sSendPackedID(const ZStreamW& iStreamW, uint32 iInt)
	{
	sSendPackedInt(iStreamW, iInt ^ 0x1);
	}

static string sReceivePackedString(const ZStreamR& iStreamR)
	{
	if (size_t theLength = sReceivePackedInt(iStreamR))
		return iStreamR.ReadString(theLength);

	return string();
	}

static void sSendPackedString(const ZStreamW& iStreamW, const string& iString)
	{
	size_t theLength = iString.length();
	sSendPackedInt(iStreamW, theLength);
	iStreamW.WriteString(iString);
	}

static bool sIsLocal(uint32 iID)
	{ return iID & 0x1; }

static bool sIsRemote(uint32 iID)
	{ return !sIsLocal(iID); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamMUX::Commer

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
	if (ZRef<ZStreamMUX> theStreamMUX = fStreamMUX.Use())
		return theStreamMUX->pRead(iStreamR);
	return false;
	}

bool ZStreamMUX::Commer::Write(const ZStreamW& iStreamW)
	{
	if (ZRef<ZStreamMUX> theStreamMUX = fStreamMUX.Use())
		return theStreamMUX->pWrite(iStreamW);
	return false;
	}

void ZStreamMUX::Commer::Started()
	{}

void ZStreamMUX::Commer::Finished()
	{
	if (ZRef<ZStreamMUX> theStreamMUX = fStreamMUX.Use())
		theStreamMUX->Finished();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamMUX::Endpoint

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

// From ZRefCountedWithFinalize via ZStreamerRWCon
	virtual void Finalize();

// From ZStreamerRCon via ZStreamerRWCon
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZStreamerRWCon
	virtual const ZStreamWCon& GetStreamWCon();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(int iMilliseconds);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(int iMilliseconds);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	ZWeakRef<ZStreamMUX> fMUX;

	ZCondition fCondition_Receive;
	ZCondition fCondition_Send;
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
	ZAssert(sIsRemote(fEPID));
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
	ZAssert(sIsLocal(fEPID));
	}

ZStreamMUX::Endpoint::~Endpoint()
	{
	ZAssert(fStateReceive == eStateReceive_CloseReceived
		|| fStateReceive == eStateReceive_Closed);
	ZAssert(fStateSend == eStateSend_Closed);

	if (ZLOG(s, eDebug + 1, "ZStreamMUX::Endpoint"))
		s.Writef("~Endpoint, this: %08X", this);
	}

void ZStreamMUX::Endpoint::Finalize()
	{
	bool hasMUX = false;
	bool needsDelete = false;

	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		{
		hasMUX = true;
		// Endpoint_Finalize calls our FinalizationComplete, we don't do it here.
		if (theMUX->Endpoint_Finalize(this))
			{
			needsDelete = true;
			}
		}

	if (hasMUX)
		{
		if (needsDelete)
			delete this;
		return;
		}

	if (this->GetRefCount() != 1)
		{
		this->FinalizationComplete();
		}
	else
		{
		this->FinalizationComplete();
		delete this;
		}
	}

const ZStreamRCon& ZStreamMUX::Endpoint::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& ZStreamMUX::Endpoint::GetStreamWCon()
	{ return *this; }

void ZStreamMUX::Endpoint::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		{
		theMUX->Endpoint_Read(this, iDest, iCount, oCountRead);
		}
	else
		{
		if (oCountRead)
			*oCountRead = 0;
		}
	}

size_t ZStreamMUX::Endpoint::Imp_CountReadable()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		return theMUX->Endpoint_CountReadable(this);
	else
		return 0;
	}

bool ZStreamMUX::Endpoint::Imp_WaitReadable(int iMilliseconds)
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		return theMUX->Endpoint_WaitReadable(this, iMilliseconds);
	return true;
	}

void ZStreamMUX::Endpoint::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		{
		theMUX->Endpoint_Write(this, iSource, iCount, oCountWritten);
		}
	else
		{
		if (oCountWritten)
			*oCountWritten = 0;
		}
	}

bool ZStreamMUX::Endpoint::Imp_ReceiveDisconnect(int iMilliseconds)
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		theMUX->Endpoint_ReceiveDisconnect(this, iMilliseconds);
	return true;
	}

void ZStreamMUX::Endpoint::Imp_SendDisconnect()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		theMUX->Endpoint_SendDisconnect(this);
	}

void ZStreamMUX::Endpoint::Imp_Abort()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		theMUX->Endpoint_Abort(this);
	}


// =================================================================================================
#pragma mark -
#pragma mark * ZStreamMUX::Listener

class ZStreamMUX::Listener : public ZStreamerRWConFactory
	{
public:
	friend class ZStreamMUX;

	Listener(ZRef<ZStreamMUX> iMUX, const string& iName, size_t iReceiveBufferSize);
	virtual ~Listener();

// From ZRefCountedWithFinalize via ZStreamerRWConFactory
	virtual void Finalize();

// From ZStreamerRWConFactory
	virtual ZRef<ZStreamerRWCon> MakeStreamerRWCon();
	virtual void Cancel();

private:
	ZWeakRef<ZStreamMUX> fMUX;
	ZCondition fCondition;
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
		s.Writef("~Listener, this: %08X", this);
	}

void ZStreamMUX::Listener::Finalize()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		{
		theMUX->Listener_Finalize(this);
		return;
		}

	this->FinalizationComplete();
	delete this;	
	}

ZRef<ZStreamerRWCon> ZStreamMUX::Listener::MakeStreamerRWCon()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		return theMUX->Listener_Listen(this);
	else
		return ZRef<ZStreamerRWCon>();
	}

void ZStreamMUX::Listener::Cancel()
	{
	if (ZRef<ZStreamMUX> theMUX = fMUX.Use())
		theMUX->Listener_Cancel(this);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamMUX::Options

ZStreamMUX::Options::Options()
:	fMaxFragmentSize(kDefaultMaxFragmentSize),
	fDefaultReceiveBufferSize(kDefaultReceiveBufferSize)
	{}

ZStreamMUX::Options::Options(size_t iMaxFragmentSize, size_t iDefaultReceiveBufferSize)
:	fMaxFragmentSize(iMaxFragmentSize),
	fDefaultReceiveBufferSize(iDefaultReceiveBufferSize)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamMUX

ZStreamMUX::ZStreamMUX()
:	fMutex("ZStreamMUX::fMutex"),
	fMaxFragmentSize(kDefaultMaxFragmentSize),
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
		s.Writef("~ZStreamMUX, this: %08X", this);

	ZAssert(fMap_IDToEndpoint.empty());
	ZAssert(fMap_NameToListener.empty());

	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		s.Writef("~ZStreamMUX, this: %08X, Exiting", this);
	}

void ZStreamMUX::Finalize()
	{
	ZMutexLocker locker(fMutex);
	if (this->GetRefCount() != 1)
		{
		this->FinalizationComplete();
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

	if (this->GetRefCount() != 1)
		{
		this->FinalizationComplete();
		return;
		}
	this->FinalizationComplete();
	locker.Release();
	delete this;
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
	ZMutexLocker locker(fMutex);

	if (fLifecycle == eLifecycle_Running)
		{
		fLifecycle = eLifecycle_StoppingRun;

		// Abort any endpoints that are connected.
		for (map<uint32, Endpoint*>::iterator i = fMap_IDToEndpoint.begin();
			i != fMap_IDToEndpoint.end(); ++i)
			{
			this->pAbort(i->second);
			}
		fCommer->Wake();
		}
	else
		{
		if (ZLOG(s, eDebug, "ZStreamMUX"))
			s << "Stop, already stopping/stopped";
		}
	}

ZRef<ZStreamerRWConFactory> ZStreamMUX::Listen(const std::string& iName)
	{ return this->Listen(iName, fDefaultReceiveBufferSize); }

ZRef<ZStreamerRWConFactory> ZStreamMUX::Listen(
	const std::string& iName, size_t iReceiveBufferSize)
	{
	ZMutexLocker locker(fMutex);
	map<std::string, Listener*>::iterator i = fMap_NameToListener.find(iName);
	if (i == fMap_NameToListener.end())
		{
		ZRef<Listener> theListener = new Listener(this, iName, iReceiveBufferSize);
		fMap_NameToListener.insert(pair<string, Listener*>(iName, theListener.GetObject()));
		return theListener;
		}
	return ZRef<ZStreamerRWConFactory>();
	}

ZRef<ZStreamerRWCon> ZStreamMUX::Connect(const std::string& iName)
	{ return this->Connect(iName, fDefaultReceiveBufferSize); }

ZRef<ZStreamerRWCon> ZStreamMUX::Connect(const std::string& iName, size_t iReceiveBufferSize)
	{
	ZMutexLocker locker(fMutex);

	if (fLifecycle == eLifecycle_Running)
		{
		const uint32 theEPID = this->pGetUnusedID();
		ZRef<Endpoint> theEndpoint = new Endpoint(this, theEPID, iReceiveBufferSize, &iName);

		if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
			{
			s.Writef("Connect, this: %08X, Connect starting , theID: %d, theState: %d",
				this, theEPID, theEndpoint->fStateEP);
			}

		fMap_IDToEndpoint.insert(pair<uint32, Endpoint*>(theEPID, theEndpoint.GetObject()));
		fCommer->Wake();

		while (theEndpoint->fStateEP != eStateEP_Connected)
			theEndpoint->fCondition_Receive.Wait(fMutex);

		if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
			{
			s.Writef("Connect, this: %08X, Connect done, theID: %d, theState: %d",
				this, theEPID, theEndpoint->fStateEP);
			}

		if (theEndpoint->fStateSend == eStateSend_Open)
			return theEndpoint;
		}

	return ZRef<ZStreamerRWCon>();
	}

uint32 ZStreamMUX::sGetConID(ZRef<ZStreamerRWCon> iCon)
	{
	if (ZRef<Endpoint> theEP = ZRefDynamicCast<Endpoint>(iCon))
		return theEP->fEPID;
	return 0;
	}

void ZStreamMUX::SetPingInterval(double iInterval)
	{
	ZMutexLocker locker(fMutex);
	fPingInterval = iInterval;
	fCommer->Wake();
	}

bool ZStreamMUX::pRead(const ZStreamR& iStreamR)
	{
	ZMutexLocker locker(fMutex);
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
			fCommer->Wake();
			}
		}

	return (fLifecycle == eLifecycle_Running
		|| fLifecycle == eLifecycle_StoppingRun);
	}

bool ZStreamMUX::pWrite(const ZStreamW& iStreamW)
	{
	ZMutexLocker locker(fMutex);

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
							fCommer->WakeAt(ZTime::sSystem() + fPingInterval);
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
				theEP->fListener->fEndpoints_Pending.Remove(theEP);
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
			s.Writef("Write, this: %08X, send goodbye", this);
		this->pSendMessage(iStreamW, eMsg_Goodbye);
		this->pFlush(iStreamW);
		}
	return false;
	}

bool ZStreamMUX::Endpoint_Finalize(Endpoint* iEP)
	{
	ZMutexLocker locker(fMutex);

	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		{
		s.Writef("Endpoint_Finalize, this: %08X, theID: %d",
			this, iEP->fEPID);
		}

	if (iEP->GetRefCount() != 1)
		{
		iEP->FinalizationComplete();
		return false;
		}

	iEP->FinalizationComplete();

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
	fCommer->Wake();

	return this->pDetachIfUnused(iEP);
	}

void ZStreamMUX::Endpoint_Read(Endpoint* iEP,
	void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(iDest);
	deque<uint8>& theBR = iEP->fBuffer_Receive;

	ZMutexLocker locker(fMutex);
	while (iCount)
		{
		if (const size_t countToRead = min(iCount, theBR.size()))
			{
			copy(theBR.begin(), theBR.begin() + countToRead, localDest);
			localDest += countToRead;
			iCount -= countToRead;
			theBR.erase(theBR.begin(), theBR.begin() + countToRead);
			iEP->fCount_Received += countToRead;
			fCommer->Wake();
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
		*oCountRead = localDest - static_cast<uint8*>(iDest);
	}

size_t ZStreamMUX::Endpoint_CountReadable(Endpoint* iEP)
	{
	ZMutexLocker locker(fMutex);
//	if (iEP->fStateEP != eStateEP_Connected
//		|| iEP->fStateReceive == eStateReceive_Closed)
//		{
//		return 0;
//		}

	return iEP->fBuffer_Receive.size();
	}

bool ZStreamMUX::Endpoint_WaitReadable(Endpoint* iEP, int iMilliseconds)
	{
	ZMutexLocker locker(fMutex);
	for (;;)
		{
		if (iEP->fStateEP != eStateEP_Connected
			|| iEP->fStateReceive == eStateReceive_Closed
			|| iEP->fStateReceive == eStateReceive_CloseReceived
			|| iEP->fBuffer_Receive.size())
			{
			return true;
			}

		if (iMilliseconds < 0)
			{
			iEP->fCondition_Receive.Wait(fMutex);
			}
		else
			{
			const ZTime start = ZTime::sSystem();
			iEP->fCondition_Receive.Wait(fMutex, iMilliseconds * 1000);
			iMilliseconds -= int(1000 * (ZTime::sSystem() - start));
			if (iMilliseconds <= 0)
				return false;
			}
		}
	}

void ZStreamMUX::Endpoint_Write(Endpoint* iEP,
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	ZMutexLocker locker(fMutex);
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
			fCommer->Wake();
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

bool ZStreamMUX::Endpoint_ReceiveDisconnect(Endpoint* iEP, int iMilliseconds)
	{
	ZMutexLocker locker(fMutex);

	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		{
		s.Writef("Endpoint_ReceiveDisconnect, this: %08X, theID: %d",
			this, iEP->fEPID);
		}

	for (;;)
		{
		if (size_t count = iEP->fBuffer_Receive.size())
			{
			iEP->fCount_Received += count;
			iEP->fBuffer_Receive.clear();
			fCommer->Wake();
			}
		else if (iEP->fStateEP != eStateEP_Connected
			|| iEP->fStateReceive == eStateReceive_CloseReceived
			|| iEP->fStateReceive == eStateReceive_Closed)
			{
			return true;
			}

		if (iMilliseconds < 0)
			{
			iEP->fCondition_Receive.Wait(fMutex);
			}
		else
			{
			const ZTime start = ZTime::sSystem();
			iEP->fCondition_Receive.Wait(fMutex, iMilliseconds * 1000);
			iMilliseconds -= int(1000 * (ZTime::sSystem() - start));
			if (iMilliseconds <= 0)
				return false;
			}
		}
	}

void ZStreamMUX::Endpoint_SendDisconnect(Endpoint* iEP)
	{
	ZMutexLocker locker(fMutex);

	if (iEP->fStateEP == eStateEP_Connected && iEP->fStateSend == eStateSend_Open)
		{
		if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
			{
			s.Writef("Endpoint_SendDisconnect, this: %08X, theID: %d",
				this, iEP->fEPID);
			}
		iEP->fStateSend = eStateSend_ClosePending;
		fCommer->Wake();
		}
	else
		{
		if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
			{
			s.Writef("Endpoint_SendDisconnect, no effect, this: %08X, theID: %d",
				this, iEP->fEPID);
			}
		}
	}

void ZStreamMUX::Endpoint_Abort(Endpoint* iEP)
	{
	ZMutexLocker locker(fMutex);

	this->pAbort(iEP);

	fCommer->Wake();
	}

void ZStreamMUX::Listener_Finalize(Listener* iListener)
	{
	ZMutexLocker locker(fMutex);
	if (iListener->GetRefCount() != 1)
		{
		iListener->FinalizationComplete();
		return;
		}

	map<std::string, Listener*>::iterator i = fMap_NameToListener.find(iListener->fName);
	ZAssert(i != fMap_NameToListener.end());
	fMap_NameToListener.erase(i);

	for (DListIteratorEraseAll<Endpoint, DLink_Endpoint_Pending>
		i = iListener->fEndpoints_Pending; i; i.Advance())
		{
		Endpoint* theEndpoint = i.Current();
		this->Endpoint_Abort(theEndpoint);
		theEndpoint->fListener = nullptr;
		}
	}

ZRef<ZStreamerRWCon> ZStreamMUX::Listener_Listen(Listener* iListener)
	{
	ZMutexLocker locker(fMutex);

	if (!iListener->fCancelled && iListener->fEndpoints_Pending.Empty())
		iListener->fCondition.Wait(fMutex);

	if (iListener->fEndpoints_Pending.Empty())
		return ZRef<ZStreamerRWCon>();

	Endpoint* theEndpoint = iListener->fEndpoints_Pending.PopFront<Endpoint>();
	theEndpoint->fListener = nullptr;
	return theEndpoint;
	}

void ZStreamMUX::Listener_Cancel(Listener* iListener)
	{
	ZMutexLocker locker(fMutex);

	// Wake everything, and let any call to Listener_Listen complete.
	iListener->fCondition.Broadcast();
	}

uint32 ZStreamMUX::pGetUnusedID()
	{
	ZAssert(fMutex.IsLocked());

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
	ZAssert(fMutex.IsLocked());
	map<uint32, Endpoint*>::iterator i = fMap_IDToEndpoint.find(iGlobalID);
	if (i != fMap_IDToEndpoint.end())
		return i->second;
	return nullptr;
	}

ZStreamMUX::Endpoint* ZStreamMUX::pGetEndpoint(uint32 iGlobalID)
	{
	ZAssert(fMutex.IsLocked());
	map<uint32, Endpoint*>::iterator i = fMap_IDToEndpoint.find(iGlobalID);
	ZAssert(i != fMap_IDToEndpoint.end());
	return i->second;
	}

bool ZStreamMUX::pDetachIfUnused(Endpoint* iEP)
	{
	ZAssert(fMutex.IsLocked());

	if (iEP->fStateSend != eStateSend_Closed)
		return false;

	if (iEP->fStateReceive != eStateReceive_Closed)
		return false;

	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		s.Writef("pDetachIfUnused, this: %08X, detaching theID: %d", this, iEP->fEPID);

	// It's dead.
	if (iEP->fEPID)
		{
		ZUtil_STL::sEraseMustContain(kDebug, fMap_IDToEndpoint, iEP->fEPID);
		iEP->fEPID = 0;
		fCommer->Wake();
		}

//	return true;
	return 0 == iEP->GetRefCount();
	}

void ZStreamMUX::pAbort(Endpoint* iEP)
	{
	ZAssert(fMutex.IsLocked());

	if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
		{
		s.Writef("pAbort, this: %08X, theID: %d",
			this, iEP->fEPID);
		}

	if (iEP->fStateSend == eStateSend_Open)
		{
		iEP->fStateSend = eStateSend_ClosePending;
		iEP->fBuffer_Send.clear();

		if (iEP->fListener)
			{
			iEP->fListener->fEndpoints_Pending.Remove(iEP);
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
	fCommer->Wake();
	}

void ZStreamMUX::pReadOne(const ZStreamR& iStreamR)
	{
	ZAssert(!fMutex.IsLocked());

	const uint8 theMessage = iStreamR.ReadUInt8();

	ZMutexLocker locker(fMutex);

	fTime_LastRead = ZTime::sSystem();

	switch (theMessage)
		{
		case eMsg_Ping:
			{
			fPingReceived = true;
			locker.Release();
			fCommer->Wake();
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
			const uint32 theID = sReceivePackedID(iStreamR);
			ZAssert(sIsRemote(theID));
			const uint32 creditLimit = sReceivePackedInt(iStreamR);
			const string listenerName = sReceivePackedString(iStreamR);

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %08X, Open, theID: %d, creditLimit: %d, name: %s",
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
			fCommer->Wake();
			break;			
			}
		case eMsg_OpenAck:
			{
			locker.Release();

			const uint32 theID = sReceivePackedID(iStreamR);
			ZAssert(sIsLocal(theID));			
			const uint32 creditLimit = sReceivePackedInt(iStreamR);

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %08X, OpenAck, theID: %d, creditLimit: %d",
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
			const uint32 theID = sReceivePackedID(iStreamR);
			ZAssert(sIsLocal(theID));			

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %08X, OpenNack, theID: %d",
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

			const uint32 theID = sReceivePackedID(iStreamR);

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %08X, CloseYourReceive, theID: %d",
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
			fCommer->Wake();
			break;
			}
		case eMsg_CloseYourSend:
			{
			locker.Release();

			const uint32 theID = sReceivePackedID(iStreamR);

			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pReadOne, this: %08X, CloseYourSend, theID: %d",
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
			fCommer->Wake();
			break;
			}
		case eMsg_Data:
			{
			locker.Release();

			const uint32 theID = sReceivePackedID(iStreamR);
			const uint32 theCount = sReceivePackedInt(iStreamR);
			if (true || sIsLocal(theID))
				{
				if (ZLOG(s, eDebug + 2, "ZStreamMUX"))
					{
					s.Writef("pReadOne, this: %08X, Data, theID: %d, size: %d",
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
				fCommer->Wake();
				}
			break;
			}
		case eMsg_AddCredit:
			{
			locker.Release();

			const uint32 theID = sReceivePackedID(iStreamR);
			const uint32 countAcked = sReceivePackedInt(iStreamR);

			if (true || sIsLocal(theID))
				{
				if (ZLOG(s, eDebug + 2, "ZStreamMUX"))
					{
					s.Writef("pReadOne, this: %08X, AddCredit, theID: %d, countAcked: %d",
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
				s.Writef("pReadOne, this: %08X, goodbye", this);

			if (fLifecycle == eLifecycle_Running || fLifecycle == eLifecycle_StoppingRun)
				{
				fLifecycle = eLifecycle_ReadDead;
				locker.Release();
				fCommer->Wake();
				}
			break;
			}
		}
	}

bool ZStreamMUX::pSendMessage(
	const ZStreamW& iStreamW, uint8 iMessage, uint32 iEPID)
	{
	fMutex.Release();
	try
		{
		iStreamW.WriteUInt8(iMessage);
		sSendPackedID(iStreamW, iEPID);
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

bool ZStreamMUX::pSendMessage(
	const ZStreamW& iStreamW, uint8 iMessage)
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

bool ZStreamMUX::pSendMessage_Param(
	const ZStreamW& iStreamW, uint8 iMessage, uint32 iEPID, uint32 iParam)
	{
	fMutex.Release();
	try
		{
		iStreamW.WriteUInt8(iMessage);
		sSendPackedID(iStreamW, iEPID);
		sSendPackedInt(iStreamW, iParam);
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

bool ZStreamMUX::pSendMessage_Open(
	const ZStreamW& iStreamW, Endpoint* iEP)
	{
	const uint32 theEPID = iEP->fEPID;
	ZAssert(sIsLocal(theEPID));
	const uint32 creditLimit = iEP->fReceiveBufferSize;
	const string& theName = *iEP->fOpenName;
	fMutex.Release();
	try
		{
		iStreamW.WriteUInt8(eMsg_Open);
		sSendPackedID(iStreamW, theEPID);
		sSendPackedInt(iStreamW, creditLimit);
		sSendPackedString(iStreamW, theName);
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

bool ZStreamMUX::pSendMessage_Data(
	const ZStreamW& iStreamW, uint32 iEPID, const void* iSource, size_t iCount)
	{
	fMutex.Release();
	try
		{
		iStreamW.WriteUInt8(eMsg_Data);
		sSendPackedID(iStreamW, iEPID);
		sSendPackedInt(iStreamW, iCount);
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
	ZAssert(fMutex.IsLocked());

	const uint32 theEPID = iEP->fEPID;

	switch (iEP->fStateEP)
		{
		case eStateEP_Accepted:
			{
			const uint32 creditLimit = iEP->fReceiveBufferSize;
			if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
				{
				s.Writef("pWriteOne, this: %08X, send OpenAck, theID: %d, credit: %d",
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
				s.Writef("pWriteOne, this: %08X, send Open, theID: %d",
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
					s.Writef(
						"pWriteOne, this: %08X, eStateReceive_CloseReceived, "
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
					s.Writef(
						"pWriteOne, this: %08X, eStateReceive_ClosePending, "
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
							s.Writef(
								"pWriteOne, this: %08X, send AddCredit, theID: %d, credit: %d",
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
					s.Writef(
						"pWriteOne, this: %08X, eStateSend_CloseReceived, "
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
						if (true || sIsLocal(iEP->fEPID))
							{
							if (ZLOG(s, eDebug + 2, "ZStreamMUX"))
								{
								s.Writef("pWriteOne, this: %08X, send Data, theID: %d, size: %d",
									this, theEPID, buffer.size());
								}
							}
						return this->pSendMessage_Data(
							iStreamW, theEPID, &buffer[0], buffer.size());
						}
					// Couldn't do any work right now.
					return false;
					}

				if (iEP->fStateSend == eStateSend_ClosePending)
					{
					// Buffer is empty and local close has occurred.
					if (ZLOG(s, eDebug + 1, "ZStreamMUX"))
						{
						s.Writef(
							"pWriteOne, this: %08X, eStateSend_ClosePending with empty buffer, "
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

NAMESPACE_ZOOLIB_END
