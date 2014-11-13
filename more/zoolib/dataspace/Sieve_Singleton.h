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

#ifndef __ZooLib_Dataspace_Sieve_Singleton_h__
#define __ZooLib_Dataspace_Sieve_Singleton_h__ 1
#include "zconfig.h"

#include "zoolib/dataspace/Daton_Val.h"
#include "zooLib/dataspace/RelsWatcher_Relater.h"
#include "zoolib/dataspace/Searcher_DatonSet.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
// MARK: - Sieve_Singleton

class Sieve_Singleton
:	public ZCounted
	{
public:
	typedef Callable<void(ZRef<Sieve_Singleton> iSieve, bool)> Callable_Changed;
	typedef Callable_Changed::Signature Signature;

	Sieve_Singleton(ZRef<RelsWatcher::Callable_Register> iCallable_Register,
		const ZRef<Callable_DatonSetUpdate>& iCallable_DatonSetUpdate,
		const ZRef<Callable_Changed>& iCallable_Changed,
		const ZRef<Expr_Rel>& iRel,
		const ColName& iDatonColName,
		const RelHead& iEditableRelHead);

// From ZCounted
	virtual void Initialize();

// Our protocol
	void Activate();
	void Deactivate();

	bool IsLoaded();

	ZMap_Any GetMap();
	ZVal_Any Get(const string8& iName);

	void Set(const string8& iName, const ZVal_Any& iVal);
	void Set(const ZMap_Any& iMap);

private:
	void pChanged(const ZRef<ZCounted>& iRegistration,
		const ZRef<Event>& iEvent,
		const ZRef<QueryEngine::Result>& iResult,
		bool iIsFirst);

	ZRef<RelsWatcher::Callable_Register> const fCallable_Register;
	ZRef<Callable_DatonSetUpdate> const fCallable_DatonSetUpdate;
	ZRef<Callable_Changed> const fCallable_Changed;
	ZRef<Expr_Rel> const fRel;
	ColName const fDatonColName;
	RelHead const fEditableRelHead;

	ZRef<ZCounted> fRegistration;
	ZRef<QueryEngine::Result> fResult;

	ZRef<Event> fEvent;

	ZQ<ZMap_Any> fMapQ;
	ZMap_Any fMapInDaton;
	DatonSet::Daton fDaton;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Sieve_Singleton_h__
