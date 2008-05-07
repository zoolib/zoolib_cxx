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

#include "ZUSB_OSX.h"

#if ZCONFIG(OS, MacOSX)

#include "ZByteSwap.h"
#include "ZLog.h"
#include "ZMemory.h" // For ZBlockCopy
#include "ZUtil_Strim_Data.h"

#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>

// =================================================================================================
#pragma mark -
#pragma mark * Other stuff

static void sThrowIfErr(IOReturn iErr)
	{
	if (iErr)
		throw runtime_error("Got error");
	}

static IOCFPlugInInterface** sCreatePluginInterface(io_service_t iService)
	{
	IOCFPlugInInterface** plugInInterface = nil;
	SInt32 theScore;
	::IOCreatePlugInInterfaceForService(iService,
		kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &theScore);
	return plugInInterface;
	}

static IOUSBDeviceInterface182** sCreate_USBDeviceInterface(io_service_t iUSBDevice)
	{
	IOUSBDeviceInterface182** theIOUSBDeviceInterface = nil;
	if (IOCFPlugInInterface** plugInInterface = sCreatePluginInterface(iUSBDevice))
		{
		if (0 != plugInInterface[0]->QueryInterface(plugInInterface,
			CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID182), (LPVOID*)&theIOUSBDeviceInterface))
			{
			theIOUSBDeviceInterface = nil;
			}
		::IODestroyPlugInInterface(plugInInterface);
		}

	return theIOUSBDeviceInterface;
	}	

// =================================================================================================
#pragma mark -
#pragma mark * ZUSBWatcher

static void sSetSInt32(CFMutableDictionaryRef iDict, CFStringRef iPropName, SInt32 iValue)
	{
	CFNumberRef numberRef = ::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &iValue);
	::CFDictionarySetValue(iDict, iPropName, numberRef);
	::CFRelease(numberRef);
	}

ZUSBWatcher::ZUSBWatcher(
	IONotificationPortRef iIONotificationPortRef, SInt32 iUSBVendor, SInt32 iUSBProduct)
:	fObserver(nil),
	fIONotificationPortRef(iIONotificationPortRef),
	fNotification(0)
	{
	CFMutableDictionaryRef matchingDict = ::IOServiceMatching(kIOUSBDeviceClassName);
	sSetSInt32(matchingDict, CFSTR(kUSBVendorID), iUSBVendor);
	sSetSInt32(matchingDict, CFSTR(kUSBProductID), iUSBProduct);

	sThrowIfErr(::IOServiceAddMatchingNotification(
		fIONotificationPortRef, kIOFirstMatchNotification, matchingDict,
		spDeviceAdded, this, &fNotification));
	}

ZUSBWatcher::~ZUSBWatcher()
	{
	if (fNotification)
		::IOObjectRelease(fNotification);
	}

void ZUSBWatcher::SetObserver(Observer* iObserver)
	{
	fObserver = iObserver;
	// Iterate once to get already-present devices and arm the notification
	this->pDeviceAdded(fNotification);
	}

void ZUSBWatcher::pDeviceAdded(io_iterator_t iIterator)
	{
	for (io_service_t iter; iter = ::IOIteratorNext(iIterator); /*no inc*/)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Info, "ZUSBWatcher"))
			s.Writef("Device added, iterator: 0x%08x.", iter);

		try
			{
			ZRef<ZUSBDevice> theUSBDevice = new ZUSBDevice(fIONotificationPortRef, iter);
			fObserver->Added(theUSBDevice);
			}
		catch (exception& ex)
			{
			if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Info, "ZUSBWatcher"))
				s << "Couldn't instantiate ZUSBDevice, caught exception: " << ex.what();

			fObserver->Added(ZRef<ZUSBDevice>());
			}

		::IOObjectRelease(iter);
		}
	}

void ZUSBWatcher::spDeviceAdded(void* iRefcon, io_iterator_t iIterator)
	{ static_cast<ZUSBWatcher*>(iRefcon)->pDeviceAdded(iIterator); }

// =================================================================================================
#pragma mark -
#pragma mark * ZUSBDevice

ZUSBDevice::ZUSBDevice(IONotificationPortRef iIONotificationPortRef, io_service_t iUSBDevice)
:	fIOUSBDeviceInterface(nil),
	fNotification(0),
	fLocationID(0),
	fObserver(nil),
	fDetached(false),
	fHasIOUSBDeviceDescriptor(false)
	{
	try
		{
		fIOUSBDeviceInterface = sCreate_USBDeviceInterface(iUSBDevice);
		if (!fIOUSBDeviceInterface)
			{
			if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZUSBDevice"))
				s.Writef("sCreate_USBDeviceInterface returned null");
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
			if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Info, "ZUSBDevice"))
				s.Writef("ZUSBDevice, USBDeviceOpen failed");
			throw runtime_error("ZUSBDevice, USBDeviceOpen failed");
			}

		UInt8 numConf;
		sThrowIfErr(fIOUSBDeviceInterface[0]->
			GetNumberOfConfigurations(fIOUSBDeviceInterface, &numConf));

		if (!numConf)
			{
			if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZUSBDevice"))
				s.Writef("Got zero configurations");
			throw runtime_error("Got zero configurations");
			}

		IOUSBConfigurationDescriptorPtr confDesc;
		sThrowIfErr(fIOUSBDeviceInterface[0]->
			GetConfigurationDescriptorPtr(fIOUSBDeviceInterface, 0, &confDesc));

		sThrowIfErr(fIOUSBDeviceInterface[0]->
			SetConfiguration(fIOUSBDeviceInterface, confDesc->bConfigurationValue));
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
			fIOUSBDeviceInterface = nil;
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
		fIOUSBDeviceInterface = nil;
		}
	}

void ZUSBDevice::SetObserver(Observer* iObserver)
	{
	fObserver = iObserver;
	if (fDetached)
		{
		fDetached = false;
		fObserver->Detached(this);
		}
	}

IOUSBDeviceInterface182** ZUSBDevice::GetIOUSBDeviceInterface()
	{ return fIOUSBDeviceInterface; }

ZRef<ZUSBInterfaceInterface> ZUSBDevice::CreateInterfaceInterface()
	{
	IOUSBFindInterfaceRequest request; 
	request.bInterfaceClass = kIOUSBFindInterfaceDontCare; 
	request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare; 
	request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare; 
	request.bAlternateSetting = kIOUSBFindInterfaceDontCare; 

	io_iterator_t iterator;
	fIOUSBDeviceInterface[0]->CreateInterfaceIterator(fIOUSBDeviceInterface, &request, &iterator); 

	if (iterator)
		{
		io_service_t usbInterface = ::IOIteratorNext(iterator);
		::IOObjectRelease(iterator);
		if (usbInterface)
			{
			SInt32 score; 
			IOCFPlugInInterface** plugInInterface;
			::IOCreatePlugInInterfaceForService(usbInterface, 
				kIOUSBInterfaceUserClientTypeID, 
				kIOCFPlugInInterfaceID, 
				&plugInInterface, &score); 
			::IOObjectRelease(usbInterface);

			IOUSBInterfaceInterface190** theIOUSBInterfaceInterface;

			if (plugInInterface[0]->QueryInterface(plugInInterface, 
					CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID190),
					(LPVOID*)&theIOUSBInterfaceInterface))
				{
				theIOUSBInterfaceInterface = nil;
				}
			::IODestroyPlugInInterface(plugInInterface);

			if (theIOUSBInterfaceInterface)
				{
				if (noErr == theIOUSBInterfaceInterface[0]->
					USBInterfaceOpen(theIOUSBInterfaceInterface))
					{
					return new ZUSBInterfaceInterface(this, theIOUSBInterfaceInterface);
					}
				theIOUSBInterfaceInterface[0]->Release(theIOUSBInterfaceInterface);
				}
			}
		}
	return ZRef<ZUSBInterfaceInterface>();
	}

void ZUSBDevice::pDeviceNotification(
	io_service_t iService, natural_t iMessageType, void* iMessageArgument)
	{
	if (iMessageType == kIOMessageServiceIsTerminated)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Info, "ZUSBDevice"))
			s.Writef("Device removed, service: 0x%08x.", iService);

		if (fObserver)
			fObserver->Detached(this);
		else
			fDetached = true;
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
#pragma mark -
#pragma mark * ZUSBInterfaceInterface::StreamerRW

class ZUSBInterfaceInterface::StreamerRW
:	public ZStreamerRW,
	private ZStreamR,
	private ZStreamW
	{
public:
	StreamerRW(ZRef<ZUSBInterfaceInterface> iUSBII,
		IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
		int iPipeRefR, int iPipeRefW);

	virtual ~StreamerRW();

// From ZStreamerR via ZStreamerRW
	virtual const ZStreamR& GetStreamR();

// From ZStreamerW via ZStreamerRW
	virtual const ZStreamW& GetStreamW();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

private:
	ZRef<ZUSBInterfaceInterface> fUSBII;
	IOUSBInterfaceInterface190** fII;
	int fPipeRefR;
	int fPipeRefW;
	const size_t fBufferRead_Size;
	size_t fBufferRead_Offset;
	size_t fBufferRead_End;
	uint8* fBufferRead;
	};

ZUSBInterfaceInterface::StreamerRW::StreamerRW(ZRef<ZUSBInterfaceInterface> iUSBII,
	IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
	int iPipeRefR, int iPipeRefW)
:	fUSBII(iUSBII),
	fII(iIOUSBInterfaceInterface),
	fPipeRefR(iPipeRefR),
	fPipeRefW(iPipeRefW),
	fBufferRead_Size(1024),
	fBufferRead_Offset(0),
	fBufferRead_End(0),
	fBufferRead(new uint8[fBufferRead_Size])
	{}

ZUSBInterfaceInterface::StreamerRW::~StreamerRW()
	{
	delete[] fBufferRead;
	}

const ZStreamR& ZUSBInterfaceInterface::StreamerRW::GetStreamR()
	{ return *this; }

const ZStreamW& ZUSBInterfaceInterface::StreamerRW::GetStreamW()
	{ return *this; }

void ZUSBInterfaceInterface::StreamerRW::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(iDest);

	for (;;)
		{
		if (fBufferRead_End > fBufferRead_Offset)
			{
			const size_t countToCopy = min(fBufferRead_End - fBufferRead_Offset, iCount);
			ZBlockCopy(fBufferRead + fBufferRead_Offset, localDest, countToCopy);
			localDest += countToCopy;
			fBufferRead_Offset += countToCopy;
			break;
			}
		else
			{
			fBufferRead_Offset = 0;
			fBufferRead_End = 0;
			UInt32 localCount = fBufferRead_Size;
			IOReturn result = fII[0]->ReadPipeTO(
				fII, fPipeRefR, fBufferRead, &localCount, 1000, 1000000);	

			if (result == kIOUSBTransactionTimeout)
				{
				fII[0]->ClearPipeStallBothEnds(fII, fPipeRefR);
				if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug + 2,
					"ZUSBInterfaceInterface::StreamerRW"))
					{
					s << "Imp_Read, Timeout";
					}
				continue;
				}				

			if (0 == result)
				{
				if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug + 2,
					"ZUSBInterfaceInterface::StreamerRW"))
					{
					s.Writef("Imp_Read, pipe: %d, ", fPipeRefR);
					ZUtil_Strim_Data::sDumpData(s, fBufferRead, localCount);
					}
				fBufferRead_End = localCount;
				}
			else
				{
				if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug,
					"ZUSBInterfaceInterface::StreamerRW"))
					{
					s << "Imp_Read, Got result: ";

					if (result == kIOReturnNotResponding) s << "kIOReturnNotResponding";
					else if (result == kIOReturnNoDevice) s << "kIOReturnNoDevice";
					else if (result == kIOReturnBadArgument) s << "kIOReturnBadArgument";
					else if (result == kIOReturnAborted) s << "kIOReturnAborted";
					else s.Writef("%X", result);
					}
				break;
				}
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(iDest);
	}

size_t ZUSBInterfaceInterface::StreamerRW::Imp_CountReadable()
	{ return fBufferRead_End - fBufferRead_Offset; }

void ZUSBInterfaceInterface::StreamerRW::Imp_Write(
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (size_t countToWrite = min(size_t(1024), iCount))
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug + 2,
			"ZUSBInterfaceInterface::StreamerRW"))
			{
			s.Writef("Imp_Write, pipe: %d, ", fPipeRefW);
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
#pragma mark -
#pragma mark * ZUSBInterfaceInterface

ZUSBInterfaceInterface::ZUSBInterfaceInterface(
ZRef<ZUSBDevice> iUSBDevice, IOUSBInterfaceInterface190** iIOUSBInterfaceInterface)
:	fUSBDevice(iUSBDevice),
	fIOUSBInterfaceInterface(iIOUSBInterfaceInterface),
	fOpen(true)
	{}

ZUSBInterfaceInterface::~ZUSBInterfaceInterface()
	{
	if (fOpen)
		fIOUSBInterfaceInterface[0]->USBInterfaceClose(fIOUSBInterfaceInterface);
	fIOUSBInterfaceInterface[0]->Release(fIOUSBInterfaceInterface);
	fIOUSBInterfaceInterface = nil;
	}

ZRef<ZUSBDevice> ZUSBInterfaceInterface::GetUSBDevice()
	{ return fUSBDevice; }

ZRef<ZStreamerRW> ZUSBInterfaceInterface::OpenRW(int iPipeRefR, int iPipeRefW)
	{
	if (fOpen)
		return new StreamerRW(this, fIOUSBInterfaceInterface, iPipeRefR, iPipeRefW);
	return ZRef<ZStreamerRW>();
	}

void ZUSBInterfaceInterface::Close()
	{
	if (fOpen)
		{
		fOpen = false;
		fIOUSBInterfaceInterface[0]->USBInterfaceClose(fIOUSBInterfaceInterface);
		}
	}

#endif // ZCONFIG(OS, MacOSX)
