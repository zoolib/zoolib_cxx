/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

// From ZCounted via Starter
	virtual void Initialize()
		{
		ZCounted::Initialize();
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
	virtual bool QStart(const ZRef<Startable>& iStartable)
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

			vector<ZRef<Startable> > toStart;

			fStartables.swap(toStart);

			fMtx.Release();

			for (vector<ZRef<Startable> >::iterator iter = toStart.begin();
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
	std::vector<ZRef<Startable> > fStartables;
	const ZQ<string> fNameQ;
	};

// =================================================================================================
#pragma mark - sStarter_ThreadLoop

ZRef<Starter> sStarter_ThreadLoop(const string& iName)
	{ return new Starter_ThreadLoop(iName); }

ZRef<Starter> sStarter_ThreadLoop()
	{ return new Starter_ThreadLoop(null); }

} // namespace ZooLib
