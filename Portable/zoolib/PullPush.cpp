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
#include "zoolib/ChanConnection_XX_MemoryPipe.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Util_Chan.h" // For sCopyFully

namespace ZooLib {
namespace PullPush {

const Any kStartMap = Any(StartMap());
const Any kStartSeq = Any(StartSeq());

const Any kEnd = Any(End());

//const Any kEndMap = Any(End());
//const Any kEndSeq = Any(End());

} // namespace PullPush

void sPush(const ChanW_Any& iChanW, const Any& iVal)
	{ sEWrite<Any>(iChanW, iVal); }

void sPullPush_UTF(const ChanR_UTF& iChanR, const ChanW_Any& iChanW)
	{
	ZRef<Channer<ChanConnection<UTF32>>> theChannerPipe = new Channer_T<ChanConnection_XX_MemoryPipe<UTF32>>;
	sPush(iChanW, ZRef<ChannerR_UTF>(theChannerPipe));
	sCopyAll(iChanR, *theChannerPipe);
	sDisconnectWrite(*theChannerPipe);
	sSkipAll(iChanR);
	}

void sPullPush_UTF(const ChanR_UTF& iChanR, const ChanW_Any& iChanW, uint64 iCount)
	{
	ZRef<Channer<ChanConnection<UTF32>>> theChannerPipe = new Channer_T<ChanConnection_XX_MemoryPipe<UTF32>>;
	sPush(iChanW, ZRef<ChannerR_UTF>(theChannerPipe));
	std::pair<uint64,uint64> counts = sCopyFully(iChanR, *theChannerPipe, iCount);
	sDisconnectWrite(*theChannerPipe);
	sSkipFully(iChanR, iCount - counts.first);
	}

void sPullPush_Bin(const ChanR_Bin& iChanR, const ChanW_Any& iChanW)
	{
	ZRef<Channer<ChanConnection<byte>>> theChannerPipe = new Channer_T<ChanConnection_XX_MemoryPipe<byte>>;
	sPush(iChanW, ZRef<ChannerR_Bin>(theChannerPipe));
	sCopyAll(iChanR, *theChannerPipe);
	sDisconnectWrite(*theChannerPipe);
	sSkipAll(iChanR);
	}

void sPullPush_Bin(const ChanR_Bin& iChanR, const ChanW_Any& iChanW, uint64 iCount)
	{
	ZRef<Channer<ChanConnection<byte>>> theChannerPipe = new Channer_T<ChanConnection_XX_MemoryPipe<byte>>;
	sPush(iChanW, ZRef<ChannerR_Bin>(theChannerPipe));
	std::pair<uint64,uint64> counts = sCopyFully(iChanR, *theChannerPipe, iCount);
	sDisconnectWrite(*theChannerPipe);
	sSkipFully(iChanR, iCount - counts.first);
	}

} // namespace ZooLib
