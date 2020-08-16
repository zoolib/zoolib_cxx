// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_RelsWatcher_Relater_h__
#define __ZooLib_Dataspace_RelsWatcher_Relater_h__ 1
#include "zconfig.h"

#include "zoolib/ValueOnce.h"

#include "zoolib/Dataspace/Relater.h"
#include "zoolib/Dataspace/RelsWatcher.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - RelsWatcher_Relater

class RelsWatcher_Relater
:	public RelsWatcher::Callable_Register
	{
	typedef RelsWatcher::Callable_Register inherited;
public:
	typedef Callable_Void Callable_NeedsUpdate;

	RelsWatcher_Relater(const ZP<Relater>& iRelater,
		const ZP<Callable_NeedsUpdate>& iCallable_NeedsUpdate);

// From Counted via Callable_Register
	virtual void Initialize();
	virtual void Finalize();

// From Callable via Callable_Register
	ZP<Counted> QCall(
		const ZP<RelsWatcher::Callable_Changed>& iCallable_Changed,
		const ZP<Expr_Rel>& iRel);

// Our protocol
	void Update();

private:
	void pCallback_Relater(ZP<Relater> iRelater);

	class Registration;
	void pFinalize(Registration* iRegistration);

	ZMtx fMtx;

	ZP<Relater> fRelater;

	FalseOnce fCalled_NeedsUpdate;
	ZP<Callable_NeedsUpdate> fCallable_NeedsUpdate;

	int64 fNextRefcon;

	typedef std::map<int64, Registration*> Map_RefconToRegistrationX;
	Map_RefconToRegistrationX fMap_RefconToRegistrationX;

	std::set<Registration*> fToAdd;
	std::set<int64> fToRemove;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_RelsWatcher_Relater_h__
