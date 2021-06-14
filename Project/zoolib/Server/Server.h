// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Server_h__
#define __ZooLib_Server_h__ 1
#include "zconfig.h"

#include "zoolib/Cancellable.h"
#include "zoolib/Connection.h"

#include "zoolib/Server/Roster.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Server

class Server
:	public Counted
	{
public:
	typedef Callable<void(ZP<Roster::Entry>,ZP<ChannerRW<byte>>)> Callable_Connection;

	Server();
	virtual ~Server();

// From Counted
	virtual void Finalize();

// Our protocol
	bool IsStarted();

	void Start(ZP<Counted> iStarter,
		ZP<Factory_ChannerRW_Bin> iFactory,
		ZP<Cancellable> iCancellable,
		ZP<Callable_Connection> iCallable_Connection);

	void Stop();
	void StopWait();

	void KillConnections();
	void KillConnectionsWait();

	ZP<Factory_ChannerRW_Bin> GetFactory();

	ZP<Callable_Connection> GetCallable_Connection();

private:
	void pRun();

	ZMtx fMtx;
	ZCnd fCnd;

	bool fIsRunning;
	ZP<Factory_ChannerRW_Bin> fFactory;
	ZP<Callable_Connection> fCallable_Connection;
	ZP<Cancellable> fCancellable;
	ZP<Roster> fRoster;
	};

} // namespace ZooLib

#endif // __ZooLib_Server_h__
