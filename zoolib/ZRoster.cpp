/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZRoster.h"
#include "zoolib/ZUtil_STL_set.h"

#include <vector>

namespace ZooLib {

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZRoster

ZRoster::ZRoster()
	{}

ZRoster::~ZRoster()
	{}

void ZRoster::Finalize()
	{
	ZGuardRMtx guard(fMtx);
	vector<ZRef<Entry> > local(fEntries.begin(), fEntries.end());

	for (vector<ZRef<Entry> >::const_iterator i = local.begin(); i != local.end(); ++i)
		(*i)->fRoster.Clear();

	if (not this->FinishFinalize())
		{
		// Someone snagged a strong reference, reinstate weak references.
		ZRef<ZRoster> thisRef = this;
		for (vector<ZRef<Entry> >::const_iterator i = local.begin(); i != local.end(); ++i)
			(*i)->fRoster = sRef(thisRef);
		}
	else
		{
		guard.Release();
		for (vector<ZRef<Entry> >::const_iterator i = local.begin(); i != local.end(); ++i)
			sCall((*i)->fCallable_RosterGone.Get());
		}
	}

ZRef<ZRoster::Entry> ZRoster::MakeEntry()
	{
	ZGuardRMtx guard(fMtx);
	ZRef<Entry> theEntry = new Entry;
	theEntry->fRoster = sWeakRef(this);
	ZUtil_STL::sInsertMustNotContain(1, fEntries, theEntry);
	fCnd.Broadcast();
	if (ZRef<Callable_Change> theCallable = fCallable_Change)
		{
		guard.Release();
		theCallable->Call();
		}
	return theEntry;
	}

void ZRoster::Broadcast()
	{
	ZGuardRMtx guard(fMtx);
	vector<ZRef<Entry> > theEntries(fEntries.begin(), fEntries.end());
	guard.Release();

	for (vector<ZRef<Entry> >::const_iterator iter = theEntries.begin();
		iter != theEntries.end(); ++iter)
		{ sCall((*iter)->fCallable_Broadcast.Get()); }
	}

size_t ZRoster::Count()
	{
	ZAcqMtx acq(fMtx);
	return fEntries.size();
	}

void ZRoster::Wait(size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	while (fEntries.size() != iCount)
		fCnd.Wait(fMtx);
	}

bool ZRoster::WaitFor(double iTimeout, size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	if (fEntries.size() != iCount)
		fCnd.WaitFor(fMtx, iTimeout);
	return fEntries.size() != iCount;
	}

bool ZRoster::WaitUntil(ZTime iDeadline, size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	if (fEntries.size() != iCount)
		fCnd.WaitUntil(fMtx, iDeadline);
	return fEntries.size() != iCount;
	}

ZRef<ZRoster::Callable_Change> ZRoster::GetSet_Callable_Change(ZRef<Callable_Change> iCallable)
	{
	ZAcqMtx acq(fMtx);
	return sGetSet(fCallable_Change, iCallable);
	}

void ZRoster::pFinalizeEntry(Entry* iEntry)
	{
	ZGuardRMtx guard(fMtx);
	
	if (iEntry->FinishFinalize())
		{
		ZUtil_STL::sEraseMustContain(1, fEntries, iEntry);
		delete iEntry;
		fCnd.Broadcast();
		if (ZRef<Callable_Change> theCallable = fCallable_Change)
			{
			guard.Release();
			theCallable->Call();
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRoster::Entry

ZRoster::Entry::Entry()
	{}

ZRoster::Entry::~Entry()
	{}

void ZRoster::Entry::Finalize()
	{
	if (ZRef<ZRoster> theRoster = fRoster)
		theRoster->pFinalizeEntry(this);
	else
		ZCounted::Finalize();
	}

ZRef<ZRoster::Entry::Callable_Broadcast>
ZRoster::Entry::GetSet_Callable_Broadcast(ZRef<Callable_Broadcast> iCallable)
	{ return fCallable_Broadcast.GetSet(iCallable); }

ZRef<ZRoster::Entry::Callable_RosterGone>
ZRoster::Entry::GetSet_Callable_RosterGone(ZRef<Callable_RosterGone> iCallable)
	{ return fCallable_RosterGone.GetSet(iCallable); }

} // namespace ZooLib
