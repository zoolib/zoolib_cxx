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

#ifndef __ZBlackBerry_OSXUSB_h__
#define __ZBlackBerry_OSXUSB_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZUSB_OSX.h"

#ifndef ZCONFIG_API_Avail__BlackBerry_OSXUSB
	#if ZCONFIG_API_Enabled(USB_OSX)
		#define ZCONFIG_API_Avail__BlackBerry_OSXUSB 1
	#endif
#endif

#ifndef ZCONFIG_API_Avail__BlackBerry_OSXUSB
	#define ZCONFIG_API_Avail__BlackBerry_OSXUSB 0
#endif

#ifndef ZCONFIG_API_Desired__BlackBerry_OSXUSB
	#define ZCONFIG_API_Desired__BlackBerry_OSXUSB 1
#endif

#include "zoolib/blackberry/ZBlackBerry.h"

#if ZCONFIG_API_Enabled(BlackBerry_OSXUSB)

#include "zoolib/ZTime.h"

#include <CoreFoundation/CFRunLoop.h> // For CFRunLoopRef
#include <mach/mach.h> // For mach_port_t

namespace ZooLib {
namespace ZBlackBerry {

// =================================================================================================
// MARK: - ZBlackBerry::Manager_OSXUSB

class Manager_OSXUSB
:	public Manager
	{
public:
	Manager_OSXUSB(CFRunLoopRef iRunLoopRef, bool iAllowMassStorage);

	virtual ~Manager_OSXUSB();

// From ZCounted
	virtual void Initialize();
	virtual void Finalize();

// From Manager
	virtual void GetDeviceIDs(std::vector<uint64>& oDeviceIDs);

	virtual ZRef<Device> Open(uint64 iDeviceID);

private:
	ZRef<ZUSBWatcher::CB_DeviceAttached> fCB_DeviceAttached;
	void pDeviceAttached(ZRef<ZUSBDevice> iDevice);

	ZRef<ZUSBDevice::CB_DeviceDetached> fCB_DeviceDetached;
	void pDeviceDetached(ZRef<ZUSBDevice> iDevice);

	ZMtxR fMutex;
	CFRunLoopRef fRunLoopRef;
	bool fAllowMassStorage;

	mach_port_t fMasterPort;
	IONotificationPortRef fIONotificationPortRef;

	ZRef<ZUSBWatcher> fUSBWatcher_Trad;
	ZRef<ZUSBWatcher> fUSBWatcher_Pearl;
	ZRef<ZUSBWatcher> fUSBWatcher_Dual;

	ZRef<ZUSBWatcher> fUSBWatcher_Trad_HS;
	ZRef<ZUSBWatcher> fUSBWatcher_Pearl_HS;
	ZRef<ZUSBWatcher> fUSBWatcher_Dual_HS;
	ZRef<ZUSBWatcher> fUSBWatcher_Storm_HS;

	struct Device_t
		{
		ZRef<ZUSBDevice> fUSBDevice;
		ZTime fInsertedAt;
		uint64 fID;
		};

	std::vector<Device_t> fDevices;
	uint64 fNextID;
	};

} // namespace ZBlackBerry
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(BlackBerry_OSXUSB)

#endif // __ZBlackBerry_OSXUSB_h__
