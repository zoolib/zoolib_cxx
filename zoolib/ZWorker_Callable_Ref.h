/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZWorker_Callable_Ref__
#define __ZWorker_Callable_Ref__ 1
#include "zconfig.h"

#include "zoolib/ZCallable_Ref.h"
#include "zoolib/ZWorker_Callable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * MakeWorker_Callable_Ref

template <class Callee_t>
ZRef<ZWorker>
MakeWorker_Callable_Ref(
	void (Callee_t::*Method)(),
	ZRef<Callee_t> iCallee)
	{
	ZRef<ZCallable_V0> theCallable =
		new ZCallable_Ref_V0<Callee_t>(Method, iCallee);
	
	return new ZWorker_Callable_Once_T0(theCallable);
	}

template <class Callee_t, class P0>
ZRef<ZWorker>
MakeWorker_Callable_Ref(
	void (Callee_t::*Method)(const P0&),
	ZRef<Callee_t> iCallee,
	const P0& iP0)
	{
	ZRef<ZCallable_V1<const P0&> > theCallable =
		new ZCallable_Ref_V1<Callee_t, const P0&>(Method, iCallee);
	
	return new ZWorker_Callable_Once_T1<P0>(theCallable, iP0);
	}

template <class Callee_t, class P0, class P1>
ZRef<ZWorker>
MakeWorker_Callable_Ref(
	void (Callee_t::*Method)(const P0&, const P1&),
	ZRef<Callee_t> iCallee,
	const P0& iP0, const P1& iP1)
	{
	ZRef<ZCallable_V2<const P0&, const P1&> > theCallable =
		new ZCallable_Ref_V2<Callee_t, const P0&, const P1&>(Method, iCallee);
	
	return new ZWorker_Callable_Once_T2<P0, P1>(theCallable, iP0, iP1);
	}

template <class Callee_t, class P0, class P1, class P2>
ZRef<ZWorker>
MakeWorker_Callable_Ref(
	void (Callee_t::*Method)(const P0&, const P1&, const P2&),
	ZRef<Callee_t> iCallee,
	const P0& iP0, const P1& iP1, const P2& iP2)
	{
	ZRef<ZCallable_V3<const P0&, const P1&, const P2&> > theCallable =
		new ZCallable_Ref_V3<Callee_t, const P0&, const P1&, const P2&>(Method, iCallee);
	
	return new ZWorker_Callable_Once_T3<P0, P1, P2>(theCallable, iP0, iP1, iP2);
	}

} // namespace ZooLib

#endif // __ZWorker_Callable_Ref__
