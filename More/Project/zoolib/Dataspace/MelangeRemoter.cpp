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

ZRef<Expr_Rel> spAsRel(const Val_Any& iVal);

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
	for (;;)
		{
		if (not fChannerR)
			break;

		ZRef<ChannerR_Bin> theChannerR = fChannerR;

		guard.Release();

		const Map_Any theMap = spReadMessage(theChannerR);

		guard.Acquire();
		fQueue_Read.push_back(theMap);
		sNextStartIn(0, fJob);
		}
	}

void MelangeServer::pWrite()
	{
	ZGuardMtxR guard(fMtxR);
	for (;;)
		{
		if (not fChannerW)
			break;

		if (sIsEmpty(fQueue_ToWrite))
			{
			fCnd.WaitFor(fMtxR, 1);
			if (sIsEmpty(fQueue_ToWrite))
				break;
			}

		if (not fChannerW)
			break;

		ZRef<ChannerW_Bin> theChannerW = fChannerW;

		vector<Map_Any> toWrite;
		swap(toWrite, fQueue_ToWrite);

		guard.Release();

		vector<Map_Any> theMessages;
		swap(fQueue_ToWrite, theMessages);
		guard.Release();

		foreachi (ii, theMessages)
			spWriteMessage(*ii, sGetChan(theChannerW));
		}
	fFalseOnce_WriteRunning.Reset();
	}

void MelangeServer::pWork()
	{
	ZGuardMtxR guard(fMtxR);

	// Pull stuff from fQueue_Read
	vector<Map_Any> theMessages;
	swap(fQueue_Read, theMessages);

	// Act on messages
	foreachv (Map_Any theMessage, theMessages)
		{
		const string8& theWhat = theMessage.Get<string8>("What");

		if (false)
			{}
		else if (theWhat == "Register")
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
			}
		else if (theWhat == "Unregister")
			{
			foreachi (ii, theMessage.Get<Seq_Any>("Registrations"))
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
			}
		else if (theWhat == "Update")
			{
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
		}

	if (sNotEmpty(fQueue_ToWrite))
		{
		if (not fFalseOnce_WriteRunning())
			sStartOnNewThread(sCallable(sRef(this), &MelangeServer::pWrite));
		fCnd.Broadcast();
		}
	}

void MelangeServer::pChanged(
	const ZRef<ZCounted>& iRegistration,
	const ZRef<Result>& iResult,
	bool iIsFirst)
	{
	// We're almost certainly on the same thread that our job will be running on.

	ZGuardMtxR guard(fMtxR);
	// Push onto the to write queue, and wake our starter.
	Map_any theMap;

	const int64 theRefcon = sGetMust(fMap_Reg2Refcon, iRegistration);
	theMap.Set("Refcon", sGetMust(fMap_Reg2Refcon, iRegistration));
	

	}

// =================================================================================================
#pragma mark -
#pragma mark Melange_Client

Melange_Client::Melange_Client(const ZRef<Factory_ChannerRW_Bin>& iFactory)
:	fFactory(iFactory)
	{
	}

ZQ<ZRef<ZCounted> > Melange_Client::QCall(
	const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
	const ZRef<Expr_Rel>& iRel)
	{
	// We're probably on our own starter.
	// Shove it into data structure, and wake the writer.
	// When we lose the connection we have to resend all our registrations. So the basic
	// structure should be one of sending everything that's been added. since the last time
	// we sent anything.
	}

ZQ<void> Melange_Client::QCall(const DatonSet::Daton& iDaton, bool iTrue)
	{
//	Shove it into data structure and wake the writer
//	When we're writing do we wait for an ack?
	}

bool Melange_Client::pTrigger()
	{
	}


void Melange_Client::Start(ZRef<Starter> iStarter)
	{
	ZAssert(not fStarter);
	fStarter = iStarter;

	ZThread::sStart_T<ZRef<Melange_Client> >(&Melange_Client::spRead, this);
	}

void Melange_Client::pRead()
	{
	for (;;)
		{
		
		}
	}

void Melange_Client::spRead(ZRef<Melange_Client> iMC)
	{ iMC->pRead(); }

} // namespace Dataspace
} // namespace ZooLib
