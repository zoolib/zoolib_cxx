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
#include "zoolib/ZMemory.h" // For ZMemCopy
#include "zoolib/ZTime.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Strim_Operators.h"

#include <oleauto.h>

namespace ZooLib {
namespace ZBlackBerry {

using std::string;
using std::vector;
using std::wstring;
using namespace ZBlackBerryCOM;
using namespace ZWinCOM;

// =================================================================================================
// MARK: - ZBlackBerry::Channel_BBDevMgr

class Channel_BBDevMgr
:	private IChannelEvents,
	public Channel,
	private ZStreamRCon,
	private ZStreamWCon
	{
	friend class Device_BBDevMgr;

public:
	Channel_BBDevMgr(ZRef<IDevice> iDevice,
		bool iPreserveBoundaries, const string& iName, const PasswordHash* iPasswordHash);

	virtual ~Channel_BBDevMgr();

// From ZCounted via ZBlackBerry::Channel
	virtual void Initialize();

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
	bool pRefill(ZGuardRMtxR& iLocker, ZRef<IChannel> iChannel, double* ioTimeout);
	void pAbort();
	ZRef<IChannel> pUseChannel();

	ZMtxR fMutex;
	ZCnd fCondition_Reader;

	ZRef<IDevice> fDevice;
	bool fPreserveBoundaries;
	string fName;

	ZRef<IChannel> fChannel;
	ChannelParams fChannelParams;

	vector<char> fBuffer;
	size_t fStart;
	size_t fEnd;

	bool fHasPasswordHash;
	PasswordHash fPasswordHash;
	bool fClosed;
	};

Channel_BBDevMgr::Channel_BBDevMgr(ZRef<IDevice> iDevice,
	bool iPreserveBoundaries, const string& iName, const PasswordHash* iPasswordHash)
:	fDevice(iDevice),
	fPreserveBoundaries(iPreserveBoundaries),
	fName(iName),
	fStart(0),
	fEnd(0),
	fHasPasswordHash(0 != iPasswordHash),
	fClosed(false)
	{
	if (iPasswordHash)
		fPasswordHash = *iPasswordHash;
	}

Channel_BBDevMgr::~Channel_BBDevMgr()
	{
	ZLOGTRACE(eDebug + 3);
	}

// Experimental


void Channel_BBDevMgr::Initialize()
	{
	wstring wideName = ZUnicode::sAsWString(fName);

	HRESULT theResult = fDevice->OpenChannel(wideName.c_str(), this, sPtr(fChannel));

	if (Success& theResult && fChannel)
		{
		fChannel->Params(&fChannelParams);
		// Allow for two extra bytes, just in case we're preserving boundaries.
		fBuffer.resize(fChannelParams.fMaxReceiveUnit + 2);
		}
	else
		{
		throw std::runtime_error("Channel_BBDevMgr::Initialize, Couldn't open channel");
		}
	}

STDMETHODIMP Channel_BBDevMgr::QueryInterface(const IID& iInterfaceID, void** oObjectRef)
	{
	if (ZLOG(s, eDebug + 4, "ZBlackBerry::Channel_BBDevMgr"))
		s << "QueryInterface";

	*oObjectRef = nullptr;

	if (iInterfaceID == IID_IUnknown)
		return sCopy<IUnknown>(oObjectRef, this);

	if (iInterfaceID == ZMACRO_UUID(IChannelEvents))
		return sCopy<IChannelEvents>(oObjectRef, this);

	return E_NOINTERFACE;
	}

ULONG STDMETHODCALLTYPE Channel_BBDevMgr::AddRef()
	{ return this->pCOMAddRef(); }

ULONG STDMETHODCALLTYPE Channel_BBDevMgr::Release()
	{ return this->pCOMRelease(); }

STDMETHODIMP Channel_BBDevMgr::CheckClientStatus(uint16 iRHS)
	{
	ZLOGTRACE(eDebug + 3);
	// Not implemented - what does this do?

	return S_OK;
	}

STDMETHODIMP Channel_BBDevMgr::OnChallenge(int32 iAttemptsRemaining, uint8 oPasswordHash[20])
	{
	ZLOGTRACE(eDebug + 3);

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
	ZLOGTRACE(eDebug + 3);

	ZGuardRMtxR locker(fMutex);
	fCondition_Reader.Broadcast();

	return NOERROR;
	}

STDMETHODIMP Channel_BBDevMgr::OnClose()
	{
	ZLOGTRACE(eDebug + 3);

	// Close initiated by device removal or the other side. An abort
	// here seems to deadlock, hence our use of the fClosed flag.

	ZGuardRMtxR locker(fMutex);
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

void Channel_BBDevMgr::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (ZLOGPF(s, eDebug + 3))
		s << "iCount: " << iCount;

	uint8* localDest = static_cast<uint8*>(oDest);

	ZGuardRMtxR locker(fMutex);

	for (;;)
		{
		if (fClosed)
			break;

		if (fEnd > fStart)
			{
			const size_t countToCopy = std::min(iCount, fEnd - fStart);
			ZMemCopy(localDest, &fBuffer[fStart], countToCopy);
			localDest += countToCopy;
			fStart += countToCopy;
			if (ZLOGPF(s, eDebug + 3))
				s << "countToCopy: " << countToCopy;
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
		*oCountRead = localDest - static_cast<uint8*>(oDest);
	}

size_t Channel_BBDevMgr::Imp_CountReadable()
	{
	ZLOGTRACE(eDebug + 3);

	ZGuardRMtxR locker(fMutex);
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
	ZLOGTRACE(eDebug + 3);

	ZGuardRMtxR locker(fMutex);

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
	ZLOGTRACE(eDebug + 3);

	ZGuardRMtxR locker(fMutex);

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
	if (ZLOGPF(s, eDebug + 3))
		s << "iCount: " << iCount;

	const uint8* localSource = static_cast<const uint8*>(iSource);

	ZGuardRMtxR locker(fMutex);

	if (ZRef<IChannel> theChannel = this->pUseChannel())
		{
		locker.Release();
		const size_t countToWrite = std::min(iCount, size_t(fChannelParams.fMaxTransmitUnit));

		if (Success& theChannel->WritePacket(localSource, countToWrite))
			{
			localSource += countToWrite;
			}
		else
			{
			if (ZLOGPF(s, eDebug + 2))
				s << "Write failed, countToWrite: " << countToWrite;
			}
		}

	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const uint8*>(iSource);
	}

void Channel_BBDevMgr::Imp_SendDisconnect()
	{
	ZLOGTRACE(eDebug + 3);

	this->pAbort();
	}

void Channel_BBDevMgr::Imp_Abort()
	{
	ZLOGTRACE(eDebug + 3);

	this->pAbort();
	}

bool Channel_BBDevMgr::pRefill(ZGuardRMtxR& iLocker, ZRef<IChannel> iChannel, double* ioTimeout)
	{
	ZLOGTRACE(eDebug + 3);

	ZAssert(iChannel);
	ZAssert(fEnd <= fStart);

	for (;;)
		{
		if (fClosed)
			return false;

		int32 packetsAvailable;
		if (Failure& iChannel->PacketsAvailable(&packetsAvailable))
			{
			return false;
			}
		else if (packetsAvailable)
			{
			iLocker.Release();

			int32 countRead;
			if (fPreserveBoundaries)
				{
				if (Failure& iChannel->ReadPacket(&fBuffer[2], fBuffer.size() - 2, &countRead))
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
				if (Failure& iChannel->ReadPacket(&fBuffer[0], fBuffer.size(), &countRead))
					countRead = 0;
				}

			if (ZLOGPF(s, eDebug + 3))
				s << "countRead: " << countRead;

			iLocker.Acquire();
			fStart = 0;
			fEnd = countRead;
			if (countRead)
				return true;
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
	}

void Channel_BBDevMgr::pAbort()
	{
	ZGuardRMtxR locker(fMutex);
	fClosed = true;
	fChannel.Clear();
	fCondition_Reader.Broadcast();
	}

ZRef<IChannel> Channel_BBDevMgr::pUseChannel()
	{
	return fChannel;
	}

// =================================================================================================
// MARK: - ZBlackBerry::Device_BBDevMgr

class Device_BBDevMgr : public Device
	{
public:
	Device_BBDevMgr(ZRef<IDevice> iDevice);
	virtual ~Device_BBDevMgr();

// From ZBlackBerry::Device
	virtual ZRef<Channel> Open(bool iPreserveBoundaries,
		const string& iName, const PasswordHash* iPasswordHash, Error* oError);
	virtual ZQ<Data> GetAttribute(uint16 iObject, uint16 iAttribute);
	virtual uint32 GetPIN();

// Called by Manager_BBDevMgr
	bool Matches(IDevice* iDevice);
	void pDisconnected();

private:
	bool pGetProperty(const string16& iName, VARIANT& oValue);
	ZRef<IDevice> pUseDevice();

	ZMtxR fMutex;
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
	if (ZLOGPF(s, eDebug + 3))
		s << "iName: " << iName;

	if (ZRef<IDevice> theDevice = this->pUseDevice())
		{
		try
			{
			ZRef<Channel> theChannel = new Channel_BBDevMgr(theDevice, iPreserveBoundaries, iName, iPasswordHash);
			return theChannel;
			}
		catch (...)
			{
			// FIXME. Failure may also be due to a bad/missing/expired password.
			//#warning "NDY"
			if (oError)
				*oError = error_UnknownChannel;
			}
		}
	else
		{
		if (oError)
			*oError = error_DeviceClosed;
		}

	return null;
	}

ZQ<Data> Device_BBDevMgr::GetAttribute(uint16 iObject, uint16 iAttribute)
	{
	ZLOGTRACE(eDebug + 3);

	return null;
	}

uint32 Device_BBDevMgr::GetPIN()
	{
	ZLOGTRACE(eDebug + 3);

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
		if (Success& theDevice->Equals(iDevice, &result))
			return result == 1;
		}

	return false;
	}

void Device_BBDevMgr::pDisconnected()
	{
	ZGuardRMtxR locker(fMutex);
	fDevice.Clear();
	this->pFinished();
	}

bool Device_BBDevMgr::pGetProperty(const string16& iName, VARIANT& oValue)
	{
	if (ZRef<IDevice> theDevice = this->pUseDevice())
		{
		ZRef<IDeviceProperties> theDPs;
		if (Success& theDevice->Properties(sPtr(theDPs)))
			{
			uint32 dpCount;
			theDPs->Count(&dpCount);
			for (uint32 x = 0; x < dpCount; ++x)
				{
				ZRef<IDeviceProperty> theDP;
				if (Success& theDPs->Item(Variant(x), sPtr(theDP)))
					{
					ZWinCOM::String theName;
					if (Success& theDP->Name(sPtr(theName)))
						{
						if (iName == theName)
							{
							if (Success& theDP->Value(&oValue))
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
	ZGuardRMtxR locker(fMutex);
	return fDevice;
	}

// =================================================================================================
// MARK: - ZBlackBerry::Manager_BBDevMgr

Manager_BBDevMgr::Manager_BBDevMgr()
:	fNextID(1)
,	fDeviceManager(ZWinCOM::sCreate<IDeviceManager>(IDeviceManager::sCLSID))
	{
	if (!fDeviceManager)
		throw std::runtime_error("Couldn't instantiate IDeviceManager");
	}

Manager_BBDevMgr::~Manager_BBDevMgr()
	{}

void Manager_BBDevMgr::Initialize()
	{
	Manager::Initialize();

	fDeviceManager->Advise(this, &fCookie);

	// Build the initial list
	ZGuardRMtxR locker(fMutex);

	if (OParam<ZRef<IDevices> > theDevices = fDeviceManager->Devices(theDevices))
		{
		if (OParam<uint32> theCount = theDevices.Get()->Count(theCount))
			{
			for (uint32 x = 0; x < theCount.Get(); ++x)
				{
				if (OParam<ZRef<IDevice> > theDevice = theDevices.Get()->Item(x, theDevice))
					{
					Entry_t anEntry;
					anEntry.fID = fNextID++;
					anEntry.fDevice = new Device_BBDevMgr(theDevice.Get());
					fEntries.push_back(anEntry);
					}
				}
			}
		}
	}

void Manager_BBDevMgr::Finalize()
	{
	fDeviceManager->Unadvise(fCookie);
	fEntries.clear();

	Manager::Finalize();
	}

STDMETHODIMP Manager_BBDevMgr::QueryInterface(
	const IID& iInterfaceID, void** oObjectRef)
	{
	if (ZLOG(s, eDebug + 4, "ZBlackBerry::Manager_BBDevMgr"))
		s << "QueryInterface";

	*oObjectRef = nullptr;

	if (iInterfaceID == IID_IUnknown)
		return sCopy<IUnknown>(oObjectRef, this);

	if (iInterfaceID == ZMACRO_UUID(IDeviceManagerEvents))
		return sCopy<IDeviceManagerEvents>(oObjectRef, this);

	return E_NOINTERFACE;
	}

ULONG STDMETHODCALLTYPE Manager_BBDevMgr::AddRef()
	{ return ZCounted::pCOMAddRef(); }

ULONG STDMETHODCALLTYPE Manager_BBDevMgr::Release()
	{ return ZCounted::pCOMRelease(); }

STDMETHODIMP Manager_BBDevMgr::DeviceConnect(IDevice* iDevice)
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager_BBDevMgr"))
		s.Writef("DeviceConnect, iDevice: %08X", iDevice);

	ZGuardRMtxR locker(fMutex);

	Entry_t anEntry;
	anEntry.fID = fNextID++;
	anEntry.fDevice = new Device_BBDevMgr(iDevice);
	fEntries.push_back(anEntry);

	locker.Release();

	Manager::pChanged();

	return S_OK;
	}

STDMETHODIMP Manager_BBDevMgr::DeviceDisconnect(IDevice* iDevice)
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager_BBDevMgr"))
		s.Writef("DeviceDisconnect, iDevice: %08X", iDevice);

	ZGuardRMtxR locker(fMutex);

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); /*no inc*/)
		{
		if (not i->fDevice->Matches(iDevice))
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

	Manager::pChanged();
	return S_OK;
	}

void Manager_BBDevMgr::GetDeviceIDs(vector<uint64>& oDeviceIDs)
	{
	ZLOGTRACE(eDebug + 3);

	ZGuardRMtxR locker(fMutex);

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		oDeviceIDs.push_back(i->fID);
	}

ZRef<Device> Manager_BBDevMgr::Open(uint64 iDeviceID)
	{
	ZLOGTRACE(eDebug + 3);

	ZGuardRMtxR locker(fMutex);

	for (vector<Entry_t>::iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		{
		if (i->fID == iDeviceID)
			return i->fDevice;;
		}

	return null;
	}

} // namespace ZBlackBerry
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(BlackBerry_BBDevMgr)
