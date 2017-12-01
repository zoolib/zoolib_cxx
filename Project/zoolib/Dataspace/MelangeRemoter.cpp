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

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Callable_PMF.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/ChanR_Bin_More.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Log.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_Any.h"
#include "zoolib/Util_Any_JSON.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_JSONB.h"

#include "zoolib/Dataspace/MelangeRemoter.h"

#include "zoolib/QueryEngine/Result.h"

#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/ChanRU_XX_Unreader.h"

#include "zoolib/StdIO.h"

namespace ZooLib {
namespace Dataspace {

using namespace Operators_Any_JSON;
using namespace Util_STL;

using QueryEngine::Result;

// =================================================================================================
#pragma mark -

static void spToChan(const ChanW_Bin& w, const string8& iString)
	{
	const size_t theLength = iString.length();
	sEWriteCount(w, theLength);
	if (theLength)
		{
		if (theLength != sQWriteMemFully(w, iString.data(), theLength))
			sThrow_ExhaustedW();
		}
	}

static string8 spStringFromChan(const ChanR_Bin& r)
	{
	if (size_t theCount = sReadCount(r))
		return sReadString(r, theCount);
	return string8();
	}

// =================================================================================================
#pragma mark -

class ReadFilter_Result
:	public Yad_JSONB::ReadFilter
	{
public:
	virtual ZQ<Any> QRead(const ZRef<ChannerR_Bin>& iChannerR_Bin)
		{
		const ChanR_Bin& r = *iChannerR_Bin;

		if (ZQ<uint8> theTypeQ = sQReadBE<uint8>(r))
			{
			switch (*theTypeQ)
				{
				case 100:
					{
					// We're at the beginning of a QE::Result. So read the RelHead to start with.
					RelHead theRelHead;
					for (size_t theCount = sReadCount(r); theCount; --theCount)
						theRelHead |= spStringFromChan(r);

					// Now the vals
					vector<Val_Any> packedRows;
					for (size_t theCount = sReadCount(r) * theRelHead.size(); theCount; --theCount)
						{
						if (ZQ<Val_Any> theQ = Yad_Any::sQFromYadR(Yad_JSONB::sYadR(this, iChannerR_Bin)))
							packedRows.push_back(*theQ);
						else
							ZUnimplemented(); // return null;
						}

					return sRef(new Result(theRelHead, &packedRows));
					}
				case 101:
					{
					Data_Any theData(sReadCount(r));
					sEReadMem(r, theData.GetPtrMutable(), theData.GetSize());
					return Daton(theData);
					}
				case 102:
					{
					return AbsentOptional_t();
					}
				}

			if (ZLOGF(w, eDebug))
				w << *theTypeQ;
			ZUnimplemented();
			}
		return null;
		}
	};

// =================================================================================================
#pragma mark -

class WriteFilter_Result
:	public Yad_JSONB::WriteFilter
	{
public:
	virtual bool QWrite(const Any& iAny, const ChanW_Bin& w)
		{
		if (false)
			{}
		else if (const ZRef<Result>* theResultP = iAny.PGet<ZRef<Result> >())
			{
			sEWriteBE<uint8>(w, 100);

			const ZRef<Result> theResult = *theResultP;

			const RelHead& theRH = theResult->GetRelHead();
			const size_t theRHCount = theRH.size();

			sEWriteCount(w, theRHCount);

			foreachi (ii, theRH)
				spToChan(w, *ii);

			const size_t theRowCount = theResult->Count();

			sEWriteCount(w, theRowCount);

			for (size_t yy = 0; yy < theRowCount; ++yy)
				{
				const Val_Any* theRow = theResult->GetValsAt(yy);
				for (size_t xx = 0; xx < theRHCount; ++xx)
					Yad_JSONB::sToChan(this, sYadR(theRow[xx]),w);
				}
			return true;
			}
		else if (const Daton* theDatonP = iAny.PGet<Daton>())
			{
			sEWriteBE<uint8>(w, 101);

			const Data_Any& theData = theDatonP->GetData();

			sEWriteCount(w, theData.GetSize());
			sEWriteMem(w, theData.GetPtr(), theData.GetSize());
			return true;
			}
		else if (iAny.PGet<AbsentOptional_t>())
			{
			sEWriteBE<uint8>(w, 102);
			return true;
			}
		else
			{
			if (ZLOGPF(w, eDebug))
				w << iAny.Type().name();
			ZUnimplemented();
			}
		return false;
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark

static void spWriteMessage(const ChanW_Bin& iChanW, const Map_Any& iMessage, const ZQ<string>& iDescriptionQ)
	{
	const ZRef<Yad_JSONB::WriteFilter> theWriteFilter = sDefault<ZRef_Counted<WriteFilter_Result> >();

	const double start = Time::sSystem();

	Yad_JSONB::sToChan(theWriteFilter, sYadR(iMessage), iChanW);
	sFlush(iChanW);

	const double finish = Time::sSystem();

	if (ZLOGF(w, eDebug + 1))
		{
		if (iDescriptionQ)
			w << *iDescriptionQ << ", ";
		w << 1e3 * (finish - start) << "ms";
		if (iMessage.IsEmpty())
			w << ", ";
		else
			w << "\n";
		Util_Any_JSON::sWrite(false, iMessage, w);
		}
	}

static Map_Any spReadMessage(const ZRef<ChannerR_Bin>& iChannerR, const ZQ<string>& iDescriptionQ)
	{
	const ZRef<Yad_JSONB::ReadFilter> theReadFilter = sDefault<ZRef_Counted<ReadFilter_Result> >();

	ZQ<Val_Any> theQ = Yad_Any::sQFromYadR(Yad_JSONB::sYadR(theReadFilter, iChannerR));
	if (not theQ)
		sThrow_ExhaustedR();

	const Map_Any theMessage = theQ->Get<Map_Any>();
	if (ZLOGF(w, eDebug + 1))
		{
		if (iDescriptionQ)
			{
			w << *iDescriptionQ;
			if (theMessage.IsEmpty())
				w << ", ";
			}
		if (not theMessage.IsEmpty())
			w << "\n";
		Util_Any_JSON::sWrite(false, theMessage, w);
		}

	return theMessage;
	}

// =================================================================================================
#pragma mark -
#pragma mark

namespace { // anonymous

ZRef<Expr_Rel> spAsRel(const Val_Any& iVal)
	{
	if (NotQ<string8> theStringQ = iVal.QGet<string8>())
		{
		return null;
		}
	else
		{
		if (ZLOGF(w, eDebug + 1))
			w << "\n" << *theStringQ;
		ZRef<Channer<ChanRU<UTF32>> > theChanner = sChanner_T<ChanRU_UTF_string8>(*theStringQ);
		return RelationalAlgebra::Util_Strim_Rel::sQFromStrim(*theChanner, *theChanner);
		}
	}

Val_Any spAsVal(ZRef<Expr_Rel> iRel)
	{
	string8 theString;
	RelationalAlgebra::Util_Strim_Rel::sToStrim_Parseable(iRel, ChanW_UTF_string8(&theString));

	if (ZLOGF(w,eDebug + 1))
		w << "\n" << theString;

	return theString;
	}

ZRef<Result> spAsResult(const Val_Any& iVal)
	{
	RelHead theRH;
	foreachv (Val_Any theVal, iVal.Get<Map_Any>().Get<Seq_Any>("RelHead"))
		sInsert(theRH, theVal.Get<string8>());

	std::vector<Val_Any> thePackedRows;

	foreachv (Val_Any theVal, iVal.Get<Map_Any>().Get<Seq_Any>("Vals"))
		sPushBack(thePackedRows, theVal);

	return new Result(theRH, &thePackedRows);
	}

Val_Any spAsVal(ZRef<Result> iResult)
	{
	Map_Any result;

	const RelHead& theRH = iResult->GetRelHead();

	Seq_Any& theSeq_RH = result.Mut<Seq_Any>("RelHead");
	foreachi (ii, theRH)
		theSeq_RH.Append(*ii);

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

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark MelangeServer

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
,	fTimeout(10)
,	fConnectionTimeout(30)
	{}

MelangeServer::~MelangeServer()
	{
	if (ZLOGF(w, eDebug))
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
		ZThread::sSetName(("MSR:" + *fDescriptionQ).c_str());
	else
		ZThread::sSetName("MelangeServer::pRead");

	ZGuardMtxR guard(fMtxR);
	for (;;)
		{
		ZRef<ChannerR_Bin> theChannerR = fChannerR;

		// Want to be able to do a WaitRead here, and wake ourselves if necessary? Then pWork
		// can abort the connection, and we're unwind. Or pWork can just check for fTimeOfLastRead
		// getting too stale and do the abort anyway.

		guard.Release();

		const Map_Any theMap = spReadMessage(theChannerR, fDescriptionQ);

		guard.Acquire();

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
		ZThread::sSetName("MelangeServer::pWrite");

	ZRef<ChannerW_Bin> theChannerW = fChannerW;

	ZGuardMtxR guard(fMtxR);
	if (sIsEmpty(fMap_Refcon2Result))
		{
		if (fTrueOnce_SendAnEmptyMessage())
			{
			guard.Release();
			spWriteMessage(*theChannerW, Map_Any(), fDescriptionQ);
			guard.Acquire();
			fTimeOfLastWrite = Time::sSystem();
			}
		}
	else while (not sIsEmpty(fMap_Refcon2Result))
		{
		fTrueOnce_SendAnEmptyMessage.Reset();
		vector<Map_Any> theMessages;
		foreachi (ii, fMap_Refcon2Result)
			{
			Map_Any theMap;
			theMap.Set("IsFirst", false); //## For old clients for now.
			theMap.Set("What", "Change");
			theMap.Set("Refcon", ii->first);
			theMap.Set("Result", spAsVal(ii->second));
			sPushBack(theMessages, theMap);
			}
		sClear(fMap_Refcon2Result);

		foreachi (ii, theMessages)
			{
			guard.Release();
			spWriteMessage(*theChannerW, *ii, fDescriptionQ);
			guard.Acquire();
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
	ZGuardMtxR guard(fMtxR);

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
		foreachi (ii, theMessage.Get<Seq_Any>("Registrations"))
			{
			if (ZQ<Map_Any> theMapQ = ii->Get<Map_Any>())
				{
				if (ZQ<int64> theRefconQ = sQCoerceInt(theMapQ->Get("Refcon")))
					{
					if (ZRef<Expr_Rel> theRel = spAsRel(theMapQ->Get("Rel")))
						{
						RefReg theReg = sCall(fMelange.f0, fCallable_Changed, theRel);
						sInsertMust(fMap_Refcon2Reg, *theRefconQ, theReg);
						sInsertMust(fMap_Reg2Refcon, theReg, *theRefconQ);
						}
					}
				}
			}

		foreachi (ii, theMessage.Get<Seq_Any>("Unregistrations"))
			{
			if (ZQ<int64> theRefconQ = sQCoerceInt(*ii))
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
					sErase(fMap_Refcon2Result, *theRefconQ);
					}
				}
			}

		vector<Daton> toInsert, toErase;
		foreachi (ii, theMessage.Get<Seq_Any>("Asserts"))
			{
			if (ZQ<Data_Any> theDataQ = ii->QGet<Data_Any>())
				sPushBack(toInsert, *theDataQ);
			}

		foreachi (ii, theMessage.Get<Seq_Any>("Retracts"))
			{
			if (ZQ<Data_Any> theDataQ = ii->QGet<Data_Any>())
				sPushBack(toErase, *theDataQ);
			}

		// Handle old message
		foreachi (ii, theMessage.Get<Seq_Any>("Updates"))
			{
			if (ZQ<Map_Any> theMapQ = ii->Get<Map_Any>())
				{
				if (ZQ<bool> theBoolQ = sQCoerceBool(theMapQ->Get("True")))
					{
					if (ZQ<Data_Any> theDataQ = theMapQ->QGet<Data_Any>("Daton"))
						{
						if (*theBoolQ)
							sPushBack(toInsert, *theDataQ);
						else
							sPushBack(toErase, *theDataQ);
						}
					}
				}
			}

		if (toInsert.size() || toErase.size())
			{
			guard.Release();
			sCall(fMelange.f1, sFirstOrNil(toInsert), toInsert.size(),
				sFirstOrNil(toErase), toErase.size());
			guard.Acquire();
			}
		}

	if (sNotEmpty(fMap_Refcon2Result) || fTimeOfLastWrite + fTimeout < Time::sSystem())
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
	sNextStartAt(fTimeOfLastWrite + fTimeout, fJob);
	}

void MelangeServer::pChanged(
	const RefReg& iRegistration,
	const ZRef<Result>& iResult)
	{
	ZGuardMtxR guard(fMtxR);

	sSet(fMap_Refcon2Result, sGetMust(fMap_Reg2Refcon, iRegistration), iResult);

	this->pWake();
	}

// =================================================================================================
#pragma mark -
#pragma mark Melange_Client::Registration

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
#pragma mark -
#pragma mark Melange_Client

Melange_Client::Melange_Client(const ZRef<Factory_Channer>& iFactory,
	const ZRef<Callable_Status>& iCallable_Status)
:	fFactory(iFactory)
,	fCallable_Status(iCallable_Status)
,	fGettingChanner(false)
,	fReadSinceWrite(false)
,	fNextRefcon(1)
	{}

ZQ<ZRef<ZCounted> > Melange_Client::QCall(
	const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
	const ZRef<Expr_Rel>& iRel)
	{
	// We're probably on our own starter.
	// Shove it into data structure, and wake the starter, which will itself wake the writer.

	ZGuardMtxR guard(fMtxR);

	ZRef<Registration> theRegistration = new Registration(this, iCallable_Changed, iRel);

	sInsertMust(fPending_Registrations, theRegistration);

	this->pWake();

	return theRegistration;
	}

ZQ<void> Melange_Client::QCall(
	const Daton* iAsserted, size_t iAssertedCount,
	const Daton* iRetracted, size_t iRetractedCount)
	{
	ZGuardMtxR guard(fMtxR);

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
	return notnull;
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

// --

class ChanR_Bin_AbortOnSlowRead
:	public ChanR_Bin
	{
public:
	typedef Melange_Client::ChanForRead ChanForRead;

	ChanR_Bin_AbortOnSlowRead(double iTimeout, const ChanForRead& iChan)
	:	fTimeout(iTimeout)
	,	fChan(iChan)
		{}

	virtual size_t QRead(byte* oDest, size_t iCount)
		{
		if (not sWaitReadable(fChan, fTimeout))
			{
			sAbort(fChan);
			sThrow_ExhaustedR();
			}

		return sQRead(fChan, oDest, iCount);
		}

	const double fTimeout;
	const ChanForRead& fChan;
	};

// --

void Melange_Client::pRead()
	{
	ZThread::sSetName("Melange_Client::pRead");

	ZGuardMtxR guard(fMtxR);
	for (;;)
		{
		try
			{
			ZRef<ChannerForRead> theChanner = this->pEnsureChannerR(guard);

			if (not theChanner)
				continue;

			Map_Any theMap;
			{
			ZRelGuardR rel(guard);
			theMap = spReadMessage(sChanner_Channer_T<ChanR_Bin_AbortOnSlowRead>(15, theChanner), null);
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
	ZThread::sSetName("Melange_Client::pWrite");

	ZGuardMtxR guard(fMtxR);
	for (;;)
		{
		if (sIsEmpty(fQueue_ToWrite))
			{
			// Give it a second to fill up.
			fCnd.WaitFor(fMtxR, 1);
			if (sIsEmpty(fQueue_ToWrite))
				{
				// It's still empty, drop out of the loop and let the thread dispose.
				break;
				}
			}

		ZRef<ChannerW_Bin> theChannerW = this->pEnsureChannerW(guard);
		if (not theChannerW)
			break;

		vector<Map_Any> theMessages;
		swap(fQueue_ToWrite, theMessages);

		try
			{
			ZRelGuardR rel(guard);
			foreachi (ii, theMessages)
				spWriteMessage(*theChannerW, *ii, null);
			}
		catch (...)
			{
			break;
			}
		}
	fTrueOnce_WriteNeedsStart.Reset();
	}

void Melange_Client::pWake()
	{ sNextStartIn(0, fJob); }

void Melange_Client::pWork()
	{
	// Handle everything that's in fQueue_Read -- mainly doing change notifications

	ZGuardMtxR guard(fMtxR);

	// Pull stuff from fQueue_Read
	vector<Map_Any> theMessages;
	swap(fQueue_Read, theMessages);
	guard.Release();

	foreachv (Map_Any theMessage, theMessages)
		{
		if (ZQ<int64> theRefconQ = sQCoerceInt(theMessage.Get("Refcon")))
			{
			// Get the registration and call its callable
			if (ZRef<Result> theResult = spAsResult(theMessage.Get("Result")))
				{
				if (ZRef<Registration> theReg = sGet(fMap_Refcon2Reg, *theRefconQ))
					sCall(theReg->fCallable_Changed, theReg, theResult);
				}
			}
		}

	// Invoke everything that needed to be called from us as a starter.
	Starter_EventLoopBase::pInvokeClearQueue();

	guard.Acquire();

	if (sNotEmpty(theMessages))
		{
		// We've read something, so may need to issue a no-op write.
		fReadSinceWrite = true;
		if (ZLOGF(w, eDebug + 1))
			w << "fReadSinceWrite = true";
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
		sClear(fPending_Registrations);
		}

	if (sNotEmpty(fPending_Unregistrations))
		{
		Seq_Any& theSeq = theMessage.Mut<Seq_Any>("Unregistrations");
		foreachi (ii, fPending_Unregistrations)
			theSeq.Append(*ii);
		sClear(fPending_Unregistrations);
		}

	if (sNotEmpty(fPending_Asserts))
		{
		Seq_Any& theSeq = theMessage.Mut<Seq_Any>("Asserts");
		foreachi (ii, fPending_Asserts)
			theSeq.Append(ii->GetData());
		sClear(fPending_Asserts);
		}

	if (sNotEmpty(fPending_Retracts))
		{
		Seq_Any& theSeq = theMessage.Mut<Seq_Any>("Retracts");
		foreachi (ii, fPending_Retracts)
			theSeq.Append(ii->GetData());
		sClear(fPending_Retracts);
		}

	if (not theMessage.IsEmpty() || (fReadSinceWrite && sIsEmpty(fQueue_ToWrite)))
		{
		if (ZLOGF(w, eDebug + 1))
			{
			w	<< "theMessage.IsEmpty() == " << theMessage.IsEmpty() << ", "
				<< "fReadSinceWrite == " << fReadSinceWrite << ", "
				<< "sIsEmpty(fQueue_ToWrite) == " << sIsEmpty(fQueue_ToWrite);
			}
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

ZRef<Melange_Client::ChannerForRead> Melange_Client::pEnsureChannerR(ZGuardMtxR& iGuard)
	{ return this->pEnsureChanner(iGuard); }

ZRef<ChannerW_Bin> Melange_Client::pEnsureChannerW(ZGuardMtxR& iGuard)
	{ return this->pEnsureChanner(iGuard); }

ZRef<Melange_Client::Channer_t> Melange_Client::pEnsureChanner(ZGuardMtxR& iGuard)
	{
	while (fGettingChanner)
		fCnd.WaitFor(fMtxR, 5);

	if (not fChanner)
		{
		// Registrations become pending, but writes are discarded -- we'll get replacement
		// values when we reconnect, and our caller can reapply their work, if appropriate.

		foreachi (ii, fMap_Reg2Refcon)
			sInsertMust(fPending_Registrations, ii->first);

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

		iGuard.Release();

		if (ZLOGF(w, eDebug - 1))
			w << "No Channer";

		sCall(fCallable_Status, false);

		ZRef<Channer_t> theChanner = sCall(fFactory);
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

		iGuard.Acquire();

		fChanner = theChanner;
		fCnd.Broadcast();
		}

	return fChanner;
	}

void Melange_Client::pFinalize(Registration* iRegistration)
	{
	ZGuardMtxR guard(fMtxR);
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
