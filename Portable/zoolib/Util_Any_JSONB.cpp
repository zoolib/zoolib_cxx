/* -------------------------------------------------------------------------------------------------
Copyright (c) 2015 Andrew Green
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

#include "zoolib/Util_Any_JSONB.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Log.h"
#include "zoolib/PullPush_Any.h"
#include "zoolib/PullPush_JSONB.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Util_Any_JSONB {

ZQ<Any> sQRead(const ChanR_Bin& iChanR)
	{
	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	ZRef<Delivery<Any>> theDelivery = sStartAsyncAs_Any(sGetClear(thePair.second));
	sPull_JSONB_Push_PPT(iChanR, null, *thePair.first);
	sDisconnectWrite(*thePair.first);

	return theDelivery->QGet();
	}

// -----

static void spFrom_Any_Push_PPT(const Any& iAny, const ZRef<ChannerWCon_PPT>& iChannerWCon)
	{
	sFrom_Any_Push_PPT(iAny, *iChannerWCon);
	sDisconnectWrite(*iChannerWCon);
	}

void sWrite(const Any& iVal, const ChanW_Bin& iChanW)
	{
	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	sStartOnNewThread(sBindR(sCallable(spFrom_Any_Push_PPT), iVal, sGetClear(thePair.first)));
	sPull_PPT_Push_JSONB(*thePair.second, null, iChanW);
	}

} // namespace Util_Any_JSONB
} // namespace ZooLib

