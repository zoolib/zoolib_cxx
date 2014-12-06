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

#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZNatter.h"
#include "zoolib/ZStream_Data_T.h"

namespace ZooLib {

using namespace Util_STL;

using std::pair;

// =================================================================================================
// MARK: - ZNatter

ZNatter::ZNatter(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fReadBusy(false)
,	fNextLocalID(2)
,	fError(false)
	{}

ZNatter::~ZNatter()
	{}

ZQ<pair<ZData_Any,ZRef<ZNatter::Exchange> > > ZNatter::QReceive()
	{
	ZRef<Exchange> theExchange = this->MakeExchange();

	ZGuardMtxR guard(fMtxR_Structure);
	const int64 theID = fNextLocalID;
	theExchange->fID = theID;
	fNextLocalID += 2;

	sInsertMust(fPending, theExchange->fID, theExchange);
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

	if (ZQ<ZData_Any> theQ = this->pReadFor(guard, theExchange))
		return pair<ZData_Any,ZRef<Exchange> >(*theQ, theExchange);

	return null;
	}

ZRef<ZNatter::Exchange> ZNatter::MakeExchange()
	{
	ZGuardMtxR guard(fMtxR_Structure);
	ZRef<Exchange> theExchange = new Exchange;

	theExchange->fNatter = this;
	theExchange->fID = 0;
	theExchange->fWaiting = false;

	return theExchange;
	}

void ZNatter::pRemove(Exchange* iExchange)
	{
	ZGuardMtxR guard(fMtxR_Structure);

	if (not iExchange->fID)
		return;

	sInsertMust(fRetired, iExchange->fID);

	guard.Release();

	if (ZLOGPF(s, eDebug))
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

ZQ<ZData_Any> ZNatter::pQSendReceive(ZRef<Exchange> iExchange, ZData_Any iData)
	{
	ZGuardMtxR guard(fMtxR_Structure);

	ZAssert(not iExchange->fDataQ);

	while (not iExchange->fID)
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

	sInsertMust(fPending, iExchange->fID, iExchange);
	iExchange->fWaiting = true;

	guard.Release();

	if (ZLOGPF(s, eDebug))
		s << this << "id: " << iExchange->fID;

	try
		{
		ZAcqMtx acq_Write(fMtx_Write);
		const ZStreamW& w = fStreamerW->GetStreamW();
		w.WriteUInt8(2);
		w.WriteInt64(iExchange->fID ^ 1);
		w.WriteCount(iData.GetSize());
		w.Write(iData.GetPtr(), iData.GetSize());
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

ZQ<ZData_Any> ZNatter::pReadFor(ZGuardMtxR& iGuard, ZRef<Exchange> iExchange)
	{
	while (not fError && iExchange->fWaiting)
		this->pRead(iGuard);

	if (fError)
		return null;

	return sGetSet(iExchange->fDataQ, null);
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

void ZNatter::pRead(ZGuardMtxR& iGuard)
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

		if (not result)
			{
			fError = true;
			}
		else
			{
			if (ZLOGPF(s, eDebug))
				s << this << ", id: " << theID << ", type: " << theType;

			if (theType == 0)
				{
				// Advertisment of an available receive.
				fRemoteIDs.push_back(theID);
				}
			else if (theType == 1)
				{
				if (not sQErase(fRetired, theID))
					{
					ZRef<Exchange> theExchange = sGetEraseMust(fPending, theID);
					ZAssert(theExchange->fWaiting);
					theExchange->fWaiting = false;
					}
				}
			else if (theType == 2)
				{
				ZRef<Exchange> theExchange = sGetEraseMust(fPending, theID);
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

ZRef<ZNatter> sNatter(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
	{
	if (iStreamerR && iStreamerW)
		return new ZNatter(iStreamerR, iStreamerW);
	return null;
	}

ZRef<ZNatter> sNatter(ZRef<ZStreamerRW> iStreamerRW)
	{
	if (iStreamerRW)
		return new ZNatter(iStreamerRW, iStreamerRW);
	return null;
	}

// =================================================================================================
// MARK: - ZNatter::Exchange

ZNatter::Exchange::Exchange()
	{}

ZNatter::Exchange::~Exchange()
	{ fNatter->pRemove(this); }

ZQ<ZData_Any> ZNatter::Exchange::QSendReceive(const ZData_Any& iData)
	{ return fNatter->pQSendReceive(this, iData); }

ZRef<ZNatter::Exchange> sNatterExhange(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
	{
	if (iStreamerR && iStreamerW)
		return (new ZNatter(iStreamerR, iStreamerW))->MakeExchange();
	return null;
	}

ZRef<ZNatter::Exchange> sNatterExhange(ZRef<ZStreamerRW> iStreamerRW)
	{
	if (iStreamerRW)
		return (new ZNatter(iStreamerRW, iStreamerRW))->MakeExchange();
	return null;
	}

} // namespace ZooLib
