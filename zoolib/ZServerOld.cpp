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
#include "zoolib/ZServerOld.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZServerOld

ZServerOld::ZServerOld()
	{}

ZServerOld::~ZServerOld()
	{
	ZAssert(fResponders.Empty());
	ZAssert(not fWorker);
	}

void ZServerOld::Finalize()
	{
	this->StopListenerWait();

	fResponders.Clear();

	ZCounted::Finalize();
	}

ZRef<ZStreamerRWFactory> ZServerOld::GetListener()
	{
	ZAcqMtx acq(fMtx);
	return fFactory;
	}

void ZServerOld::StartListener(ZRef<ZCaller> iCaller, ZRef<ZStreamerRWFactory> iFactory)
	{
	ZAcqMtx acq(fMtx);

	ZAssert(iCaller);
	ZAssert(iFactory);
	ZAssert(not fWorker);
	ZAssert(not fFactory);

	fFactory = iFactory;

	fWorker = new ZWorker
		(sCallable(sWeakRef(this), &ZServerOld::pListener_Work),
		(sCallable(sWeakRef(this), &ZServerOld::pListener_Finished)));

	fWorker->Attach(iCaller);

	fWorker->Wake();
	}

void ZServerOld::StopListener()
	{
	ZGuardRMtx guard(fMtx);
	if (ZRef<ZStreamerRWFactory> theFactory = fFactory)
		{
		fFactory.Clear();
		theFactory->Cancel();
		}
	}

void ZServerOld::StopListenerWait()
	{
	this->StopListener();

	ZAcqMtx acq(fMtx);
	while (fWorker)
		fCnd.Wait(fMtx);
	}

void ZServerOld::KillResponders()
	{
	for (ZSafeSetIterConst<ZRef<Responder> > i = fResponders; /*no test*/; /*no inc*/)
		{
		if (ZQ<ZRef<Responder>,false> theNQ = i.QReadInc())
			break;
		else
			theNQ.Get()->Kill();
		}
	}

void ZServerOld::KillRespondersWait()
	{
	this->KillResponders();

	ZAcqMtx acq(fMtx);
	while (!fResponders.Empty())
		fCnd.Wait(fMtx);
	}

ZRef<ZStreamerRWFactory> ZServerOld::GetFactory()
	{
	ZAcqMtx acq(fMtx);
	return fFactory;
	}

ZSafeSetIterConst<ZRef<ZServerOld::Responder> > ZServerOld::GetResponders()
	{ return fResponders; }

bool ZServerOld::pListener_Work(ZRef<ZWorker> iWorker)
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

void ZServerOld::pListener_Finished(ZRef<ZWorker> iWorker)
	{
	ZAcqMtx acq(fMtx);
	ZAssert(!fFactory);
	fWorker.Clear();
	fCnd.Broadcast();
	}

void ZServerOld::pResponderFinished(ZRef<Responder> iResponder)
	{
	ZAcqMtx acq(fMtx);
	fResponders.Erase(iResponder);
	fCnd.Broadcast();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZServerOld::Responder

ZServerOld::Responder::Responder(ZRef<ZServerOld> iServer)
:	fServer(iServer)
	{}

ZServerOld::Responder::~Responder()
	{}

void ZServerOld::Responder::Kill()
	{}

ZRef<ZServerOld> ZServerOld::Responder::GetServer()
	{ return fServer.Get(); }

void ZServerOld::Responder::pFinished()
	{
	if (ZRef<ZServerOld> theServer = fServer.Get())
		theServer->pResponderFinished(this);
	}

} // namespace ZooLib
