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
	ZAssert(!fWorker);
	ZAssert(!fFactory);
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

	ZRef<ZRoster> priorRoster;

	ZAcqMtx acq(fMtx);

	ZAssert(not fWorker);
	ZAssert(not fFactory);
	ZAssert(not fCallable_Connection);

	priorRoster = fRoster;
	fRoster = new ZRoster;

	fFactory = iFactory;
	fCallable_Connection = iCallable_Connection;

	fWorker = new ZWorker(sCallable(sWeakRef(this), &ZServer::pWork));

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
	fFactory.Clear();
	fCnd.Broadcast();
	while (fWorker)
		fCnd.Wait(fMtx);
	}

void ZServer::KillConnections()
	{ fRoster->Broadcast(); }

void ZServer::KillConnectionsWait()
	{
	fRoster->Broadcast();
	fRoster->Wait(0);
	}

ZRef<ZStreamerRWFactory> ZServer::GetFactory()
	{
	ZAcqMtx acq(fMtx);
	return fFactory;
	}

ZRef<ZServer::Callable_Connection> ZServer::Get_Callable_Connection()
	{
	ZAcqMtx acq(fMtx);
	return fCallable_Connection;
	}

void ZServer::Set_Callable_Connection(const ZRef<Callable_Connection>& iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCallable_Connection = iCallable;
	}

bool ZServer::CAS_Callable_Connection
	(ZRef<Callable_Connection> iPrior, ZRef<Callable_Connection> iNew)
	{
	ZAcqMtx acq(fMtx);
	if (fCallable_Connection != iPrior)
		return false;
	fCallable_Connection = iNew;
	return true;
	}

static void spKill(ZRef<ZStreamerRWCon> iSRWCon)
	{ iSRWCon->Abort(); }

bool ZServer::pWork(ZRef<ZWorker> iWorker)
	{
	ZGuardRMtx guard(fMtx);

	if (ZRef<ZStreamerRWFactory,false> theFactory = fFactory)
		{
		fWorker.Clear();
		fCnd.Broadcast();
		return false;
		}
	else
		{
		guard.Release();
		if (ZRef<ZStreamerRW> theSRW = theFactory->MakeStreamerRW())
			{
			guard.Acquire();
			if (ZRef<Callable_Connection> theCallable = fCallable_Connection)
				{
				ZRef<ZRoster::Entry> theEntry = fRoster->MakeEntry();
				if (ZRef<ZStreamerRWCon> theSRWCon = theSRW.DynamicCast<ZStreamerRWCon>())
					theEntry->Set_Callable_Broadcast(sBindR(sCallable(spKill), theSRWCon));
				guard.Release();
				theCallable->Call(theEntry, theSRW);
				}
			}
		}
	iWorker->Wake();
	return true;
	}

} // namespace ZooLib
