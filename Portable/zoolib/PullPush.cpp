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

#include "zoolib/PullPush.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/ParseException.h"
#include "zoolib/Util_Chan.h" // For sCopyFully

namespace ZooLib {

// =================================================================================================
#pragma mark -

namespace PullPush {

const PPT kStart_Map = ZP<Start>(new Start_Map);
const PPT kStart_Seq = ZP<Start>(new Start_Seq);

const PPT kEnd = End();

bool sIsStart(const PPT& iPPT)
	{
	if (const ZP<Start> theRef = sGet<ZP<Start>>(iPPT))
		return true;
	return false;
	}

bool sIsStartMap(const PPT& iPPT)
	{
	if (const ZP<Start> theRef = sGet<ZP<Start>>(iPPT))
		{
		if (theRef.DynamicCast<Start_Map>())
			return true;
		}
	return false;
	}

bool sIsStartSeq(const PPT& iPPT)
	{
	if (const ZP<Start> theRef = sGet<ZP<Start>>(iPPT))
		{
		if (theRef.DynamicCast<Start_Seq>())
			return true;
		}
	return false;
	}

bool sIsEnd(const PPT& iPPT)
	{
	if (const End* theP = sPGet<End>(iPPT))
		return true;
	return false;
	}

} // namespace PullPush

// =================================================================================================
#pragma mark -

void sPush_Start_Map(const ChanW_PPT& iChanW)
	{ sPush(PullPush::kStart_Map, iChanW); }

void sPush_Start_Seq(const ChanW_PPT& iChanW)
	{ sPush(PullPush::kStart_Seq, iChanW); }

void sPush_End(const ChanW_PPT& iChanW)
	{ sPush(PullPush::End(), iChanW); }

void sPush(const PPT& iVal, const ChanW_PPT& iChanW)
	{ sEWrite<PPT>(iChanW, iVal); }

void sPull_UTF_Push_PPT(const ChanR_UTF& iChanR, const ChanW_PPT& iChanW)
	{
	PullPushPair<UTF32> thePullPushPair = sMakePullPushPair<UTF32>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	sCopyAll(iChanR, *thePullPushPair.first);
	sDisconnectWrite(*thePullPushPair.first);
	sSkipAll(iChanR);
	}

void sPull_UTF_Push_PPT(const ChanR_UTF& iChanR, uint64 iCount, const ChanW_PPT& iChanW)
	{
	PullPushPair<UTF32> thePullPushPair = sMakePullPushPair<UTF32>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	std::pair<uint64,uint64> counts = sCopyFully(iChanR, *thePullPushPair.first, iCount);
	sDisconnectWrite(*thePullPushPair.first);
	sSkipFully(iChanR, iCount - counts.first);
	}

void sPull_Bin_Push_PPT(const ChanR_Bin& iChanR, const ChanW_PPT& iChanW)
	{
	PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	sCopyAll(iChanR, *thePullPushPair.first);
	sDisconnectWrite(*thePullPushPair.first);
	sSkipAll(iChanR);
	}

void sPull_Bin_Push_PPT(const ChanR_Bin& iChanR, uint64 iCount, const ChanW_PPT& iChanW)
	{
	PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	std::pair<uint64,uint64> counts = sCopyFully(iChanR, *thePullPushPair.first, iCount);
	sDisconnectWrite(*thePullPushPair.first);
	sSkipFully(iChanR, iCount - counts.first);
	}

bool sCopy_Node(const ChanR_PPT& iChanR, const ChanW_PPT& iChanW)
	{
	size_t depth = 0;

	while (ZQ<PPT> theQ = sQRead(iChanR))
		{
		const PPT& thePPT = *theQ;

		sPush(thePPT, iChanW);

		if (sIsStart(thePPT))
			++depth;

		if (sIsEnd(thePPT))
			--depth;

		if (depth == 0)
			break;
		}
	return depth == 0;
	}

bool sSkip_Node(const ChanR_PPT& iChanR)
	{ return sCopy_Node(iChanR, ChanW_XX_Discard<PPT>()); }

bool sTryPull_Start_Map(const ChanRU<PPT>& iChanRU)
	{
	if (ZQ<PPT> theQ = sQRead(iChanRU))
		{
		if (sIsStartMap(*theQ))
			return true;
		sUnread(iChanRU, *theQ);
		}
	return false;
	}

bool sTryPull_Start_Seq(const ChanRU<PPT>& iChanRU)
	{
	if (ZQ<PPT> theQ = sQRead(iChanRU))
		{
		if (sIsStartSeq(*theQ))
			return true;
		sUnread(iChanRU, *theQ);
		}
	return false;
	}

bool sTryPull_End(const ChanRU<PPT>& iChanRU)
	{
	if (ZQ<PPT> theQ = sQRead(iChanRU))
		{
		if (sIsEnd(*theQ))
			return true;
		sUnread(iChanRU, *theQ);
		}
	return false;
	}

bool sTryPull_Name(const Name& iName, const ChanRU<PPT>& iChanRU)
	{
	if (ZQ<PPT> theQ = sQRead(iChanRU))
		{
		if (ZQ<Name> theNameQ = sQGet<Name>(*theQ))
			{
			if (iName == *theNameQ)
				return true;
			}
		sUnread(iChanRU, *theQ);
		}
	return false;
	}

void sEPull_End(const ChanR<PPT>& iChanR)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		if (sIsEnd(*theQ))
			return;
		}
	sThrow_ParseException("Required PullPush::End is absent");
	}

ZQ<PPT> sQEReadPPTOrEnd(const ChanR<PPT>& iChanR)
	{
	if (NotQ<PPT> thePPTQ = sQRead(iChanR))
		{
		sThrow_ParseException("Expected PPT, failed to read");
		}
	else if (sIsEnd(*thePPTQ))
		{
		return null;
		}
	else
		{
		return thePPTQ;
		}
	}

ZQ<Name> sQEReadNameOrEnd(const ChanR<PPT>& iChanR)
	{
	if (NotQ<PPT> thePPTQ = sQEReadPPTOrEnd(iChanR))
		{
		return null;
		}
	else if (const Name* theNameStar = sPGet<Name>(*thePPTQ))
		{
		return *theNameStar;
		}
	else
		{
		return null;
		}
	}

} // namespace ZooLib
