/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/Callable_PMF.h"

#include "zoolib/datonset/ZDatonSet.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_Any_JSON.h"
#include "zoolib/ZVal_Any.h"

#include "zoolib/dataspace/WrappedDatonSetRemoter.h"

namespace ZooLib {
namespace Dataspace {

using namespace ZUtil_STL;
using ZDatonSet::DatonSet;
using std::set;
using std::swap;

/*
Three messages, and maybe a keepalive.

X When our fCallable_PullSuggested_Self is invoked, we send a "PullSuggested" message to the other side.

X When we receive a "PullSuggested" message we call fCallable_PullSuggested, passing our fCallable_PullFrom_Self.

X When our fCallable_PullFrom_Self is invoked, we send a "PullFrom" message to the other side,
  and XXX block waiting for our reader to do its thing with "PullFromResponse". I think we can
  safely bail after a timeout if we want.

X When we receive a "PullFrom" message, we invoke each of the fCallable_PullFroms with the Event,
  and send the info back over the wire in a "PullFromResponse".

X When we get a "PullFromResponse" message, we get that data across to the writer thread at XXX. If we
  don't have an XXX, do nothing.

If we feel like it, send a ping.

If we haven't heard anything in a while, drop the connection.

We can decouple received PullSuggested and sends of "PullFrom", so callers aren't blocked by network
latency.
*/

static void spSendMessage(const ZMap_Any& iMessage, const ChanW_Bin& iChanW)
	{
	if (ZLOGF(w, eDebug))
		{
		w << "Send: ";
		ZUtil_Any_JSON::sWrite(iMessage, w);
		}
	ZUtil_Any_JSON::sWrite(iMessage, ZStrimW_StreamUTF8(iChanW));
	sFlush(iChanW);
	}

static ZMap_Any spReadMessage(const ZRef<ChannerR_Bin>& iChannerR)
	{
	ZRef<ZStreamerR> theSR = iChannerR.DynamicCast<ZStreamerR>();
	ZMap_Any result = ZUtil_Any_JSON::sQRead(theSR).Get().Get<ZMap_Any>();
	if (ZLOGF(w, eDebug))
		{
		w << "Received: ";
		ZUtil_Any_JSON::sWrite(result, w);
		}
	return result;
	}

static ZRef<Event> spEventFromSeq(const ZSeq_Any& iSeq)
	{
	const size_t theValue = sCoerceInt(iSeq[0]);
	ZRef<Event> theLeft, theRight;
	if (iSeq.Count() > 1)
		{
		theLeft = spEventFromSeq(iSeq.Get<ZSeq_Any>(1));
		theRight = spEventFromSeq(iSeq.Get<ZSeq_Any>(2));
		}
	return Event::sMake(theValue, theLeft, theRight);
	}

static ZSeq_Any spSeqFromEvent(const ZRef<Event>& iEvent)
	{
	ZSeq_Any theSeq;
	theSeq.Append(iEvent->Value());
	if (ZRef<Event> theLeft = iEvent->Left())
		{
		theSeq.Append(spSeqFromEvent(theLeft));
		theSeq.Append(spSeqFromEvent(iEvent->Right()));
		}
	return theSeq;
	}

// =================================================================================================
// MARK: - WrappedDatonSetRemoter

WrappedDatonSetRemoter::WrappedDatonSetRemoter(
	const ZRef<ChannerRWFactory>& iChannerRWFactory,
	const ZRef<Callable_PullSuggested>& iCallable_PullSuggested_Other)
:	fChannerRWFactory(iChannerRWFactory)
,	fCallable_PullSuggested_Other(iCallable_PullSuggested_Other)
,	fConnectionBusy(false)
,	fPullFromPointer(nullptr)
	{
	ZAssert(fChannerRWFactory);
	}

WrappedDatonSetRemoter::WrappedDatonSetRemoter(
	const ChannerRW& iChannerRW,
	const ZRef<Callable_PullSuggested>& iCallable_PullSuggested_Other)
:	fCallable_PullSuggested_Other(iCallable_PullSuggested_Other)
,	fChannerRWQ(iChannerRW)
,	fConnectionBusy(false)
,	fPullFromPointer(nullptr)
	{}

void WrappedDatonSetRemoter::Initialize()
	{
	ZCounted::Initialize();

	fCallable_PullSuggested_Self = sCallable(sWeakRef(this), &WrappedDatonSetRemoter::pPullSuggested);
	fCallable_PullFrom_Self = sCallable(sWeakRef(this), &WrappedDatonSetRemoter::pPullFrom);

	ZThread::sCreate_T<ZRef<WrappedDatonSetRemoter> >(&WrappedDatonSetRemoter::spRead, this);
	}

ZRef<Callable_PullSuggested> WrappedDatonSetRemoter::GetCallable_PullSuggested()
	{ return fCallable_PullSuggested_Self; }

void WrappedDatonSetRemoter::pPullSuggested(const ZRef<Callable_PullFrom>& iCallable_PullFrom)
	{
	ZGuardMtxR guard(fMtxR);
	sInsert(fCallables_PullFrom, iCallable_PullFrom);

	for (;;)
		{
		try
			{
			guard.Release();
			ZQ<ChannerRW> theChannerRWQ = this->pQEnsureChannerRW();
			guard.Acquire();

			if (not theChannerRWQ)
				break;

			const ChanW_Bin& theChanW = sGetChan(theChannerRWQ->f1);

			while (fConnectionBusy)
				fCnd.Wait(fMtxR);

			SaveSetRestore<bool> theSSR(fConnectionBusy, true);

			guard.Release();

			ZMap_Any theMessage;
			theMessage.Set("What", "PullSuggested");
			spSendMessage(theMessage, theChanW);

			guard.Acquire();

			// Update state
			break;
			}
		catch (...)
			{
			sClear(fChannerRWQ);
			}
		}

	fConnectionBusy = false;

	fCnd.Broadcast();
	}

void WrappedDatonSetRemoter::pPullFrom(
	ZRef<Event> iEvent, ZRef<ZDatonSet::Deltas>& oDeltas, ZRef<Event>& oEvent)
	{
	ZGuardMtxR guard(fMtxR);

	for (;;)
		{
		try
			{
			guard.Release();
			ZQ<ChannerRW> theChannerRWQ = this->pQEnsureChannerRW();
			guard.Acquire();

			if (not theChannerRWQ)
				break;

			const ChanW_Bin& theChanW = sGetChan(theChannerRWQ->f1);

			while (fConnectionBusy)
				fCnd.Wait(fMtxR);

			SaveSetRestore<bool> theSSR(fConnectionBusy, true);

			ZMap_Any theMessage;
			theMessage.Set("What", "PullFrom");
			theMessage.Set("Event", spSeqFromEvent(iEvent));

			PullFrom_t thePullFrom;
			fPullFromPointer = &thePullFrom;

			ZRelMtxR rel(fMtxR);
			// Send a "PullFrom" message to the other side over theChanW, include iEvent.
			spSendMessage(theMessage, theChanW);

			ZAcqMtxR acq(fMtxR);

			// Wait till a PullFromResponse has come in (any PullfromResponse?), and
			while (fPullFromPointer)
				fCnd.Wait(fMtxR);

			oDeltas = thePullFrom.f0;
			oEvent = thePullFrom.f1;

			// write the info into oDeltas and oEvent.
			break;
			}
		catch (...)
			{
			sClear(fChannerRWQ);
			}
		}

	}

static ZRef<ZDatonSet::Delta> spDeltaFromSeq(const ZSeq_Any& iSeq)
	{
	using namespace ZDatonSet;
	Delta::Statements_t theStatements_t;
	foreachi (iter, iSeq)
		{
		const ZSeq_Any& theStatementSeq = iter->Get<ZSeq_Any>();
		const Daton& theDaton = theStatementSeq[0].Get<ZData_Any>();
		const bool theBool = theStatementSeq[1].Get<bool>();
		sInsertMust(theStatements_t, theDaton, theBool);
		}
	return new Delta(*&theStatements_t);
	}

static ZSeq_Any spSeqFromDelta(const ZRef<ZDatonSet::Delta>& iDelta)
	{
	using namespace ZDatonSet;
	ZSeq_Any theSeq;

	foreachi (iter, iDelta->GetStatements())
		{
		ZSeq_Any& theStatementSeq = theSeq.Mut<ZSeq_Any>(theSeq.Count());
		theStatementSeq.Append(iter->first.GetData());
		theStatementSeq.Append(iter->second);
		}
	return theSeq;
	}

void WrappedDatonSetRemoter::pRead()
	{
	ZThread::sSetName("WrappedDatonSetRemoter");

	ZGuardMtxR guard(fMtxR);

	for (;;)
		{
		guard.Release();
		ZQ<ChannerRW> theChannerRWQ = this->pQEnsureChannerRW();
		guard.Acquire();

		if (not theChannerRWQ)
			{
			// No Channer was available
			if (fChannerRWFactory)
				continue;
			// There's no factory to make one, bail
			break;
			}

		guard.Release();

		const ZMap_Any theMessage = spReadMessage(theChannerRWQ->f0);
		const string8& theWhat = theMessage.Get<string8>("What");

		if (false)
			{}
		else if (theWhat == "PullSuggested")
			{
			try { sCall(fCallable_PullSuggested_Other, fCallable_PullFrom_Self); }
			catch (...) {}
			}
		else if (theWhat == "PullFrom")
			{
			using namespace ZDatonSet;
			// We need the Event from theMessage
			ZRef<Event> theEvent = spEventFromSeq(theMessage.Get<ZSeq_Any>("Event"));
			// Gather the information from all our pullfroms (if any) and send it over the wire.

			guard.Acquire();

			set<ZRef<Callable_PullFrom> > theCallables_PullFrom;
			swap(theCallables_PullFrom, fCallables_PullFrom);

			guard.Release();

			foreachv (ZRef<Callable_PullFrom> theCallable, theCallables_PullFrom)
				{
				ZRef<ZDatonSet::Deltas> theDeltas;
				ZRef<Event> resultEvent;
				theCallable->Call(theEvent, theDeltas, resultEvent);

				ZMap_Any theMessage;
				theMessage.Set("What", "PullFromResponse");
				theMessage.Set("Event", spSeqFromEvent(resultEvent));
				ZSeq_Any& theDeltasSeq = theMessage.Mut<ZSeq_Any>("Deltas");
				foreachi (iterDelta, theDeltas->GetVector())
					{
					ZMap_Any& theEntry = theDeltasSeq.Mut<ZMap_Any>(theDeltasSeq.Count());
					theEntry.Set("Event", spSeqFromEvent(iterDelta->first));
					theEntry.Set("Delta", spSeqFromDelta(iterDelta->second));
					}

				guard.Acquire();

				while (fConnectionBusy)
					fCnd.Wait(fMtxR);

				SaveSetRestore<bool> theSSR(fConnectionBusy, true);
				// Send the gathered information, as a PullFromResponse message

				guard.Release();
				spSendMessage(theMessage, sGetChan(theChannerRWQ->f1));
				// Do the one and only
				break;
				}
			}
		else if (theWhat == "PullFromResponse")
			{
			using namespace ZDatonSet;
			// We should be blocked up in pPullFrom, get the data across from here to there
			const ZRef<Event> theEvent = spEventFromSeq(theMessage.Get<ZSeq_Any>("Event"));
			Vector_Event_Delta_t theVED;
			foreachi (iter, theMessage.Get<ZSeq_Any>("Deltas"))
				{
				const ZMap_Any theEntry = iter->Get<ZMap_Any>();
				ZRef<Event> anEvent = spEventFromSeq(theEntry.Get<ZSeq_Any>("Event"));
				ZRef<Delta> aDelta = spDeltaFromSeq(theEntry.Get<ZSeq_Any>("Delta"));
				theVED.push_back(Event_Delta_t(anEvent, aDelta));
				}
			ZAcqMtxR acq2(fMtxR);
			if (fPullFromPointer)
				{
				fPullFromPointer->f0 = new Deltas(*&theVED);
				fPullFromPointer->f1 = theEvent;
				fPullFromPointer = nullptr;
				fCnd.Broadcast();
				}
			}

		guard.Acquire();
		}
	}

void WrappedDatonSetRemoter::spRead(ZRef<WrappedDatonSetRemoter> iWDSR)
	{
	try
		{
		iWDSR->pRead();
		}
	catch (std::exception& ex)
		{
		if (ZLOGF(w, eDebug))
			w << "Caught: " << ex.what();
		}
	}

ZQ<ChannerRW> WrappedDatonSetRemoter::pQEnsureChannerRW()
	{
	ZGuardMtxR guard(fMtxR);

	if (not fChannerRWQ)
		{
		guard.Release();
		ZQ<ChannerRW> theChannerRWQ = sCall(fChannerRWFactory);
		guard.Acquire();
		fChannerRWQ = theChannerRWQ;
		}
	return fChannerRWQ;
	}

} // namespace Dataspace
} // namespace ZooLib
