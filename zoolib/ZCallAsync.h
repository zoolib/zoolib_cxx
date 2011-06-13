/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZCallAsync__
#define __ZCallAsync__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZFuture.h"
#include "zoolib/ZWorker.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Async

template <class T>
class ZWorker_Async
:	public ZWorker
	{
public:
	typedef ZCallable<T()> Callable;

	ZWorker_Async(ZRef<ZPromise<T> > iPromise, ZRef<Callable> iCallable)
	:	fPromise(iPromise)
	,	fCallable(iCallable)
		{}

	virtual ~ZWorker_Async()
		{}

// From ZWorker
	virtual void RunnerDetached()
		{ fPromise->Set(fCallable->Call()); }

	virtual bool Work()
		{ return false; }

private:
	ZRef<ZPromise<T> > fPromise;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Async<void>

template <>
class ZWorker_Async<void>
:	public ZWorker
	{
public:
	typedef ZCallable<void()> Callable;

	ZWorker_Async(ZRef<ZPromise<void> > iPromise, ZRef<Callable> iCallable)
	:	fPromise(iPromise)
	,	fCallable(iCallable)
		{}

	virtual ~ZWorker_Async()
		{}

// From ZWorker
	virtual void RunnerDetached()
		{
		fCallable->Call();
		fPromise->Set();
		}

	virtual bool Work()
		{ return false; }

private:
	ZRef<ZPromise<void> > fPromise;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallAsync

inline
ZRef<ZFuture<void> > CallAsync(ZRef<ZCallable<void()> > iCallable)
	{
	ZRef<ZPromise<void> > thePromise = new ZPromise<void>;
	sStartWorkerRunner(new ZWorker_Async<void>(thePromise, iCallable));
	return thePromise->Get();
	}

template <class T>
ZRef<ZFuture<T> > CallAsync(ZRef<ZCallable<T()> > iCallable)
	{
	ZRef<ZPromise<T> > thePromise = new ZPromise<T>;
	sStartWorkerRunner(new ZWorker_Async<T>(thePromise, iCallable));
	return thePromise->Get();
	}

} // namespace ZooLib

#endif // __ZWorker_Callable__
