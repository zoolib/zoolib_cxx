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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_set.h"

#include "zoolib/dataspace/WrappedDatonSet.h"

namespace ZooLib {
namespace Dataspace {

using namespace ZUtil_STL;
using ZDatonSet::DatonSet;
using std::set;
using std::swap;

// =================================================================================================
// MARK: - WrappedDatonSet

WrappedDatonSet::WrappedDatonSet(
	ZRef<DatonSet> iDatonSet,
	ZRef<Callable_NeedsUpdate> iCallable_NeedsUpdate,
	ZRef<Callable_PullSuggested> iCallable_PullSuggested_Parent)
:	fDatonSet_Committed(iDatonSet)
,	fDatonSet_Active(fDatonSet_Committed->Fork())
,	fCalled_NeedsUpdate(false)
,	fCallable_NeedsUpdate(iCallable_NeedsUpdate)
	{
	ZAssert(iCallable_PullSuggested_Parent);
	sInsert(fCallables_PullSuggested, iCallable_PullSuggested_Parent);
	}

WrappedDatonSet::WrappedDatonSet(
	ZRef<DatonSet> iDatonSet,
	ZRef<Callable_NeedsUpdate> iCallable_NeedsUpdate)
:	fDatonSet_Committed(iDatonSet)
,	fDatonSet_Active(fDatonSet_Committed->Fork())
,	fCalled_NeedsUpdate(false)
,	fCallable_NeedsUpdate(iCallable_NeedsUpdate)
	{}

WrappedDatonSet::~WrappedDatonSet()
	{}

ZRef<DatonSet> WrappedDatonSet::GetDatonSet_Active()
	{
	ZGuardMtxR guard(fMtxR);

	// Assume something's going to munge it
	this->pTrigger_NeedsUpdate();
	return fDatonSet_Active;
	}

ZRef<DatonSet> WrappedDatonSet::GetDatonSet_Committed()
	{
	ZGuardMtxR guard(fMtxR);
	return fDatonSet_Committed;
	}

void WrappedDatonSet::Update()
	{
	ZGuardMtxR guard(fMtxR);
	fCalled_NeedsUpdate = false;

	set<ZRef<WrappedDatonSet> > toPullFrom;
	swap(toPullFrom, fPullFrom);

	guard.Release();

	bool anyChange = false;

	foreachv (ZRef<WrappedDatonSet> other, toPullFrom)
		{
		ZRef<DatonSet> otherDS = other->GetDatonSet_Committed()->Fork();
		if (fDatonSet_Active->Join(otherDS))
			anyChange = true;
		}

	guard.Acquire();

	if (fDatonSet_Committed->Join(fDatonSet_Active))
		{ anyChange = true; }

	fDatonSet_Active = fDatonSet_Committed->Fork();

	if (anyChange)
		{
		// Invoke all entries in fCallables_PullSuggested, preserving
		// only those which were valid (didn't return null).
		ZRef<WrappedDatonSet> ref_this = this;

		set<ZRef<Callable_PullSuggested> > toCall;
		swap(toCall, fCallables_PullSuggested);

		guard.Release();

		set<ZRef<Callable_PullSuggested> > preserved;
		foreachv (ZRef<Callable_PullSuggested> theCallable, toCall)
			{
			if (sQCall(theCallable, ref_this))
				sInsert(preserved, theCallable);
			}

		guard.Acquire();
		fCallables_PullSuggested.insert(preserved.begin(), preserved.end());
		}
	}

ZRef<WrappedDatonSet> WrappedDatonSet::Spawn(ZRef<ZCallable_Void> iCallable_UpdateNeeded)
	{
	ZGuardMtxR guard(fMtxR);

	ZRef<WrappedDatonSet> theWDS_Spawned = new WrappedDatonSet(
		fDatonSet_Committed->Fork(),
		iCallable_UpdateNeeded,
		this->pGet_Callable_PullSuggested_Self());

	// We have to get the spawned WDS' callable after it's constructed, because it
	// simply cannot build the callable in its ctor (counted/weak lifetime rules).
	sInsert(fCallables_PullSuggested, theWDS_Spawned->pGet_Callable_PullSuggested_Self());

	return theWDS_Spawned;
	}

ZRef<WrappedDatonSet::Callable_PullSuggested> WrappedDatonSet::pGet_Callable_PullSuggested_Self()
	{
	ZGuardMtxR guard(fMtxR);
	if (not fCallable_PullSuggested_Self)
		fCallable_PullSuggested_Self = sCallable(sWeakRef(this), &WrappedDatonSet::pPullSuggested);
	return fCallable_PullSuggested_Self;
	}

void WrappedDatonSet::pPullSuggested(ZRef<WrappedDatonSet> iOther)
	{
	ZGuardMtxR guard(fMtxR);
	sInsert(fPullFrom, iOther);

	this->pTrigger_NeedsUpdate();
	}

void WrappedDatonSet::pTrigger_NeedsUpdate()
	{
	if (not sGetSet(fCalled_NeedsUpdate, true))
		sCall(fCallable_NeedsUpdate);
	}

} // namespace Dataspace
} // namespace ZooLib
