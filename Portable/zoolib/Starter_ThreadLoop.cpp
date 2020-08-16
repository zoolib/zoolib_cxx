// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Starter_ThreadLoop.h"

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
	Starter_ThreadLoop(const ZQ<string>& iNameQ)
	:	fKeepRunning(false)
	,	fNameQ(iNameQ)
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
		if (fNameQ)
			ZThread::sSetName(fNameQ->c_str());
		else
			ZThread::sSetName("STL");

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
		delete this;
		}

	static void spRun(Starter_ThreadLoop* iStarter)
		{ iStarter->pRun(); }

	ZMtx fMtx;
	ZCnd fCnd;
	bool fKeepRunning;
	std::vector<ZP<Startable>> fStartables;
	const ZQ<string> fNameQ;
	};

// =================================================================================================
#pragma mark - sStarter_ThreadLoop

ZP<Starter> sStarter_ThreadLoop(const string& iName)
	{ return new Starter_ThreadLoop(iName); }

ZP<Starter> sStarter_ThreadLoop()
	{ return new Starter_ThreadLoop(null); }

} // namespace ZooLib
