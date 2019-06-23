/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZooLib_Worker_h__
#define __ZooLib_Worker_h__ 1
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
	typedef ZP<Worker> ZRef_Worker; // CW7 workaround

	typedef Callable<void(ZRef_Worker)> Callable_Attached;
	typedef Callable<bool(ZRef_Worker)> Callable_Work;
	typedef Callable<void(ZRef_Worker)> Callable_Detached;

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

#endif // __ZooLib_Worker_h__
