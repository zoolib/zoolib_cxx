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
#include "zoolib/Starter_EventLoopBase.h"

#include "zoolib/Dataspace/Melange.h"

namespace ZooLib {
namespace Dataspace {

// ================================================================================================
// MARK: - MelangeServer

class MelangeServer
:	public ZCounted
	{
public:
	MelangeServer(const Melange_t& iMelange);

// From ZCounted
	virtual void Initialize();

// Our protocol
	void Run(const ChannerComboRW_Bin& iChannerComboRW);

private:
	void pChanged(
		const ZRef<ZCounted>& iRegistration,
		const ZRef<Result>& iResult,
		bool iIsFirst);
	ZRef<RelsWatcher::Callable_Changed> fCallable_Changed;

	void pRead();
	static void spRead(ZRef<MelangeServer> iMS);

	const Melange_t fMelange;

	std::map<int64,ZRef<ZCounted> > fMap_Registrations;
	};

// =================================================================================================
// MARK: - Melange_Client

class Melange_Client
:	public Callable_Register
,	public Callable_DatonSetUpdate
,	public Starter_EventLoopBase
	{
	Melange_Client(const ZRef<ChannerComboFactoryRW_Bin>& iChannerComboFactory);

// From Callable via Callable_Register
	virtual ZQ<ZRef<ZCounted> > QCall(
		const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
		const ZRef<Expr_Rel>& iRel);

// From Callable via Callable_DatonSetUpdate
	virtual ZQ<void> QCall(const DatonSet::Daton& iDaton, bool iTrue);

// From Starter_EventLoopBase
	virtual bool pTrigger();

// Our protocol
	void Start(ZRef<Starter> iStarter);

private:
	void pRead();
	static void spRead(ZRef<Melange_Client> iMC);

	ZQ<ChannerComboRW_Bin> pQEnsureChannerCombo();

	const ZRef<ChannerComboFactoryRW_Bin> fChannerComboFactory;

	ZMtxR fMtxR;
	ZCnd fCnd;

	ZQ<ChannerComboRW_Bin> fChannerComboQ;
	ZRef<Starter> fStarter;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_MelangeRemoter_h__
