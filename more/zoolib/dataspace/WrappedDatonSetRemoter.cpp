

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

static void spSendMessage(const ZMap_Any& iMessage, const ChanW_Bin& iChanW)
	{
	if (ZLOGF(w, eDebug))
		{
		w << "Send: ";
//		w << iMessage.Get<string8>("What");
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
//		w << result.Get<string8>("What");
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

static ZRef<ZDatonSet::Delta> spDeltaFromSeq(const ZSeq_Any& iSeq)
	{
	using namespace ZDatonSet;
	Delta::Statements_t theStatements_t;
	foreachi (iter, iSeq)
		{
		const ZSeq_Any& theStatementSeq = iter->Get<ZSeq_Any>();
		const bool theBool = theStatementSeq[0].Get<bool>();
		const Daton& theDaton = theStatementSeq[1].Get<ZData_Any>();
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
		theStatementSeq.Append(iter->second);
		theStatementSeq.Append(iter->first.GetData());
		}
	return theSeq;
	}

// =================================================================================================
// MARK: - WrappedDatonSetRemoter

WrappedDatonSetRemoter::WrappedDatonSetRemoter(
	const ZRef<ChannerComboFactoryRW_Bin>& iChannerComboFactory,
	const ZRef<Callable_PullSuggested>& iCallable_PullSuggested_Other)
:	fChannerComboFactory(iChannerComboFactory)
,	fCallable_PullSuggested_Other(iCallable_PullSuggested_Other)
,	fConnectionBusy(false)
,	fPullFromPointer(nullptr)
	{
	ZAssert(fChannerComboFactory);
	}

WrappedDatonSetRemoter::WrappedDatonSetRemoter(
	const ChannerComboRW_Bin& iChannerComboRW,
	const ZRef<Callable_PullSuggested>& iCallable_PullSuggested_Other)
:	fCallable_PullSuggested_Other(iCallable_PullSuggested_Other)
,	fChannerComboQ(iChannerComboRW)
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

	ZThread::sCreate_T<ZRef<WrappedDatonSetRemoter> >(&WrappedDatonSetRemoter::spRead, this);
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
			ZQ<ChannerComboRW_Bin> theChannerComboQ = this->pQEnsureChannerCombo();

			ZGuardMtxR guard(fMtxR);

			if (not theChannerComboQ)
				break;

			const ChanW_Bin& theChanW = sGetChan(theChannerComboQ->GetW());

			while (fConnectionBusy)
				fCnd.Wait(fMtxR);

			ZMap_Any theMessage;
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
			sClear(fChannerComboQ);
			}
		}

	fCnd.Broadcast();
	}

ZRef<ZDatonSet::Deltas> WrappedDatonSetRemoter::pPullFrom(ZRef<Event> iEvent)
	{
	for (;;)
		{
		try
			{
			ZQ<ChannerComboRW_Bin> theChannerComboQ = this->pQEnsureChannerCombo();

			ZGuardMtxR guard(fMtxR);

			if (not theChannerComboQ)
				break;

			const ChanW_Bin& theChanW = sGetChan(theChannerComboQ->GetW());

			while (fConnectionBusy)
				fCnd.Wait(fMtxR);

			ZMap_Any theMessage;
			theMessage.Set("What", "PullFrom");
			theMessage.Set("Event", spSeqFromEvent(iEvent));

			ZRef<ZDatonSet::Deltas> theDeltas;
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
			sClear(fChannerComboQ);
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
			ZQ<ChannerComboRW_Bin> theChannerComboQ = this->pQEnsureChannerCombo();

			ZGuardMtxR guard(fMtxR);

			if (not theChannerComboQ)
				{
				// No Channer was available
				if (fChannerComboFactory)
					continue;
				// There's no factory to make one, bail
				break;
				}

			ZRelGuardR rel(guard);

			const ZMap_Any theMessage = spReadMessage(theChannerComboQ->GetR());
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

				ZRef<Event> theEvent = spEventFromSeq(theMessage.Get<ZSeq_Any>("Event"));

				ZAcqGuardR acq(guard);

				set<ZRef<Callable_PullFrom> > theCallables_PullFrom;
				swap(theCallables_PullFrom, fCallables_PullFrom);

				ZRelGuardR rel(guard);

				ZMap_Any theMessage;
				theMessage.Set("What", "PullFromResponse");
				ZSeq_Any& theDeltasSeq = theMessage.Mut<ZSeq_Any>("Deltas");

				foreachv (ZRef<Callable_PullFrom> theCallable, theCallables_PullFrom)
					{
					ZRef<ZDatonSet::Deltas> theDeltas = theCallable->Call(theEvent);

					foreachi (iterDelta, theDeltas->GetVector())
						{
						ZMap_Any& theEntry = theDeltasSeq.Mut<ZMap_Any>(theDeltasSeq.Count());
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

				spSendMessage(theMessage, sGetChan(theChannerComboQ->GetW()));
				}

				fCnd.Broadcast();
				}
			else if (theWhat == "PullFromResponse")
				{
				using namespace ZDatonSet;
				// We should be blocked up in pPullFrom, get the data across from here to there
				Vector_Event_Delta_t theVED;
				foreachi (iter, theMessage.Get<ZSeq_Any>("Deltas"))
					{
					const ZMap_Any theEntry = iter->Get<ZMap_Any>();
					ZRef<Event> anEvent = spEventFromSeq(theEntry.Get<ZSeq_Any>("Event"));
					ZRef<Delta> aDelta = spDeltaFromSeq(theEntry.Get<ZSeq_Any>("Delta"));
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
		catch (std::exception& ex)
			{
			ZGuardMtxR guard(fMtxR);
			sClear(fChannerComboQ);
			}
		}
	}

void WrappedDatonSetRemoter::spRead(ZRef<WrappedDatonSetRemoter> iWDSR)
	{ iWDSR->pRead(); }

ZQ<ChannerComboRW_Bin> WrappedDatonSetRemoter::pQEnsureChannerCombo()
	{
	ZGuardMtxR guard(fMtxR);

	if (not fChannerComboQ)
		{
		guard.Release();
		ZQ<ChannerComboRW_Bin> theChannerComboQ = sCall(fChannerComboFactory);
		guard.Acquire();
		fChannerComboQ = theChannerComboQ;
		}
	return fChannerComboQ;
	}

} // namespace Dataspace
} // namespace ZooLib
