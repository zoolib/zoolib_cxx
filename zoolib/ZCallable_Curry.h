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

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_CurryTraits

template <class T> struct ZCallable_CurryTraits
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

template <class T> struct ZCallable_CurryTraits<const T&>
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

template <class T> struct ZCallable_CurryTraits<T&>
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Curry0

template <class R, class Bound>
class ZCallable_Curry0
:	public ZCallable0<R>
	{
public:
	typedef ZCallable1<R,Bound> Callable_t;

	ZCallable_Curry0(
		ZRef<Callable_t> iCallable,
		typename ZCallable_CurryTraits<Bound>::ConstRef_Add_t iBound)
	:	fCallable(iCallable)
	,	fBound(iBound)
		{}

	virtual R Invoke()
		{ return fCallable->Invoke(fBound); }

private:
	ZRef<Callable_t> fCallable;
	typename ZCallable_CurryTraits<Bound>::ConstRef_Remove_t fBound;
	};

template <class R, class Bound>
ZRef<ZCallable0<R> >
Curry(
	const ZRef<ZCallable1<R,Bound> >& iCallable,
	typename ZCallable_CurryTraits<Bound>::ConstRef_Add_t iBound)
	{ return new ZCallable_Curry0<R,Bound>(iCallable, iBound); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Curry1

template <class R, class P0, class Bound>
class ZCallable_Curry1
:	public ZCallable1<R,P0>
	{
public:
	typedef ZCallable2<R,P0,Bound> Callable_t;

	ZCallable_Curry1(
		ZRef<Callable_t> iCallable,
		typename ZCallable_CurryTraits<Bound>::ConstRef_Add_t iBound)
	:	fCallable(iCallable)
	,	fBound(iBound)
		{}

	virtual R Invoke(P0 i0)
		{ return fCallable->Invoke(i0, fBound); }

private:
	ZRef<Callable_t> fCallable;
	typename ZCallable_CurryTraits<Bound>::ConstRef_Remove_t fBound;
	};

template <class R, class P0, class Bound>
ZRef<ZCallable1<R,P0> >
Curry(
	const ZRef<ZCallable2<R,P0,Bound> >& iCallable,
	typename ZCallable_CurryTraits<Bound>::ConstRef_Add_t iBound)
	{ return new ZCallable_Curry1<R,P0,Bound>(iCallable, iBound); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Curry2

template <class R, class P0, class P1, class Bound>
class ZCallable_Curry2
:	public ZCallable2<R,P0,P1>
	{
public:
	typedef ZCallable3<R,P0,P1,Bound> Callable_t;

	ZCallable_Curry2(
		ZRef<Callable_t> iCallable,
		typename ZCallable_CurryTraits<Bound>::ConstRef_Add_t iBound)
	:	fCallable(iCallable)
	,	fBound(iBound)
		{}

	virtual R Invoke(P0 i0, P1 i1)
		{ return fCallable->Invoke(i0, i1, fBound); }

private:
	ZRef<Callable_t> fCallable;
	typename ZCallable_CurryTraits<Bound>::ConstRef_Remove_t fBound;
	};

template <class R, class P0, class P1, class Bound>
ZRef<ZCallable2<R,P0,P1> >
Curry(
	const ZRef<ZCallable3<R,P0,P1,Bound> >& iCallable,
	typename ZCallable_CurryTraits<Bound>::ConstRef_Add_t iBound)
	{ return new ZCallable_Curry2<R,P0,P1,Bound>(iCallable, iBound); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Curry3

template <class R, class P0, class P1, class P2, class Bound>
class ZCallable_Curry3
:	public ZCallable3<R,P0,P1,P2>
	{
public:
	typedef ZCallable4<R,P0,P1,P2,Bound> Callable_t;

	ZCallable_Curry3(
		ZRef<Callable_t> iCallable,
		typename ZCallable_CurryTraits<Bound>::ConstRef_Add_t iBound)
	:	fCallable(iCallable)
	,	fBound(iBound)
		{}

	virtual R Invoke(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Invoke(i0, i1, i2, fBound); }

private:
	ZRef<Callable_t> fCallable;
	typename ZCallable_CurryTraits<Bound>::ConstRef_Remove_t fBound;
	};

template <class R, class P0, class P1, class P2, class Bound>
ZRef<ZCallable3<R,P0,P1,P2> >
Curry(
	const ZRef<ZCallable4<R,P0,P1,P2, Bound> >& iCallable,
	typename ZCallable_CurryTraits<Bound>::ConstRef_Add_t iBound)
	{ return new ZCallable_Curry3<R,P0,P1,P2, Bound>(iCallable, iBound); }

} // namespace ZooLib

#endif // __ZCallable_Curry__
