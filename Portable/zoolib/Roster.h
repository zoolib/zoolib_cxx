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

#ifndef __ZooLib_Roster_h__
#define __ZooLib_Roster_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Compat_NonCopyable.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Roster

class Roster
:	public ZCounted
,	NonCopyable
	{
public:
	class Entry;

	Roster();

	Roster(const ZRef<Callable_Void>& iCallable_Change,
		const ZRef<Callable_Void>& iCallable_Gone);

	virtual ~Roster();

// From ZCounted
	virtual void Finalize();

// Our protocol
	ZRef<Entry> MakeEntry();

	ZRef<Entry> MakeEntry(const ZRef<Callable_Void>& iCallable_Broadcast,
		const ZRef<Callable_Void>& iCallable_Gone);
	
	void Broadcast();

	size_t Count();
	void Wait(size_t iCount);
	bool WaitFor(double iTimeout, size_t iCount);
	bool WaitUntil(double iDeadline, size_t iCount);

private:
	void pFinalizeEntry(Entry* iEntry, const ZRef<Callable_Void>& iCallable_Gone);

	ZMtx fMtx;
	ZCnd fCnd;
	std::set<Entry*> fEntries;
	const ZRef<Callable_Void> fCallable_Change;
	const ZRef<Callable_Void> fCallable_Gone;

	friend class Entry;
	};

// =================================================================================================
#pragma mark -
#pragma mark Roster::Entry

class Roster::Entry
:	public ZCounted
,	NonCopyable
	{
private:
	Entry(
		const ZRef<Roster>& iRoster,
		const ZRef<Callable_Void>& iCallable_Broadcast,
		const ZRef<Callable_Void>& iCallable_Gone);

public:
	virtual ~Entry();

// From ZCounted
	virtual void Finalize();

private:
	ZWeakRef<Roster> fRoster;

	const ZRef<Callable_Void> fCallable_Broadcast;
	const ZRef<Callable_Void> fCallable_Gone;

	friend class Roster;
	};

} // namespace ZooLib

#endif // __ZooLib_Roster_h__
