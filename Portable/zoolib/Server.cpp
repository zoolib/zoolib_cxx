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

#include "zoolib/Server.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Callable_PMF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Server

Server::Server()
	{}

Server::~Server()
	{}

void Server::Finalize()
	{
	{
	ZAcqMtx acq(fMtx);
	ZAssert(not fWorker);
	ZAssert(not fFactory);
	fRoster.Clear();
	fCallable_Connection.Clear();
	}

	Counted::Finalize();
	}

bool Server::IsStarted()
	{
	ZAcqMtx acq(fMtx);
	return fFactory;
	}

void Server::Start(ZRef<Starter> iStarter,
		ZRef<Factory_ChannerRW_Bin> iFactory,
		ZRef<Cancellable> iCancellable,
		ZRef<Callable_Connection> iCallable_Connection)
	{
	ZAssert(iStarter);
	ZAssert(iFactory);

	// Declared before the acq, so it goes out of scope after it, and any
	// callable on the roster is invoked with our mutex released.
	ZRef<Roster> priorRoster;

	ZAcqMtx acq(fMtx);

	ZAssert(not fWorker);
	ZAssert(not fFactory);
	ZAssert(not fCallable_Connection);

	priorRoster = fRoster;
	fRoster = new Roster;

	fFactory = iFactory;
	fCallable_Connection = iCallable_Connection;
	fCancellable = iCancellable;

	fWorker = new Worker(
		sCallable(sWeakRef(this), &Server::pWork),
		sCallable(sWeakRef(this), &Server::pWorkDetached));

	fWorker->Attach(iStarter);

	fWorker->Wake();
	}

void Server::Stop()
	{
	ZAcqMtx acq(fMtx);
	if (ZRef<Factory_ChannerRW_Bin> theFactory = fFactory)
		{
		fFactory.Clear();
		if (fCancellable)
			fCancellable->Cancel();
		fCancellable.Clear();
		}
	fCallable_Connection.Clear();
	fCnd.Broadcast();
	}

void Server::StopWait()
	{
	ZAcqMtx acq(fMtx);

	if (ZRef<Factory_ChannerRW_Bin> theFactory = fFactory)
		{
		fFactory.Clear();
		if (fCancellable)
			fCancellable->Cancel();
		fCancellable.Clear();
		}

	if (fWorker)
		{
		fWorker->Wake();
		while (fWorker)
			fCnd.Wait(fMtx);
		}
	}

void Server::KillConnections()
	{
	ZAcqMtx acq(fMtx);
	if (ZRef<Roster> theRoster = fRoster)
		{
		ZRelMtx rel(fMtx);
		theRoster->Broadcast();
		}
	}

void Server::KillConnectionsWait()
	{
	ZAcqMtx acq(fMtx);
	if (ZRef<Roster> theRoster = fRoster)
		{
		ZRelMtx rel(fMtx);
		theRoster->Broadcast();
		for (;;)
			{
			if (const size_t theCount = theRoster->Count())
				theRoster->Wait(theCount);
			else
				break;
			}
		}
	}

ZRef<Factory_ChannerRW_Bin> Server::GetFactory()
	{
	ZAcqMtx acq(fMtx);
	return fFactory;
	}

ZRef<Server::Callable_Connection> Server::GetCallable_Connection()
	{
	ZAcqMtx acq(fMtx);
	return fCallable_Connection;
	}

static void spKill(ZRef<ChannerAbort> iChannerAbort)
	{
	if (iChannerAbort)
		sAbort(*iChannerAbort);
	}

bool Server::pWork(ZRef<Worker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	if (ZRef<Factory_ChannerRW_Bin> theFactory = fFactory)
		{
		ZRelMtx rel(fMtx);
		if (ZRef<ChannerRW_Bin> theChanner = sCall(theFactory))
			{
			ZAcqMtx acq(fMtx);
			if (ZRef<Callable_Connection> theCallable = fCallable_Connection)
				{
				ZRelMtx rel(fMtx);
				try
					{
					ZRef<Callable_Void> theCallable_Kill =
						sBindR(sCallable(spKill), sRef(theChanner.DynamicCast<ChannerAbort>()));
					theCallable->Call(fRoster->MakeEntry(theCallable_Kill, null), theChanner);
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

void Server::pWorkDetached(ZRef<Worker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	fWorker.Clear();
	fCnd.Broadcast();
	}

} // namespace ZooLib
