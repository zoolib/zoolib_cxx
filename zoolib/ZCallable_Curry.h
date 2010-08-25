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

#ifndef __ZCallable_Curry__
#define __ZCallable_Curry__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZRef.h"

namespace ZooLib {
namespace ZCallable_Curry {

// =================================================================================================
#pragma mark -
#pragma mark * Traits

template <class T> struct Traits
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

template <class T> struct Traits<const T&>
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

template <class T> struct Traits<T&>
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable0

template <class R, class Bound>
class Callable0
:	public ZCallable0<R>
	{
public:
	typedef ZCallable1<R,Bound> Callable_t;

	Callable0(ZRef<Callable_t> iCallable, typename Traits<Bound>::ConstRef_Add_t iBound)
	:	fCallable(iCallable)
	,	fBound(iBound)
		{}

	virtual R Call()
		{ return fCallable->Call(fBound); }

private:
	ZRef<Callable_t> fCallable;
	typename Traits<Bound>::ConstRef_Remove_t fBound;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable1

template <class R, class P0, class Bound>
class Callable1
:	public ZCallable1<R,P0>
	{
public:
	typedef ZCallable2<R,P0,Bound> Callable_t;

	Callable1(ZRef<Callable_t> iCallable, typename Traits<Bound>::ConstRef_Add_t iBound)
	:	fCallable(iCallable)
	,	fBound(iBound)
		{}

	virtual R Call(P0 i0)
		{ return fCallable->Call(i0, fBound); }

private:
	ZRef<Callable_t> fCallable;
	typename Traits<Bound>::ConstRef_Remove_t fBound;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable2

template <class R, class P0, class P1, class Bound>
class Callable2
:	public ZCallable2<R,P0,P1>
	{
public:
	typedef ZCallable3<R,P0,P1,Bound> Callable_t;

	Callable2(ZRef<Callable_t> iCallable, typename Traits<Bound>::ConstRef_Add_t iBound)
	:	fCallable(iCallable)
	,	fBound(iBound)
		{}

	virtual R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, fBound); }

private:
	ZRef<Callable_t> fCallable;
	typename Traits<Bound>::ConstRef_Remove_t fBound;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable3

template <class R, class P0, class P1, class P2, class Bound>
class Callable3
:	public ZCallable3<R,P0,P1,P2>
	{
public:
	typedef ZCallable4<R,P0,P1,P2,Bound> Callable_t;

	Callable3(ZRef<Callable_t> iCallable, typename Traits<Bound>::ConstRef_Add_t iBound)
	:	fCallable(iCallable)
	,	fBound(iBound)
		{}

	virtual R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, fBound); }

private:
	ZRef<Callable_t> fCallable;
	typename Traits<Bound>::ConstRef_Remove_t fBound;
	};

} // namespace ZCallable_Curry

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

template <class R, class Bound>
ZRef<ZCallable0<R> >
Curry(
	const ZRef<ZCallable1<R,Bound> >& iCallable,
	typename ZCallable_Curry::Traits<Bound>::ConstRef_Add_t iBound)
	{ return new ZCallable_Curry::Callable0<R,Bound>(iCallable, iBound); }

template <class R, class P0, class Bound>
ZRef<ZCallable1<R,P0> >
Curry(
	const ZRef<ZCallable2<R,P0,Bound> >& iCallable,
	typename ZCallable_Curry::Traits<Bound>::ConstRef_Add_t iBound)
	{ return new ZCallable_Curry::Callable1<R,P0,Bound>(iCallable, iBound); }

template <class R, class P0, class P1, class Bound>
ZRef<ZCallable2<R,P0,P1> >
Curry(
	const ZRef<ZCallable3<R,P0,P1,Bound> >& iCallable,
	typename ZCallable_Curry::Traits<Bound>::ConstRef_Add_t iBound)
	{ return new ZCallable_Curry::Callable2<R,P0,P1,Bound>(iCallable, iBound); }

template <class R, class P0, class P1, class P2, class Bound>
ZRef<ZCallable3<R,P0,P1,P2> >
Curry(
	const ZRef<ZCallable4<R,P0,P1,P2, Bound> >& iCallable,
	typename ZCallable_Curry::Traits<Bound>::ConstRef_Add_t iBound)
	{ return new ZCallable_Curry::Callable3<R,P0,P1,P2, Bound>(iCallable, iBound); }

} // namespace ZooLib

#endif // __ZCallable_Curry__
