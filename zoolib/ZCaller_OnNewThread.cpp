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

#include "zoolib/ZCaller_OnNewThread.h"

#include "zoolib/ZThread.h"

#include <vector>

namespace ZooLib {

using std::vector;

// =================================================================================================
// MARK: - Caller_OnNewThread

namespace { // anonymous

class Caller_OnNewThread
:	public ZCaller
	{
public:
	Caller_OnNewThread()
	:	fKeepRunning(false)
	,	fIsRunning(false)
		{}

	virtual ~Caller_OnNewThread()
		{}

// From ZCounted via ZCaller
	virtual void Initialize()
		{
		ZCounted::Initialize();
		ZAcqMtxR acq(fMtxR);

		ZAssert(not fKeepRunning and not fIsRunning);

		fKeepRunning = true;
		ZThread::sCreate_T<Caller_OnNewThread*>(&Caller_OnNewThread::spRun, this);
		while (not fIsRunning)
			fCnd.Wait(fMtxR);
		}

	virtual void Finalize()
		{
		ZGuardMtxR guard(fMtxR);
		if (not this->FinishFinalize())
			return;

		ZAssert(fKeepRunning and fIsRunning);

		fKeepRunning = false;
		fCnd.Broadcast();
		while (fIsRunning)
			fCnd.Wait(fMtxR);
		guard.Release();
		delete this;
		}

// From ZCaller
	virtual bool Enqueue(const ZRef<ZCallable_Void>& iCallable)
		{
		ZGuardMtxR guard(fMtxR);
		fCallables.push_back(iCallable);
		fCnd.Broadcast();
		return true;
		}

private:
	void pRun()
		{
		ZGuardMtxR guard(fMtxR);
		ZAssert(not fIsRunning);
		fIsRunning = true;
		fCnd.Broadcast();

		while (fKeepRunning)
			{
			if (fCallables.empty())
				{ fCnd.Wait(fMtxR); }
			else
				{
				vector<ZRef<ZCallable_Void> > calling;

				fCallables.swap(calling);

				guard.Release();

				for (vector<ZRef<ZCallable_Void> >::iterator iter = calling.begin();
					iter != calling.end(); ++iter)
					{
					try { (*iter)->Call(); }
					catch (...) {}
					}

				guard.Acquire();
				}
			}

		fIsRunning = false;
		fCnd.Broadcast();
		}

	static void spRun(Caller_OnNewThread* iCaller)
		{ iCaller->pRun(); }

	ZMtxR fMtxR;
	ZCnd fCnd;
	bool fKeepRunning;
	bool fIsRunning;
	std::vector<ZRef<ZCallable_Void> > fCallables;
	};

} // anonymous namespace

// =================================================================================================
// MARK: - sCaller_OnNewThread

ZRef<ZCaller> sCaller_OnNewThread()
	{ return new Caller_OnNewThread; }

} // namespace ZooLib
