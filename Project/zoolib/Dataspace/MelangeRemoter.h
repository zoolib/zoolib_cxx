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

#ifndef __ZooLib_Dataspace_MelangeRemoter_h__
#define __ZooLib_Dataspace_MelangeRemoter_h__ 1
#include "zconfig.h"

#include "zoolib/Connection.h"
#include "zoolib/StartScheduler.h"

#include "zoolib/Starter_EventLoopBase.h"
#include "zoolib/ValueOnce.h"

#include "zoolib/Dataspace/Melange.h"

namespace ZooLib {
namespace Dataspace {

using std::vector;

// ================================================================================================
#pragma mark -
#pragma mark MelangeServer

class MelangeServer
:	public ZCounted
	{
public:
	MelangeServer(const Melange_t& iMelange, const ZRef<ChannerRW_Bin>& iChannerRW);

// From ZCounted
	virtual void Initialize();

private:
	typedef ZRef<ZCounted> RefReg;

	void pRead();
	void pWrite();

	void pWake();
	void pWork();
	StartScheduler::Job fJob;

	void pChanged(
		const RefReg& iRegistration,
		const ZRef<Result>& iResult);
	ZRef<RelsWatcher::Callable_Changed> fCallable_Changed;

	const Melange_t fMelange;

	ZMtxR fMtxR;
	ZCnd fCnd;

	const ZRef<ChannerR_Bin> fChannerR;
	const ZRef<ChannerW_Bin> fChannerW;

	vector<Map_Any> fQueue_Read;
	vector<Map_Any> fQueue_ToWrite;
	TrueOnce fTrueOnce_WriteNeedsStart;

	std::map<int64,RefReg> fMap_Refcon2Reg;
//##	std::map<int64,ZRef<Result>> fMap_Refcon2Result;
	std::map<RefReg,int64> fMap_Reg2Refcon;
	};

// =================================================================================================
#pragma mark -
#pragma mark Melange_Client

class Melange_Client
:	public Callable_Register
,	public Callable_DatonUpdate
,	public Starter_EventLoopBase
	{
public:
	typedef Callable<void(bool)> Callable_Status;

	Melange_Client(const ZRef<Factory_ChannerRW_Bin>& iFactory,
		const ZRef<Callable_Status>& iCallable_Status);

// From Callable via Callable_Register
	virtual ZQ<ZRef<ZCounted> > QCall(
		const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
		const ZRef<Expr_Rel>& iRel);

// From Callable via Callable_DatonUpdate
	virtual ZQ<void> QCall(
		const Daton* iAsserted, size_t iAssertedCount,
		const Daton* iRetracted, size_t iRetractedCount);

// From Starter_EventLoopBase
	virtual bool pTrigger();

// Our protocol
	void Start(ZRef<Starter> iStarter);

private:
	class Registration;
	friend class Registration;

	void pRead();
	void pWrite();

	void pWake();
	void pWork();
	StartScheduler::Job fJob;

	ZRef<ChannerR_Bin> pEnsureChannerR();
	ZRef<ChannerW_Bin> pEnsureChannerW();
	ZRef<ChannerRW_Bin> pEnsureChanner();

	void pFinalize(Registration* iRegistration);

	const ZRef<Factory_ChannerRW_Bin> fFactory;
	const ZRef<Callable_Status> fCallable_Status;

	ZMtxR fMtxR;
	ZCnd fCnd;

	bool fGettingChanner;
	ZRef<ChannerRW_Bin> fChanner;

	vector<Map_Any> fQueue_Read;
	vector<Map_Any> fQueue_ToWrite;
	bool fReadSinceWrite;
	TrueOnce fTrueOnce_WriteNeedsStart;

	int64 fNextRefcon;
	std::set<ZRef<Registration> > fPending_Registrations;
	std::set<int64> fPending_Unregistrations;
	std::set<Daton> fPending_Asserts;
	std::set<Daton> fPending_Retracts;

	std::map<int64,Registration*> fMap_Refcon2Reg;
	std::map<Registration*,int64> fMap_Reg2Refcon;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_MelangeRemoter_h__
