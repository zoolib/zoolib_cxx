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

#include "zoolib/ZActor.h"
#include "zoolib/ZThread.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZActorRunner

ZActorRunner::ZActorRunner()
	{}

ZActorRunner::~ZActorRunner()
	{}

void ZActorRunner::pAttachActor(ZRef<ZActor> iActor)
	{
	ZAssert(iActor);
	ZAssert(!ZRef<ZActorRunner>(iActor->fRunner));

	iActor->fRunner = ZRef<ZActorRunner>(this);

	iActor->pRunnerAttached();
	}

void ZActorRunner::pDetachActor(ZRef<ZActor> iActor)
	{
	ZAssert(iActor);
	ZAssert(ZRef<ZActorRunner>(iActor->fRunner) == this);

	iActor->fRunner.Clear();

	iActor->pRunnerDetached();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZActorRunner_Threaded

class ZActorRunner_Threaded : public ZActorRunner
	{
public:
	ZActorRunner_Threaded(ZRef<ZActor> iActor);
	virtual ~ZActorRunner_Threaded();

	void Start();

// From ZActorRunner
	virtual void Actor_Wake(ZRef<ZActor> iActor);
	virtual void Actor_WakeAt(ZRef<ZActor> iActor, ZTime iSystemTime);
	virtual void Actor_WakeIn(ZRef<ZActor> iActor, double iInterval);

private:
	void pRun();
	static void spRun(ZRef<ZActorRunner_Threaded> iParam);

	ZMtx fMtx;
	ZCnd fCnd;
	ZRef<ZActor> fActor;
	ZTime fNextWake;
	};

ZActorRunner_Threaded::ZActorRunner_Threaded(ZRef<ZActor> iActor)
:	fActor(iActor),
	fNextWake(ZTime::sSystem())
	{}

ZActorRunner_Threaded::~ZActorRunner_Threaded()
	{}

void ZActorRunner_Threaded::Start()
	{
	this->pAttachActor(fActor);
	ZThread::sCreate_T<ZRef<ZActorRunner_Threaded> >(spRun, this);
	}

void ZActorRunner_Threaded::Actor_Wake(ZRef<ZActor> iActor)
	{
	ZGuardMtx locker(fMtx);
	ZAssert(iActor == fActor);
	fNextWake = 0;
	fCnd.Broadcast();
	}

void ZActorRunner_Threaded::Actor_WakeAt(ZRef<ZActor> iActor, ZTime iSystemTime)
	{
	ZGuardMtx locker(fMtx);
	ZAssert(iActor == fActor);
	if (fNextWake > iSystemTime)
		{
		fNextWake = iSystemTime;
		fCnd.Broadcast();
		}
	}

void ZActorRunner_Threaded::Actor_WakeIn(ZRef<ZActor> iActor, double iInterval)
	{
	ZGuardMtx locker(fMtx);
	ZAssert(iActor == fActor);
	ZTime newWake = ZTime::sSystem() + iInterval;
	if (fNextWake > newWake)
		{
		fNextWake = newWake;
		fCnd.Broadcast();
		}
	}

void ZActorRunner_Threaded::pRun()
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
			if (!fActor->Act())
				break;
			}
		catch (...)
			{
			break;
			}
		}

	this->pDetachActor(fActor);
	fActor.Clear();
	}

void ZActorRunner_Threaded::spRun(ZRef<ZActorRunner_Threaded> iParam)
	{ iParam->pRun(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZActor

ZActor::ZActor()
	{}

void ZActor::RunnerAttached()
	{}

void ZActor::RunnerDetached()
	{}

void ZActor::Wake()
	{
	if (ZRef<ZActorRunner> theRunner = fRunner)
		theRunner->Actor_Wake(this);
	}

void ZActor::WakeIn(double iInterval)
	{
	if (ZRef<ZActorRunner> theRunner = fRunner)
		theRunner->Actor_WakeIn(this, iInterval);
	}

void ZActor::WakeAt(ZTime iSystemTime)
	{
	if (ZRef<ZActorRunner> theRunner = fRunner)
		theRunner->Actor_WakeAt(this, iSystemTime);
	}

void ZActor::pRunnerAttached()
	{ this->RunnerAttached(); }

void ZActor::pRunnerDetached()
	{ this->RunnerDetached(); }

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

void sStartActorRunner(ZRef<ZActor> iActor)
	{
	ZRef<ZActorRunner_Threaded> theRunner = new ZActorRunner_Threaded(iActor);
	theRunner->Start();
	}

NAMESPACE_ZOOLIB_END
