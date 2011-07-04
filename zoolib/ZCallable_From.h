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

#ifndef __ZCallable_From__
#define __ZCallable_From__ 1
#include "zconfig.h"

#include "zoolib/ZCallFrom.h"

namespace ZooLib {
namespace ZCallable_From {

// =================================================================================================
#pragma mark -
#pragma mark * Callable

template <class Signature> class Callable;

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params)

template <class R>
class Callable<R(void)>
:	public ZCallable<R(void)>
	{
public:
	typedef R (Signature)();

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call()
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			fCallable);

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params, void return)

template <>
class Callable<void(void)>
:	public ZCallable<void(void)>
	{
public:
	typedef void (Signature)();

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call()
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			fCallable);

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 1 param)

template <class R,
	class P0>
class Callable<R(P0)>
:	public ZCallable<R(P0)>
	{
public:
	typedef R (Signature)(P0);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 1 param, void return)

template <
	class P0>
class Callable<void(P0)>
:	public ZCallable<void(P0)>
	{
public:
	typedef void (Signature)(P0);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call(P0 i0)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 2 params)

template <class R,
	class P0, class P1>
class Callable<R(P0,P1)>
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef R (Signature)(P0,P1);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 2 params, void return)

template <
	class P0, class P1>
class Callable<void(P0,P1)>
:	public ZCallable<void(P0,P1)>
	{
public:
	typedef void (Signature)(P0,P1);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call(P0 i0, P1 i1)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
class Callable<R(P0,P1,P2)>
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef R (Signature)(P0,P1,P2);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 3 params, void return)

template <
	class P0, class P1, class P2>
class Callable<void(P0,P1,P2)>
:	public ZCallable<void(P0,P1,P2)>
	{
public:
	typedef void (Signature)(P0,P1,P2);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call(P0 i0, P1 i1, P2 i2)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class Callable<R(P0,P1,P2,P3)>
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 4 params, void return)

template <
	class P0, class P1, class P2, class P3>
class Callable<void(P0,P1,P2,P3)>
:	public ZCallable<void(P0,P1,P2,P3)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
class Callable<R(P0,P1,P2,P3,P4)>
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4>
class Callable<void(P0,P1,P2,P3,P4)>
:	public ZCallable<void(P0,P1,P2,P3,P4)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable<R(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 6 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable<void(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable<R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 7 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable<void(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 8 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6,P7);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 9 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 10 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 11 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 12 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 13 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 14 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 15 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 16 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(R())
	,	fCallable(iCallable)
		{}

	Callable
		(ZRef<ZCaller> iCaller, const R& iDefault, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fDefault(iDefault)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		ZRef<ZFuture<R> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF, fCallable));

		if (ZQ<R> theQ = theFuture->Get())
			return theQ.Get();

		return fDefault;
		}

private:
	ZRef<ZCaller> fCaller;
	const R fDefault;
	ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 16 params, void return)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef void (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable
		(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		ZRef<ZFuture<void> > theFuture = CallFrom(fCaller,
			BindL(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF, fCallable));

		theFuture->Wait();
		}

private:
	ZRef<ZCaller> fCaller;
	ZRef<ZCallable<Signature> > fCallable;
	};

} // namespace ZCallable_From

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_From

template <class Signature>
ZRef<ZCallable<Signature> >
MakeCallable_From(ZRef<ZCaller> iCaller, ZRef<ZCallable<Signature> > iCallable)
	{ return new ZCallable_From::Callable<Signature> (iCaller, iCallable); }

template <class Signature>
ZRef<ZCallable<Signature> >
MakeCallable_From
	(ZRef<ZCaller> iCaller,
	const typename ZCallable_Bind::ST_T<Signature>::R& iDefault,
	ZRef<ZCallable<Signature> > iCallable)
	{ return new ZCallable_From::Callable<Signature>(iCaller, iDefault, iCallable); }

} // namespace ZooLib

#endif // __ZCallable_From__
