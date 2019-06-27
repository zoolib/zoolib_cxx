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

#ifndef __ZooLib_Server_h__
#define __ZooLib_Server_h__ 1
#include "zconfig.h"

#include "zoolib/Cancellable.h"
#include "zoolib/Connection.h"
#include "zoolib/Roster.h"
#include "zoolib/Worker.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Server

class Server
:	public Counted
	{
public:
	typedef ZP<Roster::Entry> ZP_Roster_Entry; // CW7
	typedef Callable<void(ZP_Roster_Entry,ZP<ChannerRW<byte>>)> Callable_Connection;

	Server();
	virtual ~Server();

// From Counted
	virtual void Finalize();

// Our protocol
	bool IsStarted();

	void Start(ZP<Starter> iStarter,
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
	bool pWork(ZP<Worker> iWorker);
	void pWorkDetached(ZP<Worker> iWorker);

	ZMtx fMtx;
	ZCnd fCnd;

	ZP<Factory_ChannerRW_Bin> fFactory;
	ZP<Callable_Connection> fCallable_Connection;
	ZP<Cancellable> fCancellable;
	ZP<Roster> fRoster;

	ZP<Worker> fWorker;
	};

} // namespace ZooLib

#endif // __ZooLib_Server_h__
