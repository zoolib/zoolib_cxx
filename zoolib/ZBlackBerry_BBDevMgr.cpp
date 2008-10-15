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

#include "zoolib/ZBlackBerry_BBDevMgr.h"

#if ZCONFIG_API_Enabled(BlackBerry_BBDevMgr)

#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h" // For ZBlockCopy
#include "zoolib/ZTime.h"

#include <oleauto.h>

using std::string;
using std::vector;
using std::wstring;

namespace ZBlackBerry {

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Channel_BBDevMgr

class Channel_BBDevMgr
:	private ZBlackBerryCOM::IChannelEvents,
	public ZBlackBerry::Channel,
	private ZStreamRCon,
	private ZStreamWCon
	{
	friend class Device_BBDevMgr;

public:
	typedef ZBlackBerryCOM::IDevice IDevice;
	typedef ZBlackBerryCOM::IChannel IChannel;
	typedef ZBlackBerryCOM::ChannelParams ChannelParams;
	typedef ZBlackBerryCOM::IChannelEvents IChannelEvents;

	Channel_BBDevMgr(ZRef<Channel_BBDevMgr>& oChannel, IDevice* iDevice,
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

// From ZStreamRCon
	virtual bool Imp_WaitReadable(int iMilliseconds);
	virtual bool Imp_ReceiveDisconnect(int iMilliseconds);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

// Our protocol
	bool IsOkay();

private:
	bool pRefill(ZMutexLocker& iLocker, IChannel* iChannel, int* ioTimeout);
	void pAbort();
	IChannel* pUseChannel();

	ZMutex fMutex;
	ZCondition fCondition_Reader;
	IChannel* fChannel;
	ChannelParams fChannelParams;

	vector<char> fBuffer;
	size_t fStart;
	size_t fEnd;

	bool fHasPasswordHash;
	PasswordHash fPasswordHash;
	bool fClosed;
	};

Channel_BBDevMgr::Channel_BBDevMgr(ZRef<Channel_BBDevMgr>& oChannel, IDevice* iDevice,
	bool iPreserveBoundaries, const string& iName, const PasswordHash* iPasswordHash)
:	fChannel(nil),
	fStart(0),
	fEnd(0),
	fHasPasswordHash(0 != iPasswordHash),
	fClosed(false)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "Channel_BBDevMgr";

	if (iPasswordHash)
		fPasswordHash = *iPasswordHash;

	oChannel = this;
	wstring wideName = ZUnicode::sAsWString(iName);
	HRESULT theResult = iDevice->OpenChannel(wideName.c_str(), this, &fChannel);

	if (SUCCEEDED(theResult) && fChannel)
		{
		fChannel->Params(&fChannelParams);
		fBuffer.resize(fChannelParams.fMaxReceiveUnit);
		}
	}

Channel_BBDevMgr::~Channel_BBDevMgr()
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s << "~Channel_BBDevMgr";

	if (fChannel)
		fChannel->Release();
	}

STDMETHODIMP Channel_BBDevMgr::QueryInterface(const IID& iInterfaceID, void** oObjectRef)
	{
	if (ZLOG(s, eDebug + 4, "ZBlackBerry::Channel_BBDevMgr"))
		s << "QueryInterface";

	*oObjectRef = nil;
	if (iInterfaceID == IID_IUnknown)
		{
		*oObjectRef = static_cast<IUnknown*>(this);
		this->AddRef();
		return NOERROR;
		}

	if (iInterfaceID == ZUUIDOF(IChannelEvents))
		{
		*oObjectRef = static_cast<IChannelEvents*>(this);
		this->AddRef();
		return NOERROR;
		}

	return E_NOINTERFACE;
	}

ULONG STDMETHODCALLTYPE Channel_BBDevMgr::AddRef()
	{ return ZRefCountedWithFinalization::AddRef(); }

ULONG STDMETHODCALLTYPE Channel_BBDevMgr::Release()
	{ return ZRefCountedWithFinalization::Release(); }

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

	#warning not working yet
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
			IChannel* theChannel = this->pUseChannel();
			if (!theChannel)
				break;

			if (!this->pRefill(locker, theChannel, nil))
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
		IChannel* theChannel = this->pUseChannel();
		if (!theChannel)
			return 0;
		int theTimeout = 0;
		this->pRefill(locker, theChannel, &theTimeout);
		}
		
	return fEnd - fStart;
	}

bool Channel_BBDevMgr::Imp_WaitReadable(int iMilliseconds)
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

		IChannel* theChannel = this->pUseChannel();
		if (!theChannel)
			return true;

		if (!this->pRefill(locker, theChannel, &iMilliseconds))
			return false;
		}
	}

bool Channel_BBDevMgr::Imp_ReceiveDisconnect(int iMilliseconds)
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

		IChannel* theChannel = this->pUseChannel();
		if (!theChannel)
			return true;

		if (!this->pRefill(locker, theChannel, &iMilliseconds))
			return true;
		}
	}

void Channel_BBDevMgr::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
		s.Writef("Imp_Write, iCount: %d", iCount);

	const uint8* localSource = static_cast<const uint8*>(iSource);

	ZMutexLocker locker(fMutex);

	if (IChannel* theChannel = this->pUseChannel())
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

		theChannel->Release();
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

bool Channel_BBDevMgr::IsOkay()
	{ return fChannel; }

bool Channel_BBDevMgr::pRefill(ZMutexLocker& iLocker, IChannel* iChannel, int* ioTimeout)
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
			if (FAILED(iChannel->ReadPacket(&fBuffer[0], fBuffer.size(), &countRead)))
				countRead = 0;

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
		else if (!ioTimeout)
			{
			if (ZThread::errorTimeout == fCondition_Reader.Wait(fMutex, 1000000))
				{
				if (ZLOG(s, eDebug + 3, "ZBlackBerry::Channel_BBDevMgr"))
					s.Writef("pRefill, timed out, looping");
				continue;
				}
			}
		else
			{
			const ZTime start = ZTime::sSystem();
			fCondition_Reader.Wait(fMutex, *ioTimeout * 1000);
			*ioTimeout -= int(1000 * (ZTime::sSystem() - start));
			if (*ioTimeout <= 0)
				{
				*ioTimeout = 0;
				break;
				}
			}
		}
	iLocker.Release();
	ZAssert(!fMutex.IsLocked());
	iChannel->Release();
	iLocker.Acquire();
	return result;
	}

void Channel_BBDevMgr::pAbort()
	{
	ZMutexLocker locker(fMutex);
	if (fChannel)
		{
		fClosed = true;
		IChannel* theChannel = fChannel;
		fChannel = nil;
		fCondition_Reader.Broadcast();
		locker.Release();
		theChannel->Release();
		}
	}

ZBlackBerryCOM::IChannel* Channel_BBDevMgr::pUseChannel()
	{
	ZAssert(fMutex.IsLocked());

	if (IChannel* theChannel = fChannel)
		{
		theChannel->AddRef();
		return theChannel;
		}
	return nil;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Device_BBDevMgr

class Device_BBDevMgr : public Device
	{
public:
	typedef ZBlackBerryCOM::IDevice IDevice;

	Device_BBDevMgr(IDevice* iDevice);
	virtual ~Device_BBDevMgr();

// From ZBlackBerry::Device
	virtual ZRef<Channel> Open(bool iPreserveBoundaries,
		const string& iName, const PasswordHash* iPasswordHash, Error* oError);
	virtual ZMemoryBlock GetAttribute(uint16 iObject, uint16 iAttribute);
	virtual uint32 GetPIN();

// Called by Manager_BBDevMgr
	bool Matches(IDevice* iDevice);
	void pDisconnected();

private:
	bool pGetProperty(const string16& iName, VARIANT& oValue);
	IDevice* pUseDevice();

	ZMutex fMutex;
	IDevice* fDevice;
	};

Device_BBDevMgr::Device_BBDevMgr(IDevice* iDevice)
:	fDevice(iDevice)
	{
	ZAssert(fDevice);
	fDevice->AddRef();
	}

Device_BBDevMgr::~Device_BBDevMgr()
	{
	if (fDevice)
		fDevice->Release();
	}

ZRef<Channel> Device_BBDevMgr::Open(bool iPreserveBoundaries,
	const string& iName, const PasswordHash* iPasswordHash, Error* oError)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Device_BBDevMgr"))
		s << "Open, iName: " << iName;

	if (iPreserveBoundaries)
		{
		if (oError)
			*oError = error_Generic;
		}
	else
		{
		if (IDevice* theDevice = this->pUseDevice())
			{
			// theChannel's refcount is manipulated by both COM and ZRef. When created it is zero,
			// and in the ZRef scheme is not incremented to one until it is first assigned to a
			// ZRef<>. The process of opening a channel, and failing, will AddRef and Release
			// theChannel, and it will be Finalized and thus disposed. So we pass a reference to a
			// ZRef to the constructor, to which the constructor assigns 'this', extending the
			// lifetime appropriately.
			ZRef<Channel_BBDevMgr> theChannel;
			new Channel_BBDevMgr(theChannel, theDevice, iPreserveBoundaries, iName, iPasswordHash);
			theDevice->Release();

			if (theChannel->IsOkay())
				return theChannel;

			// FIXME. Failure may also be due to a bad/missing/expired password.
			#warning NDY
			if (oError)
				*oError = error_UnknownChannel;
			}
		else
			{
			if (oError)
				*oError = error_DeviceClosed;
			}
		}

	return ZRef<Channel>();
	}

ZMemoryBlock Device_BBDevMgr::GetAttribute(uint16 iObject, uint16 iAttribute)
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Device_BBDevMgr"))
		s << "GetAttribute";

	return ZMemoryBlock();
	}

uint32 Device_BBDevMgr::GetPIN()
	{
	if (ZLOG(s, eDebug + 3, "ZBlackBerry::Device_BBDevMgr"))
		s << "GetPIN";

	VARIANT theV;
	::VariantInit(&theV);
	uint32 thePIN = 0;
	if (this->pGetProperty(ZUnicode::sAsUTF16("BBPIN"), theV))
		{
		if (VT_I4 == theV.vt)
			thePIN = theV.lVal;
		}
	::VariantClear(&theV);
	return thePIN;
	}

bool Device_BBDevMgr::Matches(IDevice* iDevice)
	{
	int32 result = 0;
	if (IDevice* theDevice = this->pUseDevice())
		{
		if (FAILED(theDevice->Equals(iDevice, &result)))
			result = 0;

		theDevice->Release();
		}

	return result == 1;
	}

void Device_BBDevMgr::pDisconnected()
	{
	ZMutexLocker locker(fMutex);
	ZAssert(fDevice);
	IDevice* theDevice = fDevice;
	fDevice = nil;
	locker.Release();

	theDevice->Release();

	this->pFinished();
	}

bool Device_BBDevMgr::pGetProperty(const string16& iName, VARIANT& oValue)
	{
	bool gotIt = false;
	if (IDevice* theDevice = this->pUseDevice())
		{
		ZBlackBerryCOM::IDeviceProperties* theDPs;
		if (SUCCEEDED(theDevice->Properties(&theDPs)))
			{
			uint32 dpCount;
			theDPs->Count(&dpCount);
			for (uint32 x = 0; x < dpCount && !gotIt; ++x)
				{
				VARIANT indexV;
				indexV.vt = VT_I4;
				indexV.lVal = x;
				ZBlackBerryCOM::IDeviceProperty* theDP;
				if (SUCCEEDED(theDPs->Item(indexV, &theDP)))
					{
					BSTR theName;
					if (SUCCEEDED(theDP->Name(&theName)))
						{
						if (iName == theName)
							{
							if (SUCCEEDED(theDP->Value(&oValue)))
								gotIt = true;
							}	
						::SysFreeString(theName);
						}
					theDP->Release();
					}
				}
			theDPs->Release();
			}
		}
	return gotIt;
	}

ZBlackBerryCOM::IDevice* Device_BBDevMgr::pUseDevice()
	{
	ZMutexLocker locker(fMutex);
	if (IDevice* theDevice = fDevice)
		{
		theDevice->AddRef();
		return theDevice;
		}
	return nil;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Manager_BBDevMgr

Manager_BBDevMgr::Manager_BBDevMgr()
:	fMutex("Manager_BBDevMgr::fMutex"),
	fNextID(1)
	{
	::CoCreateInstance(
		IDeviceManager::sCLSID,
		(IUnknown*)0,
		CLSCTX_ALL,
		ZUUIDOF(ZBlackBerryCOM::IDeviceManager),
		(void**)&fDeviceManager);

	if (fDeviceManager)
		{
		fDeviceManager->Advise(this, &fCookie);

		// Build the initial list
		ZMutexLocker locker(fMutex);

		ZBlackBerryCOM::IDevices* theDevices;
		if (SUCCEEDED(fDeviceManager->Devices(&theDevices) && theDevices))
			{
			uint32 theCount;
			theDevices->Count(&theCount);

			for (uint32 x = 0; x < theCount; ++x)
				{
				ZBlackBerryCOM::IDevice* theDevice;
				if (SUCCEEDED(theDevices->Item(x, &theDevice)))
					{
					Entry_t anEntry;
					anEntry.fID = fNextID++;
					anEntry.fDevice = new Device_BBDevMgr(theDevice);
					fEntries.push_back(anEntry);
					theDevice->Release();
					}
				}
			theDevices->Release();
			}
		}	
	}

Manager_BBDevMgr::~Manager_BBDevMgr()
	{
	fEntries.clear();

	if (fDeviceManager)
		{
		fDeviceManager->Unadvise(fCookie);
		fDeviceManager->Release();
		}
	}

STDMETHODIMP Manager_BBDevMgr::QueryInterface(
	const IID& iInterfaceID, void** oObjectRef)
	{
	if (ZLOG(s, eDebug + 4, "ZBlackBerry::Manager_BBDevMgr"))
		s << "QueryInterface";

	*oObjectRef = nil;
	if (iInterfaceID == IID_IUnknown)
		{
		*oObjectRef = static_cast<IUnknown*>(this);
		this->AddRef();
		return NOERROR;
		}

	if (iInterfaceID == ZUUIDOF(IDeviceManagerEvents))
		{
		*oObjectRef = static_cast<IDeviceManagerEvents*>(this);
		this->AddRef();
		return NOERROR;
		}

	return E_NOINTERFACE;
	}

ULONG STDMETHODCALLTYPE Manager_BBDevMgr::AddRef()
	{ return ZRefCountedWithFinalization::AddRef(); }

ULONG STDMETHODCALLTYPE Manager_BBDevMgr::Release()
	{ return ZRefCountedWithFinalization::Release(); }

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

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		{
		if (i->fDevice->Matches(iDevice))
			{
			i->fDevice->pDisconnected();
			fEntries.erase(i);
			break;
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

#endif // ZCONFIG_API_Enabled(BlackBerry_BBDevMgr)
