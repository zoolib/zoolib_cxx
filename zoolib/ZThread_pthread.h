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

#ifndef __ZThread_pthread__
#define __ZThread_pthread__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Thread_pthread
#	define ZCONFIG_API_Avail__Thread_pthread ZCONFIG_SPI_Enabled(pthread)
#endif

#ifndef ZCONFIG_API_Desired__Thread_pthread
#	define ZCONFIG_API_Desired__Thread_pthread 1
#endif

#if ZCONFIG_API_Enabled(Thread_pthread)

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThread_T.h"

#include <pthread.h>
#include <semaphore.h>

NAMESPACE_ZOOLIB_BEGIN

class ZCnd_pthread;
class ZMtx_pthread;

// =================================================================================================
#pragma mark -
#pragma mark * ZTSS_pthread

namespace ZTSS_pthread {

typedef pthread_key_t Key;
typedef const void* Value;

Key sCreate();
void sFree(Key iKey);

void sSet(Key iKey, Value iValue);
inline Value sGet(Key iKey)
	{ return ::pthread_getspecific(iKey); }

} // namespace ZTSS_pthread

// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_pthread

class ZCnd_pthread : NonCopyable
	{
public:
	ZCnd_pthread();
	~ZCnd_pthread();

	void Wait(ZMtx_pthread& iMtx);
	bool WaitFor(ZMtx_pthread& iMtx, double iTimeout);
	bool WaitUntil(ZMtx_pthread& iMtx, ZTime iDeadline);
	void Signal();
	void Broadcast();

protected:
	pthread_cond_t fCond;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_pthread

class ZMtx_pthread : NonCopyable
	{
public:
	ZMtx_pthread(const char* iName = nullptr) { ::pthread_mutex_init(&fMutex, nullptr); }
	~ZMtx_pthread() { ::pthread_mutex_destroy(&fMutex); }

	void Acquire() { ::pthread_mutex_lock(&fMutex); }
	void Release() { ::pthread_mutex_unlock(&fMutex); }

protected:
	pthread_mutex_t fMutex;
	friend class ZCnd_pthread;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_pthread inlines

inline ZCnd_pthread::ZCnd_pthread() { ::pthread_cond_init(&fCond, nullptr); }
inline ZCnd_pthread::~ZCnd_pthread() { ::pthread_cond_destroy(&fCond); }

inline void ZCnd_pthread::Wait(ZMtx_pthread& iMtx) { ::pthread_cond_wait(&fCond, &iMtx.fMutex); }
inline void ZCnd_pthread::Signal() { ::pthread_cond_signal(&fCond); }
inline void ZCnd_pthread::Broadcast() { ::pthread_cond_broadcast(&fCond); }

// =================================================================================================
#pragma mark -
#pragma mark * ZSem_pthread

typedef ZSem_T<ZMtx_pthread, ZCnd_pthread> ZSem_pthread;

// =================================================================================================
#pragma mark -
#pragma mark * ZSemNoTimeout_pthread

class ZSemNoTimeout_pthread : NonCopyable
	{
public:
	ZSemNoTimeout_pthread();
	~ZSemNoTimeout_pthread();

	void Wait();
	bool TryWait();
	void Signal();

protected:
	sem_t fSem;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZThread_pthread

namespace ZThread_pthread {

typedef void* ProcResult_t;
typedef void* ProcParam_t;

typedef ProcResult_t (*ProcRaw_t)(ProcParam_t iParam);

typedef pthread_t ID;

void sCreateRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam);
ID sID();
void sSleep(double iDuration);

} // namespace ZThread_pthread

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Thread_pthread)

#endif // __ZThread_pthread__
