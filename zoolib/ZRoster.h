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

#ifndef __ZRoster__
#define __ZRoster__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZSafe.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZRoster

class ZRoster
:	public ZCounted
,	NonCopyable
	{
public:
	class Entry;

	ZRoster();
	virtual ~ZRoster();

// From ZCounted
	virtual void Finalize();

// Our protocol
	ZRef<Entry> MakeEntry();
	ZRef<Entry> MakeEntry(ZRef<ZCallable_Void> iCallable_Broadcast);
	
	void Broadcast();

	size_t Count();
	void Wait(size_t iCount);
	bool WaitFor(double iTimeout, size_t iCount);
	bool WaitUntil(ZTime iDeadline, size_t iCount);

	ZRef<ZCallable_Void> Get_Callable_Change();
	void Set_Callable_Change(const ZRef<ZCallable_Void>& iCallable);
	bool CAS_Callable_Change(ZRef<ZCallable_Void> iPrior, ZRef<ZCallable_Void> iNew);

	ZRef<ZCallable_Void> Get_Callable_Gone();
	void Set_Callable_Gone(const ZRef<ZCallable_Void>& iCallable);
	bool CAS_Callable_Gone(ZRef<ZCallable_Void> iPrior, ZRef<ZCallable_Void> iNew);

private:
	void pFinalizeEntry(Entry* iEntry, const ZRef<ZCallable_Void>& iCallable_Gone);

	ZMtx fMtx;
	ZCnd fCnd;
	std::set<Entry*> fEntries;
	ZRef<ZCallable_Void> fCallable_Change;
	ZRef<ZCallable_Void> fCallable_Gone;

	friend class Entry;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRoster::Entry

class ZRoster::Entry
:	public ZCounted
,	NonCopyable
	{
private:
	Entry
		(const ZRef<ZRoster>& iRoster,
		const ZRef<ZCallable_Void>& iCallable_Broadcast,
		const ZRef<ZCallable_Void>& iCallable_Gone);

public:
	virtual ~Entry();

// From ZCounted
	virtual void Finalize();

// Our protocol
	ZRef<ZCallable_Void> Get_Callable_Broadcast();
	void Set_Callable_Broadcast(const ZRef<ZCallable_Void>& iCallable);
	bool CAS_Callable_Broadcast(ZRef<ZCallable_Void> iPrior, ZRef<ZCallable_Void> iNew);

	ZRef<ZCallable_Void> Get_Callable_Gone();
	void Set_Callable_Gone(const ZRef<ZCallable_Void>& iCallable);
	bool CAS_Callable_Gone(ZRef<ZCallable_Void> iPrior, ZRef<ZCallable_Void> iNew);

private:
	ZWeakRef<ZRoster> fRoster;

	ZMtx fMtx;
	ZRef<ZCallable_Void> fCallable_Broadcast;
	ZRef<ZCallable_Void> fCallable_Gone;

	friend class ZRoster;
	};

} // namespace ZooLib

#endif // __ZYad__
