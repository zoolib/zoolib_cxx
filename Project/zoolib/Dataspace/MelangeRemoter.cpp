// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

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
#include "zoolib/StartOnNewThread.h"
#include "zoolib/Stringf.h"
#include "zoolib/PullPush_JSONB.h"
#include "zoolib/PullPush_ZZ.h"
#include "zoolib/Util_ZZ_JSON.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/QueryEngine/Result.h"

#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace Dataspace {

using namespace Operators_ZZ_JSON;
using namespace Util_STL;

using QueryEngine::Result;

static size_t kBufSize = 16;

// =================================================================================================
#pragma mark -

class Start_Result : public PullPush::Start
	{
protected:
	Start_Result() {}

public:
	static const PPT sPPT;
	static bool sIs(const PPT& iPPT);
	};

const PPT Start_Result::sPPT = ZP<Marker>(new Start_Result);

bool Start_Result::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<Start_Result>())
			return true;
		}
	return false;
	}

// =================================================================================================
#pragma mark -

static string8 spStringFromChan(const ChanR_Bin& r)
	{ return sReadCountPrefixedString(r); }

class ReadFilter
:	public Callable_ZZ_ReadFilter
,	public Callable_JSONB_ReadFilter
	{
public:
// From Callable_ZZ_ReadFilter
	virtual ZQ<bool> QCall(const PPT& iPPT, const ChanR_PPT& iChanR, Val_ZZ& oVal)
		{
		if (Start_Result::sIs(iPPT))
			{
			PPT theRHPPT = sERead(iChanR);
			RelHead& theRelHead = sMut<RelHead>(theRHPPT);
			const size_t theRowCount = size_t(sCoerceInt(sERead(iChanR)));
			size_t theCount = theRowCount * theRelHead.size();
			vector<Val_DB> theVals;
			theVals.reserve(theCount);
			while (theCount--)
				{
				PPT thePPT = sERead(iChanR);
				Val_ZZ munged;
				sPull_PPT_AsZZ(thePPT, iChanR, this, munged);
				sPushBack(theVals, munged.As<Val_DB>());
				}
			oVal = sZP(new Result(&theRelHead, &theVals));

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
						sPush(Start_Result::sPPT, iChanW);

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

						sPush_End(iChanW);

						return true;
						}
					case 101:
						{
						sPush(Daton(sRead_T<Data_ZZ>(iChanR, size_t(sReadCount(iChanR)))), iChanW);
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

static Map_ZZ spReadMessage(const ChanR_Bin& iChanR, const ZQ<string>& iDescriptionQ)
	{
	ChanR_XX_Count<ChanR_Bin> theChanR(iChanR);

	// This ReadFilter handles both the JSONB-->PPT and the PPT->Any translations for
	// Result, Daton and for AbsentOptional_t
	const ZP<ReadFilter> theReadFilter = sDefault<ZP_Counted<ReadFilter>>();

	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	ZP<Delivery<Val_ZZ>> theDelivery = sStartAsync_AsZZ(sGetClear(thePair.second), theReadFilter);
	sPull_JSONB_Push_PPT(theChanR, theReadFilter, ChanW_XX_Buffered<ChanW_PPT>(*thePair.first, kBufSize));
	sDisconnectWrite(*thePair.first);

	ZQ<Val_ZZ> theQ = theDelivery->QGet();
	if (not theQ)
		sThrow_ExhaustedR();

	const Map_ZZ theMessage = theQ->Get<Map_ZZ>();
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

		Util_ZZ_JSON::sWrite(theMessage, false, w);
		}

	return theMessage;
	}

// =================================================================================================
#pragma mark -

class WriteFilter
:	public Callable_ZZ_WriteFilter
,	public Callable_JSONB_WriteFilter
	{
public:
// From Callable_ZZ_WriteFilter
	virtual ZQ<bool> QCall(const Val_ZZ& iVal_ZZ, const ChanW_PPT& iChanW)
		{
		if (const ZP<Result>* theResultP = iVal_ZZ.PGet<ZP<Result>>())
			{
			ZP<Result> theResult = *theResultP;

			sPush(Start_Result::sPPT, iChanW);

				const RelHead& theRH = theResult->GetRelHead();
				sPush(theRH, iChanW);

				const size_t theRowCount = theResult->Count();
				sPush(theRowCount, iChanW);

				const size_t theRHCount = theRH.size();

				for (size_t yy = 0; yy < theRowCount; ++yy)
					{
					const Val_DB* theRow = theResult->GetValsAt(yy);
					for (size_t xx = 0; xx < theRHCount; ++xx)
						sFromZZ_Push_PPT(theRow[xx].As<Val_ZZ>(), this, iChanW);
					}
			sPush_End(iChanW);
			return true;
			}
		return false;
		}

// From Callable_JSONB_WriteFilter
	virtual ZQ<bool> QCall(const PPT& iPPT, const ChanR_PPT& iChanR, const ChanW_Bin& iChanW)
		{
		if (Start_Result::sIs(iPPT))
			{
			sEWriteBE<uint8>(iChanW, 254);
			sEWriteBE<uint8>(iChanW, 100);

			const RelHead theRH = sGet<RelHead>(sERead(iChanR));
			const size_t theRHCount = theRH.size();
			sEWriteCount(iChanW, theRHCount);
			foreacha (entry, theRH)
				sEWriteCountPrefixedString(iChanW, entry);

			const size_t theRowCount = sCoerceInt(sERead(iChanR).As<Val_ZZ>());
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

			const Data_ZZ& theData = theDatonP->GetData();

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

static void spFromZZ_Push_PPT(const Val_ZZ& iVal,
	const ZP<Callable_ZZ_WriteFilter>& iWriteFilter,
	const ZP<ChannerWCon_PPT>& iChannerWCon)
	{
	sFromZZ_Push_PPT(iVal, iWriteFilter, ChanW_XX_Buffered<ChanW_PPT>(*iChannerWCon, kBufSize));
	sDisconnectWrite(*iChannerWCon);
	}

static ZAtomic_t spSentMessageCounter;

static void spWriteMessage(const ChanW_Bin& iChanW, Map_ZZ iMessage, const ZQ<string>& iDescriptionQ)
	{
	ChanW_XX_Count<ChanW_Bin> theChanW(iChanW);

	const ZP<WriteFilter> theWriteFilter = sDefault<ZP_Counted<WriteFilter>>();

	const double start = Time::sSystem();

	iMessage.Set("AAA", sAtomic_Add(&spSentMessageCounter, 1));

	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	sStartOnNewThread(sBindR(sCallable(spFromZZ_Push_PPT),
		Val_ZZ(iMessage),
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
		Util_ZZ_JSON::sWrite(iMessage, false, w);
		}
	}

// =================================================================================================

namespace { // anonymous

ZP<Expr_Rel> spAsRel(const Val_ZZ& iVal)
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

Val_ZZ spAsVal(ZP<Expr_Rel> iRel)
	{
	string8 theString;
	RelationalAlgebra::Util_Strim_Rel::sToStrim_Parseable(iRel, ChanW_UTF_string8(&theString));
	return theString;
	}

// -----

ZP<Result> spAsResult(const Val_ZZ& iVal)
	{
	RelHead theRH;
	foreachv (Val_ZZ theVal, iVal["RelHead"].Get<Seq_ZZ>())
		sInsert(theRH, theVal.Get<string8>());

	std::vector<Val_DB> thePackedRows;

	foreachv (Val_ZZ theVal, iVal["Vals"].Get<Seq_ZZ>())
		sPushBack(thePackedRows, theVal.As<Val_DB>());

	return new Result(&theRH, &thePackedRows);
	}

Val_ZZ spAsVal(ZP<Result> iResult)
	{
	Map_ZZ result;

	const RelHead& theRH = iResult->GetRelHead();

	Seq_ZZ& theSeq_RH = result.Mut<Seq_ZZ>("RelHead");
	foreacha (entry, theRH)
		theSeq_RH.Append(entry);

	const size_t theCount = iResult->Count();
	result.Set("Count", int64(theCount));

	Seq_ZZ& theSeq_Vals = result.Mut<Seq_ZZ>("Vals");

	for (size_t yy = 0; yy < theCount; ++yy)
		{
		const Val_DB* theRow = iResult->GetValsAt(yy);
		for (size_t xx = 0; xx < theRH.size(); ++xx)
			theSeq_Vals.Append(theRow[xx].As<Val_ZZ>());
		}

	return result;
	}

// -----

ZP<ResultDeltas> spAsResultDeltas(const Val_ZZ& iVal)
	{
	ZP<ResultDeltas> theResultDeltas = new ResultDeltas;

	foreachv (Val_ZZ theVal, iVal["Vals"].Get<Seq_ZZ>())
		sPushBack(theResultDeltas->fPackedRows, theVal.As<Val_DB>());

	foreachv (Val_ZZ theVal, iVal["Mapping"].Get<Seq_ZZ>())
		sPushBack(theResultDeltas->fMapping, sCoerceInt(theVal));

	return theResultDeltas;
	}

Val_ZZ spAsVal(ZP<ResultDeltas> iResultDeltas)
	{
	Map_ZZ theMap;

	theMap["Mapping"] = Seq_ZZ(iResultDeltas->fMapping.begin(), iResultDeltas->fMapping.end());

	Seq_ZZ& theSeq = theMap.Mut<Seq_ZZ>("Vals");
	foreacha (aa, iResultDeltas->fPackedRows)
	  	theSeq.Append(aa.As<Val_ZZ>());

	return theMap;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - MelangeServer

struct MelangeServer::ResultCC
	{
	int64 fCC;
	ZP<Result> fResult;
	ZP<ResultDeltas> fResultDeltas;
	};

MelangeServer::MelangeServer(const Melange_t& iMelange,
	const ZP<ChannerRW_Bin>& iChannerRW,
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
	Counted::Initialize();

	fJob = StartScheduler::Job(fMelange.f2, sCallable(sWP(this), &MelangeServer::pWork));

	fCallable_Changed = sCallable(sWP(this), &MelangeServer::pChanged);

	sStartOnNewThread(sCallable(sZP(this), &MelangeServer::pRead));
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
		ZP<ChannerR_Bin> theChannerR = fChannerR;

		Map_ZZ theMap;
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

	ZP<ChannerW_Bin> theChannerW = fChannerW;

	ZAcqMtx acq(fMtx);
	if (sIsEmpty(fMap_Refcon2ResultCC))
		{
		if (fTrueOnce_SendAnEmptyMessage())
			{
			{
			ZRelMtx rel(fMtx);
			spWriteMessage(*theChannerW, Map_ZZ(), fDescriptionQ);
			}
			fTimeOfLastWrite = Time::sSystem();
			}
		}
	else while (not sIsEmpty(fMap_Refcon2ResultCC))
		{
		fTrueOnce_SendAnEmptyMessage.Reset();
		vector<Map_ZZ> theMessages;
		foreacha (entry, fMap_Refcon2ResultCC)
			{
			Map_ZZ theMap;
			theMap.Set("What", "Change");
			theMap.Set("Refcon", entry.first);
			if (sQErase(fSet_NewRefcons, entry.first)
				|| fClientVersion < 2
				|| not entry.second.fResultDeltas)
				{
				theMap.Set("Result", spAsVal(entry.second.fResult));
				}
			else
				{
				theMap.Set("Deltas", spAsVal(entry.second.fResultDeltas));
				}
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
	vector<Map_ZZ> theMessages;
	swap(fQueue_Read, theMessages);

	// Act on messages
	foreachv (Map_ZZ theMessage, theMessages)
		{
		const int64 theCC = sCoerceInt(theMessage.Get("ChangeCount"));
		fLastClientChangeCount = std::max(fLastClientChangeCount, theCC);

		foreacha (entry, theMessage.Get<Seq_ZZ>("Registrations"))
			{
			if (ZQ<Map_ZZ> theMapQ = entry.Get<Map_ZZ>())
				{
				if (ZQ<int64> theRefconQ = sQCoerceInt(theMapQ->Get("Refcon")))
					{
					if (ZP<Expr_Rel> theRel = spAsRel(theMapQ->Get("Rel")))
						{
						RefReg theReg = sCall(fMelange.f0, fCallable_Changed, theRel);
						sInsertMust(fMap_Refcon2Reg, *theRefconQ, theReg);
						sInsertMust(fMap_Reg2Refcon, theReg, *theRefconQ);
						sInsertMust(fSet_NewRefcons, *theRefconQ);
						}
					}
				}
			}

		foreacha (entry, theMessage.Get<Seq_ZZ>("Unregistrations"))
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
		foreacha (entry, theMessage.Get<Seq_ZZ>("Asserts"))
			{
			if (ZQ<Data_ZZ> theDataQ = entry.QGet<Data_ZZ>())
				sPushBack(toInsert, *theDataQ);
			}

		foreacha (entry, theMessage.Get<Seq_ZZ>("Retracts"))
			{
			if (ZQ<Data_ZZ> theDataQ = entry.QGet<Data_ZZ>())
				sPushBack(toErase, *theDataQ);
			}

		if (toInsert.size() || toErase.size())
			{
			ZRelMtx rel(fMtx);
			int64 unusedChangeCount ZMACRO_Attribute_Unused = sCall(fMelange.f1,
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
			sStartOnNewThread(sCallable(sZP(this), &MelangeServer::pWrite));
			}
		else
			{
			fCnd.Broadcast();
			}
		}

	// Should this be Time::System() + fTimeout?
	sNextStartAt(fTimeOfLastWrite + fTimeout, fJob);
	}

static ZP<ResultDeltas> spUnderOver(const ZP<ResultDeltas>& iUnder, const ZP<ResultDeltas>& iOver)
	{
	const size_t countU = iUnder ? iUnder->fMapping.size() : 0;
	if (countU == 0)
		return iOver;

	const size_t countO = iOver ? iOver->fMapping.size() : 0;
	if (countO == 0)
		return iUnder;

	const size_t colCount = iUnder->fPackedRows.size() / countU;

	ZP<ResultDeltas> result = new ResultDeltas;
	result->fPackedRows.reserve(colCount * (countU + countO) / 4);

	size_t iterU = 0, iterO = 0;
	for (;;)
		{
		if (iterU >= countU)
			{
			if (iterO >= countO)
				{
				break;
				}

			// Take/inc O
			}
		else if (iterO >= countO)
			{
			// Take/inc U
			}
		else
			{
			size_t mappingU = iUnder->fMapping[iterU];
			size_t mappingO = iOver->fMapping[iterO];
			if (mappingU < mappingO)
				{
				// Take/inc U
				}
			else if (mappingU > mappingO)
				{
				// Take/inc O
				}
			else
				{
				// inc U
				// Take/inc O
				}
			}
		}
	return result;
	}

void MelangeServer::pChanged(
	const RefReg& iRegistration,
	int64 iChangeCount,
	const ZP<Result>& iResult,
	const ZP<ResultDeltas>& iResultDeltas)
	{
	ZAcqMtx acq(fMtx);

	const int64 theRefCon = sGetMust(fMap_Reg2Refcon, iRegistration);
	ResultCC* theResultCC = sPMut(fMap_Refcon2ResultCC, theRefCon);
	if (theResultCC && theResultCC->fResultDeltas)
		{
		if (ZLOGF(w, eErr))
			{
			if (fDescriptionQ)
				w << *fDescriptionQ << " ";
			w << "Already had an entry for registration " << theRefCon;
			}
		theResultCC->fResult = iResult;
		theResultCC->fResultDeltas.Clear();

//		theResultCC->fResultDeltas = spUnderOver(theResultCC->fResultDeltas, iResultDelta);
		}
	else
		{
		sSet(fMap_Refcon2ResultCC, theRefCon,
			ResultCC({fLastClientChangeCount, iResult, iResultDeltas}));
		}

	this->pWake();
	}

// =================================================================================================
#pragma mark - Melange_Client::Registration

class Melange_Client::Registration
:	public Counted
	{
public:
	Registration(ZP<Melange_Client> iClient,
		ZP<RelsWatcher::Callable_Changed> iCallable_Changed,
		const ZP<Expr_Rel>& iRel)
	:	fClient(iClient)
	,	fCallable_Changed(iCallable_Changed)
	,	fRel(iRel)
		{}

// From Counted
	virtual void Finalize()
		{ fClient->pFinalize(this); }

	ZP<Melange_Client> fClient;
	ZP<RelsWatcher::Callable_Changed> fCallable_Changed;
	ZP<Expr_Rel> fRel;
	};

// =================================================================================================
#pragma mark - Melange_Client

Melange_Client::Melange_Client(const ZP<Factory_Channer>& iFactory,
	const ZP<Callable_Status>& iCallable_Status)
:	fFactory(iFactory)
,	fCallable_Status(iCallable_Status)
,	fGettingChanner(false)
,	fReadSinceWrite(false)
,	fChangeCount(1)
,	fNextRefcon(1)
	{}

ZP<Counted> Melange_Client::QCall(
	const ZP<RelsWatcher::Callable_Changed>& iCallable_Changed,
	const ZP<Expr_Rel>& iRel)
	{
	// We're probably on our own starter.
	// Shove it into data structure, and wake the starter, which will itself wake the writer.

	ZAcqMtx acq(fMtx);

	ZP<Registration> theRegistration = new Registration(this, iCallable_Changed, iRel);

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

void Melange_Client::Start(ZP<Starter> iStarter)
	{
	fJob = StartScheduler::Job(iStarter, sCallable(sWP(this), &Melange_Client::pWork));

	sStartOnNewThread(sCallable(sZP(this), &Melange_Client::pRead));
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
			ZP<ChannerForRead> theChanner = this->pEnsureChannerR();

			if (not theChanner)
				continue;

			Map_ZZ theMap;
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

		ZP<ChannerW_Bin> theChannerW = this->pEnsureChannerW();
		if (not theChannerW)
			break;

		vector<Map_ZZ> theMessages;
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
	std::set<ZP<Registration>> localRegistrations;

	ZAcqMtx acq(fMtx);

	// Pull stuff from fQueue_Read
	vector<Map_ZZ> theMessages;
	swap(fQueue_Read, theMessages);

	{
	ZRelMtx rel(fMtx);

	foreachv (Map_ZZ theMessage, theMessages)
		{
		if (ZQ<int64> theRefconQ = sQCoerceInt(theMessage.Get("Refcon")))
			{
			const int64 theChangeCount = sCoerceInt(theMessage.Get("ChangeCount"));

			ZP<ResultDeltas> theResultDeltas;
			if (const Val_ZZ* theP = theMessage.PGet("Deltas"))
				theResultDeltas = spAsResultDeltas(*theP);

			ZP<Result> theResult;
			if (const Val_ZZ* theP = theMessage.PGet("Result"))
				theResult = spAsResult(*theP);

			if (ZP<Registration> theReg = sGet(fMap_Refcon2Reg, *theRefconQ))
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

	Map_ZZ theMessage;

	if (sNotEmpty(fPending_Registrations))
		{
		Seq_ZZ& theSeq = theMessage.Mut<Seq_ZZ>("Registrations");
		foreachv (ZP<Registration> theReg, fPending_Registrations)
			{
			Map_ZZ theMap;
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
		Seq_ZZ& theSeq = theMessage.Mut<Seq_ZZ>("Unregistrations");
		foreacha (entry, fPending_Unregistrations)
			theSeq.Append(entry);
		sClear(fPending_Unregistrations);
		}

	if (sNotEmpty(fPending_Asserts))
		{
		Seq_ZZ& theSeq = theMessage.Mut<Seq_ZZ>("Asserts");
		foreacha (entry, fPending_Asserts)
			theSeq.Append(entry.GetData());
		sClear(fPending_Asserts);
		}

	if (sNotEmpty(fPending_Retracts))
		{
		Seq_ZZ& theSeq = theMessage.Mut<Seq_ZZ>("Retracts");
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
			sStartOnNewThread(sCallable(sZP(this), &Melange_Client::pWrite));
		else
			fCnd.Broadcast();
		}
	}

ZP<Melange_Client::ChannerForRead> Melange_Client::pEnsureChannerR()
	{ return this->pEnsureChanner(); }

ZP<ChannerW_Bin> Melange_Client::pEnsureChannerW()
	{ return this->pEnsureChanner(); }

ZP<Melange_Client::Channer_t> Melange_Client::pEnsureChanner()
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

		ZP<Channer_t> theChanner;
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
