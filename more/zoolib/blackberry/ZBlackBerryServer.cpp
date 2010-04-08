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

#include "zoolib/ZCommer.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZStreamerCopier.h"
#include "zoolib/ZUtil_STL.h"

using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerryServer::Handler_ManagerChanged

class ZBlackBerryServer::Handler_ManagerChanged : public ZCommer
	{
public:
	Handler_ManagerChanged(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
		ZBlackBerryServer* iServer);
	virtual ~Handler_ManagerChanged();

// From ZCommer
	virtual bool Read(const ZStreamR& r);
	virtual bool Write(const ZStreamW& w);

	virtual void Detached();

// Called by ZBlackBerryServer
	void TripIt();

private:
	ZBlackBerryServer* fServer;
	ZMutex fMutex;
	enum EState
		{ eState_Quiet, eState_Changed, eState_Waiting, eState_SendChanged, eState_SendClosed};
	EState fState;
	};

ZBlackBerryServer::Handler_ManagerChanged::Handler_ManagerChanged(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
	ZBlackBerryServer* iServer)
:	ZCommer(iStreamerR, iStreamerW),
	fServer(iServer),
	fMutex("ZBlackBerryServer::Handler_ManagerChanged::fMutex"),
	fState(eState_Quiet)
	{}

ZBlackBerryServer::Handler_ManagerChanged::~Handler_ManagerChanged()
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_ManagerChanged~"))
		{
		s << "~Handler_ManagerChanged";
		}
	}

bool ZBlackBerryServer::Handler_ManagerChanged::Read(const ZStreamR& r)
	{
	const bool req = r.ReadBool();

	ZMutexLocker locker(fMutex);
	if (!req)
		{
		fState = eState_SendClosed;
		locker.Release();
		ZStreamerWriter::Wake();
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
			ZStreamerWriter::Wake();
			return true;
			}
		}

	ZUnimplemented();
	return false;
	}

bool ZBlackBerryServer::Handler_ManagerChanged::Write(const ZStreamW& w)
	{
	ZMutexLocker locker(fMutex);

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

void ZBlackBerryServer::Handler_ManagerChanged::Detached()
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_ManagerChanged"))
		{
		s << "Detached";
		}

	fServer->pRemove_ManagerChanged(this);

	delete this;
	}

void ZBlackBerryServer::Handler_ManagerChanged::TripIt()
	{
	ZMutexLocker locker(fMutex);

	if (fState == eState_Waiting)
		fState = eState_SendChanged;
	else if (fState == eState_Quiet)
		fState = eState_Changed;

	locker.Release();

	ZStreamerWriter::Wake();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerryServer::Handler_DeviceFinished

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

	virtual void Detached();

// Called by ZBlackBerryServer
	void TripIt();

private:
	ZBlackBerryServer* fServer;
	bool fClientOpen;
	bool fRunning;
	};

ZBlackBerryServer::Handler_DeviceFinished::Handler_DeviceFinished(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
	ZBlackBerryServer* iServer)
:	ZCommer(iStreamerR, iStreamerW),
	fServer(iServer),
	fClientOpen(true),
	fRunning(true)
	{}

ZBlackBerryServer::Handler_DeviceFinished::~Handler_DeviceFinished()
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_DeviceFinished"))
		{
		s << "~Handler_DeviceFinished";
		}
	}

bool ZBlackBerryServer::Handler_DeviceFinished::Read(const ZStreamR& r)
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_DeviceFinished"))
		{
		s << "Read, entered";
		}

	const bool req = r.ReadBool();
	ZAssert(!req);

	if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_DeviceFinished"))
		{
		s << "Read, got false";
		}

	ZAssert(fClientOpen);
	fClientOpen = false;
	ZStreamerWriter::Wake();
	return false;
	}

bool ZBlackBerryServer::Handler_DeviceFinished::Write(const ZStreamW& w)
	{
	if (!fClientOpen || !fRunning)
		{
		if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_DeviceFinished"))
			{
			s << "Write false, return false";
			}

		w.WriteBool(false);
		return false;
		}

	if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_DeviceFinished"))
		{
		s << "Write, Return true";
		}

	return true;
	}

void ZBlackBerryServer::Handler_DeviceFinished::Detached()
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_DeviceFinished"))
		{
		s << "Detached";
		}

	fServer->pRemove_DeviceFinished(this);

	delete this;
	}

void ZBlackBerryServer::Handler_DeviceFinished::TripIt()
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerryServer::Handler_DeviceFinished"))
		{
		s << "TripIt";
		}
	fRunning = false;
	ZStreamerWriter::Wake();
	}

// =================================================================================================
#pragma mark -
#pragma mark * StreamCopier_Chunked

namespace ZANONYMOUS {

class StreamerCopier_Chunked : public ZWorker
	{
public:
	StreamerCopier_Chunked(ZRef<ZStreamerRCon> iStreamerRCon, ZRef<ZStreamerWCon> iStreamerWCon);

	~StreamerCopier_Chunked();

// From ZWorker
	virtual bool Work();

private:
	ZRef<ZStreamerRCon> fStreamerRCon;
	ZRef<ZStreamerWCon> fStreamerWCon;
	};

StreamerCopier_Chunked::StreamerCopier_Chunked(
	ZRef<ZStreamerRCon> iStreamerRCon, ZRef<ZStreamerWCon> iStreamerWCon)
:	fStreamerRCon(iStreamerRCon),
	fStreamerWCon(iStreamerWCon)
	{}

StreamerCopier_Chunked::~StreamerCopier_Chunked()
	{}

bool StreamerCopier_Chunked::Work()
	{
	ZWorker::Wake();//##
	const ZStreamRCon& r = fStreamerRCon->GetStreamRCon();
	const ZStreamWCon& w = fStreamerWCon->GetStreamWCon();

	try
		{
		const size_t theSize = r.ReadUInt16LE();

		if (theSize == 0)
			{
			// End of stream
			r.ReceiveDisconnect(-1);
			w.SendDisconnect();
			return false;
			}
		else
			{
			std::vector<char> buffer(theSize);

			r.Read(&buffer[0], theSize);
			w.Write(&buffer[0], theSize);

			return true;
			}
		}
	catch (...)
		{
		r.Abort();
		w.Abort();
		throw;
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerryServer

static string spReadString(const ZStreamR& r)
	{
	if (size_t theLength = r.ReadCount())
		return r.ReadString(theLength);
	return string();
	}

ZBlackBerryServer::ZBlackBerryServer(ZRef<ZBlackBerry::Manager> iManager)
:	fManager(iManager)
	{
	fManager->ObserverAdd(this);
	this->ManagerChanged(fManager);
	}

ZBlackBerryServer::~ZBlackBerryServer()
	{
	ZAssert(fHandlers_ManagerChanged.empty());

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		ZAssert(i->fHandlers.empty());

	fManager->ObserverRemove(this);
	}

void ZBlackBerryServer::HandleRequest(ZRef<ZStreamerRWCon> iSRWCon)
	{
	const ZStreamR& r = iSRWCon->GetStreamR();
	const ZStreamW& w = iSRWCon->GetStreamW();

	const int req = r.ReadUInt8();
	if (req == 0)
		{		
		// Async changed notifications
		ZMutexLocker locker(fMutex);
		Handler_ManagerChanged* theHandler = new Handler_ManagerChanged(iSRWCon, iSRWCon, this);
		fHandlers_ManagerChanged.push_back(theHandler);
		locker.Release();

		sStartCommerRunners(theHandler);
		}
	else if (req == 1)
		{
		// Synchronous get device IDs
		ZMutexLocker locker(fMutex);

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

		ZMutexLocker locker(fMutex);

		bool gotIt = false;
		for (vector<Entry_t>::iterator i = fEntries.begin(); !gotIt && i != fEntries.end(); ++i)
			{
			if (i->fLive && i->fID == deviceID)
				{
				Handler_DeviceFinished* theHandler =
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
			const ZBlackBerry::Data theMB = theDevice->GetAttribute(object, attribute);
			w.WriteBool(true);
			w.WriteCount(theMB.GetSize());
			w.Write(theMB.GetData(), theMB.GetSize());
			}
		else
			{
			w.WriteBool(false);
			}
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
				ZRef<ZWorker> deviceToClient =
					new ZStreamerCopier(ZRef<ZTaskMaster>(), deviceCon, iSRWCon, readSize);
				sStartWorkerRunner(deviceToClient);

				// And our specialized copier for the client-->device direction.
				ZRef<ZWorker> clientToDevice =
					new StreamerCopier_Chunked(iSRWCon, deviceCon);
				sStartWorkerRunner(clientToDevice);

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

ZRef<ZBlackBerry::Device> ZBlackBerryServer::pGetDevice(uint64 iDeviceID)
	{
	ZMutexLocker locker(fMutex);

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		{
		if (i->fLive && i->fID == iDeviceID)
			return i->fDevice;
		}
	return ZRef<ZBlackBerry::Device>();
	}

void ZBlackBerryServer::ManagerChanged(ZRef<ZBlackBerry::Manager> iManager)
	{
	// Hmmm. Deadlock?
	ZMutexLocker locker(fMutex);

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
				theDevice->ObserverAdd(this);
				}
			}
		}

	for (vector<Handler_ManagerChanged*>::iterator i = fHandlers_ManagerChanged.begin();
		i != fHandlers_ManagerChanged.end(); ++i)
		{
		(*i)->TripIt();
		}
	}

void ZBlackBerryServer::Finished(ZRef<ZBlackBerry::Device> iDevice)
	{
	ZMutexLocker locker(fMutex);
	for (vector<Entry_t>::iterator i = fEntries.begin();
		i != fEntries.end(); ++i)
		{
		if (i->fDevice == iDevice)
			{
			i->fLive = false;
			iDevice->ObserverRemove(this);

			if (i->fHandlers.empty())
				{
				fEntries.erase(i);
				}
			else
				{
				for (vector<Handler_DeviceFinished*>::iterator j = i->fHandlers.begin();
					j != i->fHandlers.end(); ++j)
					{
					(*j)->TripIt();
					}
				}
			break;
			}
		}
	}

void ZBlackBerryServer::pRemove_ManagerChanged(Handler_ManagerChanged* iHandler)
	{
	ZMutexLocker locker(fMutex);
	ZUtil_STL::sEraseMustContain(1, fHandlers_ManagerChanged, iHandler);
	}

void ZBlackBerryServer::pRemove_DeviceFinished(Handler_DeviceFinished* iHandler)
	{
	ZMutexLocker locker(fMutex);
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

NAMESPACE_ZOOLIB_END
