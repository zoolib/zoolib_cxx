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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZObjC.h"

using std::deque;
using std::min;
using std::runtime_error;

namespace ZooLib {

// =================================================================================================
// MARK: - ZNetListener_RFCOMM_OSX

ZNetListener_RFCOMM_OSX::ZNetListener_RFCOMM_OSX(CFDictionaryRef iServiceDict)
	{
	fDelegate.Set(@selector(channelOpened:channel:),
		sCallable(this, &ZNetListener_RFCOMM_OSX::pChannelOpened));

	// iServiceDict should (must?) have an entry for "0100 - ServiceName*"

	IOBluetoothSDPServiceRecordRef serviceRecordRef;
	if (kIOReturnSuccess != IOBluetoothAddServiceDict(iServiceDict, &serviceRecordRef))
		{
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
	}

ZRef<ZNetEndpoint> ZNetListener_RFCOMM_OSX::Listen()
	{
	ZRef<ZNetEndpoint> result;

	ZAcqMtx acq(fMutex);
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

void ZNetListener_RFCOMM_OSX::pChannelOpened(
	IOBluetoothUserNotification*, IOBluetoothRFCOMMChannel* iChannel)
	{
	ZAcqMtx acq(fMutex);
	[iChannel release]; // ??
	fQueue.push_back(iChannel);
	fCondition.Broadcast();
	}

// =================================================================================================

ZNetEndpoint_RFCOMM_OSX::ZNetEndpoint_RFCOMM_OSX(IOBluetoothDevice* iDevice, uint8 iChannelID)
:	fChannel(nullptr)
,	fOpen(true)
	{
	fDelegate.Set(@selector(rfcommChannelData:data:length:),
		sCallable(this, &ZNetEndpoint_RFCOMM_OSX::pReceived));

	fDelegate.Set(@selector(rfcommChannelClosed:),
		sCallable(this, &ZNetEndpoint_RFCOMM_OSX::pClosed));

	if (kIOReturnSuccess !=
		[iDevice openRFCOMMChannelSync:&fChannel withChannelID:iChannelID delegate:fDelegate])
		{
		throw runtime_error("ZNetEndpoint_RFCOMM_OSX, couldn't connect");
		}
	}

ZNetEndpoint_RFCOMM_OSX::ZNetEndpoint_RFCOMM_OSX(IOBluetoothRFCOMMChannel* iChannel)
:	fChannel(iChannel)
,	fOpen(true)
	{
	[fChannel retain];
	[fChannel setDelegate:fDelegate];
	}

ZNetEndpoint_RFCOMM_OSX::~ZNetEndpoint_RFCOMM_OSX()
	{ [fChannel release]; }

const ZStreamRCon& ZNetEndpoint_RFCOMM_OSX::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& ZNetEndpoint_RFCOMM_OSX::GetStreamWCon()
	{ return *this; }

ZRef<ZNetAddress> ZNetEndpoint_RFCOMM_OSX::GetLocalAddress()
	{ return ZNetEndpoint::GetLocalAddress(); }

ZRef<ZNetAddress> ZNetEndpoint_RFCOMM_OSX::GetRemoteAddress()
	{ return ZNetEndpoint::GetRemoteAddress(); }

void ZNetEndpoint_RFCOMM_OSX::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	ZAcqMtx acq(fMutex);

	char* localDest = static_cast<char*>(oDest);

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
		*oCountRead = localDest - static_cast<char*>(oDest);
	}

size_t ZNetEndpoint_RFCOMM_OSX::Imp_CountReadable()
	{
	ZAcqMtx acq(fMutex);
	return fBuffer.size();
	}

bool ZNetEndpoint_RFCOMM_OSX::Imp_WaitReadable(double iTimeout)
	{
	const ZTime deadline = ZTime::sSystem() + iTimeout;
	ZAcqMtx acq(fMutex);
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
	ZAcqMtx acq(fMutex);
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

void ZNetEndpoint_RFCOMM_OSX::pReceived(IOBluetoothRFCOMMChannel*, void* iSource, size_t iLength)
	{
	ZAcqMtx acq(fMutex);
	const uint8* data = static_cast<const uint8*>(iSource);
	fBuffer.insert(fBuffer.end(), data, data + iLength);
	fCondition.Broadcast();
	}

void ZNetEndpoint_RFCOMM_OSX::pClosed(IOBluetoothRFCOMMChannel*)
	{
	ZAcqMtx acq(fMutex);
	fOpen = false;
	fCondition.Broadcast();
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_RFCOMM_OSX)
