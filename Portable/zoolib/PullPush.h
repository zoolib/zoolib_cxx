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
#include "zoolib/ChanConnection_XX_MemoryPipe.h"
#include "zoolib/ChanFilter.h"
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
#pragma mark

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

template <class EE>
using PullPushPair = std::pair<ZRef<ChannerWCon<EE>>,ZRef<ChannerR<EE>>>;

// ----------

template <class EE>
class Channer_On_Finalize_DisconnectRead
: public Channer_Channer_T<ChanFilter<ChanRCon<EE>>>
	{
	typedef Channer_Channer_T<ChanFilter<ChanRCon<EE>>> inherited;
public:
	Channer_On_Finalize_DisconnectRead(const ZRef<Channer<ChanRCon<EE>>>& iChanner)
	:	inherited(iChanner)
		{}

	virtual void Finalize()
		{
		sDisconnectRead(inherited::pGetChan());
		inherited::Finalize();
		}
	};

// ----------

template <class EE>
PullPushPair<EE> sMakePullPushPair()
	{
	ZRef<Channer<ChanConnection<EE>>> thePipeChanner =
		new Channer_T<ChanConnection_XX_MemoryPipe<EE>>;

	ZRef<Channer<ChanRCon<EE>>> theChannerRCon =
		new Channer_On_Finalize_DisconnectRead<EE>(thePipeChanner);

	return PullPushPair<EE>(thePipeChanner, theChannerRCon);
	}

} // namespace ZooLib

#endif // __ZooLib_PullPush_h__
