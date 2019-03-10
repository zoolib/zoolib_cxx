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

void sFrom_Any_Push_PPT(const Any& iAny,
	const ZRef<Callable_Any_WriteFilter>& iWriteFilter,
	const ChanW_PPT& iChanW)
	{
	if (const Seq_Any* theSeq = sPGet<Seq_Any>(iAny))
		{
		sPush(PullPush::kStartSeq, iChanW);
		for (size_t xx = 0; xx < theSeq->Count(); ++xx)
			sFrom_Any_Push_PPT(theSeq->Get(xx), iWriteFilter, iChanW);
		sPush(PullPush::kEnd, iChanW);
		}

	else if (const Map_Any* theMap = sPGet<Map_Any>(iAny))
		{
		sPush(PullPush::kStartMap, iChanW);
		for (Map_Any::Index_t iter = theMap->Begin(), end = theMap->End();
			iter != end; ++iter)
			{
			sPush(sName(iter->first), iChanW);
			sFrom_Any_Push_PPT(iter->second, iWriteFilter, iChanW);
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

void sFrom_Any_Push_PPT(const Any& iAny,
	const ChanW_PPT& iChanW)
	{ sFrom_Any_Push_PPT(iAny, null, iChanW); }

// =================================================================================================
#pragma mark - 

void sPull_PPT_As_Any(const PPT& iPPT,
	const ChanR_PPT& iChanR,
	const ZRef<Callable_Any_ReadFilter>& iReadFilter,
	Any& oAny)
	{
	if (false)
		{}

	else if (const string* theString = sPGet<string>(iPPT))
		{
		oAny = *theString;
		}

	else if (ZRef<ChannerR_UTF> theChanner = sGet<ZRef<ChannerR_UTF>>(iPPT))
		{
		oAny = sReadAllUTF8(*theChanner);
		}

	else if (const Data_Any* theData = sPGet<Data_Any>(iPPT))
		{
		oAny = *theData;
		}

	else if (ZRef<ChannerR_Bin> theChanner = sGet<ZRef<ChannerR_Bin>>(iPPT))
		{
		oAny = sReadAll_T<Data_Any>(*theChanner);
		}

	else if (sPGet<PullPush::StartMap>(iPPT))
		{
		Map_Any theMap;
		for (;;)
			{
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				break;
				}
			else if (NotQ<PPT> theNotQ = sQRead(iChanR))
				{
				sThrow_ParseException("Require value after Name from ChanR_PPT");
				}
			else
				{
				Any theAny;
				sPull_PPT_As_Any(*theNotQ, iChanR, iReadFilter, theAny);
				theMap.Set(*theNameQ, theAny);
				}
			}
		oAny = theMap;
		}

	else if (sPGet<PullPush::StartSeq>(iPPT))
		{
		Seq_Any theSeq;
		for (;;)
			{
			if (NotQ<PPT> theQ = sQEReadPPTOrEnd(iChanR))
				{
				break;
				}
			else
				{
				Any theAny;
				sPull_PPT_As_Any(*theQ, iChanR, iReadFilter, theAny);
				theSeq.Append(theAny);
				}
			}
		oAny = theSeq;
		}

	else
		{
		if (iReadFilter)
			{
			if (ZQ<bool> theQ = iReadFilter->QCall(iPPT, iChanR, oAny))
				{
				if (*theQ)
					return;
				}
			}
		oAny = iPPT.As<Any>();
		}
	}

bool sPull_PPT_As_Any(const ChanR_PPT& iChanR,
	const ZRef<Callable_Any_ReadFilter>& iReadFilter,
	Any& oAny)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		sPull_PPT_As_Any(*theQ, iChanR, iReadFilter, oAny);
		return true;
		}
	return false;
	}

bool sPull_PPT_As_Any(const ChanR_PPT& iChanR, Any& oAny)
	{ return sPull_PPT_As_Any(iChanR, null, oAny); }

ZQ<Any> sQAs_Any(const ChanR_PPT& iChanR)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		Any theAny;
		sPull_PPT_As_Any(*theQ, iChanR, null, theAny);
		return theAny;
		}
	return null;
	}

Any sAs_Any(const ChanR_PPT& iChanR)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		Any theAny;
		sPull_PPT_As_Any(*theQ, iChanR, null, theAny);
		return theAny;
		}
	return Any();
	}

static void spAsyncsAs_Any(const ZRef<ChannerR_PPT>& iChannerR, const ZRef<Promise<Any>>& iPromise)
	{
	if (ZQ<Any> theAny = sAs_Any(*iChannerR))
		iPromise->Deliver(*theAny);
	}

ZRef<Delivery<Any>> sStartAsyncAs_Any(const ZRef<ChannerR_PPT>& iChannerR)
	{
	ZRef<Promise<Any>> thePromise = sPromise<Any>();
	sStartOnNewThread(sBindR(sCallable(spAsyncsAs_Any), iChannerR, thePromise));
	return thePromise->GetDelivery();
	}

} // namespace ZooLib
