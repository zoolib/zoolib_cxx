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

#include "zoolib/Dataspace/MelangeRemoter.h"

#include "zoolib/Coerce_Any.h"
#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Callable_PMF.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/ChanR_Bin_More.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/ChanR_XX_AbortOnSlowRead.h"
#include "zoolib/Chan_XX_Buffered.h"
#include "zoolib/Chan_XX_Count.h"
#include "zoolib/Log.h"
#include "zoolib/Promise.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/Stringf.h"
#include "zoolib/PullPush_Any.h"
#include "zoolib/PullPush_JSONB.h"
#include "zoolib/Util_Any_JSON.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/QueryEngine/Result.h"

#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace Dataspace {

using namespace Operators_Any_JSON;
using namespace Util_STL;

using QueryEngine::Result;

static size_t kBufSize = 16;

// =================================================================================================
#pragma mark -

struct Start_Result : PullPush::Start
	{};

const PPT kStart_Result = ZRef<PullPush::Start>(new Start_Result);

static string8 spStringFromChan(const ChanR_Bin& r)
	{ return sReadCountPrefixedString(r); }

class ReadFilter
:	public Callable_Any_ReadFilter
,	public Callable_JSONB_ReadFilter
	{
public:
// From Callable_Any_ReadFilter
	virtual ZQ<bool> QCall(const PPT& iPPT, const ChanR_PPT& iChanR, Any& oAny)
		{
		if (const ZRef<PullPush::Start> theRef = sGet<ZRef<PullPush::Start>>(iPPT))
			{
			if (not theRef.DynamicCast<Start_Result>())
				return false;

			PPT theRHPPT = sERead(iChanR);
			RelHead& theRelHead = sMut<RelHead>(theRHPPT);
			const size_t theRowCount = size_t(sCoerceInt(sERead(iChanR).As<Any>()));
			size_t theCount = theRowCount * theRelHead.size();
			vector<Val_Any> theVals;
			theVals.reserve(theCount);
			while (theCount--)
				{
				PPT thePPT = sERead(iChanR);
				Any munged;
				sPull_PPT_AsAny(thePPT, iChanR, this, munged);
				sPushBack(theVals, munged);
				}
			oAny = sRef(new Result(&theRelHead, &theVals));

			sEPull_End(iChanR);
			return true;
			}

		return false;
		}

// From Callable_JSONB_ReadFilter
	virtual ZQ<bool> QCall(uint8 iType, const ChanR_Bin& iChanR, const ChanW_PPT& iChanW)
		{
		if (iType == 254)
			{
			if (ZQ<uint8> theTypeQ = sQRead(iChanR))
				{
				switch (*theTypeQ)
					{
					case 100:
						{
						sPush(kStart_Result, iChanW);

							// We're at the beginning of a QE::Result. So copy the RelHead to start with.
							PPT thePPT = sAnyCounted<RelHead,PullPush::Tag_PPT>();
							RelHead& theRH = sMut<RelHead>(thePPT);
							for (uint64 theCount = sReadCount(iChanR); theCount; --theCount)
								theRH |= spStringFromChan(iChanR);
							sPush(thePPT, iChanW);

							const uint64 theCount = sReadCount(iChanR);
							sPush(theCount, iChanW);

							// Now copy the vals.
							for (uint64 xx = theCount * theRH.size(); xx; --xx)
								sPull_JSONB_Push_PPT(iChanR, this, iChanW);

						sPush(PullPush::End(), iChanW);

						return true;
						}
					case 101:
						{
						sPush(Daton(sRead_T<Data_Any>(iChanR, size_t(sReadCount(iChanR)))), iChanW);
						return true;
						}
					case 102:
						{
						sPush(AbsentOptional_t(), iChanW);
						return true;
						}
					}
				return false;
				}
			}
		return null;
		}
	};

static Map_Any spReadMessage(const ChanR_Bin& iChanR, const ZQ<string>& iDescriptionQ)
	{
	ChanR_XX_Count<ChanR_Bin> theChanR(iChanR);

	// This ReadFilter handles both the JSONB-->PPT and the PPT->Any translations for
	// Result, Daton and for AbsentOptional_t
	const ZRef<ReadFilter> theReadFilter = sDefault<ZRef_Counted<ReadFilter> >();

	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	ZRef<Delivery<Any>> theDelivery = sStartAsync_AsAny(sGetClear(thePair.second), theReadFilter);
	sPull_JSONB_Push_PPT(theChanR, theReadFilter, ChanW_XX_Buffered<ChanW_PPT>(*thePair.first, kBufSize));
	sDisconnectWrite(*thePair.first);

	ZQ<Any> theQ = theDelivery->QGet();
	if (not theQ)
		sThrow_ExhaustedR();

	const Map_Any theMessage = theQ->Get<Map_Any>();
	if (ZLOGF(w, eDebug + 1))
		{
		w << theChanR.GetCount() << " bytes, ";

		if (iDescriptionQ)
			{
			w << *iDescriptionQ;
			if (theMessage.IsEmpty())
				w << ", ";
			}
		if (not theMessage.IsEmpty())
			w << "\n";

		Util_Any_JSON::sWrite(false, Any(theMessage), w);
		}

	return theMessage;
	}

// =================================================================================================
#pragma mark -

class WriteFilter
:	public Callable_Any_WriteFilter
,	public Callable_JSONB_WriteFilter
	{
public:
// From Callable_Any_WriteFilter
	virtual ZQ<bool> QCall(const Any& iAny, const ChanW_PPT& iChanW)
		{
		if (const ZRef<Result>* theResultP = iAny.PGet<ZRef<Result> >())
			{
			ZRef<Result> theResult = *theResultP;

			sPush(kStart_Result, iChanW);

				const RelHead& theRH = theResult->GetRelHead();
				sPush(theRH, iChanW);
				const size_t theRHCount = theRH.size();

				const size_t theRowCount = theResult->Count();
				sPush(theRowCount, iChanW);
				for (size_t yy = 0; yy < theRowCount; ++yy)
					{
					const Val_Any* theRow = theResult->GetValsAt(yy);
					for (size_t xx = 0; xx < theRHCount; ++xx)
						sFromAny_Push_PPT(theRow[xx], this, iChanW);
					}
			sPush(PullPush::End(), iChanW);
			return true;
			}
		return false;
		}

// From Callable_JSONB_WriteFilter
	virtual ZQ<bool> QCall(const PPT& iPPT, const ChanR_PPT& iChanR, const ChanW_Bin& iChanW)
		{
		if (const ZRef<PullPush::Start> theRef = sGet<ZRef<PullPush::Start>>(iPPT))
			{
			if (not theRef.DynamicCast<Start_Result>())
				return false;

			sEWriteBE<uint8>(iChanW, 254);
			sEWriteBE<uint8>(iChanW, 100);

			const RelHead theRH = sGet<RelHead>(sERead(iChanR));
			const size_t theRHCount = theRH.size();
			sEWriteCount(iChanW, theRHCount);
			foreacha (entry, theRH)
				sEWriteCountPrefixedString(iChanW, entry);

			const size_t theRowCount = sCoerceInt(sERead(iChanR).As<Any>());
			sEWriteCount(iChanW, theRowCount);

			for (size_t xx = theRHCount * theRowCount; xx; --xx)
				sPull_PPT_Push_JSONB(iChanR, this, iChanW);

			sEPull_End(iChanR);
			return true;
			}

		if (const Daton* theDatonP = iPPT.PGet<Daton>())
			{
			sEWriteBE<uint8>(iChanW, 254);
			sEWriteBE<uint8>(iChanW, 101);

			const Data_Any& theData = theDatonP->GetData();

			sEWriteCount(iChanW, theData.GetSize());
			sEWriteMem(iChanW, theData.GetPtr(), theData.GetSize());
			return true;
			}

		if (iPPT.PGet<AbsentOptional_t>())
			{
			sEWriteBE<uint8>(iChanW, 254);
			sEWriteBE<uint8>(iChanW, 102);
			return true;
			}

		return false;
		}
	};

static void spFromAny_Push_PPT(const Any& iAny,
	const ZRef<Callable_Any_WriteFilter>& iWriteFilter,
	const ZRef<ChannerWCon_PPT>& iChannerWCon)
	{
	sFromAny_Push_PPT(iAny, iWriteFilter, ChanW_XX_Buffered<ChanW_PPT>(*iChannerWCon, kBufSize));
	sDisconnectWrite(*iChannerWCon);
	}

static ZAtomic_t spSentMessageCounter;

static void spWriteMessage(const ChanW_Bin& iChanW, Map_Any iMessage, const ZQ<string>& iDescriptionQ)
	{
	ChanW_XX_Count<ChanW_Bin> theChanW(iChanW);

	const ZRef<WriteFilter> theWriteFilter = sDefault<ZRef_Counted<WriteFilter> >();

	const double start = Time::sSystem();

	iMessage.Set("AAA", sAtomic_Add(&spSentMessageCounter, 1));

	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	sStartOnNewThread(sBindR(sCallable(spFromAny_Push_PPT),
		Any(iMessage),
		theWriteFilter,
		sGetClear(thePair.first)));
	sPull_PPT_Push_JSONB(ChanR_XX_Buffered<ChanR_PPT>(*thePair.second, kBufSize), theWriteFilter, theChanW);

	sFlush(theChanW);

	const double finish = Time::sSystem();

	if (ZLOGF(w, eDebug + 1))
		{
		w << theChanW.GetCount() << " bytes, ";

		if (iDescriptionQ)
			w << *iDescriptionQ << ", ";

		w << 1e3 * (finish - start) << "ms";

		if (iMessage.IsEmpty())
			w << ", ";
		else
			w << "\n";
		Util_Any_JSON::sWrite(false, Any(iMessage), w);
		}
	}

// =================================================================================================

namespace { // anonymous

ZRef<Expr_Rel> spAsRel(const Val_Any& iVal)
	{
	if (NotQ<string8> theStringQ = iVal.QGet<string8>())
		{
		return null;
		}
	else
		{
		return RelationalAlgebra::Util_Strim_Rel::sQFromStrim(ChanRU_UTF_string8(*theStringQ));
		}
	}

Val_Any spAsVal(ZRef<Expr_Rel> iRel)
	{
	string8 theString;
	RelationalAlgebra::Util_Strim_Rel::sToStrim_Parseable(iRel, ChanW_UTF_string8(&theString));
	return theString;
	}

// -----

ZRef<Result> spAsResult(const Val_Any& iVal)
	{
	RelHead theRH;
	foreachv (Val_Any theVal, iVal["RelHead"].Get<Seq_Any>())
		sInsert(theRH, theVal.Get<string8>());

	std::vector<Val_Any> thePackedRows;

	foreachv (Val_Any theVal, iVal["Vals"].Get<Seq_Any>())
		sPushBack(thePackedRows, theVal);

	return new Result(&theRH, &thePackedRows);
	}

Val_Any spAsVal(ZRef<Result> iResult)
	{
	Map_Any result;

	const RelHead& theRH = iResult->GetRelHead();

	Seq_Any& theSeq_RH = result.Mut<Seq_Any>("RelHead");
	foreacha (entry, theRH)
		theSeq_RH.Append(entry);

	const size_t theCount = iResult->Count();
	result.Set("Count", int64(theCount));

	Seq_Any& theSeq_Vals = result.Mut<Seq_Any>("Vals");

	for (size_t yy = 0; yy < theCount; ++yy)
		{
		const Val_Any* theRow = iResult->GetValsAt(yy);
		for (size_t xx = 0; xx < theRH.size(); ++xx)
			theSeq_Vals.Append(theRow[xx]);
		}

	return result;
	}

// -----

ZRef<ResultDeltas> spAsResultDeltas(const Val_Any& iVal)
	{
	ZRef<ResultDeltas> theResultDeltas = new ResultDeltas;

	foreachv (Val_Any theVal, iVal["Vals"].Get<Seq_Any>())
		sPushBack(theResultDeltas->fPackedRows, theVal);

	foreachv (Val_Any theVal, iVal["Mapping"].Get<Seq_Any>())
		sPushBack(theResultDeltas->fMapping, sCoerceInt(theVal));

	return theResultDeltas;
	}

Val_Any spAsVal(ZRef<ResultDeltas> iResultDeltas)
	{
	Map_Any theMap;

	theMap["Mapping"] = Seq_Any(iResultDeltas->fMapping.begin(), iResultDeltas->fMapping.end());

	theMap["Vals"] = Seq_Any(iResultDeltas->fPackedRows);

	return theMap;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - MelangeServer

MelangeServer::MelangeServer(const Melange_t& iMelange,
	const ZRef<ChannerRW_Bin>& iChannerRW,
	int64 iClientVersion,
	const ZQ<string>& iDescriptionQ)
:	fMelange(iMelange)
,	fChannerR(iChannerRW)
,	fChannerW(iChannerRW)
,	fClientVersion(iClientVersion)
,	fDescriptionQ(iDescriptionQ)
,	fTimeOfLastRead(0)
,	fTimeOfLastWrite(0)
,	fLastClientChangeCount(0)
,	fTimeout(10)
,	fConnectionTimeout(30)
	{}

MelangeServer::~MelangeServer()
	{
	if (ZLOGF(w, eNotice))
		{
		if (fDescriptionQ)
			w << *fDescriptionQ;
		}
	}

void MelangeServer::Initialize()
	{
	ZCounted::Initialize();

	fJob = StartScheduler::Job(fMelange.f2, sCallable(sWeakRef(this), &MelangeServer::pWork));

	fCallable_Changed = sCallable(sWeakRef(this), &MelangeServer::pChanged);

	sStartOnNewThread(sCallable(sRef(this), &MelangeServer::pRead));
	}

void MelangeServer::pRead()
	{
	if (fDescriptionQ)
		{
		string threadName = *fDescriptionQ;
		size_t length = threadName.size();
		if (length >= 15)
			threadName = threadName.substr(length - 15, 15);
		ZThread::sSetName(threadName.c_str());
		}
	else
		{
		ZThread::sSetName("MSR");
		}

	ZAcqMtx acq(fMtx);
	for (;;)
		{
		ZRef<ChannerR_Bin> theChannerR = fChannerR;

		Map_Any theMap;
		{
		ZRelMtx rel(fMtx);
		theMap = spReadMessage(*theChannerR, fDescriptionQ);
		}

		fTimeOfLastRead = Time::sSystem();

		fTrueOnce_SendAnEmptyMessage.Reset();

		fQueue_Read.push_back(theMap);
		this->pWake();
		}
	}

void MelangeServer::pWrite()
	{
	if (fDescriptionQ)
		ZThread::sSetName(("MSW:" + *fDescriptionQ).c_str());
	else
		ZThread::sSetName("MSW");

	ZRef<ChannerW_Bin> theChannerW = fChannerW;

	ZAcqMtx acq(fMtx);
	if (sIsEmpty(fMap_Refcon2ResultCC))
		{
		if (fTrueOnce_SendAnEmptyMessage())
			{
			{
			ZRelMtx rel(fMtx);
			spWriteMessage(*theChannerW, Map_Any(), fDescriptionQ);
			}
			fTimeOfLastWrite = Time::sSystem();
			}
		}
	else while (not sIsEmpty(fMap_Refcon2ResultCC))
		{
		fTrueOnce_SendAnEmptyMessage.Reset();
		vector<Map_Any> theMessages;
		foreacha (entry, fMap_Refcon2ResultCC)
			{
			Map_Any theMap;
			theMap.Set("What", "Change");
			theMap.Set("Refcon", entry.first);
			if (sQErase(fSet_NewRefcons, entry.first) || fClientVersion < 2 || not entry.second.fResultDeltas)
				theMap.Set("Result", spAsVal(entry.second.fResult));
			else
				theMap.Set("Deltas", spAsVal(entry.second.fResultDeltas));
			theMap.Set("ChangeCount", entry.second.fCC);
			sPushBack(theMessages, theMap);
			}
		sClear(fMap_Refcon2ResultCC);

		foreacha (entry, theMessages)
			{
			{
			ZRelMtx rel(fMtx);
			spWriteMessage(*theChannerW, entry, fDescriptionQ);
			}
			fTimeOfLastWrite = Time::sSystem();
			}
		}
	fTrueOnce_WriteNeedsStart.Reset();
	}

void MelangeServer::pWake()
	{
	sNextStartIn(0, fJob);
	}

void MelangeServer::pWork()
	{
	ZAcqMtx acq(fMtx);

	if (Time::sSystem() - fTimeOfLastRead > fConnectionTimeout)
		{
		if (ZLOGF(w, eDebug))
			{
			w << "MelangeServer, no data read in prior " << fConnectionTimeout << "s";
			if (fDescriptionQ)
				w << ", " << *fDescriptionQ;
			}
		}

	// Pull stuff from fQueue_Read
	vector<Map_Any> theMessages;
	swap(fQueue_Read, theMessages);

	// Act on messages
	foreachv (Map_Any theMessage, theMessages)
		{
		const int64 theCC = sCoerceInt(theMessage.Get("ChangeCount"));
		fLastClientChangeCount = std::max(fLastClientChangeCount, theCC);

		foreacha (entry, theMessage.Get<Seq_Any>("Registrations"))
			{
			if (ZQ<Map_Any> theMapQ = entry.Get<Map_Any>())
				{
				if (ZQ<int64> theRefconQ = sQCoerceInt(theMapQ->Get("Refcon")))
					{
					if (ZRef<Expr_Rel> theRel = spAsRel(theMapQ->Get("Rel")))
						{
						RefReg theReg = sCall(fMelange.f0, fCallable_Changed, theRel);
						sInsertMust(fMap_Refcon2Reg, *theRefconQ, theReg);
						sInsertMust(fMap_Reg2Refcon, theReg, *theRefconQ);
						sInsertMust(fSet_NewRefcons, *theRefconQ);
						}
					}
				}
			}

		foreacha (entry, theMessage.Get<Seq_Any>("Unregistrations"))
			{
			if (ZQ<int64> theRefconQ = sQCoerceInt(entry))
				{
				if (NotQ<RefReg> theRegQ = sQGetErase(fMap_Refcon2Reg, *theRefconQ))
					{
					if (ZLOGF(w, eErr))
						w << "Failed to find Refcon2Reg entry for " << *theRefconQ;
					}
				else if (NotQ<int64> theMapRefconQ = sQGetErase(fMap_Reg2Refcon, *theRegQ))
					{
					if (ZLOGF(w, eErr))
						w << "Failed to find Refcon2Reg entry for " << *theRefconQ << ", " << theRegQ->Get();
					}
				else if (*theRefconQ != *theMapRefconQ)
					{
					if (ZLOGF(w, eErr))
						w << "Failed *theRefconQ: " << *theRefconQ << " != *theMapRefconQ: " << *theMapRefconQ;
					}
				else
					{
					// Also toss any result for the refcon.
					sErase(fMap_Refcon2ResultCC, *theRefconQ);
					}
				}
			}

		vector<Daton> toInsert, toErase;
		foreacha (entry, theMessage.Get<Seq_Any>("Asserts"))
			{
			if (ZQ<Data_Any> theDataQ = entry.QGet<Data_Any>())
				sPushBack(toInsert, *theDataQ);
			}

		foreacha (entry, theMessage.Get<Seq_Any>("Retracts"))
			{
			if (ZQ<Data_Any> theDataQ = entry.QGet<Data_Any>())
				sPushBack(toErase, *theDataQ);
			}

		if (toInsert.size() || toErase.size())
			{
			ZRelMtx rel(fMtx);
			int64 unusedChangeCount = sCall(fMelange.f1,
				sFirstOrNil(toInsert), toInsert.size(),
				sFirstOrNil(toErase), toErase.size());
			}
		}

	// We're not handling the potential asynchrony here -- we're using the client's change count
	// when communicating back. What about the one we get from our melange?

	if (sNotEmpty(fMap_Refcon2ResultCC) || fTimeOfLastWrite + fTimeout < Time::sSystem())
		{
		fTimeOfLastWrite = Time::sSystem();
		if (fTrueOnce_WriteNeedsStart())
			{
			sStartOnNewThread(sCallable(sRef(this), &MelangeServer::pWrite));
			}
		else
			{
			fCnd.Broadcast();
			}
		}

	// Should this be Time::System() + fTimeout?
	sNextStartAt(fTimeOfLastWrite + fTimeout, fJob);
	}

void MelangeServer::pChanged(
	const RefReg& iRegistration,
	int64 iChangeCount,
	const ZRef<Result>& iResult,
	const ZRef<ResultDeltas>& iResultDeltas)
	{
	ZAcqMtx acq(fMtx);

	sSet(fMap_Refcon2ResultCC, sGetMust(fMap_Reg2Refcon, iRegistration),
		ResultCC({fLastClientChangeCount, iResult, iResultDeltas}));

	this->pWake();
	}

// =================================================================================================
#pragma mark - Melange_Client::Registration

class Melange_Client::Registration
:	public ZCounted
	{
public:
	Registration(ZRef<Melange_Client> iClient,
		ZRef<RelsWatcher::Callable_Changed> iCallable_Changed,
		const ZRef<Expr_Rel>& iRel)
	:	fClient(iClient)
	,	fCallable_Changed(iCallable_Changed)
	,	fRel(iRel)
		{}

// From ZCounted
	virtual void Finalize()
		{ fClient->pFinalize(this); }

	ZRef<Melange_Client> fClient;
	ZRef<RelsWatcher::Callable_Changed> fCallable_Changed;
	ZRef<Expr_Rel> fRel;
	};

// =================================================================================================
#pragma mark - Melange_Client

Melange_Client::Melange_Client(const ZRef<Factory_Channer>& iFactory,
	const ZRef<Callable_Status>& iCallable_Status)
:	fFactory(iFactory)
,	fCallable_Status(iCallable_Status)
,	fGettingChanner(false)
,	fReadSinceWrite(false)
,	fChangeCount(1)
,	fNextRefcon(1)
	{}

ZQ<ZRef<ZCounted> > Melange_Client::QCall(
	const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
	const ZRef<Expr_Rel>& iRel)
	{
	// We're probably on our own starter.
	// Shove it into data structure, and wake the starter, which will itself wake the writer.

	ZAcqMtx acq(fMtx);

	ZRef<Registration> theRegistration = new Registration(this, iCallable_Changed, iRel);

	sInsertMust(fPending_Registrations, theRegistration);

	this->pWake();

	return theRegistration;
	}

ZQ<int64> Melange_Client::QCall(
	const Daton* iAsserted, size_t iAssertedCount,
	const Daton* iRetracted, size_t iRetractedCount)
	{
	ZAcqMtx acq(fMtx);

	while (iAssertedCount--)
		{
		const Daton& theDaton = *iAsserted++;
		if (not sQErase(fPending_Retracts, theDaton))
			sInsertMust(fPending_Asserts, theDaton);
		}

	while (iRetractedCount--)
		{
		const Daton& theDaton = *iRetracted++;
		if (not sQErase(fPending_Asserts, theDaton))
			sInsertMust(fPending_Retracts, theDaton);
		}

	this->pWake();
	return ++fChangeCount;
	}

bool Melange_Client::pTrigger()
	{
	this->pWake();
	return true;
	}

void Melange_Client::Start(ZRef<Starter> iStarter)
	{
	fJob = StartScheduler::Job(iStarter, sCallable(sWeakRef(this), &Melange_Client::pWork));

	sStartOnNewThread(sCallable(sRef(this), &Melange_Client::pRead));
	}

bool Melange_Client::Kill()
	{
	ZAcqMtx acq(fMtx);
	fJob.first.Clear();
	fJob.second.Clear();
	if (fChanner)
		sAbort(*fChanner);
	fCnd.Broadcast();
	return true;
	}

void Melange_Client::pRead()
	{
	ZThread::sSetName("MCR");

	ZAcqMtx acq(fMtx);
	while (fJob.first)
		{
		try
			{
			ZRef<ChannerForRead> theChanner = this->pEnsureChannerR();

			if (not theChanner)
				continue;

			Map_Any theMap;
			{
			ZRelMtx rel(fMtx);
			if (::getenv("ZOOLIB_DONT_ABORT_ON_SLOW_READ"))
				theMap = spReadMessage(*theChanner, null);
			else
				theMap = spReadMessage(ChanR_XX_AbortOnSlowRead<byte>(*theChanner, 15), null);
			}

			fQueue_Read.push_back(theMap);
			this->pWake();
			}
		catch (...)
			{
			fChanner.Clear();
			}
		}
	}

void Melange_Client::pWrite()
	{
	ZThread::sSetName("MCW");

	ZAcqMtx acq(fMtx);
	while (fJob.first)
		{
		if (sIsEmpty(fQueue_ToWrite))
			{
			// Give it a second to fill up.
			fCnd.WaitFor(fMtx, 1);
			if (sIsEmpty(fQueue_ToWrite))
				{
				// It's still empty, drop out of the loop and let the thread dispose.
				break;
				}
			}

		ZRef<ChannerW_Bin> theChannerW = this->pEnsureChannerW();
		if (not theChannerW)
			break;

		vector<Map_Any> theMessages;
		swap(fQueue_ToWrite, theMessages);

		try
			{
			ZRelMtx rel(fMtx);
			foreacha (entry, theMessages)
				spWriteMessage(*theChannerW, entry, null);
			}
		catch (...)
			{
			break;
			}
		}
	fTrueOnce_WriteNeedsStart.Reset();
	}

void Melange_Client::pWake()
	{
	if (fJob.first)
		sNextStartIn(0, fJob);
	}

void Melange_Client::pWork()
	{
	// Handle everything that's in fQueue_Read -- mainly doing change notifications

	// Any regs that are to be tossed will end up in here. Individually out of the scope of fMtx.
	std::set<ZRef<Registration>> localRegistrations;

	ZAcqMtx acq(fMtx);

	// Pull stuff from fQueue_Read
	vector<Map_Any> theMessages;
	swap(fQueue_Read, theMessages);

	{
	ZRelMtx rel(fMtx);

	foreachv (Map_Any theMessage, theMessages)
		{
		if (ZQ<int64> theRefconQ = sQCoerceInt(theMessage.Get("Refcon")))
			{
			const int64 theChangeCount = sCoerceInt(theMessage.Get("ChangeCount"));

			ZRef<ResultDeltas> theResultDeltas;
			if (const Val_Any* theP = theMessage.PGet("Deltas"))
				theResultDeltas = spAsResultDeltas(*theP);

			ZRef<Result> theResult;
			if (const Val_Any* theP = theMessage.PGet("Result"))
				theResult = spAsResult(*theP);

			if (ZRef<Registration> theReg = sGet(fMap_Refcon2Reg, *theRefconQ))
				sCall(theReg->fCallable_Changed, theReg, theChangeCount, theResult, theResultDeltas);
			}
		}

	// Invoke everything that needed to be called from us as a starter.
	Starter_EventLoopBase::pInvokeClearQueue();
	}

	if (sNotEmpty(theMessages))
		{
		// We've read something, so may need to issue a no-op write.
		fReadSinceWrite = true;
		}

	Map_Any theMessage;

	if (sNotEmpty(fPending_Registrations))
		{
		Seq_Any& theSeq = theMessage.Mut<Seq_Any>("Registrations");
		foreachv (ZRef<Registration> theReg, fPending_Registrations)
			{
			Map_Any theMap;
			const int64 theRefcon = fNextRefcon++;
			theMap.Set("Refcon", theRefcon);
			theMap.Set("Rel", spAsVal(theReg->fRel));
			sInsertMust(fMap_Refcon2Reg, theRefcon, theReg.Get());
			sInsertMust(fMap_Reg2Refcon, theReg.Get(), theRefcon);
			theSeq.Append(theMap);
			}
		swap(localRegistrations, fPending_Registrations);
		}

	if (sNotEmpty(fPending_Unregistrations))
		{
		Seq_Any& theSeq = theMessage.Mut<Seq_Any>("Unregistrations");
		foreacha (entry, fPending_Unregistrations)
			theSeq.Append(entry);
		sClear(fPending_Unregistrations);
		}

	if (sNotEmpty(fPending_Asserts))
		{
		Seq_Any& theSeq = theMessage.Mut<Seq_Any>("Asserts");
		foreacha (entry, fPending_Asserts)
			theSeq.Append(entry.GetData());
		sClear(fPending_Asserts);
		}

	if (sNotEmpty(fPending_Retracts))
		{
		Seq_Any& theSeq = theMessage.Mut<Seq_Any>("Retracts");
		foreacha (entry, fPending_Retracts)
			theSeq.Append(entry.GetData());
		sClear(fPending_Retracts);
		}

	if (not theMessage.IsEmpty() || (fReadSinceWrite && sIsEmpty(fQueue_ToWrite)))
		{
		theMessage.Set("ChangeCount", fChangeCount);
		fReadSinceWrite = false;
		sPushBack(fQueue_ToWrite, theMessage);
		}

	if (sNotEmpty(fQueue_ToWrite))
		{
		if (fTrueOnce_WriteNeedsStart())
			sStartOnNewThread(sCallable(sRef(this), &Melange_Client::pWrite));
		else
			fCnd.Broadcast();
		}
	}

ZRef<Melange_Client::ChannerForRead> Melange_Client::pEnsureChannerR()
	{ return this->pEnsureChanner(); }

ZRef<ChannerW_Bin> Melange_Client::pEnsureChannerW()
	{ return this->pEnsureChanner(); }

ZRef<Melange_Client::Channer_t> Melange_Client::pEnsureChanner()
	{
	while (fGettingChanner)
		fCnd.WaitFor(fMtx, 5);

	if (not fChanner)
		{
		// Registrations become pending, but writes are discarded -- we'll get replacement
		// values when we reconnect, and our caller can reapply their work, if appropriate.

		foreacha (entry, fMap_Reg2Refcon)
			sInsertMust(fPending_Registrations, entry.first);

		sClear(fPending_Asserts);
		sClear(fPending_Retracts);
		sClear(fPending_Unregistrations);
		sClear(fMap_Refcon2Reg);
		sClear(fMap_Reg2Refcon);
		sClear(fQueue_Read);
		sClear(fQueue_ToWrite);

		fReadSinceWrite = false;

		this->pWake();

		SaveSetRestore<bool> theSSR(fGettingChanner, true);

		ZRef<Channer_t> theChanner;
		{
		ZRelMtx rel(fMtx);

		if (ZLOGF(w, eDebug - 1))
			w << "No Channer";

		sCall(fCallable_Status, false);

		theChanner = sCall(fFactory);
		if (not theChanner)
			{
			// No Channer was available, pause for 1s.
			if (ZLOGF(w, eDebug - 1))
				w << "Still no Channer";
			ZThread::sSleep(1);
			}
		else
			{
			if (ZLOGF(w, eDebug - 1))
				w << "Has Channer";

			sCall(fCallable_Status, true);
			}
		}

		fChanner = theChanner;
		fCnd.Broadcast();
		}

	return fChanner;
	}

void Melange_Client::pFinalize(Registration* iRegistration)
	{
	ZAcqMtx acq(fMtx);
	if (not iRegistration->FinishFinalize())
		return;

	if (ZQ<int64> theRefconQ = sQGetErase(fMap_Reg2Refcon, iRegistration))
		{
		sEraseMust(fMap_Refcon2Reg, *theRefconQ);
		sInsert(fPending_Unregistrations, *theRefconQ);
		this->pWake();
		}

	delete iRegistration;
	}

} // namespace Dataspace
} // namespace ZooLib
