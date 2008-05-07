/* ------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZServer.h"
#include "ZMemory.h"
#include "ZThreadSimple.h"

#include "ZCompat_algorithm.h"

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZServer

ZServer::ZServer()
:	fMutex("ZServer::fMutex"),
	fCondition("ZServer::fCondition"),
	fSem(0, "ZServer::fSem"),
	fMaxResponders(0),
	fAcceptedCount(0)
	{}

ZServer::~ZServer()
	{
	this->StopWaitingForConnections();

	ZMutexLocker locker(fMutex);
	// If we get deleted before our responders, then tell them we've gone
	for (vector<Responder*>::iterator i = fResponders.begin(); i != fResponders.end(); ++i)
		(*i)->ServerDeleted(this);
	fResponders.erase(fResponders.begin(), fResponders.end());
	}

void ZServer::StartWaitingForConnections(ZRef<ZStreamerRWConFactory> iFactory)
	{
	ZAssert(!fMutex.IsLocked());

	ZMutexLocker locker(fMutex);
	ZAssert(!fFactory);

	fFactory = iFactory;

	(new ZThreadSimple<ZServer*>(sRunThread, this, "ZServer::sRunThread"))->Start();

	// Wait for the thread to start before we return.
	fSem.Wait(1);
	}

void ZServer::StopWaitingForConnections()
	{
	ZAssert(!fMutex.IsLocked());

	ZMutexLocker locker(fMutex);
	if (ZRef<ZStreamerRWConFactory> theFactory = fFactory)
		{
		fFactory.Clear();
		theFactory->Cancel();
		fCondition.Broadcast();
		locker.Release();

		// Wait for listen thread to exit before we return
		fSem.Wait(1);
		}
	}

void ZServer::KillAllConnections()
	{
	// First of all, cancel our listener thread, if any.
	this->StopWaitingForConnections();

	// Now, tell all our responders to bail
	ZMutexLocker locker(fMutex);
	while (!fResponders.empty())
		{
		Responder* aResponder = *(fResponders.begin());
		aResponder->KillConnection();
		fCondition.Wait(fMutex);
		}
	}

void ZServer::KillAllNoWait()
	{
	ZMutexLocker locker(fMutex);
	if (ZRef<ZStreamerRWConFactory> theFactory = fFactory)
		{
		fFactory.Clear();
		theFactory->Cancel();
		fCondition.Broadcast();
		}

	// Now, tell all our responders to bail
	for (vector<Responder*>::iterator i = fResponders.begin(); i != fResponders.end(); ++i)
		(*i)->KillConnection();
	}

void ZServer::SetMaxResponders(size_t iCount)
	{
	ZMutexLocker locker(fMutex);
	fMaxResponders = iCount;
	fCondition.Broadcast();
	}

ZRef<ZStreamerRWConFactory> ZServer::GetFactory()
	{ return fFactory; }

void ZServer::ResponderFinished(Responder* iResponder)
	{
	ZMutexLocker locker(fMutex);

	vector<Responder*>::iterator theIter = find(fResponders.begin(), fResponders.end(), iResponder);
	ZAssert(theIter != fResponders.end());
	fResponders.erase(theIter);
	fCondition.Broadcast();
	}

void ZServer::RunThread()
	{
	// AG 98-02-05. consecutiveErrorCount is a preemptive strike against nasty situations that
	// may arise where the networking subsystem starts failing consistently. If we're running
	// at deferred task time on the Mac, just Yielding is not enough -- we have to sleep or be blocked
	// in some other way if we're ever gonna leave Deferred Task time.
	int consecutiveErrorCount = 0;

	fSem.Signal(1);
	ZMutexLocker locker(fMutex);

	for (;;)
		{
		while (fFactory && fMaxResponders && fResponders.size() >= fMaxResponders)
			fCondition.Wait(fMutex);

		ZRef<ZStreamerRWConFactory> theFactory = fFactory;

		if (!theFactory)
			break;

		locker.Release();

		if (ZRef<ZStreamerRWCon> theStreamerRWCon = theFactory->MakeStreamerRWCon())
			{
			if (Responder* theResponder = this->CreateResponder())
				{
				locker.Acquire();
				fResponders.push_back(theResponder);
				fCondition.Broadcast();
				++fAcceptedCount;
				locker.Release();

				theResponder->HandleConnection(this, theStreamerRWCon);
				}
			}
		else
			{
			if (++consecutiveErrorCount == 10)
				{
				consecutiveErrorCount = 0;
				ZThread::sSleep(1000);
				}
			}
		locker.Acquire();
		}
	fSem.Signal(1);
	}

void ZServer::sRunThread(ZServer* iServer)
	{ iServer->RunThread(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZServer::Responder

ZServer::Responder::Responder()
:	fServer(nil),
	fThread(nil)
	{}

ZServer::Responder::~Responder()
	{
	ZAssert(fThread == nil);
	}

void ZServer::Responder::HandleConnection(ZServer* iServer, ZRef<ZStreamerRWCon> iStreamerRWCon)
	{
	ZAssert(iServer);
	ZAssert(iStreamerRWCon);
	ZAssert(!fServer);
	ZAssert(!fStreamerRWCon);

	fServer = iServer;
	fStreamerRWCon = iStreamerRWCon;

	// Create and start the thread that will actually service the socket
	fThread = new ZThreadSimple<Responder*>(sRunThread, this, "ZServer::Responder::sRunThread");
	fThread->Start();
	}

void ZServer::Responder::ServerDeleted(ZServer* iServer)
	{
	ZAssert(iServer == fServer);
	fServer = nil;
	}

void ZServer::Responder::KillConnection()
	{
	// This will cause any pending comms to abort, returning an error
	// to our thread which is reading/writing the endpoint. So any code
	// which gets such an error should unwind explicitly or by throwing an
	// exception.
	fStreamerRWCon->Abort();
	}

void ZServer::Responder::RunThread()
	{
	ZAssert(fStreamerRWCon);
	try
		{
		this->DoRun(fStreamerRWCon);
		}
	catch (...)
		{
		fStreamerRWCon->Abort();
		}

	if (fServer)
		fServer->ResponderFinished(this);

	fThread = nil;

	delete this;
	}

void ZServer::Responder::sRunThread(Responder* iResponder)
	{ iResponder->RunThread(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZServer_Simple::Responder

class ZServer_Simple::Responder : public ZServer::Responder
	{
public:
	Responder(ZServer_Simple* iServer);

// From ZServer::Responder
	virtual void DoRun(ZRef<ZStreamerRWCon> iStreamerRWCon);

private:
	ZServer_Simple* fServer;
	};

ZServer_Simple::Responder::Responder(ZServer_Simple* iServer)
:	fServer(iServer)
	{}

void ZServer_Simple::Responder::DoRun(ZRef<ZStreamerRWCon> iStreamerRWCon)
	{ fServer->DoRun(iStreamerRWCon); }

// =================================================================================================
#pragma mark -
#pragma mark * ZServer_Simple

ZServer_Simple::ZServer_Simple()
	{}

ZServer::Responder* ZServer_Simple::CreateResponder()
	{ return new Responder(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZServer_Callback::Responder

class ZServer_Callback::Responder : public ZServer::Responder
	{
public:
	Responder(Callback_t iCallback, void* iRefcon);

// From ZServer::Responder
	virtual void DoRun(ZRef<ZStreamerRWCon> iStreamerRWCon);

private:
	Callback_t fCallback;
	void* fRefcon;
	};

ZServer_Callback::Responder::Responder(Callback_t iCallback, void* iRefcon)
:	fCallback(iCallback),
	fRefcon(iRefcon)
	{}

void ZServer_Callback::Responder::DoRun(ZRef<ZStreamerRWCon> iStreamerRWCon)
	{ fCallback(fRefcon, iStreamerRWCon); }

// =================================================================================================
#pragma mark -
#pragma mark * ZServer_Callback

ZServer_Callback::ZServer_Callback(Callback_t iCallback, void* iRefcon)
:	fCallback(iCallback),
	fRefcon(iRefcon)
	{}

ZServer::Responder* ZServer_Callback::CreateResponder()
	{ return new Responder(fCallback, fRefcon); }
