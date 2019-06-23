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

#include "zoolib/Roster.h"

#include "zoolib/Util_STL_set.h"

#include <vector>

namespace ZooLib {

using std::set;
using std::vector;

using namespace Util_STL;

// =================================================================================================
#pragma mark - Roster

Roster::Roster()
	{}

Roster::Roster(const ZP<Callable_Void>& iCallable_Change,
	const ZP<Callable_Void>& iCallable_Gone)
:	fCallable_Change(iCallable_Change)
,	fCallable_Gone(iCallable_Gone)
	{}

Roster::~Roster()
	{}

void Roster::Finalize()
	{
	ZP<Callable_Void> theCallable;

	{
	ZAcqMtx acq(fMtx);

	if (not	this->FinishFinalize())
		return;

	for (set<Entry*>::const_iterator ii = fEntries.begin(); ii != fEntries.end(); ++ii)
		(*ii)->fRoster.Clear();

	theCallable = fCallable_Gone;

	}

	delete this;

	sCall(theCallable);
	}

ZP<Roster::Entry> Roster::MakeEntry()
	{ return this->MakeEntry(null, null); }

ZP<Roster::Entry> Roster::MakeEntry(const ZP<Callable_Void>& iCallable_Broadcast,
	const ZP<Callable_Void>& iCallable_Gone)
	{
	ZP<Entry> theEntry = new Entry(this, iCallable_Broadcast, iCallable_Gone);

	{
	ZAcqMtx acq(fMtx);
	sInsertMust(fEntries, theEntry.Get());
	fCnd.Broadcast();
	}

	sCall(fCallable_Change);

	return theEntry;
	}

void Roster::Broadcast()
	{
	ZAcqMtx acq(fMtx);
	vector<ZP<Entry> > local(fEntries.begin(), fEntries.end());

	ZRelMtx rel(fMtx);

	for (vector<ZP<Entry> >::const_iterator ii = local.begin(); ii != local.end(); ++ii)
		sCall((*ii)->fCallable_Broadcast);
	}

size_t Roster::Count()
	{
	ZAcqMtx acq(fMtx);
	return fEntries.size();
	}

void Roster::Wait(size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	while (fEntries.size() == iCount)
		fCnd.Wait(fMtx);
	}

bool Roster::WaitFor(double iTimeout, size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	if (fEntries.size() == iCount)
		fCnd.WaitFor(fMtx, iTimeout);
	return fEntries.size() != iCount;
	}

bool Roster::WaitUntil(double iDeadline, size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	if (fEntries.size() == iCount)
		fCnd.WaitUntil(fMtx, iDeadline);
	return fEntries.size() != iCount;
	}

void Roster::pFinalizeEntry(Entry* iEntry, const ZP<Callable_Void>& iCallable_Gone)
	{
	{
	ZAcqMtx acq(fMtx);
	
	if (not iEntry->FinishFinalize())
		return;

	sEraseMust(fEntries, iEntry);
	delete iEntry;
	fCnd.Broadcast();
	}
	
	sCall(fCallable_Change);
	sCall(iCallable_Gone);
	}

// =================================================================================================
#pragma mark - Roster::Entry

Roster::Entry::Entry(
	const ZP<Roster>& iRoster,
	const ZP<Callable_Void>& iCallable_Broadcast,
	const ZP<Callable_Void>& iCallable_Gone)
:	fRoster(iRoster)
,	fCallable_Broadcast(iCallable_Broadcast)
,	fCallable_Gone(iCallable_Gone)
	{}

Roster::Entry::~Entry()
	{}

void Roster::Entry::Finalize()
	{
	ZP<Callable_Void> theCallable = fCallable_Gone;

	if (ZP<Roster> theRoster = fRoster)
		{
		theRoster->pFinalizeEntry(this, theCallable);
		}
	else if (this->FinishFinalize())
		{
		delete this;
		sCall(theCallable);
		}
	}

} // namespace ZooLib
