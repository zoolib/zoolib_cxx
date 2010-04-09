/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZServer.h"
#include "zoolib/ZStreamerListener.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZServer::StreamerListener

class ZServer::StreamerListener
:	public ZStreamerListener
,	public ZTask
	{
public:
	StreamerListener(ZRef<ZServer> iServer, ZRef<ZStreamerRWFactory> iFactory);
	virtual ~StreamerListener();

// From StreamerListener
	virtual void ListenFinished();

	virtual bool Connected(ZRef<ZStreamerRW> iStreamer);

// From ZTask
	virtual void Kill();
	};

ZServer::StreamerListener::StreamerListener(
	ZRef<ZServer> iServer, ZRef<ZStreamerRWFactory> iFactory)
:	ZStreamerListener(iFactory)
,	ZTask(iServer)
	{}

ZServer::StreamerListener::~StreamerListener()
	{}

void ZServer::StreamerListener::ListenFinished()
	{
	ZTask::pFinished();
	ZStreamerListener::ListenFinished();
	}

bool ZServer::StreamerListener::Connected(ZRef<ZStreamerRW> iStreamerRW)
	{
	if (ZRef<ZTaskMaster> theTaskMaster = this->GetTaskMaster())
		{
		theTaskMaster.StaticCast<ZServer>()->pConnected(iStreamerRW);
		return true;
		}
	return false;
	}

void ZServer::StreamerListener::Kill()
	{
	ZStreamerListener::Stop();
	ZWorker::Wake();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZServer

ZServer::ZServer()
	{}

ZServer::~ZServer()
	{
	ZAssert(fResponders.Empty());
	ZAssert(!fStreamerListener);
	}

void ZServer::Finalize()
	{
	{ // Scope for locker
	ZGuardMtx locker(fMtx);
	if (fStreamerListener)
		{
		fStreamerListener->Kill();
		while (fStreamerListener)
			fCnd.Wait(fMtx);
		}

	for (ZSafeSetIter<ZRef<Responder> > i = fResponders; /*no test*/; /*no inc*/)
		{
		if (ZRef<Responder> theResponder = i.ReadErase())
			this->pDetachTask(theResponder);
		else
			break;
		}

	if (this->GetRefCount() != 1)
		{
		this->FinalizationComplete();
		return;
		}
	this->FinalizationComplete();
	}

	delete this;
	}

void ZServer::Task_Finished(ZRef<ZTask> iTask)
	{
	ZGuardMtx locker(fMtx);

	if (fStreamerListener == iTask)
		{
		fStreamerListener.Clear();
		}
	else if (ZRef<Responder> theResponder = ZRefDynamicCast<Responder>(iTask))
		{
		fResponders.Erase(theResponder);
		}

	fCnd.Broadcast();
	}

void ZServer::StartListener(ZRef<ZStreamerRWFactory> iFactory)
	{
	ZGuardMtx locker(fMtx);

	ZAssert(!fStreamerListener);
	ZAssert(iFactory);

	fStreamerListener = new StreamerListener(this, iFactory);

	sStartWorkerRunner(fStreamerListener);
	}

void ZServer::StopListener()
	{
	ZRef<StreamerListener> theSL;

	{
	ZGuardMtx locker(fMtx);
	theSL = fStreamerListener;
	}

	if (theSL)
		theSL->Kill();
	}

void ZServer::StopListenerWait()
	{
	this->StopListener();

	ZGuardMtx locker(fMtx);
	while (fStreamerListener)
		fCnd.Wait(fMtx);
	}

void ZServer::KillResponders()
	{
	for (ZSafeSetIterConst<ZRef<Responder> > i = fResponders; /*no test*/; /*no inc*/)
		{
		if (ZRef<Responder> theResponder = i.ReadInc())
			theResponder->Kill();
		else
			break;
		}
	}

void ZServer::KillRespondersWait()
	{
	this->KillResponders();

	ZGuardMtx locker(fMtx);
	while (!fResponders.Empty())
		fCnd.Wait(fMtx);
	}

ZRef<ZStreamerRWFactory> ZServer::GetFactory()
	{
	if (ZRef<StreamerListener> theStreamerListener = fStreamerListener)
		return theStreamerListener->GetFactory();
	return ZRef<ZStreamerRWFactory>();
	}

ZSafeSetIterConst<ZRef<ZServer::Responder> > ZServer::GetResponders()
	{ return fResponders; }

void ZServer::pConnected(ZRef<ZStreamerRW> iStreamerRW)
	{
	if (iStreamerRW)
		{
		if (ZRef<Responder> theResponder = this->MakeResponder())
			{
			fResponders.Insert(theResponder);
			theResponder->Respond(iStreamerRW);
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZServer::Responder

ZServer::Responder::Responder(ZRef<ZServer> iServer)
:	ZTask(iServer)
	{}

ZServer::Responder::~Responder()
	{}

ZRef<ZServer> ZServer::Responder::GetServer()
	{ return this->GetTaskMaster().StaticCast<ZServer>(); }

NAMESPACE_ZOOLIB_END
