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

namespace ZCallable_Function {

// =================================================================================================
#pragma mark -
#pragma mark * Callable0

template <class R>
class Callable0
:	public ZCallable<R(void)>
	{
public:
	typedef R (*Function_t)();

	Callable0(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable
	virtual R Call()
		{ return fFunction(); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable1

template <class R, class P0>
class Callable1
:	public ZCallable<R(P0)>
	{
public:
	typedef R (*Function_t)(P0);

	Callable1(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable
	virtual R Call(P0 i0)
		{ return fFunction(i0); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable2

template <class R, class P0, class P1>
class Callable2
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef R (*Function_t)(P0, P1);

	Callable2(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{ return fFunction(i0, i1); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable3

template <class R, class P0, class P1, class P2>
class Callable3
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef R (*Function_t)(P0, P1, P2);

	Callable3(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{ return fFunction(i0, i1, i2); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable4

template <class R, class P0, class P1, class P2, class P3>
class Callable4
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef R (*Function_t)(P0, P1, P2, P3);

	Callable4(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fFunction(i0, i1, i2, i3); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable5

template <class R, class P0, class P1, class P2, class P3, class P4>
class Callable5
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef R (*Function_t)(P0, P1, P2, P3,P4);

	Callable5(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fFunction(i0, i1, i2, i3, i4); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallablle

template <class R>
ZRef<ZCallable<R(void)> >
MakeCallable(R (*iFunction)())
	{ return new Callable0<R>(iFunction); }

template <class R, class P0>
ZRef<ZCallable<R(P0)> >
MakeCallable(R (*iFunction)(P0))
	{ return new Callable1<R,P0>(iFunction); }

template <class R, class P0, class P1>
ZRef<ZCallable<R(P0,P1)> >
MakeCallable(R (*iFunction)(P0,P1))
	{ return new Callable2<R,P0,P1>(iFunction); }

template <class R, class P0, class P1, class P2>
ZRef<ZCallable<R(P0,P1,P2)> >
MakeCallable(R (*iFunction)(P0,P1,P2))
	{ return new Callable3<R,P0,P1,P2>(iFunction); }

template <class R, class P0, class P1, class P2, class P3>
ZRef<ZCallable<R(P0,P1,P2,P3)> >
MakeCallable(R (*iFunction)(P0,P1,P2,P3))
	{ return new Callable4<R,P0,P1,P2,P3>(iFunction); }

template <class R, class P0, class P1, class P2, class P3, class P4>
ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >
MakeCallable(R (*iFunction)(P0,P1,P2,P3,P4))
	{ return new Callable5<R,P0,P1,P2,P3,P4>(iFunction); }

} // namespace ZCallable_Function

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

using ZCallable_Function::MakeCallable;

} // namespace ZooLib

#endif // __ZCallable_Function__
