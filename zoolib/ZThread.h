/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include <stdexcept>
#include <vector>

#include "ZAtomic.h"
#include "ZCompat_NonCopyable.h"
#include "ZDebug.h"
#include "ZTypes.h" // For bigtime_t

// =================================================================================================
#pragma mark -
#pragma mark * ZThread Configuration

// Do we want deadlock detection to be active?
#ifndef ZCONFIG_Thread_DeadlockDetect
#	if ZCONFIG_Debug >= 1
#		define ZCONFIG_Thread_DeadlockDetect 1
#	else
#		define ZCONFIG_Thread_DeadlockDetect 0
#	endif
#endif

// ==================================================
// Include appropriate headers for each platform, and set up ZCONFIG_Thread_Preemptive.
//
// ZCONFIG_Thread_Preemptive is 1 if threading is preemptive, that is if control can switch
// between one thread and another at *any* time. 

#if ZCONFIG(API_Thread, Mac)
#	include "ZThreadTM.h"
#	define ZCONFIG_Thread_Preemptive 0
#endif


#if ZCONFIG(API_Thread, Win32)
#	ifndef _MT
#		define _MT
#	endif
#	define ZCONFIG_Thread_Preemptive 1
	// Explicitly declare the Win32 types used in this header, so that
	// we don't pull ZWinHeader.h into every file that includes ZThread.h.
	typedef void *HANDLE;
	typedef unsigned long DWORD;
	typedef void *LPVOID;
#endif

#if ZCONFIG(API_Thread, POSIX)
#	include <pthread.h>
#	define ZCONFIG_Thread_Preemptive 1
#endif


#if ZCONFIG(API_Thread, Be)
#	include <OS.h>
#	include "TLS.h"
#	define ZCONFIG_Thread_Preemptive 1
#endif

// ==================================================
// A macro that has proven to be useful.
#define ZAssertLocked(a, b) ZAssertStop(a, (b).IsLocked())

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadSafe Macros
// "Safe" increment and decrement. In non-preemptive environments this will do cheap addition/subtraction,
// and in preemptive situations it will do slightly less efficient, but thread-safe operations. You need to use
// the ZThreadSafe_t type for variables that will manipulated by this. Right now it is a struct, but might
// degenerate into a simple integer at some point. ZThreadSafe_IncReturnNew and ZThreadSafe_DecReturnNew
// are used by windows COM objects' AddRef and Release methods.

typedef ZAtomic_t ZThreadSafe_t;

#if ZCONFIG_Thread_Preemptive

	#define ZThreadSafe_Get(x) ZAtomic_Get(&x)
	#define ZThreadSafe_Set(x, y) ZAtomic_Set(&x, y)
	#define ZThreadSafe_Swap(x, y) ZAtomic_Swap(&x, y)

	#define ZThreadSafe_Inc(x) ZAtomic_Inc(&x)
	#define ZThreadSafe_Dec(x) ZAtomic_Dec(&x)
	#define ZThreadSafe_DecAndTest(x) ZAtomic_DecAndTest(&x)

	#define ZThreadSafe_IncReturnNew(x) (ZAtomic_Add(&x, 1) + 1)
	#define ZThreadSafe_DecReturnNew(x) (ZAtomic_Add(&x, -1) - 1)

	#define ZThreadSafe_IncReturnOld(x) (ZAtomic_Add(&x, 1))
	#define ZThreadSafe_DecReturnOld(x) (ZAtomic_Add(&x, -1))

#else // ZCONFIG_Thread_Preemptive

	inline int ZThreadSafe_SwapHelper(ZThreadSafe_t& inX, int inY)
		{
		int oldValue = inX.fValue;
		inX.fValue = inY;
		return oldValue;
		}
	#define ZThreadSafe_Swap(x, y) ZThreadSafe_SwapHelper(x, y)

	#define ZThreadSafe_Get(x) (x.fValue)
	#define ZThreadSafe_Set(x, y) ((void)((x).fValue = y))

	#define ZThreadSafe_Inc(x) ((void)(++(x).fValue))
	#define ZThreadSafe_Dec(x) ((void)(--(x).fValue))
	#define ZThreadSafe_DecAndTest(x) ((--(x).fValue) == 0)

	#define ZThreadSafe_IncReturnNew(x) (++(x).fValue)
	#define ZThreadSafe_DecReturnNew(x) (--(x).fValue)

	#define ZThreadSafe_IncReturnOld(x) ((x).fValue++)
	#define ZThreadSafe_DecReturnOld(x) ((x).fValue--)

#endif // ZCONFIG_Thread_Preemptive

// =================================================================================================
#pragma mark -
#pragma mark * ZThread

class ZMutexBase;

class ZThread : ZooLib::NonCopyable
	{
protected:
	// This protected constructor is used indirectly by ZThread::InitHelper.
	ZThread(struct Dummy*);

	// Our destructor is protected, so even with its pointer semantics it's illegal to
	// simply delete a thread -- it must return from its Run method, at which point it is deleted.
	virtual ~ZThread();

public:
	enum Error { errorNone, errorDisposed, errorTimeout };
	class Ex_Disposed;

	// Appropriate per-platform definitions of thread ID, TLS key and TLS data
	#if ZCONFIG(API_Thread, Mac)
		typedef uint32 ThreadID;
		typedef struct TLSKey_t_Opaque* TLSKey_t;
		typedef struct TLSData_t_Opaque* TLSData_t;
	#elif ZCONFIG(API_Thread, Win32)
		typedef DWORD ThreadID;
		typedef DWORD TLSKey_t;
		typedef LPVOID TLSData_t;
	#elif ZCONFIG(API_Thread, POSIX)
		typedef pthread_t ThreadID;
		typedef pthread_key_t TLSKey_t;
		typedef void* TLSData_t;
	#elif ZCONFIG(API_Thread, Be)
		typedef thread_id ThreadID;
		typedef int TLSKey_t;
		typedef tls_data_t TLSData_t;
	#endif // ZCONFIG(API_Thread)

	ZThread(const char* iName = nil);
	void Start();
	virtual void Run() = 0;

	// Identity
	static ZThread::ThreadID sCurrentID();

	// Sleep
	static void sSleepMicro(bigtime_t iMicroseconds);
	static void sSleep(int32 iMilliseconds);

	// Process times
	static void sGetProcessTimes(bigtime_t& oRealTime, bigtime_t& oCumulativeRunTime);

	// Thread local storage (aka thread-specific data)
	static TLSKey_t sTLSAllocate();
	static void sTLSFree(TLSKey_t iTLSKey);
	static void sTLSSet(TLSKey_t iTLSKey, TLSData_t iValue);
	static TLSData_t sTLSGet(TLSKey_t iTLSKey);

	// InitHelper's role is explained in its definition below
	class InitHelper;
	friend class InitHelper;

	// Deadlock detection.
	typedef void (*DeadlockHandler_t)(const std::string& iString);
	static DeadlockHandler_t sSetDeadlockHandler(DeadlockHandler_t iHandler);
	#if ZCONFIG_Thread_DeadlockDetect
		std::string* CheckForDeadlock(ZThread* iAcquiringThread);
	#endif

protected:
	// State and informational variables
	ThreadID fThreadID;
	bool fStarted;
	const char* fName;

	#if ZCONFIG(API_Thread, Mac)
		ZThreadTM_State* fTMState;
	#elif ZCONFIG(API_Thread, Win32)
	public:
		HANDLE fThreadHANDLE;
	protected:
	#elif ZCONFIG(API_Thread, POSIX)
		pthread_mutex_t fMutex_Start;
		pthread_cond_t fCond_Start;
	#endif // ZCONFIG(API_Thread)


	#if ZCONFIG_Thread_DeadlockDetect
		ZMutexBase* fMutexBase_Wait;
		friend class ZMutexBase;
	#endif

private:
	#if ZCONFIG(API_Thread, Mac)
		static void sThreadEntry_MacTM(void* iArg);
	#elif ZCONFIG(API_Thread, Win32)
		static unsigned int __stdcall sThreadEntry_Win32(void* iArg);
	#elif ZCONFIG(API_Thread, POSIX)
		static void* sThreadEntry_POSIX(void* iArg);
	#elif ZCONFIG(API_Thread, Be)
		static int32 sThreadEntry_Be(void* iArg);
	#endif // ZCONFIG(API_Thread)
	};

// ==================================================

#if ZCONFIG(API_Thread, Win32)
//##ZThread::ThreadID ZThread::sCurrentID();

#else

inline ZThread::ThreadID ZThread::sCurrentID()
	{
#if ZCONFIG(API_Thread, Mac)
	::ThreadID currentID;
	::GetCurrentThread(&currentID);
	return (ZThread::ThreadID)currentID;


#elif ZCONFIG(API_Thread, POSIX)
	return ::pthread_self();

#elif ZCONFIG(API_Thread, Be)
	return ::find_thread(nil);

#endif // ZCONFIG(API_Thread)
	}

#endif // ZCONFIG(API_Thread, Win32)

// ==================================================

#if ZCONFIG(API_Thread, Win32)
//##ZThread::TLSData_t ZThread::sTLSGet(ZThread::TLSKey_t iTLSKey);

#else
inline ZThread::TLSData_t ZThread::sTLSGet(ZThread::TLSKey_t iTLSKey)
	{
#if ZCONFIG(API_Thread, Mac)
	return reinterpret_cast<TLSData_t>(ZThreadTM_TLSGet(reinterpret_cast<ZThreadTM_TLSKey_t>(iTLSKey)));

#elif ZCONFIG(API_Thread, POSIX)
	return ::pthread_getspecific(iTLSKey);

#elif ZCONFIG(API_Thread, Be)
	tls_data_t theTLSData;
	::tls_get(iTLSKey, &theTLSData);
	return theTLSData;

#endif // ZCONFIG(API_Thread)
	}

#endif // ZCONFIG(API_Thread, Win32)

// =================================================================================================
#pragma mark -
#pragma mark * ZThread::InitHelper

class ZThread::InitHelper
	{
public:
	InitHelper();
	~InitHelper();
	};

// This static is what makes auto-initialization work. Every file that (transitively) includes ZThread.h will
// get their own instance of a ZThread::InitHelper, whose constructor creates the main thread object and
// whose destructor deletes it. This ensures that ZThread is initialized at static construction time
// and is usable regardless of the link order. Unfortunately this chokes with the BeOS compiler, but ZThread
// will still be initialized before main() is invoked.

#if !ZCONFIG(OS, Be)
static ZThread::InitHelper sZThread_InitHelper;
#endif // !ZCONFIG(OS, Be)

// =================================================================================================
#pragma mark -
#pragma mark * ZThread::Ex_Disposed

class ZThread::Ex_Disposed : public std::runtime_error
	{
public:
	Ex_Disposed();
	virtual ~Ex_Disposed() throw();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSemaphore

// ZSemaphore is available for use by applications and provides the foundation for all the other
// synchronization facilities. ZFiber and BeOS provide exactly the behavior we want, but in the case
// of BeOS the system-wide limit on semaphores precludes their profligate use, which can be problematic.
// A call to Wait specifies a count and a timeout (the version without a timeout parameter has an effectively
// infinite timeout). Wait will return one of three results: ZThread::errorNone if all the signals were
// satisfied within the timeout, ZThread::errorTimeout if not all the signals could be satisified
// within the timeout and ZThread::errorDisposed if the semaphore was destroyed *whilst* Wait was blocked.
// If code is calling Wait whilst the destructor is being called the behavior is not well-defined -- any
// Waits have to be already blocked.

class ZSemaphore : ZooLib::NonCopyable
	{
public:
	ZSemaphore(int32 iInitialCount = 0, const char* iName = nil);
	~ZSemaphore();

	ZThread::Error Wait(int32 iCount);
	ZThread::Error Wait(int32 iCount, bigtime_t iMicroseconds);
	void Signal(int32 iCount);

private:
	const char* fName;

	#if ZCONFIG(API_Thread, Mac)

		ZThreadTM_Sem fSem_TM;

	#elif ZCONFIG(API_Thread, Win32)

		ZThread::Error Internal_Wait_Win32(int32 iCount, bigtime_t iMicroseconds);
		void Internal_Signal_Win32(int32 iCount);

		struct Waiter_Win32;
		ZAtomic_t fSpinlock;
		HANDLE fSemaphoreHANDLE;
		Waiter_Win32* fWaiter_Head;
		Waiter_Win32* fWaiter_Tail;
		int32 fAvailable;

	#elif ZCONFIG(API_Thread, POSIX)

		ZThread::Error Internal_Wait_POSIX(int32 iCount, bigtime_t iMicroseconds);
		void Internal_Signal_POSIX(int32 iCount);

		struct Waiter_POSIX;
		Waiter_POSIX* fWaiter_Head;
		Waiter_POSIX* fWaiter_Tail;
		int32 fAvailable;
		bool fDisposed;
		pthread_mutex_t fMutex;
		pthread_cond_t fCond;

	#elif ZCONFIG(API_Thread, Be)

		sem_id fSem;

	#endif // ZCONFIG(API_Thread)
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexNR

class ZMutexNR : ZooLib::NonCopyable
	{
public:
	ZMutexNR(const char* iName = nil);
	~ZMutexNR();

	ZThread::Error Acquire();
	void Release();

private:
	ZSemaphore fSem;
	ZThreadSafe_t fLock;
	friend class ZCondition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexBase

// ZMutexBase is the base for ZMutex, ZMutexComposite and the read/write locks from ZRWLock.
// ie anything that can be acquired recursively and enforces some variety of mutual exclusion.

class ZMutexBase : ZooLib::NonCopyable
	{
protected:
	ZMutexBase(const char* iName = nil);
	~ZMutexBase();

public:
	// Accessors that return an error if there is a problem
	virtual ZThread::Error Acquire() = 0;
	virtual ZThread::Error Acquire(bigtime_t iMicroseconds) = 0;
	virtual void Release() = 0;

	virtual bool IsLocked() = 0;

	// Convenience accessor that throws an exception if there is a problem
	void AcquireOrThrow();

	// For debugging. -ec 01.02.23
	void SetName(const char* iName) { fName = iName; }

	#if ZCONFIG_Thread_DeadlockDetect
		void CheckForDeadlock_Pre(ZThread* iAcquiringThread);
		void CheckForDeadlock_Post(ZThread* iAcquiringThread);
		virtual std::string* CheckForDeadlockImp(ZThread* iAcquiringThread) = 0;
		std::string* CheckDeadlockThread(ZThread* iAcquiringThread, ZThread* iOwnerThread);
		void ReleaseForDeadlock_Pre();
		void ReleaseForDeadlock_Post();
		friend class ZThread;
	#endif

protected:
	const char* fName;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMutex

class ZMutex : public ZMutexBase
	{
public:
	ZMutex(const char* iName = nil, bool iCreateAcquired = false);
	~ZMutex();

	virtual ZThread::Error Acquire();
	virtual ZThread::Error Acquire(bigtime_t iMicroseconds);
	virtual void Release();

	virtual bool IsLocked();

	// These methods are available so we can bypass the virtual
	// method dispatch overhead of Acquire/Release
	ZThread::Error MutexAcquire(bigtime_t iMicroseconds);
	void MutexRelease();

	// FullRelease and FullAcquire are required for certain *very rare*
	// circumstances. Do not abuse them.
	ZThread::Error FullAcquire(int32 iCount);
	int32 FullRelease();

private:
	ZThread::Error Internal_Acquire(bigtime_t iMicroseconds);
	void Internal_Release();

	ZThread::ThreadID fThreadID_Owner;
	ZSemaphore fSem;
	int32 fCount;

	friend class ZCondition;

	#if ZCONFIG_Thread_DeadlockDetect
		ZThread* fThread_Owner;
		virtual std::string* CheckForDeadlockImp(ZThread* iAcquiringThread);
	#endif // ZCONFIG_Thread_DeadlockDetect
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCondition

class ZCondition : ZooLib::NonCopyable
	{
public:
	ZCondition(const char* iName = nil);
	~ZCondition();

	ZThread::Error Wait(ZMutex& iMutex);
	ZThread::Error Wait(ZMutex& iMutex, bigtime_t iMicroseconds);
	ZThread::Error Wait(ZMutexNR& iMutex);
	ZThread::Error Wait(ZMutexNR& iMutex, bigtime_t iMicroseconds);
	void Signal();
	void Broadcast();

private:
	ZSemaphore fSem_Wait;
	ZThreadSafe_t fWaitingThreads;
	const char* fName;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRWLock

class ZRWLock : ZooLib::NonCopyable
	{
public:
	ZRWLock(const char* iName = nil);
	~ZRWLock();

	ZMutexBase& GetRead() { return fRead; }
	ZMutexBase& GetWrite() { return fWrite; }

	bool CanRead();
	bool CanWrite();

	void SetName(const char* iName);

	void AcquireRead();
	void ReleaseRead();

	void AcquireWrite();
	void ReleaseWrite();

	void DemoteWriteToRead();

protected:
	ZThread::Error AcquireWrite(bigtime_t iMicroseconds);
	ZThread::Error AcquireRead(bigtime_t iMicroseconds);

	void ReturnLock();

private:
	// Mutex to provide mutual exclusion while fiddling with internal data
	ZMutexNR fMutex_Structure;

	// Used to queue up a reader and multiple writers.
	ZSemaphore fSem_Access;

	// Used to block subsequent readers
	ZSemaphore fSem_SubsequentReaders;

	// Writers waiting for the lock
	int32 fCount_WaitingWriters;

	// Writers holding the lock (the same writer recursively, of course)
	int32 fCount_CurrentWriter;
	ZThread::ThreadID fThreadID_CurrentWriter;

	// Readers waiting for the lock
	int32 fCount_WaitingReaders;

	// Readers holding the lock (to allow recursive calls to AcquireRead)
	int32 fCount_CurrentReaders;
	std::vector<ZThread::ThreadID> fVector_CurrentReaders;

	class Read : public ZMutexBase
		{
	public:
		Read(ZRWLock* iRWLock, const char* iName) : ZMutexBase(iName), fRWLock(iRWLock) {}
		~Read() {}
	
		virtual ZThread::Error Acquire();
		virtual ZThread::Error Acquire(bigtime_t iMicroseconds);
		virtual void Release();
	
		virtual bool IsLocked();
	
		#if ZCONFIG_Thread_DeadlockDetect
			virtual std::string* CheckForDeadlockImp(ZThread* iAcquiringThread);
		#endif
	
	private:
		ZRWLock* fRWLock;
		};
	
	Read fRead;
	friend class Read;

public:
	class Write : public ZMutexBase
		{
	public:
		Write(ZRWLock* iRWLock, const char* iName) : ZMutexBase(iName), fRWLock(iRWLock) {}
		~Write() {}
	
		virtual ZThread::Error Acquire();
		virtual ZThread::Error Acquire(bigtime_t iMicroseconds);
		virtual void Release();
	
		virtual bool IsLocked();
	
		#if ZCONFIG_Thread_DeadlockDetect
			virtual std::string* CheckForDeadlockImp(ZThread* iAcquiringThread);
		#endif
	
		ZRWLock& GetRWLock() { return *fRWLock; }
	private:
		ZRWLock* fRWLock;
		};
private:

	Write fWrite;
	friend class Write;

	#if ZCONFIG_Thread_DeadlockDetect
		ZThread* fThread_CurrentWriter;
		std::vector<ZThread*> fVector_Thread_CurrentReaders;
		std::string* Internal_CheckForDeadlock(ZThread* iAcquiringThread, ZMutexBase* iLock);
	#endif
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexComposite

class ZMutexComposite : public ZMutexBase
	{
public:
	ZMutexComposite();
	virtual ~ZMutexComposite();

	void Add(ZMutexBase& iLock);

	virtual ZThread::Error Acquire();
	virtual ZThread::Error Acquire(bigtime_t iMicroseconds);
	virtual void Release();

	virtual bool IsLocked();

	#if ZCONFIG_Thread_DeadlockDetect
	// From ZMutexBase
	virtual std::string* CheckForDeadlockImp(ZThread* iAcquiringThread);
	#endif

private:
	std::vector<ZMutexBase*> fLocks;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZLocker

class ZLocker : ZooLib::NonCopyable
	{
public:
	ZLocker(const ZMutexBase& iMutex, bool iThrowIfProblem = true);
	~ZLocker();

	// Used to temporarily release, then (usually) reacquire the lock
	ZThread::Error Acquire();
	void Release();

	bool IsOkay() { return fOkay; }

private:
	ZMutexBase& fMutexBase;
	int32 fAcquisitions;
	bool fOkay;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexLocker

class ZMutexLocker : ZooLib::NonCopyable
	{
public:
	ZMutexLocker(const ZMutex& iMutex, bool iThrowIfProblem = true);
	~ZMutexLocker();

	// Used to temporarily release, then (usually) reacquire the lock
	ZThread::Error Acquire();
	void Release();

	bool IsOkay() { return fOkay; }

private:
	ZMutex& fMutex;
	int32 fAcquisitions;
	bool fOkay;
	};

// =================================================================================================

#endif // __ZThread__
