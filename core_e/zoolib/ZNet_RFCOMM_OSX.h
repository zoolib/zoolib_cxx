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

#ifndef __ZNet_RFCOMM_OSX_h__
#define __ZNet_RFCOMM_OSX_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

// RFCOMM_OSX is only available when building for 10.2 and later
#ifndef ZCONFIG_API_Avail__Net_RFCOMM_OSX
	#if ZCONFIG_SPI_Enabled(MacOSX)
		#include <AvailabilityMacros.h>
		#if defined(MAC_OS_X_VERSION_MIN_REQUIRED)
			#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
				#define ZCONFIG_API_Avail__Net_RFCOMM_OSX 1
			#endif
		#endif
	#endif
#endif

#ifndef ZCONFIG_API_Avail__Net_RFCOMM_OSX
	#define ZCONFIG_API_Avail__Net_RFCOMM_OSX 0
#endif

#ifndef ZCONFIG_API_Desired__Net_RFCOMM_OSX
	#define ZCONFIG_API_Desired__Net_RFCOMM_OSX 1
#endif

#include "zoolib/ZNet_RFCOMM.h"

#if ZCONFIG_API_Enabled(Net_RFCOMM_OSX)

#include "zoolib/ZDelegate.h"
#include "zoolib/ZThread.h"

#include <deque>

#include <CoreFoundation/CFDictionary.h>
#include <IOBluetooth/Bluetooth.h>

#ifdef __OBJC__

	// We're being included in ObjC/ObjC++ code, so use appropriate forward declarations.
	@class IOBluetoothDevice;
	@class IOBluetoothRFCOMMChannel;
	@class IOBluetoothUserNotification;

#else

	#include <IOBluetooth/IOBluetoothUserLib.h>

	typedef OpaqueIOBluetoothObjectRef IOBluetoothDevice;
	typedef OpaqueIOBluetoothObjectRef IOBluetoothRFCOMMChannel;
	typedef OpaqueIOBluetoothObjectRef IOBluetoothUserNotification;

#endif

namespace ZooLib {

// =================================================================================================
// MARK: - ZNetListener_RFCOMM_OSX

class ZNetListener_RFCOMM_OSX : public ZNetListener_RFCOMM
	{
public:
	ZNetListener_RFCOMM_OSX(CFDictionaryRef iServiceDict);
	virtual ~ZNetListener_RFCOMM_OSX();

// From ZNetListener via ZNetListener_RFCOMM
	virtual ZRef<ZNetEndpoint> Listen();
	virtual void CancelListen();

private:
	void pChannelOpened(IOBluetoothUserNotification*, IOBluetoothRFCOMMChannel* iChannel);

	ZDelegate fDelegate;

	BluetoothRFCOMMChannelID fChannelID;
	BluetoothSDPServiceRecordHandle fHandle;
	IOBluetoothUserNotification* fNotification;

	ZMtx fMutex;
	ZCnd fCondition;
	std::deque<IOBluetoothRFCOMMChannel*> fQueue;
	};

// =================================================================================================
// MARK: - ZNetEndpoint_RFCOMM_OSX

class ZNetEndpoint_RFCOMM_OSX
:	public ZNetEndpoint_RFCOMM,
	private ZStreamRCon,
	private ZStreamWCon
	{
public:
	ZNetEndpoint_RFCOMM_OSX(IOBluetoothDevice* iDevice, uint8 iChannelID);
	ZNetEndpoint_RFCOMM_OSX(IOBluetoothRFCOMMChannel* iChannel);

	virtual ~ZNetEndpoint_RFCOMM_OSX();

// From ZStreamerRCon via ZNetEndpoint_RFCOMM
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZNetEndpoint_RFCOMM
	virtual const ZStreamWCon& GetStreamWCon();

// From ZNetEndpoint via ZNetEndpoint_RFCOMM
	virtual ZRef<ZNetAddress> GetLocalAddress();
	virtual ZRef<ZNetAddress> GetRemoteAddress();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(double iTimeout);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	void pReceived(IOBluetoothRFCOMMChannel*, void* iSource, size_t iLength);
	void pClosed(IOBluetoothRFCOMMChannel*);

	ZDelegate fDelegate;

	IOBluetoothRFCOMMChannel* fChannel;
	ZMtx fMutex;
	ZCnd fCondition;
	std::deque<uint8> fBuffer;
	bool fOpen;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_RFCOMM_OSX)

#endif // __ZNet_RFCOMM_OSX_h__
