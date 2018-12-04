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
namespace PullPush {

const Any kStartMap = Any(StartMap());
const Any kStartSeq = Any(StartSeq());

const Any kEnd = Any(End());

} // namespace PullPush

void sPush(const Any& iVal, const ChanW_Any& iChanW)
	{ sEWrite<Any>(iChanW, iVal); }

void sPull_UTF_Push(const ChanR_UTF& iChanR, const ChanW_Any& iChanW)
	{
	PullPushPair<UTF32> thePullPushPair = sMakePullPushPair<UTF32>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	sCopyAll(iChanR, *thePullPushPair.first);
	sDisconnectWrite(*thePullPushPair.first);
	sSkipAll(iChanR);
	}

void sPull_UTF_Push(const ChanR_UTF& iChanR, uint64 iCount, const ChanW_Any& iChanW)
	{
	PullPushPair<UTF32> thePullPushPair = sMakePullPushPair<UTF32>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	std::pair<uint64,uint64> counts = sCopyFully(iChanR, *thePullPushPair.first, iCount);
	sDisconnectWrite(*thePullPushPair.first);
	sSkipFully(iChanR, iCount - counts.first);
	}

void sPull_Bin_Push(const ChanR_Bin& iChanR, const ChanW_Any& iChanW)
	{
	PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	sCopyAll(iChanR, *thePullPushPair.first);
	sDisconnectWrite(*thePullPushPair.first);
	sSkipAll(iChanR);
	}

void sPull_Bin_Push(const ChanR_Bin& iChanR, uint64 iCount, const ChanW_Any& iChanW)
	{
	PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	std::pair<uint64,uint64> counts = sCopyFully(iChanR, *thePullPushPair.first, iCount);
	sDisconnectWrite(*thePullPushPair.first);
	sSkipFully(iChanR, iCount - counts.first);
	}

bool sCopy_Node(const ChanR_Any& iChanR, const ChanW_Any& iChanW)
	{
	using namespace PullPush;

	size_t depth = 0;

	while (ZQ<Any> theQ = sQRead(iChanR))
		{
		const Any& theAny = *theQ;

		sPush(theAny, iChanW);

		if (sPGet<StartMap>(theAny) || sPGet<StartSeq>(theAny))
			{
			++depth;
			}
		else if (sPGet<End>(theAny) && 0 == --depth)
			{
			break;
			}
		}
	return depth == 0;
	}

bool sSkip_Node(const ChanR_Any& iChanR)
	{ return sCopy_Node(iChanR, ChanW_XX_Discard<Any>()); }

bool sTryPull_StartMap(const ChanRU<Any>& iChanRU)
	{
	if (ZQ<Any> theQ = sQRead(iChanRU))
		{
		if (sPGet<PullPush::StartMap>(*theQ))
			return true;
		sUnread(iChanRU, *theQ);
		}
	return false;
	}

bool sTryPull_StartSeq(const ChanRU<Any>& iChanRU)
	{
	if (ZQ<Any> theQ = sQRead(iChanRU))
		{
		if (sPGet<PullPush::StartSeq>(*theQ))
			return true;
		sUnread(iChanRU, *theQ);
		}
	return false;
	}

bool sTryPull_End(const ChanRU<Any>& iChanRU)
	{
	if (ZQ<Any> theQ = sQRead(iChanRU))
		{
		if (sPGet<PullPush::End>(*theQ))
			return true;
		sUnread(iChanRU, *theQ);
		}
	return false;
	}

bool sTryPull_Name(const Name& iName, const ChanRU<Any>& iChanRU)
	{
	if (ZQ<Any> theQ = sQRead(iChanRU))
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

ZQ<Any> sQEReadAnyOrEnd(const ChanR<Any>& iChanR)
	{
	if (NotQ<Any> theAnyQ = sQRead(iChanR))
		{
		sThrow_ParseException("Expected Any, failed to read");
		return null;
		}
	else if (not sPGet<PullPush::End>(*theAnyQ))
		{
		return null;
		}
	else
		{
		return theAnyQ;
		}
	}

ZQ<Name> sQEReadNameOrEnd(const ChanR<Any>& iChanR)
	{
	if (NotQ<Any> theAnyQ = sQEReadAnyOrEnd(iChanR))
		{
		return null;
		}
	else if (const Name* theNameStar = sPGet<Name>(*theAnyQ))
		{
		return *theNameStar;
		}
	else
		{
		return null;
		}
	}

} // namespace ZooLib
