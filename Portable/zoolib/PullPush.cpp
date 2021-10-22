// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/PullPush.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/ParseException.h"
#include "zoolib/Util_Chan.h" // For sCopyFully

#include "zoolib/Log.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

namespace PullPush {

const PPT Start::sPPT = ZP<Marker>(new Start);

bool Start::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<Start>())
			return true;
		}
	return false;
	}

// -------------------------------------------------------------------------------------------------

const PPT Start_Map::sPPT = ZP<Marker>(new Start_Map);

bool Start_Map::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<Start_Map>())
			return true;
		}
	return false;
	}

// -------------------------------------------------------------------------------------------------

const PPT Start_Seq::sPPT = ZP<Marker>(new Start_Seq);

bool Start_Seq::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<Start_Seq>())
			return true;
		}
	return false;
	}

// -------------------------------------------------------------------------------------------------

const PPT End::sPPT = ZP<Marker>(new End);

bool End::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<End>())
			return true;
		}
	return false;
	}

// -------------------------------------------------------------------------------------------------

//bool sIsMarker(const PPT& iPPT)
//	{
//	if (const ZP<Marker> theRef = sGet<ZP<Marker>>(iPPT))
//		return true;
//	return false;
//	}

bool sIsStart(const PPT& iPPT)
	{ return Start::sIs(iPPT); }

bool sIsStart_Map(const PPT& iPPT)
	{ return Start_Map::sIs(iPPT); }

bool sIsStart_Seq(const PPT& iPPT)
	{ return Start_Seq::sIs(iPPT); }

bool sIsEnd(const PPT& iPPT)
	{ return End::sIs(iPPT); }

} // namespace PullPush

// =================================================================================================
#pragma mark -

void sPush_Start_Map(const ChanW_PPT& iChanW)
	{ sPush(PullPush::Start_Map::sPPT, iChanW); }

void sPush_Start_Seq(const ChanW_PPT& iChanW)
	{ sPush(PullPush::Start_Seq::sPPT, iChanW); }

void sPush_End(const ChanW_PPT& iChanW)
	{ sPush(PullPush::End::sPPT, iChanW); }

void sPush_Marker(const ZP<PullPush::Marker>& iMarker, const ChanW_PPT& iChanW)
	{ sPush(PPT(iMarker), iChanW); }

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

bool sCopy_Node(const ChanR_PPT& iChanR, size_t iStartDepth, const ChanW_PPT& iChanW)
	{
	size_t depth = iStartDepth;

	while (ZQ<PPT> theQ = sQRead(iChanR))
		{
		const PPT& thePPT = *theQ;

		sPush(thePPT, iChanW);

		if (sIsStart(thePPT))
			++depth;

		else if (sIsEnd(thePPT))
			--depth;

		if (depth == 0)
			return true;
		}

	// We'll never have entered the loop, so there was nothing to read at all.
	return false;
	}

bool sSkip_Node(const ChanR_PPT& iChanR)
	{ return sCopy_Node(iChanR, 0, ChanW_XX_Discard<PPT>()); }

bool sSkip_Node(const ChanR_PPT& iChanR, size_t iStartDepth)
	{ return sCopy_Node(iChanR, 1, ChanW_XX_Discard<PPT>()); }

bool sTryPull_Start_Map(const ChanRU<PPT>& iChanRU)
	{
	if (ZQ<PPT> theQ = sQRead(iChanRU))
		{
		if (sIsStart_Map(*theQ))
			return true;
		sUnread(iChanRU, *theQ);
		}
	return false;
	}

bool sTryPull_Start_Seq(const ChanRU<PPT>& iChanRU)
	{
	if (ZQ<PPT> theQ = sQRead(iChanRU))
		{
		if (sIsStart_Seq(*theQ))
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
		sThrow_ParseException("Expected Name");
		}
	}

} // namespace ZooLib
