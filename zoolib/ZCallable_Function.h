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

#ifndef __ZCallable_Function__
#define __ZCallable_Function__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function0

template <class R>
class ZCallable_Function0
:	public ZCallable0<R>
	{
public:
	typedef R (*Function_t)();

	ZCallable_Function0(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable0
	virtual R Call()
		{ return fFunction(); }

private:
	Function_t fFunction;
	};

template <class R>
ZRef<ZCallable0<R> >
MakeCallable(R (*iFunction)())
	{ return new ZCallable_Function0<R>(iFunction); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function1

template <class R, class P0>
class ZCallable_Function1
:	public ZCallable1<R,P0>
	{
public:
	typedef R (*Function_t)(P0);

	ZCallable_Function1(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable1
	virtual R Call(P0 i0)
		{ return fFunction(i0); }

private:
	Function_t fFunction;
	};

template <class R, class P0>
ZRef<ZCallable1<R,P0> >
MakeCallable(R (*iFunction)(P0))
	{ return new ZCallable_Function1<R,P0>(iFunction); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function2

template <class R, class P0, class P1>
class ZCallable_Function2
:	public ZCallable2<R,P0,P1>
	{
public:
	typedef R (*Function_t)(P0, P1);

	ZCallable_Function2(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable2
	virtual R Call(P0 i0, P1 i1)
		{ return fFunction(i0, i1); }

private:
	Function_t fFunction;
	};

template <class R, class P0, class P1>
ZRef<ZCallable2<R,P0,P1> >
MakeCallable(R (*iFunction)(P0,P1))
	{ return new ZCallable_Function2<R,P0,P1>(iFunction); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function3

template <class R, class P0, class P1, class P2>
class ZCallable_Function3
:	public ZCallable3<R,P0,P1,P2>
	{
public:
	typedef R (*Function_t)(P0, P1, P2);

	ZCallable_Function3(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable3
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{ return fFunction(i0, i1, i2); }

private:
	Function_t fFunction;
	};

template <class R, class P0, class P1, class P2>
ZRef<ZCallable3<R,P0,P1,P2> >
MakeCallable(R (*iFunction)(P0,P1,P2))
	{ return new ZCallable_Function3<R,P0,P1,P2>(iFunction); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Function4

template <class R, class P0, class P1, class P2, class P3>
class ZCallable_Function4
:	public ZCallable4<R,P0,P1,P2,P3>
	{
public:
	typedef R (*Function_t)(P0, P1, P2, P3);

	ZCallable_Function4(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable4
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fFunction(i0, i1, i2, i3); }

private:
	Function_t fFunction;
	};

template <class R, class P0, class P1, class P2, class P3>
ZRef<ZCallable4<R,P0,P1,P2,P3> >
MakeCallable(R (*iFunction)(P0,P1,P2,P3))
	{ return new ZCallable_Function4<R,P0,P1,P2,P3>(iFunction); }

} // namespace ZooLib

#endif // __ZCallable_Function__
