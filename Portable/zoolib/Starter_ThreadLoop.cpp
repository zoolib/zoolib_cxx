// Copyright (c) 2014-2021 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Starter_ThreadLoop.h"
#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"

#include "zoolib/ZThread.h"

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
	Starter_ThreadLoop(const ZP<Callable<void(bool)>>& iRunCB)
	:	fRunCB(iRunCB)
	,	fKeepRunning(false)
		{}

	virtual ~Starter_ThreadLoop()
		{}

// From Counted via Starter
	virtual void Initialize()
		{
		Counted::Initialize();
		ZAcqMtx acq(fMtx);

		ZAssert(not fKeepRunning);

		fKeepRunning = true;
		ZThread::sStart_T<Starter_ThreadLoop*>(&Starter_ThreadLoop::spRun, this);
		}

	virtual void Finalize()
		{
		ZAcqMtx acq(fMtx);
		if (not this->FinishFinalize())
			return;

		ZAssert(fKeepRunning);

		fKeepRunning = false;
		fCnd.Broadcast();
		}

// From Starter
	virtual bool QStart(const ZP<Startable>& iStartable)
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
		sCall(fRunCB, true);

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

			for (vector<ZP<Startable>>::iterator iter = toStart.begin();
				iter != toStart.end(); ++iter)
				{
				try { (*iter)->Call(); }
				catch (...) {}
				}
			this->Release();
			fMtx.Acquire();
			}
		fMtx.Release();

		sCall(fRunCB, false);

		delete this;
		}

	static void spRun(Starter_ThreadLoop* iStarter)
		{ iStarter->pRun(); }

	const ZP<Callable<void(bool)>> fRunCB;
	ZMtx fMtx;
	ZCnd fCnd;
	bool fKeepRunning;
	std::vector<ZP<Startable>> fStartables;
	};

// =================================================================================================
#pragma mark - sStarter_ThreadLoop

ZP<Starter> sStarter_ThreadLoop(const ZP<Callable<void(bool)>>& iRunCB)
	{ return new Starter_ThreadLoop(iRunCB); }

static void spSetThreadName(bool iStarting, const std::string& iName)
	{
	if (iStarting)
		ZThread::sSetName(iName.c_str());
	}

ZP<Starter> sStarter_ThreadLoop(const std::string& iName)
	{ return sStarter_ThreadLoop(sBindR(sCallable(spSetThreadName), iName)); }

ZP<Starter> sStarter_ThreadLoop()
	{ return sStarter_ThreadLoop("STL"); }


} // namespace ZooLib
