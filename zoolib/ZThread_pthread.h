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


#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThread_T.h"

#include <errno.h> // For EINTR
#include <pthread.h>
#include <semaphore.h>

namespace ZooLib {

// =================================================================================================
// MARK: - ZThread_pthread

namespace ZThread_pthread {

typedef void* ProcResult_t;
typedef void* ProcParam_t;

typedef ProcResult_t (*ProcRaw_t)(ProcParam_t iParam);

typedef pthread_t ID;

void sCreateRaw(size_t iStackSize, ProcRaw_t iProc, void* iParam);

ZMACRO_Attribute_NoThrow
inline
ID sID()
	{ return ::pthread_self(); }

ZMACRO_Attribute_NoThrow
void sSleep(double iDuration);

} // namespace ZThread_pthread

// =================================================================================================
// MARK: - ZTSS_pthread

namespace ZTSS_pthread {

typedef pthread_key_t Key;
typedef const void* Value;

Key sCreate();
void sFree(Key iKey);

ZMACRO_Attribute_NoThrow
inline
void sSet(Key iKey, Value iValue)
	{ ::pthread_setspecific(iKey, iValue); }

ZMACRO_Attribute_NoThrow
inline
Value sGet(Key iKey)
	{ return ::pthread_getspecific(iKey); }

} // namespace ZTSS_pthread

// =================================================================================================
// MARK: - ZCnd_pthread

class ZCnd_pthread
:	public pthread_cond_t
,	NonCopyable
	{
public:
	ZMACRO_Attribute_NoThrow
	inline
	ZCnd_pthread() { ::pthread_cond_init(this, nullptr); }

	ZMACRO_Attribute_NoThrow
	inline
	~ZCnd_pthread() { ::pthread_cond_destroy(this); }

	ZMACRO_Attribute_NoThrow
	inline
	void Wait(pthread_mutex_t& iMtx) { ::pthread_cond_wait(this, &iMtx); }

	ZMACRO_Attribute_NoThrow
	inline
	bool WaitFor(pthread_mutex_t& iMtx, double iTimeout)
		{ return this->pWaitFor(iMtx, iTimeout); }

	ZMACRO_Attribute_NoThrow
	inline
	bool WaitUntil(pthread_mutex_t& iMtx, ZTime iDeadline)
		{ return this->pWaitUntil(iMtx, iDeadline); }

	ZMACRO_Attribute_NoThrow
	inline
	void Signal() { ::pthread_cond_signal(this); }

	ZMACRO_Attribute_NoThrow
	inline
	void Broadcast() { ::pthread_cond_broadcast(this); }

protected:
	ZMACRO_Attribute_NoThrow
	bool pWaitFor(pthread_mutex_t& iMtx, double iTimeout);

	ZMACRO_Attribute_NoThrow
	bool pWaitUntil(pthread_mutex_t& iMtx, ZTime iDeadline);
	};

// =================================================================================================
// MARK: - ZMtx_pthread_base

class ZMtx_pthread_base
:	public pthread_mutex_t
,	NonCopyable
	{
public:
	#if ZCONFIG_pthread_Debug

		~ZMtx_pthread_base();
		void Acquire();
		void Release();

	#else

		ZMACRO_Attribute_NoThrow
		inline
		~ZMtx_pthread_base() { ::pthread_mutex_destroy(this); }

		ZMACRO_Attribute_NoThrow
		inline
		void Acquire() { ::pthread_mutex_lock(this); }

		ZMACRO_Attribute_NoThrow
		inline
		void Release() { ::pthread_mutex_unlock(this); }

	#endif
	};

// =================================================================================================
// MARK: - ZMtx_pthread

class ZMtx_pthread : public ZMtx_pthread_base
	{
public:
	#if ZCONFIG_pthread_Debug

		ZMtx_pthread();

	#else

		ZMACRO_Attribute_NoThrow
		inline
		ZMtx_pthread() { ::pthread_mutex_init(this, nullptr); }

	#endif
	};

// =================================================================================================
// MARK: - ZMtxR_pthread

class ZMtxR_pthread : public ZMtx_pthread_base
	{
public:
	ZMACRO_Attribute_NoThrow
	ZMtxR_pthread();
	};

// =================================================================================================
// MARK: - ZSem_pthread

typedef ZSem_T<ZMtx_pthread, ZCnd_pthread> ZSem_pthread;

// =================================================================================================
// MARK: - ZSemNoTimeout_pthread

class ZSemNoTimeout_pthread : NonCopyable
	{
public:
	ZMACRO_Attribute_NoThrow
	inline
	ZSemNoTimeout_pthread() { ::sem_init(&fSem, 0, 0); }

	ZMACRO_Attribute_NoThrow
	inline 
	~ZSemNoTimeout_pthread() { ::sem_destroy(&fSem); }

	ZMACRO_Attribute_NoThrow
	inline 
	void Procure()
		{ while (EINTR == ::sem_wait(&fSem)) {} }

	ZMACRO_Attribute_NoThrow
	inline
	bool TryProcure()
		{ return 0 == ::sem_trywait(&fSem); }

	ZMACRO_Attribute_NoThrow
	inline
	void Vacate()
		{ ::sem_post(&fSem); }

protected:
	sem_t fSem;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_pthread)

#endif // __ZThread_pthread_h__
