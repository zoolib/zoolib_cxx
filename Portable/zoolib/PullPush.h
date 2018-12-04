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
#include "zoolib/Chan_XX_PipePair.h"
#include "zoolib/Channer.h"
#include "zoolib/ChanR.h"
#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW.h"
#include "zoolib/Name.h"

#include <string>

namespace ZooLib {
namespace PullPush {

// =================================================================================================
#pragma mark - 

struct StartMap {};
extern const Any kStartMap;

struct StartSeq {};
extern const Any kStartSeq;

struct End {};
extern const Any kEnd;

} // namespace PullPush

using ChanR_Any = ChanR<Any>;
using ChannerR_Any = Channer<ChanR_Any>;

using ChanW_Any = ChanW<Any>;
using ChannerW_Any = Channer<ChanW_Any>;

using ChanWCon_Any = ChanWCon<Any>;
using ChannerWCon_Any = Channer<ChanWCon_Any>;

void sPush(const Any& iVal, const ChanW_Any& iChanW);

template <class T>
void sPush(const T& iVal, const ChanW_Any& iChanW)
	{ sEWrite<Any>(iChanW, Any(iVal)); }

// Special-case NotRef -- we generally want them on the chan as ZRef.
template <class T>
void sPush(const ZRef<T,false>& iVal, const ChanW_Any& iChanW)
	{ sPush(sRef(iVal), iChanW); }

template <class T>
void sPush(const Name& iName, const T& iVal, const ChanW_Any& iChanW)
	{
	sPush(iName, iChanW);
	sPush(iVal, iChanW);
	}

void sPull_UTF_Push(const ChanR_UTF& iChanR, const ChanW_Any& iChanW);
void sPull_UTF_Push(const ChanR_UTF& iChanR, uint64 iCount, const ChanW_Any& iChanW);

void sPull_Bin_Push(const ChanR_Bin& iChanR, const ChanW_Any& iChanW);
void sPull_Bin_Push(const ChanR_Bin& iChanR, uint64 iCount, const ChanW_Any& iChanW);

// ----------

bool sCopy_Node(const ChanR_Any& iChanR, const ChanW_Any& iChanW);
bool sSkip_Node(const ChanR_Any& iChanR);

// ----------

bool sTryPull_StartMap(const ChanRU<Any>& iChanRU);
bool sTryPull_StartSeq(const ChanRU<Any>& iChanRU);
bool sTryPull_End(const ChanRU<Any>& iChanRU);
bool sTryPull_Name(const Name& iName, const ChanRU<Any>& iChanRU);

// ----------

// Throw if the chan is empty, return a false ZQ<Any> if we hit an end, otherwise return the Any.
ZQ<Any> sQEReadAnyOrEnd(const ChanR<Any>& iChanR);

// Throw if the chan is empty, return a false ZQ<Name> if we hit an end, otherwise return the Name.
ZQ<Name> sQEReadNameOrEnd(const ChanR<Any>& iChanR);

// ----------

template <class EE>
using PullPushPair = std::pair<ZRef<ChannerWCon<EE>>,ZRef<ChannerR<EE>>>;

// ----------

template <class EE>
PullPushPair<EE> sMakePullPushPair()
	{
	ZRef<ImpPipePair<EE>> theImp = new ImpPipePair<EE>;
	ZRef<Channer<ChanWCon<EE>>> theChannerWCon = sChanner_T<ChanWCon_PipePair<EE>>(theImp);
	ZRef<Channer<ChanR<EE>>> theChannerR = sChanner_T<ChanR_PipePair<EE>>(theImp);
	return PullPushPair<EE>(theChannerWCon, theChannerR);
	}

} // namespace ZooLib

#endif // __ZooLib_PullPush_h__
