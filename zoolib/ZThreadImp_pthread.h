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
//#include "zoolib/ZTypes.h"

#include <pthread.h>
#include <semaphore.h>

namespace ZooLib {

class ZCnd_pthread;

// =================================================================================================
#pragma mark -
#pragma mark * ZTSS_pthread

class ZTSS_pthread
	{
	ZTSS_pthread();
public:
	typedef pthread_key_t Key;
	typedef void* Value;

	static Key sCreate();
	static void sFree(Key iKey);

	static void sSet(Key iKey, Value iValue);
	static Value sGet(Key iKey);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_pthread

class ZMtx_pthread : NonCopyable
	{
public:
	ZMtx_pthread();
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
#pragma mark * ZSem_pthread

class ZSem_pthread : NonCopyable
	{
public:
	ZSem_pthread();
	~ZSem_pthread();

	void Wait();
	bool TryWait();
	void Signal();

protected:
	sem_t fSem;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadImp_pthread

class ZThreadImp_pthread : NonCopyable
	{
private:
	ZThreadImp_pthread();

public:
	typedef void* (*Proc_t)(void* iParam);
	typedef pthread_t ID;

	static ID sCreate(size_t iStackSize, Proc_t iProc, void* iParam);
	static ID sID();
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(ThreadImp_pthread)

#endif // __ZThreadImp_pthread__
