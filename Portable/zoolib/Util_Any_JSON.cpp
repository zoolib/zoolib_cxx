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

#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/ChanRU_XX_Unreader.h"
#include "zoolib/PullPush_Any.h"
#include "zoolib/PullPush_JSON.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_JSON.h"

#include "zoolib/Callable_Function.h"
#include "zoolib/Callable_Bind.h"
#include "zoolib/Promise.h"
#include "zoolib/StartOnNewThread.h"

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

ZQ<Any> sQRead(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, const PullPush_JSON::ReadOptions& iRO)
	{
	PullPushPair<Any> thePair = sMakePullPushPair<Any>();
	ZRef<Delivery<Any>> theDelivery = spStartAsyncPullAny(sGetClear(thePair.second));
	sPull_JSON_Push(iChanR, iChanU, iRO, *thePair.first);
	sDisconnectWrite(*thePair.first);

	return theDelivery->QGet();
	}

ZQ<Any> sQRead(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{ return sQRead(iChanR, iChanU, Util_Chan_JSON::sReadOptions_Extended()); }

ZQ<Any> sQRead(const ChanRU_UTF& iChanRU, const PullPush_JSON::ReadOptions& iRO)
	{ return sQRead(iChanRU, iChanRU, iRO); }

ZQ<Any> sQRead(const ChanRU_UTF& iChanRU)
	{ return sQRead(iChanRU, Util_Chan_JSON::sReadOptions_Extended()); }

//ZQ<Val_Any> sQRead(const ZRef<ChannerR_UTF>& iChannerR, const ZRef<ChannerU_UTF>& iChannerU)
//	{
//	if (iChannerR and iChannerU)
//		{
//		Yad_JSON::ReadOptions theRO = Yad_JSON::sReadOptions_Extended();
//		if (ZRef<YadR> theYad = Yad_JSON::sYadR(theRO, iChannerR, iChannerU))
//			{
//			if (ZQ<Val_Any> resultQ = Yad_Any::sQFromYadR(theYad))
//				return resultQ;
//			}
//		}
//	return null;
//	}
//
//ZQ<Val_Any> sQRead(const ZRef<ChannerR_UTF>& iChannerR)
//	{
//	ZRef<ChannerRU<UTF32> > theChanner = sChanner_Channer_T<ChanRU_XX_Unreader<UTF32>>(iChannerR);
//	return sQRead(theChanner, theChanner);
//	}
//
//ZQ<Val_Any> sQRead(const ZRef<ChannerR_Bin>& iChannerR)
//	{
//	ZRef<ChannerR_UTF> theChannerR = sChanner_Channer_T<ChanR_UTF_Chan_Bin_UTF8>(iChannerR);
//	return sQRead(theChannerR);
//	}

void sWrite(const Val_Any& iVal, const ChanW_UTF& iChanW)
	{ Yad_JSON::sToChan(sYadR(iVal), iChanW); }

void sWrite(bool iPrettyPrint, const Val_Any& iVal, const ChanW_UTF& iChanW)
	{
	if (iPrettyPrint)
		Yad_JSON::sToChan(0, YadOptions(true), sYadR(iVal), iChanW);
	else
		Yad_JSON::sToChan(sYadR(iVal), iChanW);
	}

string8 sAsJSON(const Val_Any& iVal)
	{
	string8 result;
	sWrite(iVal, ChanW_UTF_string8(&result));
	return result;
	}

// =================================================================================================
#pragma mark -

const Val_Any sFromJSON(const string8& iString)
	{
//	ZRef<ChannerRU_UTF> theChannerRU = sChanner_T<ChanRU_UTF_string8>(iString);
	return sQRead(ChanRU_UTF_string8(iString)).Get();
	}

} // namespace Util_Any_JSON

namespace Operators_Any_JSON {

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Val_Any& iVal)
	{
	Util_Any_JSON::sWrite(iVal, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Map_Any& iMap)
	{
	Util_Any_JSON::sWrite(iMap, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Seq_Any& iSeq)
	{
	Util_Any_JSON::sWrite(iSeq, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Data_Any& iData)
	{
	Util_Any_JSON::sWrite(iData, iChanW);
	return iChanW;
	}

} // namespace Operators_Any_JSON

} // namespace ZooLib
