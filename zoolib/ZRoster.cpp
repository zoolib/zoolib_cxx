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

ZRoster::ZRoster(const ZRef<ZCallable_Void>& iCallable_Change,
	const ZRef<ZCallable_Void>& iCallable_Gone)
:	fCallable_Change(iCallable_Change)
,	fCallable_Gone(iCallable_Gone)
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
		sCall(theCallable);
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
	{ return this->MakeEntry(null, null); }

ZRef<ZRoster::Entry> ZRoster::MakeEntry
	(const ZRef<ZCallable_Void>& iCallable_Broadcast, const ZRef<ZCallable_Void>& iCallable_Gone)
	{
	ZRef<ZRoster> thisRef = this;

	ZGuardRMtx guard(fMtx);

	ZRef<Entry> theEntry = new Entry(thisRef, iCallable_Broadcast, null);
	ZUtil_STL::sInsertMustNotContain(fEntries, theEntry.Get());
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

void ZRoster::pFinalizeEntry(Entry* iEntry, const ZRef<ZCallable_Void>& iCallable_Gone)
	{
	ZGuardRMtx guard(fMtx);
	
	if (iEntry->FinishFinalize())
		{
		ZUtil_STL::sEraseMustContain(fEntries, iEntry);
		delete iEntry;
		fCnd.Broadcast();
		ZRef<ZCallable_Void> theCallable_Change = fCallable_Change;
		guard.Release();
		
		sCall(theCallable_Change);
		sCall(iCallable_Gone);
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
		sCall(theCallable);
		}
	}

} // namespace ZooLib
