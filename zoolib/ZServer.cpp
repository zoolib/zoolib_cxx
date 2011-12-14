/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZCallable_Bind.h"
#include "zoolib/ZCallable_Function.h"
#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZServer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZServer

ZServer::ZServer()
	{}

ZServer::~ZServer()
	{}

void ZServer::Finalize()
	{
	ZGuardRMtx guard(fMtx);
	ZAssert(not fWorker);
	ZAssert(not fFactory);
	fRoster.Clear();
	fCallable_Connection.Clear();
	guard.Release();

	ZCounted::Finalize();
	}

void ZServer::Start(ZRef<ZCaller> iCaller,
	ZRef<ZStreamerRWFactory> iFactory,
	ZRef<Callable_Connection> iCallable_Connection)
	{
	ZAssert(iCaller);
	ZAssert(iFactory);

	// Declared before the acq, so it goes out of scope
	// after it, and any callable is invoked with our mutex released.
	ZRef<ZRoster> priorRoster;

	ZAcqMtx acq(fMtx);

	ZAssert(not fWorker);
	ZAssert(not fFactory);
	ZAssert(not fCallable_Connection);

	priorRoster = fRoster;
	fRoster = new ZRoster;

	fFactory = iFactory;
	fCallable_Connection = iCallable_Connection;

	fWorker = new ZWorker
		(sCallable(sWeakRef(this), &ZServer::pWork),
		sCallable(sWeakRef(this), &ZServer::pWorkDetached));

	fWorker->Attach(iCaller);

	fWorker->Wake();
	}

void ZServer::Stop()
	{
	ZAcqMtx acq(fMtx);
	if (ZRef<ZStreamerRWFactory> theFactory = fFactory)
		{
		fFactory.Clear();
		theFactory->Cancel();
		}
	fCallable_Connection.Clear();
	fCnd.Broadcast();
	}

void ZServer::StopWait()
	{
	ZAcqMtx acq(fMtx);

	if (ZRef<ZStreamerRWFactory> theFactory = fFactory)
		{
		fFactory.Clear();
		theFactory->Cancel();
		}

	if (fWorker)
		{
		fWorker->Wake();
		while (fWorker)
			fCnd.Wait(fMtx);
		}
	}

void ZServer::KillConnections()
	{ fRoster->Broadcast(); }

void ZServer::KillConnectionsWait()
	{
	fRoster->Broadcast();
	for (;;)
		{
		if (const size_t theCount = fRoster->Count())
			fRoster->Wait(theCount);
		else
			break;
		}
	}

ZRef<ZStreamerRWFactory> ZServer::GetFactory()
	{
	ZAcqMtx acq(fMtx);
	return fFactory;
	}

ZRef<ZServer::Callable_Connection> ZServer::GetCallable_Connection()
	{
	ZAcqMtx acq(fMtx);
	return fCallable_Connection;
	}

static void spKill(ZRef<ZStreamerRWCon> iSRWCon)
	{
	if (iSRWCon)
		iSRWCon->Abort();
	}

bool ZServer::pWork(ZRef<ZWorker> iWorker)
	{
	ZGuardRMtx guard(fMtx);

	if (ZRef<ZStreamerRWFactory> theFactory = fFactory)
		{
		guard.Release();
		if (ZRef<ZStreamerRW> theSRW = theFactory->MakeStreamerRW())
			{
			guard.Acquire();
			if (ZRef<Callable_Connection> theCallable = fCallable_Connection)
				{
				guard.Release();
				try
					{
					ZRef<ZCallable_Void> theCallable_Kill =
						sBindR(sCallable(spKill), theSRW.DynamicCast<ZStreamerRWCon>());
					theCallable->Call(fRoster->MakeEntry(theCallable_Kill, null), theSRW);
					}
				catch (...)
					{}
				}
			}
		iWorker->Wake();
		return true;
		}
	return false;
	}

void ZServer::pWorkDetached(ZRef<ZWorker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	fWorker.Clear();
	fCnd.Broadcast();
	}

} // namespace ZooLib
