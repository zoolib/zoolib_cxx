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
#pragma mark * ZCndBase_T

/*
"Implementing Condition Variables with Semaphores",
Andrew D. Birrell
<http://research.microsoft.com/apps/pubs/default.aspx?id=64242>

"Strategies for Implementing POSIX Condition Variables on Win32",
Douglas C. Schmidt and Irfan Pyarali
<http://www.cse.wustl.edu/~schmidt/win32-cv-1.html>
*/

template <class Mtx, class Sem>
class ZCndBase_T : NonCopyable
	{
private:
	Sem fSem;
	ZAtomic_t fWaitingThreads;

public:
	ZCndBase_T() : fWaitingThreads(0) {}
	~ZCndBase_T() {}

	void Wait(Mtx& iMtx)
		{
		ZAtomic_Inc(&fWaitingThreads);

		ZReleaser_T<Mtx> rel(iMtx);

		fSem.Procure();
		}

	bool WaitFor(Mtx& iMtx, double iTimeout)
		{
		ZAtomic_Inc(&fWaitingThreads);

		ZReleaser_T<Mtx> rel(iMtx);

		if (fSem.TryProcureFor(iTimeout))
			return true;

		ZAtomic_Dec(&fWaitingThreads);
		return false;
		}

	bool WaitUntil(Mtx& iMtx, ZTime iDeadline)
		{
		ZAtomic_Inc(&fWaitingThreads);

		ZReleaser_T<Mtx> rel(iMtx);

		if (fSem.TryProcureUntil(iDeadline))
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

				fSem.Vacate();
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
					fSem.Vacate();
				}
			break;
			}
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCndR_T

template <class MtxR, class Cnd>
class ZCndR_T : public Cnd
	{
public:
	void Wait(MtxR& iMtxR)
		{ iMtxR.pWait(*this); }

	bool WaitFor(MtxR& iMtxR, double iTimeout)
		{ return iMtxR.pWaitFor(*this, iTimeout); }

	bool WaitUntil(MtxR& iMtxR, ZTime iDeadline)
		{ return iMtxR.pWaitUntil(*this, iDeadline); }

	using Cnd::Wait;
	using Cnd::WaitFor;
	using Cnd::WaitUntil;
	using Cnd::Signal;
	using Cnd::Broadcast;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_T

template <class Sem>
class ZMtx_T : NonCopyable
	{
private:
	Sem fSem;

public:
	ZMtx_T(const char* iName = nullptr) { fSem.Signal(); }
	~ZMtx_T() {}

	void Acquire() { fSem.Procure(); }
	void Release() { fSem.Vacate(); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMtxR_T

template <class Mtx, class Cnd, class ThreadID, ThreadID (*GetThreadIDProc)()>
class ZMtxR_T : NonCopyable
	{
public:
	friend class ZCndR_T<ZMtxR_T, Cnd>;

	ZMtxR_T() : fOwner(0), fCount(0) {}

	~ZMtxR_T() {}

	void Acquire()
		{
		const ThreadID current = GetThreadIDProc();
		if (fOwner != current)
			{
			fMtx.Acquire();
			fOwner = current;
			ZAssert(fCount == 0);
			}
		++fCount;
		}

	void Release()
		{
		ZAssert(fOwner == GetThreadIDProc());

		if (0 == --fCount)
			{
			fOwner = 0;
			fMtx.Release();
			}
		}

private:
	void pWait(Cnd& iCnd)
		{
		const ThreadID current = GetThreadIDProc();
		ZAssert(fOwner == current);

		const int priorCount = fCount;
		fCount = 0;
		fOwner = 0;
		iCnd.Wait(fMtx);
		fOwner = current;
		fCount = priorCount;
		}

	bool pWaitFor(Cnd& iCnd, double iTimeout)
		{
		const ThreadID current = GetThreadIDProc();
		ZAssert(fOwner == current);

		const int priorCount = fCount;
		fCount = 0;
		fOwner = 0;
		bool result = iCnd.WaitFor(fMtx, iTimeout);
		fOwner = current;
		fCount = priorCount;
		return result;
		}

	bool pWaitUntil(Cnd& iCnd, ZTime iDeadline)
		{
		const ThreadID current = GetThreadIDProc();
		ZAssert(fOwner == current);

		const int priorCount = fCount;
		fCount = 0;
		fOwner = 0;
		bool result = iCnd.WaitUntil(fMtx, iDeadline);
		fOwner = current;
		fCount = priorCount;
		return result;
		}

	ThreadID fOwner;
	Mtx fMtx;
	int fCount;
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

	void Procure()
		{ this->pProcure(1); }

	bool TryProcureFor(double iTimeout)
		{ return this->pTryProcureUntil(1, ZTime::sSystem() + iTimeout); }

	bool TryProcureUntil(ZTime iDeadline)
		{ return this->pTryProcureUntil(1, iDeadline); }

	void Vacate() { this->pVacate(1); }

	void pProcure(int iCount)
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

	bool pTryProcureUntil(int iCount, ZTime iDeadline)
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
			this->pVacate(acquired);
			return false;
			}

		return true;
		}

	void pVacate(int iCount)
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
