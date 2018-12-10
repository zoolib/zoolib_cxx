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
#include "zoolib/Val_Any.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark - 

void sPull_Any_Push(const Any& iAny,
	const ZRef<Callable_Any_WriteFilter>& iWriteFilter,
	const ChanW_Any& iChanW)
	{
	if (const Seq_Any* theSeq = sPGet<Seq_Any>(iAny))
		{
		sPush(PullPush::kStartSeq, iChanW);
		for (size_t xx = 0; xx < theSeq->Count(); ++xx)
			sPull_Any_Push(theSeq->Get(xx), iChanW);
		sPush(PullPush::kEnd, iChanW);
		}

	else if (const Map_Any* theMap = sPGet<Map_Any>(iAny))
		{
		sPush(PullPush::kStartMap, iChanW);
		for (Map_Any::Index_t iter = theMap->Begin(), end = theMap->End();
			iter != end; ++iter)
			{
			sPush(sName(iter->first), iChanW);
			sPull_Any_Push(iter->second, iChanW);
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
		sPull_Bin_Push(ChanRPos_Bin_Data<Data_Any>(*theData), iChanW);
		}

	else if (not iWriteFilter || not sCall(iWriteFilter, iAny, iChanW))
		{
		sPush(iAny, iChanW);
		}
	}

void sPull_Any_Push(const Any& iAny,
	const ChanW_Any& iChanW)
	{ sPull_Any_Push(iAny, null, iChanW); }

// =================================================================================================
#pragma mark - 

void sPull_Push_Any(const Any& iAny,
	const ZRef<Callable_Any_ReadFilter>& iReadFilter,
	const ChanR_Any& iChanR, Any& oAny)
	{
	if (false)
		{}

	else if (const string* theString = sPGet<string>(iAny))
		{
		oAny = *theString;
		}

	else if (ZRef<ChannerR_UTF> theChanner = sGet<ZRef<ChannerR_UTF>>(iAny))
		{
		oAny = sReadAllUTF8(*theChanner);
		}

	else if (const Data_Any* theData = sPGet<Data_Any>(iAny))
		{
		oAny = *theData;
		}

	else if (ZRef<ChannerR_Bin> theChanner = sGet<ZRef<ChannerR_Bin>>(iAny))
		{
		oAny = sReadAll_T<Data_Any>(*theChanner);
		}

	else if (sPGet<PullPush::StartMap>(iAny))
		{
		Map_Any theMap;
		for (;;)
			{
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				break;
				}
			else if (NotQ<Any> theNotQ = sQRead(iChanR))
				{
				sThrow_ParseException("Require value after Name from ChanR_Any");
				}
			else
				{
				Any theAny;
				sPull_Push_Any(*theNotQ, iReadFilter, iChanR, theAny);
				theMap.Set(*theNameQ, theAny);
				}
			}
		oAny = theMap;
		}

	else if (sPGet<PullPush::StartSeq>(iAny))
		{
		Seq_Any theSeq;
		for (;;)
			{
			if (NotQ<Any> theQ = sQEReadAnyOrEnd(iChanR))
				{
				break;
				}
			else
				{
				Any theAny;
				sPull_Push_Any(*theQ, iReadFilter, iChanR, theAny);
				theSeq.Append(theAny);
				}
			}
		oAny = theSeq;
		}

	else
		{
		if (iReadFilter)
			{
			if (ZQ<bool> theQ = iReadFilter->QCall(iAny, iChanR, oAny))
				{
				if (*theQ)
					return;
				}
			}
		oAny = iAny;
		}
	}

bool sPull_Push_Any(const ChanR_Any& iChanR,
	const ZRef<Callable_Any_ReadFilter>& iReadFilter,
	Any& oAny)
	{
	if (ZQ<Any> theQ = sQRead(iChanR))
		{
		sPull_Push_Any(*theQ, iReadFilter, iChanR, oAny);
		return true;
		}
	return false;
	}

bool sPull_Push_Any(const ChanR_Any& iChanR, Any& oAny)
	{ return sPull_Push_Any(iChanR, null, oAny); }

ZQ<Any> sQPull_Any(const ChanR_Any& iChanR)
	{
	if (ZQ<Any> theQ = sQRead(iChanR))
		{
		Any theAny;
		sPull_Push_Any(*theQ, null, iChanR, theAny);
		return theAny;
		}
	return null;
	}

Any sPull_Any(const ChanR_Any& iChanR)
	{
	if (ZQ<Any> theQ = sQRead(iChanR))
		{
		Any theAny;
		sPull_Push_Any(*theQ, null, iChanR, theAny);
		return theAny;
		}
	return Any();
	}

} // namespace ZooLib
