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

#include "zoolib/ZNet_RFCOMM_OSX.h"

#if ZCONFIG_API_Enabled(Net_RFCOMM_OSX)

// For bluetooth support we need to be on 10.2
#ifndef MAC_OS_X_VERSION_MIN_REQUIRED
#	define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_2
#endif

#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothRFCOMMChannel.h>
#import <IOBluetooth/objc/IOBluetoothSDPServiceRecord.h>

#include "zoolib/ZOBJC.h"

using namespace ZooLib;

using std::deque;
using std::min;
using std::runtime_error;

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

#include "zoolib/ZFactoryChain.h"

namespace ZANONYMOUS {

/*

bool sMake_NameLookup(ZRef<ZNetNameLookup>& oResult, ZNetName_Internet::LookupParam_t iParam)
	{
	try
		{
		oResult = new ZNetNameLookup_Internet_Socket(iParam.f0, iParam.f1, iParam.f2);
		return true;
		}
	catch (...)
		{}
	return false;
	}

ZFactoryChain_Maker_T<ZRef<ZNetNameLookup>, ZNetName_Internet::LookupParam_t>
	sMaker1(sMake_NameLookup);
*/
/*
bool sMake_Listener(ZRef<ZNetListener_RFCOMM>& oResult, ZNetListener_RFCOMM::MakeParam_t iParam)
	{
	try
		{
		oResult = new ZNetListener_RFCOMM_OSX(iParam.f0, iParam.f1, iParam.f2);
		return true;
		}
	catch (...)
		{}
	return false;
	}

ZFactoryChain_Maker_T<ZRef<ZNetListener_RFCOMM>, ZNetListener_RFCOMM::MakeParam_t>
	sMaker2(sMake_Listener);

bool sMake_Endpoint(ZRef<ZNetEndpoint_RFCOMM>& oResult, ZNetEndpoint_RFCOMM::MakeParam_t iParam)
	{
	try
		{
		oResult = new ZNetEndpoint_RFCOMM_OSX(iParam.f0, iParam.f1);
		return true;
		}
	catch (...)
		{}
	return false;
	}

ZFactoryChain_Maker_T<ZRef<ZNetEndpoint_TCP>, ZNetEndpoint_TCP::MakeParam_t>
	sMaker3(sMake_Endpoint);

*/

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Delegate_ZNetListener_RFCOMM_OSX

@interface Delegate_ZNetListener_RFCOMM_OSX : NSObject
	{
	ZNetListener_RFCOMM_OSX* fListener;
	}
@end

@implementation Delegate_ZNetListener_RFCOMM_OSX

- initWithListener:(ZNetListener_RFCOMM_OSX*)iListener
	{
	self = [super init];
	fListener = iListener;
	return self;
	}

- (void)channelOpened:(IOBluetoothUserNotification *)iNotification
	channel:(IOBluetoothRFCOMMChannel*)iChannel
	{
	fListener->pChannelOpened(iChannel);
	}
@end

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_RFCOMM_OSX

ZNetListener_RFCOMM_OSX::ZNetListener_RFCOMM_OSX(CFDictionaryRef iServiceDict)
	{
	fDelegate = [[Delegate_ZNetListener_RFCOMM_OSX alloc] initWithListener:this];
	// iServiceDict should (must?) have an entry for "0100 - ServiceName*"

	IOBluetoothSDPServiceRecordRef serviceRecordRef;
	if (kIOReturnSuccess != IOBluetoothAddServiceDict(iServiceDict, &serviceRecordRef))
		{
		DESTROY(fDelegate);
		throw runtime_error("ZNetListener_RFCOMM_OSX, couldn't initialize");
		}

	IOBluetoothSDPServiceRecord* serviceRecord
		= [IOBluetoothSDPServiceRecord withSDPServiceRecordRef:serviceRecordRef];

	[serviceRecord getRFCOMMChannelID:&fChannelID];
	[serviceRecord getServiceRecordHandle:&fHandle];

	::IOBluetoothObjectRelease(serviceRecordRef);

	fNotification = [IOBluetoothRFCOMMChannel
		registerForChannelOpenNotifications:fDelegate
		selector:@selector(channelOpened:channel:)
		withChannelID:fChannelID
		direction:kIOBluetoothUserNotificationChannelDirectionIncoming];
	}

ZNetListener_RFCOMM_OSX::~ZNetListener_RFCOMM_OSX()
	{
	if (fNotification)
		[fNotification unregister];
	fNotification = nil;

	DESTROY(fDelegate);
	}

ZRef<ZNetEndpoint> ZNetListener_RFCOMM_OSX::Listen()
	{
	ZRef<ZNetEndpoint> result;

	ZMutexLocker locker(fMutex);
	if (fQueue.empty())
		fCondition.Wait(fMutex);

	if (!fQueue.empty())
		{
		result = new ZNetEndpoint_RFCOMM_OSX(fQueue.front());
		fQueue.pop_front();
		}

	return result;
	}

void ZNetListener_RFCOMM_OSX::CancelListen()
	{
	}

void ZNetListener_RFCOMM_OSX::pChannelOpened(IOBluetoothRFCOMMChannel* iChannel)
	{
	ZMutexLocker locker(fMutex);
	[iChannel release];
	fQueue.push_back(iChannel);
	fCondition.Broadcast();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Delegate_ZNetEndpoint_RFCOMM_OSX

@interface Delegate_ZNetEndpoint_RFCOMM_OSX : NSObject
	{
	ZNetEndpoint_RFCOMM_OSX* fEndpoint;
	}
@end

@implementation Delegate_ZNetEndpoint_RFCOMM_OSX

- initWithEndpoint:(ZNetEndpoint_RFCOMM_OSX*)iEndpoint
	{
	self = [super init];
	fEndpoint = iEndpoint;
	return self;
	}

// @protocol IOBluetoothRFCOMMChannelDelegate
- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel *)rfcommChannel data:(void *)dataPointer length:(size_t)dataLength
	{
	fEndpoint->pReceived(dataPointer, dataLength);
	}

//- (void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel*)rfcommChannel status:(IOReturn)error;

- (void)rfcommChannelClosed:(IOBluetoothRFCOMMChannel *)rfcommChannel
	{
	fEndpoint->pClosed();
	}

//- (void)rfcommChannelControlSignalsChanged:(IOBluetoothRFCOMMChannel*)rfcommChannel;

//- (void)rfcommChannelFlowControlChanged:(IOBluetoothRFCOMMChannel*)rfcommChannel;

//- (void)rfcommChannelWriteComplete:(IOBluetoothRFCOMMChannel*)rfcommChannel refcon:(void*)refcon status:(IOReturn)error;

//- (void)rfcommChannelQueueSpaceAvailable:(IOBluetoothRFCOMMChannel*)rfcommChannel;


@end

// =================================================================================================

ZNetEndpoint_RFCOMM_OSX::ZNetEndpoint_RFCOMM_OSX(IOBluetoothDevice* iDevice, uint8 iChannelID)
:	fDelegate(nil),
	fChannel(nil),
	fOpen(true)
	{
	fDelegate = [[Delegate_ZNetEndpoint_RFCOMM_OSX alloc] initWithEndpoint:this];

	if (kIOReturnSuccess != [iDevice openRFCOMMChannelSync:&fChannel withChannelID:iChannelID delegate:fDelegate])
		{
		[fDelegate release];
		throw runtime_error("ZNetEndpoint_RFCOMM_OSX, couldn't connect");
		}

	[fChannel retain];
	}

ZNetEndpoint_RFCOMM_OSX::ZNetEndpoint_RFCOMM_OSX(IOBluetoothRFCOMMChannel* iChannel)
:	fDelegate(nil),
	fChannel(iChannel),
	fOpen(true)
	{
	[fChannel retain];

	fDelegate = [[Delegate_ZNetEndpoint_RFCOMM_OSX alloc] initWithEndpoint:this];

	[fChannel setDelegate: fDelegate];
	}

ZNetEndpoint_RFCOMM_OSX::~ZNetEndpoint_RFCOMM_OSX()
	{
	DESTROY(fChannel);
	DESTROY(fDelegate);
	}

const ZStreamRCon& ZNetEndpoint_RFCOMM_OSX::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& ZNetEndpoint_RFCOMM_OSX::GetStreamWCon()
	{ return *this; }

ZRef<ZNetAddress> ZNetEndpoint_RFCOMM_OSX::GetLocalAddress()
	{ return ZNetEndpoint::GetLocalAddress(); }

ZRef<ZNetAddress> ZNetEndpoint_RFCOMM_OSX::GetRemoteAddress()
	{ return ZNetEndpoint::GetRemoteAddress(); }

void ZNetEndpoint_RFCOMM_OSX::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	ZMutexLocker locker(fMutex);

	char* localDest = static_cast<char*>(iDest);

	while (iCount)
		{
		if (false)
			{
			break;
			}
		else if (fBuffer.empty())
			{			
			fCondition.Wait(fMutex);
			}
		else
			{
			const size_t countToCopy = min(iCount, fBuffer.size());

			const deque<uint8>::iterator first = fBuffer.begin();
			const deque<uint8>::iterator last = first + countToCopy;

			std::copy(first, last, localDest);
			fBuffer.erase(first, last);

			localDest += countToCopy;
			iCount -= countToCopy;

			break;
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<char*>(iDest);
	}

size_t ZNetEndpoint_RFCOMM_OSX::Imp_CountReadable()
	{
	ZMutexLocker locker(fMutex);
	return fBuffer.size();
	}

bool ZNetEndpoint_RFCOMM_OSX::Imp_WaitReadable(int iMilliseconds)
	{
	ZMutexLocker locker(fMutex);
	if (!fBuffer.size())
		fCondition.Wait(fMutex, iMilliseconds);

	return fBuffer.size();
	}

void ZNetEndpoint_RFCOMM_OSX::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const char* localSource = static_cast<const char*>(iSource);

	const BluetoothRFCOMMMTU theMTU = [fChannel getMTU];

	while (iCount)
		{
		const size_t countToWrite = min(size_t(theMTU), iCount);
		if (kIOReturnSuccess != [fChannel writeSync:const_cast<char*>(localSource) length:countToWrite])
			break;

		localSource += countToWrite;
		iCount -= countToWrite;
		}

	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const char*>(iSource);
	}

bool ZNetEndpoint_RFCOMM_OSX::Imp_ReceiveDisconnect(int iMilliseconds)
	{
	ZMutexLocker locker(fMutex);

	for (;;)
		{
		if (!fBuffer.empty())
			fBuffer.clear();

		if (!fOpen)
			return true;

		// Really want something that will track how long we waited.
		if (ZThread::errorTimeout == fCondition.Wait(fMutex, iMilliseconds))
			return false;
		}
	}

void ZNetEndpoint_RFCOMM_OSX::Imp_SendDisconnect()
	{
	[fChannel closeChannel];
	}

void ZNetEndpoint_RFCOMM_OSX::Imp_Abort()
	{
	[fChannel closeChannel];
	}

void ZNetEndpoint_RFCOMM_OSX::pReceived(const void* iSource, size_t iLength)
	{
	ZMutexLocker locker(fMutex);
	const uint8* data = static_cast<const uint8*>(iSource);
	fBuffer.insert(fBuffer.end(), data, data + iLength);
	fCondition.Broadcast();
	}

void ZNetEndpoint_RFCOMM_OSX::pClosed()
	{
	ZMutexLocker locker(fMutex);
	fOpen = false;
	fCondition.Broadcast();
	}

#endif // ZCONFIG_API_Enabled(Net_RFCOMM_OSX)
