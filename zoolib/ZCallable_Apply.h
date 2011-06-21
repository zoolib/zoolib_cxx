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

#ifndef __ZCallable_Apply__
#define __ZCallable_Apply__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Apply

template <class A, class B, class C>
class ZCallable_Apply
:	public ZCallable<A(C)>
	{
public:
	ZCallable_Apply(const ZRef<ZCallable<A(B)> >& iApply, const ZRef<ZCallable<B(C)> >& iCallable)
	:	fApply(iApply)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual A Call(C iC)
		{ return fApply->Call(fCallable->Call(iC)); }

private:
	const ZRef<ZCallable<A(B)> > fApply;
	const ZRef<ZCallable<B(C)> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Apply

template <class A, class B, class C>
ZRef<ZCallable<A(C)> >
MakeCallable_Apply(const ZRef<ZCallable<A(B)> >& iApply, const ZRef<ZCallable<B(C)> >& iCallable)
	{ return new ZCallable_Apply<A,B,C>(iApply, iCallable); }

template <class A, class B, class C>
ZRef<ZCallable<A(C)> >
Apply(const ZRef<ZCallable<A(B)> >& iApply, const ZRef<ZCallable<B(C)> >& iCallable)
	{ return new ZCallable_Apply<A,B,C>(iApply, iCallable); }

} // namespace ZooLib

#endif // __ZCallable_Apply__
