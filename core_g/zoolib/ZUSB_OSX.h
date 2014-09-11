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

#ifndef __ZUSB_OSX_h__
#define __ZUSB_OSX_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__USB_OSX
	#define ZCONFIG_API_Avail__USB_OSX ZCONFIG_SPI_Enabled(MacOSX)
#endif

#ifndef ZCONFIG_API_Desired__USB_OSX
	#define ZCONFIG_API_Desired__USB_OSX 1
#endif

#if ZCONFIG_API_Enabled(USB_OSX)

#include <IOKit/IOKitLib.h> // For IONotificationPortRef
#include <IOKit/usb/IOUSBLib.h>

#include "zoolib/ZCounted.h"
#include "zoolib/Safe.h"

#include "zoolib/Callable.h"
#include "zoolib/ZStreamer.h"

namespace ZooLib {

class ZUSBDevice;
class ZUSBInterfaceInterface;

// =================================================================================================
// MARK: - ZUSBWatcher

class ZUSBWatcher : public ZCounted
	{
public:
	ZUSBWatcher(IONotificationPortRef iIONotificationPortRef,
		SInt32 iUSBVendor, SInt32 iUSBProduct);
	virtual ~ZUSBWatcher();

	typedef Callable<void(ZRef<ZUSBDevice>)> CB_DeviceAttached;
	void SetCallable(ZRef<CB_DeviceAttached> iCallable);

private:
	void pDeviceAdded(io_iterator_t iIterator);
	static void spDeviceAdded(void* iRefcon, io_iterator_t iIterator);

	IONotificationPortRef fIONotificationPortRef;
	io_iterator_t fNotification;

	Safe<ZRef<CB_DeviceAttached> > fCallable;
	};

// ================================================================================================
#pragma mark -
#pragma mark * ZUSBDevice

class ZUSBDevice : public ZCounted
	{
public:
	ZUSBDevice(IONotificationPortRef iIONotificationPortRef, io_service_t iUSBDevice);
	virtual ~ZUSBDevice();

	typedef Callable<void(ZRef<ZUSBDevice>)> CB_DeviceDetached;
	void SetCallable(ZRef<CB_DeviceDetached> iCallable);

	IOUSBDeviceInterface182** GetIOUSBDeviceInterface();

	ZRef<ZUSBInterfaceInterface> CreateInterfaceInterface(
		CFRunLoopRef iRunLoopRef, uint8 iProtocol);

	uint16 GetIDVendor();
	uint16 GetIDProduct();

	const IOUSBDeviceDescriptor& GetDeviceDescriptor();

private:
	void pFetchDeviceDescriptor();

	void pDeviceNotification(
		io_service_t iService, natural_t iMessageType, void* iMessageArgument);

	static void spDeviceNotification(
		void* iRefcon, io_service_t iService, natural_t iMessageType, void* iMessageArgument);

	IOUSBDeviceInterface182** fIOUSBDeviceInterface;
	io_object_t fNotification;
	UInt32 fLocationID;
	bool fDetached;
	bool fHasIOUSBDeviceDescriptor;
	IOUSBDeviceDescriptor fIOUSBDeviceDescriptor;
	Safe<ZRef<CB_DeviceDetached> > fCallable;
	};

// =================================================================================================
// MARK: - ZUSBInterfaceInterface

class ZUSBInterfaceInterface : public CountedWithoutFinalize
	{
	friend class ZUSBDevice;

	ZUSBInterfaceInterface(ZRef<ZUSBDevice> iUSBDevice,
		CFRunLoopRef iRunLoopRef,
		IOUSBInterfaceInterface190** iIOUSBInterfaceInterface);

public:
	virtual ~ZUSBInterfaceInterface();

	IOUSBInterfaceInterface190** GetIOUSBInterfaceInterface();

	ZRef<ZUSBDevice> GetUSBDevice();

	ZRef<ZStreamerR> OpenR(int iPipeRefR);
	ZRef<ZStreamerW> OpenW(int iPipeRefW);

	void Close();

private:
	ZRef<ZUSBDevice> fUSBDevice;
	IOUSBInterfaceInterface190** fII;
	CFRunLoopRef fRunLoopRef;
	bool fOpen;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(USB_OSX)

#endif // __ZUSB_OSX_h__
