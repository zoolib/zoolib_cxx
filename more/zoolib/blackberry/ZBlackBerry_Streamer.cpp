/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/blackberry/ZBlackBerry_Streamer.h"

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZCommer.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h" // For ZMemCopy
#include "zoolib/ZStream_Limited.h"
#include "zoolib/ZStream_SHA1.h"
#include "zoolib/ZUtil_STL_vector.h"

#include <deque>

using std::deque;
using std::min;
using std::string;
using std::vector;

namespace ZooLib {
namespace ZBlackBerry {

// =================================================================================================
// MARK: - Protocol constants

enum EMsg
	{
// Messages we send to the BB
	eMsg_GetAttribute = 0x05, //
	eMsg_SelectMode = 0x07,
	eMsg_ChannelOpen = 0x0A,
	eMsg_PasswordResponse = 0x0F,

// Messages received from the BB
	eMsg_GetAttribute_Ack = 0x06, //
	eMsg_SelectMode_Ack = 0x08,
	eMsg_SelectMode_Nack = 0x09,
	eMsg_PasswordChallenge = 0x0E,
	eMsg_ChannelOpen_Ack = 0x10,
	eMsg_PasswordFailed = 0x11,
	eMsg_SequenceHandshake = 0x13,

// Message that go both ways
	eMsg_ChannelClose = 0x0B,
	eMsg_ChannelClose_Ack = 0x0C,
	eMsg_ChannelClose_Nack = 0x0D // ??
	};

enum EState
	{
	eState_Dead,

	eState_LookupNeeded,
	eState_LookupSent,
	eState_ConnectNeeded,
	eState_ConnectSent,
	eState_ChallengeRcvd,

	eState_Connected,

	eState_CloseNeeded,
	eState_CloseSent, // Need to receive closeAck
	eState_CloseRcvd, // Need to send closeAck
	eState_CloseSentAndRcvd // Need to send and receive closeAck
	};

/* State transitions for channels.
Current         Function/                      New
State           MsgRcvd      MsgSent           State
----------------------------------------------------
Dead            FN:OpenRW                      LookupNeeded

LookupNeeded                SelectMode         LookupSent

LookupSent      SelectMode_Ack                 ConnectNeeded
LookupSent      SelectMode_Nack                Dead

ConnectNeeded               ChannelOpen        ConnectSent

ConnectSent     PasswordChallenge              ChallengeRcvd
ConnectSent     PasswordFailed                 CloseNeeded
ConnectSent     ChannelOpen_Ack                Connected

ChallengeRcvd               PasswordResponse   ConnectSent

Connected       FN:Write                       Connected
                                               (appends data to buffer)

Connected                   DATA               Connected
(when buffer is not empty)                     (removes data from buffer, sets fWaitingForSequence)

Connected       SequenceHandshake              Connected
                                               (clears fWaitingForSequence)

ConnectedData   FN:Close                       CloseNeeded
Connected       FN:Close                       CloseNeeded
Connected       ChannelClose                   CloseRcvd

CloseNeeded                 ChannelClose       CloseSent
CloseNeeded     ChannelClose                   CloseRcvd

CloseSent       ChannelClose_Ack               Dead
CloseSent       ChannelClose                   CloseSentAndRcvd

CloseRcvd                   ChannelClose_Ack   Dead

CloseSentAndRcvd            ChannelClose_Ack   Dead

Note that ConnectedData is a virtual state -- it's just
Connected, but with a non-empty send buffer.
*/

// =================================================================================================
// MARK: - StreamW_Chunked

class StreamW_Chunked : public ZStreamW
	{
public:
	StreamW_Chunked();
	~StreamW_Chunked();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// Our protocol
	bool Send(uint16 iChannelID, const ZStreamW& iStreamW);

private:
	vector<uint8> fBuffer;
	};

StreamW_Chunked::StreamW_Chunked()
	{
	fBuffer.reserve(1024);
	fBuffer.resize(4);
	}

StreamW_Chunked::~StreamW_Chunked()
	{}

void StreamW_Chunked::Imp_Write(
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = static_cast<const uint8*>(iSource);
	fBuffer.insert(fBuffer.end(), localSource, localSource + iCount);
	if (oCountWritten)
		*oCountWritten = iCount;
	}

bool StreamW_Chunked::Send(uint16 iChannelID, const ZStreamW& iStreamW)
	{
	const size_t theSize = fBuffer.size();
	ZByteSwap_WriteLittle16(&fBuffer[0], iChannelID);
	ZByteSwap_WriteLittle16(&fBuffer[2], theSize);

	size_t countCopied;

	iStreamW.Write(&fBuffer[0], theSize, &countCopied);

	fBuffer.resize(4);
	return countCopied == theSize;
	}

// =================================================================================================
// MARK: - ZBlackBerry::Commer_Streamer declaration

class Channel_Streamer;

class Commer_Streamer
:	public ZCommer
	{
	friend class Channel_Streamer;

public:
	Commer_Streamer(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~Commer_Streamer();

// From ZStreamReader via ZCommer
	virtual bool Read(const ZStreamR& iStreamR);

// From ZStreamWriter via ZCommer
	virtual bool Write(const ZStreamW& iStreamW);

// Called by Device_Streamer
	void Stop();

	ZRef<Channel> Open(bool iPreserveBoundaries,
		const std::string& iName, const PasswordHash* iPasswordHash, Device::Error* oError);

	Data GetAttribute(uint16 iObject, uint16 iAttribute);

	uint32 GetPIN();

private:
	bool Channel_Finalize(Channel_Streamer* iChannel);

	void Channel_Read(Channel_Streamer* iChannel, void* oDest, size_t iCount, size_t* oCountRead);

	size_t Channel_CountReadable(Channel_Streamer* iChannel);

	bool Channel_WaitReadable(Channel_Streamer* iChannel, double iTimeout);

	void Channel_Write(
		Channel_Streamer* iChannel, const void* iSource, size_t iCount, size_t* oCountWritten);

	void Channel_SendDisconnect(Channel_Streamer* iChannel);

	bool Channel_ReceiveDisconnect(Channel_Streamer* iChannel, double iTimeout);

	void Channel_Abort(Channel_Streamer* iChannel);

	ZRef<Channel_Streamer> pFindChannel(uint16 iChannelID);
	ZRef<Channel_Streamer> pFindChannel(const std::string& iName);
	bool pDetachIfUnused(Channel_Streamer* iChannel);

	void pReadOne(uint16 iChannelID, uint16 iPayloadSize, const ZStreamR& iStreamR);

	bool pWriteOne(const ZStreamW& iStreamW, Channel_Streamer* iChannel);

	bool pSend(StreamW_Chunked& iSC, uint16 iChannelID, const ZStreamW& iStreamW);
	bool pSendFunky(uint16 iLength, const ZStreamW& iStreamW);
	void pFlush(const ZStreamW& iStreamW);

	ZMtxR fMutex;
	ZCnd fCondition;

	struct GetAttribute_t
		{
		uint16 fObject;
		uint16 fAttribute;
		bool fFinished;
		Data fResult;
		};
	bool fGetAttributeSent;
	GetAttribute_t* fGetAttribute;

	enum ELifecycle
		{
		eLifecycle_Running,
		eLifecycle_StreamsDead,
		eLifecycle_StoppingRun,
		eLifecycle_StoppingRead
		};

	ELifecycle fLifecycle;

	std::vector<Channel_Streamer*> fChannels;
	};

// =================================================================================================
// MARK: - Channel_Streamer

class Channel_Streamer
:	public ZBlackBerry::Channel,
	private ZStreamRCon,
	private ZStreamWCon
	{
	friend class Commer_Streamer;
	Channel_Streamer(ZRef<Commer_Streamer> iCommer_Streamer,
		bool iPreserveBoundaries, const string& iName, const PasswordHash* iPasswordHash);

public:
	virtual ~Channel_Streamer();

// From ZRefCountedWithFinalize via ZStreamerRW
	virtual void Finalize();

// From ZBlackBerry::Channel
	virtual size_t GetIdealSize_Read();
	virtual size_t GetIdealSize_Write();

// From ZStreamerR via ZStreamerRWCon
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerW via ZStreamerRWCon
	virtual const ZStreamWCon& GetStreamWCon();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(double iTimeout);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	ZWeakRef<Commer_Streamer> fCommer_Streamer;
	EState fState;
	Device::Error fError;
	bool fPreserveBoundaries;
	string fName;
	bool fHasPasswordHash;
	PasswordHash fPasswordHash;
	uint16 fChannelID;
	uint32 fChallenge;
	ZCnd fCondition_Receive;
	ZCnd fCondition_Send;
	uint32 fNextSequence;
	bool fWaitingForSequence;

	size_t fReceive_ChunkSize;
	deque<uint8> fReceive_Buffer;

	size_t fSend_ChunkSize;
	const uint8* fSend_Data;
	size_t fSend_Size;
	};

Channel_Streamer::Channel_Streamer(ZRef<Commer_Streamer> iCommer_Streamer,
	bool iPreserveBoundaries, const string& iName, const PasswordHash* iPasswordHash)
:	fCommer_Streamer(iCommer_Streamer),
	fState(eState_Dead),
	fError(Device::error_None),
	fPreserveBoundaries(iPreserveBoundaries),
	fName(iName),
	fHasPasswordHash(0 != iPasswordHash),
	fChannelID(0),
	fNextSequence(0),
	fWaitingForSequence(true),
	fReceive_ChunkSize(0),
	fSend_ChunkSize(0),
	fSend_Data(nullptr),
	fSend_Size(0)
	{
	if (iPasswordHash)
		fPasswordHash = *iPasswordHash;
	}

Channel_Streamer::~Channel_Streamer()
	{
	ZLOGFUNCTION(eDebug + 1);
	}

void Channel_Streamer::Finalize()
	{
	bool hasCommer_Streamer = false;
	bool needsDelete = false;

	if (ZRef<Commer_Streamer> theCommer_Streamer = fCommer_Streamer)
		{
		hasCommer_Streamer = true;
		// Channel_Finalize calls our FinalizationComplete, we don't do it here.
		if (theCommer_Streamer->Channel_Finalize(this))
			needsDelete = true;
		}

	// Do not do else if here -- keeps theCommer_Streamer in scope, and keeps use count non-zero.
	if (hasCommer_Streamer)
		{
		if (needsDelete)
			delete this;
		return;
		}

	if (this->FinishFinalize())
		delete this;
	}

size_t Channel_Streamer::GetIdealSize_Read()
	{ return fReceive_ChunkSize; }

size_t Channel_Streamer::GetIdealSize_Write()
	{ return fSend_ChunkSize; }

const ZStreamRCon& Channel_Streamer::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& Channel_Streamer::GetStreamWCon()
	{ return *this; }

void Channel_Streamer::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (ZRef<Commer_Streamer> theCommer_Streamer = fCommer_Streamer)
		{
		theCommer_Streamer->Channel_Read(this, oDest, iCount, oCountRead);
		return;
		}

	if (oCountRead)
		*oCountRead = 0;
	}

size_t Channel_Streamer::Imp_CountReadable()
	{
	if (ZRef<Commer_Streamer> theCommer_Streamer = fCommer_Streamer)
		return theCommer_Streamer->Channel_CountReadable(this);

	return 0;
	}

bool Channel_Streamer::Imp_WaitReadable(double iTimeout)
	{
	if (ZRef<Commer_Streamer> theCommer_Streamer = fCommer_Streamer)
		return theCommer_Streamer->Channel_WaitReadable(this, iTimeout);

	return true;
	}

bool Channel_Streamer::Imp_ReceiveDisconnect(double iTimeout)
	{
	if (ZRef<Commer_Streamer> theCommer_Streamer = fCommer_Streamer)
		return theCommer_Streamer->Channel_ReceiveDisconnect(this, iTimeout);

	return true;
	}

void Channel_Streamer::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (ZRef<Commer_Streamer> theCommer_Streamer = fCommer_Streamer)
		{
		theCommer_Streamer->Channel_Write(this, iSource, iCount, oCountWritten);
		return;
		}

	if (oCountWritten)
		*oCountWritten = 0;
	}

void Channel_Streamer::Imp_SendDisconnect()
	{
	if (ZRef<Commer_Streamer> theCommer_Streamer = fCommer_Streamer)
		theCommer_Streamer->Channel_SendDisconnect(this);
	}

void Channel_Streamer::Imp_Abort()
	{
	if (ZRef<Commer_Streamer> theCommer_Streamer = fCommer_Streamer)
		theCommer_Streamer->Channel_Abort(this);
	}

// =================================================================================================
// MARK: - Commer_Streamer definition

Commer_Streamer::Commer_Streamer(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	ZCommer(iStreamerR, iStreamerW),
	fGetAttributeSent(false),
	fGetAttribute(nullptr),
	fLifecycle(eLifecycle_Running)
	{}

Commer_Streamer::~Commer_Streamer()
	{
	ZLOGFUNCTION(eDebug + 2);
	}

bool Commer_Streamer::Read(const ZStreamR& iStreamR)
	{
	ZGuardRMtxR locker(fMutex);
	if (fLifecycle == eLifecycle_Running
		|| fLifecycle == eLifecycle_StoppingRun)
		{
		locker.Release();
		try
			{
			const uint16 theChannelID = iStreamR.ReadUInt16LE();
			const uint16 thePayloadSize = iStreamR.ReadUInt16LE() - 4;

			ZAssert(thePayloadSize < 0xFFFC);

			// Read no more than thePayloadSize bytes from iStreamR.
			ZStreamR_Limited theSRL(thePayloadSize, iStreamR);

			this->pReadOne(theChannelID, thePayloadSize, theSRL);

			// Suck up and ignore any payload not handled by pReadOne
			theSRL.SkipAll();
			locker.Acquire();
			}
		catch (...)
			{
			locker.Acquire();
			fLifecycle = eLifecycle_StreamsDead;
			this->WakeWrite();
			}
		}

	return (fLifecycle == eLifecycle_Running
		|| fLifecycle == eLifecycle_StoppingRun);
	}

bool Commer_Streamer::Write(const ZStreamW& iStreamW)
	{
	ZGuardRMtxR locker(fMutex);

	size_t nextIndex = 0;
	bool aChannelWroteSinceReset = false;
	bool writtenSinceFlush = false;
	while (fLifecycle == eLifecycle_Running
		|| fLifecycle == eLifecycle_StoppingRun)
		{
		if (fGetAttribute && !fGetAttributeSent)
			{
			fGetAttributeSent = true;
			StreamW_Chunked w;
			w.WriteUInt8(eMsg_GetAttribute);
			w.WriteUInt16LE(0xFF); // Dummy channel ID
			w.WriteUInt8(0x02);
			w.WriteUInt16LE(fGetAttribute->fObject);
			w.WriteUInt16LE(fGetAttribute->fAttribute);

			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s << "Write, GetAttribute";
				}

			this->pSend(w, 0, iStreamW);
			writtenSinceFlush = false;
			this->pFlush(iStreamW);
			continue;
			}

		bool aChannelWrote = false;
		while (!aChannelWrote)
			{
			const size_t theIndex = nextIndex;
			++nextIndex;
			if (theIndex >= fChannels.size())
				break;

			Channel_Streamer* theChannel = fChannels.at(theIndex);
			if (this->pWriteOne(iStreamW, theChannel))
				{
				writtenSinceFlush = true;
				aChannelWrote = true;
				aChannelWroteSinceReset = true;
				}
			}

		if (!aChannelWrote)
			{
			if (!aChannelWroteSinceReset)
				{
				if (writtenSinceFlush)
					{
					writtenSinceFlush = false;
					this->pFlush(iStreamW);
					}
				else
					{
					if (fLifecycle == eLifecycle_StoppingRun && fChannels.empty())
						{
						break;
						}
					else
						{
						return true;
						}
					}
				}
			nextIndex = 0;
			aChannelWroteSinceReset = false;
			}
		}

	if (fLifecycle == eLifecycle_StreamsDead)
		{
		vector<ZRef<Channel_Streamer> > localChannels;
		for (vector<Channel_Streamer*>::iterator i = fChannels.begin();
			i != fChannels.end(); ++i)
			{
			Channel_Streamer* theChannel = *i;
			theChannel->fState = eState_Dead;
			theChannel->fWaitingForSequence = false;
			theChannel->fReceive_Buffer.clear();
			// Don't zero fSend_Data -- a pending write will need its value.
			// theChannel->fSend_Data = nullptr;
			theChannel->fSend_Size = 0;
			theChannel->fCondition_Receive.Broadcast();
			theChannel->fCondition_Send.Broadcast();
			localChannels.push_back(theChannel);
			}
		fChannels.clear();

		locker.Release();

		// Clear their references to us.
		for (vector<ZRef<Channel_Streamer> >::iterator i = localChannels.begin();
			i != localChannels.end(); ++i)
			{ (*i)->fCommer_Streamer.Clear(); }

		locker.Acquire();
		}
	else
		{
		ZAssert(fChannels.empty());

		fLifecycle = eLifecycle_StoppingRead;

		if (true)//!fReaderFinished)
			{
			// Send a bogus lookup, to cause data to be returned and the reader to wake.
			StreamW_Chunked w;
			w.WriteUInt8(eMsg_SelectMode);
			w.WriteUInt16LE(0xFF); // Dummy channel ID
			w.WriteUInt8(0x00);
			w.WriteString("ZZWakeReader");
			for (size_t x = 12; x < 16; ++x)
				w.WriteUInt8(0x00); // Padding
			this->pSend(w, 0, iStreamW);
			this->pFlush(iStreamW);
			}
		}

	// Finally, Kill any outstanding GetAttribute call
	if (fGetAttribute)
		{
		fGetAttribute->fFinished = true;
		fGetAttribute->fResult = Data();
		fGetAttribute = nullptr;
		fGetAttributeSent = false;
		fCondition.Broadcast();
		}

	return false;
	}

void Commer_Streamer::Stop()
	{
	ZLOGFUNCTION(eDebug);

	ZGuardRMtxR locker(fMutex);

	if (fLifecycle == eLifecycle_Running)
		{
		if (ZLOG(s, eDebug, "ZBlackBerry::Commer_Streamer"))
			s << "Stop, go from Running to StoppingRun";

		fLifecycle = eLifecycle_StoppingRun;

		// Abort any channel that's connected.
		for (vector<Channel_Streamer*>::iterator i = fChannels.begin();
			i != fChannels.end(); ++i)
			{
			Channel_Streamer* theChannel = *i;
			if (theChannel->fState == eState_Connected)
				{
				theChannel->fState = eState_CloseNeeded;
				theChannel->fCondition_Receive.Broadcast();
				theChannel->fCondition_Send.Broadcast();
				}
			}
		this->WakeWrite();
		}
	else
		{
		if (ZLOG(s, eDebug, "ZBlackBerry::Commer_Streamer"))
			s << "Stop, already stopping/stopped";
		}
	}

ZRef<Channel> Commer_Streamer::Open(bool iPreserveBoundaries,
	const string& iName, const PasswordHash* iPasswordHash, Device::Error* oError)
	{
	ZGuardRMtxR locker(fMutex);

	if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
		s << "Open name: " << iName;

	if (fLifecycle == eLifecycle_Running)
		{
		ZRef<Channel_Streamer> theChannel =
			new Channel_Streamer(this, iPreserveBoundaries, iName, iPasswordHash);
		fChannels.push_back(theChannel.Get());
		theChannel->fState = eState_LookupNeeded;

		this->WakeWrite();

		while (theChannel->fState != eState_Dead
			&& theChannel->fState != eState_Connected)
			{
			theChannel->fCondition_Receive.Wait(fMutex);
			}

		if (theChannel->fState == eState_Connected)
			{
			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s << "Open name: " << iName << ", succeeded";
				}

			if (oError)
				*oError = Device::error_None;
			return theChannel;
			}

		if (oError)
			*oError = theChannel->fError;
		}
	else
		{
		if (oError)
			*oError = Device::error_DeviceClosed;
		}

	if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
		s << "Open name: " << iName << ", failed";

	return null;
	}

Data Commer_Streamer::GetAttribute(uint16 iObject, uint16 iAttribute)
	{
	ZGuardRMtxR locker(fMutex);

	if (fLifecycle != eLifecycle_Running)
		return Data();

	while (fGetAttribute)
		fCondition.Wait(fMutex);

	GetAttribute_t theGA;
	theGA.fObject = iObject;
	theGA.fAttribute = iAttribute;
	theGA.fFinished = false;

	fGetAttribute = &theGA;

	this->WakeWrite();

	while (!theGA.fFinished)
		fCondition.Wait(fMutex);

	return theGA.fResult;
	}

uint32 Commer_Streamer::GetPIN()
	{
	Data theMB_PIN = this->GetAttribute(8, 4);
	if (theMB_PIN.GetSize() >= 15)
		return ZByteSwap_ReadLittle32(static_cast<const char*>(theMB_PIN.GetPtr()) + 11);
	return 0;
	}

bool Commer_Streamer::Channel_Finalize(Channel_Streamer* iChannel)
	{
	ZGuardRMtxR locker(fMutex);

	if (not iChannel->FinishFinalize())
		return false;

	switch (iChannel->fState)
		{
		case eState_Connected:
			{
			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s.Writef("Channel_Finalize, ID: %d, Connected, Switch to CloseNeeded",
					iChannel->fChannelID);
				}

			iChannel->fState = eState_CloseNeeded;
			this->WakeWrite();
			break;
			}
		case eState_Dead:
			{
			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s.Writef("Channel_Finalize, ID: %d, Dead", iChannel->fChannelID);
				}
			break;
			}
		case eState_CloseSent:
			{
			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s.Writef("Channel_Finalize, ID: %d, CloseSent", iChannel->fChannelID);
				}
			break;
			}
		case eState_CloseRcvd:
			{
			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s.Writef("Channel_Finalize, ID: %d, CloseRcvd", iChannel->fChannelID);
				}
			break;
			}
		case eState_CloseSentAndRcvd:
			{
			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s.Writef("Channel_Finalize, ID: %d, CloseSentAndRcvd", iChannel->fChannelID);
				}
			break;
			}
		default:
			{
			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s.Writef("Channel_Finalize, ID: %d, Bad State: %d ??",
					iChannel->fChannelID, iChannel->fState);
				}
//			ZUnimplemented();
			break;
			}
		}

	return this->pDetachIfUnused(iChannel);
	}

void Commer_Streamer::Channel_Read(
	Channel_Streamer* iChannel, void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(oDest);
	deque<uint8>& theBuffer = iChannel->fReceive_Buffer;

	ZGuardRMtxR locker(fMutex);
	while (iCount)
		{
		if (iChannel->fState != eState_Connected)
			{
			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s.Writef("Channel_Read, ID: %d, not connected", iChannel->fChannelID);
				}
			break;
			}

		if (const size_t countToRead = min(iCount, theBuffer.size()))
			{
			copy(theBuffer.begin(), theBuffer.begin() + countToRead, localDest);
			localDest += countToRead;
			iCount -= countToRead;
			theBuffer.erase(theBuffer.begin(), theBuffer.begin() + countToRead);
			this->WakeWrite();
			break;
			}
		else
			{
			iChannel->fCondition_Receive.Wait(fMutex);
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(oDest);
	}

size_t Commer_Streamer::Channel_CountReadable(Channel_Streamer* iChannel)
	{
	ZGuardRMtxR locker(fMutex);
	if (iChannel->fState != eState_Connected)
		return 0;
	return iChannel->fReceive_Buffer.size();
	}

bool Commer_Streamer::Channel_WaitReadable(Channel_Streamer* iChannel, double iTimeout)
	{
	const ZTime deadline = ZTime::sSystem() + iTimeout;
	ZGuardRMtxR locker(fMutex);
	for (;;)
		{
		if (iChannel->fState != eState_Connected || iChannel->fReceive_Buffer.size())
			return true;
		if (not iChannel->fCondition_Receive.WaitUntil(fMutex, deadline))
			return false;
		}
	}

void Commer_Streamer::Channel_Write(
	Channel_Streamer* iChannel, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	ZGuardRMtxR locker(fMutex);
	const uint8* localSource = static_cast<const uint8*>(iSource);

	// Wait till any other write has completed
	while (iChannel->fSend_Data)
		iChannel->fCondition_Send.Wait(fMutex);

	if (iChannel->fState == eState_Connected)
		{
		iChannel->fSend_Data = localSource;
		iChannel->fSend_Size = iCount;
		this->WakeWrite();

		while (iChannel->fSend_Size)
			iChannel->fCondition_Send.Wait(fMutex);

		if (oCountWritten)
			*oCountWritten = iChannel->fSend_Data - localSource;

		iChannel->fSend_Data = nullptr;
		iChannel->fCondition_Send.Broadcast();
		}
	else
		{
		if (oCountWritten)
			*oCountWritten = 0;
		}
	}

void Commer_Streamer::Channel_SendDisconnect(Channel_Streamer* iChannel)
	{
	ZGuardRMtxR locker(fMutex);

	if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
		s.Writef("Channel_SendDisconnect, ID: %d", iChannel->fChannelID);

	if (iChannel->fState == eState_Connected)
		{
		iChannel->fState = eState_CloseNeeded;
		this->WakeWrite();
		}
	}

bool Commer_Streamer::Channel_ReceiveDisconnect(Channel_Streamer* iChannel, double iTimeout)
	{
	ZGuardRMtxR locker(fMutex);

	if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
		s.Writef("Channel_ReceiveDisconnect, ID: %d", iChannel->fChannelID);

	while (iChannel->fState != eState_Dead)
		iChannel->fCondition_Receive.Wait(fMutex);

	return true;
	}

void Commer_Streamer::Channel_Abort(Channel_Streamer* iChannel)
	{
	ZGuardRMtxR locker(fMutex);

	if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
		s.Writef("Channel_Abort, ID: %d", iChannel->fChannelID);

	if (iChannel->fState == eState_Connected)
		{
		if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
			{
			s.Writef("Channel_Abort, ID: %d, connected, switch to close needed",
				iChannel->fChannelID);
			}
		iChannel->fState = eState_CloseNeeded;
		iChannel->fReceive_Buffer.clear();
		iChannel->fCondition_Receive.Broadcast();
		iChannel->fCondition_Send.Broadcast();
		this->WakeWrite();
		}
	}

ZRef<Channel_Streamer> Commer_Streamer::pFindChannel(uint16 iChannelID)
	{
	for (vector<Channel_Streamer*>::iterator i = fChannels.begin(); i != fChannels.end(); ++i)
		{
		if ((*i)->fChannelID == iChannelID)
			{
			if ((*i)->fState != eState_Dead)
				return *i;
			if (ZLOG(s, eDebug, "ZBlackBerry::Commer_Streamer"))
				s.Writef("pFindChannel, found eState_Dead for channelID: %d", iChannelID);
			}
		}

	if (ZLOG(s, eDebug, "ZBlackBerry::Commer_Streamer"))
		s.Writef("pFindChannel, couldn't find channel for channelID: %d", iChannelID);

	return null;
	}

ZRef<Channel_Streamer> Commer_Streamer::pFindChannel(const string& iName)
	{
	for (vector<Channel_Streamer*>::iterator i = fChannels.begin(); i != fChannels.end(); ++i)
		{
		if ((*i)->fName == iName)
			return *i;
		}

	if (ZLOG(s, eDebug, "ZBlackBerry::Commer_Streamer"))
		s << "pFindChannel, couldn't find channel for name: " << iName;

	return null;
	}

bool Commer_Streamer::pDetachIfUnused(Channel_Streamer* iChannel)
	{
	if (iChannel->IsReferenced())
		return false;

	if (iChannel->fState != eState_Dead)
		return false;

	ZUtil_STL::sEraseMustContain(1, fChannels, iChannel);
	this->WakeWrite();
	return true;
	}

void Commer_Streamer::pReadOne(uint16 iChannelID, uint16 iPayloadSize, const ZStreamR& iStreamR)
	{
	if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
		s.Writef("pReadOne, ID: %d, size: %d", iChannelID, iPayloadSize);

	if (iChannelID != 0)
		{
		// Data for a channel.
		vector<uint8> readBuffer(iPayloadSize, 0);
		iStreamR.Read(&readBuffer[0], iPayloadSize);

		ZGuardRMtxR locker(fMutex);
		if (ZRef<Channel_Streamer> theChannel = this->pFindChannel(iChannelID))
			{
			deque<uint8>& theBuffer = theChannel->fReceive_Buffer;
			if (theChannel->fPreserveBoundaries)
				{
				// We're using little-endian notation here, as
				// that's the standard BlackBerry generally uses.
				theBuffer.insert(theBuffer.end(), iPayloadSize);
				theBuffer.insert(theBuffer.end(), iPayloadSize >> 8);
				}

			theBuffer.insert(theBuffer.end(), readBuffer.begin(), readBuffer.end());
			theChannel->fCondition_Receive.Broadcast();
			}
		else
			{
			if (ZLOG(s, eDebug, "ZBlackBerry::Commer_Streamer"))
				{
				s.Writef("pReadOne, channel not allocated, ID: %d, size; %d",
					iChannelID, iPayloadSize);
				}
			}
		}
	else
		{
		// Control message.
		const uint8 theCommand = iStreamR.ReadUInt8();
		switch (theCommand)
			{
			case eMsg_GetAttribute_Ack:
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					{
					s << "pReadOne, GetAttribute_Ack";
					}

				// We've already read one byte (containing the command);
				Data theMB(iPayloadSize - 1);
				iStreamR.Read(theMB.GetPtrMutable(), theMB.GetSize());

				ZGuardRMtxR locker(fMutex);
				ZAssert(fGetAttribute);
				fGetAttribute->fFinished = true;
				fGetAttribute->fResult = theMB;
				fGetAttribute = nullptr;
				fGetAttributeSent = false;
				fCondition.Broadcast();
				break;
				}
			case eMsg_SelectMode_Ack:
			case eMsg_SelectMode_Nack:
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					{
					if (theCommand == eMsg_SelectMode_Ack)
						s << "pReadOne, SelectMode_Ack";
					else
						s << "pReadOne, SelectMode_Nack";
					}

				const uint16 theChannelID = iStreamR.ReadUInt16LE();
				/*const uint8 ignore = */iStreamR.ReadUInt8();

				char nameBuf[17];
				iStreamR.Read(nameBuf, 16);
				nameBuf[16] = 0;
				const string theName(nameBuf);

				ZGuardRMtxR locker(fMutex);
				if (ZRef<Channel_Streamer> theChannel = this->pFindChannel(theName))
					{
					ZAssert(theChannel->fState == eState_LookupSent);
					ZAssert(theChannel->fChannelID == 0);
					// Clear out the name, so it doesn't show up again.
					theChannel->fName.clear();
					if (theCommand == eMsg_SelectMode_Ack)
						{
						theChannel->fState = eState_ConnectNeeded;
						theChannel->fChannelID = theChannelID;
						// AG2008-03-02. Hmm -- I say there's three bytes, and then one
						// more and then the sizes, but the line following skips *5*. Why?

						// There's three bytes, then the following:
						// 01 SS SS 00 -- max BB send size
						// 02 RR RR 00 -- max BB receive size
						// 03 01 00 00
						// 04 01 00 00
						iStreamR.Skip(5);

						theChannel->fReceive_ChunkSize = iStreamR.ReadUInt16LE();
						iStreamR.Skip(2);
						theChannel->fSend_ChunkSize = iStreamR.ReadUInt16LE();

						this->WakeWrite();
						}
					else
						{
						// Nack.
						ZAssert(theChannelID == 0xFF);
						/*uint16 theError = */iStreamR.ReadUInt16LE();
						theChannel->fState = eState_Dead;
						theChannel->fError = Device::error_UnknownChannel;
						theChannel->fCondition_Receive.Broadcast();
						}
					}
				break;
				}
			case eMsg_PasswordChallenge:
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					{
					s << "pReadOne, PasswordChallenge";
					}

				const uint16 theChannelID = iStreamR.ReadInt16LE();
				/*const uint8 ignore = */iStreamR.ReadUInt8();
				const uint8 remainingTries = iStreamR.ReadUInt8();

				/*const uint8 unknown = */iStreamR.ReadUInt8();
				/*const uint16 param = */iStreamR.ReadUInt16LE();
				const uint32 challenge = iStreamR.ReadUInt32();

				ZGuardRMtxR locker(fMutex);
				if (ZRef<Channel_Streamer> theChannel = this->pFindChannel(theChannelID))
					{
					if (!theChannel->fHasPasswordHash)
						{
						theChannel->fState = eState_CloseNeeded;
						theChannel->fError = Device::error_PasswordNeeded;
						this->WakeWrite();
						}
					else if (remainingTries <= 3)
						{
						theChannel->fState = eState_CloseNeeded;
						theChannel->fError = Device::error_PasswordExhausted;
						this->WakeWrite();
						}
					else
						{
						theChannel->fState = eState_ChallengeRcvd;
						theChannel->fChallenge = challenge;
						this->WakeWrite();
						}
					}
				break;
				}
			case eMsg_PasswordFailed:
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					{
					s << "pReadOne, PasswordFailed";
					}

				const uint16 theChannelID = iStreamR.ReadUInt16LE();

				ZGuardRMtxR locker(fMutex);
				if (ZRef<Channel_Streamer> theChannel = this->pFindChannel(theChannelID))
					{
					theChannel->fState = eState_CloseNeeded;
					theChannel->fError = Device::error_PasswordIncorrect;
					this->WakeWrite();
					}
				break;
				}
			case eMsg_ChannelOpen_Ack:
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					{
					s << "pReadOne, Open_Ack";
					}

				const uint16 theChannelID = iStreamR.ReadUInt16LE();

				ZGuardRMtxR locker(fMutex);
				if (ZRef<Channel_Streamer> theChannel = this->pFindChannel(theChannelID))
					{
					theChannel->fState = eState_Connected;
					theChannel->fCondition_Receive.Broadcast();
					}
				break;
				}
			case eMsg_ChannelClose:
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					{
					s << "pReadOne, Close";
					}

				const uint16 theChannelID = iStreamR.ReadUInt8();

				if (ZLOG(s, eDebug, "ZBlackBerry::Commer_Streamer"))
					{
					s.Writef("Received close for channelID: %d", theChannelID);
					}

				ZGuardRMtxR locker(fMutex);
				if (ZRef<Channel_Streamer> theChannel = this->pFindChannel(theChannelID))
					{
					if (theChannel->fState == eState_Connected)
						{
						theChannel->fState = eState_CloseRcvd;
						}
					else if (theChannel->fState == eState_CloseNeeded)
						{
						theChannel->fState = eState_CloseRcvd;
						}
					else if (theChannel->fState == eState_CloseSent)
						{
						theChannel->fState = eState_CloseSentAndRcvd;
						}
					else
						{
						if (ZLOG(s, eDebug, "ZBlackBerry::Commer_Streamer"))
							{
							s.Writef("Bad state, channelID: %d", theChannelID);
							}
						}
					theChannel->fReceive_Buffer.clear();
					theChannel->fCondition_Receive.Broadcast();
					theChannel->fCondition_Send.Broadcast();
					this->WakeWrite();
					}
				break;
				}
			case eMsg_ChannelClose_Ack:
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					{
					s << "pReadOne, Close_Ack";
					}

				const uint16 theChannelID = iStreamR.ReadUInt16LE();

				ZGuardRMtxR locker(fMutex);
				if (ZRef<Channel_Streamer> theChannel = this->pFindChannel(theChannelID))
					{
					theChannel->fState = eState_Dead;
					theChannel->fCondition_Receive.Broadcast();
					theChannel->fCondition_Send.Broadcast();
					}
				break;
				}
			case eMsg_ChannelClose_Nack:
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					{
					s << "pReadOne, Close_Nack";
					}

				const uint16 theChannelID = iStreamR.ReadUInt16LE();

				ZGuardRMtxR locker(fMutex);
				if (ZRef<Channel_Streamer> theChannel = this->pFindChannel(theChannelID))
					{
					theChannel->fState = eState_Dead;
					theChannel->fCondition_Receive.Broadcast();
					theChannel->fCondition_Send.Broadcast();
					}
				break;
				}
			case eMsg_SequenceHandshake:
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					{
					s << "pReadOne, SequenceHandshake";
					}

				// First byte is LSB of the channel ID.
				uint8 seqChannelID = iStreamR.ReadUInt8();

				// Next byte is usually 0x01. It may be part of
				// the channel ID, and may include some other flag.
				// Ignore for now.
				/*uint8 ignore = */iStreamR.ReadUInt8();

				// Usually zero. More flags?
				/*uint8 flag = */iStreamR.ReadUInt8();

				// A sequence that increments with each chunk sent
				// for a particular channel.
				uint32 sequence = iStreamR.ReadUInt32LE();

				ZGuardRMtxR locker(fMutex);
				if (ZRef<Channel_Streamer> theChannel = this->pFindChannel(seqChannelID))
					{
					// Check that next sequence is good
					if (theChannel->fNextSequence != sequence)
						{
						if (ZLOG(s, eNotice, "ZBlackBerry::Commer_Streamer"))
							{
							s.Writef("channel %d, expected sequence: %u, received: %u",
								seqChannelID, theChannel->fNextSequence, sequence);
							}
						}
					ZAssert(theChannel->fWaitingForSequence);
					theChannel->fWaitingForSequence = false;
					theChannel->fNextSequence = sequence + 1;
					theChannel->fCondition_Send.Broadcast();
					this->WakeWrite();
					}
				break;
				}
			}
		}
	}

static void spSHA1(const void* iSource, size_t iSourceSize, uint8 oDigest[20])
	{
	ZStream_SHA1::Context theContext;
	sInit(theContext);
	sUpdate(theContext, iSource, iSourceSize);
	sFinal(theContext, oDigest);
	}

bool Commer_Streamer::pWriteOne(const ZStreamW& iStreamW, Channel_Streamer* iChannel)
	{
	switch (iChannel->fState)
		{
		case eState_LookupNeeded:
			{
			iChannel->fState = eState_LookupSent;

			StreamW_Chunked w;
			w.WriteUInt8(eMsg_SelectMode);
			w.WriteUInt16LE(0xFF); // Dummy channel ID
			w.WriteUInt8(0x00); // Flags. Barry uses 0x05, why?
			const size_t nameLength = iChannel->fName.length();
			w.WriteString(iChannel->fName);
			for (size_t x = nameLength; x < 16; ++x)
				w.WriteUInt8(0x00); // Padding

			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s << "pWriteOne, SelectMode";
				}

			return this->pSend(w, 0, iStreamW);
			}
		case eState_ConnectNeeded:
			{
			iChannel->fState = eState_ConnectSent;

			StreamW_Chunked w;
			w.WriteUInt8(eMsg_ChannelOpen);
			w.WriteUInt16LE(iChannel->fChannelID);
			w.WriteUInt8(0x00);

			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s << "pWriteOne, Open";
				}

			return this->pSend(w, 0, iStreamW);
			}
		case eState_ChallengeRcvd:
			{
			// Hmm. We should probably assert that we've got a hash -- we should not
			// switch to this state without one.
			// ZAssert(iChannel->fHasPasswordHash);

			iChannel->fState = eState_ConnectSent;
			const PasswordHash& thePWHash = iChannel->fPasswordHash;

			// Hash the password into bytes 4 - 24
			uint8 prefixedHash[24];
			ZMemCopy(&prefixedHash[4], thePWHash.fData, 20);

			// Put the challenge into bytes 0 - 3
			ZByteSwap_WriteBig32(&prefixedHash[0], iChannel->fChallenge);

			// Hash the whole thing.
			uint8 sentHash[20];
			spSHA1(prefixedHash, sizeof(prefixedHash), sentHash);

			StreamW_Chunked w;
			w.WriteUInt8(eMsg_PasswordResponse);
			w.WriteUInt16LE(iChannel->fChannelID); //??
			w.WriteUInt8(0); // Sequence
			w.WriteUInt8(0); // Remaining tries
			w.WriteUInt8(0); // Unknown
			w.WriteUInt16LE(0x14); // Param ??
			w.Write(sentHash, 20);

			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s << "pWriteOne, PasswordResponse";
				}

			return this->pSend(w, 0, iStreamW);
			}
		case eState_Connected:
			{
			if (iChannel->fSend_Size == 0)
				return false;

			if (iChannel->fWaitingForSequence)
				return false;

			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s << "pWriteOne, Data";
				}

			size_t countToWrite = min(iChannel->fSend_ChunkSize, iChannel->fSend_Size);

			// If the physical amount written is a multiple of 64 bytes we need to send
			// a funky three byte packet. It is virtually certain that we're running over
			// a USB channel, so the boundary of the write will be known to the receiver
			// and although a regular stream won't be able to detect this weirdness, a BB
			// will. Perhaps this should be a parameterizable behavior.
			if (0 == ((countToWrite + 4) % 0x40))
				{
				if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
					s << "pWriteOne, Sending funky";

				if (!this->pSendFunky(countToWrite + 4, iStreamW))
					return false;
				}

			StreamW_Chunked w;
			size_t countWritten;
			w.Write(iChannel->fSend_Data, countToWrite, &countWritten);

			iChannel->fSend_Data += countWritten;
			iChannel->fSend_Size = 0;
			iChannel->fCondition_Send.Broadcast();

			iChannel->fWaitingForSequence = true;

			return this->pSend(w, iChannel->fChannelID, iStreamW);
			}
		case eState_CloseNeeded:
			{
			iChannel->fState = eState_CloseSent;

			StreamW_Chunked w;
			w.WriteUInt8(eMsg_ChannelClose);
			w.WriteUInt16LE(iChannel->fChannelID);
			w.WriteUInt8(0x00);

			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s << "pWriteOne, Close";
				}

			return this->pSend(w, 0, iStreamW);
			}
		case eState_CloseRcvd:
			{
			iChannel->fState = eState_Dead;

			StreamW_Chunked w;
			w.WriteUInt8(eMsg_ChannelClose_Ack);
			w.WriteUInt16LE(iChannel->fChannelID);
			w.WriteUInt8(0x00);

			this->pDetachIfUnused(iChannel);

			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s << "pWriteOne, eState_CloseRcvd, Close_Ack";
				}

			return this->pSend(w, 0, iStreamW);
			}
		case eState_CloseSentAndRcvd:
			{
			iChannel->fState = eState_CloseSent;

			StreamW_Chunked w;
			w.WriteUInt8(eMsg_ChannelClose_Ack);
			w.WriteUInt16LE(iChannel->fChannelID);
			w.WriteUInt8(0x00);

			if (ZLOG(s, eDebug + 1, "ZBlackBerry::Commer_Streamer"))
				{
				s << "pWriteOne, eState_CloseSentAndRcvd, Close_Ack";
				}

			return this->pSend(w, 0, iStreamW);
			}
		}

	return false;
	}

bool Commer_Streamer::pSend(StreamW_Chunked& iSC, uint16 iChannelID, const ZStreamW& iStreamW)
	{
	fMutex.Release();
	if (iSC.Send(iChannelID, iStreamW))
		{
		fMutex.Acquire();
		return true;
		}
	fMutex.Acquire();
	fLifecycle = eLifecycle_StreamsDead;
// Should we return false here?
	return false;
	}

bool Commer_Streamer::pSendFunky(uint16 iLength, const ZStreamW& iStreamW)
	{
	ZAssert(0 == (iLength % 0x40));
	const uint8 buffer[3] = { iLength, iLength >> 8, 0 };

	fMutex.Release();
	size_t countWritten;
	iStreamW.Write(buffer, 3, &countWritten);
	fMutex.Acquire();

	if (countWritten == 3)
		return true;

	fLifecycle = eLifecycle_StreamsDead;
	return false;
	}

void Commer_Streamer::pFlush(const ZStreamW& iStreamW)
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

// =================================================================================================
// MARK: - ZBlackBerry::Device_Streamer

Device_Streamer::Device_Streamer(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fStreamerR(iStreamerR),
	fStreamerW(iStreamerW)
	{}

Device_Streamer::~Device_Streamer()
	{}

void Device_Streamer::Initialize()
	{
	Device::Initialize();
	fCommer = new Commer_Streamer(fStreamerR, fStreamerW);
	fCommer->SetCallable_Finished(sCallable(sWeakRef(this), &Device_Streamer::pCommerFinished));
	sStartCommerRunners(fCommer);
	}

void Device_Streamer::Finalize()
	{
	if (ZRef<Commer_Streamer> theCommer = fCommer)
		{
		fCommer.Clear();
		theCommer->Stop();
		}
	Device::Finalize();
	}

ZRef<Channel> Device_Streamer::Open(bool iPreserveBoundaries,
	const std::string& iName, const PasswordHash* iPasswordHash, Error* oError)
	{
	if (fCommer)
		return fCommer->Open(iPreserveBoundaries, iName, iPasswordHash, oError);
	return null;
	}

ZQ<Data> Device_Streamer::GetAttribute(uint16 iObject, uint16 iAttribute)
	{
	if (fCommer)
		return fCommer->GetAttribute(iObject, iAttribute);
	return null;
	}

uint32 Device_Streamer::GetPIN()
	{
	if (fCommer)
		return fCommer->GetPIN();
	return 0;
	}

void Device_Streamer::pCommerFinished(ZRef<ZCommer> iCommer)
	{
	ZAssert(iCommer == fCommer);
	fCommer.Clear();
	Device::pFinished();
	}

} // namespace ZBlackBerry
} // namespace ZooLib
