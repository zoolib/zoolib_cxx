// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Server/Server.h"

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

void Server::Start(ZP<Starter> iStarter,
		ZP<Factory_ChannerRW_Bin> iFactory,
		ZP<Cancellable> iCancellable,
		ZP<Callable_Connection> iCallable_Connection)
	{
	ZAssert(iStarter);
	ZAssert(iFactory);

	// Declared before the acq, so it goes out of scope after it, and any
	// callable on the roster is invoked with our mutex released.
	ZP<Roster> priorRoster;

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
		sCallable(sWP(this), &Server::pWork),
		sCallable(sWP(this), &Server::pWorkDetached));

	fWorker->Attach(iStarter);

	fWorker->Wake();
	}

void Server::Stop()
	{
	ZAcqMtx acq(fMtx);
	if (ZP<Factory_ChannerRW_Bin> theFactory = fFactory)
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

	if (ZP<Factory_ChannerRW_Bin> theFactory = fFactory)
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
	if (ZP<Roster> theRoster = fRoster)
		{
		ZRelMtx rel(fMtx);
		theRoster->Broadcast();
		}
	}

void Server::KillConnectionsWait()
	{
	ZAcqMtx acq(fMtx);
	if (ZP<Roster> theRoster = fRoster)
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

ZP<Factory_ChannerRW_Bin> Server::GetFactory()
	{
	ZAcqMtx acq(fMtx);
	return fFactory;
	}

ZP<Server::Callable_Connection> Server::GetCallable_Connection()
	{
	ZAcqMtx acq(fMtx);
	return fCallable_Connection;
	}

static void spKill(ZP<ChannerAbort> iChannerAbort)
	{
	if (iChannerAbort)
		sAbort(*iChannerAbort);
	}

bool Server::pWork(ZP<Worker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	if (ZP<Factory_ChannerRW_Bin> theFactory = fFactory)
		{
		ZRelMtx rel(fMtx);
		if (ZP<ChannerRW_Bin> theChanner = sCall(theFactory))
			{
			ZAcqMtx acq(fMtx);
			if (ZP<Callable_Connection> theCallable = fCallable_Connection)
				{
				ZRelMtx rel(fMtx);
				try
					{
					ZP<Callable_Void> theCallable_Kill =
						sBindR(sCallable(spKill), sZP(theChanner.DynamicCast<ChannerAbort>()));
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

void Server::pWorkDetached(ZP<Worker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	fWorker.Clear();
	fCnd.Broadcast();
	}

} // namespace ZooLib
