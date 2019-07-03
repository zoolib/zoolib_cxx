/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/PullPush_Any.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/NameUniquifier.h"
#include "zoolib/ParseException.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/Val_Any.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark - 

void sFromAny_Push_PPT(const Any& iAny,
	const ZP<Callable_Any_WriteFilter>& iWriteFilter,
	const ChanW_PPT& iChanW)
	{
	if (const Seq_Any* theSeq = sPGet<Seq_Any>(iAny))
		{
		sPush_Start_Seq(iChanW);
		for (size_t xx = 0; xx < theSeq->Count(); ++xx)
			sFromAny_Push_PPT(theSeq->Get(xx), iWriteFilter, iChanW);
		sPush(PullPush::kEnd, iChanW);
		}

	else if (const Map_Any* theMap = sPGet<Map_Any>(iAny))
		{
		sPush_Start_Map(iChanW);
		for (Map_Any::Index_t iter = theMap->Begin(), end = theMap->End();
			iter != end; ++iter)
			{
			sPush(sName(iter->first), iChanW);
			sFromAny_Push_PPT(iter->second, iWriteFilter, iChanW);
			}
		sPush(PullPush::kEnd, iChanW);
		}

	else if (const string* theString = sPGet<string>(iAny))
		{
		sPush(*theString, iChanW);
//		sPull_UTF_Push(ChanRU_UTF_string8(*theString), iChanW);
		}

	else if (const Data_Any* theData = sPGet<Data_Any>(iAny))
		{
		sPull_Bin_Push_PPT(ChanRPos_Bin_Data<Data_Any>(*theData), iChanW);
		}

	else if (not iWriteFilter || not sCall(iWriteFilter, iAny, iChanW))
		{
		sPush(iAny.As<PPT>(), iChanW);
		}
	}

void sFromAny_Push_PPT(const Any& iAny,
	const ChanW_PPT& iChanW)
	{ sFromAny_Push_PPT(iAny, null, iChanW); }

// =================================================================================================
#pragma mark - 

void sPull_PPT_AsAny(const PPT& iPPT,
	const ChanR_PPT& iChanR,
	const ZP<Callable_Any_ReadFilter>& iReadFilter,
	Any& oAny)
	{
	// Handle the filter *first*, in case we may have Start derivatives in the chan.
	if (iReadFilter)
		{
		if (ZQ<bool> theQ = iReadFilter->QCall(iPPT, iChanR, oAny))
			{
			if (*theQ)
				return;
			}
		}

	// Handle standard stuff.

	if (const string* theString = sPGet<string>(iPPT))
		{
		oAny = *theString;
		return;
		}

	if (ZP<ChannerR_UTF> theChanner = sGet<ZP<ChannerR_UTF>>(iPPT))
		{
		oAny = sReadAllUTF8(*theChanner);
		return;
		}

	if (const Data_Any* theData = sPGet<Data_Any>(iPPT))
		{
		oAny = *theData;
		return;
		}

	if (ZP<ChannerR_Bin> theChanner = sGet<ZP<ChannerR_Bin>>(iPPT))
		{
		oAny = sReadAll_T<Data_Any>(*theChanner);
		return;
		}

	if (sIsStartMap(iPPT))
		{
		Map_Any theMap;
		for (;;)
			{
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				break;
				}
			else if (NotQ<PPT> thePPTQ = sQRead(iChanR))
				{
				sThrow_ParseException("Require value after Name from ChanR_PPT");
				}
			else
				{
				Any theAny;
				sPull_PPT_AsAny(*thePPTQ, iChanR, iReadFilter, theAny);
				theMap.Set(*theNameQ, theAny);
				}
			}
		oAny = theMap;
		return;
		}

	// This could be just Start, to generically handle Start derivatives
	if (sIsStartSeq(iPPT))
		{
		Seq_Any theSeq;
		for (;;)
			{
			if (NotQ<PPT> thePPTQ = sQEReadPPTOrEnd(iChanR))
				{
				break;
				}
			else
				{
				Any theAny;
				sPull_PPT_AsAny(*thePPTQ, iChanR, iReadFilter, theAny);
				theSeq.Append(theAny);
				}
			}
		oAny = theSeq;
		return;
		}

	oAny = iPPT.As<Any>();
	}

bool sPull_PPT_AsAny(const ChanR_PPT& iChanR,
	const ZP<Callable_Any_ReadFilter>& iReadFilter,
	Any& oAny)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		sPull_PPT_AsAny(*theQ, iChanR, iReadFilter, oAny);
		return true;
		}
	return false;
	}

bool sPull_PPT_AsAny(const ChanR_PPT& iChanR, Any& oAny)
	{ return sPull_PPT_AsAny(iChanR, null, oAny); }

Any sAsAny(const PPT& iPPT, const ChanR_PPT& iChanR)
	{
	Any theAny;
	sPull_PPT_AsAny(iPPT, iChanR, null, theAny);
	return theAny;
	}

ZQ<Any> sQAsAny(const ChanR_PPT& iChanR)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		return sAsAny(*theQ, iChanR);
	return null;
	}

Any sAsAny(const ChanR_PPT& iChanR)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		Any theAny;
		sPull_PPT_AsAny(*theQ, iChanR, null, theAny);
		return theAny;
		}
	return Any();
	}

static void spAsync_AsAny(const ZP<ChannerR_PPT>& iChannerR,
	const ZP<Callable_Any_ReadFilter>& iReadFilter,
	const ZP<Promise<Any>>& iPromise)
	{
	ZThread::sSetName("spAsync_AsAny");
	Any result;
	if (sPull_PPT_AsAny(*iChannerR, iReadFilter, result))
		iPromise->Deliver(result);
	}

ZP<Delivery<Any>> sStartAsync_AsAny(const ZP<ChannerR_PPT>& iChannerR)
	{ return sStartAsync_AsAny(iChannerR, null); }

ZP<Delivery<Any>> sStartAsync_AsAny(const ZP<ChannerR_PPT>& iChannerR,
	const ZP<Callable_Any_ReadFilter>& iReadFilter)
	{
	ZP<Promise<Any>> thePromise = sPromise<Any>();
	sStartOnNewThread(sBindR(sCallable(spAsync_AsAny), iChannerR, iReadFilter, thePromise));
	return thePromise->GetDelivery();
	}

} // namespace ZooLib
