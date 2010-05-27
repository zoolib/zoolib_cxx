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

#ifndef __ZThread_T__
#define __ZThread_T__ 1
#include "zconfig.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZDList.h"
#include "zoolib/ZTime.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZAcquirer_T

template <class Mtx>
class ZAcquirer_T : NonCopyable
	{
private:
	Mtx& fMtx;

public:
	ZAcquirer_T(Mtx& iMtx) : fMtx(iMtx) { fMtx.Acquire(); }
	ZAcquirer_T(const Mtx& iMtx) : fMtx(const_cast<Mtx&>(iMtx)) { fMtx.Acquire(); }
	~ZAcquirer_T() { fMtx.Release(); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZReleaser_T

template <class Mtx>
class ZReleaser_T : NonCopyable
	{
private:
	Mtx& fMtx;

public:
	ZReleaser_T(Mtx& iMtx) : fMtx(iMtx) { fMtx.Release(); }
	ZReleaser_T(const Mtx& iMtx) : fMtx(const_cast<Mtx&>(iMtx)) { fMtx.Release(); }
	~ZReleaser_T() { fMtx.Acquire(); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZGuardR_T

template <class Mtx>
class ZGuardR_T : NonCopyable
	{
private:
	Mtx& fMtx;
	int fCount;

public:
	ZGuardR_T(Mtx& iMtx) : fMtx(iMtx) , fCount(1) { fMtx.Acquire(); }
	ZGuardR_T(const Mtx& iMtx) : fMtx(const_cast<Mtx&>(iMtx)) , fCount(1) { fMtx.Acquire(); }

	~ZGuardR_T()
		{
		while (fCount--)
			fMtx.Release();
		}

	void Release()
		{
		--fCount;
		fMtx.Release();
		}

	void Acquire()
		{
		fMtx.Acquire();
		++fCount;
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_T

/*
"Implementing Condition Variables with Semaphores",
Andrew D. Birrell
<http://research.microsoft.com/apps/pubs/default.aspx?id=64242>

"Strategies for Implementing POSIX Condition Variables on Win32",
Douglas C. Schmidt and Irfan Pyarali
http://www.cse.wustl.edu/~schmidt/win32-cv-1.html
*/

template <class Mtx, class Sem>
class ZCnd_T : NonCopyable
	{
private:
	Sem fSem;
	ZAtomic_t fWaitingThreads;

public:
	ZCnd_T() : fWaitingThreads(0) {}
	~ZCnd_T() {}

	void Wait(Mtx& iMtx)
		{
		ZAtomic_Inc(&fWaitingThreads);

		ZReleaser_T<Mtx> rel(iMtx);

		fSem.Wait();
		}

	bool WaitFor(Mtx& iMtx, double iTimeout)
		{
		ZAtomic_Inc(&fWaitingThreads);

		ZReleaser_T<Mtx> rel(iMtx);

		if (fSem.WaitFor(iTimeout))
			return true;

		ZAtomic_Dec(&fWaitingThreads);
		return false;
		}

	bool WaitUntil(Mtx& iMtx, ZTime iDeadline)
		{
		ZAtomic_Inc(&fWaitingThreads);

		ZReleaser_T<Mtx> rel(iMtx);

		if (fSem.WaitUntil(iDeadline))
			return true;

		ZAtomic_Dec(&fWaitingThreads);
		return false;
		}

	void Signal()
		{
		for (;;)
			{
			if (int oldCount = ZAtomic_Get(&fWaitingThreads))
				{
				if (!ZAtomic_CompareAndSwap(&fWaitingThreads, oldCount, oldCount - 1))
					continue;

				fSem.Signal();
				}
			break;
			}
		}

	void Broadcast()
		{
		for (;;)
			{
			if (int oldCount = ZAtomic_Get(&fWaitingThreads))
				{
				if (!ZAtomic_CompareAndSwap(&fWaitingThreads, oldCount, 0))
					continue;

				while (oldCount--)
					fSem.Signal();
				}
			break;
			}
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_Win

template <class Sem>
class ZMtx_T : NonCopyable
	{
private:
	Sem fSem;

public:
	ZMtx_T(const char* iName = nullptr) { fSem.Signal(); }
	~ZMtx_T() {}

	void Acquire() { fSem.Wait(); }
	void Release() { fSem.Signal(); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSem_T

template <class Mtx, class Cnd>
class ZSem_T : NonCopyable
	{
private:
	Mtx fMtx;
	Cnd fCnd;

	int fAvailable;

	class Waiter;
	class DLink_Waiter
	:	public DListLink<Waiter, DLink_Waiter, 1>
		{};

	DListHead<DLink_Waiter> fWaiters;

	class Waiter
	:	public DLink_Waiter
		{
	public:
		Waiter(int iCount) : fCount(iCount) {}
		int fCount;
		};

public:
	ZSem_T() : fAvailable(0) {}
	~ZSem_T() {}

	void Wait() { this->pWait(1); }
	bool WaitFor(double iTimeout) { return this->pWaitUntil(1, ZTime::sSystem() + iTimeout); }
	bool WaitUntil(ZTime iDeadline) { return this->pWaitUntil(1, iDeadline); }
	void Signal() { this->pSignal(1); }

	void pWait(int iCount)
		{
		ZAcquirer_T<Mtx> acq(fMtx);

		if (fAvailable)
			{
			--fAvailable;
			return;
			}

		Waiter theWaiter(iCount);
		fWaiters.PushBack(&theWaiter);

		while (theWaiter.fCount > 0)
			fCnd.Wait(fMtx);

		fWaiters.Erase(&theWaiter);
		}

	bool pWaitUntil(int iCount, ZTime iDeadline)
		{
		ZAcquirer_T<Mtx> acq(fMtx);

		if (fAvailable >= iCount)
			{
			fAvailable -= iCount;
			return true;
			}

		Waiter theWaiter(iCount);
		fWaiters.PushBack(&theWaiter);

		while (theWaiter.fCount > 0 && fCnd.WaitUntil(fMtx, iDeadline))
			{}

		fWaiters.Erase(&theWaiter);

		if (int acquired = iCount - theWaiter.fCount)
			{
			this->pSignal(acquired);
			return false;
			}
		
		return true;
		}

	void pSignal(int iCount)
		{
		ZAcquirer_T<Mtx> acq(fMtx);

		if (!fWaiters)
			{
			fAvailable += iCount;
			}
		else
			{
			for (DListIterator<Waiter, DLink_Waiter> i = fWaiters; i && iCount > 0; i.Advance())
				{
				Waiter* current = i.Current();
				if (iCount >= current->fCount)
					{
					iCount -= current->fCount;
					current->fCount = 0;
					}
				else
					{
					current->fCount -= iCount;
					iCount = 0;
					}
				}

			fAvailable += iCount;

			fCnd.Broadcast();
			}
		}
	};

} // namespace ZooLib

#endif // __ZThread_T__
