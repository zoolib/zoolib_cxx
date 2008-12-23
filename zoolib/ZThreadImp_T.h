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

#ifndef __ZThreadImp_T__
#define __ZThreadImp_T__ 1
#include "zconfig.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZDList.h"
#include "zoolib/ZTime.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZGuard_T

template <class Mtx>
class ZGuard_T : NonCopyable
	{
public:
	ZGuard_T(Mtx& iMtx)
	:	fMtx(iMtx)
		{ fMtx.Acquire(); }

	ZGuard_T(const Mtx& iMtx)
	:	fMtx(const_cast<Mtx&>(iMtx))
		{ fMtx.Acquire(); }

	~ZGuard_T()
		{ fMtx.Release(); }

protected:
	Mtx& fMtx;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZGuardR_T

template <class Mtx>
class ZGuardR_T : NonCopyable
	{
public:
	ZGuardR_T(Mtx& iMtx)
	:	fMtx(iMtx),
		fCount(1)
		{ fMtx.Acquire(); }

	ZGuardR_T(const Mtx& iMtx)
	:	fMtx(const_cast<Mtx&>(iMtx)),
		fCount(1)
		{ fMtx.Acquire(); }

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

protected:
	Mtx& fMtx;
	int fCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSem_T

template <class Mtx, class Cnd>
class ZSem_T
	{
protected:
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
	ZSem_T()
	:	fAvailable(0)
		{}

	~ZSem_T() {}

	void Wait() { this->Imp_Wait(1); }
	bool Wait(double iTimeout) { return this->Imp_Wait(1, iTimeout); }
	void Signal() { this->Imp_Signal(1); }

	void Imp_Wait(int iCount)
		{
		fMtx.Acquire();

		if (fAvailable >= iCount)
			{
			fAvailable -= iCount;
			fMtx.Release();
			return;
			}

		Waiter theWaiter(iCount);
		fWaiters.PushBack(&theWaiter);

		while (theWaiter.fCount > 0)
			fCnd.Wait(fMtx);

		fWaiters.Remove(&theWaiter);

		fMtx.Release();
		}

	bool Imp_Wait(int iCount, double iTimeout)
		{
		ZTime expired = ZTime::sSystem() + iTimeout;
		fMtx.Acquire();

		if (fAvailable >= iCount)
			{
			fAvailable -= iCount;
			fMtx.Release();
			return true;
			}

		if (iTimeout <= 0)
			return false;

		Waiter theWaiter(iCount);
		fWaiters.PushBack(&theWaiter);

		while (theWaiter.fCount > 0 && expired > ZTime::sSystem())
			fCnd.Wait(fMtx, expired - ZTime::sSystem());

		fWaiters.Remove(&theWaiter);

		if (int acquired = iCount - theWaiter.fCount)
			{
			this->Imp_Signal(acquired);
			return false;
			}
		
		fMtx.Release();

		return true;
		}

	void Imp_Signal(int iCount)
		{
		fMtx.Acquire();
		this->pSignal(iCount);
		fMtx.Release();
		}

	void pSignal(int iCount)
		{
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


// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_T

template <class Mtx, class Sem>
class ZCnd_T
	{
public:
	ZCnd_T()
	:	fWaitingThreads(0)
		{}

	~ZCnd_T() {}

	void Wait(Mtx& iMtx) { this->Imp_Wait(iMtx); }
	void Wait(Mtx& iMtx, double iTimeout) { this->Imp_Wait(iMtx, iTimeout); }
	void Signal() { this->Imp_Signal(); }
	void Broadcast() { this->Imp_Broadcast(); }

	void Imp_Wait(Mtx& iMtx)
		{
		ZAtomic_Inc(&fWaitingThreads);

		iMtx.Release();

		fSem.Wait();

//		ZAtomic_Dec(&fWaitingThreads);
		
		iMtx.Acquire();
		}

	void Imp_Wait(Mtx& iMtx, double iTimeout)
		{
		ZAtomic_Inc(&fWaitingThreads);

		iMtx.Release();

		if (!fSem.Wait(iTimeout))
			ZAtomic_Dec(&fWaitingThreads);
		
		iMtx.Acquire();
		}

	void Imp_Signal()
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

	void Imp_Broadcast()
		{
		for (;;)
			{
			if (int oldCount = ZAtomic_Get(&fWaitingThreads))
				{
				if (!ZAtomic_CompareAndSwap(&fWaitingThreads, oldCount, 0))
					continue;

//				while (oldCount-- > 0)
				while (oldCount--)
					fSem.Signal();
				}
			break;
			}
		}

protected:
	Sem fSem;
	ZAtomic_t fWaitingThreads;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZThreadImp_T__
