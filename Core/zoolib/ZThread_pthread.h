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

#ifndef __ZThread_pthread_h__
#define __ZThread_pthread_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Thread_pthread
	#define ZCONFIG_API_Avail__Thread_pthread ZCONFIG_SPI_Enabled(pthread)
#endif

#ifndef ZCONFIG_API_Desired__Thread_pthread
	#define ZCONFIG_API_Desired__Thread_pthread 1
#endif

#if ZCONFIG_API_Enabled(Thread_pthread)

#ifndef ZCONFIG_pthread_Debug
	#if ZCONFIG_Debug
		#define ZCONFIG_pthread_Debug 1
	#endif
#endif

#ifndef ZCONFIG_pthread_Debug
	#define ZCONFIG_pthread_Debug 0
#endif

#include "zoolib/Compat_NonCopyable.h"
#include "zoolib/ZThread_T.h"

#include <errno.h> // For EINTR
#include <pthread.h>
#include <semaphore.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - ZThread_pthread

namespace ZThread_pthread {

typedef void* ProcResult_t;
typedef void* ProcParam_t;

typedef ProcResult_t (*ProcRaw_t)(ProcParam_t iParam);

typedef pthread_t ID;

void sStartRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam);

inline ID sID()
	{ return ::pthread_self(); }

void sSleep(double iDuration);

void sSetName(const char* iName);

} // namespace ZThread_pthread

// =================================================================================================
#pragma mark - ZTSS_pthread

namespace ZTSS_pthread {

typedef pthread_key_t Key;
typedef void* Value;

Key sCreate();
void sFree(Key iKey);

inline void sSet(Key iKey, Value iValue)
	{ ::pthread_setspecific(iKey, iValue); }

inline Value sGet(Key iKey)
	{ return ::pthread_getspecific(iKey); }

} // namespace ZTSS_pthread

// =================================================================================================
#pragma mark - ZMtx_pthread

class ZCndBase_pthread;

class ZMtx_pthread
:	NonCopyable
	{
public:
	#if ZCONFIG_pthread_Debug

		ZMtx_pthread();
		~ZMtx_pthread();
		void Acquire();
		void Release();

	#else

		inline ZMtx_pthread() { ::pthread_mutex_init(&f_pthread_mutex_t, nullptr); }

		inline ~ZMtx_pthread() { ::pthread_mutex_destroy(&f_pthread_mutex_t); }

		inline void Acquire() { ::pthread_mutex_lock(&f_pthread_mutex_t); }

		inline void Release() { ::pthread_mutex_unlock(&f_pthread_mutex_t); }

	#endif

protected:
	pthread_mutex_t f_pthread_mutex_t;
	friend class ZCndBase_pthread;
	};

// =================================================================================================
#pragma mark - ZCndBase_pthread

class ZCndBase_pthread
:	NonCopyable
	{
public:
	inline ZCndBase_pthread() { ::pthread_cond_init(&f_pthread_cond_t, nullptr); }

	inline ~ZCndBase_pthread() { ::pthread_cond_destroy(&f_pthread_cond_t); }

	inline void Wait(ZMtx_pthread& iMtx)
		{ ::pthread_cond_wait(&f_pthread_cond_t, &iMtx.f_pthread_mutex_t); }

	inline bool WaitFor(ZMtx_pthread& iMtx, double iTimeout)
		{ return this->pWaitFor(iMtx, iTimeout); }

	inline bool WaitUntil(ZMtx_pthread& iMtx, double iDeadline)
		{ return this->pWaitUntil(iMtx, iDeadline); }

	inline void Signal() { ::pthread_cond_signal(&f_pthread_cond_t); }

	inline void Broadcast() { ::pthread_cond_broadcast(&f_pthread_cond_t); }

protected:
	bool pWaitFor(ZMtx_pthread& iMtx, double iTimeout);

	bool pWaitUntil(ZMtx_pthread& iMtx, double iDeadline);

	pthread_cond_t f_pthread_cond_t;
	};

// =================================================================================================
#pragma mark - ZMtxChecked_pthread

typedef ZMtxChecked_T<ZMtx_pthread, ZCndBase_pthread, ZThread_pthread::ID, ZThread_pthread::sID>
	ZMtxChecked_pthread;

// =================================================================================================
#pragma mark - ZCndChecked_pthread

typedef ZCndChecked_T<ZMtxChecked_pthread, ZCndBase_pthread> ZCnd_pthread;

// =================================================================================================
#pragma mark - ZSem_pthread

typedef ZSem_T<ZMtx_pthread, ZCndBase_pthread> ZSem_pthread;

// =================================================================================================
#pragma mark - ZSemNoTimeout_pthread

#if !defined(__MACH__)
// Unnamed semaphores are not supported on OSX (or I presume on iOS).

class ZSemNoTimeout_pthread : NonCopyable
	{
public:
	inline ZSemNoTimeout_pthread() { ZEnsure(0 == ::sem_init(&f_sem_t, 0, 0)); }

	inline ~ZSemNoTimeout_pthread() { ::sem_destroy(&f_sem_t); }

	inline void Procure()
		{ while (EINTR == ::sem_wait(&f_sem_t)) {} }

	inline bool TryProcure()
		{ return 0 == ::sem_trywait(&f_sem_t); }

	inline void Vacate()
		{ ::sem_post(&f_sem_t); }

protected:
	sem_t f_sem_t;
	};

#endif // !defined(__MACH__)

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_pthread)

#endif // __ZThread_pthread_h__
