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

#ifndef __ZNatter_h__
#define __ZNatter_h__ 1
#include "zconfig.h"

#include "zoolib/ZData_Any.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZStreamer.h"

#include <deque>
#include <map>
#include <set>

namespace ZooLib {

// =================================================================================================
// MARK: - ZNatter

class ZNatter
:	public ZCounted
	{
public:
	class Exchange;
	friend class Exchange;

	ZNatter(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~ZNatter();

	ZQ<std::pair<ZData_Any,ZRef<Exchange> > > QReceive();

	ZRef<Exchange> MakeExchange();

private:
// Called by Exchange instances
	void pRemove(Exchange* iExchange);
	ZQ<ZData_Any> pQSendReceive(ZRef<Exchange> iExchange, ZData_Any iData);

// Internals
	ZQ<ZData_Any> pReadFor(ZGuardMtxR& iGuard, ZRef<Exchange> iExchange);
	void pRead(ZGuardMtxR& iGuard);

	ZRef<ZStreamerR> fStreamerR;

	ZMtx fMtx_Write;
	ZRef<ZStreamerW> fStreamerW;

	ZMtxR fMtxR_Structure;
	ZCnd fCnd;

	bool fReadBusy;
	int64 fNextLocalID;
	bool fError;
	std::deque<int64> fRemoteIDs;
	std::set<int64> fRetired;
	std::map<int64, ZRef<Exchange> > fPending;
	};

ZRef<ZNatter> sNatter(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
ZRef<ZNatter> sNatter(ZRef<ZStreamerRW> iStreamerRW);

// =================================================================================================
// MARK: - ZNatter::Exchange

class ZNatter::Exchange
:	public ZCounted
	{
private:
	Exchange();

public:
	virtual ~Exchange();

// Our protocol
	ZQ<ZData_Any> QSendReceive(const ZData_Any& iData);

private:
	ZRef<ZNatter> fNatter;
	int64 fID;
	bool fWaiting;
	ZQ<ZData_Any> fDataQ;

	friend class ZNatter;
	};

ZRef<ZNatter::Exchange> sNatterExhange(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
ZRef<ZNatter::Exchange> sNatterExhange(ZRef<ZStreamerRW> iStreamerRW);

} // namespace ZooLib

#endif // __ZNatter_h__
