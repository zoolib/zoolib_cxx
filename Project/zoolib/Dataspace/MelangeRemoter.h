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

using std::string;
using std::vector;

// ================================================================================================
#pragma mark - MelangeServer

class MelangeServer
:	public ZCounted
	{
public:
	MelangeServer(const Melange_t& iMelange,
		const ZP<ChannerRW_Bin>& iChannerRW,
		int64 iClientVersion,
		const ZQ<string>& iDescriptionQ);

	virtual ~MelangeServer();

// From ZCounted
	virtual void Initialize();

private:
	typedef ZP<ZCounted> RefReg;

	void pRead();
	void pWrite();

	void pWake();
	void pWork();
	StartScheduler::Job fJob;

	void pChanged(
		const RefReg& iRegistration,
		int64 iChangeCount,
		const ZP<Result>& iResult,
		const ZP<ResultDeltas>& iResultDeltas);
	ZP<RelsWatcher::Callable_Changed> fCallable_Changed;

	const Melange_t fMelange;

	ZMtx fMtx;
	ZCnd fCnd;

	const ZP<ChannerR_Bin> fChannerR;
	const ZP<ChannerW_Bin> fChannerW;
	const int64 fClientVersion;
	const ZQ<string> fDescriptionQ;

	vector<Map_Any> fQueue_Read;
	TrueOnce fTrueOnce_WriteNeedsStart;
	TrueOnce fTrueOnce_SendAnEmptyMessage;

	double fTimeOfLastRead;
	double fTimeOfLastWrite;

	int64 fLastClientChangeCount;

	const double fTimeout;
	const double fConnectionTimeout;

	std::map<int64,RefReg> fMap_Refcon2Reg;
	struct ResultCC
		{
		int64 fCC;
		ZP<Result> fResult;
		ZP<ResultDeltas> fResultDeltas;
		};
	std::map<int64,ResultCC> fMap_Refcon2ResultCC;
	std::map<RefReg,int64> fMap_Reg2Refcon;
	std::set<int64> fSet_NewRefcons;
	};

// =================================================================================================
#pragma mark - Melange_Client

class Melange_Client
:	public Callable_Register
,	public Callable_DatonUpdate
,	public Starter_EventLoopBase
	{
public:
	using Chan_t = DeriveFrom<
		ChanAspect_Abort,
		ChanAspect_Read<byte>,
		ChanAspect_WaitReadable,
		ChanAspect_Write<byte>>;

	using Channer_t = Channer<Chan_t>;

	using Factory_Channer = Factory<ZP<Channer_t>>;

	using ChanForRead = DeriveFrom<
		ChanAspect_Abort,
		ChanAspect_Read<byte>,
		ChanAspect_WaitReadable>;

	using ChannerForRead = Channer<ChanForRead>;

	typedef Callable<void(bool)> Callable_Status;

	Melange_Client(const ZP<Factory_Channer>& iFactory,
		const ZP<Callable_Status>& iCallable_Status);

// From Callable via Callable_Register
	virtual ZQ<ZP<ZCounted> > QCall(
		const ZP<RelsWatcher::Callable_Changed>& iCallable_Changed,
		const ZP<Expr_Rel>& iRel);

// From Callable via Callable_DatonUpdate
	virtual ZQ<int64> QCall(
		const Daton* iAsserted, size_t iAssertedCount,
		const Daton* iRetracted, size_t iRetractedCount);

// From Starter_EventLoopBase
	virtual bool pTrigger();

// Our protocol
	void Start(ZP<Starter> iStarter);

	bool Kill();

private:
	class Registration;
	friend class Registration;

	void pRead();
	void pWrite();

	void pWake();
	void pWork();
	StartScheduler::Job fJob;

	ZP<ChannerForRead> pEnsureChannerR();
	ZP<ChannerW_Bin> pEnsureChannerW();
	ZP<Channer_t> pEnsureChanner();

	void pFinalize(Registration* iRegistration);

	const ZP<Factory_Channer> fFactory;
	const ZP<Callable_Status> fCallable_Status;

	ZMtx fMtx;
	ZCnd fCnd;

	bool fGettingChanner;
	ZP<Channer_t> fChanner;

	vector<Map_Any> fQueue_Read;
	vector<Map_Any> fQueue_ToWrite;
	bool fReadSinceWrite;
	TrueOnce fTrueOnce_WriteNeedsStart;

	int64 fChangeCount;

	int64 fNextRefcon;
	std::set<ZP<Registration> > fPending_Registrations;
	std::set<int64> fPending_Unregistrations;
	std::set<Daton> fPending_Asserts;
	std::set<Daton> fPending_Retracts;

	std::map<int64,Registration*> fMap_Refcon2Reg;
	std::map<Registration*,int64> fMap_Reg2Refcon;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_MelangeRemoter_h__
