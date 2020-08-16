// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Sieve_Singleton_h__
#define __ZooLib_Dataspace_Sieve_Singleton_h__ 1
#include "zconfig.h"

#include "zoolib/Val_ZZ.h"

#include "zoolib/Dataspace/Daton_Val.h"
#include "zoolib/Dataspace/Melange.h" // For Callable_DatonUpdate et al
#include "zoolib/Dataspace/ResultHandler.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - Sieve_Singleton

class Sieve_Singleton
:	public Counted
	{
public:
	typedef Callable<void(ZP<Sieve_Singleton> iSieve, bool iWasLoad)> Callable_Changed;
	typedef Callable_Changed::Signature Signature;

	Sieve_Singleton(ZP<Callable_Register> iCallable_Register,
		const ZP<Callable_DatonUpdate>& iCallable_DatonUpdate,
		const ZP<Callable_Changed>& iCallable_Changed,
		const ZP<Expr_Rel>& iRel,
		const ColName& iDatonColName,
		const RelHead& iEditableRelHead);

// From Counted
	virtual void Initialize();

// Our protocol
	void Activate();
	void Deactivate();

	ZQ<bool> QExists();

	bool IsLoadedAndExists();

	bool IsLoaded();

	ZQ<Map_ZZ> QGetMap();
	Map_ZZ GetMap();
	Val_ZZ Get(const string8& iName);

	void Set(const string8& iName, const Val_ZZ& iVal);
	void Set(const Map_ZZ& iMap);

private:
	void pChanged(const ZP<Counted>& iRegistration, const ZP<Result>& iResult);

	ZP<RelsWatcher::Callable_Register> const fCallable_Register;
	ZP<Callable_DatonUpdate> const fCallable_DatonUpdate;
	ZP<Callable_Changed> const fCallable_Changed;
	ZP<Expr_Rel> const fRel;
	ColName const fDatonColName;
	RelHead const fEditableRelHead;

	ZP<Counted> fRegistration;
	ZP<ResultHandler> fResultHandler;
	ZP<QueryEngine::Result> fResult;

	ZQ<Map_ZZ> fMapQ;
	Map_ZZ fMapInDaton;
	Daton fDaton;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Sieve_Singleton_h__
