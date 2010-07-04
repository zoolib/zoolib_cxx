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

#ifndef __ZNatter__
#define __ZNatter__ 1
#include "zconfig.h"

#include "zoolib/ZData_Any.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZStreamerReader.h"

#include <deque>
#include <map>
#include <set>

namespace ZooLib {
namespace ZNatter {

class Exchange;

// =================================================================================================
#pragma mark -
#pragma mark * Channel

class Channel
:	public ZCounted
	{
public:
	Channel(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~Channel();

	ZQ<ZData_Any> Receive(ZRef<Exchange>* oExchange);
	void Send(ZData_Any iData);

private:
	// Called by Exchange instances
	void pAdd(Exchange* iExchange);
	void pRemove(Exchange* iExchange);
	ZQ<ZData_Any> pSendReceive(ZRef<Exchange> iExchange, ZData_Any iData);

	ZQ<ZData_Any> pReadFor(ZGuardRMtxR& iGuard, ZRef<Exchange> iExchange);
	void pRead(ZGuardRMtxR& iGuard);

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

	friend class Exchange;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Exchange

class Exchange
:	public ZCounted
	{
public:
	Exchange(ZRef<Channel> iChannel);
	virtual ~Exchange();

// Our protocol
	ZQ<ZData_Any> SendReceive(const ZData_Any& iData);

private:
	ZRef<Channel> fChannel;
	int64 fID;
	bool fWaiting;
	ZQ<ZData_Any> fDataQ;

	friend class Channel;
	};

} // namespace ZNatter
} // namespace ZooLib

#endif // __ZNatter__
