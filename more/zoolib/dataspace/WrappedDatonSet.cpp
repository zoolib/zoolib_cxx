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

WrappedDatonSet::WrappedDatonSet(const ZRef<DatonSet>& iDatonSet)
:	fDatonSet_Committed(iDatonSet)
,	fDatonSet_Active(fDatonSet_Committed->Fork())
,	fCalled_NeedsUpdate(false)
	{}

WrappedDatonSet::~WrappedDatonSet()
	{}

void WrappedDatonSet::Initialize()
	{
	ZCounted::Initialize();

	fCallable_PullFrom_Self = sCallable(sWeakRef(this), &WrappedDatonSet::pPullFrom);
	}

ZRef<DatonSet> WrappedDatonSet::GetDatonSet_Active()
	{
	ZGuardMtxR guard(fMtxR);

	ZRef<DatonSet> result = fDatonSet_Active;
	if (not sGetSet(fCalled_NeedsUpdate, true))
		{
		guard.Release();
		sCall(fCallable_NeedsUpdate);
		}

	return result;
	}

ZRef<DatonSet> WrappedDatonSet::GetDatonSet_Committed()
	{
	ZGuardMtxR guard(fMtxR);
	return fDatonSet_Committed;
	}

void WrappedDatonSet::SetCallable_NeedsUpdate(
	const ZRef<Callable_NeedsUpdate>& iCallable_NeedsUpdate)
	{
	ZAssert(not fCallable_NeedsUpdate);
	fCallable_NeedsUpdate = iCallable_NeedsUpdate;
	}

void WrappedDatonSet::Update()
	{
	ZGuardMtxR guard(fMtxR);
	fCalled_NeedsUpdate = false;

	set<ZRef<Callable_PullFrom> > theCallables_PullFrom;
	swap(theCallables_PullFrom, fCallables_PullFrom);

	guard.Release();

	bool anyChange = false;

	foreachv (ZRef<Callable_PullFrom> theCallable, theCallables_PullFrom)
		{
		ZRef<ZDatonSet::Deltas> theDeltas;
		ZRef<Event> theEvent;
		theCallable->Call(fDatonSet_Active->GetEvent(), theDeltas, theEvent);
		if (fDatonSet_Active->IncorporateDeltas(theDeltas, theEvent))
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

		for (set<ZRef<Callable_PullSuggested> >::iterator iter = toCall.begin();
			/*no test*/; /*no inc*/)
			{
			set<ZRef<Callable_PullSuggested> >::iterator iterNext = iter;
			++iterNext;
			if (not sQCall(*iter, fCallable_PullFrom_Self))
				toCall.erase(iter);
			if (iterNext == toCall.end())
				break;
			iter = iterNext;
			}

		guard.Acquire();
		fCallables_PullSuggested.insert(toCall.begin(), toCall.end());
		}
	}

void WrappedDatonSet::PokeThis(const ZRef<Callable_PullSuggested>& iCallable_PullSuggested)
	{ sCall(iCallable_PullSuggested, fCallable_PullFrom_Self); }

void WrappedDatonSet::InsertCallable_PullSuggested(
	ZRef<Callable_PullSuggested> iCallable_PullSuggested)
	{
	ZGuardMtxR guard(fMtxR);
	sInsert(fCallables_PullSuggested, iCallable_PullSuggested);
	}

ZRef<Callable_PullSuggested> WrappedDatonSet::GetCallable_PullSuggested()
	{
	ZGuardMtxR guard(fMtxR);
	if (not fCallable_PullSuggested_Self)
		fCallable_PullSuggested_Self = sCallable(sWeakRef(this), &WrappedDatonSet::pPullSuggested);
	return fCallable_PullSuggested_Self;
	}

void WrappedDatonSet::pPullSuggested(const ZRef<Callable_PullFrom>& iCallable_PullFrom)
	{
	ZGuardMtxR guard(fMtxR);
	sInsert(fCallables_PullFrom, iCallable_PullFrom);

	if (not sGetSet(fCalled_NeedsUpdate, true))
		{
		guard.Release();
		sCall(fCallable_NeedsUpdate);
		}
	}

void WrappedDatonSet::pPullFrom(
	ZRef<Event> iEvent, ZRef<ZDatonSet::Deltas>& oDeltas, ZRef<Event>& oEvent)
	{
	ZGuardMtxR guard(fMtxR);
	fDatonSet_Committed->GetDeltas(iEvent, oDeltas, oEvent);
	}


ZRef<WrappedDatonSet> sSpawned(const ZRef<WrappedDatonSet>& iParent,
	const ZRef<WrappedDatonSet::Callable_NeedsUpdate>& iCallable_NeedsUpdate)
	{
	ZRef<WrappedDatonSet> result =
		new WrappedDatonSet(iParent->GetDatonSet_Committed()->Fork());

	result->InsertCallable_PullSuggested(iParent->GetCallable_PullSuggested());
	iParent->InsertCallable_PullSuggested(result->GetCallable_PullSuggested());

	result->SetCallable_NeedsUpdate(iCallable_NeedsUpdate);

	return result;
	}

} // namespace Dataspace
} // namespace ZooLib
