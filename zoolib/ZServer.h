/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZServer__
#define __ZServer__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"
#include "zoolib/ZThread.h"

#include <vector>

// =================================================================================================
#pragma mark -
#pragma mark * ZServer

class ZServer
	{
public:
	class Responder;
	friend class Responder;

	ZServer();
	virtual ~ZServer();

	void StartWaitingForConnections(ZRef<ZStreamerRWConFactory> iFactory);
	void StopWaitingForConnections();
	void KillAllConnections();
	void KillAllNoWait();

	void SetMaxResponders(size_t iCount);
	ZRef<ZStreamerRWConFactory> GetFactory();

protected:
	void ResponderFinished(Responder* iResponder);

	virtual Responder* CreateResponder() = 0;

private:
	void RunThread();
	static void sRunThread(ZServer* iServer);

private:
	ZooLib::ZMutex fMutex;
	ZooLib::ZCondition fCondition;
	ZooLib::ZSemaphore fSem;
	std::vector<Responder*> fResponders;

	ZRef<ZStreamerRWConFactory> fFactory;
	size_t fMaxResponders;
	size_t fAcceptedCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZServer::Responder

class ZServer::Responder
	{
public:
	Responder();
	virtual ~Responder();

	void KillConnection();

	virtual void DoRun(ZRef<ZStreamerRWCon> iStreamerRWCon) = 0;

protected:
	friend class ZServer;
	void HandleConnection(ZServer* iServer, ZRef<ZStreamerRWCon> iStreamerRWCon);
	void ServerDeleted(ZServer* iServer);

	void RunThread();
	static void sRunThread(Responder* iResponder);

	ZooLib::ZThread* fThread;
	ZServer* fServer;
	ZRef<ZStreamerRWCon> fStreamerRWCon;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZServer_Simple

class ZServer_Simple : public ZServer
	{
public:
	ZServer_Simple();

// From ZServer
	virtual Responder* CreateResponder();

// Our protocol
	virtual void DoRun(ZRef<ZStreamerRWCon> iConnection) = 0;

private:
	class Responder;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZServer_Callback

class ZServer_Callback : public ZServer
	{
public:
	typedef void (*Callback_t)(void* iRefcon, ZRef<ZStreamerRWCon> iConnection);
	ZServer_Callback(Callback_t iCallback, void* iRefcon);

// From ZServer
	virtual Responder* CreateResponder();

private:
	Callback_t fCallback;
	void* fRefcon;

	class Responder;
	};

#endif // __ZServer__
