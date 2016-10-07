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

#include "zoolib/Callable_PMF.h"
#include "zoolib/Util_STL_set.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/DatonSet/WrappedDatonSet.h"

namespace ZooLib {
namespace DatonSet {

using namespace Util_STL;
using std::set;
using std::swap;

// =================================================================================================
#pragma mark -
#pragma mark WrappedDatonSet

WrappedDatonSet::WrappedDatonSet(const ZRef<DatonSet>& iDatonSet,
	const ZRef<Callable_NeedsUpdate>& iCallable_NeedsUpdate)
:	fDatonSet_Committed(iDatonSet)
,	fDatonSet_Active(fDatonSet_Committed->Fork())
,	fCallable_NeedsUpdate(iCallable_NeedsUpdate)
	{}

WrappedDatonSet::~WrappedDatonSet()
	{}

void WrappedDatonSet::Initialize()
	{
	ZCounted::Initialize();

	fCallable_PullSuggested_Self = sCallable(sWeakRef(this), &WrappedDatonSet::pPullSuggested);
	fCallable_PullFrom_Self = sCallable(sWeakRef(this), &WrappedDatonSet::pPullFrom);
	}

ZRef<DatonSet> WrappedDatonSet::GetDatonSet_Active()
	{
	ZGuardMtxR guard(fMtxR);

	ZRef<DatonSet> result = fDatonSet_Active;
	if (not fCalled_NeedsUpdate())
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

void WrappedDatonSet::Update()
	{
	ZGuardMtxR guard(fMtxR);
	fCalled_NeedsUpdate.Reset();

	set<ZRef<Callable_PullFrom> > theCallables_PullFrom;
	swap(theCallables_PullFrom, fCallables_PullFrom);

	guard.Release();

	bool anyChange = false;

	foreachv (ZRef<Callable_PullFrom> theCallable, theCallables_PullFrom)
		{
		ZRef<Deltas> theDeltas = theCallable->Call(fDatonSet_Active->GetEvent());
		if (fDatonSet_Active->IncorporateDeltas(theDeltas))
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

void WrappedDatonSet::SuggestPull(const ZRef<Callable_PullSuggested>& iCallable_PullSuggested)
	{ sCall(iCallable_PullSuggested, fCallable_PullFrom_Self); }

void WrappedDatonSet::InsertCallable_PullSuggested(
	const ZRef<Callable_PullSuggested>& iCallable_PullSuggested)
	{
	ZGuardMtxR guard(fMtxR);
	sInsert(fCallables_PullSuggested, iCallable_PullSuggested);
	}

ZRef<Callable_PullSuggested> WrappedDatonSet::GetCallable_PullSuggested()
	{ return fCallable_PullSuggested_Self; }

void WrappedDatonSet::pPullSuggested(const ZRef<Callable_PullFrom>& iCallable_PullFrom)
	{
	ZGuardMtxR guard(fMtxR);
	sInsert(fCallables_PullFrom, iCallable_PullFrom);

	if (not fCalled_NeedsUpdate())
		{
		guard.Release();
		sCall(fCallable_NeedsUpdate);
		}
	}

ZRef<Deltas> WrappedDatonSet::pPullFrom(ZRef<Event> iEvent)
	{
	ZGuardMtxR guard(fMtxR);
	return fDatonSet_Committed->GetDeltas(iEvent);
	}

ZRef<WrappedDatonSet> sSpawned(const ZRef<WrappedDatonSet>& iParent,
	const ZRef<WrappedDatonSet::Callable_NeedsUpdate>& iCallable_NeedsUpdate)
	{
	ZRef<WrappedDatonSet> result =
		new WrappedDatonSet(iParent->GetDatonSet_Committed()->Fork(), iCallable_NeedsUpdate);

	result->InsertCallable_PullSuggested(iParent->GetCallable_PullSuggested());
	iParent->InsertCallable_PullSuggested(result->GetCallable_PullSuggested());

	return result;
	}

} // namespace DatonSet
} // namespace ZooLib
