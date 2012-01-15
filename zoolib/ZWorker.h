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

#ifndef __ZWorker_h__
#define __ZWorker_h__ 1
#include "zconfig.h"

#include "zoolib/ZCaller.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTime.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZWorker

class ZWorker
:	public ZCallable_Void
	{
public:
	typedef ZRef<ZWorker> ZRef_ZWorker; // CW7 workaround

	typedef ZCallable<void(ZRef_ZWorker)> Callable_Attached;
	typedef ZCallable<bool(ZRef_ZWorker)> Callable_Work;
	typedef ZCallable<void(ZRef_ZWorker)> Callable_Detached;

	ZWorker
		(const ZRef<Callable_Attached>& iCallable_Attached,
		const ZRef<Callable_Work>& iCallable_Work,
		const ZRef<Callable_Detached>& iCallable_Detached);

	ZWorker
		(const ZRef<Callable_Attached>& iCallable_Attached,
		const ZRef<Callable_Work>& iCallable_Work);

	ZWorker
		(const ZRef<Callable_Work>& iCallable_Work,
		const ZRef<Callable_Detached>& iCallable_Detached);

	ZWorker(const ZRef<Callable_Work>& iCallable_Work);

	ZWorker();

// From ZCallable_Void
	ZQ<void> QCall();

// Our protocol
	void Wake();
	void WakeAt(ZTime iSystemTime);
	void WakeIn(double iInterval);
	bool IsAwake();

	bool IsWorking();

	bool Attach(ZRef<ZCaller> iCaller);
	bool IsAttached();

private:
	void pWakeAt(ZTime iSystemTime);

	ZMtx fMtx;
	ZRef<ZCaller> fCaller;
	ZThread::ID fWorking;
	ZTime fNextWake;

	const ZRef<Callable_Attached> fCallable_Attached;
	const ZRef<Callable_Work> fCallable_Work;
	const ZRef<Callable_Detached> fCallable_Detached;
	};

} // namespace ZooLib

#endif // __ZWorker_h__
