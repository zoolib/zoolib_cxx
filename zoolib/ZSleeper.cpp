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

#include "zoolib/ZSleeper.h"

#include "zoolib/ZThreadSimple.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeper

ZSleeper::ZSleeper()
:	fRunner(nil)
	{}

ZSleeper::~ZSleeper()
	{
	// By the time a sleeper's destructor is called, it must have been
	// detached from its runner. That's the subclass' responsibility.
	ZAssert(!fRunner);
	}

void ZSleeper::RunnerAttached(ZSleeperRunner* iRunner)
	{}

void ZSleeper::RunnerDetached(ZSleeperRunner* iRunner)
	{}

void ZSleeper::Wake()
	{
	if (fRunner)
		fRunner->WakeAt(this, ZTime::sSystem());
	}

void ZSleeper::WakeAt(ZTime iSystemTime)
	{
	if (fRunner)
		fRunner->WakeAt(this, iSystemTime);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeperRunner

void ZSleeperRunner::pAttachSleeper(ZSleeper* iSleeper)
	{
	ZAssert(iSleeper);
	ZAssert(!iSleeper->fRunner);

	iSleeper->fRunner = this;

	iSleeper->RunnerAttached(this);
	}

void ZSleeperRunner::pDetachSleeper(ZSleeper* iSleeper)
	{
	ZAssert(iSleeper);
	ZAssert(iSleeper->fRunner = this);

	iSleeper->fRunner = nil;

	iSleeper->RunnerDetached(this);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeperRunner_Threaded

ZSleeperRunner_Threaded::ZSleeperRunner_Threaded(ZSleeper* iSleeper)
:	fMutex("ZSleeperRunner_Threaded::fMutex"),
	fSleeper(iSleeper),
	fNextWake(ZTime::sSystem())
	{}

ZSleeperRunner_Threaded::~ZSleeperRunner_Threaded()
	{
	ZAssert(!fSleeper);
	}

void ZSleeperRunner_Threaded::Start()
	{
	this->pAttachSleeper(fSleeper);

	(new ZThreadSimple<ZSleeperRunner_Threaded*>(spRun, this))->Start();
	// Do we need to be able to wait for start?
	}

void ZSleeperRunner_Threaded::WakeAt(ZSleeper* iSleeper, ZTime iSystemTime)
	{
	ZMutexLocker locker(fMutex);
	ZAssert(iSleeper == fSleeper);
	if (fNextWake > iSystemTime)
		{
		fNextWake = iSystemTime;
		fCondition.Broadcast();
		}
	}

void ZSleeperRunner_Threaded::pRun()
	{
	for (;;)
		{
		ZMutexLocker locker(fMutex);
		for (;;)
			{
			const ZTime now = ZTime::sSystem();
			if (fNextWake <= now)
				{
				fNextWake = now + 3600;
				break;
				}

			fCondition.Wait(fMutex, bigtime_t(1000000 * (fNextWake - now)));
			}
		locker.Release();

		try
			{
			if (!fSleeper->Execute())
				break;
			}
		catch (...)
			{
			break;
			}
		}

	this->pDetachSleeper(fSleeper);

	fSleeper = nil;

	delete this;
	}

void ZSleeperRunner_Threaded::spRun(ZSleeperRunner_Threaded* iRunner)
	{ iRunner->pRun(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeperRunnerFactory_Threaded

void ZSleeperRunnerFactory_Threaded::MakeRunner(ZSleeper* iSleeper)
	{
	ZSleeperRunner_Threaded* theRunner = new ZSleeperRunner_Threaded(iSleeper);
	theRunner->Start();
	}
