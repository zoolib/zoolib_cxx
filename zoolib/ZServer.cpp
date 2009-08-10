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

//using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZServer::StreamerListener

class ZServer::StreamerListener : public ZStreamerListener
	{
public:
	StreamerListener(ZRef<ZServer> iServer, ZRef<ZStreamerRWFactory> iFactory);
	virtual ~StreamerListener();

// From StreamerListener
	virtual bool Connected(ZRef<ZStreamerRW> iStreamer);

private:
	void pStop();

	ZRefWeak<ZServer> fServer;
	friend class ZServer;
	};

ZServer::StreamerListener::StreamerListener(
	ZRef<ZServer> iServer, ZRef<ZStreamerRWFactory> iFactory)
:	ZStreamerListener(iFactory),
	fServer(iServer)
	{}

ZServer::StreamerListener::~StreamerListener()
	{}

bool ZServer::StreamerListener::Connected(ZRef<ZStreamerRW> iStreamerRW)
	{
	if (ZRef<ZServer> theServer = fServer)
		{
		theServer->pConnected(iStreamerRW);
		return true;
		}
	return false;
	}

void ZServer::StreamerListener::pStop()
	{
	fServer.Clear();
	fFactory->Cancel();
	ZActor::Wake();
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
	if (ZRef<StreamerListener> theStreamerListener = fStreamerListener)
		{
		fStreamerListener.Clear();
		theStreamerListener->pStop();
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
	delete this;
	}

void ZServer::Task_Finished(ZRef<ZTask> iTask)
	{
	ZRef<Responder> theResponder = ZRefStaticCast<Responder>(iTask);
	fResponders.Erase(theResponder);
	}

void ZServer::StartListener(ZRef<ZStreamerRWFactory> iFactory)
	{
	ZAssert(!fStreamerListener);
	ZAssert(iFactory);

	fStreamerListener = new StreamerListener(this, iFactory);

	sStartActorRunner(fStreamerListener);
	}

void ZServer::StopListener()
	{
	if (ZRef<StreamerListener> theStreamerListener = fStreamerListener)
		{
		fStreamerListener.Clear();
		theStreamerListener->pStop();
		}
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
			fResponders.Add(theResponder);
			theResponder->Handle(iStreamerRW);
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
	{ return ZRefStaticCast<ZServer>(this->GetOwner()); }

NAMESPACE_ZOOLIB_END
