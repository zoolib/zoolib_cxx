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

#include "zoolib/Roster.h"
//#include "zoolib/Channer.h"
#include "zoolib/Connection.h"
#include "zoolib/Worker.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Server

class Server
:	public ZCounted
	{
public:
	typedef ZRef<Roster::Entry> ZRef_Roster_Entry; // CW7
	typedef Callable<void(ZRef_Roster_Entry,ZRef<ZStreamerRW>)> Callable_Connection;

	Server();
	virtual ~Server();

// From ZCounted
	virtual void Finalize();

// Our protocol
	bool IsStarted();

	void Start(ZRef<Starter> iStarter,
		ZRef<ZStreamerRWFactory> iFactory,
		ZRef<Callable_Connection> iCallable_Connection);

	void Stop();
	void StopWait();

	void KillConnections();
	void KillConnectionsWait();

	ZRef<ZStreamerRWFactory> GetFactory();

	ZRef<Callable_Connection> GetCallable_Connection();

private:
	bool pWork(ZRef<ZWorker> iWorker);
	void pWorkDetached(ZRef<ZWorker> iWorker);

	ZMtx fMtx;
	ZCnd fCnd;

	ZRef<ZStreamerRWFactory> fFactory;
	ZRef<Callable_Connection> fCallable_Connection;
	ZRef<Roster> fRoster;

	ZRef<ZWorker> fWorker;
	};

} // namespace ZooLib

#endif // __ZooLib_Server_h__
