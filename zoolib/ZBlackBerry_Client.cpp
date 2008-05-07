static const char rcsid[] =
"@(#) $Id: ZBlackBerry_Client.cpp,v 1.5 2008/03/27 00:14:47 agreen Exp $";

/* ------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZBlackBerry_Client.h"

#include "ZLog.h"

namespace ZBlackBerry {

// =================================================================================================
#pragma mark -
#pragma mark * Channel_Client

class Channel_Client : public Channel
	{
public:
	Channel_Client(ZRef<ZStreamerRWCon> iSRWCon);
	virtual ~Channel_Client();

// From ZStreamerRCon via Channel
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via Channel
	virtual const ZStreamWCon& GetStreamWCon();

private:
	ZRef<ZStreamerRWCon> fSRWCon;
	};

Channel_Client::Channel_Client(ZRef<ZStreamerRWCon> iSRWCon)
:	fSRWCon(iSRWCon)
	{}
	
Channel_Client::~Channel_Client()
	{}

const ZStreamRCon& Channel_Client::GetStreamRCon()
	{ return fSRWCon->GetStreamRCon(); }

const ZStreamWCon& Channel_Client::GetStreamWCon()
	{ return fSRWCon->GetStreamWCon(); }

// =================================================================================================
#pragma mark -
#pragma mark * Device_Client

class Device_Client
:	public Device,
	private ZCommer
	{
public:
	Device_Client(ZRef<ZStreamerRWCon> iSRWCon,
		ZRef<ZStreamerRWConFactory> iFactory, uint64 iDeviceID);

	virtual ~Device_Client();

// From Device
	virtual void Stop();

	virtual ZRef<Channel> Open(
		const string& iName, const PasswordHash* iPasswordHash, Error* oError);
	virtual ZMemoryBlock GetAttribute(uint16 iObject, uint16 iAttribute);

// From ZStreamReader via ZCommer
	virtual void RunnerDetached(ZStreamReaderRunner* iRunner);

// From ZCommer
	virtual bool Read(const ZStreamR& r);
	virtual bool Write(const ZStreamW& w);

	virtual void Detached();

private:
	ZRef<ZStreamerRWConFactory> fFactory;
	uint64 fDeviceID;
	bool fOpen;
	};

Device_Client::Device_Client(ZRef<ZStreamerRWCon> theSRWCon,
	ZRef<ZStreamerRWConFactory> iFactory, uint64 iDeviceID)
:	fFactory(iFactory),
	fDeviceID(iDeviceID),
	fOpen(true)
	{
	sStartRunners(this, theSRWCon, theSRWCon);	
	}

Device_Client::~Device_Client()
	{
	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug + 2, "ZBlackBerry::Device_Client"))
		s << "~Device_Client";
	}

static void sWriteString(const ZStreamW& w, const string& iString)
	{
	if (size_t theSize = iString.size())
		{
		w.WriteCount(theSize);
		w.WriteString(iString);
		}
	}

void Device_Client::Stop()
	{
	fOpen = false;
	this->Wake();
	}

ZRef<Channel> Device_Client::Open(
	const string& iName, const PasswordHash* iPasswordHash, Error* oError)
	{
	try
		{
		if (ZRef<ZStreamerRWCon> theSRWCon = fFactory->MakeStreamerRWCon())
			{
			const ZStreamW& w = theSRWCon->GetStreamW();
			w.WriteUInt8(3);
			w.WriteUInt64(fDeviceID);
			if (iPasswordHash)
				{
				w.WriteBool(true);
				w.Write(iPasswordHash, sizeof(PasswordHash));
				}
			else
				{
				w.WriteBool(false);
				}
			sWriteString(w, iName);
			w.Flush();

			const ZStreamR& r = theSRWCon->GetStreamR();

			Error theError = static_cast<Error>(r.ReadUInt32());

			if (oError)
				*oError = theError;

			if (theError == error_None)
				return new Channel_Client(theSRWCon);
			}
		}
	catch (...)
		{}

	return ZRef<Channel>();
	}

ZMemoryBlock Device_Client::GetAttribute(uint16 iObject, uint16 iAttribute)
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
				ZMemoryBlock theMB(r.ReadCount());
				r.Read(theMB.GetData(), theMB.GetSize());
				return theMB;
				}
			}
		}
	catch (...)
		{}
	return ZMemoryBlock();
	}

void Device_Client::RunnerDetached(ZStreamReaderRunner* iRunner)
	{
	ZCommer::RunnerDetached(iRunner);
	fOpen = false;
	this->Wake();
	}

bool Device_Client::Read(const ZStreamR& r)
	{
	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug + 2, "ZBlackBerry::Device_Client"))
		s << "Read, entered";
	const bool req = r.ReadBool();
	ZAssert(!req);
	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug + 2, "ZBlackBerry::Device_Client"))
		s << "Read, got false";
	return false;
	}

bool Device_Client::Write(const ZStreamW& w)
	{
	if (!fOpen)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug + 2, "ZBlackBerry::Device_Client"))
			s << "Write false, return false";
		w.WriteBool(false);
		return false;
		}
	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug + 2, "ZBlackBerry::Device_Client"))
		s << "Write, Return true";
	return true;
	}

void Device_Client::Detached()
	{
	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug + 2, "ZBlackBerry::Device_Client"))
		s << "Detached";
	this->pFinished();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Manager_Client

Manager_Client::Manager_Client()
	{}

Manager_Client::Manager_Client(ZRef<ZStreamerRWConFactory> iFactory)
:	fFactory(iFactory),
	fSendNotificationRequest(true),
	fSendClose(false)
	{}

Manager_Client::~Manager_Client()
	{}

void Manager_Client::Start()
	{
	ZRef<ZStreamerRWCon> theSRWCon = fFactory->MakeStreamerRWCon();
	if (!theSRWCon)
		throw runtime_error("ZBlackBerry::Manager_Client, Couldn't connect");

	this->pStarted();

	const ZStreamW& w = theSRWCon->GetStreamW();
	w.WriteUInt8(0);

	sStartRunners(this, theSRWCon, theSRWCon);
	}

void Manager_Client::Stop()
	{
	fSendClose = true;
	this->Wake();
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
			oDeviceIDs.reserve(theCount);

			for (size_t x = 0; x < theCount; ++x)
				oDeviceIDs.push_back(r.ReadUInt64());
			}
		}
	catch (...)
		{
		oDeviceIDs.clear();
		}
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
	return ZRef<Device>();
	}

void Manager_Client::RunnerDetached(ZStreamReaderRunner* iRunner)
	{
	ZCommer::RunnerDetached(iRunner);
	// Force the writer to attempt to close, and thus to exit.
	fSendClose = true;
	this->Wake();
	}

bool Manager_Client::Read(const ZStreamR& r)
	{
	const bool req = r.ReadBool();
	if (req)
		{
		this->pNotifyObservers();
		fSendNotificationRequest = true;
		this->Wake();
		return true;		
		}
	else
		{
		return false;
		}
	}

bool Manager_Client::Write(const ZStreamW& w)
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

void Manager_Client::Detached()
	{
	this->pStopped();
	}

} // namespace ZBlackBerry
