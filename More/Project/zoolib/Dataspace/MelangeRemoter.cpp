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
#include "zoolib/Log.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_Any.h"
#include "zoolib/Util_Any_JSONB.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/dataspace/MelangeRemoter.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace Dataspace {

using namespace Util_STL;

// =================================================================================================
#pragma mark -
#pragma mark

static void spWriteMessage(const Map_Any& iMessage, const ChanW_Bin& iChanW)
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

// namespace { // anonymous

ZRef<Expr_Rel> spAsRel(const Val_Any& iVal);

static Val_Any spAsVal(ZRef<Expr_Rel> iRel);

static ZRef<Result> spAsResult(const Val_Any& iVal);

static Val_Any spAsVal(ZRef<Result> iResult);

// } // anonymous namespace

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

Melange_Client::Melange_Client(const ZRef<Factory_ChannerRW_Bin>& iFactory)
:	fFactory(iFactory)
,	fNextRefcon(1)
	{}

// From Callable_Register
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

// From Callable_DatonSetUpdate
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

// From Starter_EventLoopBase
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
			if (ZQ<bool> theIsFirst = sQCoerceInt(theMessage.Get("IsFirst")))
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
	for (;;)
		{
		try
			{
			ZRef<ChannerRW_Bin> theChanner = this->pEnsureChanner();

			ZGuardMtxR guard(fMtxR);

			if (not theChanner)
				{
				// No Channer was available
				continue;
				}

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
	fTrueOnce_WriteNeedsStart.Reset();

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

	// The write failed in some fashion, clean up and trigger pWork.

	ZGuardMtxR guard(fMtxR);

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
	}

bool Melange_Client::pWrite_Inner()
	{
	ZRef<ChannerW_Bin> theChannerW = this->pEnsureChanner();

	ZGuardMtxR guard(fMtxR);

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

	if (not fChanner)
		{
		guard.Release();
		ZRef<ChannerRW_Bin> theChanner = sCall(fFactory);
		guard.Acquire();
		fChanner = theChanner;
		}
	return fChanner;
	}

void Melange_Client::pFinalize(Registration* iRegistration)
	{
	ZGuardMtxR guard(fMtxR);
	if (not iRegistration->FinishFinalize())
		return;

	if (not sQErase(fPending_Registrations, iRegistration))
		{
		// This wasn't a pending registration, so we'll have assigned it a refcon, need to
		// clear it from our maps, and to tell the server.
		const int64 theRefcon =
			sGetEraseMust(fMap_Reg2Refcon, iRegistration);

		sEraseMust(fMap_Refcon2Reg, theRefcon);
		sInsert(fPending_Unregistrations, theRefcon);

		this->pWake();
		}

	delete iRegistration;
	}

} // namespace Dataspace
} // namespace ZooLib
