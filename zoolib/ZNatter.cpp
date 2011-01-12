/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZLog.h"
#include "zoolib/ZNatter.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"

namespace ZooLib {
namespace ZNatter {

// =================================================================================================
#pragma mark -
#pragma mark * Channel

Channel::Channel(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fReadBusy(false)
,	fNextLocalID(2)
,	fError(false)
	{}

Channel::~Channel()
	{}

ZQ<ZData_Any> Channel::Receive(ZRef<Exchange>* oExchange)
	{
	ZRef<Exchange> theExchange = new Exchange(this);

	ZGuardRMtxR guard(fMtxR_Structure);
	const int64 theID = fNextLocalID;
	theExchange->fID = theID;
	fNextLocalID += 2;

	ZUtil_STL::sInsertMustNotContain(1, fPending, theExchange->fID, theExchange);
	theExchange->fWaiting = true;

	guard.Release();

	// Send a notification that we've got an unsolicited receive pending
	try
		{
		const ZStreamW& w = fStreamerW->GetStreamW();
		ZAcqMtx acq_Write(fMtx_Write);
		w.WriteUInt8(0);
		w.WriteInt64(theID ^ 1);
		w.Flush();
		}
	catch (...)
		{
		guard.Acquire();
		theExchange->fWaiting = false;
		fError = true;
		fCnd.Broadcast();
		return null;
		}

	guard.Acquire();

	if (oExchange)
		*oExchange = theExchange;

	return this->pReadFor(guard, theExchange);
	}

void Channel::Send(ZData_Any iData)
	{ (new Exchange(this))->SendReceive(iData); }

void Channel::pAdd(Exchange* iExchange)
	{
	// Be careful in here -- do *NOT* pulse iExchange's refcount.

	ZGuardRMtxR guard(fMtxR_Structure);

	iExchange->fChannel = this;
	iExchange->fID = 0;
	iExchange->fWaiting = false;
	}

void Channel::pRemove(Exchange* iExchange)
	{
	ZGuardRMtxR guard(fMtxR_Structure);

	if (!iExchange->fID)
		return;

	ZUtil_STL::sInsertMustNotContain(1, fRetired, iExchange->fID);

	guard.Release();

	if (ZLOGF(s, eDebug))
		s << this << ", id: " << iExchange->fID;

	try
		{
		ZAcqMtx acq_Write(fMtx_Write);
		const ZStreamW& w = fStreamerW->GetStreamW();
		w.WriteUInt8(1);
		w.WriteInt64(iExchange->fID ^ 1);
		w.Flush();
		}
	catch (...)
		{
		guard.Acquire();
		fError = true;
		fCnd.Broadcast();
		}
	}

ZQ<ZData_Any> Channel::pSendReceive(ZRef<Exchange> iExchange, ZData_Any iData)
	{
	ZGuardRMtxR guard(fMtxR_Structure);

	ZAssert(!iExchange->fDataQ);

	while (!iExchange->fID)
		{
		if (fError)
			return null;

		if (fRemoteIDs.empty())
			{
			this->pRead(guard);
			}
		else
			{
			iExchange->fID = fRemoteIDs.front();
			fRemoteIDs.pop_front();
			ZAssert(iExchange->fID & 1);
			}
		}

	ZUtil_STL::sInsertMustNotContain(1, fPending, iExchange->fID, iExchange);
	iExchange->fWaiting = true;

	guard.Release();

	if (ZLOGF(s, eDebug))
		s << this << "id: " << iExchange->fID;

	try
		{
		ZAcqMtx acq_Write(fMtx_Write);
		const ZStreamW& w = fStreamerW->GetStreamW();
		w.WriteUInt8(2);
		w.WriteInt64(iExchange->fID ^ 1);
		w.WriteCount(iData.GetSize());
		w.Write(iData.GetData(), iData.GetSize());
		w.Flush();
		}
	catch (...)
		{
		guard.Acquire();
		iExchange->fWaiting = false;
		fError = true;
		fCnd.Broadcast();
		return null;
		}

	guard.Acquire();

	return this->pReadFor(guard, iExchange);
	}

ZQ<ZData_Any> Channel::pReadFor(ZGuardRMtxR& iGuard, ZRef<Exchange> iExchange)
	{
	while (!fError && iExchange->fWaiting)
		this->pRead(iGuard);

	if (fError)
		return null;

	ZQ<ZData_Any> result;
	result.swap(iExchange->fDataQ);
	return result;
	}

static bool spReadPacket(uint8& oType, int64& oID, ZData_Any& oData, const ZStreamR& r)
	{
	try
		{
		oType = r.ReadUInt8();
		oID = r.ReadInt64();
		if (oType == 2)
			oData = sRead_T<ZData_Any>(r, r.ReadCount());
		return true;
		}
	catch (...)
		{}
	return false;
	}

void Channel::pRead(ZGuardRMtxR& iGuard)
	{
	if (fReadBusy)
		{
		fCnd.Wait(fMtxR_Structure);
		}
	else
		{
		fReadBusy = true;
		iGuard.Release();

		uint8 theType;
		int64 theID;
		ZData_Any theData;
		const bool result = spReadPacket(theType, theID, theData, fStreamerR->GetStreamR());

		iGuard.Acquire();
		fReadBusy = false;

		if (!result)
			{
			fError = true;
			}
		else
			{
			if (ZLOGF(s, eDebug))
				s << this << ", id: " << theID << ", type: " << theType;

			if (theType == 0)
				{
				// Advertisment of an available receive.
				fRemoteIDs.push_back(theID);
				}
			else if (theType == 1)
				{
				if (!ZUtil_STL::sEraseIfContains(fRetired, theID))
					{
					ZRef<Exchange> theExchange = ZUtil_STL::sEraseAndReturn(1, fPending, theID);
					ZAssert(theExchange->fWaiting);
					theExchange->fWaiting = false;
					}
				}
			else if (theType == 2)
				{
				ZRef<Exchange> theExchange = ZUtil_STL::sEraseAndReturn(1, fPending, theID);
				ZAssert(theExchange->fWaiting);
				theExchange->fWaiting = false;
				theExchange->fDataQ = theData;
				}
			else
				{
				ZUnimplemented();
				}
			}
		fCnd.Broadcast();
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Channel

Exchange::Exchange(ZRef<Channel> iChannel)
	{ iChannel->pAdd(this); }

Exchange::~Exchange()
	{ fChannel->pRemove(this); }

ZQ<ZData_Any> Exchange::SendReceive(const ZData_Any& iData)
	{ return fChannel->pSendReceive(this, iData); }

} // namespace ZNatter
} // namespace ZooLib
