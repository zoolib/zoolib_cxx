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

#include "zoolib/ZThread.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZCompat_algorithm.h" // for find, lower_bound, swap
#include "zoolib/ZTime.h"


#if ZCONFIG_Thread_DeadlockDetect
#	include "zoolib/ZString.h" // For ZString::sFormat
#endif

#include <string> // because runtime_error relies on it

#if ZCONFIG(API_Thread, Win32)
#	include "zoolib/ZWinHeader.h"
#	include <process.h>
#endif

#if ZCONFIG(API_Thread, POSIX)
#	include <errno.h>
#endif

using std::bad_alloc;
using std::exception;
using std::min;
using std::vector;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * kDebug

#define kDebug_Thread 1

// =================================================================================================
#pragma mark -
#pragma mark * ZThread static data

static ZThread* sMainThread;

static const ZThread::ThreadID kThreadID_None = 0;

#if ZCONFIG(API_Thread, POSIX)
#	include <unistd.h> // For usleep
#	include <sys/time.h> // For timeval
#endif 

#if ZCONFIG(Compiler, MSVC)
#	define WAITFOREVER 0x7FFFFFFFFFFFFFFFi64
#else
#	define WAITFOREVER 0x7FFFFFFFFFFFFFFFLL
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Current ZThread

// Used when deadlock detection is active, and in some situations on windows.

static ZThread::TLSKey_t sKeyCurrentThread;

static ZThread* sCurrent()
	{ return reinterpret_cast<ZThread*>(ZThread::sTLSGet(sKeyCurrentThread)); }

// =================================================================================================
#pragma mark -
#pragma mark * Win32 utility functions

#if ZCONFIG_SPI_Enabled(Win)
	#if ZCONFIG(Compiler, CodeWarrior)

		#include <ThreadLocalData.h>

		static HANDLE sCurrentThreadHANDLE()
			{
			if (_ThreadLocalData* theData = _GetThreadLocalData(false))
				return theData->thread_handle;
			return 0;
			}

		static inline void sReleaseCurrentThreadHANDLE(HANDLE iHANDLE)
			{}

	#elif ZCONFIG(Compiler, MSVC)

		struct _tiddata
			{
			unsigned long   _tid;       /* thread ID */
			unsigned long   _thandle;   /* thread handle */
		 	// Extra stuff follows, but is not needed by our code.
		 	};

		extern "C" _tiddata* __cdecl _getptd(); // return's the address of per-thread CRT data.

		static HANDLE sCurrentThreadHANDLE()
			{
			if (_tiddata* theData = _getptd())
				return (HANDLE)(theData->_thandle);
			return 0;
			}

		static inline void sReleaseCurrentThreadHANDLE(HANDLE iHANDLE)
			{}

	#elif ZCONFIG(Compiler, GCC)

		static HANDLE sCurrentThreadHANDLE()
			{
			if (ZThread* currentThread = sCurrent())
				return currentThread->fThreadHANDLE;

			HANDLE theHANDLE;
			::DuplicateHandle(::GetCurrentProcess(), ::GetCurrentThread(), 
				::GetCurrentProcess(), &theHANDLE,
				0, true, DUPLICATE_SAME_ACCESS);
			return theHANDLE;
			}

		static void sReleaseCurrentThreadHANDLE(HANDLE iHANDLE)
			{
			if (!sCurrent())
				::CloseHandle(iHANDLE);			
			}
	#else

		#error Unsupported compiler/runtime

	#endif
#endif

// =================================================================================================
#pragma mark -
#pragma mark * General utility functions

static void sAcquireSpinlock(ZAtomic_t* iSpinlock)
	{
	while (ZAtomic_Swap(iSpinlock, 1) == 1)
		ZThread::sSleep(0);
	}

static void sReleaseSpinlock(ZAtomic_t* iSpinlock)
	{
	int priorValue = ZAtomic_Swap(iSpinlock, 0);
	ZAssertStop(kDebug_Thread, priorValue == 1);
	}

// ==================================================
#pragma mark -
#pragma mark * ZThread ctor/dtor

// Constructor called only by MainThread at initialization time.

ZThread::ZThread(struct Dummy*)
	{
	ZAssertStop(kDebug_Thread, sMainThread == nil);

	#if ZCONFIG_Thread_DeadlockDetect
	fMutexBase_Wait = nil;
	#endif

#if ZCONFIG(API_Thread, Be)

	::tls_init(TLS_VERSION);

#endif // ZCONFIG_API_Thread

	// Rememeber our thread ID
	fThreadID = sCurrentID();

	// Allocate the current thread key
	sKeyCurrentThread = sTLSAllocate();

	// And remember our current thread object
	sTLSSet(sKeyCurrentThread, reinterpret_cast<TLSData_t>(this));

	sMainThread = this;

	fStarted = true;

	fName = "ZThread::sMainThread";
	}

ZThread::ZThread(const char* iName)
	{
	if (ZCONFIG_Thread_DeadlockDetect || ZCONFIG(API_Thread, Win32))
		ZAssertStop(kDebug_Thread, sMainThread);

	fThreadID = kThreadID_None;
	fStarted = false;
	fName = iName;

	#if ZCONFIG_Thread_DeadlockDetect
	fMutexBase_Wait = nil;
	#endif // ZCONFIG_Thread_DeadlockDetect

#if 0
#elif ZCONFIG(API_Thread, Win32)

	fThreadHANDLE = (HANDLE) ::_beginthreadex(nil, 0, sThreadEntry_Win32,
						this, fStarted ? 0 : CREATE_SUSPENDED, reinterpret_cast<unsigned int*>(&fThreadID));
	if (!fThreadHANDLE)
		throw bad_alloc();

#elif ZCONFIG(API_Thread, POSIX)

	// Because there's no standard way to create a suspended thread we allocate a mutex/condlock combo, which
	// will get fired by our Start method. If we used a mutex, then only the thread that creates this thread
	// would be able to release it to allow this thread to start executing.
	::pthread_mutex_init(&fMutex_Start, nil);
	::pthread_cond_init(&fCond_Start, nil);

	pthread_attr_t threadAttr;
	::pthread_attr_init(&threadAttr);
	size_t defaultSize;
	::pthread_attr_getstacksize(&threadAttr, &defaultSize);
	::pthread_attr_setstacksize(&threadAttr, min(defaultSize, size_t(1024*1024)));
	::pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
	int result = ::pthread_create(&fThreadID, &threadAttr, sThreadEntry_POSIX, (void*)this);
	::pthread_attr_destroy(&threadAttr);

	if (result != 0)
		{
		::pthread_mutex_destroy(&fMutex_Start);
		::pthread_cond_destroy(&fCond_Start);
		throw bad_alloc();
		}

#elif ZCONFIG(API_Thread, Be)

	fThreadID = ::spawn_thread(sThreadEntry_Be, fName ? fName : "", B_NORMAL_PRIORITY, (void*)this);
	if (!fThreadID)
		throw bad_alloc();

	if (fStarted)
		::resume_thread(fThreadID);

#endif // ZCONFIG_API_Thread
	}

ZThread::~ZThread()
	{
	if (sMainThread == this)
		{
		sMainThread = nil;
		sTLSFree(sKeyCurrentThread);

#if 0
#elif ZCONFIG(API_Thread, Be)

		::tls_term();

#endif // ZCONFIG(API_Thread)
		}
	else
		{
		// Back to asserting that the main thread is not nil -- it's just a design flaw
		// that the main thread exit before any others.
		if (ZCONFIG_Thread_DeadlockDetect || ZCONFIG(API_Thread, Win32))
			ZAssertStop(kDebug_Thread, sMainThread != nil);
		}

#if ZCONFIG(API_Thread, Win32)

	// AG 2002-05-02. After examining MetroWerks' and Microsoft's runtime library sources
	// it's clear that we have to close the thread handle iff we create the thread using beginthreadex.
	if (HANDLE theThreadHANDLE = sCurrentThreadHANDLE())
		::CloseHandle(theThreadHANDLE);

#endif // ZCONFIG(API_Thread)
	}

// ==================================================
#pragma mark -
#pragma mark * ZThread other facilities

void ZThread::Start()
	{
	ZAssertStop(kDebug_Thread, !fStarted);

#if 0

#elif ZCONFIG(API_Thread, Win32)

	fStarted = true;
	::ResumeThread(fThreadHANDLE);

#elif ZCONFIG(API_Thread, POSIX)

	::pthread_mutex_lock(&fMutex_Start);
	fStarted = true;
	::pthread_cond_signal(&fCond_Start);
	::pthread_mutex_unlock(&fMutex_Start);

#elif ZCONFIG(API_Thread, Be)

	fStarted = true;
	::resume_thread(fThreadID);

#endif // ZCONFIG_API_Thread
	}

void ZThread::sSleepMicro(bigtime_t iMicroseconds)
	{
#if 0

#elif ZCONFIG(API_Thread, Win32)

	while (iMicroseconds > 0)
		{
		int32 currentDelay = 0x40000000;
		if (currentDelay > iMicroseconds)
			currentDelay = iMicroseconds;
		iMicroseconds -= currentDelay;
		::Sleep(currentDelay / 1000);
		}

#elif ZCONFIG(API_Thread, POSIX)

	while (iMicroseconds > 0)
		{
		int32 currentDelay = 0x40000000;
		if (currentDelay > iMicroseconds)
			currentDelay = iMicroseconds;
		iMicroseconds -= currentDelay;
		::usleep(currentDelay);
		}

#elif ZCONFIG(API_Thread, Be)

	::snooze(iMicroseconds);

#endif
	}

void ZThread::sSleep(int32 iMilliseconds)
	{
#if 0

#elif ZCONFIG(API_Thread, Win32)

	::Sleep(iMilliseconds);

#elif ZCONFIG(API_Thread, POSIX)

	while (iMilliseconds > 0)
		{
		int32 currentDelay = 0x00200000;
		if (currentDelay > iMilliseconds)
			currentDelay = iMilliseconds;
		iMilliseconds -= currentDelay;
		::usleep(currentDelay * 1000);
		}

#elif ZCONFIG(API_Thread, Be)

	::snooze(bigtime_t(iMilliseconds) * 1000LL);

#endif
	}

#if !ZCONFIG_Thread_DeadlockDetect
ZThread::DeadlockHandler_t ZThread::sSetDeadlockHandler(DeadlockHandler_t iHandler)
	{
	return nil;
	}
#endif // !ZCONFIG_Thread_DeadlockDetect

#if ZCONFIG_Thread_DeadlockDetect
static ZThread::DeadlockHandler_t sDeadlockHandler;

ZThread::DeadlockHandler_t ZThread::sSetDeadlockHandler(DeadlockHandler_t iHandler)
	{
	swap(sDeadlockHandler, iHandler);
	return iHandler;
	}

string* ZThread::CheckForDeadlock(ZThread* iAcquiringThread)
	{
	if (this == iAcquiringThread)	
		{
		// We've encountered the acquiring thread, so we're deadlocked.
		return new string;
		}
	else if (fMutexBase_Wait)
		{
		return fMutexBase_Wait->CheckForDeadlockImp(iAcquiringThread);
		}
	else
		{
		return nil;
		}
	}

#endif // ZCONFIG_Thread_DeadlockDetect

// ==================================================
#pragma mark -
#pragma mark * Process times

void ZThread::sGetProcessTimes(bigtime_t& oRealTime, bigtime_t& oCumulativeRunTime)
	{
#if 0

#elif ZCONFIG(API_Thread, Win32)

	FILETIME creationTime, exitTime, kernelTime, userTime;
	if (::GetProcessTimes(::GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime))
		{
		FILETIME timeNow;
		::GetSystemTimeAsFileTime(&timeNow);
		oRealTime = (*reinterpret_cast<int64*>(&timeNow)) / 10;
		oCumulativeRunTime = (*reinterpret_cast<int64*>(&kernelTime) + *reinterpret_cast<int64*>(&userTime)) / 10;
		}
	else
		{
		oRealTime = 0;
		oCumulativeRunTime = 0;
		}

#elif ZCONFIG(API_Thread, POSIX)

	oRealTime = 0;
	oCumulativeRunTime = 0;

#elif ZCONFIG(API_Thread, Be)

	oCumulativeRunTime = 0;
	thread_info theThreadInfo;
	int32 cookie = 0;
	while (::get_next_thread_info(0, &cookie, &theThreadInfo) == B_OK)
		oCumulativeRunTime += (theThreadInfo.user_time + theThreadInfo.kernel_time);
	oRealTime = ::system_time();

#endif
	}

// ==================================================
#pragma mark -
#pragma mark * ZThread::sCurrentID, Windows

#if ZCONFIG(API_Thread, Win32)

ZThread::ThreadID ZThread::sCurrentID()
	{ return ::GetCurrentThreadId(); }

#endif

// ==================================================
#pragma mark -
#pragma mark * Thread local storage

ZThread::TLSKey_t ZThread::sTLSAllocate()
	{
#if 0

#elif ZCONFIG(API_Thread, Win32)

	return ::TlsAlloc();

#elif ZCONFIG(API_Thread, POSIX)

	pthread_key_t newKey;
	::pthread_key_create(&newKey, nil);
	return newKey;

#elif ZCONFIG(API_Thread, Be)

	TLSKey_t newKey;
	status_t result = ::tls_alloc(&newKey);
	ZAssert(result == B_NO_ERROR);
	return newKey;

#endif // API_Thread
	}

void ZThread::sTLSFree(TLSKey_t iTLSKey)
	{
#if 0

#elif ZCONFIG(API_Thread, Win32)

	::TlsFree(iTLSKey);

#elif ZCONFIG(API_Thread, POSIX)

	::pthread_key_delete(iTLSKey);

#elif ZCONFIG(API_Thread, Be)

	::tls_free(iTLSKey);

#endif // API_Thread
	}

#if ZCONFIG(API_Thread, Win32)
ZThread::TLSData_t ZThread::sTLSGet(ZThread::TLSKey_t iTLSKey)
	{ return ::TlsGetValue(iTLSKey); }
#endif // ZCONFIG(API_Thread, Win32)


void ZThread::sTLSSet(TLSKey_t iKey, TLSData_t iValue)
	{
#if 0

#elif ZCONFIG(API_Thread, Win32)

	::TlsSetValue(iKey, iValue);

#elif ZCONFIG(API_Thread, POSIX)

	::pthread_setspecific(iKey, iValue);

#elif ZCONFIG(API_Thread, Be)

	::tls_set(iKey, iValue);

#endif // API_Thread
	}

// ==================================================
#pragma mark -
#pragma mark * ZThread::sThreadEntry

#if 0

#elif ZCONFIG(API_Thread, Win32)

unsigned int __stdcall ZThread::sThreadEntry_Win32(void* iArg)
	{
	ZThread* currentThread = reinterpret_cast<ZThread*>(iArg);
	ZAssertStop(kDebug_Thread, currentThread);
	try
		{
		sTLSSet(sKeyCurrentThread, reinterpret_cast<TLSData_t>(currentThread));

//		::CoInitializeEx(NULL, COINIT_MULTITHREADED);

		currentThread->Run();
		}
	catch (exception& theEx)
		{
		ZDebugLogf(2, ("Uncaught exception: %s", theEx.what()));
		}
	catch (...)
		{
		ZDebugLogf(2, ("Uncaught exception, unknown type"));
		}

	delete currentThread;
	return 0;
	}

#elif ZCONFIG(API_Thread, POSIX)

void* ZThread::sThreadEntry_POSIX(void* iArg)
	{
	ZThread* currentThread = reinterpret_cast<ZThread*>(iArg);
	ZAssertStop(kDebug_Thread, currentThread);
	try
		{
		sTLSSet(sKeyCurrentThread, reinterpret_cast<TLSData_t>(currentThread));

		// Wait till our start mutex is released
		::pthread_mutex_lock(&currentThread->fMutex_Start);
		while (!currentThread->fStarted)
			::pthread_cond_wait(&currentThread->fCond_Start, &currentThread->fMutex_Start);
		::pthread_mutex_unlock(&currentThread->fMutex_Start);

		// Destroy the mutex and cond, they won't be referenced again in our
		// thread's lifetime, and they might consume limited resources.
		::pthread_mutex_destroy(&currentThread->fMutex_Start);
		::pthread_cond_destroy(&currentThread->fCond_Start);

		currentThread->Run();
		}
	catch (exception& theEx)
		{
		ZDebugLogf(2, ("Uncaught exception: %s", theEx.what()));
		}
	catch (...)
		{
		ZDebugLogf(2, ("Uncaught exception, unknown type"));
		}
	delete currentThread;
	return nil;
	}

#elif ZCONFIG(API_Thread, Be)

int32 ZThread::sThreadEntry_Be(void* iArg)
	{
	ZThread* currentThread = reinterpret_cast<ZThread*>(iArg);
	ZAssertStop(kDebug_Thread, currentThread);
	try
		{
		sTLSSet(sKeyCurrentThread, reinterpret_cast<TLSData_t>(currentThread));

		currentThread->Run();
		}
	catch (exception& theEx)
		{
		ZDebugLogf(2, ("Uncaught exception: %s", theEx.what()));
		}
	catch (...)
		{
		ZDebugLogf(2, ("Uncaught exception, unknown type"));
		}

	delete currentThread;
	return 0;
	}

#endif // API_Thread

// ==================================================
#pragma mark -
#pragma mark * ZThread::Ex_Disposed

ZThread::Ex_Disposed::Ex_Disposed()
:	runtime_error("ZThread::Ex_Disposed")
	{}

ZThread::Ex_Disposed::~Ex_Disposed() throw()
	{}

// =================================================================================================
#if ZCONFIG(API_Thread, Win32)

#pragma mark -
#pragma mark * ZSemaphore - Win32

struct ZSemaphore::Waiter_Win32
	{
	Waiter_Win32* fPrev;
	Waiter_Win32* fNext;
	int32 fCount;
	ZTime fWake;
	bool fSemaphoreDisposed;
	HANDLE fThreadHANDLE;
	ZThread::ThreadID fThreadID;
	};

static bool sWaitForSemaphoreAbsolute(HANDLE iSemaphoreHANDLE, ZTime iWaitUntil)
	{
	while (true)
		{
		double timeout = iWaitUntil - ZTime::sSystem();
		if (timeout <= 0)
			break;

		double currentDelay = 1;
		if (currentDelay > timeout)
			currentDelay = timeout;

		DWORD result = ::WaitForSingleObject(iSemaphoreHANDLE, currentDelay * 1000);

		if (result == WAIT_OBJECT_0)
			return true;
		}
	return false;
	}

ZSemaphore::ZSemaphore(int32 iInitialCount, const char* iName)
	{
	ZAssertStop(kDebug_Thread, iInitialCount >= 0);
	fName = iName;
	fSpinlock.fValue = 0;
	fSemaphoreHANDLE = ::CreateSemaphore(nil, 0, 0x7FFFFFFF, nil);
	fWaiter_Head = nil;
	fWaiter_Tail = nil;
	fAvailable = iInitialCount;
	}

ZSemaphore::~ZSemaphore()
	{
	::sAcquireSpinlock(&fSpinlock);
	if (fWaiter_Head)
		{
		fWaiter_Head->fSemaphoreDisposed = true;
		::ReleaseSemaphore(fSemaphoreHANDLE, 1, nil);
		}
	::CloseHandle(fSemaphoreHANDLE);
	::sReleaseSpinlock(&fSpinlock);
	}

ZThread::Error ZSemaphore::Wait(int32 iCount)
	{
	ZAssertStop(kDebug_Thread, iCount >= 0);
	if (iCount == 0)
		return ZThread::errorNone;

	::sAcquireSpinlock(&fSpinlock);
	ZThread::Error result = this->Internal_Wait_Win32(iCount, WAITFOREVER);
	if (result != ZThread::errorDisposed)
		::sReleaseSpinlock(&fSpinlock);
	return result;
	}

ZThread::Error ZSemaphore::Wait(int32 iCount, bigtime_t iMicroseconds)
	{
	ZAssertStop(kDebug_Thread, iCount >= 0);
	if (iCount == 0)
		return ZThread::errorNone;

	::sAcquireSpinlock(&fSpinlock);
	ZThread::Error result = this->Internal_Wait_Win32(iCount, iMicroseconds);
	if (result != ZThread::errorDisposed)
		::sReleaseSpinlock(&fSpinlock);
	return result;
	}

void ZSemaphore::Signal(int32 iCount)
	{
	ZAssertStop(kDebug_Thread, iCount >= 0);
	if (iCount == 0)
		return;

	::sAcquireSpinlock(&fSpinlock);
	this->Internal_Signal_Win32(iCount);
	::sReleaseSpinlock(&fSpinlock);
	}

ZThread::Error ZSemaphore::Internal_Wait_Win32(int32 iCount, bigtime_t iMicroseconds)
	{
	if (fAvailable >= iCount)
		{
		fAvailable -= iCount;
		return ZThread::errorNone;
		}

	if (iMicroseconds <= 0)
		return ZThread::errorTimeout;

	// Set up our waiter
	Waiter_Win32 theWaiter;

	theWaiter.fWake = ZTime::sSystem() + (iMicroseconds / 1e6);

	theWaiter.fSemaphoreDisposed = false;
	theWaiter.fThreadHANDLE = nil;
	theWaiter.fThreadID = ZThread::sCurrentID();

	theWaiter.fCount = iCount - fAvailable;
	fAvailable = 0;

	// Link on to the chain
	theWaiter.fPrev = fWaiter_Tail;
	if (fWaiter_Tail)
		fWaiter_Tail->fNext = &theWaiter;
	else
		fWaiter_Head = &theWaiter;
	fWaiter_Tail = &theWaiter;
	theWaiter.fNext = nil;


	while (true)
		{
		if (fWaiter_Head == &theWaiter)
			{
			ZTime earliestWake = theWaiter.fWake;
			Waiter_Win32* current = theWaiter.fNext;
			while (current)
				{
				if (earliestWake > current->fWake)
					earliestWake = current->fWake;
				current = current->fNext;
				}
			::sReleaseSpinlock(&fSpinlock);
			::sWaitForSemaphoreAbsolute(fSemaphoreHANDLE, earliestWake);
			}
		else
			{
			if (HANDLE myThreadHANDLE = ::sCurrentThreadHANDLE())
				{
				theWaiter.fThreadHANDLE = myThreadHANDLE;
				::sReleaseSpinlock(&fSpinlock);
				::SuspendThread(theWaiter.fThreadHANDLE);
				theWaiter.fThreadHANDLE = nil;
				::sReleaseCurrentThreadHANDLE(myThreadHANDLE);
				}
			else
				{
				::sReleaseSpinlock(&fSpinlock);
				Sleep(1);
				}
			}

		if (theWaiter.fSemaphoreDisposed)
			{
			if (theWaiter.fNext)
				{
				theWaiter.fNext->fSemaphoreDisposed = true;
				while (theWaiter.fNext->fThreadHANDLE && ::ResumeThread(theWaiter.fNext->fThreadHANDLE) != 1)
					{}
				}
			return ZThread::errorDisposed;
			}

		::sAcquireSpinlock(&fSpinlock);
		if (theWaiter.fNext)
			{
			while (theWaiter.fNext->fThreadHANDLE && ::ResumeThread(theWaiter.fNext->fThreadHANDLE) != 1)
				{}
			}

		if (theWaiter.fWake < ZTime::sSystem())
			break;
		if (theWaiter.fCount == 0)
			break;
		}

	// Unlink ourselves
	if (theWaiter.fNext)
		theWaiter.fNext->fPrev = theWaiter.fPrev;
	if (theWaiter.fPrev)
		theWaiter.fPrev->fNext = theWaiter.fNext;
	if (fWaiter_Head == &theWaiter)
		fWaiter_Head = theWaiter.fNext;
	if (fWaiter_Tail == &theWaiter)
		fWaiter_Tail = theWaiter.fPrev;

	if (theWaiter.fCount > 0)
		{
		// If we have count left, restore whatever we were able to get
		this->Internal_Signal_Win32(iCount - theWaiter.fCount);
		return ZThread::errorTimeout;
		}

	return ZThread::errorNone;
	}

void ZSemaphore::Internal_Signal_Win32(int32 iCount)
	{
	if (Waiter_Win32* current = fWaiter_Head)
		{
		while (current && iCount > 0)
			{
			if (iCount >= current->fCount)
				{
				iCount -= current->fCount;
				current->fCount = 0;
				current = current->fNext;
				}
			else
				{
				current->fCount -= iCount;
				iCount = 0;
				}
			}
		fAvailable += iCount;
		::ReleaseSemaphore(fSemaphoreHANDLE, 1, nil);
		}
	else
		{
		fAvailable += iCount;
		}
	}

#endif // ZCONFIG(API_Thread, Win32)

// ==================================================
#if ZCONFIG(API_Thread, POSIX)
#pragma mark -
#pragma mark * ZSemaphore - POSIX

struct ZSemaphore::Waiter_POSIX
	{
	Waiter_POSIX* fPrev;
	Waiter_POSIX* fNext;
	int32 fCount;
	};

ZSemaphore::ZSemaphore(int32 iInitialCount, const char* iName)
	{
	ZAssertStop(kDebug_Thread, iInitialCount >= 0);
	fName = iName;
	fWaiter_Head = nil;
	fWaiter_Tail = nil;
	fAvailable = iInitialCount;
	fDisposed = false;
	::pthread_mutex_init(&fMutex, nil);
	::pthread_cond_init(&fCond, nil);
	}

ZSemaphore::~ZSemaphore()
	{
	::pthread_mutex_lock(&fMutex);
	fDisposed = true;
	if (fWaiter_Tail)
		::pthread_cond_broadcast(&fCond);

	while (fWaiter_Tail)
		::pthread_cond_wait(&fCond, &fMutex);
	::pthread_mutex_unlock(&fMutex);

	::pthread_mutex_destroy(&fMutex);
	::pthread_cond_destroy(&fCond);
	}

ZThread::Error ZSemaphore::Wait(int32 iCount)
	{
	ZAssertStop(kDebug_Thread, iCount >= 0 && fDisposed == false);
	if (iCount == 0)
		return ZThread::errorNone;

	::pthread_mutex_lock(&fMutex);
	ZThread::Error result = this->Internal_Wait_POSIX(iCount, WAITFOREVER);
	if (result != ZThread::errorDisposed)
		::pthread_mutex_unlock(&fMutex);
	return result;
	}

ZThread::Error ZSemaphore::Wait(int32 iCount, bigtime_t iMicroseconds)
	{
	ZAssertStop(kDebug_Thread, iCount >= 0 && fDisposed == false);
	if (iCount == 0)
		return ZThread::errorNone;

	::pthread_mutex_lock(&fMutex);
	ZThread::Error result = this->Internal_Wait_POSIX(iCount, iMicroseconds);
	::pthread_mutex_unlock(&fMutex);
	return result;
	}

void ZSemaphore::Signal(int32 iCount)
	{
	::pthread_mutex_lock(&fMutex);
	this->Internal_Signal_POSIX(iCount);
	::pthread_mutex_unlock(&fMutex);
	}

ZThread::Error ZSemaphore::Internal_Wait_POSIX(int32 iCount, bigtime_t iMicroseconds)
	{
	if (fAvailable >= iCount)
		{
		fAvailable -= iCount;
		return ZThread::errorNone;
		}

	if (iMicroseconds <= 0)
		return ZThread::errorTimeout;

	// Set up our waiter
	Waiter_POSIX theWaiter;
	theWaiter.fCount = iCount - fAvailable;
	fAvailable = 0;

	// Link on to the chain
	theWaiter.fPrev = fWaiter_Tail;
	if (fWaiter_Tail)
		fWaiter_Tail->fNext = &theWaiter;
	else
		fWaiter_Head = &theWaiter;
	fWaiter_Tail = &theWaiter;
	theWaiter.fNext = nil;

	if (iMicroseconds < WAITFOREVER)
		{
		timeval theTimeVal;
		::gettimeofday(&theTimeVal, nil);
		bigtime_t timeNow = (theTimeVal.tv_sec * 1000000LL) + theTimeVal.tv_usec;
		bigtime_t timeWake = timeNow + iMicroseconds;
		if (timeWake < timeNow || timeWake > (0x7FFFFFFFLL * 1000000LL))
			timeWake = 0x7FFFFFFFLL * 1000000LL;
		timespec theTimeSpec;
		theTimeSpec.tv_sec = timeWake / 1000000;
		theTimeSpec.tv_nsec = (timeWake % 1000000) * 1000;

		while (theWaiter.fCount != 0 && !fDisposed)
			{
			int result = ::pthread_cond_timedwait(&fCond, &fMutex, &theTimeSpec);
			if (ETIMEDOUT == result)
				break;
			}
		}
	else
		{
		while (theWaiter.fCount != 0 && !fDisposed)
			::pthread_cond_wait(&fCond, &fMutex);
		}

	// Unlink from the chain
	if (theWaiter.fNext)
		theWaiter.fNext->fPrev = theWaiter.fPrev;
	if (theWaiter.fPrev)
		theWaiter.fPrev->fNext = theWaiter.fNext;
	if (fWaiter_Head == &theWaiter)
		fWaiter_Head = theWaiter.fNext;
	if (fWaiter_Tail == &theWaiter)
		fWaiter_Tail = theWaiter.fPrev;

	if (fDisposed)
		{
		// Let our destructor know we've unlinked
		::pthread_cond_broadcast(&fCond);
		return ZThread::errorDisposed;
		}

	if (theWaiter.fCount != 0)
		{
		// If we have count left, restore whatever we were able to get.
		this->Internal_Signal_POSIX(iCount - theWaiter.fCount);
		return ZThread::errorTimeout;
		}

	return ZThread::errorNone;
	}

void ZSemaphore::Internal_Signal_POSIX(int32 iCount)
	{
	if (Waiter_POSIX* current = fWaiter_Head)
		{
		while (current && iCount > 0)
			{
			if (iCount >= current->fCount)
				{
				iCount -= current->fCount;
				current->fCount = 0;
				current = current->fNext;
				}
			else
				{
				current->fCount -= iCount;
				iCount = 0;
				}
			}
		fAvailable += iCount;
		::pthread_cond_broadcast(&fCond);
		}
	else
		{
		fAvailable += iCount;
		}
	}

#endif // ZCONFIG(API_Thread, POSIX)

// ==================================================
#if ZCONFIG(API_Thread, Be)
#pragma mark -
#pragma mark * ZSemaphore - Be

ZSemaphore::ZSemaphore(int32 iInitialCount, const char* iName)
	{
	ZAssertStop(kDebug_Thread, iInitialCount >= 0);
	fName = iName;
	fSem = ::create_sem(iInitialCount, iName);
	}

ZSemaphore::~ZSemaphore()
	{
	::delete_sem(fSem);
	}

ZThread::Error ZSemaphore::Wait(int32 iCount)
	{
	if (B_NO_ERROR == ::acquire_sem(fSem))
		return ZThread::errorNone;
	return ZThread::errorDisposed;
	}

ZThread::Error ZSemaphore::Wait(int32 iCount, bigtime_t iMicroseconds)
	{
	ZAssertStop(kDebug_Thread, iCount >= 0);
	if (iCount == 0)
		return ZThread::errorNone;
	if (iMicroseconds <= 0)
		iMicroseconds = 0;
	status_t result = ::acquire_sem_etc(fSem, iCount, B_RELATIVE_TIMEOUT, iMicroseconds);
	if (result == B_NO_ERROR)
		return ZThread::errorNone;
	if (result == B_TIMED_OUT)
		return ZThread::errorTimeout;
	return ZThread::errorDisposed;
	}

void ZSemaphore::Signal(int32 iCount)
	{
	ZAssertStop(kDebug_Thread, iCount >= 0);
	if (iCount > 0)
		::release_sem_etc(fSem, iCount, B_DO_NOT_RESCHEDULE);
	}

#endif // ZCONFIG(API_Thread, Be)

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexNR

// AG 2000-03-08. ZMutexNR is now a Benaphore. When acquiring, we increment fLock, and if it was already
// greater than zero, then we block on fSem. When releasing we decrement fLock, and if it was greater than one,
// then we release fSem.

ZMutexNR::ZMutexNR(const char* iName)
:	fSem(0, iName)
	{
	fLock.fValue = 0;
	}

ZMutexNR::~ZMutexNR()
	{}

ZThread::Error ZMutexNR::Acquire()
	{
	if (ZThreadSafe_IncReturnOld(fLock) > 0)
		return fSem.Wait(1);
	return ZThread::errorNone;
	}

void ZMutexNR::Release()
	{
	if (ZThreadSafe_DecReturnOld(fLock) > 1)
		fSem.Signal(1);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexBase

ZMutexBase::ZMutexBase(const char* iName)
:	fName(iName)
	{}

ZMutexBase::~ZMutexBase()
	{}

void ZMutexBase::AcquireOrThrow()
	{
	if (this->Acquire() != ZThread::errorNone)
		throw ZThread::Ex_Disposed();
	}

#if ZCONFIG_Thread_DeadlockDetect

static ZAtomic_t sSpinlock_Deadlock;

void ZMutexBase::CheckForDeadlock_Pre(ZThread* iAcquiringThread)
	{
	if (iAcquiringThread)
		{
		sAcquireSpinlock(&sSpinlock_Deadlock);
		if (string* result = this->CheckForDeadlockImp(iAcquiringThread))
			{
			sReleaseSpinlock(&sSpinlock_Deadlock);
			if (sDeadlockHandler)
				{
				string stackString = *result;
				delete result;
				sDeadlockHandler(stackString);
				}
			else
				{
				// AG 2000-07-12. We call ZDebugStop in a loop so that the message comes up over and over,
				// so if I over-eagerly tell the debugger to start running again, we'll just drop back into the debugger.
				for (;;)
					{
					ZDebugStopf(0, (result->c_str()));
					}
				}
			}
		else
			{
			iAcquiringThread->fMutexBase_Wait = this;
			sReleaseSpinlock(&sSpinlock_Deadlock);
			}
		}
	}
void ZMutexBase::CheckForDeadlock_Post(ZThread* iAcquiringThread)
	{
	if (iAcquiringThread)
		{
		sAcquireSpinlock(&sSpinlock_Deadlock);
		iAcquiringThread->fMutexBase_Wait = nil;
		sReleaseSpinlock(&sSpinlock_Deadlock);
		}
	}

string* ZMutexBase::CheckDeadlockThread(ZThread* iAcquiringThread, ZThread* iOwnerThread)
	{
	if (string* result = iOwnerThread->CheckForDeadlock(iAcquiringThread))
		{
		*result += ZString::sFormat("mutex %08X (\"%s\"), owned by thread %08X ID %08X (\"%s\")\n",
									this, this->fName, iOwnerThread, iOwnerThread->fThreadID, iOwnerThread->fName);
		return result;
		}
	return nil;
	}

void ZMutexBase::ReleaseForDeadlock_Pre()
	{
	sAcquireSpinlock(&sSpinlock_Deadlock);
	}

void ZMutexBase::ReleaseForDeadlock_Post()
	{
	sReleaseSpinlock(&sSpinlock_Deadlock);
	}

#endif // ZCONFIG_Thread_DeadlockDetect

// =================================================================================================
#pragma mark -
#pragma mark * ZMutex

ZMutex::ZMutex(const char* iName, bool iCreateAcquired)
:	ZMutexBase(iName),
	fSem(iCreateAcquired ? 0 : 1, iName)
	{
	if (iCreateAcquired)
		{
		#if ZCONFIG_Thread_DeadlockDetect
		fThread_Owner = ::sCurrent();
		#endif // ZCONFIG_Thread_DeadlockDetect

		fCount = 1;
		fThreadID_Owner = ZThread::sCurrentID();
		}
	else
		{
		#if ZCONFIG_Thread_DeadlockDetect
		fThread_Owner = nil;
		#endif // ZCONFIG_Thread_DeadlockDetect

		fCount = 0;
		fThreadID_Owner = kThreadID_None;
		}
	}

ZMutex::~ZMutex()
	{
	// It's only legal to destroy an unowned mutex, or one that is owned by the current thread
	ZAssertStopf(kDebug_Thread, fThreadID_Owner == kThreadID_None || fThreadID_Owner == ZThread::sCurrentID(), ("Mutex name %s", (fName ? fName : "none")));
	}

ZThread::Error ZMutex::Acquire()
	{ return this->MutexAcquire(WAITFOREVER); }

ZThread::Error ZMutex::Acquire(bigtime_t iMicroseconds)
	{ return this->MutexAcquire(iMicroseconds); }

void ZMutex::Release()
	{ this->MutexRelease(); }

bool ZMutex::IsLocked()
	{ return fThreadID_Owner == ZThread::sCurrentID(); }

ZThread::Error ZMutex::MutexAcquire(bigtime_t iMicroseconds)
	{
	// This check of fThreadID_Owner is safe, even if it doesn't look it. fThreadID_Owner is going to have one of
	// three values. It could be nil, in which case it won't match sCurrentID, or it could be
	// the ID of some other thread, in which case it still won't match currentID. In both these
	// cases we'll simply call Internal_Acquire, which will block on fSem. The third value it
	// could have is sCurrentID, in which case we don't block on fSem, we just increment fCount.
	// In the first two cases some other thread could race us and change the value from nil to
	// some other ID, or vice versa -- which won't make any difference to us, we need to block
	// anyway. The only way this could fuck up is if some other thread changes the value of fThreadID_Owner,
	// and that change is not atomic -- so that at the point we read its value, fThreadID_Owner's bytes have
	// taken on some value which isn't nil, and isn't the ID of the other thread, *and* which
	// accidentally matches our ID.
	if (fThreadID_Owner != ZThread::sCurrentID())
		{
		ZThread::Error err = this->Internal_Acquire(iMicroseconds);
		if (err != ZThread::errorNone)
			return err;
		}
	++fCount;
	return ZThread::errorNone;
	}

void ZMutex::MutexRelease()
	{
	ZAssertStop(kDebug_Thread, fThreadID_Owner == ZThread::sCurrentID());
	if (--fCount == 0)
		this->Internal_Release();
	}

ZThread::Error ZMutex::FullAcquire(int32 iCount)
	{
	if (iCount > 0)
		{
		if (fThreadID_Owner != ZThread::sCurrentID())
			{
			ZThread::Error err = this->Internal_Acquire(WAITFOREVER);
			if (err != ZThread::errorNone)
				return err;
			}
		fCount += iCount;
		}
	return ZThread::errorNone;
	}

int32 ZMutex::FullRelease()
	{
	if (fThreadID_Owner != ZThread::sCurrentID())
		return 0;
	int32 oldCount = fCount;
	fCount = 0;
	this->Internal_Release();
	return oldCount;
	}

#if !ZCONFIG_Thread_DeadlockDetect
ZThread::Error ZMutex::Internal_Acquire(bigtime_t iMicroseconds)
	{
	ZThread::Error err = fSem.Wait(1, iMicroseconds);
	if (err != ZThread::errorNone)
		return err;
	fThreadID_Owner = ZThread::sCurrentID();
	return ZThread::errorNone;
	}

void ZMutex::Internal_Release()
	{
	fThreadID_Owner = kThreadID_None;
	fSem.Signal(1);
	}
#endif // !ZCONFIG_Thread_DeadlockDetect

#if ZCONFIG_Thread_DeadlockDetect
ZThread::Error ZMutex::Internal_Acquire(bigtime_t iMicroseconds)
	{
	ZThread* currentThread = ::sCurrent();

	if (iMicroseconds == WAITFOREVER)
		this->CheckForDeadlock_Pre(currentThread);

	ZThread::Error err = fSem.Wait(1, iMicroseconds);

	if (iMicroseconds == WAITFOREVER)
		this->CheckForDeadlock_Post(currentThread);

	if (err != ZThread::errorNone)
		return err;

	fThreadID_Owner = ZThread::sCurrentID();
	fThread_Owner = currentThread;

	return ZThread::errorNone;
	}

void ZMutex::Internal_Release()
	{
	this->ReleaseForDeadlock_Pre();
	fThread_Owner = nil;
	this->ReleaseForDeadlock_Post();

	fThreadID_Owner = kThreadID_None;
	fSem.Signal(1);
	}

string* ZMutex::CheckForDeadlockImp(ZThread* iAcquiringThread)
	{
	if (fThread_Owner)
		return this->CheckDeadlockThread(iAcquiringThread, fThread_Owner);
	return nil;
	}
#endif // ZCONFIG_Thread_DeadlockDetect

// =================================================================================================
#pragma mark -
#pragma mark * ZCondition

ZCondition::ZCondition(const char* iName)
:	fSem_Wait(0, iName),
	fName(iName)
	{
	ZThreadSafe_Set(fWaitingThreads, 0);
	}

ZCondition::~ZCondition()
	{
	ZAssertStop(kDebug_Thread, 0 == ZThreadSafe_Get(fWaitingThreads));
	}

ZThread::Error ZCondition::Wait(ZMutex& iMutex)
	{
	return this->Wait(iMutex, WAITFOREVER);
	}

ZThread::Error ZCondition::Wait(ZMutex& iMutex, bigtime_t iMicroseconds)
	{
	ZAssertStop(kDebug_Thread, iMutex.fThreadID_Owner == ZThread::sCurrentID());

	// Record the fact that we're waiting
	ZThreadSafe_Inc(fWaitingThreads);

	// Completely release iMutex
	int32 oldCount = iMutex.fCount;
	iMutex.fCount = 0;
	iMutex.Internal_Release();

	// Wait for the wait semaphore to be signalled
	if (fSem_Wait.Wait(1, iMicroseconds) != ZThread::errorNone)
		ZThreadSafe_Dec(fWaitingThreads);

	// Completely reacquire iMutex
	ZThread::Error err = iMutex.Internal_Acquire(WAITFOREVER);
	if (err != ZThread::errorNone)
		return err;

	iMutex.fCount = oldCount;
	return ZThread::errorNone;
	}

ZThread::Error ZCondition::Wait(ZMutexNR& iMutex)
	{
	return this->Wait(iMutex, WAITFOREVER);
	}

ZThread::Error ZCondition::Wait(ZMutexNR& iMutex, bigtime_t iMicroseconds)
	{
	// Record the fact that we're waiting
	ZThreadSafe_Inc(fWaitingThreads);

	// Release iMutex
	if (ZThreadSafe_DecReturnOld(iMutex.fLock) > 1)
		iMutex.fSem.Signal(1);

	// Wait for the wait semaphore to be signalled
	fSem_Wait.Wait(1, iMicroseconds);

	// Reacquire iMutex
	if (ZThreadSafe_IncReturnOld(iMutex.fLock) > 0)
		return iMutex.fSem.Wait(1);

	return ZThread::errorNone;
	}

void ZCondition::Signal()
	{
	for (;;)
		{
		if (int oldCount = ZAtomic_Get(&fWaitingThreads))
			{
			if (ZAtomic_CompareAndSwap(&fWaitingThreads, oldCount, oldCount - 1))
				{
				fSem_Wait.Signal(1);
				break;
				}
			}
		else
			{
			break;
			}
		}
	}

void ZCondition::Broadcast()
	{
	for (;;)
		{
		if (int oldCount = ZAtomic_Get(&fWaitingThreads))
			{
			if (ZAtomic_CompareAndSwap(&fWaitingThreads, oldCount, 0))
				{
				fSem_Wait.Signal(oldCount);
				break;
				}
			}
		else
			{
			break;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRWLock

ZRWLock::ZRWLock(const char* iName)
:	fMutex_Structure("ZRWLock::fMutex_Structure"),
	fSem_Access(1, "ZRWLock::fSem_Access"),
	fSem_SubsequentReaders(0, "ZRWLock::fSem_SubsequentReaders"),
	fCount_WaitingWriters(0),
	fCount_CurrentWriter(0), fThreadID_CurrentWriter(kThreadID_None),
	fCount_WaitingReaders(0),
	fCount_CurrentReaders(0),
	fRead(this, iName),
	fWrite(this, iName)
	{
	#if ZCONFIG_Thread_DeadlockDetect
	fThread_CurrentWriter = nil;
	#endif
	}

ZRWLock::~ZRWLock()
	{
	ZAssertStop(kDebug_Thread, fCount_WaitingWriters == 0);
	ZAssertStop(kDebug_Thread, fCount_CurrentWriter == 0);
	ZAssertStop(kDebug_Thread, fCount_WaitingReaders == 0);
	ZAssertStop(kDebug_Thread, fCount_CurrentReaders == 0);
	ZAssertStop(kDebug_Thread, fVector_CurrentReaders.size() == 0);
	ZAssertStop(kDebug_Thread, fThreadID_CurrentWriter == kThreadID_None);
	}

void ZRWLock::AcquireRead()
	{ this->AcquireRead(WAITFOREVER); }

void ZRWLock::ReleaseRead()
	{ this->ReturnLock(); }

void ZRWLock::AcquireWrite()
	{ this->AcquireWrite(WAITFOREVER); }

void ZRWLock::ReleaseWrite()
	{ this->ReturnLock(); }

void ZRWLock::DemoteWriteToRead()
	{
	fMutex_Structure.Acquire();

	const ZThread::ThreadID currentID = ZThread::sCurrentID();

	ZAssert(fThreadID_CurrentWriter == currentID);
	fThreadID_CurrentWriter = kThreadID_None;

	ZAssert(fCount_CurrentWriter);
	fCount_CurrentWriter = 0;

	ZAssert(fCount_CurrentReaders == 0);
	fCount_CurrentReaders = 1;
	fVector_CurrentReaders.push_back(currentID);

	fSem_SubsequentReaders.Signal(fCount_WaitingReaders);
	fCount_CurrentReaders += fCount_WaitingReaders;
	fCount_WaitingReaders = 0;

	fMutex_Structure.Release();
	}


#define kDebug_RWLock 3

ZThread::Error ZRWLock::AcquireWrite(bigtime_t iMicroseconds)
	{
	// AG 2000-08-01. WARNING! WARNING! WARNING!. This method does not yet support timeouts.
	ZAssertStop(kDebug_RWLock, iMicroseconds >= WAITFOREVER);

	fMutex_Structure.Acquire();
	ZThread::ThreadID currentID = ZThread::sCurrentID();

	// Trip an assertion if this is a call on a lock we've already locked for reading, which means we're gonna deadlock.
	ZAssertLogf(kDebug_Thread, find(fVector_CurrentReaders.begin(), fVector_CurrentReaders.end(), currentID) == fVector_CurrentReaders.end(), ("DEADLOCK"));

	if (fThreadID_CurrentWriter == currentID)
		{
		// We're the current writer so this is a recursive acquisition and we can just increment the count.
		++fCount_CurrentWriter;
		ZAssertStop(kDebug_Thread, fCount_CurrentReaders == 0);
		ZDebugLogf(kDebug_RWLock, ("Thread %04X, Got it write recursive %d", ZThread::sCurrentID(), fCount_CurrentWriter));
		fMutex_Structure.Release();
		return ZThread::errorNone;
		}

	// Indicate that we're waiting to write
	++fCount_WaitingWriters;
	ZDebugLogf(kDebug_RWLock, ("Thread %04X, Waiting write", ZThread::sCurrentID()));
	fMutex_Structure.Release();

	#if ZCONFIG_Thread_DeadlockDetect
	ZThread* currentThread = ::sCurrent();
	fWrite.CheckForDeadlock_Pre(currentThread);
	#endif // ZCONFIG_Thread_DeadlockDetect

	ZThread::Error err = fSem_Access.Wait(1, iMicroseconds);

	#if ZCONFIG_Thread_DeadlockDetect
	fWrite.CheckForDeadlock_Post(currentThread);
	#endif // ZCONFIG_Thread_DeadlockDetect

	ZAssertStop(kDebug_Thread, err == ZThread::errorNone);

	fMutex_Structure.Acquire();
	ZDebugLogf(kDebug_RWLock, ("Thread %04X, Got it write", ZThread::sCurrentID()));
	// We're no longer waiting
	--fCount_WaitingWriters;

	ZAssertStop(kDebug_Thread, fThreadID_CurrentWriter == kThreadID_None && fCount_CurrentWriter == 0);
	ZAssertStop(kDebug_Thread, fCount_CurrentReaders == 0);
	fThreadID_CurrentWriter = currentID;

	#if ZCONFIG_Thread_DeadlockDetect
	fThread_CurrentWriter = currentThread;
	#endif // ZCONFIG_Thread_DeadlockDetect

	fCount_CurrentWriter = 1;
	fMutex_Structure.Release();

	return ZThread::errorNone;
	}

ZThread::Error ZRWLock::AcquireRead(bigtime_t iMicroseconds)
	{
	// AG 2000-08-01. WARNING! WARNING! WARNING!. This method does not yet support
	// timeouts. The issue is that the first reader releases a lock that other readers are
	// waiting on -- if it times out one of the other readers must wake up and take over the
	// job, and I haven't decided how to do that yet.
	ZAssertStop(kDebug_RWLock, iMicroseconds >= WAITFOREVER);
	fMutex_Structure.Acquire();

	ZThread::ThreadID currentID = ZThread::sCurrentID();

	if (fThreadID_CurrentWriter == currentID)
		{
		// We're the current writer, just reacquire the write lock (this is fair, cause we've already
		// locked out other readers by the fact that we posess the write lock. And anyway, if we were
		// to fail to acquire the read lock, we'd just deadlock the entire system)
		++fCount_CurrentWriter;
		}
	else
		{
		#if ZCONFIG_Thread_DeadlockDetect
		ZThread* currentThread = ::sCurrent();
		#endif // ZCONFIG_Thread_DeadlockDetect

		// The following check on fCount_CurrentReaders short circuits the call to find(); if
		// fCount_CurrentReaders is zero then obviously we can't find ourselves in the vector.
		if (fCount_CurrentReaders && find(fVector_CurrentReaders.begin(), fVector_CurrentReaders.end(), currentID) != fVector_CurrentReaders.end())
			{
			// We're a current reader, so just stick ourselves on the list again.
			++fCount_CurrentReaders;
			fVector_CurrentReaders.push_back(currentID);

			#if ZCONFIG_Thread_DeadlockDetect
			fVector_Thread_CurrentReaders.push_back(currentThread);
			#endif // ZCONFIG_Thread_DeadlockDetect

			ZDebugLogf(kDebug_RWLock, ("Thread %04X, Got it read recursive, total: %d", ZThread::sCurrentID(), fCount_CurrentReaders));
			}
		else
			{
			if (fCount_WaitingWriters != 0 || fCount_CurrentReaders == 0)
				{
				// There are waiting writers, or there are no current readers, so we have to ensure the access lock is grabbed
				++fCount_WaitingReaders;
				if (fCount_WaitingReaders == 1)
					{
					// We're the first reader to come along, acquire the access lock.
					ZDebugLogf(kDebug_RWLock, ("Thread %04X, Waiting read", ZThread::sCurrentID()));
					fMutex_Structure.Release();

					#if ZCONFIG_Thread_DeadlockDetect
					fRead.CheckForDeadlock_Pre(currentThread);
					#endif // ZCONFIG_Thread_DeadlockDetect

					fSem_Access.Wait(1);

					#if ZCONFIG_Thread_DeadlockDetect
					fRead.CheckForDeadlock_Post(currentThread);
					#endif // ZCONFIG_Thread_DeadlockDetect

					fMutex_Structure.Acquire();

					ZDebugLogf(kDebug_RWLock, ("Thread %04X, Got it read", ZThread::sCurrentID()));
					ZAssertStop(kDebug_Thread, fThreadID_CurrentWriter == 0 && fCount_CurrentWriter == 0);
					ZDebugLogf(kDebug_RWLock, ("Thread %04X, Signal subs: %d", ZThread::sCurrentID(), fCount_WaitingReaders - 1));

					// We've acquired the access lock, so let the subsequent readers through.
					fSem_SubsequentReaders.Signal(fCount_WaitingReaders - 1);
					fCount_CurrentReaders += fCount_WaitingReaders;
					fCount_WaitingReaders = 0;
					fVector_CurrentReaders.push_back(currentID);

					#if ZCONFIG_Thread_DeadlockDetect
					fVector_Thread_CurrentReaders.push_back(currentThread);
					#endif // ZCONFIG_Thread_DeadlockDetect
					}
				else
					{
					// We're a subsequent reader. Wait for the read barrier to be opened
					ZDebugLogf(kDebug_RWLock, ("Thread %04X, Waiting subsequent", ZThread::sCurrentID()));

					fMutex_Structure.Release();

					#if ZCONFIG_Thread_DeadlockDetect
					fRead.CheckForDeadlock_Pre(currentThread);
					#endif // ZCONFIG_Thread_DeadlockDetect

					fSem_SubsequentReaders.Wait(1);

					#if ZCONFIG_Thread_DeadlockDetect
					fRead.CheckForDeadlock_Post(currentThread);
					#endif // ZCONFIG_Thread_DeadlockDetect

					fMutex_Structure.Acquire();

					ZDebugLogf(kDebug_RWLock, ("Thread %04X, Got it subsequent, total: %d", ZThread::sCurrentID(), fCount_CurrentReaders));
					ZAssertStop(kDebug_Thread, fThreadID_CurrentWriter == 0 && fCount_CurrentWriter == 0);

					fVector_CurrentReaders.push_back(currentID);

					#if ZCONFIG_Thread_DeadlockDetect
					fVector_Thread_CurrentReaders.push_back(currentThread);
					#endif // ZCONFIG_Thread_DeadlockDetect
					}
				}
			else
				{
				// There were no waiting writers and there are current readers.
				ZAssertStop(kDebug_Thread, fThreadID_CurrentWriter == 0 && fCount_CurrentWriter == 0);
				ZDebugLogf(kDebug_RWLock, ("Thread %04X, Got it other", ZThread::sCurrentID()));

				++fCount_CurrentReaders;
				fVector_CurrentReaders.push_back(currentID);

				#if ZCONFIG_Thread_DeadlockDetect
				fVector_Thread_CurrentReaders.push_back(currentThread);
				#endif // ZCONFIG_Thread_DeadlockDetect
				}
			}
		}
	fMutex_Structure.Release();
	return ZThread::errorNone;
	}

void ZRWLock::ReturnLock()
	{
	fMutex_Structure.Acquire();
	// Current writer must be positive
	ZAssertStop(kDebug_Thread, fCount_CurrentWriter >= 0);
	// And reader or writer count must be non zero
	ZAssertStop(kDebug_Thread, fCount_CurrentWriter != 0 || fVector_CurrentReaders.size() != 0);
	// *And* they can't both be non zero
	ZAssertStop(kDebug_Thread, !(fCount_CurrentWriter != 0 && fVector_CurrentReaders.size() != 0));

	ZThread::ThreadID currentID = ZThread::sCurrentID();


	if (fThreadID_CurrentWriter == currentID)
		{
		#if ZCONFIG_Thread_DeadlockDetect
		fWrite.ReleaseForDeadlock_Pre();
		#endif // ZCONFIG_Thread_DeadlockDetect
	
		// We're returning the the write lock.
		if (--fCount_CurrentWriter == 0)
			{
			// We must also release the access lock
			fThreadID_CurrentWriter = kThreadID_None;

			#if ZCONFIG_Thread_DeadlockDetect
			fThread_CurrentWriter = nil;
			#endif // ZCONFIG_Thread_DeadlockDetect

			ZDebugLogf(kDebug_RWLock, ("Thread %04X, Returning write", ZThread::sCurrentID()));
			fSem_Access.Signal(1);
			}

		#if ZCONFIG_Thread_DeadlockDetect
		fWrite.ReleaseForDeadlock_Post();
		#endif // ZCONFIG_Thread_DeadlockDetect
		}
	else
		{
		#if ZCONFIG_Thread_DeadlockDetect
		fRead.ReleaseForDeadlock_Pre();
		#endif // ZCONFIG_Thread_DeadlockDetect

		// We're not the current writer, so we're returning the read lock. In which case
		// there can't be a current writer
		ZAssertStop(kDebug_Thread, fThreadID_CurrentWriter == kThreadID_None);

		vector<ZThread::ThreadID>::iterator theIter = find(fVector_CurrentReaders.begin(), fVector_CurrentReaders.end(), currentID);
		ZAssertStop(kDebug_Thread, theIter != fVector_CurrentReaders.end());
		fVector_CurrentReaders.erase(theIter);

		#if ZCONFIG_Thread_DeadlockDetect
		ZThread* currentThread = ::sCurrent();
		vector<ZThread*>::iterator iterThread = find(fVector_Thread_CurrentReaders.begin(), fVector_Thread_CurrentReaders.end(), currentThread);
		ZAssertStop(kDebug_Thread, iterThread != fVector_Thread_CurrentReaders.end());
		fVector_Thread_CurrentReaders.erase(iterThread);
		#endif // ZCONFIG_Thread_DeadlockDetect

		if (--fCount_CurrentReaders == 0)
			{
			ZAssertStop(kDebug_Thread, fVector_CurrentReaders.size() == 0);
			ZDebugLogf(kDebug_RWLock, ("Thread %04X, Returning read", ZThread::sCurrentID()));

			fSem_Access.Signal(1);
			}

		#if ZCONFIG_Thread_DeadlockDetect
		fRead.ReleaseForDeadlock_Post();
		#endif // ZCONFIG_Thread_DeadlockDetect
		}

	fMutex_Structure.Release();
	}

bool ZRWLock::CanRead()
	{
	fMutex_Structure.Acquire();
	ZThread::ThreadID currentID = ZThread::sCurrentID();
	if (fCount_CurrentWriter != 0 && fThreadID_CurrentWriter == currentID)
		{
		// We're the current writer, so we can also read.
		fMutex_Structure.Release();
		return true;
		}

	if (find(fVector_CurrentReaders.begin(), fVector_CurrentReaders.end(), currentID) != fVector_CurrentReaders.end())
		{
		// We're a current reader.
		fMutex_Structure.Release();
		return true;
		}

	fMutex_Structure.Release();
	return false;
	}

bool ZRWLock::CanWrite()
	{
	fMutex_Structure.Acquire();
	if (fCount_CurrentWriter != 0 && fThreadID_CurrentWriter == ZThread::sCurrentID())
		{
		// We're the current writer.
		fMutex_Structure.Release();
		return true;
		}
	fMutex_Structure.Release();
	return false;
	}

void ZRWLock::SetName(const char* iName)
	{
	fMutex_Structure.Acquire();
	fRead.SetName(iName);
	fWrite.SetName(iName);
	fMutex_Structure.Release();
	}

#if ZCONFIG_Thread_DeadlockDetect
string* ZRWLock::Internal_CheckForDeadlock(ZThread* iAcquiringThread, ZMutexBase* iLock)
	{
	if (fThread_CurrentWriter)
		{
		return iLock->CheckDeadlockThread(iAcquiringThread, fThread_CurrentWriter);
		}
	else
		{
		for (vector<ZThread*>::iterator i = fVector_Thread_CurrentReaders.begin(); i != fVector_Thread_CurrentReaders.end(); ++i)
			{
			if (string* result = iLock->CheckDeadlockThread(iAcquiringThread, *i))
				return result;
			}
		}
	return nil;
	}
#endif // ZCONFIG_Thread_DeadlockDetect

// =================================================================================================
#pragma mark -
#pragma mark * ZRWLock::Read

ZThread::Error ZRWLock::Read::Acquire()
	{ return fRWLock->AcquireRead(WAITFOREVER); }

ZThread::Error ZRWLock::Read::Acquire(bigtime_t iMicroseconds)
	{ return fRWLock->AcquireRead(iMicroseconds); }

void ZRWLock::Read::Release()
	{ fRWLock->ReturnLock(); }

bool ZRWLock::Read::IsLocked()
	{ return fRWLock->CanRead(); }

#if ZCONFIG_Thread_DeadlockDetect
string* ZRWLock::Read::CheckForDeadlockImp(ZThread* iAcquiringThread)
	{ return fRWLock->Internal_CheckForDeadlock(iAcquiringThread, this); }
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZRWLock::Write

ZThread::Error ZRWLock::Write::Acquire()
	{ return fRWLock->AcquireWrite(WAITFOREVER); }

ZThread::Error ZRWLock::Write::Acquire(bigtime_t iMicroseconds)
	{ return fRWLock->AcquireWrite(iMicroseconds); }

void ZRWLock::Write::Release()
	{ fRWLock->ReturnLock(); }

bool ZRWLock::Write::IsLocked()
	{ return fRWLock->CanWrite(); }

#if ZCONFIG_Thread_DeadlockDetect
string* ZRWLock::Write::CheckForDeadlockImp(ZThread* iAcquiringThread)
	{ return fRWLock->Internal_CheckForDeadlock(iAcquiringThread, this); }
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexComposite

ZMutexComposite::ZMutexComposite()
	{}

ZMutexComposite::~ZMutexComposite()
	{}

void ZMutexComposite::Add(ZMutexBase& iLock)
	{
	vector<ZMutexBase*>::iterator insertPos = lower_bound(fLocks.begin(), fLocks.end(), &iLock);
	fLocks.insert(insertPos, &iLock);
	}

ZThread::Error ZMutexComposite::Acquire()
	{
	for (vector<ZMutexBase*>::iterator w = fLocks.begin(); w != fLocks.end(); ++w)
		{
		ZThread::Error err = (*w)->Acquire();
		if (err != ZThread::errorNone)
			{
			for (vector<ZMutexBase*>::iterator i = fLocks.begin(); i != w; ++i)
				(*i)->Release();
			return err;
			}
		}
	return ZThread::errorNone;
	}

ZThread::Error ZMutexComposite::Acquire(bigtime_t iMicroseconds)
	{
	for (vector<ZMutexBase*>::iterator w = fLocks.begin(); w != fLocks.end(); ++w)
		{
		ZTime startTime = ZTime::sSystem();
		ZThread::Error err = (*w)->Acquire(iMicroseconds);
		if (err != ZThread::errorNone)
			{
			for (vector<ZMutexBase*>::iterator i = fLocks.begin(); i != w; ++i)
				(*i)->Release();
			return err;
			}

		bigtime_t timeConsumed = (ZTime::sSystem() - startTime) * 1e6;
		if (iMicroseconds >= timeConsumed)
			iMicroseconds -= timeConsumed;
		else
			iMicroseconds = 0;
		}
	return ZThread::errorNone;
	}

void ZMutexComposite::Release()
	{
	for (vector<ZMutexBase*>::iterator w = fLocks.begin(); w != fLocks.end(); ++w)
		(*w)->Release();
	}

bool ZMutexComposite::IsLocked()
	{
	if (fLocks.size() == 0)
		return false;
	return fLocks[fLocks.size() - 1]->IsLocked();
	}

#if ZCONFIG_Thread_DeadlockDetect
string* ZMutexComposite::CheckForDeadlockImp(ZThread* iAcquiringThread)
	{
	// This can't ever get called as we never get put into any
	// thread's fMutexBase_Wait field.
	ZUnimplemented();
	return nil;
	}
#endif // ZCONFIG_Thread_DeadlockDetect

// =================================================================================================
#pragma mark -
#pragma mark * ZLocker

ZLocker::ZLocker(const ZMutexBase& iMutexBase, bool iThrowIfProblem)
:	fMutexBase(const_cast<ZMutexBase&>(iMutexBase))
	{
	ZThread::Error err = fMutexBase.Acquire();
	if (err == ZThread::errorDisposed)
		{
		fOkay = false;
		if (iThrowIfProblem)
			throw ZThread::Ex_Disposed();
		fAcquisitions = 0;
		}
	else
		{
		ZAssertStop(kDebug_Thread, err == ZThread::errorNone);
		fOkay = true;
		fAcquisitions = 1;
		}
	}

ZLocker::~ZLocker()
	{
	ZAssertStop(kDebug_Thread, fOkay || fAcquisitions == 0);
	if (fOkay && fAcquisitions)
		{
		while (fAcquisitions--)
			fMutexBase.Release();
		}
	}

ZThread::Error ZLocker::Acquire()
	{
	ZAssertStop(kDebug_Thread, fOkay);
	ZThread::Error err = fMutexBase.Acquire();
	if (err == ZThread::errorDisposed)
		{
		fOkay = false;
		return err;
		}
	ZAssertStop(kDebug_Thread, err == ZThread::errorNone);
	++fAcquisitions;
	return ZThread::errorNone;
	}

void ZLocker::Release()
	{
	ZAssertStop(kDebug_Thread, fOkay);
	// We mustn't call signal more times than we've called wait
	ZAssertStop(kDebug_Thread, fAcquisitions > 0);
	--fAcquisitions;
	fMutexBase.Release();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMutexLocker

ZMutexLocker::ZMutexLocker(const ZMutex& iMutex, bool iThrowIfProblem)
:	fMutex(const_cast<ZMutex&>(iMutex))
	{
	ZThread::Error err = fMutex.MutexAcquire(WAITFOREVER);
	if (err == ZThread::errorDisposed)
		{
		fOkay = false;
		if (iThrowIfProblem)
			throw ZThread::Ex_Disposed();
		fAcquisitions = 0;
		}
	else
		{
		ZAssertStop(kDebug_Thread, err == ZThread::errorNone);
		fOkay = true;
		fAcquisitions = 1;
		}
	}

ZMutexLocker::~ZMutexLocker()
	{
	ZAssertStop(kDebug_Thread, fOkay || fAcquisitions == 0);
	if (fOkay && fAcquisitions)
		{
		while (fAcquisitions--)
			fMutex.MutexRelease();
		}
	}

ZThread::Error ZMutexLocker::Acquire()
	{
	ZAssertStop(kDebug_Thread, fOkay);
	ZThread::Error err = fMutex.MutexAcquire(WAITFOREVER);
	if (err == ZThread::errorDisposed)
		{
		fOkay = false;
		return err;
		}
	ZAssertStop(kDebug_Thread, err == ZThread::errorNone);
	++fAcquisitions;
	return ZThread::errorNone;
	}

void ZMutexLocker::Release()
	{
	ZAssertStop(kDebug_Thread, fOkay);
	// We mustn't call signal more times than we've called wait
	ZAssertStop(kDebug_Thread, fAcquisitions > 0);
	--fAcquisitions;
	fMutex.MutexRelease();
	}

// =================================================================================================
#pragma mark -
#pragma mark * CodeWarrior runtime library support

#if 0 // ZCONFIG(OS, Carbon) && ZCONFIG(Compiler, CodeWarrior)
#include <pool_alloc.h>

extern "C" void* __sys_alloc(size_t size)
	{
	ZAssertStop(0, !::MPTaskIsPreemptive(::MPCurrentTaskID()));
	return ::MPAllocateAligned(size, kMPAllocateDefaultAligned, 0);
	}

extern "C" void __sys_free(void* voidPtr)
	{
	ZAssertStop(0, !::MPTaskIsPreemptive(::MPCurrentTaskID()));
	if (voidPtr)
		::MPFree(voidPtr);
	}

#endif // ZCONFIG(OS, Carbon) && ZCONFIG(Compiler, CodeWarrior)
