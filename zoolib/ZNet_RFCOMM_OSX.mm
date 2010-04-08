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

#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothRFCOMMChannel.h>
#import <IOBluetooth/objc/IOBluetoothSDPServiceRecord.h>

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZOBJC.h"

using std::deque;
using std::min;
using std::runtime_error;

NAMESPACE_ZOOLIB_USING

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
- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel *)rfcommChannel
	data:(void *)dataPointer length:(size_t)dataLength
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

//- (void)rfcommChannelWriteComplete:(IOBluetoothRFCOMMChannel*)rfcommChannel
//	refcon:(void*)refcon status:(IOReturn)error;

//- (void)rfcommChannelQueueSpaceAvailable:(IOBluetoothRFCOMMChannel*)rfcommChannel;


@end

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

namespace ZANONYMOUS {

} // anonymous namespace

NAMESPACE_ZOOLIB_BEGIN

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

	IOBluetoothSDPServiceRecord* serviceRecord =
		[IOBluetoothSDPServiceRecord withSDPServiceRecordRef:serviceRecordRef];

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
	fNotification = nullptr;

	DESTROY(fDelegate);
	}

ZRef<ZNetEndpoint> ZNetListener_RFCOMM_OSX::Listen()
	{
	ZRef<ZNetEndpoint> result;

	ZGuardMtx locker(fMutex);
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
	fCondition.Broadcast();
	}

void ZNetListener_RFCOMM_OSX::pChannelOpened(IOBluetoothRFCOMMChannel* iChannel)
	{
	ZGuardMtx locker(fMutex);
	[iChannel release]; // ??
	fQueue.push_back(iChannel);
	fCondition.Broadcast();
	}

// =================================================================================================

ZNetEndpoint_RFCOMM_OSX::ZNetEndpoint_RFCOMM_OSX(IOBluetoothDevice* iDevice, uint8 iChannelID)
:	fDelegate(nullptr),
	fChannel(nullptr),
	fOpen(true)
	{
	fDelegate = [[Delegate_ZNetEndpoint_RFCOMM_OSX alloc] initWithEndpoint:this];

	if (kIOReturnSuccess
		!= [iDevice openRFCOMMChannelSync:&fChannel withChannelID:iChannelID delegate:fDelegate])
		{
		[fDelegate release];
		throw runtime_error("ZNetEndpoint_RFCOMM_OSX, couldn't connect");
		}

	[fChannel retain];
	}

ZNetEndpoint_RFCOMM_OSX::ZNetEndpoint_RFCOMM_OSX(IOBluetoothRFCOMMChannel* iChannel)
:	fDelegate(nullptr),
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
	ZGuardMtx locker(fMutex);

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
	ZGuardMtx locker(fMutex);
	return fBuffer.size();
	}

bool ZNetEndpoint_RFCOMM_OSX::Imp_WaitReadable(double iTimeout)
	{
	const ZTime deadline = ZTime::sSystem() + iTimeout;
	ZGuardMtx locker(fMutex);
	for (;;)
		{
		if (fBuffer.size())
			return true;
		if (!fCondition.WaitUntil(fMutex, deadline))
			return false;
		}
	}

void ZNetEndpoint_RFCOMM_OSX::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const char* localSource = static_cast<const char*>(iSource);

	const BluetoothRFCOMMMTU theMTU = [fChannel getMTU];

	while (iCount)
		{
		const size_t countToWrite = min(size_t(theMTU), iCount);
		if (kIOReturnSuccess
			!= [fChannel writeSync:const_cast<char*>(localSource) length:countToWrite])
			{
			break;
			}

		localSource += countToWrite;
		iCount -= countToWrite;
		}

	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const char*>(iSource);
	}

bool ZNetEndpoint_RFCOMM_OSX::Imp_ReceiveDisconnect(double iTimeout)
	{
	const ZTime deadline = ZTime::sSystem() + iTimeout;
	ZGuardMtx locker(fMutex);
	for (;;)
		{
		if (!fBuffer.empty())
			fBuffer.clear();

		if (!fOpen)
			return true;

		if (!fCondition.WaitUntil(fMutex, deadline))
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
	ZGuardMtx locker(fMutex);
	const uint8* data = static_cast<const uint8*>(iSource);
	fBuffer.insert(fBuffer.end(), data, data + iLength);
	fCondition.Broadcast();
	}

void ZNetEndpoint_RFCOMM_OSX::pClosed()
	{
	ZGuardMtx locker(fMutex);
	fOpen = false;
	fCondition.Broadcast();
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Net_RFCOMM_OSX)
