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

#include "zoolib/ZThreadOld.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZTime.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZMutex

ZMutex::ZMutex(const char* iName, bool iCreateAcquired)
	{
	if (iCreateAcquired)
		{
		fThreadID_Owner = ZThread::sID();
		fCount = 1;
		fMtx.Acquire();
		}
	else
		{
		fThreadID_Owner = 0;
		fCount = 0;
		}
	}
	
ZMutex::~ZMutex()
	{}

void ZMutex::Acquire()
	{
	const ZThread::ID current = ZThread::sID();
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
	{ return ZThread::sID() == fThreadID_Owner; }

void ZMutex::pWait(ZCnd& iCnd)
	{
	int priorCount = fCount;
	fCount = 0;
	fThreadID_Owner = 0;
	iCnd.Wait(fMtx);
	fThreadID_Owner = ZThread::sID();
	fCount = priorCount;
	}

void ZMutex::pWait(ZCnd& iCnd, double iTimeout)
	{
	int priorCount = fCount;
	fCount = 0;
	fThreadID_Owner = 0;
	iCnd.Wait(fMtx, iTimeout);
	fThreadID_Owner = ZThread::sID();
	fCount = priorCount;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZSemaphore

ZSemaphore::ZSemaphore(int32 iInitialCount)
	{
	while (iInitialCount--)
		ZSem::Signal();
	}

ZSemaphore::ZSemaphore(int32 iInitialCount, const char* iName)
	{
	while (iInitialCount--)
		ZSem::Signal();
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

NAMESPACE_ZOOLIB_END
