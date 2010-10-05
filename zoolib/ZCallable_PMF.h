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

#ifndef __ZCallable_PMF__
#define __ZCallable_PMF__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {
namespace ZCallable_PMF {

// =================================================================================================
#pragma mark -
#pragma mark * Traits

template <class T>
class Traits
	{};

template <class T>
class Traits<T*>
	{
public:
	typedef T* Passed_t;
	typedef T* Stored_t;
	typedef T* Temp_t;
	typedef T* Ptr_t;

	static Stored_t sGetStored(const Passed_t& iPassed)
		{ return iPassed; }

	static Temp_t sGetTemp(const Stored_t& iStored)
		{ return iStored; }

	static Ptr_t sGetPtr(const Temp_t& iTemp)
		{ return iTemp; }
	};

template <class T>
class Traits<ZRef<T> >
	{
public:
	typedef ZRef<T> Passed_t;
	typedef ZRef<T> Stored_t;
	typedef ZRef<T> Temp_t;
	typedef T* Ptr_t;

	static Stored_t sGetStored(const Passed_t& iPassed)
		{ return iPassed; }

	static Temp_t sGetTemp(const Stored_t& iStored)
		{ return iStored; }

	static Ptr_t sGetPtr(const Temp_t& iTemp)
		{ return iTemp.Get(); }
	};

template <class T>
class Traits<ZWeakRef<T> >
	{
public:
	typedef ZWeakRef<T> Passed_t;
	typedef ZWeakRef<T> Stored_t;
	typedef ZRef<T> Temp_t;
	typedef T* Ptr_t;

	static Stored_t sGetStored(const Passed_t& iPassed)
		{ return iPassed; }

	static Temp_t sGetTemp(const Stored_t& iStored)
		{ return iStored; }

	static Ptr_t sGetPtr(const Temp_t& iTemp)
		{ return iTemp.Get(); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable0

template <class Callee_t, class Passed_t, class R>
class Callable0
:	public ZCallable<R(void)>
	{
public:
	typedef typename Traits<Passed_t>::Stored_t Stored_t;
	typedef typename Traits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)();

	Callable0(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable
	virtual R Call()
		{
		if (Temp_t temp = Traits<Stored_t>::sGetTemp(fCallee))
			return (Traits<Temp_t>::sGetPtr(temp)->*fMethod)();
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable1

template <class Callee_t, class Passed_t, class R, class P0>
class Callable1
:	public ZCallable<R(P0)>
	{
public:
	typedef typename Traits<Passed_t>::Stored_t Stored_t;
	typedef typename Traits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0);

	Callable1(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable
	virtual R Call(P0 i0)
		{
		if (Temp_t temp = Traits<Stored_t>::sGetTemp(fCallee))
			return (Traits<Temp_t>::sGetPtr(temp)->*fMethod)(i0);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable2

template <class Callee_t, class Passed_t, class R, class P0, class P1>
class Callable2
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef typename Traits<Passed_t>::Stored_t Stored_t;
	typedef typename Traits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0, P1);

	Callable2(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{
		if (Temp_t temp = Traits<Stored_t>::sGetTemp(fCallee))
			return (Traits<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable3

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2>
class Callable3
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef typename Traits<Passed_t>::Stored_t Stored_t;
	typedef typename Traits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0, P1, P2);

	Callable3(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{
		if (Temp_t temp = Traits<Stored_t>::sGetTemp(fCallee))
			return (Traits<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable4

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2, class P3>
class Callable4
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef typename Traits<Passed_t>::Stored_t Stored_t;
	typedef typename Traits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0, P1, P2, P3);

	Callable4(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (Temp_t temp = Traits<Stored_t>::sGetTemp(fCallee))
			return (Traits<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2, i3);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable5

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2, class P3, class P4>
class Callable5
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef typename Traits<Passed_t>::Stored_t Stored_t;
	typedef typename Traits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0, P1, P2, P3, P4);

	Callable5(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (Temp_t temp = Traits<Stored_t>::sGetTemp(fCallee))
			return (Traits<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2, i3, i4);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

template <class Callee_t, class Passed_t, class R>
ZRef<ZCallable<R(void)> >
MakeCallable(R (Callee_t::*iMethod)(), const Passed_t& iCallee)
	{ return new Callable0<Callee_t, Passed_t, R>(iMethod, iCallee); }

template <class Callee_t, class Passed_t, class R, class P0>
ZRef<ZCallable<R(P0)> >
MakeCallable(R (Callee_t::*iMethod)(P0), const Passed_t& iCallee)
	{ return new Callable1<Callee_t, Passed_t, R, P0>(iMethod, iCallee); }

template <class Callee_t, class Passed_t, class R, class P0, class P1>
ZRef<ZCallable<R(P0,P1)> >
MakeCallable(R (Callee_t::*iMethod)(P0, P1), const Passed_t& iCallee)
	{ return new Callable2<Callee_t, Passed_t, R, P0, P1>(iMethod, iCallee); }

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2>
ZRef<ZCallable<R(P0,P1,P2)> >
MakeCallable(R (Callee_t::*iMethod)(P0, P1, P2), const Passed_t& iCallee)
	{ return new Callable3<Callee_t, Passed_t, R, P0, P1, P2>(iMethod, iCallee); }

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2, class P3>
ZRef<ZCallable<R(P0,P1,P2,P3)> >
MakeCallable(R (Callee_t::*iMethod)(P0, P1, P2, P3), const Passed_t& iCallee)
	{ return new Callable4<Callee_t, Passed_t, R, P0, P1, P2, P3>(iMethod, iCallee); }

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2, class P3, class P4>
ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >
MakeCallable(R (Callee_t::*iMethod)(P0, P1, P2, P3, P4), const Passed_t& iCallee)
	{ return new Callable5<Callee_t, Passed_t, R, P0, P1, P2, P3, P4>(iMethod, iCallee); }

} // namespace ZCallable_PMF

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

using ZCallable_PMF::MakeCallable;

} // namespace ZooLib

#endif // __ZCallable_PMF__
