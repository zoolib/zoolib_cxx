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

#include "zoolib/blackberry/ZBlackBerryServer.h"

#include "zoolib/ZCallable_Bind.h"
#include "zoolib/ZCallable_Function.h"
#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZCaller_Thread.h"
#include "zoolib/ZCommer.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUtil_STL_vector.h"

using std::string;
using std::vector;

namespace ZooLib {

// =================================================================================================
// MARK: - ZBlackBerryServer::Handler_ManagerChanged

class ZBlackBerryServer::Handler_ManagerChanged
:	public ZCommer
	{
public:
	Handler_ManagerChanged(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
		ZBlackBerryServer* iServer);
	virtual ~Handler_ManagerChanged();

// From ZCommer
	virtual bool Read(const ZStreamR& r);
	virtual bool Write(const ZStreamW& w);

	virtual void Finished();

// Called by ZBlackBerryServer
	void TripIt();

private:
	ZBlackBerryServer* fServer;
	ZMtxR fMutex;
	enum EState
		{ eState_Quiet, eState_Changed, eState_Waiting, eState_SendChanged, eState_SendClosed};
	EState fState;
	};

ZBlackBerryServer::Handler_ManagerChanged::Handler_ManagerChanged(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
	ZBlackBerryServer* iServer)
:	ZCommer(iStreamerR, iStreamerW),
	fServer(iServer),
	fState(eState_Quiet)
	{}

ZBlackBerryServer::Handler_ManagerChanged::~Handler_ManagerChanged()
	{
	ZLOGFUNCTION(eDebug + 2);
	}

bool ZBlackBerryServer::Handler_ManagerChanged::Read(const ZStreamR& r)
	{
	const bool req = r.ReadBool();

	ZGuardRMtxR locker(fMutex);
	if (!req)
		{
		fState = eState_SendClosed;
		locker.Release();
		//##ZStreamerWriter::Wake();
		return false;
		}

	switch (fState)
		{
		case eState_Quiet:
			{
			fState = eState_Waiting;
			return true;
			}
		case eState_Changed:
			{
			fState = eState_SendChanged;
			locker.Release();
			//##ZStreamerWriter::Wake();
			return true;
			}
		}

	ZUnimplemented();
	return false;
	}

bool ZBlackBerryServer::Handler_ManagerChanged::Write(const ZStreamW& w)
	{
	ZGuardRMtxR locker(fMutex);

	if (fState == eState_SendChanged)
		{
		fState = eState_Quiet;
		locker.Release();
		w.WriteBool(true);
		return true;
		}
	else if (fState == eState_SendClosed)
		{
		locker.Release();
		w.WriteBool(false);
		return false;
		}

	return true;
	}

void ZBlackBerryServer::Handler_ManagerChanged::Finished()
	{
	ZLOGFUNCTION(eDebug+2);

	fServer->pRemove_ManagerChanged(this);
	}

void ZBlackBerryServer::Handler_ManagerChanged::TripIt()
	{
	ZGuardRMtxR locker(fMutex);

	if (fState == eState_Waiting)
		fState = eState_SendChanged;
	else if (fState == eState_Quiet)
		fState = eState_Changed;

	locker.Release();

	//##ZStreamerWriter::Wake();
	}

// =================================================================================================
// MARK: - ZBlackBerryServer::Handler_DeviceFinished

class ZBlackBerryServer::Handler_DeviceFinished
:	public ZCommer
	{
public:
	Handler_DeviceFinished(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
		ZBlackBerryServer* iServer);
	virtual ~Handler_DeviceFinished();

// From ZCommer
	virtual bool Read(const ZStreamR& r);
	virtual bool Write(const ZStreamW& w);

	virtual void Finished();

// Called by ZBlackBerryServer
	void TripIt();

private:
	ZBlackBerryServer* fServer;
	bool fOpen;
	};

ZBlackBerryServer::Handler_DeviceFinished::Handler_DeviceFinished(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
	ZBlackBerryServer* iServer)
:	ZCommer(iStreamerR, iStreamerW),
	fServer(iServer),
	fOpen(true)
	{}

ZBlackBerryServer::Handler_DeviceFinished::~Handler_DeviceFinished()
	{
	ZLOGFUNCTION(eDebug + 2);
	}

/// \sa ZBlackBerry::Device_Client::Read
bool ZBlackBerryServer::Handler_DeviceFinished::Read(const ZStreamR& r)
	{
	ZLOGFUNCTION(eDebug + 2);

	const bool req = r.ReadBool();
	ZAssert(!req);

	fOpen = false;
	//##ZStreamerWriter::Wake();
	return false;
	}

/// \sa ZBlackBerry::Device_Client::Write
bool ZBlackBerryServer::Handler_DeviceFinished::Write(const ZStreamW& w)
	{
	if (!fOpen)
		{
		if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_DeviceFinished"))
			s << "Write false, return false";

		w.WriteBool(false);
		return false;
		}

	if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_DeviceFinished"))
		s << "Write nothing, return true";

	return true;
	}

void ZBlackBerryServer::Handler_DeviceFinished::Finished()
	{
	ZLOGFUNCTION(eDebug+2);
	fServer->pRemove_DeviceFinished(this);
	}

/// \sa ZBlackBerry::Device_Client::Stop
void ZBlackBerryServer::Handler_DeviceFinished::TripIt()
	{
	ZLOGFUNCTION(eDebug + 2);

	fOpen = false;
	//##ZStreamerWriter::Wake();
	}

// =================================================================================================
// MARK: - ZBlackBerryServer

static string spReadString(const ZStreamR& r)
	{
	if (size_t theLength = r.ReadCount())
		return r.ReadString(theLength);
	return string();
	}

ZBlackBerryServer::ZBlackBerryServer(ZRef<ZBlackBerry::Manager> iManager)
:	fManager(iManager)
,	fCB_ManagerChanged(sCallable(this, &ZBlackBerryServer::pManagerChanged))
,	fCB_DeviceFinished(sCallable(this, &ZBlackBerryServer::pDeviceFinished))
	{
	fManager->SetCallable(fCB_ManagerChanged);
	this->pManagerChanged(fManager);
	}

ZBlackBerryServer::~ZBlackBerryServer()
	{
	ZAssert(fHandlers_ManagerChanged.empty());

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		ZAssert(i->fHandlers.empty());

	fManager->SetCallable(null);
	}

static void spCopyChunked(ZRef<ZStreamerRCon> iStreamerRCon, ZRef<ZStreamerWCon> iStreamerWCon)
	{
	const ZStreamRCon& r = iStreamerRCon->GetStreamRCon();
	const ZStreamWCon& w = iStreamerWCon->GetStreamWCon();
	try
		{
		while (true)
			{
			const size_t theSize = r.ReadUInt16LE();

			if (theSize == 0)
				{
				// End of stream
				r.ReceiveDisconnect(-1);
				w.SendDisconnect();
				break;
				}
			else
				{
				std::vector<char> buffer(theSize);

				r.Read(&buffer[0], theSize);
				w.Write(&buffer[0], theSize);
				}
			}
		}
	catch (...)
		{
		r.Abort();
		w.Abort();
		}
	}

static void spCopyAllCon(
	ZRef<ZStreamerRCon> iStreamerRCon, ZRef<ZStreamerWCon> iStreamerWCon, size_t iChunkSize)
	{
	while (ZStream::sCopyAllCon(
		iStreamerRCon->GetStreamRCon(), iChunkSize, iStreamerWCon->GetStreamWCon(), 10))
		{}
	}

void ZBlackBerryServer::HandleRequest(ZRef<ZStreamerRWCon> iSRWCon)
	{
	const ZStreamR& r = iSRWCon->GetStreamR();
	const ZStreamW& w = iSRWCon->GetStreamW();

	const int req = r.ReadUInt8();
	if (req == 0)
		{
		// Async changed notifications
		ZGuardRMtxR locker(fMutex);
		ZRef<Handler_ManagerChanged> theHandler = new Handler_ManagerChanged(iSRWCon, iSRWCon, this);
		fHandlers_ManagerChanged.push_back(theHandler);
		locker.Release();

		sStartCommerRunners(theHandler);
		}
	else if (req == 1)
		{
		// Synchronous get device IDs
		ZGuardRMtxR locker(fMutex);

		vector<uint64> theIDs;
		for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
			{
			if (i->fLive)
				theIDs.push_back(i->fID);
			}
		locker.Release();

		w.WriteCount(theIDs.size());
		for (vector<uint64>::iterator i = theIDs.begin(); i != theIDs.end(); ++i)
			w.WriteUInt64(*i);
		}
	else if (req == 2)
		{
		// Async device finished notifications
		const uint64 deviceID = r.ReadUInt64();

		ZGuardRMtxR locker(fMutex);

		bool gotIt = false;
		for (vector<Entry_t>::iterator i = fEntries.begin(); !gotIt && i != fEntries.end(); ++i)
			{
			if (i->fLive && i->fID == deviceID)
				{
				ZRef<Handler_DeviceFinished> theHandler =
					new Handler_DeviceFinished(iSRWCon, iSRWCon, this);
				i->fHandlers.push_back(theHandler);
				locker.Release();

				w.WriteBool(true);
				sStartCommerRunners(theHandler);
				gotIt = true;
				}
			}

		if (!gotIt)
			{
			locker.Release();
			w.WriteBool(false);
			}
		}
	else if (req == 4)
		{
		// Synchronous get attribute
		const uint64 deviceID = r.ReadUInt64();
		const uint16 object = r.ReadUInt16();
		const uint16 attribute = r.ReadUInt16();
		if (ZRef<ZBlackBerry::Device> theDevice = this->pGetDevice(deviceID))
			{
			if (ZQ<ZBlackBerry::Data> theQ = theDevice->GetAttribute(object, attribute))
				{
				ZBlackBerry::Data theMB = theQ.Get();
				w.WriteBool(true);
				w.WriteCount(theMB.GetSize());
				w.Write(theMB.GetPtr(), theMB.GetSize());
				return;
				}
			}
		w.WriteBool(false);
		}
	else if (req == 5)
		{
		// Synchronous get PIN
		const uint64 deviceID = r.ReadUInt64();
		if (ZRef<ZBlackBerry::Device> theDevice = this->pGetDevice(deviceID))
			{
			uint32 thePIN = theDevice->GetPIN();
			w.WriteBool(true);
			w.WriteUInt32(thePIN);
			}
		else
			{
			w.WriteBool(false);
			}
		}
	else if (req == 6)
		{
		// Open channel
		const uint64 deviceID = r.ReadUInt64();

		const bool preserveBoundaries = r.ReadBool();

		const bool gotHash = r.ReadBool();
		ZBlackBerry::PasswordHash thePasswordHash;
		if (gotHash)
			r.Read(&thePasswordHash, sizeof(thePasswordHash));

		const string channelName = spReadString(r);

		ZBlackBerry::Device::Error theError = ZBlackBerry::Device::error_DeviceClosed;

		if (ZRef<ZBlackBerry::Device> theDevice = this->pGetDevice(deviceID))
			{
			if (ZRef<ZBlackBerry::Channel> deviceCon = theDevice->Open(preserveBoundaries,
				channelName, gotHash ? &thePasswordHash : nullptr, &theError))
				{
				const size_t readSize = deviceCon->GetIdealSize_Read();
				const size_t writeSize = deviceCon->GetIdealSize_Write();
				w.WriteUInt32(ZBlackBerry::Device::error_None);
				w.WriteUInt32(readSize);
				w.WriteUInt32(writeSize);
				w.Flush();

				// Use a standard copier for the device-->client direction
				sCallOnNewThread(
					sBindR(sCallable(&spCopyAllCon), deviceCon, iSRWCon, readSize));

				// And our specialized copier for the client-->device direction.
				sCallOnNewThread(
					sBindR(sCallable(&spCopyChunked), iSRWCon, deviceCon));
				return;
				}
			}

		if (theError == ZBlackBerry::Device::error_None)
			{
			if (ZLOG(s, eDebug + 1, "ZBlackBerryServer::HandleRequest"))
				{
				s << "Open failed, but got error_None";
				}
			theError = ZBlackBerry::Device::error_Generic;
			}
		w.WriteUInt32(theError);
		}
	}

void ZBlackBerryServer::pManagerChanged(ZRef<ZBlackBerry::Manager> iManager)
	{
	// Hmmm. Deadlock?
	ZGuardRMtxR locker(fMutex);

	vector<uint64> theIDs;
	fManager->GetDeviceIDs(theIDs);

	for (vector<uint64>::iterator iterID = theIDs.begin(); iterID != theIDs.end(); ++iterID)
		{
		const uint64 curID = *iterID;
		bool foundIt = false;
		for (vector<Entry_t>::iterator i = fEntries.begin();
			!foundIt && i != fEntries.end(); ++i)
			{
			if (i->fID == curID)
				foundIt = true;
			}

		if (!foundIt)
			{
			if (ZRef<ZBlackBerry::Device> theDevice = fManager->Open(curID))
				{
				Entry_t theEntry;
				theEntry.fLive = true;
				theEntry.fID = curID;
				theEntry.fDevice = theDevice;
				fEntries.push_back(theEntry);
				theDevice->SetCallable(fCB_DeviceFinished);
				}
			}
		}

	for (vector<ZRef<Handler_ManagerChanged> >::iterator i = fHandlers_ManagerChanged.begin();
		i != fHandlers_ManagerChanged.end(); ++i)
		{
		(*i)->TripIt();
		}
	}

void ZBlackBerryServer::pDeviceFinished(ZRef<ZBlackBerry::Device> iDevice)
	{
	ZGuardRMtxR locker(fMutex);
	for (vector<Entry_t>::iterator i = fEntries.begin();
		i != fEntries.end(); ++i)
		{
		if (i->fDevice == iDevice)
			{
			i->fLive = false;
			iDevice->SetCallable(null);

			if (i->fHandlers.empty())
				{
				fEntries.erase(i);
				}
			else
				{
				for (vector<ZRef<Handler_DeviceFinished> >::iterator j = i->fHandlers.begin();
					j != i->fHandlers.end(); ++j)
					{
					(*j)->TripIt();
					}
				}
			break;
			}
		}
	}

ZRef<ZBlackBerry::Device> ZBlackBerryServer::pGetDevice(uint64 iDeviceID)
	{
	ZGuardRMtxR locker(fMutex);

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		{
		if (i->fLive && i->fID == iDeviceID)
			return i->fDevice;
		}
	return null;
	}

void ZBlackBerryServer::pRemove_ManagerChanged(ZRef<Handler_ManagerChanged> iHandler)
	{
	ZGuardRMtxR locker(fMutex);
	ZUtil_STL::sEraseMustContain(1, fHandlers_ManagerChanged, iHandler);
	}

void ZBlackBerryServer::pRemove_DeviceFinished(ZRef<Handler_DeviceFinished> iHandler)
	{
	ZGuardRMtxR locker(fMutex);
	for (vector<Entry_t>::iterator i = fEntries.begin();
		i != fEntries.end(); ++i)
		{
		if (ZUtil_STL::sEraseIfContains(i->fHandlers, iHandler))
			{
			if (i->fHandlers.empty() && !i->fLive)
				fEntries.erase(i);
			break;
			}
		}
	}

} // namespace ZooLib
