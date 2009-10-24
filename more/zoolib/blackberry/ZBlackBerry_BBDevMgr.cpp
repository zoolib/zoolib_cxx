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

#include "zoolib/blackberry/ZBlackBerry_BBDevMgr.h"

#if ZCONFIG_API_Enabled(BlackBerry_BBDevMgr)

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h" // For ZBlockCopy
#include "zoolib/ZTime.h"
#include "zoolib/ZUnicode.h"

#include <oleauto.h>

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;
using std::wstring;
using namespace ZBlackBerryCOM;
using namespace ZWinCOM;

namespace ZBlackBerry {

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Channel_BBDevMgr

class Channel_BBDevMgr
:	private IChannelEvents,
	public Channel,
	private ZStreamRCon,
	private ZStreamWCon
	{
	friend class Device_BBDevMgr;

public:
	Channel_BBDevMgr(ZRef<Channel>& oChannel, ZRef<IDevice> iDevice,
		bool iPreserveBoundaries, const string& iName, const PasswordHash* iPasswordHash);

	virtual ~Channel_BBDevMgr();

// From IUnknown via IChannelEvents
	virtual STDMETHODIMP QueryInterface(const IID& inInterfaceID, void** outObjectRef);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

// From IChannelEvents
	virtual STDMETHODIMP CheckClientStatus(uint16 iRHS);
	virtual STDMETHODIMP OnChallenge(int32 iAttemptsRemaining, uint8 oPasswordHash[20]);
	virtual STDMETHODIMP OnNewData();
	virtual STDMETHODIMP OnClose();

// From ZBlackBerry::Channel
	virtual size_t GetIdealSize_Read();
	virtual size_t GetIdealSize_Write();

// From ZStreamerR via ZBlackBerry::Channel
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerW via ZBlackBerry::Channel
	virtual const ZStreamWCon& GetStreamWCon();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
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
	bool pRefill(ZMutexLocker& iLocker, ZRef<IChannel> iChannel, double* ioTimeout);
	void pAbort();
	ZRef<IChannel> pUseChannel();

	ZMutex fMutex;
	ZCondition fCondition_Reader;
	ZRef<IChannel> fChannel;
	ChannelParams fChannelParams;

	bool fPreserveBoundaries;

	vector<char> fBuffer;
	size_t fStart;
	size_t fEnd;

	bool fHasPasswordHash;
	PasswordHash fPasswordHash;
	bool fClosed;
	};

Channel_BBDevMgr::Channel_BBDevMgr(ZRef<Channel>& oChannel, ZRef<IDevice> iDevice,
	bool iPreserveBoundaries, const string& iName, const PasswordHash* iPasswordHash)
:	fChannel(nullptr),
	fPreserveBoundaries(iPreserveBoundaries),
	fStart(0),
	fEnd(0),
	fHasPasswordHash(0 != iPasswordHash),
	fClosed(false)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "Channel_BBDevMgr";

	if (iPasswordHash)
		fPasswordHash = *iPasswordHash;

	// The call to OpenChannel will AddRef/Release us. So keep
	// a reference around till we return.
	ZRef<Channel> self = this;
	wstring wideName = ZUnicode::sAsWString(iName);
	HRESULT theResult = iDevice->OpenChannel(wideName.c_str(), this, sCOMPtr(fChannel));

	if (SUCCEEDED(theResult) && fChannel)
		{
		fChannel->Params(&fChannelParams);
		// Allow for two extra bytes, just in case we're preserving boundaries.
		fBuffer.resize(fChannelParams.fMaxReceiveUnit + 2);
		oChannel = self;
		}
	}

Channel_BBDevMgr::~Channel_BBDevMgr()
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "~Channel_BBDevMgr";
	}

STDMETHODIMP Channel_BBDevMgr::QueryInterface(const IID& iInterfaceID, void** oObjectRef)
	{
	if (ZLOG(s, eDebug + 4, "ZBlackBerry::Channel_BBDevMgr"))
		s << "QueryInterface";

	*oObjectRef = nullptr;

	if (iInterfaceID == IID_IUnknown)
		return sCOMCopy<IUnknown>(oObjectRef, this);

	if (iInterfaceID == ZUUIDOF(IChannelEvents))
		return sCOMCopy<IChannelEvents>(oObjectRef, this);

	return E_NOINTERFACE;
	}

ULONG STDMETHODCALLTYPE Channel_BBDevMgr::AddRef()
	{ return this->pCOMAddRef(); }

ULONG STDMETHODCALLTYPE Channel_BBDevMgr::Release()
	{ return this->pCOMRelease(); }

STDMETHODIMP Channel_BBDevMgr::CheckClientStatus(uint16 iRHS)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s.Writef("CheckClientStatus, iRHS: %d", iRHS);
	// Not implemented - what does this do?

	return S_OK;
	}

STDMETHODIMP Channel_BBDevMgr::OnChallenge(int32 iAttemptsRemaining, uint8 oPasswordHash[20])
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "OnChallenge";

	//#warning "not working yet"
	if (false && fHasPasswordHash && iAttemptsRemaining > 6)
		{
		*reinterpret_cast<PasswordHash*>(oPasswordHash) = fPasswordHash;
		return S_OK;
		}
	else
		{
		return E_FAIL;
		}
	}

STDMETHODIMP Channel_BBDevMgr::OnNewData()
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "OnNewData";

	ZMutexLocker locker(fMutex);
	fCondition_Reader.Broadcast();

	return NOERROR;
	}

STDMETHODIMP Channel_BBDevMgr::OnClose()
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "OnClose";

	// Close initiated by device removal or the other side. An abort
	// here seems to deadlock, hence our use of the fClosed flag.

	ZMutexLocker locker(fMutex);
	fClosed = true;
	fCondition_Reader.Broadcast();

	return NOERROR;
	}

size_t Channel_BBDevMgr::GetIdealSize_Read()
	{ return fBuffer.size(); }

size_t Channel_BBDevMgr::GetIdealSize_Write()
	{ return fChannelParams.fMaxTransmitUnit; }

const ZStreamRCon& Channel_BBDevMgr::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& Channel_BBDevMgr::GetStreamWCon()
	{ return *this; }

void Channel_BBDevMgr::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s.Writef("Imp_Read, iCount: %d", iCount);

	uint8* localDest = static_cast<uint8*>(iDest);

	ZMutexLocker locker(fMutex);

	for (;;)
		{
		if (fClosed)
			break;

		if (fEnd > fStart)
			{
			const size_t countToCopy = std::min(iCount, fEnd - fStart);
			ZBlockCopy(&fBuffer[fStart], localDest, countToCopy);
			localDest += countToCopy;
			fStart += countToCopy;
			if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
				s.Writef("Imp_Read, countToCopy: %d", countToCopy);
			break;
			}
		else
			{
			ZRef<IChannel> theChannel = this->pUseChannel();
			if (!theChannel)
				break;

			if (!this->pRefill(locker, theChannel, nullptr))
				break;
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(iDest);
	}

size_t Channel_BBDevMgr::Imp_CountReadable()
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "Imp_CountReadable";

	ZMutexLocker locker(fMutex);
	if (fEnd <= fStart)
		{
		ZRef<IChannel> theChannel = this->pUseChannel();
		if (!theChannel)
			return 0;
		double theTimeout = 0;
		this->pRefill(locker, theChannel, &theTimeout);
		}
		
	return fEnd - fStart;
	}

bool Channel_BBDevMgr::Imp_WaitReadable(double iTimeout)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "Imp_WaitReadable";

	ZMutexLocker locker(fMutex);

	for (;;)
		{
		if (fClosed)
			return true;

		if (fEnd > fStart)
			return true;

		ZRef<IChannel> theChannel = this->pUseChannel();
		if (!theChannel)
			return true;

		if (!this->pRefill(locker, theChannel, &iTimeout))
			return false;
		}
	}

bool Channel_BBDevMgr::Imp_ReceiveDisconnect(double iTimeout)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "Imp_ReceiveDisconnect";

	ZMutexLocker locker(fMutex);

	for (;;)
		{
		if (fClosed)
			return true;

		fEnd = 0;
		fStart = 0;

		ZRef<IChannel> theChannel = this->pUseChannel();
		if (!theChannel)
			return true;

		if (!this->pRefill(locker, theChannel, &iTimeout))
			return true;
		}
	}

void Channel_BBDevMgr::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s.Writef("Imp_Write, iCount: %d", iCount);

	const uint8* localSource = static_cast<const uint8*>(iSource);

	ZMutexLocker locker(fMutex);

	if (ZRef<IChannel> theChannel = this->pUseChannel())
		{
		locker.Release();
		const size_t countToWrite = std::min(iCount, size_t(fChannelParams.fMaxTransmitUnit));

		if (SUCCEEDED(theChannel->WritePacket(localSource, countToWrite)))
			{
			localSource += countToWrite;
			}
		else
			{
			if (ZLOG(s, eDebug + 2, "ZBlackBerry::Channel_BBDevMgr"))
				s.Writef("Write failed, count: %d, %d", iCount, countToWrite);
			}
		}

	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const uint8*>(iSource);
	}

void Channel_BBDevMgr::Imp_SendDisconnect()
	{	
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "Imp_SendDisconnect";

	this->pAbort();
	}

void Channel_BBDevMgr::Imp_Abort()
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "Imp_Abort";

	this->pAbort();
	}

bool Channel_BBDevMgr::pRefill(ZMutexLocker& iLocker, ZRef<IChannel> iChannel, double* ioTimeout)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s.Writef("pRefill");

	ZAssert(iChannel);
	ZAssert(fEnd <= fStart);

	bool result = false;
	for (;;)
		{
		if (fClosed)
			break;

		int32 packetsAvailable;
		if (FAILED(iChannel->PacketsAvailable(&packetsAvailable)))
			{
			break;
			}
		else if (packetsAvailable)
			{
			iLocker.Release();

			int32 countRead;
			if (fPreserveBoundaries)
				{
				if (FAILED(iChannel->ReadPacket(&fBuffer[2], fBuffer.size() - 2, &countRead)))
					{
					countRead = 0;
					}
				else
					{
					ZByteSwap_WriteLittle16(&fBuffer[0], countRead);
					countRead += 2;
					}
				}
			else
				{
				if (FAILED(iChannel->ReadPacket(&fBuffer[0], fBuffer.size(), &countRead)))
					countRead = 0;
				}

			if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
				s.Writef("pRefill, countRead: %d", countRead);

			iLocker.Acquire();
			fStart = 0;
			fEnd = countRead;
			if (countRead)
				{
				result = true;
				break;
				}
			}
		else if (ioTimeout)
			{
			if (!fCondition_Reader.WaitFor(fMutex, *ioTimeout))
				return false;
			}
		else
			{
			fCondition_Reader.WaitFor(fMutex, 1.0);
			}
		}
	return result;
	}

void Channel_BBDevMgr::pAbort()
	{
	ZMutexLocker locker(fMutex);
	fClosed = true;
	fChannel.Clear();
	fCondition_Reader.Broadcast();
	}

ZRef<IChannel> Channel_BBDevMgr::pUseChannel()
	{
	ZAssert(fMutex.IsLocked());
	return fChannel;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Device_BBDevMgr

class Device_BBDevMgr : public Device
	{
public:
	Device_BBDevMgr(ZRef<IDevice> iDevice);
	virtual ~Device_BBDevMgr();

// From ZBlackBerry::Device
	virtual ZRef<Channel> Open(bool iPreserveBoundaries,
		const string& iName, const PasswordHash* iPasswordHash, Error* oError);
	virtual Data GetAttribute(uint16 iObject, uint16 iAttribute);
	virtual uint32 GetPIN();

// Called by Manager_BBDevMgr
	bool Matches(IDevice* iDevice);
	void pDisconnected();

private:
	bool pGetProperty(const string16& iName, VARIANT& oValue);
	ZRef<IDevice> pUseDevice();

	ZMutex fMutex;
	ZRef<IDevice> fDevice;
	};

Device_BBDevMgr::Device_BBDevMgr(ZRef<IDevice> iDevice)
:	fDevice(iDevice)
	{}

Device_BBDevMgr::~Device_BBDevMgr()
	{}

ZRef<Channel> Device_BBDevMgr::Open(bool iPreserveBoundaries,
	const string& iName, const PasswordHash* iPasswordHash, Error* oError)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Device_BBDevMgr"))
		s << "Open, iName: " << iName;

	if (ZRef<IDevice> theDevice = this->pUseDevice())
		{
		// theChannel's refcount is manipulated by both COM and ZRef. When created it is zero,
		// and in the ZRef scheme is not incremented to one until it is first assigned to a
		// ZRef<>. The process of opening a channel, and failing, will AddRef and Release
		// theChannel, and it will be Finalized and thus disposed. So we pass a reference to a
		// ZRef to the constructor, to which the constructor assigns 'this', extending the
		// lifetime appropriately.
		ZRef<Channel> theChannel;
		new Channel_BBDevMgr(theChannel, theDevice, iPreserveBoundaries, iName, iPasswordHash);

		if (theChannel)
			return theChannel;

		// FIXME. Failure may also be due to a bad/missing/expired password.
		//#warning "NDY"
		if (oError)
			*oError = error_UnknownChannel;
		}
	else
		{
		if (oError)
			*oError = error_DeviceClosed;
		}

	return ZRef<Channel>();
	}

Data Device_BBDevMgr::GetAttribute(uint16 iObject, uint16 iAttribute)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Device_BBDevMgr"))
		s << "GetAttribute";

	return Data();
	}

uint32 Device_BBDevMgr::GetPIN()
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Device_BBDevMgr"))
		s << "GetPIN";

	Variant thePIN;
	if (this->pGetProperty(ZUnicode::sAsUTF16("BBPIN"), thePIN))
		return thePIN.GetInt32();

	return 0;
	}

bool Device_BBDevMgr::Matches(IDevice* iDevice)
	{
	if (ZRef<IDevice> theDevice = this->pUseDevice())
		{
		int32 result;
		if (SUCCEEDED(theDevice->Equals(iDevice, &result)))
			return result == 1;
		}

	return false;
	}

void Device_BBDevMgr::pDisconnected()
	{
	ZMutexLocker locker(fMutex);
	fDevice.Clear();
	this->pFinished();
	}

bool Device_BBDevMgr::pGetProperty(const string16& iName, VARIANT& oValue)
	{
	if (ZRef<IDevice> theDevice = this->pUseDevice())
		{
		ZRef<IDeviceProperties> theDPs;
		if (SUCCEEDED(theDevice->Properties(sCOMPtr(theDPs))))
			{
			uint32 dpCount;
			theDPs->Count(&dpCount);
			for (uint32 x = 0; x < dpCount; ++x)
				{
				ZRef<IDeviceProperty> theDP;
				if (SUCCEEDED(theDPs->Item(Variant(x), sCOMPtr(theDP))))
					{
					ZWinCOM::String theName;
					if (SUCCEEDED(theDP->Name(sCOMPtr(theName))))
						{
						if (iName == theName)
							{
							if (SUCCEEDED(theDP->Value(&oValue)))
								return true;
							}	
						}
					}
				}
			}
		}
	return false;
	}

ZRef<IDevice> Device_BBDevMgr::pUseDevice()
	{
	ZMutexLocker locker(fMutex);
	return fDevice;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Manager_BBDevMgr

Manager_BBDevMgr::Manager_BBDevMgr()
:	fMutex("Manager_BBDevMgr::fMutex"),
	fNextID(1)
	{
	HRESULT theHRESULT = ::CoCreateInstance(
		IDeviceManager::sCLSID,
		nullptr,
		CLSCTX_ALL,
		ZUUIDOF(IDeviceManager),
		sCOMVoidPtr(fDeviceManager));

	printf("%08X", theHRESULT);

	if (fDeviceManager)
		{
		fDeviceManager->Advise(this, &fCookie);

		// Build the initial list
		ZMutexLocker locker(fMutex);

		ZRef<IDevices> theDevices;
		if (SUCCEEDED(fDeviceManager->Devices(sCOMPtr(theDevices)) && theDevices))
			{
			uint32 theCount;
			theDevices->Count(&theCount);

			for (uint32 x = 0; x < theCount; ++x)
				{
				ZRef<IDevice> theDevice;
				if (SUCCEEDED(theDevices->Item(x, sCOMPtr(theDevice))))
					{
					Entry_t anEntry;
					anEntry.fID = fNextID++;
					anEntry.fDevice = new Device_BBDevMgr(theDevice);
					fEntries.push_back(anEntry);
					}
				}
			}
		}	
	}

Manager_BBDevMgr::~Manager_BBDevMgr()
	{
	fEntries.clear();

	if (fDeviceManager)
		fDeviceManager->Unadvise(fCookie);
	}

STDMETHODIMP Manager_BBDevMgr::QueryInterface(
	const IID& iInterfaceID, void** oObjectRef)
	{
	if (ZLOG(s, eDebug + 4, "ZBlackBerry::Manager_BBDevMgr"))
		s << "QueryInterface";

	*oObjectRef = nullptr;

	if (iInterfaceID == IID_IUnknown)
		return sCOMCopy<IUnknown>(oObjectRef, this);

	if (iInterfaceID == ZUUIDOF(IDeviceManagerEvents))
		return sCOMCopy<IDeviceManagerEvents>(oObjectRef, this);

	return E_NOINTERFACE;
	}

ULONG STDMETHODCALLTYPE Manager_BBDevMgr::AddRef()
	{ return ZRefCountedWithFinalize::pCOMAddRef(); }

ULONG STDMETHODCALLTYPE Manager_BBDevMgr::Release()
	{ return ZRefCountedWithFinalize::pCOMRelease(); }

STDMETHODIMP Manager_BBDevMgr::DeviceConnect(IDevice* iDevice)
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager_BBDevMgr"))
		s.Writef("DeviceConnect, iDevice: %08X", iDevice);

	ZMutexLocker locker(fMutex);

	Entry_t anEntry;
	anEntry.fID = fNextID++;
	anEntry.fDevice = new Device_BBDevMgr(iDevice);
	fEntries.push_back(anEntry);

	locker.Release();

	this->pNotifyObservers();

	return S_OK;
	}

STDMETHODIMP Manager_BBDevMgr::DeviceDisconnect(IDevice* iDevice)
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager_BBDevMgr"))
		s.Writef("DeviceDisconnect, iDevice: %08X", iDevice);

	ZMutexLocker locker(fMutex);

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); /*no inc*/)
		{
		if (! i->fDevice->Matches(iDevice))
			{
			++i;
			}
		else
			{
			i->fDevice->pDisconnected();
			i = fEntries.erase(i);
			}
		}

	locker.Release();

	this->pNotifyObservers();
	return S_OK;
	}

void Manager_BBDevMgr::GetDeviceIDs(vector<uint64>& oDeviceIDs)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Manager_BBDevMgr"))
		s << "GetDeviceIDs";

	ZMutexLocker locker(fMutex);

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		oDeviceIDs.push_back(i->fID);
	}

ZRef<Device> Manager_BBDevMgr::Open(uint64 iDeviceID)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Manager_BBDevMgr"))
		s << "Open";

	ZMutexLocker locker(fMutex);

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		{
		if (i->fID == iDeviceID)
			return i->fDevice;;
		}

	return ZRef<Device>();
	}

} // namespace ZBlackBerry

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(BlackBerry_BBDevMgr)
