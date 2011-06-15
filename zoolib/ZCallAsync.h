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

#include "zoolib/ZCallable_Bind.h"
#include "zoolib/ZCallable_Function.h"
#include "zoolib/ZFuture.h"
#include "zoolib/ZWorker_Callable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * CallAsync

template <class T>
void sCallAsync_T(ZRef<ZPromise<T> > iPromise, ZRef<ZCallable<T()> > iCallable)
	{ iPromise->Set(iCallable->Call()); }

template <class T>
ZRef<ZFuture<T> > CallAsync(ZRef<ZCallable<T()> > iCallable)
	{
	ZRef<ZPromise<T> > thePromise = new ZPromise<T>;
	sStartWorkerRunner(MakeWorker(BindL(thePromise, iCallable, MakeCallable(sCallAsync_T<T>))));
	return thePromise->Get();
	}

inline
void sCallAsyncVoid(ZRef<ZPromise<void> > iPromise, ZRef<ZCallable<void()> > iCallable)
	{
	iCallable->Call();
	iPromise->Set();
	}

inline
ZRef<ZFuture<void> > CallAsync(ZRef<ZCallable<void()> > iCallable)
	{
	ZRef<ZPromise<void> > thePromise = new ZPromise<void>;
	sStartWorkerRunner(MakeWorker(BindL(thePromise, iCallable, MakeCallable(sCallAsyncVoid))));
	return thePromise->Get();
	}

} // namespace ZooLib

#endif // __ZCallAsync__
