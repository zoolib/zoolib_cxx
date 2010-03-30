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

#include "zoolib/blackberry/ZBlackBerry_OSXUSB.h"

#if ZCONFIG_API_Enabled(BlackBerry_OSXUSB)

#include "zoolib/blackberry/ZBlackBerry_Streamer.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"

using std::runtime_error;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

namespace ZBlackBerry {

// =================================================================================================
#pragma mark -
#pragma mark * BlackBerry USB stuff

static bool spSendControlMessage(IOUSBDeviceInterface182** iUDI,
	uint8_t bRequestType, uint8_t bRequest,
	uint16_t wValue, uint16_t wIndex,
	void* bytes, unsigned int numbytes,
	unsigned int timeout)
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

static bool spSetConfiguration(IOUSBDeviceInterface182** iUDI)
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

static UInt8 spGetUsedPower(ZRef<ZUSBDevice> iUSBDevice)
	{
	if (IOUSBDeviceInterface182** theUDI = iUSBDevice->GetIOUSBDeviceInterface())
		{
		IOUSBConfigurationDescriptorPtr confDesc;
		if (noErr == theUDI[0]->GetConfigurationDescriptorPtr(theUDI, 0, &confDesc))
			return confDesc->MaxPower;
		}
	return 0;
	}

static bool spUseHighPower(ZRef<ZUSBDevice> iUSBDevice)
	{
	if (IOUSBDeviceInterface182** theUDI = iUSBDevice->GetIOUSBDeviceInterface())
		{
		char buffer[2];

		if (!spSendControlMessage(theUDI, 0xc0, 0xa5, 0, 0, buffer, 2, 100))
			return false;

		if (!spSendControlMessage(theUDI, 0x40, 0xa2, 0, 0, buffer, 0, 100))
			return false;

		return spSetConfiguration(theUDI);
		}
	return false;
	}

static void spChangeMode(ZRef<ZUSBDevice> iUSBDevice, bool iAllowMassStorage)
	{
	ZLOGFUNCTION(eDebug);

	if (IOUSBDeviceInterface182** theUDI = iUSBDevice->GetIOUSBDeviceInterface())
		{
		char buffer[2];
		
		uint16_t theValue = 0;
		if (iAllowMassStorage)
			theValue = 1;

		if (!spSendControlMessage(theUDI, 0xc0, 0xa9, theValue, 0, buffer, 2, 100))
			{
			if (ZLOG(s, eInfo, "ZBlackBerry_OSXUSB"))
				s << "spChangeMode, Failed to send message";
			}
		else
			{
			if (!spSetConfiguration(theUDI))
				{
				if (ZLOG(s, eInfo, "ZBlackBerry_OSXUSB"))
					s << "spChangeMode, Failed to set configuration";
				}
			else
				{
				theUDI[0]->ResetDevice(theUDI);
				}
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Manager_OSXUSB

Manager_OSXUSB::Manager_OSXUSB(CFRunLoopRef iRunLoopRef, bool iAllowMassStorage)
:	fRunLoopRef(iRunLoopRef),
	fAllowMassStorage(iAllowMassStorage),
	fMasterPort(0),
	fIONotificationPortRef(nullptr),
	fNextID(1)
	{
	kern_return_t kr = ::IOMasterPort(MACH_PORT_NULL, &fMasterPort);
	if (kr || !fMasterPort)
		{
		if (ZLOG(s, eErr, "ZBlackBerry::Manager_OSXUSB"))
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
	if (ZLOG(s, eDebug, "ZBlackBerry::Manager_OSXUSB"))
		s << "Start";

	fIONotificationPortRef = ::IONotificationPortCreate(fMasterPort);

	fUSBWatcher_Trad = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 1);
	fUSBWatcher_Trad->SetObserver(this);

	fUSBWatcher_Pearl = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 6);
	fUSBWatcher_Pearl->SetObserver(this);

	fUSBWatcher_Dual = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 4);
	fUSBWatcher_Dual->SetObserver(this);

	fUSBWatcher_Trad_HS = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 0x8001);
	fUSBWatcher_Trad_HS->SetObserver(this);

	fUSBWatcher_Pearl_HS = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 0x8006);
	fUSBWatcher_Pearl_HS->SetObserver(this);

	fUSBWatcher_Dual_HS = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 0x8004);
	fUSBWatcher_Dual_HS->SetObserver(this);

	fUSBWatcher_Storm_HS = new ZUSBWatcher(fIONotificationPortRef, 0xFCA, 0x8007);
	fUSBWatcher_Storm_HS->SetObserver(this);

	::CFRunLoopAddSource(fRunLoopRef,
		::IONotificationPortGetRunLoopSource(fIONotificationPortRef),
		kCFRunLoopDefaultMode);

	Manager::Start();
	}

void Manager_OSXUSB::Stop()
	{
	if (ZLOG(s, eDebug, "ZBlackBerry::Manager_OSXUSB"))
		s << "Stop";

	::CFRunLoopRemoveSource(fRunLoopRef,
		::IONotificationPortGetRunLoopSource(fIONotificationPortRef),
		kCFRunLoopDefaultMode);

	fUSBWatcher_Trad.Clear();
	fUSBWatcher_Pearl.Clear();
	fUSBWatcher_Dual.Clear();

	fUSBWatcher_Trad_HS.Clear();
	fUSBWatcher_Pearl_HS.Clear();
	fUSBWatcher_Dual_HS.Clear();
	fUSBWatcher_Storm_HS.Clear();

	if (fIONotificationPortRef)
		{
		::IONotificationPortDestroy(fIONotificationPortRef);
		fIONotificationPortRef = nullptr;
		}
	Manager::Stop();
	}

void Manager_OSXUSB::GetDeviceIDs(vector<uint64>& oDeviceIDs)
	{
	ZMutexLocker locker(fMutex);
	const ZTime atMostThis = ZTime::sSystem();
	for (vector<Device_t>::iterator i = fDevices.begin(); i != fDevices.end(); ++i)
		{
		// See comment in Added as to why this check is bypassed.
		if (true || i->fInsertedAt <= atMostThis)
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
				if (ZRef<ZUSBInterfaceInterface> theII =
					theUSBDevice->CreateInterfaceInterface(fRunLoopRef, 0xFF))
					{
					if (ZRef<ZStreamerR> theSR = theII->OpenR(3))
						{
						if (ZRef<ZStreamerW> theSW = theII->OpenW(4))
							{
							ZRef<Device_Streamer> theDevice = new Device_Streamer(theSR, theSW);
							sStartCommerRunners(theDevice);
							return theDevice;
							}
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
		if (ZLOG(s, eNotice, "ZBlackBerry::Manager_OSXUSB"))
			s << "Added, invoked with null device, PocketMac drivers installed?";
//##		if (fPocketMacDriverInstalled)
//##			fObserver->AddFailed();
		return;
		}

	uint16 theIDVendor = iUSBDevice->GetIDVendor();
	uint16 theIDProduct = iUSBDevice->GetIDProduct();
	const IOUSBDeviceDescriptor theDescriptor = iUSBDevice->GetDeviceDescriptor();

	if (ZLOG(s, eDebug, "ZBlackBerry::Manager_OSXUSB"))
		s.Writef("Added, IDVendor: 0x%X, IDProduct: 0x%X", theIDVendor, theIDProduct);

	if (theIDVendor != 0xFCA)
		{
		if (ZLOG(s, eDebug, "ZBlackBerry::Manager_OSXUSB"))
			s << "theIDVendor != 0xFCA";
		return;
		}

	const UInt8 usedPower = spGetUsedPower(iUSBDevice);
	if (ZLOG(s, eDebug, "ZBlackBerry::Manager_OSXUSB"))
		s.Writef("Used power: %d", usedPower);

	if (usedPower < 250)
		{
		// Note that call to spUseHighPower causes our connection to reset,
		// so this device will disappear and be rediscovered.
		if (!spUseHighPower(iUSBDevice))
			{
			if (ZLOG(s, eDebug, "ZBlackBerry::Manager_OSXUSB"))
				s << "spUseHighPower returned false";
			}
		else
			{
			const UInt8 usedPower = spGetUsedPower(iUSBDevice);
			if (ZLOG(s, eDebug, "ZBlackBerry::Manager_OSXUSB"))
				s.Writef("After calling UseHighPower, used power: %d", usedPower);
			}
		}

	if (theIDProduct == 6 || theIDProduct == 0x8006)
		{
		if (ZLOG(s, eDebug, "ZBlackBerry::Manager_OSXUSB"))
			s << "Mass storage only, changing modes.";

		spChangeMode(iUSBDevice, fAllowMassStorage);

		return;
		}

	ZMutexLocker locker(fMutex);
	Device_t theD;
	theD.fID = fNextID++;
	theD.fUSBDevice = iUSBDevice;
	
	if (usedPower < 250)
		{
		// We're not on full power, and so may have reconfigured the device to
		// request full power, in which case it'll be resetting and we should
		// not return it in our roster for a couple of seconds. That said,
		// I've disabled the check of this value in GetDeviceIDs because we don't
		// trip an observer notification in two seconds if the device didn't
		// actually reset, and so any observer won't know to check in with us.
		theD.fInsertedAt = ZTime::sSystem() + 2;
		}
	else
		{
		theD.fInsertedAt = ZTime::sSystem();
		}

	fDevices.push_back(theD);
	locker.Release();

	iUSBDevice->SetObserver(this);

	this->pNotifyObservers();
	}

void Manager_OSXUSB::Detached(ZRef<ZUSBDevice> iUSBDevice)
	{
	// Trip if we recurse somehow.
	ZAssert(!fMutex.IsLocked());

	if (ZLOG(s, eDebug, "ZBlackBerry::Manager_OSXUSB"))
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

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(BlackBerry_OSXUSB)
