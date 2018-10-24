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

#ifndef __ZooLib_PullPush_h__
#define __ZooLib_PullPush_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"
#include "zoolib/Chan.h"
#include "zoolib/ChanR.h"
#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW.h"
#include "zoolib/Channer.h"
#include "zoolib/Name.h"
#include "zoolib/ZQ.h"

#include <string>

namespace ZooLib {
namespace PullPush {

// =================================================================================================
#pragma mark -
#pragma mark

struct StartMap {};
extern const Any kStartMap;

struct StartSeq {};
extern const Any kStartSeq;

struct End {};
extern const Any kEnd;

} // namespace PullPush

using ChanR_Any = ChanR<Any>;
using ChanW_Any = ChanW<Any>;
using ChanRCon_Bin = ChanRCon<byte>;

using ChanWCon_Any = ChanWCon<Any>;
using ChannerWCon_Any = Channer<ChanWCon_Any>;

void sPush(const ChanW_Any& iChanW, const Any& iVal);

template <class T>
void sPush(const ChanW_Any& iChanW, const T& iVal)
	{ sEWrite<Any>(iChanW, Any(iVal)); }

void sPullPush_UTF(const ChanR_UTF& iChanR, const ChanW_Any& iChanW);
void sPullPush_UTF(const ChanR_UTF& iChanR, const ChanW_Any& iChanW, uint64 iCount);

void sPullPush_Bin(const ChanR_Bin& iChanR, const ChanW_Any& iChanW);
void sPullPush_Bin(const ChanR_Bin& iChanR, const ChanW_Any& iChanW, uint64 iCount);

} // namespace ZooLib

#endif // __ZooLib_PullPush_h__
