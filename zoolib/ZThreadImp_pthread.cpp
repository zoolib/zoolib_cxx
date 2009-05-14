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

#include "zoolib/ZThreadImp_pthread.h"

#if ZCONFIG_API_Enabled(ThreadImp_pthread)

#include "zoolib/ZCompat_cmath.h" // for fmod
#include "zoolib/ZDebug.h"

#include <errno.h> // For errno
#include <sys/time.h> // For gettimeofday
#include <unistd.h> // For usleep

#include <new> // for std::bad_alloc

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZTSS_pthread

ZTSS_pthread::Key ZTSS_pthread::sCreate()
	{
	Key theKey;
	::pthread_key_create(&theKey, nullptr);
	return theKey;
	}

void ZTSS_pthread::sFree(Key iKey)
	{ ::pthread_key_delete(iKey); }

void ZTSS_pthread::sSet(Key iKey, Value iValue)
	{ ::pthread_setspecific(iKey, iValue); }

ZTSS_pthread::Value ZTSS_pthread::sGet(Key iKey)
	{ return ::pthread_getspecific(iKey); }

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_pthread

ZMtx_pthread::ZMtx_pthread(const char* iName)
	{ ::pthread_mutex_init(&fMutex, nullptr); }

ZMtx_pthread::~ZMtx_pthread()
	{ ::pthread_mutex_destroy(&fMutex); }

void ZMtx_pthread::Acquire()
	{ ::pthread_mutex_lock(&fMutex); }

void ZMtx_pthread::Release()
	{ ::pthread_mutex_unlock(&fMutex); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_pthread

ZCnd_pthread::ZCnd_pthread()
	{ ::pthread_cond_init(&fCond, nullptr); }

ZCnd_pthread::~ZCnd_pthread()
	{ ::pthread_cond_destroy(&fCond); }

void ZCnd_pthread::Wait(ZMtx_pthread& iMtx)
	{ ::pthread_cond_wait(&fCond, &iMtx.fMutex); }

void ZCnd_pthread::Wait(ZMtx_pthread& iMtx, double iTimeout)
	{
	timeval theTimeVal;
	::gettimeofday(&theTimeVal, nullptr);
	double wakeTime = theTimeVal.tv_sec + theTimeVal.tv_usec / 1e6 + iTimeout;

	timespec theTimeSpec;
	theTimeSpec.tv_sec = time_t(wakeTime);
	theTimeSpec.tv_nsec = int(fmod(wakeTime, 1.0) * 1e9);

	::pthread_cond_timedwait(&fCond, &iMtx.fMutex, &theTimeSpec);
	}

void ZCnd_pthread::Signal()
	{ ::pthread_cond_signal(&fCond); }

void ZCnd_pthread::Broadcast()
	{ ::pthread_cond_broadcast(&fCond); }

// =================================================================================================
#pragma mark -
#pragma mark * ZSemNoTimeout_pthread

ZSemNoTimeout_pthread::ZSemNoTimeout_pthread()
	{ ::sem_init(&fSem, 0, 0); }

ZSemNoTimeout_pthread::~ZSemNoTimeout_pthread()
	{ ::sem_destroy(&fSem); }

void ZSemNoTimeout_pthread::Wait()
	{
	for (;;)
		{
		if (int result = ::sem_wait(&fSem))
			{
			if (EINTR == result)
				continue;
			ZUnimplemented();
			}
		break;
		}
	}

bool ZSemNoTimeout_pthread::TryWait()
	{ return 0 == ::sem_trywait(&fSem); }

void ZSemNoTimeout_pthread::Signal()
	{ ::sem_post(&fSem); }

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadImp_pthread

ZThreadImp_pthread::ID ZThreadImp_pthread::sCreate(size_t iStackSize, Proc_t iProc, void* iParam)
	{
	pthread_attr_t threadAttr;
	::pthread_attr_init(&threadAttr);

	if (iStackSize == 0)
		{
		size_t defaultSize;
		::pthread_attr_getstacksize(&threadAttr, &defaultSize);
		iStackSize = defaultSize;
		}

	::pthread_attr_setstacksize(&threadAttr, iStackSize);
	::pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

	ID theID;
	int result = ::pthread_create(&theID, &threadAttr, iProc, iParam);
		
	::pthread_attr_destroy(&threadAttr);

	if (result != 0)
		throw std::bad_alloc();

	return theID;
	}

ZThreadImp_pthread::ID ZThreadImp_pthread::sID()
	{ return ::pthread_self(); }

void ZThreadImp_pthread::sSleep(double iDuration)
	{ ::usleep(useconds_t(iDuration * 1e6)); }

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(ThreadImp_pthread)
