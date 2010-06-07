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

#ifndef __ZWorker__
#define __ZWorker__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZWeakRef.h"

namespace ZooLib {

class ZWorkerRunner;

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker

class ZWorker
:	public ZCounted
	{
public:
	virtual void RunnerAttached();
	virtual void RunnerDetached();

	virtual bool Work() = 0;

	void Wake();
	void WakeAt(ZTime iSystemTime);
	void WakeIn(double iInterval);

	bool IsAwake();
	bool IsAttached();

private:
	ZWeakRef<ZWorkerRunner> fRunner;
	friend class ZWorkerRunner;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner

class ZWorkerRunner
:	public ZCounted
,	public ZWeakReferee
	{
protected:
// Called by subclasses
	void pAttachWorker(ZRef<ZWorker> iWorker);
	void pDetachWorker(ZRef<ZWorker> iWorker);

// Called by ZWorker instances.
	virtual void Wake(ZRef<ZWorker> iWorker) = 0;
	virtual void WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime) = 0;
	virtual void WakeIn(ZRef<ZWorker> iWorker, double iInterval) = 0;
	virtual bool IsAwake(ZRef<ZWorker> iWorker) = 0;

	friend class ZWorker;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

void sStartWorkerRunner(ZRef<ZWorker> iWorker);

} // namespace ZooLib

#endif // __ZWorker__
