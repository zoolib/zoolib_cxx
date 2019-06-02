/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#include "zoolib/StartOnNewThread.h"

#include "zoolib/Log.h"
#include "zoolib/Singleton.h"
#include "zoolib/Util_Debug.h"
#include "zoolib/ZThread.h"

#include <list>

namespace ZooLib {

// =================================================================================================
#pragma mark - sStartOnNewThread

class StartOnNewThreadHandler
	{
public:
	StartOnNewThreadHandler()
	:	fIdleThreads(0)
	,	fActiveThreads(0)
	,	fKeepRunning(true)
	,	fExpireAfter(10)
		{}

	~StartOnNewThreadHandler()
		{
		ZAcqMtx acq(fMtx);
		fKeepRunning = false;
		fCnd.Broadcast();
		while (fActiveThreads)
			fCnd.WaitFor(fMtx, 7);
		}

	void Start(const ZRef<Callable<void()>>& iCallable)
		{
		ZAcqMtx acq(fMtx);
		fQueue.push_back(iCallable);
		if (fIdleThreads < fQueue.size())
			{
			++fActiveThreads;
			for (int xx = 0; /*no test*/; ++xx)
				{
				try
					{
					ZThread::sStartRaw(0, (ZThread::ProcRaw_t)spRunLoop, this);
					if (xx != 0)
						{
						if (ZLOGF(w, eErr))
							w << "ZThread::sStartRaw succeeded after " << xx << " additional tries.";
						}
					break;
					}
				catch (std::exception& ex)
					{
					if (xx == 0)
						{
						if (ZLOGF(w, eErr))
							{
							w << "Exception: " << ex.what() << "\n";
							Util_Debug::sWriteBacktrace(w);
							}
						}
					else
						{
						ZThread::sSleep(0.01);
						}
					}
				}
			}
		else
			{
			fCnd.Broadcast();
			}
		}

	void ProcessIsAboutToExit()
		{
		ZAcqMtx acq(fMtx);
		fExpireAfter = 0;
		fCnd.Broadcast();
		while (fActiveThreads)
			fCnd.WaitFor(fMtx, 1);
		}

	void RunLoop()
		{
		ZAcqMtx acq(fMtx);
		const double start = Time::sSystem();
		for (;;)
			{
			if (fQueue.empty())
				{
				ZThread::sSetName("SONT idle");
				if (not fKeepRunning)
					{
					fCnd.Broadcast();
					break;
					}
				else if (Time::sSystem() - start >= fExpireAfter)
					{
					break;
					}
				++fIdleThreads;
				fCnd.WaitFor(fMtx, 5);
				--fIdleThreads;
 				}
			else
				{
				ZThread::sSetName("SONT call");
				ZRef<Callable<void()>> theCallable = fQueue.front();
				fQueue.pop_front();

				try
					{
					ZRelMtx acq(fMtx);
					theCallable->QCall();
					}
				catch (...)
					{}
				}
			}
		--fActiveThreads;
		}

	static void spRunLoop(void* iRefcon)
		{
		ZThread::sStarted();
		static_cast<StartOnNewThreadHandler*>(iRefcon)->RunLoop();
		ZThread::sFinished();
		}

	ZMtx fMtx;
	ZCnd fCnd;
	size_t fIdleThreads;
	size_t fActiveThreads;
	bool fKeepRunning;
	double fExpireAfter;
	std::list<ZRef<Callable<void()>>> fQueue;
	};

// ----------

void sStartOnNewThread(const ZRef<Callable<void()> >& iCallable)
	{
	if (iCallable)
		sSingleton<StartOnNewThreadHandler>().Start(iCallable);
	}

void sStartOnNewThread_ProcessIsAboutToExit()
	{ sSingleton<StartOnNewThreadHandler>().ProcessIsAboutToExit(); }

} // namespace ZooLib
