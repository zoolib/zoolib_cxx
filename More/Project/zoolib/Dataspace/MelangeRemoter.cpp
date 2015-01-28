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
#include "zoolib/Stringf.h"
#include "zoolib/Util_Any.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/dataspace/MelangeRemoter.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_Any_JSON.h"
#include "zoolib/ZVal_Any.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_JSONB.h"

namespace ZooLib {
namespace Dataspace {

using namespace Util_STL;

static void spSendMessage(const ZMap_Any& iMessage, const ChanW_Bin& iChanW)
	{
	const ZTime start = ZTime::sSystem();
	ZYad_JSONB::sToChan(sYadR(iMessage), iChanW);
//	ZUtil_Any_JSON::sWrite(iMessage, ZStrimW_StreamUTF8(iChanW));
	sFlush(iChanW);
	if (ZLOGF(w, eDebug+1))
		{
		w << "Sent in " << sStringf("%.3gms: ", (ZTime::sSystem() - start) * 1e3);
//		w << iMessage.Get<string8>("What");
		ZUtil_Any_JSON::sWrite(iMessage, w);
		}
	}

static ZMap_Any spReadMessage(const ZRef<ChannerR_Bin>& iChannerR)
	{
	ZRef<ZStreamerR> theSR = iChannerR.DynamicCast<ZStreamerR>();
	const ZTime start = ZTime::sSystem();
	ZQ<ZVal_Any> theQ = ZYad_Any::sQFromYadR(ZYad_JSONB::sYadR(theSR));
	if (not theQ)
		sThrow_ExhaustedR();

	const ZMap_Any result = theQ->Get<ZMap_Any>();
	if (ZLOGF(w, eDebug+1))
		{
		w << "Received in " << sStringf("%.3gms: ", (ZTime::sSystem() - start) * 1e3);
//		w << result.Get<string8>("What");
		ZUtil_Any_JSON::sWrite(result, w);
		}
	return result;
	}

ZRef<Expr_Rel> spAsRel(const ZVal_Any& iVal);

// =================================================================================================
#pragma mark -
#pragma mark MelangeServer

MelangeServer::MelangeServer(const Melange_t& iMelange)
:	fMelange(iMelange)
	{
	}

void MelangeServer::Initialize()
	{
	ZCounted::Initialize();

	fCallable_Changed = sCallable(sWeakRef(this), &MelangeServer::pChanged);

	ZThread::sStart_T<ZRef<MelangeServer> >(&MelangeServer::spRead, this);
	}

void MelangeServer::Run(const ChannerComboRW_Bin& iChannerComboRW)
	{
	for (;;)
		{
		const ZSeq_Any theMessages = spReadMessage(iChannerComboRW.GetR()).Get<ZSeq_Any>("Messages");
		foreachi (ii, theMessages)
			{
			const ZMap_Any theMessage = ii->Get<ZMap_Any>();

			const string8& theWhat = theMessage.Get<string8>("What");

			if (false)
				{}
			else if (theWhat == "Register")
				{
				foreachi (ii, theMessage.Get<ZSeq_Any>("Registrations"))
					{
					if (ZQ<ZMap_Any> theMapQ = ii->Get<ZMap_Any>())
						{
						if (ZQ<int64> theRefconQ = sQCoerceInt(theMapQ->Get("Refcon")))
							{
							if (ZRef<Expr_Rel> theRel = spAsRel(theMapQ->Get("Rel")))
								{
								ZRef<ZCounted> theRegistration =
									sCall(fMelange.f0, fCallable_Changed, theRel);
								sInsertMust(fMap_Registrations, *theRefconQ, theRegistration);
								}
							}
						}
					}
				}
			else if (theWhat == "Unregister")
				{
				foreachi (ii, theMessage.Get<ZSeq_Any>("Registrations"))
					{
					if (ZQ<int64> theRefconQ = sQCoerceInt(*ii))
						sEraseMust(fMap_Registrations, *theRefconQ);
					}
				}
			else if (theWhat == "Update")
				{
				foreachi (ii, theMessage.Get<ZSeq_Any>("Updates"))
					{
					if (ZQ<ZMap_Any> theMapQ = ii->Get<ZMap_Any>())
						{
						if (ZQ<bool> theBoolQ = sQCoerceInt(theMapQ->Get("True")))
							{
							if (ZQ<ZData_Any> theDataQ = theMapQ->QGet<ZData_Any>("Daton"))
								sCall(fMelange.f1, *theDataQ, *theBoolQ);
							}
						}
					}
				}

			// Read daton writes
			// Read registration requests
	//		Qeuue them up for application in the writer thread.
	//		Push them across to be invoked by the starter
			}
		}
	}

void MelangeServer::pRead()
	{
	for (;;)
		{
		// Read daton writes
		// Read registration requests
//		Qeuue them up for application in the writer thread.
//		Push them across to be invoked by the starter
		}
	}

void MelangeServer::spRead(ZRef<MelangeServer> iMS)
	{ iMS->pRead(); }

// =================================================================================================
#pragma mark -
#pragma mark Melange_Client

Melange_Client::Melange_Client(const ZRef<ChannerComboFactoryRW_Bin>& iChannerComboFactory)
:	fChannerComboFactory(iChannerComboFactory)
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
