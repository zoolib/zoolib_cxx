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

#include "zoolib/ZThread_pthread.h"

#if ZCONFIG_API_Enabled(Thread_pthread)

#include "zoolib/ZCompat_cmath.h" // For fmod

#include <sys/time.h> // For gettimeofday
#include <unistd.h> // For usleep

#include <new> // For std::bad_alloc

// =================================================================================================
// MARK: - ZThread_pthread::sSetName

#if ZCONFIG_SPI_Enabled(MacOSX)
	#if defined(MAC_OS_X_VERSION_MIN_REQUIRED)
		#if defined(MAC_OS_X_VERSION_10_6) && MAC_OS_X_VERSION_10_6 <= MAC_OS_X_VERSION_MIN_REQUIRED
			void ZooLib::ZThread_pthread::sSetName(const char* iName)
				{ ::pthread_setname_np(iName); }
		#else
			extern int pthread_setname_np(const char*) __attribute__((weak_import));

			void ZooLib::ZThread_pthread::sSetName(const char* iName)
				{
				if (pthread_setname_np)
					::pthread_setname_np(iName);
				}
		#endif
	#endif
#elif ZCONFIG_SPI_Enabled(iPhone)
	void ZooLib::ZThread_pthread::sSetName(const char* iName)
		{ ::pthread_setname_np(iName); }
#else
	void ZooLib::ZThread_pthread::sSetName(const char* iName)
		{ ::pthread_setname_np(::ptherad_self(), iName); }
#endif

namespace ZooLib {

// =================================================================================================
// MARK: - ZThread_pthread

namespace ZThread_pthread {

void sCreateRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam)
	{
	pthread_attr_t threadAttr;
	::pthread_attr_init(&threadAttr);

	if (iStackSize != 0)
		::pthread_attr_setstacksize(&threadAttr, iStackSize);

	::pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

	ID theID;
	int result = ::pthread_create(&theID, &threadAttr, iProc, iParam);

	::pthread_attr_destroy(&threadAttr);

	if (result != 0)
		throw std::bad_alloc();
	}

void sSleep(double iDuration)
	{
	if (iDuration <= 0)
		::usleep(0);
	else if (iDuration < 1000)
		::usleep(useconds_t(iDuration * 1e6));
	else
		::usleep(1000000000);
	}

} // namespace ZThread_pthread

// =================================================================================================
// MARK: - ZTSS_pthread

namespace ZTSS_pthread {

Key sCreate()
	{
	Key theKey;
	::pthread_key_create(&theKey, nullptr);
	return theKey;
	}

void sFree(Key iKey)
	{ ::pthread_key_delete(iKey); }

} // namespace ZTSS_pthread

// =================================================================================================
// MARK: - ZCnd_pthread

bool ZCnd_pthread::pWaitFor(ZMtx_pthread_base& iMtx, double iTimeout)
	{
	if (iTimeout <= 0)
		return false;

	#if defined(__APPLE__) || defined(__ANDROID__)
		const timespec the_timespec = { time_t(iTimeout), long(1e9 * fmod(iTimeout, 1.0)) };
		return 0 == ::pthread_cond_timedwait_relative_np
			(&f_pthread_cond_t, &iMtx.f_pthread_mutex_t, &the_timespec);
	#else
		return this->pWaitUntil(iMtx, ZTime::sSystem() + iTimeout);
	#endif
	}

bool ZCnd_pthread::pWaitUntil(ZMtx_pthread_base& iMtx, ZTime iDeadline)
	{
	const timespec the_timespec = { time_t(iDeadline.fVal), long(1e9 * fmod(iDeadline.fVal, 1.0)) };
	return 0 == ::pthread_cond_timedwait(&f_pthread_cond_t, &iMtx.f_pthread_mutex_t, &the_timespec);
	}

// =================================================================================================
// MARK: - ~ZMtx_pthread_base

#if ZCONFIG_pthread_Debug

ZMtx_pthread_base::~ZMtx_pthread_base()
	{
	int result = ::pthread_mutex_destroy(&f_pthread_mutex_t);
	ZAssert(result == 0);
	}

void ZMtx_pthread_base::Acquire()
	{
	int result = ::pthread_mutex_lock(&f_pthread_mutex_t);
	ZAssert(result == 0);
	}

void ZMtx_pthread_base::Release()
	{
	int result = ::pthread_mutex_unlock(&f_pthread_mutex_t);
	ZAssert(result == 0);
	}

#endif // ZCONFIG_pthread_Debug

// =================================================================================================
// MARK: - ZMtx_pthread

#if ZCONFIG_pthread_Debug

ZMtx_pthread::ZMtx_pthread()
	{
	pthread_mutexattr_t attr;
	::pthread_mutexattr_init(&attr);
	::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	::pthread_mutex_init(&f_pthread_mutex_t, &attr);
	::pthread_mutexattr_destroy(&attr);
	}

#endif // ZCONFIG_pthread_Debug

// =================================================================================================
// MARK: - ZMtxR_pthread

ZMtxR_pthread::ZMtxR_pthread()
	{
	pthread_mutexattr_t attr;
	::pthread_mutexattr_init(&attr);
	::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	int result = ::pthread_mutex_init(&f_pthread_mutex_t, &attr);
	::pthread_mutexattr_destroy(&attr);

	ZAssert(result == 0);
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_pthread)
