/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZWaiter.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZWaiterRunner

ZWaiterRunner::ZWaiterRunner()
	{}

ZWaiterRunner::~ZWaiterRunner()
	{}

void ZWaiterRunner::pAttachWaiter(ZRef<ZWaiter> iWaiter)
	{
	ZAssert(iWaiter);
	ZAssert(!iWaiter->fRunner.Use());

	iWaiter->fRunner = ZRef<ZWaiterRunner>(this);

	iWaiter->pRunnerAttached();
	}

void ZWaiterRunner::pDetachWaiter(ZRef<ZWaiter> iWaiter)
	{
	ZAssert(iWaiter);
	ZAssert(iWaiter->fRunner.Use() == this);

	iWaiter->fRunner.Clear();

	iWaiter->pRunnerDetached();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZWaiterRunner_Threaded

class ZWaiterRunner_Threaded : public ZWaiterRunner
	{
public:
	ZWaiterRunner_Threaded(ZRef<ZWaiter> iWaiter);
	virtual ~ZWaiterRunner_Threaded();

	void Start();

// From ZWaiterRunner
	virtual void Waiter_WakeAt(ZRef<ZWaiter> iWaiter, ZTime iSystemTime);

private:
	void pRun();
	static ZThreadImp::ProcResult_t spRun(void* iRefcon);

	ZMtx fMtx;
	ZCnd fCnd;
	ZRef<ZWaiter> fWaiter;
	ZTime fNextWake;
	ZRef<ZWaiterRunner_Threaded> fSelf;
	};

ZWaiterRunner_Threaded::ZWaiterRunner_Threaded(ZRef<ZWaiter> iWaiter)
:	fWaiter(iWaiter),
	fNextWake(ZTime::sSystem())
	{}

ZWaiterRunner_Threaded::~ZWaiterRunner_Threaded()
	{}

void ZWaiterRunner_Threaded::Start()
	{
	fSelf = this;
	ZRef<ZWaiterRunner_Threaded> self = this;
	this->pAttachWaiter(fWaiter);
	ZThreadImp::sCreate(0, spRun, this);
	}

void ZWaiterRunner_Threaded::Waiter_WakeAt(ZRef<ZWaiter> iWaiter, ZTime iSystemTime)
	{
	ZGuardMtx locker(fMtx);
	ZAssert(iWaiter == fWaiter);
	if (fNextWake > iSystemTime)
		{
		fNextWake = iSystemTime;
		fCnd.Broadcast();
		}
	}

void ZWaiterRunner_Threaded::pRun()
	{
	for (;;)
		{
		{
		ZGuardMtx locker(fMtx);
		for (;;)
			{
			const ZTime now = ZTime::sSystem();
			if (fNextWake <= now)
				{
				fNextWake = now + 3600;
				break;
				}

			fCnd.Wait(fMtx, fNextWake - now);
			}
		}

		try
			{
			if (!fWaiter->Execute())
				break;
			}
		catch (...)
			{
			break;
			}
		}

	this->pDetachWaiter(fWaiter);
	fWaiter.Clear();
	fSelf.Clear();
	}

ZThreadImp::ProcResult_t ZWaiterRunner_Threaded::spRun(void* iRefcon)
	{
	static_cast<ZWaiterRunner_Threaded*>(iRefcon)->pRun();
	return ZThreadImp::ProcResult_t();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZWaiter

ZWaiter::ZWaiter()
	{}

void ZWaiter::RunnerAttached()
	{}

void ZWaiter::RunnerDetached()
	{}

void ZWaiter::Wake()
	{
	if (ZRef<ZWaiterRunner> theRunner = fRunner.Use())
		theRunner->Waiter_WakeAt(this, ZTime::sSystem());
	}

void ZWaiter::WakeAt(ZTime iSystemTime)
	{
	if (ZRef<ZWaiterRunner> theRunner = fRunner.Use())
		theRunner->Waiter_WakeAt(this, iSystemTime);
	}

void ZWaiter::pRunnerAttached()
	{ this->RunnerAttached(); }

void ZWaiter::pRunnerDetached()
	{ this->RunnerDetached(); }

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

void sStartWaiterRunner(ZRef<ZWaiter> iWaiter)
	{
	ZRef<ZWaiterRunner_Threaded> theRunner = new ZWaiterRunner_Threaded(iWaiter);
	theRunner->Start();
	}

NAMESPACE_ZOOLIB_END
