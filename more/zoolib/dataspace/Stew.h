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

#ifndef __ZooLib_Dataspace_Stew_h__
#define __ZooLib_Dataspace_Stew_h__ 1
#include "zconfig.h"

#include "zoolib/ZCaller.h"
#include "zoolib/ZCallScheduler.h"
#include "zoolib/ZWorker.h"

#include "zoolib/dataspace/Relater.h"
#include "zoolib/dataspace/Searcher_DatonSet.h"
#include "zoolib/dataspace/WrappedDatonSet.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
// MARK: - Stew

class Stew
:	public ZCounted
	{
public:
	typedef RelationalAlgebra::Expr_Rel Expr_Rel;

	class Registration;

	Stew(ZRef<WrappedDatonSet> iWDS_Parent);

// From ZCounted
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	void Start(ZRef<ZCaller> iCaller);
	void Stop();
	void StopWait();

	typedef ZCallable<void(
			const ZRef<Registration>& iRegistration,
			const ZRef<QueryEngine::Result>& iResult,
			bool iIsFirst)>
		Callable_RegistrationChanged;

	ZRef<Registration> Register(
		const ZRef<Callable_RegistrationChanged>& iCallable,
		const ZRef<Expr_Rel>& iRel);

	ZRef<WrappedDatonSet> GetWrappedDatonSet();

	ZRef<ZCaller> GetCaller();

	ZRef<ZWorker> GetWorker();

private:
	class Registration_Real;

	bool pWork(ZRef<ZWorker> iWorker);
	void pDetached(ZRef<ZWorker> iWorker);

	void pUpdateNeeded();

	bool pTriggerEventLoop();

	void pCallback_Relater(ZRef<Dataspace::Relater> iRelater);
	void pFinalize(Registration_Real* iRegistration);

	// ---

	ZRef<WrappedDatonSet> fWDS_Parent;
	ZRef<WrappedDatonSet> fWDS;

	class Caller_EventLoop_Callable;
	ZRef<Caller_EventLoop_Callable> fCaller;

	ZRef<Dataspace::Relater> fRelater;

	ZMtxR fMtxR;
	ZCnd fCnd;

	bool fKeepWorking;
	ZRef<ZWorker> fWorker;

	int64 fNextRefcon;

	typedef std::map<int64, Registration_Real*> Map_RefconToRegistration;
	Map_RefconToRegistration fMap_RefconToRegistration;

	std::set<Registration_Real*> fToAdd;
	std::set<int64> fToRemove;
	};

// =================================================================================================
// MARK: - Stew::Registration

class Stew::Registration : public ZCounted
	{};

// =================================================================================================
// MARK: - StewFactory

typedef ZCallable<ZRef<Stew>()> StewFactory;

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Stew_h__
