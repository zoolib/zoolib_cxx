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

#include "zoolib/Singleton.h"
#include "zoolib/ZThread.h"

#include <list>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark sStartOnNewThread

class StartOnNewThreadHandler
	{
public:
	StartOnNewThreadHandler()
	:	fSpareThreads(0)
		{}

	void Start(const ZRef<Callable<void()>>& iCallable)
		{
		ZAcqMtx acq(fMtx);
		fQueue.push_back(iCallable);
		if (fSpareThreads == 0)
			ZThread::sStartRaw(0, (ZThread::ProcRaw_t)sDoOne, this);
		else
			fCnd.Broadcast();
		}

	void DoOne()
		{
		ZAcqMtx acq(fMtx);
		double expires = Time::sSystem() + 10;
		++fSpareThreads;
		for (;;)
			{
			if (fQueue.empty())
				{
				if (Time::sSystem() > expires)
					break;
				fCnd.WaitFor(fMtx, 5);
				}
			else
				{
				ZRef<Callable<void()>> theCallable = fQueue.front();
				fQueue.pop_front();

				--fSpareThreads;

				try
					{
					ZRelMtx acq(fMtx);
					theCallable->QCall();
					}
				catch (...)
					{}

				++fSpareThreads;
				}
			}
		--fSpareThreads;
		}

	static void sDoOne(void* iRefcon)
		{ static_cast<StartOnNewThreadHandler*>(iRefcon)->DoOne(); }

	ZMtx fMtx;
	ZCnd fCnd;
	int fSpareThreads;
	std::list<ZRef<Callable<void()>>> fQueue;
	};

// ----------

void sStartOnNewThread(const ZRef<Callable<void()> >& iCallable)
	{
	if (iCallable)
		sSingleton<StartOnNewThreadHandler>().Start(iCallable);
	}

} // namespace ZooLib
