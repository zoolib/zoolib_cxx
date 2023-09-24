// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

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
	,	fExpireAfter(600)
	,	fIdleFor(10)
		{}

	~StartOnNewThreadHandler()
		{
		ZAcqMtx acq(fMtx);
		fKeepRunning = false;
		fCnd.Broadcast();
		while (fActiveThreads)
			fCnd.WaitFor(fMtx, 7);
		}

	void Start(const ZP<Startable>& iStartable)
		{
		ZAcqMtx acq(fMtx);
		fQueue.push_back(iStartable);
		if (fIdleThreads < fQueue.size())
			{
			++fActiveThreads;
			for (int xx = 0; /*no test*/; ++xx)
				{
				try
					{
					std::thread(&StartOnNewThreadHandler::RunLoop, this).detach();
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
		double lastActive = start;
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
				else
					{
					const double now = Time::sSystem();
					if (now - start >= fExpireAfter)
						{
						break;
						}
					else if (now - lastActive >= fIdleFor)
						{
						break;
						}
					}
				++fIdleThreads;
				fCnd.WaitFor(fMtx, 5);
				--fIdleThreads;
 				}
			else
				{
				ZThread::sSetName("SONT call");
				ZP<Startable> theCallable = fQueue.front();
				fQueue.pop_front();

				try
					{
					ZRelMtx acq(fMtx);
					theCallable->QCall();
					}
				catch (...)
					{}
				lastActive = Time::sSystem();
				}
			}
		--fActiveThreads;
		}

	static void spRunLoop(StartOnNewThreadHandler* iHandler)
		{ iHandler->RunLoop(); }

	ZMtx fMtx;
	ZCnd fCnd;
	size_t fIdleThreads;
	size_t fActiveThreads;
	bool fKeepRunning;
	double fExpireAfter;
	double fIdleFor;
	std::list<ZP<Startable>> fQueue;
	};

// ----------

void sStartOnNewThread(const ZP<Startable>& iStartable)
	{
	if (iStartable)
		sSingleton<StartOnNewThreadHandler>().Start(iStartable);
	}

void sStartOnNewThread_ProcessIsAboutToExit()
	{ sSingleton<StartOnNewThreadHandler>().ProcessIsAboutToExit(); }

} // namespace ZooLib
