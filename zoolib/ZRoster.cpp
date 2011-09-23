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
	ZRef<ZCallable_Void> theCallable = fCallable_Gone;

	for (vector<ZRef<Entry> >::const_iterator ii = local.begin(); ii != local.end(); ++ii)
		(*ii)->fRoster.Clear();

	if (this->FinishFinalize())
		{
		guard.Release();
		if (theCallable)
			theCallable->Call();
		delete this;
		}
	else
		{
		// Rare/impossible? Someone snagged a strong reference, reinstate entries' weak references.
		ZRef<ZRoster> thisRef = this;
		for (vector<ZRef<Entry> >::const_iterator ii = local.begin(); ii != local.end(); ++ii)
			(*ii)->fRoster = thisRef;
		}
	}

ZRef<ZRoster::Entry> ZRoster::MakeEntry()
	{ return this->MakeEntry(null); }

ZRef<ZRoster::Entry> ZRoster::MakeEntry(ZRef<ZCallable_Void> iCallable_Broadcast)
	{
	ZRef<ZRoster> thisRef = this;

	ZGuardRMtx guard(fMtx);

	ZRef<Entry> theEntry = new Entry(thisRef, iCallable_Broadcast, null);
	ZUtil_STL::sInsertMustNotContain(1, fEntries, theEntry.Get());
	fCnd.Broadcast();

	if (ZRef<ZCallable_Void> theCallable = fCallable_Change)
		{
		guard.Release();
		theCallable->Call();
		}

	return theEntry;
	}

void ZRoster::Broadcast()
	{
	ZGuardRMtx guard(fMtx);
	vector<ZRef<Entry> > local(fEntries.begin(), fEntries.end());
	guard.Release();

	for (vector<ZRef<Entry> >::const_iterator ii = local.begin(); ii != local.end(); ++ii)
		{
		ZGuardRMtx guardEntry((*ii)->fMtx);
		if (ZRef<ZCallable_Void> theCallable = (*ii)->fCallable_Broadcast)
			{
			guardEntry.Release();
			theCallable->Call();
			guardEntry.Acquire();
			}
		}
	}

size_t ZRoster::Count()
	{
	ZAcqMtx acq(fMtx);
	return fEntries.size();
	}

void ZRoster::Wait(size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	while (fEntries.size() == iCount)
		fCnd.Wait(fMtx);
	}

bool ZRoster::WaitFor(double iTimeout, size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	if (fEntries.size() == iCount)
		fCnd.WaitFor(fMtx, iTimeout);
	return fEntries.size() != iCount;
	}

bool ZRoster::WaitUntil(ZTime iDeadline, size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	if (fEntries.size() == iCount)
		fCnd.WaitUntil(fMtx, iDeadline);
	return fEntries.size() != iCount;
	}

ZRef<ZCallable_Void> ZRoster::Get_Callable_Change()
	{
	ZAcqMtx acq(fMtx);
	return fCallable_Change;
	}

void ZRoster::Set_Callable_Change(const ZRef<ZCallable_Void>& iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCallable_Change = iCallable;
	}

bool ZRoster::CAS_Callable_Change(ZRef<ZCallable_Void> iPrior, ZRef<ZCallable_Void> iNew)
	{
	ZAcqMtx acq(fMtx);
	if (fCallable_Change != iPrior)
		return false;
	fCallable_Change = iNew;
	return true;
	}

ZRef<ZCallable_Void> ZRoster::Get_Callable_Gone()
	{
	ZAcqMtx acq(fMtx);
	return fCallable_Gone;
	}

void ZRoster::Set_Callable_Gone(const ZRef<ZCallable_Void>& iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCallable_Gone = iCallable;
	}

bool ZRoster::CAS_Callable_Gone(ZRef<ZCallable_Void> iPrior, ZRef<ZCallable_Void> iNew)
	{
	ZAcqMtx acq(fMtx);
	if (fCallable_Gone != iPrior)
		return false;
	fCallable_Gone = iNew;
	return true;
	}

void ZRoster::pFinalizeEntry(Entry* iEntry, const ZRef<ZCallable_Void>& iCallable_Gone)
	{
	ZGuardRMtx guard(fMtx);
	
	if (iEntry->FinishFinalize())
		{
		ZUtil_STL::sEraseMustContain(1, fEntries, iEntry);
		delete iEntry;
		fCnd.Broadcast();
		ZRef<ZCallable_Void> theCallable_Change = fCallable_Change;
		guard.Release();
		
		if (theCallable_Change)
			theCallable_Change->Call();

		if (iCallable_Gone)
			iCallable_Gone->Call();
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRoster::Entry

ZRoster::Entry::Entry
	(const ZRef<ZRoster>& iRoster,
	const ZRef<ZCallable_Void>& iCallable_Broadcast,
	const ZRef<ZCallable_Void>& iCallable_Gone)
:	fRoster(iRoster)
,	fCallable_Broadcast(iCallable_Broadcast)
,	fCallable_Gone(iCallable_Gone)
	{}

ZRoster::Entry::~Entry()
	{}

void ZRoster::Entry::Finalize()
	{	
	ZGuardRMtx guard(fMtx);
	ZRef<ZCallable_Void> theCallable = fCallable_Gone;

	if (ZRef<ZRoster> theRoster = fRoster)
		{
		guard.Release();
		theRoster->pFinalizeEntry(this, theCallable);
		}
	else if (this->FinishFinalize())
		{
		guard.Release();
		delete this;
		theCallable->Call();
		}
	}

ZRef<ZCallable_Void> ZRoster::Entry::Get_Callable_Broadcast()
	{
	ZAcqMtx acq(fMtx);
	return fCallable_Broadcast;
	}

void ZRoster::Entry::Set_Callable_Broadcast(const ZRef<ZCallable_Void>& iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCallable_Broadcast = iCallable;
	}

bool ZRoster::Entry::CAS_Callable_Broadcast(ZRef<ZCallable_Void> iPrior, ZRef<ZCallable_Void> iNew)
	{
	ZAcqMtx acq(fMtx);
	if (fCallable_Broadcast != iPrior)
		return false;
	fCallable_Broadcast = iNew;
	return true;
	}

ZRef<ZCallable_Void> ZRoster::Entry::Get_Callable_Gone()
	{
	ZAcqMtx acq(fMtx);
	return fCallable_Gone;
	}

void ZRoster::Entry::Set_Callable_Gone(const ZRef<ZCallable_Void>& iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCallable_Gone = iCallable;
	}

bool ZRoster::Entry::CAS_Callable_Gone(ZRef<ZCallable_Void> iPrior, ZRef<ZCallable_Void> iNew)
	{
	ZAcqMtx acq(fMtx);
	if (fCallable_Gone != iPrior)
		return false;
	fCallable_Gone = iNew;
	return true;
	}

} // namespace ZooLib
