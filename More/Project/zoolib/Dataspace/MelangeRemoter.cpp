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

#include "zoolib/dataspace/MelangeRemoter.h"

#include "zoolib/QueryEngine/Result.h"

#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/ChanRU_XX_Unreader.h"

#include "zoolib/StdIO.h"

#include "zoolib/QueryEngine/Transform_Search.h"
#include "zoolib/RelationalAlgebra/Transform_ConsolidateRenames.h"
#include "zoolib/RelationalAlgebra/Transform_DecomposeRestricts.h"
#include "zoolib/RelationalAlgebra/Transform_PushDownRestricts.h"

namespace ZooLib {
namespace Dataspace {

using namespace Operators_Any_JSON;
using namespace Util_STL;

using QueryEngine::Result;
using DatonSet::Daton;

// =================================================================================================
#pragma mark -

static void spToChan(const string8& iString, const ChanW_Bin& w)
	{
	const size_t theLength = iString.length();
	sWriteCountMust(theLength, w);
	if (theLength)
		{
		if (theLength != sQWriteFully(iString.data(), theLength, w))
			sThrow_ExhaustedW();
		}
	}

static string8 spStringFromChan(const ChanR_Bin& r)
	{
	if (size_t theCount = sReadCount(r))
		return sReadString(theCount, r);
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
		const ChanR_Bin& r = sGetChan(iChannerR_Bin);

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
					sReadMust(theData.GetPtrMutable(), theData.GetSize(), r);
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
			sWriteBE<uint8>(100, w);

			const ZRef<Result> theResult = *theResultP;

			const RelHead& theRH = theResult->GetRelHead();
			const size_t theRHCount = theRH.size();

			sWriteCountMust(theRHCount, w);

			foreachi (ii, theRH)
				spToChan(*ii, w);

			const size_t theRowCount = theResult->Count();

			sWriteCountMust(theRowCount, w);

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
			sWriteBE<uint8>(101, w);

			const Data_Any& theData = theDatonP->GetData();

			sWriteCountMust(theData.GetSize(), w);
			sWriteMust(theData.GetPtr(), theData.GetSize(), w);
			return true;
			}
		else if (iAny.PGet<AbsentOptional_t>())
			{
			sWriteBE<uint8>(102, w);
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

static void spWriteMessage(const Map_Any& iMessage, const ChanW_Bin& iChanW)
	{
	const double start = Time::sSystem();

	Yad_JSONB::sToChan(sSingleton<ZRef_Counted<WriteFilter_Result> >(), sYadR(iMessage), iChanW);
	sFlush(iChanW);

	const double finish = Time::sSystem();

	if (ZLOGF(w, eDebug))
		w << 1e3 * (finish - start) << "ms\n" << iMessage;
	}

static Map_Any spReadMessage(const ZRef<ChannerR_Bin>& iChannerR)
	{
	ZRef<ChannerRU_XX_Unreader<byte> > theChannerRU = new ChannerRU_XX_Unreader<byte>(iChannerR);

	ZQ<byte> theByteQ = sQRead(sGetChan<ChanR_Bin>(theChannerRU));
	if (not theByteQ)
		sThrow_ExhaustedR();

	sUnread(*theByteQ, sGetChan<ChanU<byte> >(theChannerRU));

	const double start = Time::sSystem();

	ZQ<Val_Any> theQ = Yad_Any::sQFromYadR(Yad_JSONB::sYadR(sSingleton<ZRef_Counted<ReadFilter_Result> >(), theChannerRU));
	if (not theQ)
		sThrow_ExhaustedR();

	const double finish = Time::sSystem();

	const Map_Any theMessage = theQ->Get<Map_Any>();
	if (ZLOGF(w, eDebug))
		w << 1e3 * (finish - start) << "ms\n" << theMessage;

	return theMessage;
	}

namespace { // anonymous

ZRef<Expr_Rel> spAsRel(const Val_Any& iVal)
	{
	if (NotQ<string8> theStringQ = iVal.QGet<string8>())
		{
		return null;
		}
	else
		{
		if (ZLOGF(w, eDebug))
			w << "\n" << *theStringQ;
		ChanRU_UTF_string8 theChan(*theStringQ);
		return RelationalAlgebra::Util_Strim_Rel::sQFromStrim(theChan, theChan);
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

MelangeServer::MelangeServer(const Melange_t& iMelange, const ZRef<ChannerRW_Bin>& iChannerRW)
:	fMelange(iMelange)
,	fChannerR(iChannerRW)
,	fChannerW(iChannerRW)
	{}

void MelangeServer::Initialize()
	{
	ZCounted::Initialize();

	fJob = StartScheduler::Job(fMelange.f2, sCallable(sWeakRef(this), &MelangeServer::pWork));

	fCallable_Changed = sCallable(sWeakRef(this), &MelangeServer::pChanged);

	sStartOnNewThread(sCallable(sRef(this), &MelangeServer::pRead));
	}

void MelangeServer::pRead()
	{
	ZThread::sSetName("MelangeServer::pRead");

	ZGuardMtxR guard(fMtxR);
	while (fChannerR)
		{
		ZRef<ChannerR_Bin> theChannerR = fChannerR;

		guard.Release();

		const Map_Any theMap = spReadMessage(theChannerR);

		guard.Acquire();

		fQueue_Read.push_back(theMap);
		this->pWake();
		}
	}

void MelangeServer::pWrite()
	{
	ZGuardMtxR guard(fMtxR);
	while (fChannerW)
		{
		if (sIsEmpty(fQueue_ToWrite))
			{
			fCnd.WaitFor(fMtxR, 1);
			if (sIsEmpty(fQueue_ToWrite))
				break;
			continue;
			}

		ZRef<ChannerW_Bin> theChannerW = fChannerW;

		vector<Map_Any> theMessages;
		swap(fQueue_ToWrite, theMessages);

		guard.Release();

		foreachi (ii, theMessages)
			spWriteMessage(*ii, sGetChan(theChannerW));

		guard.Acquire();
		}
	fTrueOnce_WriteNeedsStart.Reset();
	}

void MelangeServer::pWake()
	{ sNextStartIn(0, fJob); }

void MelangeServer::pWork()
	{
	ZGuardMtxR guard(fMtxR);

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
						ZRef<ZCounted> theRegistration =
							sCall(fMelange.f0, fCallable_Changed, theRel);
						sInsertMust(fMap_Refcon2Reg, *theRefconQ, theRegistration);
						sInsertMust(fMap_Reg2Refcon, theRegistration, *theRefconQ);
						}
					}
				}
			}

		foreachi (ii, theMessage.Get<Seq_Any>("Unregistrations"))
			{
			if (ZQ<int64> theRefconQ = sQCoerceInt(*ii))
				{
				const ZRef<ZCounted> theRegistration =
					sGetEraseMust(fMap_Refcon2Reg, *theRefconQ);

				const int64 theRefcon =
					sGetEraseMust(fMap_Reg2Refcon, theRegistration);

				ZAssert(theRefcon == *theRefconQ);
				}
			}

		foreachi (ii, theMessage.Get<Seq_Any>("Updates"))
			{
			if (ZQ<Map_Any> theMapQ = ii->Get<Map_Any>())
				{
				if (ZQ<bool> theBoolQ = sQCoerceBool(theMapQ->Get("True")))
					{
					if (ZQ<Data_Any> theDataQ = theMapQ->QGet<Data_Any>("Daton"))
						sCall(fMelange.f1, *theDataQ, *theBoolQ);
					}
				}
			}
		}

	if (sNotEmpty(fQueue_ToWrite))
		{
		if (fTrueOnce_WriteNeedsStart())
			sStartOnNewThread(sCallable(sRef(this), &MelangeServer::pWrite));
		else
			fCnd.Broadcast();
		}
	}

void MelangeServer::pChanged(
	const ZRef<ZCounted>& iRegistration,
	const ZRef<Result>& iResult,
	bool iIsFirst)
	{
	ZGuardMtxR guard(fMtxR);
	Map_Any theMap;

	theMap.Set("What", "Change");
	theMap.Set("Refcon", sGetMust(fMap_Reg2Refcon, iRegistration));
	theMap.Set("IsFirst", iIsFirst);
	theMap.Set("Result", spAsVal(iResult));

	sPushBack(fQueue_ToWrite, theMap);

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

Melange_Client::Melange_Client(const ZRef<Factory_ChannerRW_Bin>& iFactory,
  const ZRef<Callable_Status>& iCallable_Status)
:	fFactory(iFactory)
, fCallable_Status(iCallable_Status)
,	fGettingChanner(false)
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

ZQ<void> Melange_Client::QCall(const DatonSet::Daton& iDaton, bool iTrue)
	{
	std::map<Daton,bool>::iterator iter = fPending_Updates.lower_bound(iDaton);
	if (iter != fPending_Updates.end() && iter->first == iDaton)
		{
		if (iter->second != iTrue)
			fPending_Updates.erase(iter);
		}
	else
		{
		sInsertMust(fPending_Updates, iDaton, iTrue);
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

void Melange_Client::pWake()
	{ sNextStartIn(0, fJob); }

void Melange_Client::pWork()
	{
	// Handle everything that's in fQueue_Read -- mainly doing change notifications

	ZGuardMtxR guard(fMtxR);

	// Pull stuff from fQueue_Read
	vector<Map_Any> theMessages;
	swap(fQueue_Read, theMessages);

	foreachv (Map_Any theMessage, theMessages)
		{
		if (ZQ<int64> theRefconQ = sQCoerceInt(theMessage.Get("Refcon")))
			{
			// Get the registration and call its callable
			if (ZQ<bool> theIsFirst = sQCoerceBool(theMessage.Get("IsFirst")))
				{
				if (ZRef<Result> theResult = spAsResult(theMessage.Get("Result")))
					{
					if (ZRef<Registration> theReg = sGet(fMap_Refcon2Reg, *theRefconQ))
						sCall(theReg->fCallable_Changed, theReg, theResult, *theIsFirst);
					}
				}
			}
		}

	// trigger write of anything that's now pending, if necessary.
	if (sNotEmpty(fPending_Registrations)
		|| sNotEmpty(fPending_Unregistrations)
		|| sNotEmpty(fPending_Updates))
		{
		if (fTrueOnce_WriteNeedsStart())
			sStartOnNewThread(sCallable(sRef(this), &Melange_Client::pWrite));
		else
			fCnd.Broadcast();
		}

	// Invoke everything that needed to be called from us as a starter.
	Starter_EventLoopBase::pInvokeClearQueue();
	}

void Melange_Client::pRead()
	{
	ZThread::sSetName("Melange_Client::pRead");
	for (;;)
		{
		try
			{
			ZRef<ChannerRW_Bin> theChanner = this->pEnsureChanner();

			ZGuardMtxR guard(fMtxR);

			if (not theChanner)
				continue;

			Map_Any theMap;
			{
			ZRelGuardR rel(guard);
			theMap = spReadMessage(theChanner);
			}

			fQueue_Read.push_back(theMap);
			this->pWake();
			}
		catch (...)
			{
			ZGuardMtxR guard(fMtxR);
			fChanner.Clear();
			}
		}
	}

void Melange_Client::pWrite()
	{
	ZThread::sSetName("Melange_Client::pWrite");

	try
		{
		if (this->pWrite_Inner())
			{
			// the write succeded. It will have moved pending entries into their real containers.
			return;
			}
		}
	catch (...)
		{}

	// The write failed in some fashion, ditch the channer (if any).

	ZGuardMtxR guard(fMtxR);

	fChanner.Clear();
	}

bool Melange_Client::pWrite_Inner()
	{
	ZRef<ChannerW_Bin> theChannerW = this->pEnsureChanner();

	ZGuardMtxR guard(fMtxR);

	fTrueOnce_WriteNeedsStart.Reset();

	if (not theChannerW)
		{
		// No Channer was available
		return false;
		}

	Map_Any theMessage;

	if (not sIsEmpty(fPending_Registrations))
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

	if (not sIsEmpty(fPending_Unregistrations))
		{
		Seq_Any& theSeq = theMessage.Mut<Seq_Any>("Unregistrations");
		foreachi (ii, fPending_Unregistrations)
			theSeq.Append(*ii);
		sClear(fPending_Unregistrations);
		}

	if (not sIsEmpty(fPending_Updates))
		{
		Seq_Any& theSeq = theMessage.Mut<Seq_Any>("Updates");
		foreachi (ii, fPending_Updates)
			{
			Map_Any theMap;
			theMap.Set("True", ii->second);
			theMap.Set("Daton", ii->first.GetData());
			theSeq.Append(theMap);
			}
		sClear(fPending_Updates);
		}

	guard.Release();

	if (not theMessage.IsEmpty())
		spWriteMessage(theMessage, sGetChan(theChannerW));

	return true;
	}

ZRef<ChannerRW_Bin> Melange_Client::pEnsureChanner()
	{
	ZGuardMtxR guard(fMtxR);

	while (fGettingChanner)
		fCnd.WaitFor(fMtxR, 5);

	if (not fChanner)
		{
		// Registrations become pending, but writes are discarded -- we'll get replacement
		// values when we reconnect, and our caller can reapply their work, if appropriate.

		foreachi (ii, fMap_Reg2Refcon)
			sInsertMust(fPending_Registrations, ii->first);

		sClear(fPending_Unregistrations);
		sClear(fPending_Updates);
		sClear(fMap_Refcon2Reg);
		sClear(fMap_Reg2Refcon);
		sClear(fQueue_Read);

		this->pWake();

		SaveSetRestore<bool> theSSR(fGettingChanner, true);

		guard.Release();

    if (ZLOGF(w, eDebug))
      w << "No Channer";

    sCall(fCallable_Status, false);

		ZRef<ChannerRW_Bin> theChanner = sCall(fFactory);
		if (not theChanner)
			{
			// No Channer was available, pause for 1s;
      if (ZLOGF(w, eDebug))
        w << "Still no Channer";
			ZThread::sSleep(1);
			}
    else
      {
      if (ZLOGF(w, eDebug))
        w << "Has Channer";

      sCall(fCallable_Status, true);
      }

		guard.Acquire();

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

ZRef<Dataspace::Expr_Rel> sTestRel(const ZRef<ChannerRU_UTF>& iChannerRU);
ZRef<Dataspace::Expr_Rel> sTestRel(const ZRef<ChannerRU_UTF>& iChannerRU)
	{
	using RelationalAlgebra::Expr_Rel;

	ZRef<Expr_Rel> theRel = RelationalAlgebra::Util_Strim_Rel::sQFromStrim(
		sGetChan<ChanR_UTF>(iChannerRU), sGetChan<ChanU_UTF>(iChannerRU));

	StdIO::sChan_UTF_Out << "\n" << theRel << "\n";

//	theRel = RelationalAlgebra::Transform_DecomposeRestricts().Do(theRel);
//	StdIO::sChan_UTF_Out << "\n" << theRel << "\n";

// This next one needs some work doing with Embeds
//	theRel = RelationalAlgebra::Transform_PushDownRestricts().Do(theRel);
//	StdIO::sChan_UTF_Out << "\n" << theRel << "\n";

//	theRel = RelationalAlgebra::sTransform_ConsolidateRenames(theRel);
//	StdIO::sChan_UTF_Out << "\n" << theRel << "\n";

	theRel = QueryEngine::sTransform_Search(theRel);
	StdIO::sChan_UTF_Out << "\n" << theRel << "\n";

	return theRel;
	}

//ZRef<RelationalAlgebra::Expr_Rel> sTestRel(const std::string& iString)
//	{
//	ChanRU_UTF_string8 theChan(iString);
//	return RelationalAlgebra::Util_Strim_Rel::sQFromStrim(theChan, theChan);
//	}

} // namespace ZooLib
