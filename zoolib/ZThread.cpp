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

using namespace ZooLib;

//using std::exception;
//using std::min;

ZThread::ThreadID ZThread::kThreadID_None;

// =================================================================================================
#pragma mark -
#pragma mark * kDebug

#define kDebug_Thread 1

// =================================================================================================
#pragma mark -
#pragma mark * ZThread

ZThread::ZThread(const char* iName)
:	fStarted(false)
	{}

void ZThread::Start()
	{
	ZGuard_T<ZMtx> locker(fMtx_Start);
	while (!fStarted)
		fCnd_Start.Wait(fMtx_Start);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZSemaphore

ZSemaphore::ZSemaphore(int32 iInitialCount)
	{
	while (iInitialCount--)
		ZSem::Wait();
	}

ZSemaphore::ZSemaphore(int32 iInitialCount, const char* iName)
	{
	while (iInitialCount--)
		ZSem::Wait();
	}

ZSemaphore::~ZSemaphore()
	{}

void ZSemaphore::Wait(int32 iCount)
	{
	while (iCount--)
		ZSem::Wait();
	}

bool ZSemaphore::Wait(int32 iCount, bigtime_t iMicroseconds)
	{
	ZTime expired = ZTime::sSystem() + iMicroseconds / 1e6;
	int32 acquired = 0;
	while (iCount)
		{
		if (!ZSem::Wait(expired - ZTime::sSystem()))
			{
			this->Signal(acquired);
			return false;
			}
		else
			{
			--iCount;
			++acquired;
			}
		}
	return true;
	}

void ZSemaphore::Signal(int32 iCount)
	{
	while (iCount--)
		ZSem::Signal();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMutex

ZMutex::ZMutex(const char* iName, bool iCreateAcquired)
	{
	if (iCreateAcquired)
		{
		fThreadID_Owner = ZThreadImp::sID();
		fCount = 1;
		fMtx.Acquire();
		}
	}
	
ZMutex::~ZMutex()
	{}

void ZMutex::Acquire()
	{
	const ZThreadImp::ID current = ZThreadImp::sID();
	if (current != fThreadID_Owner)
		{
		fMtx.Acquire();
		fThreadID_Owner = current;
		}
	++fCount;
	}

void ZMutex::Release()
	{
	if (0 == --fCount)
		{
		fThreadID_Owner = 0;
		fMtx.Release();
		}
	}

bool ZMutex::IsLocked()
	{ return ZThreadImp::sID() == fThreadID_Owner; }

void ZMutex::pWait(ZCnd& iCnd)
	{
	int priorCount = fCount;
	fCount = 0;
	fThreadID_Owner = 0;
	iCnd.Wait(fMtx);
	fThreadID_Owner = ZThreadImp::sID();
	fCount = priorCount;
	}

void ZMutex::pWait(ZCnd& iCnd, double iTimeout)
	{
	int priorCount = fCount;
	fCount = 0;
	fThreadID_Owner = 0;
	iCnd.Wait(fMtx, iTimeout);
	fThreadID_Owner = ZThreadImp::sID();
	fCount = priorCount;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZCondition

void ZCondition::Wait(ZMutex& iMutex)
	{ iMutex.pWait(fCnd); }

void ZCondition::Wait(ZMutex& iMutex, bigtime_t iMicroseconds)
	{ iMutex.pWait(fCnd, iMicroseconds / 1e6); }

void ZCondition::Wait(ZMtx& iMtx)
	{ fCnd.Wait(iMtx); }

void ZCondition::Wait(ZMtx& iMtx, bigtime_t iMicroseconds)
	{ fCnd.Wait(iMtx, iMicroseconds / 1e6); }

void ZCondition::Signal()
	{ fCnd.Signal(); }

void ZCondition::Broadcast()
	{ fCnd.Broadcast(); }
