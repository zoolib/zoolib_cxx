/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/Util_Any_JSON.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/ChanRU_XX_Unreader.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Promise.h"
#include "zoolib/PullPush_Any.h"
#include "zoolib/PullPush_JSON.h"
#include "zoolib/StartOnNewThread.h"

#include "zoolib/pdesc.h"
#if defined(ZMACRO_pdesc)
	#include "zoolib/StdIO.h"
	#include "zoolib/Util_Chan_UTF_Operators.h"
#endif

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Util_Any_JSON {

static void spAsyncPullAny(const ZRef<ChannerR_Any>& iChannerR, const ZRef<Promise<Any>>& iPromise)
	{
	if (ZQ<Any> theAny = sQPull_Any(*iChannerR))
		iPromise->Deliver(*theAny);
	}

static ZRef<Delivery<Any>> spStartAsyncPullAny(const ZRef<ChannerR_Any>& iChannerR)
	{
	ZRef<Promise<Any>> thePromise = sPromise<Any>();
	sStartOnNewThread(sBindR(sCallable(spAsyncPullAny), iChannerR, thePromise));
	return thePromise->GetDelivery();
	}

ZQ<Any> sQRead(const ChanRU_UTF& iChanRU,
	const PullPush_JSON::ReadOptions& iRO)
	{
	PullPushPair<Any> thePair = sMakePullPushPair<Any>();
	ZRef<Delivery<Any>> theDelivery = spStartAsyncPullAny(sGetClear(thePair.second));
	sPull_JSON_Push(iChanRU, iRO, *thePair.first);
	sDisconnectWrite(*thePair.first);

	return theDelivery->QGet();
	}

ZQ<Any> sQRead(const ChanRU_UTF& iChanRU)
	{ return sQRead(iChanRU, Util_Chan_JSON::sReadOptions_Extended()); }

// -----

void sWrite(const Any& iVal, const ChanW_UTF& iChanW)
	{ sWrite(false, iVal, iChanW); }

static void spPull_Any_Push(const Any& iAny, const ZRef<ChannerWCon_Any>& iChannerWCon)
	{
	sPull_Any_Push(iAny, *iChannerWCon);
	sDisconnectWrite(*iChannerWCon);
	}

void sWrite(bool iPrettyPrint, const Any& iVal, const ChanW_UTF& iChanW)
	{
	PullPushPair<Any> thePair = sMakePullPushPair<Any>();
	sStartOnNewThread(sBindR(sCallable(spPull_Any_Push), iVal, sGetClear(thePair.first)));
	sPull_Push_JSON(*thePair.second, 0, YadOptions(iPrettyPrint), iChanW);
	}

string8 sAsJSON(const Any& iVal)
	{
	string8 result;
	sWrite(iVal, ChanW_UTF_string8(&result));
	return result;
	}

const Any sFromJSON(const string8& iString)
	{ return sQRead(ChanRU_UTF_string8(iString)).Get(); }

} // namespace Util_Any_JSON

// =================================================================================================
#pragma mark -

namespace Operators_Any_JSON {

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Val_Any& iVal)
	{
	Util_Any_JSON::sWrite(iVal, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Map_Any& iMap)
	{
	Util_Any_JSON::sWrite(Any(iMap), iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Seq_Any& iSeq)
	{
	Util_Any_JSON::sWrite(Any(iSeq), iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Data_Any& iData)
	{
	Util_Any_JSON::sWrite(Any(iData), iChanW);
	return iChanW;
	}

} // namespace Operators_Any_JSON

} // namespace ZooLib

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const Val_Any& iVal)
	{
	Util_Any_JSON::sWrite(iVal, StdIO::sChan_UTF_Err);
	StdIO::sChan_UTF_Err << "\n";
	}

ZMACRO_pdesc(const Map_Any& iMap)
	{
	Util_Any_JSON::sWrite(Any(iMap), StdIO::sChan_UTF_Err);
	StdIO::sChan_UTF_Err << "\n";
	}

ZMACRO_pdesc(const Seq_Any& iSeq)
	{
	Util_Any_JSON::sWrite(Any(iSeq), StdIO::sChan_UTF_Err);
	StdIO::sChan_UTF_Err << "\n";
	}

ZMACRO_pdesc(const Data_Any& iData)
	{
	Util_Any_JSON::sWrite(Any(iData), StdIO::sChan_UTF_Err);
	StdIO::sChan_UTF_Err << "\n";
	}

#endif // defined(ZMACRO_pdesc)
