/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZThreadOld__
#define __ZThreadOld__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThread.h"

// =================================================================================================
// A macro that has proven to be useful.
#define ZAssertLocked(a, b) ZAssertStop(a, (b).IsLocked())

// =================================================================================================
#pragma mark -
#pragma mark * ZThread

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexBase

// ZMutexBase is the base for ZMutex, ZMutexComposite and the read/write locks from ZRWLock.
// ie anything that can be acquired recursively and enforces some variety of mutual exclusion.

class ZMutexBase : NonCopyable
	{
protected:
	ZMutexBase() {}
	~ZMutexBase() {}

public:
	virtual void Acquire() = 0;
	virtual void Release() = 0;
	virtual bool IsLocked() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMutex

class ZMutex : public ZMutexBase
	{
public:
	ZMutex(const char* iName = nullptr, bool iCreateAcquired = false);
	~ZMutex();

// From ZMutexBase
	virtual void Acquire();
	virtual void Release();

	virtual bool IsLocked();

private:
	void pWait(ZCnd& iCnd);
	bool pWaitFor(ZCnd& iCnd, double iTimeout);
	bool pWaitUntil(ZCnd& iCnd, ZTime iDeadline);

	ZThread::ID fThreadID_Owner;
	ZMtx fMtx;
	int fCount;

	friend class ZCondition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCondition

class ZCondition : ZCnd
	{
public:
	ZCondition(const char* iName = nullptr) {}
	~ZCondition() {}

	void Wait(ZMutex& iMutex)
		{ iMutex.pWait(*this); }

	bool WaitFor(ZMutex& iMutex, double iTimeout)
		{ return iMutex.pWaitFor(*this, iTimeout); }

	bool WaitUntil(ZMutex& iMutex, ZTime iDeadline)
		{ return iMutex.pWaitUntil(*this, iDeadline); }

	using ZCnd::Wait;
	using ZCnd::WaitFor;
	using ZCnd::WaitUntil;
	using ZCnd::Signal;
	using ZCnd::Broadcast;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSemaphore

class ZSemaphore : protected ZSem
	{
public:
	ZSemaphore() {}
	ZSemaphore(int32 iInitialCount);
	ZSemaphore(int32 iInitialCount, const char* iName);
	~ZSemaphore();

	void Wait(int32 iCount);
	bool Wait(int32 iCount, int64 iMicroseconds);
	void Signal(int32 iCount);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexNR

typedef ZMtx ZMutexNR;

// =================================================================================================
#pragma mark -
#pragma mark * Guards (aka lockers)

typedef ZGuard_T<ZMutexBase> ZLocker;
typedef ZGuard_T<ZMutex> ZMutexLocker;
typedef ZAcqMtx ZMutexNRLocker;

// =================================================================================================

} // namespace ZooLib

#endif // __ZThreadOld__
