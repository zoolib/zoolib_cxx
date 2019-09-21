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

#ifndef __ZThread_T_h__
#define __ZThread_T_h__ 1
#include "zconfig.h"

#include "zoolib/Atomic.h"
#include "zoolib/Compat_NonCopyable.h"
#include "zoolib/DList.h"
#include "zoolib/Time.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ZAcquirer_T

template <class Mtx>
class ZAcquirer_T : NonCopyable
	{
public:
	inline ZAcquirer_T(Mtx& iMtx) : fMtx(iMtx) { fMtx.Acquire(); }

	inline ZAcquirer_T(const Mtx& iMtx) : fMtx(const_cast<Mtx&>(iMtx)) { fMtx.Acquire(); }

	inline ~ZAcquirer_T() { fMtx.Release(); }

private:
	Mtx& fMtx;
	};

// =================================================================================================
#pragma mark - ZReleaser_T

template <class Mtx>
class ZReleaser_T : NonCopyable
	{
public:
	inline ZReleaser_T(Mtx& iMtx) : fMtx(iMtx) { fMtx.Release(); }

	inline ZReleaser_T(const Mtx& iMtx) : fMtx(const_cast<Mtx&>(iMtx)) { fMtx.Release(); }

	inline ~ZReleaser_T() { fMtx.Acquire(); }

private:
	Mtx& fMtx;
	};

// =================================================================================================
#pragma mark - ZCndBase_T

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
public:
	inline ZCndBase_T() : fWaitingThreads(0) {}

	inline ~ZCndBase_T() {}

	inline void Wait(Mtx& iMtx)
		{
		sAtomic_Inc(&fWaitingThreads);

		ZReleaser_T<Mtx> rel(iMtx);

		fSem.Procure();
		}

	inline bool WaitFor(Mtx& iMtx, double iTimeout)
		{
		sAtomic_Inc(&fWaitingThreads);

		ZReleaser_T<Mtx> rel(iMtx);

		if (fSem.TryProcureFor(iTimeout))
			return true;

		sAtomic_Dec(&fWaitingThreads);
		return false;
		}

	inline bool WaitUntil(Mtx& iMtx, double iDeadline)
		{
		sAtomic_Inc(&fWaitingThreads);

		ZReleaser_T<Mtx> rel(iMtx);

		if (fSem.TryProcureUntil(iDeadline))
			return true;

		sAtomic_Dec(&fWaitingThreads);
		return false;
		}

	inline void Signal()
		{
		for (;;)
			{
			if (int oldCount = sAtomic_Get(&fWaitingThreads))
				{
				if (not sAtomic_CAS(&fWaitingThreads, oldCount, oldCount - 1))
					continue;

				fSem.Vacate();
				}
			break;
			}
		}

	inline void Broadcast()
		{
		for (;;)
			{
			if (int oldCount = sAtomic_Get(&fWaitingThreads))
				{
				if (not sAtomic_CAS(&fWaitingThreads, oldCount, 0))
					continue;

				while (oldCount--)
					fSem.Vacate();
				}
			break;
			}
		}

private:
	Sem fSem;
	ZAtomic_t fWaitingThreads;
	};

// =================================================================================================
#pragma mark - ZCndChecked_T

template <class MtxChecked, class Cnd>
class ZCndChecked_T : public Cnd
	{
public:
	inline void Wait(MtxChecked& iChecked)
		{ iChecked.pWait(*this); }

	inline bool WaitFor(MtxChecked& iChecked, double iTimeout)
		{ return iChecked.pWaitFor(*this, iTimeout); }

	inline bool WaitUntil(MtxChecked& iChecked, double iDeadline)
		{ return iChecked.pWaitUntil(*this, iDeadline); }

	using Cnd::Wait;
	using Cnd::WaitFor;
	using Cnd::WaitUntil;
	using Cnd::Signal;
	using Cnd::Broadcast;
	};

// =================================================================================================
#pragma mark - ZMtx_T

template <class Sem>
class ZMtx_T : NonCopyable
	{
public:
	inline ZMtx_T(const char* iName = nullptr) { fSem.Signal(); }

	inline ~ZMtx_T() {}

	inline void Acquire() { fSem.Procure(); }

	inline void Release() { fSem.Vacate(); }

private:
	Sem fSem;
	};

// =================================================================================================
#pragma mark - ZMtxChecked_T

template <class Mtx, class Cnd, class ThreadID, ThreadID (*GetThreadIDProc)(void)>
class ZMtxChecked_T : NonCopyable
	{
public:
	friend class ZCndChecked_T<ZMtxChecked_T, Cnd>;

	inline ZMtxChecked_T() : fOwner(0) {}

	inline ~ZMtxChecked_T() {}

	inline void Acquire()
		{
		const ThreadID current = GetThreadIDProc();
		ZAssert(fOwner != current);
		fMtx.Acquire();
		fOwner = current;
		}

	inline void Release()
		{
		ZAssert(fOwner == GetThreadIDProc());
		fOwner = 0;
		fMtx.Release();
		}

	void AssertOwned()
		{ this->pAssertOwned(GetThreadIDProc()); }

private:
	void pAssertOwned(ThreadID current)
		{
		if (fOwner != current)
			{
			//##fprintf(stderr, "fOwner = %X, current = %X\n", (int)fOwner, (int)current);
			ZAssert(false);
			}
		}

	inline void pWait(Cnd& iCnd)
		{
		const ThreadID current = GetThreadIDProc();
		this->pAssertOwned(current);
		fOwner = 0;
		iCnd.Wait(fMtx);
		fOwner = current;
		}

	inline bool pWaitFor(Cnd& iCnd, double iTimeout)
		{
		const ThreadID current = GetThreadIDProc();
		this->pAssertOwned(current);
		fOwner = 0;
		bool result = iCnd.WaitFor(fMtx, iTimeout);
		fOwner = current;
		return result;
		}

	inline bool pWaitUntil(Cnd& iCnd, double iDeadline)
		{
		const ThreadID current = GetThreadIDProc();
		this->pAssertOwned(current);
		fOwner = 0;
		bool result = iCnd.WaitUntil(fMtx, iDeadline);
		fOwner = current;
		return result;
		}

	ThreadID fOwner;
	Mtx fMtx;
	};

// =================================================================================================
#pragma mark - ZSem_T

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
		inline Waiter(int iCount) : fCount(iCount) {}
		int fCount;
		};

public:
	inline ZSem_T() : fAvailable(0) {}

	inline ~ZSem_T() {}

	inline void Procure()
		{ this->pProcure(1); }

	inline bool TryProcureFor(double iTimeout)
		{ return this->pTryProcureUntil(1, Time::sSystem() + iTimeout); }

	inline bool TryProcureUntil(double iDeadline)
		{ return this->pTryProcureUntil(1, iDeadline); }

	inline void Vacate() { this->pVacate(1); }

	inline void pProcure(int iCount)
		{
		ZAcquirer_T<Mtx> acq(fMtx);

		if (fAvailable)
			{
			--fAvailable;
			return;
			}

		Waiter theWaiter(iCount);
		sInsertBackMust(fWaiters, &theWaiter);

		while (theWaiter.fCount > 0)
			fCnd.Wait(fMtx);

		sEraseMust(fWaiters, &theWaiter);
		}

	inline bool pTryProcureUntil(int iCount, double iDeadline)
		{
		ZAcquirer_T<Mtx> acq(fMtx);

		if (fAvailable >= iCount)
			{
			fAvailable -= iCount;
			return true;
			}

		Waiter theWaiter(iCount);
		sInsertBackMust(fWaiters, &theWaiter);

		while (theWaiter.fCount > 0 && fCnd.WaitUntil(fMtx, iDeadline))
			{}

		sEraseMust(fWaiters, &theWaiter);

		if (int acquired = iCount - theWaiter.fCount)
			{
			this->pVacate(acquired);
			return false;
			}

		return true;
		}

	inline void pVacate(int iCount)
		{
		ZAcquirer_T<Mtx> acq(fMtx);

		if (not fWaiters)
			{
			fAvailable += iCount;
			}
		else
			{
			for (DListIterator<Waiter, DLink_Waiter> ii = fWaiters; ii && iCount > 0; ii.Advance())
				{
				Waiter* current = ii.Current();
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

#endif // __ZThread_T_h__
