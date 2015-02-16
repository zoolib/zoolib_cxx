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
#include "zoolib/Log.h"
#include "zoolib/Util_Any.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_Any_JSONB.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/DatonSet/DatonSet.h"
#include "zoolib/DatonSet/WrappedDatonSetRemoter.h"

namespace ZooLib {
namespace DatonSet {

using namespace Util_STL;
using std::set;
using std::swap;

/*
Three messages, and maybe a keepalive.

X When our fCallable_PullSuggested_Self is invoked, we send a "PullSuggested" message to
  the other side.

X When we receive a "PullSuggested" message we call fCallable_PullSuggested_Other, passing
  our fCallable_PullFrom_Self.

X When our fCallable_PullFrom_Self is invoked, we send a "PullFrom" message to the other side,
  and XXX block waiting for our reader to do its thing with "PullFromResponse". I think we can
  safely bail after a timeout if we want.

X When we receive a "PullFrom" message, we invoke each of the fCallable_PullFroms with the Event,
  and send the info back over the wire in a "PullFromResponse".

X When we get a "PullFromResponse" message, we get that data across to the writer thread
  at XXX (above). If we don't have an XXX, do nothing.

If we feel like it, send a ping.

If we haven't heard anything in a while, drop the connection.

We could decouple received PullSuggested and sends of "PullFrom", so callers aren't
blocked by network latency.
*/

// =================================================================================================
#pragma mark -
#pragma mark

static void spSendMessage(const Map_Any& iMessage, const ChanW_Bin& iChanW)
	{
	Util_Any_JSONB::sWrite(iMessage, iChanW);
	sFlush(iChanW);
	}

static Map_Any spReadMessage(const ZRef<ChannerR_Bin>& iChannerR)
	{
	ZQ<Val_Any> theQ = Util_Any_JSONB::sQRead(iChannerR);
	if (not theQ)
		sThrow_ExhaustedR();

	return theQ->Get<Map_Any>();
	}

static ZRef<Event> spEventFromSeq(const Seq_Any& iSeq)
	{
	const size_t theValue = sCoerceInt(iSeq[0]);
	ZRef<Event> theLeft, theRight;
	if (iSeq.Count() > 1)
		{
		theLeft = spEventFromSeq(iSeq.Get<Seq_Any>(1));
		theRight = spEventFromSeq(iSeq.Get<Seq_Any>(2));
		}
	return Event::sMake(theValue, theLeft, theRight);
	}

static Seq_Any spSeqFromEvent(const ZRef<Event>& iEvent)
	{
	Seq_Any theSeq;
	theSeq.Append(iEvent->Value());
	if (ZRef<Event> theLeft = iEvent->Left())
		{
		theSeq.Append(spSeqFromEvent(theLeft));
		theSeq.Append(spSeqFromEvent(iEvent->Right()));
		}
	return theSeq;
	}

static ZRef<Delta> spDeltaFromSeq(const Seq_Any& iSeq)
	{
	Delta::Statements_t theStatements_t;
	foreachi (iter, iSeq)
		{
		const Seq_Any& theStatementSeq = iter->Get<Seq_Any>();
		const bool theBool = theStatementSeq[0].Get<bool>();
		const Daton& theDaton = theStatementSeq[1].Get<Data_Any>();
		sInsertMust(theStatements_t, theDaton, theBool);
		}
	return new Delta(*&theStatements_t);
	}

static Seq_Any spSeqFromDelta(const ZRef<Delta>& iDelta)
	{
	Seq_Any theSeq;

	foreachi (iter, iDelta->GetStatements())
		{
		Seq_Any& theStatementSeq = theSeq.Mut<Seq_Any>(theSeq.Count());
		theStatementSeq.Append(iter->second);
		theStatementSeq.Append(iter->first.GetData());
		}
	return theSeq;
	}

// =================================================================================================
#pragma mark -
#pragma mark WrappedDatonSetRemoter

WrappedDatonSetRemoter::WrappedDatonSetRemoter(
	const ZRef<Factory_ChannerRW_Bin>& iFactory,
	const ZRef<Callable_PullSuggested>& iCallable_PullSuggested_Other)
:	fFactory(iFactory)
,	fCallable_PullSuggested_Other(iCallable_PullSuggested_Other)
,	fConnectionBusy(false)
,	fPullFromPointer(nullptr)
	{
	ZAssert(fFactory);
	}

WrappedDatonSetRemoter::WrappedDatonSetRemoter(
	const ZRef<ChannerRW_Bin>& iChanner,
	const ZRef<Callable_PullSuggested>& iCallable_PullSuggested_Other)
:	fCallable_PullSuggested_Other(iCallable_PullSuggested_Other)
,	fChanner(iChanner)
,	fConnectionBusy(false)
,	fPullFromPointer(nullptr)
	{}

void WrappedDatonSetRemoter::Initialize()
	{
	ZCounted::Initialize();

	fCallable_PullSuggested_Self =
		sCallable(sWeakRef(this), &WrappedDatonSetRemoter::pPullSuggested);

	fCallable_PullFrom_Self =
		sCallable(sWeakRef(this), &WrappedDatonSetRemoter::pPullFrom);

	ZThread::sStart_T<ZRef<WrappedDatonSetRemoter> >(&WrappedDatonSetRemoter::spRead, this);
	}

ZRef<Callable_PullSuggested> WrappedDatonSetRemoter::GetCallable_PullSuggested()
	{ return fCallable_PullSuggested_Self; }

void WrappedDatonSetRemoter::pPullSuggested(const ZRef<Callable_PullFrom>& iCallable_PullFrom)
	{
	// iCallable_PullFrom will be the counterpart to fCallable_PullSuggested_Other.

	{
	ZGuardMtxR guard(fMtxR);
	sInsert(fCallables_PullFrom, iCallable_PullFrom);
	}

	for (;;)
		{
		try
			{
			ZRef<ChannerRW_Bin> theChanner = this->pEnsureChanner();

			ZGuardMtxR guard(fMtxR);

			if (not theChanner)
				break;

			const ChanW_Bin& theChanW = sGetChan<ChanW_Bin>(theChanner);

			while (fConnectionBusy)
				fCnd.Wait(fMtxR);

			Map_Any theMessage;
			theMessage.Set("What", "PullSuggested");

			{
			SaveSetRestore<bool> theSSR(fConnectionBusy, true);
			ZRelGuardR rel2(guard);

			spSendMessage(theMessage, theChanW);
			}

			fCnd.Broadcast();
			break;
			}
		catch (...)
			{
			ZGuardMtxR guard(fMtxR);
			fChanner.Clear();
			}
		}

	fCnd.Broadcast();
	}

ZRef<Deltas> WrappedDatonSetRemoter::pPullFrom(ZRef<Event> iEvent)
	{
	for (;;)
		{
		try
			{
			ZRef<ChannerRW_Bin> theChanner = this->pEnsureChanner();

			ZGuardMtxR guard(fMtxR);

			if (not theChanner)
				break;

			const ChanW_Bin& theChanW = sGetChan<ChanW_Bin>(theChanner);

			while (fConnectionBusy)
				fCnd.Wait(fMtxR);

			Map_Any theMessage;
			theMessage.Set("What", "PullFrom");
			theMessage.Set("Event", spSeqFromEvent(iEvent));

			ZRef<Deltas> theDeltas;
			fPullFromPointer = &theDeltas;

			{
			SaveSetRestore<bool> theSSR(fConnectionBusy, true);
			ZRelGuardR rel(guard);

			spSendMessage(theMessage, theChanW);
			}

			fCnd.Broadcast();

			// Wait till a PullFromResponse has come in (any PullfromResponse?), and
			while (fPullFromPointer)
				fCnd.Wait(fMtxR);

			return theDeltas;
			}
		catch (...)
			{
			ZGuardMtxR guard(fMtxR);
			fChanner.Clear();
			}
		}
	// Never get here.
	return null;
	}

void WrappedDatonSetRemoter::pRead()
	{
	ZThread::sSetName("WrappedDatonSetRemoter");

	for (;;)
		{
		try
			{
			ZRef<ChannerRW_Bin> theChanner = this->pEnsureChanner();

			ZGuardMtxR guard(fMtxR);

			if (not theChanner)
				{
				// No Channer was available
				if (fFactory)
					continue;
				// There's no factory to make one, bail
				break;
				}

			ZRelGuardR rel(guard);

			const Map_Any theMessage = spReadMessage(sGetChanner<ChanR_Bin>(theChanner));
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
				using namespace DatonSet;

				ZRef<Event> theEvent = spEventFromSeq(theMessage.Get<Seq_Any>("Event"));

				ZAcqGuardR acq(guard);

				set<ZRef<Callable_PullFrom> > theCallables_PullFrom;
				swap(theCallables_PullFrom, fCallables_PullFrom);

				ZRelGuardR rel(guard);

				Map_Any theMessage;
				theMessage.Set("What", "PullFromResponse");
				Seq_Any& theDeltasSeq = theMessage.Mut<Seq_Any>("Deltas");

				foreachv (ZRef<Callable_PullFrom> theCallable, theCallables_PullFrom)
					{
					ZRef<Deltas> theDeltas = theCallable->Call(theEvent);

					foreachi (iterDelta, theDeltas->GetVector())
						{
						Map_Any& theEntry = theDeltasSeq.Mut<Map_Any>(theDeltasSeq.Count());
						theEntry.Set("Event", spSeqFromEvent(iterDelta->first));
						theEntry.Set("Delta", spSeqFromDelta(iterDelta->second));
						}
					}

				ZAcqGuardR acq2(guard);

				while (fConnectionBusy)
					fCnd.Wait(fMtxR);

				{
				SaveSetRestore<bool> theSSR(fConnectionBusy, true);
				ZRelGuardR rel2(guard);

				spSendMessage(theMessage, sGetChan<ChanW_Bin>(theChanner));
				}

				fCnd.Broadcast();
				}
			else if (theWhat == "PullFromResponse")
				{
				using namespace DatonSet;
				// We should be blocked up in pPullFrom, get the data across from here to there
				Vector_Event_Delta_t theVED;
				foreachi (iter, theMessage.Get<Seq_Any>("Deltas"))
					{
					const Map_Any theEntry = iter->Get<Map_Any>();
					ZRef<Event> anEvent = spEventFromSeq(theEntry.Get<Seq_Any>("Event"));
					ZRef<Delta> aDelta = spDeltaFromSeq(theEntry.Get<Seq_Any>("Delta"));
					theVED.push_back(Event_Delta_t(anEvent, aDelta));
					}

				ZAcqGuardR acq(guard);
				if (fPullFromPointer)
					{
					*fPullFromPointer = new Deltas(*&theVED);
					fPullFromPointer = nullptr;
					fCnd.Broadcast();
					}
				}
			}
		catch (...)
			{
			ZGuardMtxR guard(fMtxR);
			fChanner.Clear();
			}
		}
	}

void WrappedDatonSetRemoter::spRead(ZRef<WrappedDatonSetRemoter> iWDSR)
	{ iWDSR->pRead(); }

ZRef<ChannerRW_Bin> WrappedDatonSetRemoter::pEnsureChanner()
	{
	ZGuardMtxR guard(fMtxR);

	if (not fChanner)
		{
		guard.Release();
		ZRef<ChannerRW_Bin> theChanner = sCall(fFactory);
		guard.Acquire();
		fChanner = theChanner;
		}
	return fChanner;
	}

} // namespace DatonSet
} // namespace ZooLib
