/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Dataspace_Melange_h__
#define __ZooLib_Dataspace_Melange_h__ 1
#include "zconfig.h"

#include "zoolib/Caller.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZMulti_T.h"

#include "zooLib/dataspace/RelsWatcher.h"
#include "zoolib/datonset/ZDatonSet.h"

// =================================================================================================
// MARK: -

namespace ZooLib {
namespace Dataspace {

typedef ZCallable<ZRef<Event>(const ZQ<ZDatonSet::Daton>& iPrior, const ZQ<ZDatonSet::Daton>& iNew)>
	Callable_DatonSetUpdate;

typedef ZMulti_T3<
	ZRef<RelsWatcher::Callable_Register>,
	ZRef<Callable_DatonSetUpdate>,
	ZRef<Caller>
	> Melange_t;

typedef ZCallable<Melange_t()> MelangeFactory;

typedef ZCallable<void(ZRef<Event> iEvent, ZRef<ZDatonSet::Deltas>& oDeltas, ZRef<Event>& oEvent)>
	Callable_PullFrom;

typedef ZCallable<void(const ZRef<Callable_PullFrom>& iCallable_PullFrom)> Callable_PullSuggested;

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Melange_h__
