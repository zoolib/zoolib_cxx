// Copyright (c) 2014-2021 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Starter_ThreadLoop.h"
#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Callable_PMF.h"

#include "zoolib/StartOnNewThread.h"

#include <vector>

namespace ZooLib {

using std::string;
using std::vector;

// =================================================================================================
#pragma mark - Starter_ThreadLoop

class Starter_ThreadLoop
:	public Starter
	{
public:
	Starter_ThreadLoop(const ZP<Callable_Bookend>& iBookend_Loop,
		const ZP<Callable_Bookend>& iBookend_Startable)
	:	fBookend_Loop(iBookend_Loop)
	,	fBookend_Startable(iBookend_Startable)
	,	fKeepRunning(false)
		{}

	~Starter_ThreadLoop() override
		{}

// From Counted via Starter
	void Initialize() override
		{
		Counted::Initialize();
		ZAcqMtx acq(fMtx);

		ZAssert(not fKeepRunning);

		fKeepRunning = true;

		ZP<Callable_Void> theCallable = sCallable(this, &Starter_ThreadLoop::pRun);

		if (fBookend_Loop)
			theCallable = sBindR(fBookend_Loop, theCallable);

		sStartOnNewThread(theCallable);
		}

	void Finalize() override
		{
		ZAcqMtx acq(fMtx);
		if (not this->FinishFinalize())
			return;

		ZAssert(fKeepRunning);

		fKeepRunning = false;
		fCnd.Broadcast();
		}

// From Starter
	bool QStart(const ZP<Startable>& iStartable) override
		{
		if (iStartable)
			{
			ZAcqMtx acq(fMtx);
			fStartables.push_back(iStartable);
			fCnd.Broadcast();
			return true;
			}
		return false;
		}

private:
	void pRun()
		{
		fMtx.Acquire();
		while (fKeepRunning)
			{
			if (fStartables.empty())
				{
				fCnd.Wait(fMtx);
				continue;
				}

			// Ensure we stick around while we're processing something.
			this->Retain();

			vector<ZP<Startable>> toStart;

			fStartables.swap(toStart);

			fMtx.Release();

			for (ZP<Startable> theStartable: toStart)
				{
				try
					{
					if (fBookend_Startable)
						fBookend_Startable->Call(theStartable);
					else
						theStartable->Call();
					}
				catch (...) {}
				}

			this->Release();

			fMtx.Acquire();
			}
		fMtx.Release();

		delete this;
		}

	const ZP<Callable_Bookend> fBookend_Loop;
	const ZP<Callable_Bookend> fBookend_Startable;
	ZMtx fMtx;
	ZCnd fCnd;
	bool fKeepRunning;
	std::vector<ZP<Startable>> fStartables;
	};

// =================================================================================================
#pragma mark - sStarter_ThreadLoop

ZP<Starter> sStarter_ThreadLoop(
	const ZP<Callable_Bookend>& iBookend_Loop,
	const ZP<Callable_Bookend>& iBookend_Startable)
	{ return new Starter_ThreadLoop(iBookend_Loop, iBookend_Startable); }


static void spSetThreadName(const ZP<Startable>& iStartable, const std::string& iName)
	{
	ZThread::sSetName(iName.c_str());
	sCall(iStartable);
	}

ZP<Starter> sStarter_ThreadLoop(const std::string& iName)
	{ return sStarter_ThreadLoop(sBindR(sCallable(spSetThreadName), iName), null); }

ZP<Starter> sStarter_ThreadLoop()
	{ return sStarter_ThreadLoop("STL"); }


} // namespace ZooLib
