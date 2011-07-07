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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZServer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZServer

ZServer::ZServer()
	{}

ZServer::~ZServer()
	{
	ZAssert(fResponders.Empty());
	ZAssert(not fWorker);
	}

void ZServer::Finalize()
	{
	this->StopListenerWait();

	fResponders.Clear();

	if (this->FinishFinalize())
		delete this;
	}

void ZServer::StartListener(ZRef<ZCaller> iCaller, ZRef<ZStreamerRWFactory> iFactory)
	{
	ZAcqMtx acq(fMtx);

	ZAssert(iCaller);
	ZAssert(iFactory);
	ZAssert(not fWorker);
	ZAssert(not fFactory);

	fFactory = iFactory;
	fWorker = new ZWorker
		(sCallable(sWeakRef(this), &ZServer::pListener_Work),
		(sCallable(sWeakRef(this), &ZServer::pListener_Finished)));

	fWorker->Attach(iCaller);
	fWorker->Wake();
	}

void ZServer::StopListener()
	{
	ZGuardRMtx guard(fMtx);
	if (ZRef<ZStreamerRWFactory> theFactory = fFactory)
		{
		fFactory.Clear();
		theFactory->Cancel();
		}
	}

void ZServer::StopListenerWait()
	{
	this->StopListener();

	ZAcqMtx acq(fMtx);
	while (fWorker)
		fCnd.Wait(fMtx);
	}

void ZServer::KillResponders()
	{
	for (ZSafeSetIterConst<ZRef<Responder> > i = fResponders; /*no test*/; /*no inc*/)
		{
		if (ZQ<ZRef<Responder>,false> theNQ = i.QReadInc())
			break;
		else
			theNQ.Get()->Kill();
		}
	}

void ZServer::KillRespondersWait()
	{
	this->KillResponders();

	ZAcqMtx acq(fMtx);
	while (!fResponders.Empty())
		fCnd.Wait(fMtx);
	}

ZRef<ZStreamerRWFactory> ZServer::GetFactory()
	{
	ZAcqMtx acq(fMtx);
	return fFactory;
	}

ZSafeSetIterConst<ZRef<ZServer::Responder> > ZServer::GetResponders()
	{ return fResponders; }

bool ZServer::pListener_Work(ZRef<ZWorker> iWorker)
	{
	iWorker->Wake();
	ZGuardRMtx guard(fMtx);
	if (ZRef<ZStreamerRWFactory> theFactory = fFactory)
		{
		guard.Release();
		if (ZRef<ZStreamerRW> iStreamerRW = theFactory->MakeStreamerRW())
			{
			if (ZRef<Responder> theResponder = this->MakeResponder())
				{
				fResponders.Insert(theResponder);
				theResponder->Respond(iStreamerRW);
				}
			}
		return true;
		}
	return false;
	}

void ZServer::pListener_Finished(ZRef<ZWorker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	ZAssert(!fFactory);
	fWorker.Clear();
	fCnd.Broadcast();
	}

void ZServer::pResponderFinished(ZRef<Responder> iResponder)
	{
	ZAcqMtx acq(fMtx);
	fResponders.Erase(iResponder);
	fCnd.Broadcast();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZServer::Responder

ZServer::Responder::Responder(ZRef<ZServer> iServer)
:	fServer(iServer)
	{}

ZServer::Responder::~Responder()
	{}

void ZServer::Responder::Kill()
	{}

ZRef<ZServer> ZServer::Responder::GetServer()
	{ return fServer.Get(); }

void ZServer::Responder::pFinished()
	{
	if (ZRef<ZServer> theServer = fServer.Get())
		theServer->pResponderFinished(this);
	}

} // namespace ZooLib
