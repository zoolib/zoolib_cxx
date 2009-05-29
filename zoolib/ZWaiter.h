/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZWaiter__
#define __ZWaiter__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZThreadImp.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZWeakRef.h"

NAMESPACE_ZOOLIB_BEGIN

class ZWaiter;

// =================================================================================================
#pragma mark -
#pragma mark * ZWaiterRunner

class ZWaiterRunner
:	public ZRefCountedWithFinalize,
	public ZWeakReferee
	{
public:
	ZWaiterRunner();
	virtual ~ZWaiterRunner();

protected:
// Called by subclasses
	void pAttachWaiter(ZRef<ZWaiter> iWaiter);
	void pDetachWaiter(ZRef<ZWaiter> iWaiter);

// Called by ZWaiter instances.
	virtual void WakeAt(ZRef<ZWaiter> iWaiter, ZTime iSystemTime) = 0;

	friend class ZWaiter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWaiterRunner_Threaded

class ZWaiterRunner_Threaded : public ZWaiterRunner
	{
public:
	ZWaiterRunner_Threaded(ZRef<ZWaiter> iWaiter);
	virtual ~ZWaiterRunner_Threaded();

	void Start();

// From ZWaiterRunner
	virtual void WakeAt(ZRef<ZWaiter> iWaiter, ZTime iSystemTime);

private:
	void pRun();
	static void spRun(ZRef<ZWaiterRunner_Threaded> iRunner);

	ZMtx fMtx;
	ZCnd fCnd;
	ZRef<ZWaiter> fWaiter;
	ZTime fNextWake;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWaiter

class ZWaiter : public ZRefCountedWithFinalize
	{
public:
	ZWaiter();

	virtual void RunnerAttached();
	virtual void RunnerDetached();

	virtual bool Execute() = 0;

	void Wake();
	void WakeAt(ZTime iSystemTime);

private:
	void pRunnerAttached();
	void pRunnerDetached();

	ZWeakRef<ZWaiterRunner> fRunner;
	friend class ZWaiterRunner;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

// Here for now till I find a better home.
void sStartWaiterRunner(ZRef<ZWaiter> iWaiter);

NAMESPACE_ZOOLIB_END

#endif // __ZWaiter__
