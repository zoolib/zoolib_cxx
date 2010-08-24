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
#include "zoolib/ZRef.h"
#include "zoolib/ZWeakRef.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_PMFTraits

template <class T>
class ZCallable_PMFTraits
	{};

template <class T>
class ZCallable_PMFTraits<T*>
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
class ZCallable_PMFTraits<ZRef<T> >
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
class ZCallable_PMFTraits<ZWeakRef<T> >
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
#pragma mark * ZCallable_PMF0

template <class Callee_t, class Passed_t, class R>
class ZCallable_PMF0
:	public ZCallable0<R>
	{
public:
	typedef typename ZCallable_PMFTraits<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallable_PMFTraits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)();

	ZCallable_PMF0(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable0
	virtual R Invoke()
		{
		if (Temp_t temp = ZCallable_PMFTraits<Stored_t>::sGetTemp(fCallee))
			return (ZCallable_PMFTraits<Temp_t>::sGetPtr(temp)->*fMethod)();
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class R>
ZRef<ZCallable0<R> >
MakeCallable(R (Callee_t::*iMethod)(), const Passed_t& iCallee)
	{ return new ZCallable_PMF0<Callee_t, Passed_t, R>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_PMF1

template <class Callee_t, class Passed_t, class R, class P0>
class ZCallable_PMF1
:	public ZCallable1<R,P0>
	{
public:
	typedef typename ZCallable_PMFTraits<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallable_PMFTraits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0);

	ZCallable_PMF1(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable1
	virtual R Invoke(P0 i0)
		{
		if (Temp_t temp = ZCallable_PMFTraits<Stored_t>::sGetTemp(fCallee))
			return (ZCallable_PMFTraits<Temp_t>::sGetPtr(temp)->*fMethod)(i0);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class R, class P0>
ZRef<ZCallable1<R,P0> >
MakeCallable(R (Callee_t::*iMethod)(P0), const Passed_t& iCallee)
	{ return new ZCallable_PMF1<Callee_t, Passed_t, R, P0>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_PMF2

template <class Callee_t, class Passed_t, class R, class P0, class P1>
class ZCallable_PMF2
:	public ZCallable2<R,P0,P1>
	{
public:
	typedef typename ZCallable_PMFTraits<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallable_PMFTraits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0, P1);

	ZCallable_PMF2(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable2
	virtual R Invoke(P0 i0, P1 i1)
		{
		if (Temp_t temp = ZCallable_PMFTraits<Stored_t>::sGetTemp(fCallee))
			return (ZCallable_PMFTraits<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class R, class P0, class P1>
ZRef<ZCallable2<R,P0,P1> >
MakeCallable(R (Callee_t::*iMethod)(P0, P1), const Passed_t& iCallee)
	{ return new ZCallable_PMF2<Callee_t, Passed_t, R, P0, P1>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_PMF3

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2>
class ZCallable_PMF3
:	public ZCallable3<R,P0,P1,P2>
	{
public:
	typedef typename ZCallable_PMFTraits<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallable_PMFTraits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0, P1, P2);

	ZCallable_PMF3(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable3
	virtual R Invoke(P0 i0, P1 i1, P2 i2)
		{
		if (Temp_t temp = ZCallable_PMFTraits<Stored_t>::sGetTemp(fCallee))
			return (ZCallable_PMFTraits<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2>
ZRef<ZCallable3<R,P0,P1,P2> >
MakeCallable(R (Callee_t::*iMethod)(P0, P1, P2), const Passed_t& iCallee)
	{ return new ZCallable_PMF3<Callee_t, Passed_t, R, P0, P1, P2>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_PMF4

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2, class P3>
class ZCallable_PMF4
:	public ZCallable4<R,P0,P1,P2,P3>
	{
public:
	typedef typename ZCallable_PMFTraits<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallable_PMFTraits<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0, P1, P2, P3);

	ZCallable_PMF4(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable3
	virtual R Invoke(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (Temp_t temp = ZCallable_PMFTraits<Stored_t>::sGetTemp(fCallee))
			return (ZCallable_PMFTraits<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2, i3);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2, class P3>
ZRef<ZCallable4<R,P0,P1,P2,P3> >
MakeCallable(R (Callee_t::*iMethod)(P0, P1, P2, P3), const Passed_t& iCallee)
	{ return new ZCallable_PMF4<Callee_t, Passed_t, R, P0, P1, P2, P3>(iMethod, iCallee); }

} // namespace ZooLib

#endif // __ZCallable_PMF__
