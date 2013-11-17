/* -------------------------------------------------------------------------------------------------
Copyright (c) 2013 Andrew Green
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

#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZSocketWatcher.h"

// See comment in Solaris' /usr/include/sys/ioctl.h
#if __sun__
	#define BSD_COMP
#endif

#include <sys/ioctl.h>
#include <sys/select.h>

#include <set>

namespace ZooLib {

using std::pair;
using std::set;

// =================================================================================================
// MARK: - ZSocketWatcher

ZSocketWatcher::ZSocketWatcher()
:	fThreadRunning(false)
	{}

bool ZSocketWatcher::Watch(int iSocket, const ZRef<ZCallable_Void>& iCallable)
	{
	ZAcqMtx acq(fMtx);
	if (not fSet.insert(pair<int,ZRef<ZCallable_Void> >(iSocket, iCallable)).second)
		return false;

	if (not fThreadRunning)
		{
		fThreadRunning = true;
		ZThread::sCreate_T<ZSocketWatcher*>(spRun, this);
		}
	fCnd.Broadcast();
	return true;
	}

bool ZSocketWatcher::Cancel(int iSocket, const ZRef<ZCallable_Void>& iCallable)
	{
	ZAcqMtx acq(fMtx);
	return fSet.erase(pair<int,ZRef<ZCallable_Void> >(iSocket, iCallable));
	}

void ZSocketWatcher::pRun()
	{
	ZGuardMtx guard(fMtx);
	for (;;)
		{
		if (fSet.empty())
			{
			// Nothing pending, wait 100ms in case something else comes along.
			fCnd.WaitFor(fMtx, 0.1);
			if (fSet.empty())
				{
				// Still nothing pending, exit thread.
				fThreadRunning = false;
				break;
				}
			}
		else
			{
			fd_set readSet;
			FD_ZERO(&readSet);
			int largest = 0;
			foreachi (ii, fSet)
				{
				const int theFD = ii->first;
				if (largest < theFD)
					largest = theFD;
				FD_SET(ii->first, &readSet);
				}

			fd_set exceptSet;
			FD_COPY(&readSet, &exceptSet);

			struct timeval timeout;
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			guard.Release();

			int count = ::select(largest + 1, &readSet, nullptr, &exceptSet, &timeout);

			guard.Acquire();

			if (count < 1)
				{
				// Didn't get any
				continue;
				}

			// Gather the callables
			set<ZRef<ZCallable_Void> > toCall;

			for (int fd = 1; fd <= largest; ++fd)
				{
				if (FD_ISSET(fd, &readSet))
					{
					--count;
					if (FD_ISSET(fd, &exceptSet))
						--count;
					}
				else if (FD_ISSET(fd, &exceptSet))
					--count;
				else
					continue;

				const Set_t::iterator iterBegin =
					fSet.lower_bound(pair<int,ZRef<ZCallable_Void> >(fd, null));
				Set_t::iterator iterEnd = iterBegin;
				const Set_t::iterator SetEnd = fSet.end();
				while (iterEnd->first == fd and iterEnd != SetEnd)
					{
					toCall.insert(iterEnd->second);
					++iterEnd;
					}
				fSet.erase(iterBegin, iterEnd);

				if (count <= 0)
					break;
				}

			guard.Release();
			foreachi (ii, toCall)
				{
				try { (*ii)->Call(); }
				catch (...){}
				}
			guard.Acquire();
			}
		}
	}

void ZSocketWatcher::spRun(ZSocketWatcher* iSocketWatcher)
	{
	ZThread::sSetName("ZSocketWatcher");

	iSocketWatcher->pRun();
	}

} // namespace ZooLib
