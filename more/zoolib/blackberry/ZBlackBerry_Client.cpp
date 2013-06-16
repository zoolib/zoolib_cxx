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

#include "zoolib/blackberry/ZBlackBerry_Client.h"

#include "zoolib/ZCommer.h"
#include "zoolib/ZLog.h"

using std::max;
using std::min;
using std::runtime_error;
using std::string;
using std::vector;

namespace ZooLib {
namespace ZBlackBerry {

// =================================================================================================
// MARK: - Channel_Client

class Channel_Client
:	public Channel,
	private ZStreamWCon
	{
public:
	Channel_Client(ZRef<ZStreamerRWCon> iSRWCon, size_t iIdealReadSize, size_t iIdealWriteSize);
	virtual ~Channel_Client();

// From ZBlackBerry::Channel
	virtual size_t GetIdealSize_Read();
	virtual size_t GetIdealSize_Write();

// From ZStreamerR via ZStreamerRWCon
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerW via ZStreamerRWCon
	virtual const ZStreamWCon& GetStreamWCon();

private:
// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamWCon
	virtual void Imp_Abort();

private:
	ZRef<ZStreamerRWCon> fSRWCon;
	size_t fIdealReadSize;
	size_t fIdealWriteSize;
	};

Channel_Client::Channel_Client(
	ZRef<ZStreamerRWCon> iSRWCon, size_t iIdealReadSize, size_t iIdealWriteSize)
:	fSRWCon(iSRWCon),
	fIdealReadSize(fIdealReadSize),
	fIdealWriteSize(fIdealWriteSize)
	{}

Channel_Client::~Channel_Client()
	{}

size_t Channel_Client::GetIdealSize_Read()
	{ return fIdealReadSize; }

size_t Channel_Client::GetIdealSize_Write()
	{ return fIdealWriteSize; }

const ZStreamRCon& Channel_Client::GetStreamRCon()
	{ return fSRWCon->GetStreamRCon(); }

const ZStreamWCon& Channel_Client::GetStreamWCon()
	{ return *this; }

void Channel_Client::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (size_t countToWrite = min(iCount, size_t(0xFFFFU)))
		{
		const ZStreamW& w = fSRWCon->GetStreamW();
		w.WriteUInt16LE(countToWrite);

		// We rely on ZStreamW::Write's behavior that it will push out
		// everything we pass to it by repeatedly invoking Imp_Write if
		// necessary. If countWritten is less than countToWrite then
		// the stream must have closed, and next time round we'll get zero written.
		w.Write(iSource, countToWrite, oCountWritten);
		}
	else if (oCountWritten)
		{
		*oCountWritten = 0;
		}
	}

void Channel_Client::Imp_Flush()
	{ fSRWCon->GetStreamWCon().Flush(); }

void Channel_Client::Imp_SendDisconnect()
	{
	// Hmm. Send zero-sized chunk?
	fSRWCon->GetStreamWCon().SendDisconnect();
	}

void Channel_Client::Imp_Abort()
	{ fSRWCon->GetStreamWCon().Abort(); }

// =================================================================================================
// MARK: - Device_Client

class Device_Client
:	public Device,
	private ZCommer
	{
public:
	Device_Client(ZRef<ZStreamerRWCon> iSRWCon,
		ZRef<ZStreamerRWConFactory> iFactory, uint64 iDeviceID);

	virtual ~Device_Client();

// From Device
	virtual ZRef<Channel> Open(bool iPreserveBoundaries,
		const string& iName, const PasswordHash* iPasswordHash, Error* oError);
	virtual ZQ<Data> GetAttribute(uint16 iObject, uint16 iAttribute);
	virtual uint32 GetPIN();

// From ZCommer
	virtual bool Read(const ZStreamR& r);
	virtual bool Write(const ZStreamW& w);

	virtual void Finished();

// Our protocol
	void Stop();

private:
	ZRef<ZStreamerRWConFactory> fFactory;
	uint64 fDeviceID;
	bool fOpen;
	};

Device_Client::Device_Client(ZRef<ZStreamerRWCon> theSRWCon,
	ZRef<ZStreamerRWConFactory> iFactory, uint64 iDeviceID)
:	ZCommer(theSRWCon, theSRWCon),
	fFactory(iFactory),
	fDeviceID(iDeviceID),
	fOpen(true)
	{
	sStartCommerRunners(this);
	}

Device_Client::~Device_Client()
	{
	ZLOGFUNCTION(eDebug + 2);
	}

ZRef<Channel> Device_Client::Open(bool iPreserveBoundaries,
	const string& iName, const PasswordHash* iPasswordHash, Error* oError)
	{
	try
		{
		if (ZRef<ZStreamerRWCon> theSRWCon = fFactory->MakeStreamerRWCon())
			{
			const ZStreamW& w = theSRWCon->GetStreamW();
			w.WriteUInt8(6);
			w.WriteUInt64(fDeviceID);
			w.WriteBool(iPreserveBoundaries);

			if (iPasswordHash)
				{
				w.WriteBool(true);
				w.Write(iPasswordHash, sizeof(PasswordHash));
				}
			else
				{
				w.WriteBool(false);
				}

			w.WriteCount(iName.size());
			w.WriteString(iName);

			w.Flush();

			const ZStreamR& r = theSRWCon->GetStreamR();

			const Error theError = static_cast<Error>(r.ReadUInt32());
			const size_t idealReadSize = r.ReadUInt32();
			const size_t idealWriteSize = r.ReadUInt32();

			if (oError)
				*oError = theError;

			if (theError == error_None)
				return new Channel_Client(theSRWCon, idealReadSize, idealWriteSize);
			}
		}
	catch (...)
		{}

	return null;
	}

ZQ<Data> Device_Client::GetAttribute(uint16 iObject, uint16 iAttribute)
	{
	try
		{
		if (ZRef<ZStreamerRWCon> theSRWCon = fFactory->MakeStreamerRWCon())
			{
			const ZStreamW& w = theSRWCon->GetStreamW();
			w.WriteUInt8(4);
			w.WriteUInt64(fDeviceID);
			w.WriteUInt16(iObject);
			w.WriteUInt16(iAttribute);
			w.Flush();

			const ZStreamR& r = theSRWCon->GetStreamR();
			if (r.ReadBool())
				{
				Data theMB(r.ReadCount());
				r.Read(theMB.GetPtrMutable(), theMB.GetSize());
				return theMB;
				}
			}
		}
	catch (...)
		{}
	return null;
	}

uint32 Device_Client::GetPIN()
	{
	try
		{
		if (ZRef<ZStreamerRWCon> theSRWCon = fFactory->MakeStreamerRWCon())
			{
			const ZStreamW& w = theSRWCon->GetStreamW();
			w.WriteUInt8(5);
			w.WriteUInt64(fDeviceID);
			w.Flush();

			const ZStreamR& r = theSRWCon->GetStreamR();
			if (r.ReadBool())
				return r.ReadUInt32();
			}
		}
	catch (...)
		{}
	return 0;
	}

/// \sa ZBlackBerryServer::Handler_DeviceFinished::Read
bool Device_Client::Read(const ZStreamR& r)
	{
	ZLOGFUNCTION(eDebug + 2);

	const bool req = r.ReadBool();
	ZAssert(!req);

	fOpen = false;
//##	ZStreamerWriter::Wake();
	return false;
	}

/// \sa ZBlackBerryServer::Handler_DeviceFinished::Read
bool Device_Client::Write(const ZStreamW& w)
	{
	if (!fOpen)
		{
		if (ZLOG(s, eDebug + 2, "ZBlackBerry::Device_Client"))
			s << "Write false, return false";
		w.WriteBool(false);
		return false;
		}

	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Device_Client"))
		s << "Write nothing, return true";

	return true;
	}

void Device_Client::Finished()
	{
	ZLOGFUNCTION(eDebug + 2);
	Device::pFinished();
	}

/// \sa ZBlackBerryServer::Handler_DeviceFinished::TripIt
void Device_Client::Stop()
	{
	fOpen = false;
//##	ZStreamerWriter::Wake();
	}

// =================================================================================================
// MARK: - Manager_Client::Commer_Changed

class Manager_Client::Commer_Changed : public ZCommer
	{
public:
	Commer_Changed(ZRef<ZStreamerRWCon> iSRWCon, ZRef<Manager_Client> iManager);

// From ZStreamerReader via ZCommer
	virtual void ReadFinished();

// From ZCommer
	virtual bool Read(const ZStreamR& r);
	virtual bool Write(const ZStreamW& w);
	virtual void Finished();

private:
	ZWeakRef<Manager_Client> fManager;
	bool fSendNotificationRequest;
	bool fSendClose;

	friend class Manager_Client;
	};

Manager_Client::Commer_Changed::Commer_Changed(
	ZRef<ZStreamerRWCon> iSRWCon, ZRef<Manager_Client> iManager)
:	ZCommer(iSRWCon, iSRWCon),
	fManager(iManager),
	fSendNotificationRequest(true),
	fSendClose(false)
	{}

void Manager_Client::Commer_Changed::ReadFinished()
	{
	// Force the writer to attempt to close, and thus to exit.
	fSendClose = true;
	this->WakeWrite();

//##	ZCommer::ReadFinished();
	}

bool Manager_Client::Commer_Changed::Read(const ZStreamR& r)
	{
	const bool req = r.ReadBool();
	if (req)
		{
		if (ZRef<Manager_Client> theManager = fManager)
			theManager->pChanged();

		fSendNotificationRequest = true;
		this->WakeWrite();
		return true;
		}
	else
		{
		return false;
		}
	}

bool Manager_Client::Commer_Changed::Write(const ZStreamW& w)
	{
	if (fSendClose)
		{
		w.WriteBool(false);
		return false;
		}

	if (fSendNotificationRequest)
		{
		fSendNotificationRequest = false;
		w.WriteBool(true);
		}

	return true;
	}

void Manager_Client::Commer_Changed::Finished()
	{
	ZLOGFUNCTION(eDebug + 2);

	if (ZRef<Manager_Client> theManager = fManager)
		theManager->pDetached(this);
	}

// =================================================================================================
// MARK: - Manager_Client

Manager_Client::Manager_Client()
	{}

Manager_Client::Manager_Client(ZRef<ZStreamerRWConFactory> iFactory, bool iAutoReconnect)
:	fFactory(iFactory),
	fAutoReconnect(iAutoReconnect)
	{}

Manager_Client::~Manager_Client()
	{}

void Manager_Client::Initialize()
	{
	Manager::Initialize();

	ZRef<ZStreamerRWCon> theSRWCon = fFactory->MakeStreamerRWCon();
	if (!theSRWCon)
		throw runtime_error("ZBlackBerry::Manager_Client, Couldn't connect");

	this->pStartCommer(theSRWCon);
	}

void Manager_Client::Finalize()
	{
//	fAutoReconnect = false;
	if (ZRef<Commer_Changed> theCommer_Changed = fCommer_Changed)
		{
		fCommer_Changed.Clear();
		theCommer_Changed->fManager.Clear();
		//##theCommer_Changed->ZStreamerWriter::Wake();
		}

	Manager::Finalize();
	}

void Manager_Client::GetDeviceIDs(vector<uint64>& oDeviceIDs)
	{
	try
		{
		if (ZRef<ZStreamerRWCon> theSRWCon = fFactory->MakeStreamerRWCon())
			{
			const ZStreamW& w = theSRWCon->GetStreamW();
			w.WriteUInt8(1);
			w.Flush();

			const ZStreamR& r = theSRWCon->GetStreamR();

			size_t theCount = r.ReadCount();

			vector<uint64> deviceIDs;
			deviceIDs.reserve(theCount);

			for (size_t x = 0; x < theCount; ++x)
				deviceIDs.push_back(r.ReadUInt64());

			oDeviceIDs.swap(deviceIDs);
			}
		}
	catch (...)
		{}
	}

ZRef<Device> Manager_Client::Open(uint64 iDeviceID)
	{
	try
		{
		if (ZRef<ZStreamerRWCon> theSRWCon = fFactory->MakeStreamerRWCon())
			{
			const ZStreamW& w = theSRWCon->GetStreamW();
			w.WriteUInt8(2);
			w.WriteUInt64(iDeviceID);
			w.Flush();

			const ZStreamR& r = theSRWCon->GetStreamR();
			if (r.ReadBool())
				return new Device_Client(theSRWCon, fFactory, iDeviceID);
			}
		}
	catch (...)
		{}
	return null;
	}


void Manager_Client::pDetached(ZRef<Commer_Changed> iCommer)
	{
	ZAssert(iCommer == fCommer_Changed);
	fCommer_Changed.Clear();

	while (fAutoReconnect)
		{
		if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager_Client"))
			s << "Detached, retrying";
		ZTime nextTry = ZTime::sNow() + 2.0;
		if (ZRef<ZStreamerRWCon> theSRWCon = fFactory->MakeStreamerRWCon())
			{
			this->pStartCommer(theSRWCon);
			return;
			}
		if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager_Client"))
			s << "Detached, retry failed";

		ZThread::sSleep(max(0.1, nextTry - ZTime::sNow()));
		}

	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager_Client"))
		s << "Detached, not retrying";

	this->pChanged();
	}

void Manager_Client::pStartCommer(ZRef<ZStreamerRWCon> iSRWCon)
	{
	iSRWCon->GetStreamW().WriteUInt8(0);

	fCommer_Changed = new Commer_Changed(iSRWCon, this);
	this->pChanged();

	sStartCommerRunners(fCommer_Changed);
	}

} // namespace ZBlackBerry
} // namespace ZooLib
