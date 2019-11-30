// Copyright (c) 2018-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/PullPush_ZZ.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/NameUniquifier.h"
#include "zoolib/ParseException.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/Val_ZZ.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark - 

void sFromZZ_Push_PPT(const Val_ZZ& iVal,
	const ZP<Callable_ZZ_WriteFilter>& iWriteFilter,
	const ChanW_PPT& iChanW)
	{
	if (const Seq_ZZ* theSeq = sPGet<Seq_ZZ>(iVal))
		{
		sPush_Start_Seq(iChanW);
		for (size_t xx = 0; xx < theSeq->Count(); ++xx)
			sFromZZ_Push_PPT(theSeq->Get(xx).As<Val_ZZ>(), iWriteFilter, iChanW);
		sPush_End(iChanW);
		}

	else if (const Map_ZZ* theMap = sPGet<Map_ZZ>(iVal))
		{
		sPush_Start_Map(iChanW);
		for (Map_ZZ::Index_t iter = theMap->Begin(), end = theMap->End();
			iter != end; ++iter)
			{
			sPush(sName(iter->first), iChanW);
			sFromZZ_Push_PPT(iter->second.As<Val_ZZ>(), iWriteFilter, iChanW);
			}
		sPush_End(iChanW);
		}


	// Could add in support for other map/seq types.

	else if (const string* theString = sPGet<string>(iVal))
		{
		sPush(*theString, iChanW);
//		sPull_UTF_Push(ChanRU_UTF_string8(*theString), iChanW);
		}

	else if (const Data_ZZ* theData = sPGet<Data_ZZ>(iVal))
		{
		sPull_Bin_Push_PPT(ChanRPos_Bin_Data<Data_ZZ>(*theData), iChanW);
		}

	else if (not iWriteFilter || not sCall(iWriteFilter, iVal, iChanW))
		{
		sPush(iVal.As<PPT>(), iChanW);
		}
	}

void sFromZZ_Push_PPT(const Val_ZZ& iVal,
	const ChanW_PPT& iChanW)
	{ sFromZZ_Push_PPT(iVal, null, iChanW); }

// =================================================================================================
#pragma mark - 

void sPull_PPT_AsZZ(const PPT& iPPT,
	const ChanR_PPT& iChanR,
	const ZP<Callable_ZZ_ReadFilter>& iReadFilter,
	Val_ZZ& oVal)
	{
	// Handle the filter *first*, in case we may have Start derivatives in the chan.
	if (iReadFilter)
		{
		if (ZQ<bool> theQ = iReadFilter->QCall(iPPT, iChanR, oVal))
			{
			if (*theQ)
				return;
			}
		}

	// Handle standard stuff.

	if (const string* theString = sPGet<string>(iPPT))
		{
		oVal = *theString;
		return;
		}

	if (ZP<ChannerR_UTF> theChanner = sGet<ZP<ChannerR_UTF>>(iPPT))
		{
		oVal = sReadAllUTF8(*theChanner);
		return;
		}

	if (const Data_ZZ* theData = sPGet<Data_ZZ>(iPPT))
		{
		oVal = *theData;
		return;
		}

	if (ZP<ChannerR_Bin> theChanner = sGet<ZP<ChannerR_Bin>>(iPPT))
		{
		oVal = sReadAll_T<Data_ZZ>(*theChanner);
		return;
		}

	if (sIsStart_Map(iPPT))
		{
		Map_ZZ theMap;
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
				Val_ZZ theVal;
				sPull_PPT_AsZZ(*thePPTQ, iChanR, iReadFilter, theVal);
				theMap.Set(*theNameQ, theVal.As<Val_ZZ>());
				}
			}
		oVal = theMap;
		return;
		}

	// This could be just Start, to generically handle Start derivatives
	if (sIsStart_Seq(iPPT))
		{
		Seq_ZZ theSeq;
		for (;;)
			{
			if (NotQ<PPT> thePPTQ = sQEReadPPTOrEnd(iChanR))
				{
				break;
				}
			else
				{
				Val_ZZ theVal;
				sPull_PPT_AsZZ(*thePPTQ, iChanR, iReadFilter, theVal);
				theSeq.Append(theVal.As<Val_ZZ>());
				}
			}
		oVal = theSeq;
		return;
		}

	oVal = iPPT.As<Val_ZZ>();
	}

bool sPull_PPT_AsZZ(const ChanR_PPT& iChanR,
	const ZP<Callable_ZZ_ReadFilter>& iReadFilter,
	Val_ZZ& oVal)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		sPull_PPT_AsZZ(*theQ, iChanR, iReadFilter, oVal);
		return true;
		}
	return false;
	}

bool sPull_PPT_AsZZ(const ChanR_PPT& iChanR, Val_ZZ& oVal)
	{ return sPull_PPT_AsZZ(iChanR, null, oVal); }

Val_ZZ sAsZZ(const PPT& iPPT, const ChanR_PPT& iChanR)
	{
	Val_ZZ theVal;
	sPull_PPT_AsZZ(iPPT, iChanR, null, theVal);
	return theVal;
	}

ZQ<Val_ZZ> sQAsZZ(const ChanR_PPT& iChanR)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		return sAsZZ(*theQ, iChanR);
	return null;
	}

Val_ZZ sAsZZ(const ChanR_PPT& iChanR)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		Val_ZZ theVal;
		sPull_PPT_AsZZ(*theQ, iChanR, null, theVal);
		return theVal;
		}
	return Val_ZZ();
	}

static void spAsync_AsZZ(const ZP<ChannerR_PPT>& iChannerR,
	const ZP<Callable_ZZ_ReadFilter>& iReadFilter,
	const ZP<Promise<Val_ZZ>>& iPromise)
	{
	ZThread::sSetName("spAsync_AsZZ");
	Val_ZZ result;
	if (sPull_PPT_AsZZ(*iChannerR, iReadFilter, result))
		iPromise->Deliver(result);
	}

ZP<Delivery<Val_ZZ>> sStartAsync_AsZZ(const ZP<ChannerR_PPT>& iChannerR)
	{ return sStartAsync_AsZZ(iChannerR, null); }

ZP<Delivery<Val_ZZ>> sStartAsync_AsZZ(const ZP<ChannerR_PPT>& iChannerR,
	const ZP<Callable_ZZ_ReadFilter>& iReadFilter)
	{
	ZP<Promise<Val_ZZ>> thePromise = sPromise<Val_ZZ>();
	sStartOnNewThread(sBindR(sCallable(spAsync_AsZZ), iChannerR, iReadFilter, thePromise));
	return thePromise->GetDelivery();
	}

} // namespace ZooLib
