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

#include "zoolib/ZUSB_OSX.h"

#if ZCONFIG_API_Enabled(USB_OSX)

#include "zoolib/Memory.h" // For sMemCopy
#include "zoolib/Stringf.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZUtil_Strim_Data.h"
#include "zoolib/ZVal_CF.h"

#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>

#include <vector>

namespace ZooLib {

using std::exception;
using std::min;
using std::runtime_error;
using std::string;

// =================================================================================================
// MARK: - Other stuff

static string spErrorAsString(IOReturn iError)
	{
	switch (iError)
		{
		case kIOReturnNotResponding: return "kIOReturnNotResponding";
		case kIOReturnNoDevice: return "kIOReturnNotResponding";
		case kIOReturnBadArgument: return "kIOReturnNotResponding";
		case kIOReturnAborted: return "kIOReturnAborted";
		default: return sStringf("%d", iError);
		}
	}

static void spThrowIfErr(IOReturn iErr)
	{
	if (iErr)
		throw runtime_error("Got error");
	}

static IOCFPlugInInterface** spCreate_PluginInterface(io_service_t iService)
	{
	IOCFPlugInInterface** plugInInterface = nullptr;
	SInt32 theScore;
	::IOCreatePlugInInterfaceForService(iService,
		kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &theScore);
	return plugInInterface;
	}

static IOUSBDeviceInterface182** spCreate_USBDeviceInterface(io_service_t iUSBDevice)
	{
	IOUSBDeviceInterface182** theIOUSBDeviceInterface = nullptr;
	if (IOCFPlugInInterface** plugInInterface = spCreate_PluginInterface(iUSBDevice))
		{
		if (0 != plugInInterface[0]->QueryInterface(plugInInterface,
			CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID182), (LPVOID*)&theIOUSBDeviceInterface))
			{
			theIOUSBDeviceInterface = nullptr;
			}
		::IODestroyPlugInInterface(plugInInterface);
		}

	return theIOUSBDeviceInterface;
	}

// =================================================================================================
// MARK: - ZUSBWatcher

ZUSBWatcher::ZUSBWatcher(
	IONotificationPortRef iIONotificationPortRef, SInt32 iUSBVendor, SInt32 iUSBProduct)
:	fIONotificationPortRef(iIONotificationPortRef)
,	fNotification(0)
	{
	ZMap_CF theMap = sAdopt& ::IOServiceMatching(kIOUSBDeviceClassName);
	theMap.Set(kUSBVendorID, int32(iUSBVendor));
	theMap.Set(kUSBProductID, int32(iUSBProduct));

	spThrowIfErr(::IOServiceAddMatchingNotification(
		fIONotificationPortRef, kIOFirstMatchNotification, theMap.Orphan(),
		spDeviceAdded, this, &fNotification));
	}

ZUSBWatcher::~ZUSBWatcher()
	{
	if (fNotification)
		::IOObjectRelease(fNotification);
	}

void ZUSBWatcher::SetCallable(ZRef<CB_DeviceAttached> iCallable)
	{
	fCallable = iCallable;
	if (iCallable)
		this->pDeviceAdded(fNotification);
	}

void ZUSBWatcher::pDeviceAdded(io_iterator_t iIterator)
	{
	for (io_service_t iter; (iter = ::IOIteratorNext(iIterator)); /*no inc*/)
		{
		if (ZLOGPF(s, eInfo))
			s << "iterator: " << iter;

		try
			{
			ZRef<ZUSBDevice> theUSBDevice = new ZUSBDevice(fIONotificationPortRef, iter);
			sCall(fCallable.Get(), theUSBDevice);
			}
		catch (exception& ex)
			{
			if (ZLOGPF(s, eInfo))
				s << "Couldn't instantiate ZUSBDevice, caught exception: " << ex.what();

			sCall(fCallable.Get(), null);
			}

		::IOObjectRelease(iter);
		}
	}

void ZUSBWatcher::spDeviceAdded(void* iRefcon, io_iterator_t iIterator)
	{ static_cast<ZUSBWatcher*>(iRefcon)->pDeviceAdded(iIterator); }

// =================================================================================================
// MARK: - ZUSBDevice

ZUSBDevice::ZUSBDevice(IONotificationPortRef iIONotificationPortRef, io_service_t iUSBDevice)
:	fIOUSBDeviceInterface(nullptr)
,	fNotification(0)
,	fLocationID(0)
,	fDetached(false)
,	fHasIOUSBDeviceDescriptor(false)
	{
	try
		{
		fIOUSBDeviceInterface = spCreate_USBDeviceInterface(iUSBDevice);
		if (!fIOUSBDeviceInterface)
			{
			if (ZLOGPF(s, eDebug))
				s << "sCreate_USBDeviceInterface returned null";
			throw runtime_error("Couldn't create fIOUSBDeviceInterface");
			}

		fIOUSBDeviceInterface[0]->GetLocationID(fIOUSBDeviceInterface, &fLocationID);

		kern_return_t result = ::IOServiceAddInterestNotification(
			iIONotificationPortRef, iUSBDevice, kIOGeneralInterest,
			spDeviceNotification, this, &fNotification);

		if (result != KERN_SUCCESS || fNotification == 0)
			throw runtime_error("Couldn't create notification");

		result = fIOUSBDeviceInterface[0]->USBDeviceOpen(fIOUSBDeviceInterface);

		if (result)
			{
			if (ZLOGPF(s, eInfo))
				s << "USBDeviceOpen failed: " << result;
			throw runtime_error("ZUSBDevice, USBDeviceOpen failed");
			}

		UInt8 numConf;
		spThrowIfErr(fIOUSBDeviceInterface[0]->
			GetNumberOfConfigurations(fIOUSBDeviceInterface, &numConf));

		if (!numConf)
			{
			if (ZLOGPF(s, eDebug))
				s << "Got zero configurations";
			throw runtime_error("Got zero configurations");
			}

		// If the device is composite, which is common and definitely the case for BlackBerrys,
		// then the AppleUSBComposite driver will have already configured it. So I'm
		// disabling this call to SetConfiguration.
		if (false)
			{
			IOUSBConfigurationDescriptorPtr confDesc;
			spThrowIfErr(fIOUSBDeviceInterface[0]->
				GetConfigurationDescriptorPtr(fIOUSBDeviceInterface, 0, &confDesc));

			spThrowIfErr(fIOUSBDeviceInterface[0]->
			SetConfiguration(fIOUSBDeviceInterface, confDesc->bConfigurationValue));
			}
		}
	catch (...)
		{
		if (fNotification)
			{
			::IOObjectRelease(fNotification);
			fNotification = 0;
			}

		if (fIOUSBDeviceInterface)
			{
			fIOUSBDeviceInterface[0]->USBDeviceClose(fIOUSBDeviceInterface);
			fIOUSBDeviceInterface[0]->Release(fIOUSBDeviceInterface);
			fIOUSBDeviceInterface = nullptr;
			}
		throw;
		}
	}

ZUSBDevice::~ZUSBDevice()
	{
	if (fNotification)
		{
		::IOObjectRelease(fNotification);
		fNotification = 0;
		}

	if (fIOUSBDeviceInterface )
		{
		fIOUSBDeviceInterface[0]->USBDeviceClose(fIOUSBDeviceInterface);
		fIOUSBDeviceInterface[0]->Release(fIOUSBDeviceInterface);
		fIOUSBDeviceInterface = nullptr;
		}
	}

void ZUSBDevice::SetCallable(ZRef<CB_DeviceDetached> iCallable)
	{
	fCallable = iCallable;
	if (fDetached)
		sCall(fCallable.Get(), this);
	}

IOUSBDeviceInterface182** ZUSBDevice::GetIOUSBDeviceInterface()
	{ return fIOUSBDeviceInterface; }

ZRef<ZUSBInterfaceInterface> ZUSBDevice::CreateInterfaceInterface(
	CFRunLoopRef iRunLoopRef, uint8 iProtocol)
	{
	IOUSBFindInterfaceRequest request;
	request.bInterfaceClass = kIOUSBFindInterfaceDontCare;
	request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
	request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
	request.bAlternateSetting = kIOUSBFindInterfaceDontCare;

	io_iterator_t iterator;
	if (kIOReturnSuccess != fIOUSBDeviceInterface[0]->CreateInterfaceIterator(
		fIOUSBDeviceInterface, &request, &iterator))
		{ iterator = 0; }

	while (iterator)
		{
		io_service_t usbInterface = ::IOIteratorNext(iterator);
		if (!usbInterface)
			{
			::IOObjectRelease(iterator);
			iterator = 0;
			continue;
			}

		SInt32 score;
		IOCFPlugInInterface** plugInInterface;
		if (kIOReturnSuccess != ::IOCreatePlugInInterfaceForService(
			usbInterface, kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID,
			&plugInInterface, &score))
			{ plugInInterface = nullptr; }

		::IOObjectRelease(usbInterface);

		if (plugInInterface)
			{
			IOUSBInterfaceInterface190** theIOUSBInterfaceInterface;

			if (plugInInterface[0]->QueryInterface(plugInInterface,
				CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID190),
				(LPVOID*)&theIOUSBInterfaceInterface))
				{
				theIOUSBInterfaceInterface = nullptr;
				}
			::IODestroyPlugInInterface(plugInInterface);

			if (theIOUSBInterfaceInterface)
				{
				UInt8 protocol = 0;
				if (0 == theIOUSBInterfaceInterface[0]->
					GetInterfaceProtocol(theIOUSBInterfaceInterface, &protocol))
					{
					if (protocol == iProtocol)
						{
						if (kIOReturnSuccess == theIOUSBInterfaceInterface[0]->USBInterfaceOpen(
							theIOUSBInterfaceInterface))
							{
							::IOObjectRelease(iterator);
							return new ZUSBInterfaceInterface(this,
								iRunLoopRef, theIOUSBInterfaceInterface);
							}
						}
					}
				theIOUSBInterfaceInterface[0]->Release(theIOUSBInterfaceInterface);
				}
			}
		}
	return null;
	}

void ZUSBDevice::pDeviceNotification(
	io_service_t iService, natural_t iMessageType, void* iMessageArgument)
	{
	if (iMessageType == kIOMessageServiceIsTerminated)
		{
		if (ZLOGPF(s, eInfo))
			s << "Device removed, service: " << iService;

		fDetached = true;
		sCall(fCallable.Get(), this);
		}
	}

void ZUSBDevice::spDeviceNotification(
	void* iRefcon, io_service_t iService, natural_t iMessageType, void* iMessageArgument)
	{
	static_cast<ZUSBDevice*>(iRefcon)->
		pDeviceNotification(iService, iMessageType, iMessageArgument);
	}

uint16 ZUSBDevice::GetIDVendor()
	{
	this->pFetchDeviceDescriptor();
	return ZByteSwap_LittleToHost16(fIOUSBDeviceDescriptor.idVendor);
	}

uint16 ZUSBDevice::GetIDProduct()
	{
	this->pFetchDeviceDescriptor();
	return ZByteSwap_LittleToHost16(fIOUSBDeviceDescriptor.idProduct);
	}

const IOUSBDeviceDescriptor& ZUSBDevice::GetDeviceDescriptor()
	{
	this->pFetchDeviceDescriptor();
	return fIOUSBDeviceDescriptor;
	}

// If we use IOUSBDeviceInterface197 then we can bypass this crap.
void ZUSBDevice::pFetchDeviceDescriptor()
	{
	if (fHasIOUSBDeviceDescriptor)
		return;

	IOUSBDevRequest req;
	req.bmRequestType = USBmakebmRequestType(kUSBIn, kUSBStandard, kUSBDevice);
	req.bRequest = kUSBRqGetDescriptor;
	req.wValue = kUSBDeviceDesc << 8;
	req.wIndex = 0;
	req.wLength = sizeof(IOUSBDeviceDescriptor);
	req.pData = &fIOUSBDeviceDescriptor;
	if (0 == fIOUSBDeviceInterface[0]->DeviceRequest(fIOUSBDeviceInterface, &req))
		fHasIOUSBDeviceDescriptor = true;
	}

// =================================================================================================
// MARK: - StreamerR_TO

namespace { // anonymous

class StreamerR_TO
:	public ZStreamerR,
	private ZStreamR
	{
public:
	StreamerR_TO(ZRef<ZUSBInterfaceInterface> iUSBII,
		IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
		int iPipeRefR,
		size_t iBufferSize);

	virtual ~StreamerR_TO();

// From ZStreamerR via ZStreamerR
	virtual const ZStreamR& GetStreamR();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

private:
	bool pRefill(double iTimeout);

	ZRef<ZUSBInterfaceInterface> fUSBII;
	IOUSBInterfaceInterface190** fII;
	int fPipeRefR;
	std::vector<uint8> fBuffer;
	size_t fOffset;
	size_t fEnd;
	};

} // anonymous namespace

StreamerR_TO::StreamerR_TO(ZRef<ZUSBInterfaceInterface> iUSBII,
	IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
	int iPipeRefR,
	size_t iBufferSize)
:	fUSBII(iUSBII),
	fII(iIOUSBInterfaceInterface),
	fPipeRefR(iPipeRefR),
	fBuffer(iBufferSize, 0),
	fOffset(0),
	fEnd(0)
	{}

StreamerR_TO::~StreamerR_TO()
	{}

const ZStreamR& StreamerR_TO::GetStreamR()
	{ return *this; }

void StreamerR_TO::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(oDest);

	for (;;)
		{
		if (fEnd > fOffset)
			{
			const size_t countToCopy = min(fEnd - fOffset, iCount);
			sMemCopy(localDest, &fBuffer[fOffset], countToCopy);
			localDest += countToCopy;
			fOffset += countToCopy;
			break;
			}
		else
			{
			if (!this->pRefill(1.0))
				break;
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(oDest);
	}

size_t StreamerR_TO::Imp_CountReadable()
	{ return fEnd - fOffset; }

bool StreamerR_TO::Imp_WaitReadable(double iTimeout)
	{
	if (this->pRefill(iTimeout))
		return fEnd > fOffset;

	return true;
	}

bool StreamerR_TO::pRefill(double iTimeout)
	{
	if (fEnd > fOffset)
		return true;

	fOffset = 0;
	fEnd = 0;
	UInt32 localCount = fBuffer.size();
	IOReturn result = fII[0]->ReadPipeTO(
		fII, fPipeRefR, &fBuffer[0], &localCount, iTimeout * 1e3, 1000000);

	if (kIOUSBTransactionTimeout == result)
		{
		fII[0]->ClearPipeStallBothEnds(fII, fPipeRefR);
		if (ZLOGPF(s, eDebug + 2))
			{
			s << "pRefill, Timeout";
			}
		return true;
		}
	else if (0 == result)
		{
		if (ZLOGPF(s, eDebug + 2))
			{
			s << "pipe: "<< fPipeRefR << ", ";
			ZUtil_Strim_Data::sDumpData(s, &fBuffer[0], localCount);
			}
		fEnd = localCount;
		return true;
		}
	else
		{
		if (ZLOGPF(s, eDebug))
			{
			s << "pRefill, Got result: " << spErrorAsString(result);
			}
		return false;
		}
	}

// =================================================================================================
// MARK: - StreamerR_Async

namespace { // anonymous

class StreamerR_Async
:	public ZStreamerR,
	private ZStreamR
	{
public:
	StreamerR_Async(ZRef<ZUSBInterfaceInterface> iUSBII,
		IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
		int iPipeRefR,
		size_t iBufferSize);

	virtual ~StreamerR_Async();

// From ZStreamerR via ZStreamerR
	virtual const ZStreamR& GetStreamR();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

private:
	void pTriggerRead();
	void pCompletion(IOReturn iResult, void* iArg);
	static void spCompletion(void* iRefcon, IOReturn iResult, void* iArg);

	ZRef<ZUSBInterfaceInterface> fUSBII;
	IOUSBInterfaceInterface190** fII;
	int fPipeRefR;

	ZMtx fMtx;
	ZCnd fCnd;
	std::vector<uint8> fBuffer;
	size_t fOffset;
	size_t fEnd;
	bool fPending;
	bool fOpen;
	};

} // anonymous namespace

StreamerR_Async::StreamerR_Async(ZRef<ZUSBInterfaceInterface> iUSBII,
	IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
	int iPipeRefR,
	size_t iBufferSize)
:	fUSBII(iUSBII),
	fII(iIOUSBInterfaceInterface),
	fPipeRefR(iPipeRefR),
	fBuffer(iBufferSize,0),
	fOffset(0),
	fEnd(0),
	fPending(false),
	fOpen(true)
	{}

StreamerR_Async::~StreamerR_Async()
	{
	ZAcqMtx acq(fMtx);
	fOpen = false;
	if (fPending)
		{
		fII[0]->AbortPipe(fII, fPipeRefR);
		while (fPending)
			fCnd.Wait(fMtx);
		}
	}

const ZStreamR& StreamerR_Async::GetStreamR()
	{ return *this; }

void StreamerR_Async::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(oDest);

	ZAcqMtx acq(fMtx);
	while (fOpen)
		{
		if (fOffset < fEnd)
			{
			const size_t countToCopy = min(fEnd - fOffset, iCount);
			sMemCopy(localDest, &fBuffer[fOffset], countToCopy);
			localDest += countToCopy;
			fOffset += countToCopy;
			break;
			}
		else if (!fPending)
			{
			this->pTriggerRead();
			}
		else
			{
			fCnd.Wait(fMtx);
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(oDest);
	}

size_t StreamerR_Async::Imp_CountReadable()
	{ return fEnd - fOffset; }

bool StreamerR_Async::Imp_WaitReadable(double iTimeout)
	{
	ZAcqMtx acq(fMtx);

	if (fOpen)
		{
		this->pTriggerRead();

		if (fOffset >= fEnd)
			fCnd.WaitFor(fMtx, iTimeout);
		}

	return fOffset < fEnd;
	}

void StreamerR_Async::pTriggerRead()
	{
	if (fPending)
		return;

	ZAssert(fOffset >= fEnd);
	fOffset = 0;
	fEnd = 0;

	IOReturn result = fII[0]->ReadPipeAsync(
		fII, fPipeRefR, &fBuffer[0], fBuffer.size(), spCompletion, this);

	if (kIOReturnSuccess == result)
		fPending = true;
	}

void StreamerR_Async::pCompletion(IOReturn iResult, void* iArg)
	{
	// We'll deadlock if completion is called as a result of ReadPipeAsync being called.
	ZAcqMtx acq(fMtx);
	fPending = false;
	ZAssert(fOffset == 0 && fEnd == 0);
	fEnd = reinterpret_cast<size_t>(iArg);
	fCnd.Broadcast();
	}

void StreamerR_Async::spCompletion(void* iRefcon, IOReturn iResult, void* iArg)
	{ static_cast<StreamerR_Async*>(iRefcon)->pCompletion(iResult, iArg); }

// =================================================================================================
// MARK: - StreamerW

namespace { // anonymous

class StreamerW
:	public ZStreamerW,
	private ZStreamW
	{
public:
	StreamerW(ZRef<ZUSBInterfaceInterface> iUSBII,
		IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
		int iPipeRefW);

	virtual ~StreamerW();

// From ZStreamerW via ZStreamerRW
	virtual const ZStreamW& GetStreamW();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

private:
	ZRef<ZUSBInterfaceInterface> fUSBII;
	IOUSBInterfaceInterface190** fII;
	int fPipeRefW;
	};

} // anonymous namespace

StreamerW::StreamerW(ZRef<ZUSBInterfaceInterface> iUSBII,
	IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
	int iPipeRefW)
:	fUSBII(iUSBII),
	fII(iIOUSBInterfaceInterface),
	fPipeRefW(iPipeRefW)
	{}

StreamerW::~StreamerW()
	{}

const ZStreamW& StreamerW::GetStreamW()
	{ return *this; }

void StreamerW::Imp_Write(
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{
//	if (size_t countToWrite = min(size_t(1024), iCount))
	if (size_t countToWrite = iCount)
		{
		if (ZLOGPF(s, eDebug + 2))
			{
			s << "Imp_Write, pipe: " << fPipeRefW << ", ";
			ZUtil_Strim_Data::sDumpData(s, iSource, countToWrite);
			}

		if (0 == fII[0]->WritePipe(fII, fPipeRefW, const_cast<void*>(iSource), countToWrite))
			{
			if (oCountWritten)
				*oCountWritten = countToWrite;
			return;
			}
		}

	if (oCountWritten)
		*oCountWritten = 0;
	}

// =================================================================================================
// MARK: - ZUSBInterfaceInterface

ZUSBInterfaceInterface::ZUSBInterfaceInterface(ZRef<ZUSBDevice> iUSBDevice,
	CFRunLoopRef iRunLoopRef,
	IOUSBInterfaceInterface190** iIOUSBInterfaceInterface)
:	fUSBDevice(iUSBDevice),
	fII(iIOUSBInterfaceInterface),
	fRunLoopRef(iRunLoopRef),
	fOpen(true)
	{
	CFRunLoopSourceRef theSource;
	fII[0]->CreateInterfaceAsyncEventSource(fII, &theSource);
	::CFRunLoopAddSource(fRunLoopRef, theSource, kCFRunLoopDefaultMode);
	}

ZUSBInterfaceInterface::~ZUSBInterfaceInterface()
	{
	CFRunLoopSourceRef theSource = fII[0]->GetInterfaceAsyncEventSource(fII);
	::CFRunLoopRemoveSource(fRunLoopRef, theSource, kCFRunLoopDefaultMode);

	if (fOpen)
		fII[0]->USBInterfaceClose(fII);
	fII[0]->Release(fII);
	fII = nullptr;
	}

IOUSBInterfaceInterface190** ZUSBInterfaceInterface::GetIOUSBInterfaceInterface()
	{ return fII; }

ZRef<ZUSBDevice> ZUSBInterfaceInterface::GetUSBDevice()
	{ return fUSBDevice; }

ZRef<ZStreamerR> ZUSBInterfaceInterface::OpenR(int iPipeRefR)
	{
	if (fOpen)
//		return new StreamerR_Async(this, fII, iPipeRefR, 1024);
		return new StreamerR_TO(this, fII, iPipeRefR, 1024);

	return null;
	}

ZRef<ZStreamerW> ZUSBInterfaceInterface::OpenW(int iPipeRefW)
	{
	if (fOpen)
		{
		// We call ClearPipeStallBothEnds in case a previous use had not cleanly
		// disconnected, which can leave the peripheral device stalled. Read will
		// do this whenever it times out, so we don't have to do it in OpenR.
		fII[0]->ClearPipeStallBothEnds(fII, iPipeRefW);
		return new StreamerW(this, fII, iPipeRefW);
		}
	return null;
	}

void ZUSBInterfaceInterface::Close()
	{
	if (fOpen)
		{
		fOpen = false;
		fII[0]->USBInterfaceClose(fII);
		}
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(USB_OSX)
