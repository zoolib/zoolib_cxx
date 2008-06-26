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

#ifndef __ZNet_RFCOMM_OSX__
#define __ZNet_RFCOMM_OSX__ 1
#include "zconfig.h"
#include "ZCONFIG_API.h"
#include "ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Net_RFCOMM_OSX
#	define ZCONFIG_API_Avail__Net_RFCOMM_OSX ZCONFIG_SPI_Enabled(MacOSX)
#endif

#ifndef ZCONFIG_API_Desired__Net_RFCOMM_OSX
#	define ZCONFIG_API_Desired__Net_RFCOMM_OSX 1
#endif

#include "ZNet_RFCOMM.h"

#if ZCONFIG_API_Enabled(Net_RFCOMM_OSX)

#include <deque>

#include <CoreFoundation/CFDictionary.h>
#include <IOBluetooth/Bluetooth.h>

#ifdef __OBJC__
	// We're being included in ObjC/ObjC++ code, so use appropriate forward declarations.
	@class IOBluetoothDevice;
	@class IOBluetoothRFCOMMChannel;
	@class IOBluetoothUserNotification;

	@class Delegate_ZNetListener_RFCOMM_OSX;
	@class Delegate_ZNetEndpoint_RFCOMM_OSX;
#else
	// Otherwise pretend its something compatible.
	#include <IOBluetooth/IOBluetoothUserLib.h>
	typedef OpaqueIOBluetoothObjectRef IOBluetoothDevice;
	typedef OpaqueIOBluetoothObjectRef IOBluetoothRFCOMMChannel;
	typedef OpaqueIOBluetoothObjectRef IOBluetoothUserNotification;

	typedef void* Delegate_ZNetListener_RFCOMM_OSX;
	typedef void* Delegate_ZNetEndpoint_RFCOMM_OSX;
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_RFCOMM_OSX

class ZNetListener_RFCOMM_OSX : public ZNetListener_RFCOMM
	{
public:
	ZNetListener_RFCOMM_OSX(CFDictionaryRef iServiceDict);
	virtual ~ZNetListener_RFCOMM_OSX();

// From ZNetListener via ZNetListener_RFCOMM
	virtual ZRef<ZNetEndpoint> Listen();
	virtual void CancelListen();

	void pChannelOpened(IOBluetoothRFCOMMChannel* iChannel);

private:
	Delegate_ZNetListener_RFCOMM_OSX* fDelegate;

	BluetoothRFCOMMChannelID fChannelID;
	BluetoothSDPServiceRecordHandle fHandle;
	IOBluetoothUserNotification* fNotification;

	ZMutex fMutex;
	ZCondition fCondition;
	deque<IOBluetoothRFCOMMChannel*> fQueue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_RFCOMM_OSX

class ZNetEndpoint_RFCOMM_OSX
:	public ZNetEndpoint_RFCOMM,
	private ZStreamRCon,
	private ZStreamWCon
	{
public:
	ZNetEndpoint_RFCOMM_OSX(IOBluetoothDevice* iDevice, uint8 iChannelID);
	ZNetEndpoint_RFCOMM_OSX(IOBluetoothRFCOMMChannel* iChannel);

	virtual ~ZNetEndpoint_RFCOMM_OSX();

// From ZStreamerRCon via ZNetListener_RFCOMM
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZNetListener_RFCOMM
	virtual const ZStreamWCon& GetStreamWCon();

// From ZNetEndpoint via ZNetListener_RFCOMM
	virtual ZRef<ZNetAddress> GetLocalAddress();
	virtual ZRef<ZNetAddress> GetRemoteAddress();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamRCon
	virtual bool Imp_WaitReadable(int iMilliseconds);
	virtual bool Imp_ReceiveDisconnect(int iMilliseconds);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

// Private, but needs to be accessible to our delegate
	void pReceived(const void* iSource, size_t iLength);
	void pClosed();

private:
	Delegate_ZNetEndpoint_RFCOMM_OSX* fDelegate;
	
//	void pEventCallback(IOBluetoothRFCOMMChannelEvent* iEvent);
//	static void spEventCallback(
//		IOBluetoothRFCOMMChannelRef iChannel,
//		void* iRefcon, IOBluetoothRFCOMMChannelEvent* iEvent);

	IOBluetoothRFCOMMChannel* fChannel;
	ZMutex fMutex;
	ZCondition fCondition;
	deque<uint8> fBuffer;
	bool fOpen;
	};

#endif // ZCONFIG_API_Enabled(Net_RFCOMM_OSX)

#endif // __ZNet_RFCOMM_OSX__
