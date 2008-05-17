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

#include "ZBlackBerry_OSXUSB.h"

#if ZCONFIG_SPI_Enabled(MacOSX)

#include "ZBlackBerry_Streamer.h"

#include "ZLog.h"
#include "ZMemory.h"

namespace ZBlackBerry {

// =================================================================================================
#pragma mark -
#pragma mark * BlackBerry USB stuff

static bool sSendControlMessage(IOUSBDeviceInterface182** iUDI,
	uint8_t bRequestType, uint8_t bRequest,
	uint16_t wValue, uint16_t wIndex, void* bytes,
	unsigned int numbytes, unsigned int timeout)
	{
	IOUSBDevRequestTO urequest;
	ZBlockZero(&urequest, sizeof(urequest));

	urequest.bmRequestType = bRequestType;
	urequest.bRequest = bRequest;
	urequest.wValue = wValue;
	urequest.wIndex = wIndex;
	urequest.wLength = numbytes;
	urequest.pData = bytes;
	urequest.completionTimeout = timeout;

	return noErr == iUDI[0]->DeviceRequestTO(iUDI, &urequest);
	}

static bool sSetConfiguration(IOUSBDeviceInterface182** iUDI)
	{
	IOUSBConfigurationDescriptorPtr confDesc;
	if (0 == iUDI[0]->GetConfigurationDescriptorPtr(iUDI, 0, &confDesc))
		{
		if (0 == iUDI[0]->SetConfiguration(iUDI, confDesc->bConfigurationValue))
			{
			return true;
			}
		}
	return false;
	}
static UInt8 sGetUsedPower(ZRef<ZUSBDevice> iUSBDevice)
	{
	if (IOUSBDeviceInterface182** theUDI = iUSBDevice->GetIOUSBDeviceInterface())
		{
		IOUSBConfigurationDescriptorPtr confDesc;
		if (noErr == theUDI[0]->GetConfigurationDescriptorPtr(theUDI, 0, &confDesc))
			return confDesc->MaxPower;
		}
	return 0;
	}

static bool sUseHighPower(ZRef<ZUSBDevice> iUSBDevice)
	{
	if (IOUSBDeviceInterface182** theUDI = iUSBDevice->GetIOUSBDeviceInterface())
		{
		char buffer[2];

		if (!sSendControlMessage(theUDI, 0xc0, 0xa5, 0, 1, buffer, 2, 100))
			return false;

		if (!sSendControlMessage(theUDI, 0x40, 0xa2, 0, 1, buffer, 0, 100))
			return false;

		return sSetConfiguration(theUDI);
		}
	return false;
	}

static void sChangeMode(ZRef<ZUSBDevice> iUSBDevice)
	{
	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry_OSXUSB"))
		s << "sChangeMode, Enter";

	if (IOUSBDeviceInterface182** theUDI = iUSBDevice->GetIOUSBDeviceInterface())
		{
		char buffer[2];
		
		// This line sets the operational mode to be Product ID 1
		if (!sSendControlMessage(theUDI, 0xc0, 0xa9, 0, 1, buffer, 2, 100))
		// whereas this sets it to be Product ID 4 (dual mode)
		// if (!sSendControlMessage(theUDI, 0xc0, 0xa9, 1, 1, buffer, 2, 100))
			{
			if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Info, "ZBlackBerry_OSXUSB"))
				s << "sChangeMode, Failed to send message";
			}
		else
			{
			if (!sSetConfiguration(theUDI))
				{
				if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Info, "ZBlackBerry_OSXUSB"))
					s << "sChangeMode, Failed to set configuration";
				}
			else
				{
				theUDI[0]->ResetDevice(theUDI);
				}
			}
		}

	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry_OSXUSB"))
		s << "sChangeMode, Exit";
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Manager_OSXUSB

Manager_OSXUSB::Manager_OSXUSB()
:	fMasterPort(0),
	fIONotificationPortRef(nil),
	fNextID(1)
	{
	kern_return_t kr = ::IOMasterPort(MACH_PORT_NULL, &fMasterPort);
	if (kr || !fMasterPort)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Err, "ZBlackBerry::Manager_OSXUSB"))
			s.Writef("Constructor, couldn't create a master IOKit Port (%08x)\n", kr);
		throw runtime_error("Manager_OSXUSB");
		}
	}

Manager_OSXUSB::~Manager_OSXUSB()
	{
	if (fMasterPort)
		::mach_port_deallocate(::mach_task_self(), fMasterPort);
	}

void Manager_OSXUSB::Start()
	{
	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
		s << "Start";

	CFRunLoopRef theRunLoopRef = ::CFRunLoopGetCurrent();

	fIONotificationPortRef = ::IONotificationPortCreate(fMasterPort);
	::CFRunLoopAddSource(theRunLoopRef,
		::IONotificationPortGetRunLoopSource(fIONotificationPortRef), kCFRunLoopDefaultMode);

	fUSBWatcher_Traditional = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 1);
	fUSBWatcher_Traditional->SetObserver(this);

	fUSBWatcher_Pearl = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 6);
	fUSBWatcher_Pearl->SetObserver(this);

	fUSBWatcher_PearlDual = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 4);
	fUSBWatcher_PearlDual->SetObserver(this);

	fUSBWatcher_Pearl8120 = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 0x8004);
	fUSBWatcher_Pearl8120->SetObserver(this);

	Manager::Start();
	}

void Manager_OSXUSB::Stop()
	{
	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
		s << "Stop";

	fUSBWatcher_Traditional.Clear();
	fUSBWatcher_Pearl.Clear();
	fUSBWatcher_PearlDual.Clear();
	fUSBWatcher_Pearl8120.Clear();

	if (fIONotificationPortRef)
		{
		::IONotificationPortDestroy(fIONotificationPortRef);
		fIONotificationPortRef = nil;
		}
	Manager::Stop();
	}

void Manager_OSXUSB::GetDeviceIDs(vector<uint64>& oDeviceIDs)
	{
	ZMutexLocker locker(fMutex);
	const ZTime atMostThis = ZTime::sSystem();
	for (vector<Device_t>::iterator i = fDevices.begin(); i != fDevices.end(); ++i)
		{
		if (i->fInsertedAt <= atMostThis)
			oDeviceIDs.push_back(i->fID);
		}
	}

ZRef<Device> Manager_OSXUSB::Open(uint64 iDeviceID)
	{
	ZMutexLocker locker(fMutex);
	for (vector<Device_t>::iterator i = fDevices.begin(); i != fDevices.end(); ++i)
		{
		if (i->fID == iDeviceID)
			{
			if (ZRef<ZUSBDevice> theUSBDevice = i->fUSBDevice)
				{
				if (ZRef<ZUSBInterfaceInterface> theII = theUSBDevice->CreateInterfaceInterface())
					{
					if (ZRef<ZStreamerRW> theSRW = theII->OpenRW(3, 4))
						{
						ZRef<Device_Streamer> theDevice = new Device_Streamer;
						sStartRunners(theDevice.GetObject(), theSRW, theSRW);
						return theDevice;
						}
					}
				}
			}
		}
	return ZRef<Device>();
	}

void Manager_OSXUSB::Added(ZRef<ZUSBDevice> iUSBDevice)
	{
	if (!iUSBDevice)
		{
		// Watcher fired, but could not create a USBDevice.
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Notice, "ZBlackBerry::Manager_OSXUSB"))
			s << "Added, invoked with null device, PocketMac drivers installed?";
//##		if (fPocketMacDriverInstalled)
//##			fObserver->AddFailed();
		return;
		}

	uint16 theIDVendor = iUSBDevice->GetIDVendor();
	uint16 theIDProduct = iUSBDevice->GetIDProduct();

	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
		s.Writef("Added, IDVendor: 0x%X, IDProduct: 0x%X", theIDVendor, theIDProduct);

	if (theIDVendor != 0xFCA)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
			s << "theIDVendor != 0xFCA";
		return;
		}

	if (theIDProduct == 6)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
			s << "Got an 8xxx series, changing modes";

		sChangeMode(iUSBDevice);

		return;
		}

	if (theIDProduct == 4)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
			s << "Got an 8xxx series, dual mode";
		}

	if (theIDProduct == 0x8004)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
			s << "Got an 8120";
		}

	if (theIDProduct == 1)
		{
		if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
			s << "Got an older BlackBerry";
		}

#if 1
	const UInt8 usedPower = sGetUsedPower(iUSBDevice);
	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
		s.Writef("Used power: %d", usedPower);

	if (usedPower < 250)
		{
		// Note that call to sUseHighPower causes our connection to reset,
		// so this device will disappear and be rediscovered.
		if (!sUseHighPower(iUSBDevice))
			{
			if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
				s << "UseHighPower returned false";
			}
		else
			{
			const UInt8 usedPower = sGetUsedPower(iUSBDevice);
			if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
				s.Writef("After calling UseHighPower, used power: %d", usedPower);
			}
		}
#endif

	ZMutexLocker locker(fMutex);
	Device_t theD;
	theD.fID = fNextID++;
	theD.fUSBDevice = iUSBDevice;
	if (usedPower < 250)
		theD.fInsertedAt = ZTime::sSystem() + 2;
	else
		theD.fInsertedAt = ZTime::sSystem();
	fDevices.push_back(theD);
	locker.Release();

	iUSBDevice->SetObserver(this);

	this->pNotifyObservers();
	}

void Manager_OSXUSB::Detached(ZRef<ZUSBDevice> iUSBDevice)
	{
	// Trip if we recurse somehow.
	ZAssert(!fMutex.IsLocked());

	if (const ZLog::S& s = ZLog::S(ZLog::ePriority_Debug, "ZBlackBerry::Manager_OSXUSB"))
		s << "Detached";

	ZMutexLocker locker(fMutex);
	bool gotIt = false;
	for (vector<Device_t>::iterator i = fDevices.begin(); i != fDevices.end(); ++i)
		{
		if (i->fUSBDevice == iUSBDevice)
			{
			fDevices.erase(i);
			gotIt = true;
			break;
			}
		}
	ZAssert(gotIt);
		
	locker.Release();

	this->pNotifyObservers();
	}

// =================================================================================================

} // namespace ZBlackBerry

#endif // ZCONFIG_SPI_Enabled(MacOSX)
