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

bool ZCnd_pthread::pWaitFor(pthread_mutex_t& iMtx, double iTimeout)
	{
	if (iTimeout <= 0)
		return false;
	return this->pWaitUntil(iMtx, ZTime::sSystem() + iTimeout);
	}

bool ZCnd_pthread::pWaitUntil(pthread_mutex_t& iMtx, ZTime iDeadline)
	{
	timespec theTimeSpec;
	theTimeSpec.tv_sec = time_t(iDeadline.fVal);
	theTimeSpec.tv_nsec = int(fmod(iDeadline.fVal, 1.0) * 1e9);

	int result = ::pthread_cond_timedwait(this, &iMtx, &theTimeSpec);
	return result == 0;
	}

// =================================================================================================
// MARK: - ZMtxR_pthread

ZMtxR_pthread::ZMtxR_pthread()
	{
	pthread_mutexattr_t attr;
	::pthread_mutexattr_init(&attr);
	::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	::pthread_mutex_init(this, &attr);
	::pthread_mutexattr_destroy(&attr);
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_pthread)
