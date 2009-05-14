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

#ifndef __ZThread__
#define __ZThread__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThreadImp.h"
#include "zoolib/ZThreadSafe.h"
#include "zoolib/ZTypes.h" // For bigtime_t and nil

// =================================================================================================
// A macro that has proven to be useful.
#define ZAssertLocked(a, b) ZAssertStop(a, (b).IsLocked())

// =================================================================================================
#pragma mark -
#pragma mark * ZThread

NAMESPACE_ZOOLIB_BEGIN

class ZThread : NonCopyable
	{
protected:
	virtual ~ZThread() {}

public:
	typedef bool Error;
	static const Error errorNone = true;
	static const Error errorTimeout = false;

	typedef ZThreadImp::ID ThreadID;
	static ThreadID kThreadID_None;

	typedef ZTSS::Key TLSKey_t;
	typedef ZTSS::Value TLSData_t;

	ZThread(const char* iName = nullptr);

	void Start();
	virtual void Run() = 0;

	static ZThread::ThreadID sCurrentID() { return ZThreadImp::sID(); }

	static void sSleepMicro(bigtime_t iMicroseconds) { ZThreadImp::sSleep(iMicroseconds / 1e6); }
	static void sSleep(int32 iMilliseconds) { ZThreadImp::sSleep(iMilliseconds / 1e3); }

	static TLSKey_t sTLSAllocate() { return ZTSS::sCreate(); }
	static void sTLSFree(TLSKey_t iKey) { ZTSS::sFree(iKey); }
	static void sTLSSet(TLSKey_t iKey, TLSData_t iValue) { ZTSS::sSet(iKey, iValue); }
	static TLSData_t sTLSGet(TLSKey_t iKey) { return ZTSS::sGet(iKey); }

protected:
	void pRun();

	#if ZCONFIG_API_Enabled(ThreadImp_Win)
		static ZThreadImp::ProcResult_t __stdcall spRun(ZThreadImp::ProcParam_t iParam);
	#else
		static ZThreadImp::ProcResult_t spRun(ZThreadImp::ProcParam_t iParam);
	#endif

	ZMtx* fMtx_Start;
	ZCnd* fCnd_Start;
	bool fStarted;
	};

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
	void pWait(ZCnd& iCnd, double iTimeout);

	ZThread::ThreadID fThreadID_Owner;
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

	void Wait(ZMutex& iMutex, bigtime_t iMicroseconds)
		{ iMutex.pWait(*this, iMicroseconds / 1e6); }

	void Wait(ZMtx& iMtx)
		{ ZCnd::Wait(iMtx); }

	void Wait(ZMtx& iMtx, bigtime_t iMicroseconds)
		{ ZCnd::Wait(iMtx, iMicroseconds / 1e6); }

	void Signal()
		{ ZCnd::Signal(); }

	void Broadcast()
		{ ZCnd::Broadcast(); }
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
	bool Wait(int32 iCount, bigtime_t iMicroseconds);
	void Signal(int32 iCount);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexNR

typedef ZMtx ZMutexNR;

// =================================================================================================
#pragma mark -
#pragma mark * Guards (aka lockers)

typedef ZGuardR_T<ZMutexBase> ZLocker;
typedef ZGuardR_T<ZMutex> ZMutexLocker;
typedef ZGuardMtx ZMutexNRLocker;

// =================================================================================================

NAMESPACE_ZOOLIB_END

#endif // __ZThread__
