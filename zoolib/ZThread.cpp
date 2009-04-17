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

#include "zoolib/ZThread.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZTime.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZThread

ZThread::ThreadID ZThread::kThreadID_None = 0;

ZThread::ZThread(const char* iName)
:	fStarted(false)
	{
	fMtx_Start = new ZMtx;
	fCnd_Start = new ZCnd;
	ZThreadImp::sCreate(0, spRun, this);
	}

void ZThread::Start()
	{
	ZGuard_T<ZMtx> locker(*fMtx_Start);
	fStarted = true;
	fCnd_Start->Broadcast();
	}

void ZThread::pRun()
	{
	try
		{
		{
		ZGuard_T<ZMtx> locker(*fMtx_Start);
		while (!fStarted)
			fCnd_Start->Wait(*fMtx_Start);
		}
		delete fMtx_Start;
		delete fCnd_Start;

		this->Run();
		}
	catch (std::exception& ex)
		{
		if (ZLOG(s, eNotice, "ZThread"))
			s << "pRun, uncaught exception: " << ex.what();
		}
	catch (...)
		{
		if (ZLOG(s, eNotice, "ZThread::pRun"))
			s << "pRun, uncaught exception, not derived fron std::exception";
		}

	delete this;
	}

ZThreadImp::ProcResult_t ZThread::spRun(ZThreadImp::ProcParam_t iParam)
	{
	static_cast<ZThread*>(iParam)->pRun();
	return 0;
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
