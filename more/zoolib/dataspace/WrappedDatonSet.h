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
#include "zoolib/datonset/ZDatonSet.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
// MARK: - WrappedDatonSet

class WrappedDatonSet
:	public ZCounted
	{
public:
	typedef ZCallable_Void Callable_NeedsUpdate;

	typedef ZCallable<void(ZRef<WrappedDatonSet>)> Callable_PullSuggested;

	typedef ZDatonSet::DatonSet DatonSet;

protected:
// Spawn ctor
	WrappedDatonSet(ZRef<DatonSet> iDatonSet,
		ZRef<Callable_NeedsUpdate> iCallable_NeedsUpdate,
		ZRef<Callable_PullSuggested> iCallable_PullSuggested_Parent);

public:
// Root ctor
	WrappedDatonSet(ZRef<DatonSet> iDatonSet,
		ZRef<Callable_NeedsUpdate> iCallable_NeedsUpdate);

	virtual ~WrappedDatonSet();

// Our protocol
	ZRef<DatonSet> GetDatonSet_Active();

	ZRef<DatonSet> GetDatonSet_Committed();

	void Update();

	ZRef<WrappedDatonSet> Spawn(ZRef<ZCallable_Void> iCallable_UpdateNeeded);

private:
	ZRef<Callable_PullSuggested> pGet_Callable_PullSuggested_Self();

	void pPullSuggested(ZRef<WrappedDatonSet> iOther);

	void pTrigger_NeedsUpdate();

	// ---

	ZMtxR fMtxR;
	ZCnd fCnd;

	ZRef<DatonSet> fDatonSet_Committed;
	ZRef<DatonSet> fDatonSet_Active;

	bool fCalled_NeedsUpdate;
	ZRef<Callable_NeedsUpdate> fCallable_NeedsUpdate;

	ZRef<Callable_PullSuggested> fCallable_PullSuggested_Self;

	std::set<ZRef<Callable_PullSuggested> > fCallables_PullSuggested;

	std::set<ZRef<WrappedDatonSet> > fPullFrom;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_WrappedDatonSet_h__
