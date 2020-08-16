// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Server_Worker_h__
#define __ZooLib_Server_Worker_h__ 1
#include "zconfig.h"

#include "zoolib/Starter.h"

#include "zoolib/ZThread.h" // For ZThread::ID

namespace ZooLib {

// =================================================================================================
#pragma mark - Worker

class Worker
:	public Callable_Void
	{
public:
	typedef ZP<Worker> ZP_Worker; // CW7 workaround

	typedef Callable<void(ZP_Worker)> Callable_Attached;
	typedef Callable<bool(ZP_Worker)> Callable_Work;
	typedef Callable<void(ZP_Worker)> Callable_Detached;

	Worker(
		const ZP<Callable_Attached>& iCallable_Attached,
		const ZP<Callable_Work>& iCallable_Work,
		const ZP<Callable_Detached>& iCallable_Detached);

	Worker(
		const ZP<Callable_Attached>& iCallable_Attached,
		const ZP<Callable_Work>& iCallable_Work);

	Worker(
		const ZP<Callable_Work>& iCallable_Work,
		const ZP<Callable_Detached>& iCallable_Detached);

	Worker(const ZP<Callable_Work>& iCallable_Work);

	Worker();

// From Callable_Void
	virtual bool QCall();

// Our protocol
	void Wake();
	void WakeAt(double iSystemTime);
	void WakeIn(double iInterval);

	bool IsWorking();

	bool Attach(ZP<Starter> iStarter);
	bool IsAttached();

private:
	void pWakeAt(double iSystemTime);

	ZMtx fMtx;
	ZCnd fCnd;
	ZP<Starter> fStarter;
	ZThread::ID fWorking;
	double fNextWake;

	const ZP<Callable_Attached> fCallable_Attached;
	const ZP<Callable_Work> fCallable_Work;
	const ZP<Callable_Detached> fCallable_Detached;
	};

} // namespace ZooLib

#endif // __ZooLib_Server_Worker_h__
