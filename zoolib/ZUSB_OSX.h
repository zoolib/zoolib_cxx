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

#ifndef __ZUSB_OSX__
#define __ZUSB_OSX__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__USB_OSX
#	define ZCONFIG_API_Avail__USB_OSX ZCONFIG_SPI_Enabled(MacOSX)
#endif

#ifndef ZCONFIG_API_Desired__USB_OSX
#	define ZCONFIG_API_Desired__USB_OSX 1
#endif

#if ZCONFIG_API_Enabled(USB_OSX)

#include <IOKit/IOKitLib.h> // For IONotificationPortRef
#include <IOKit/usb/IOUSBLib.h>

#include "zoolib/ZCallback_T.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZStreamer.h"

namespace ZooLib {

class ZUSBDevice;
class ZUSBInterfaceInterface;

// =================================================================================================
#pragma mark -
#pragma mark * ZUSBWatcher

class ZUSBWatcher : public ZCounted
	{
public:
	ZUSBWatcher(IONotificationPortRef iIONotificationPortRef,
		SInt32 iUSBVendor, SInt32 iUSBProduct);
	virtual ~ZUSBWatcher();

	typedef ZCallback_T<ZRef<ZUSBDevice> > CB_DeviceAttached;
	void RegisterDeviceAttached(ZRef<CB_DeviceAttached> iCallback);
	void UnregisterDeviceAttached(ZRef<CB_DeviceAttached> iCallback);

private:
	void pDeviceAdded(io_iterator_t iIterator);
	static void spDeviceAdded(void* iRefcon, io_iterator_t iIterator);

	IONotificationPortRef fIONotificationPortRef;
	io_iterator_t fNotification;

	ZCallbackSet_T<ZRef<ZUSBDevice> > fCallbacks;
	};

// ================================================================================================
#pragma mark -
#pragma mark * ZUSBDevice

class ZUSBDevice : public ZCounted
	{
public:
	ZUSBDevice(IONotificationPortRef iIONotificationPortRef, io_service_t iUSBDevice);
	virtual ~ZUSBDevice();

	typedef ZCallback_T<ZRef<ZUSBDevice> > CB_DeviceDetached;
	void RegisterDeviceDetached(ZRef<CB_DeviceDetached> iCallback);
	void UnregisterDeviceDetached(ZRef<CB_DeviceDetached> iCallback);

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
	ZCallbackSet_T<ZRef<ZUSBDevice> > fCallbacks;
	bool fDetached;
	bool fHasIOUSBDeviceDescriptor;
	IOUSBDeviceDescriptor fIOUSBDeviceDescriptor;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUSBInterfaceInterface

class ZUSBInterfaceInterface : public ZCountedWithoutFinalize
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

#endif // __ZUSB_OSX__
