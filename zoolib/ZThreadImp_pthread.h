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

#ifndef __ZThreadImp_pthread__
#define __ZThreadImp_pthread__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__ThreadImp_pthread
#	define ZCONFIG_API_Avail__ThreadImp_pthread ZCONFIG_SPI_Enabled(pthread)
#endif

#ifndef ZCONFIG_API_Desired__ThreadImp_pthread
#	define ZCONFIG_API_Desired__ThreadImp_pthread 1
#endif

#if ZCONFIG_API_Enabled(ThreadImp_pthread)

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThreadImp_T.h"

#include <pthread.h>
#include <semaphore.h>

NAMESPACE_ZOOLIB_BEGIN

class ZCnd_pthread;

// =================================================================================================
#pragma mark -
#pragma mark * ZTSS_pthread

namespace ZTSS_pthread {

typedef pthread_key_t Key;
typedef void* Value;

Key sCreate();
void sFree(Key iKey);

void sSet(Key iKey, Value iValue);
Value sGet(Key iKey);

} // namespace ZTSS_pthread

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_pthread

class ZMtx_pthread : NonCopyable
	{
public:
	ZMtx_pthread(const char* iName = nullptr);
	~ZMtx_pthread();

	void Acquire();
	void Release();

protected:
	pthread_mutex_t fMutex;
	friend class ZCnd_pthread;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_pthread

class ZCnd_pthread : NonCopyable
	{
public:
	ZCnd_pthread();

	~ZCnd_pthread();

	void Wait(ZMtx_pthread& iMtx);
	void Wait(ZMtx_pthread& iMtx, double iTimeout);
	void Signal();
	void Broadcast();

protected:
	pthread_cond_t fCond;
	};

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
#pragma mark * ZSem_pthread

typedef ZSem_T<ZMtx_pthread, ZCnd_pthread> ZSem_pthread;

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadImp_pthread

namespace ZThreadImp_pthread {

typedef void* ProcResult_t;
typedef void* ProcParam_t;

typedef ProcResult_t (*Proc_t)(ProcParam_t iParam);

typedef pthread_t ID;

ID sCreate(size_t iStackSize, Proc_t iProc, void* iParam);
ID sID();
void sSleep(double iDuration);

} // namespace ZThreadImp_pthread

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(ThreadImp_pthread)

#endif // __ZThreadImp_pthread__
