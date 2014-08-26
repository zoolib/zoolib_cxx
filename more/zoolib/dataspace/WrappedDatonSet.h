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

#ifndef __ZooLib_Dataspace_WrappedDatonSet_h__
#define __ZooLib_Dataspace_WrappedDatonSet_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ValueOnce.h"

#include "zoolib/datonset/ZDatonSet.h"
#include "zooLib/dataspace/Melange.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
// MARK: - WrappedDatonSet

class WrappedDatonSet
:	public ZCounted
	{
public:
	typedef ZDatonSet::DatonSet DatonSet;
	typedef ZCallable_Void Callable_NeedsUpdate;

	WrappedDatonSet(const ZRef<DatonSet>& iDatonSet,
		const ZRef<Callable_NeedsUpdate>& iCallable_NeedsUpdate);

	virtual ~WrappedDatonSet();

// From ZCounted
	virtual void Initialize();

// Our protocol
	ZRef<DatonSet> GetDatonSet_Active();

	ZRef<DatonSet> GetDatonSet_Committed();

	void Update();

	void SuggestPull(const ZRef<Callable_PullSuggested>& iCallable_PullSuggested);

	void InsertCallable_PullSuggested(const ZRef<Callable_PullSuggested>& iCallable_PullSuggested);

	ZRef<Callable_PullSuggested> GetCallable_PullSuggested();

private:
	void pPullSuggested(const ZRef<Callable_PullFrom>& iCallable_PullFrom);

	void pPullFrom(ZRef<Event> iEvent, ZRef<ZDatonSet::Deltas>& oDeltas, ZRef<Event>& oEvent);

	// ---

	ZMtxR fMtxR;
	ZCnd fCnd;

	ZRef<DatonSet> fDatonSet_Committed;
	ZRef<DatonSet> fDatonSet_Active;

	FalseOnce fCalled_NeedsUpdate;
	ZRef<Callable_NeedsUpdate> fCallable_NeedsUpdate;

	ZRef<Callable_PullSuggested> fCallable_PullSuggested_Self;

	std::set<ZRef<Callable_PullSuggested> > fCallables_PullSuggested;

	ZRef<Callable_PullFrom> fCallable_PullFrom_Self;

	std::set<ZRef<Callable_PullFrom> > fCallables_PullFrom;
	};

ZRef<WrappedDatonSet> sSpawned(const ZRef<WrappedDatonSet>& iParent,
	const ZRef<WrappedDatonSet::Callable_NeedsUpdate>& iCallable_NeedsUpdate);

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_WrappedDatonSet_h__
