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

#ifndef __ZCallable_T__
#define __ZCallable_T__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableTraits_T

template <class T>
class ZCallableTraits_T
	{};

template <class T>
class ZCallableTraits_T<T*>
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
class ZCallableTraits_T<ZRef<T> >
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
class ZCallableTraits_T<ZWeakRef<T> >
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
#pragma mark * ZCallable_T_R0

template <class Callee_t, class Passed_t, class R>
class ZCallable_T_R0
:	public ZCallable_R0<R>
	{
public:
	typedef typename ZCallableTraits_T<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallableTraits_T<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)();

	ZCallable_T_R0(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_R0
	virtual R Invoke()
		{
		if (Temp_t temp = ZCallableTraits_T<Stored_t>::sGetTemp(fCallee))
			return (ZCallableTraits_T<Temp_t>::sGetPtr(temp)->*fMethod)();
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class R>
ZRef<ZCallable_R0<R> >
MakeCallable(R (Callee_t::*iMethod)(), const Passed_t& iCallee)
	{ return new ZCallable_T_R0<Callee_t, Passed_t, R>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_T_R1

template <class Callee_t, class Passed_t, class R, class P0>
class ZCallable_T_R1
:	public ZCallable_R1<R, P0>
	{
public:
	typedef typename ZCallableTraits_T<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallableTraits_T<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0);

	ZCallable_T_R1(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_R1
	virtual R Invoke(P0 i0)
		{
		if (Temp_t temp = ZCallableTraits_T<Stored_t>::sGetTemp(fCallee))
			return (ZCallableTraits_T<Temp_t>::sGetPtr(temp)->*fMethod)(i0);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class R, class P0>
ZRef<ZCallable_R1<R, P0> >
MakeCallable(R (Callee_t::*iMethod)(P0), const Passed_t& iCallee)
	{ return new ZCallable_T_R1<Callee_t, Passed_t, R, P0>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_T_R2

template <class Callee_t, class Passed_t, class R, class P0, class P1>
class ZCallable_T_R2
:	public ZCallable_R2<R, P0, P1>
	{
public:
	typedef typename ZCallableTraits_T<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallableTraits_T<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0, P1);

	ZCallable_T_R2(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_R2
	virtual R Invoke(P0 i0, P1 i1)
		{
		if (Temp_t temp = ZCallableTraits_T<Stored_t>::sGetTemp(fCallee))
			return (ZCallableTraits_T<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class R, class P0, class P1>
ZRef<ZCallable_R2<R, P0, P1> >
MakeCallable(R (Callee_t::*iMethod)(P0, P1), const Passed_t& iCallee)
	{ return new ZCallable_T_R2<Callee_t, Passed_t, R, P0, P1>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_T_R3

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2>
class ZCallable_T_R3
:	public ZCallable_R3<R, P0, P1, P2>
	{
public:
	typedef typename ZCallableTraits_T<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallableTraits_T<Stored_t>::Temp_t Temp_t;

	typedef R (Callee_t::*Method_t)(P0, P1);

	ZCallable_T_R3(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_R3
	virtual R Invoke(P0 i0, P1 i1, P2 i2)
		{
		if (Temp_t temp = ZCallableTraits_T<Stored_t>::sGetTemp(fCallee))
			return (ZCallableTraits_T<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2);
		return R();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class R, class P0, class P1, class P2>
ZRef<ZCallable_R3<R, P0, P1, P2> >
MakeCallable(R (Callee_t::*iMethod)(P0, P1, P2), const Passed_t& iCallee)
	{ return new ZCallable_T_R3<Callee_t, Passed_t, R, P0, P1, P2>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_T_V0

template <class Callee_t, class Passed_t>
class ZCallable_T_V0
:	public ZCallable_V0
	{
public:
	typedef typename ZCallableTraits_T<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallableTraits_T<Stored_t>::Temp_t Temp_t;

	typedef void (Callee_t::*Method_t)();

	ZCallable_T_V0(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_V0
	virtual void Invoke()
		{
		if (Temp_t temp = ZCallableTraits_T<Stored_t>::sGetTemp(fCallee))
			(ZCallableTraits_T<Temp_t>::sGetPtr(temp)->*fMethod)();
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t>
ZRef<ZCallable_V0>
MakeCallable(void (Callee_t::*iMethod)(), const Passed_t& iCallee)
	{ return new ZCallable_T_V0<Callee_t, Passed_t>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_T_V1

template <class Callee_t, class Passed_t, class P0>
class ZCallable_T_V1
:	public ZCallable_V1<P0>
	{
public:
	typedef typename ZCallableTraits_T<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallableTraits_T<Stored_t>::Temp_t Temp_t;

	typedef void (Callee_t::*Method_t)(P0);

	ZCallable_T_V1(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_V1
	virtual void Invoke(P0 i0)
		{
		if (Temp_t temp = ZCallableTraits_T<Stored_t>::sGetTemp(fCallee))
			(ZCallableTraits_T<Temp_t>::sGetPtr(temp)->*fMethod)(i0);
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class P0>
ZRef<ZCallable_V1<P0> >
MakeCallable(void (Callee_t::*iMethod)(P0), const Passed_t& iCallee)
	{ return new ZCallable_T_V1<Callee_t, Passed_t, P0>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_T_V2

template <class Callee_t, class Passed_t, class P0, class P1>
class ZCallable_T_V2
:	public ZCallable_V2<P0, P1>
	{
public:
	typedef typename ZCallableTraits_T<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallableTraits_T<Stored_t>::Temp_t Temp_t;

	typedef void (Callee_t::*Method_t)(P0, P1);

	ZCallable_T_V2(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_V2
	virtual void Invoke(P0 i0, P1 i1)
		{
		if (Temp_t temp = ZCallableTraits_T<Stored_t>::sGetTemp(fCallee))
			(ZCallableTraits_T<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1);
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class P0, class P1>
ZRef<ZCallable_V2<P0, P1> >
MakeCallable(void (Callee_t::*iMethod)(P0, P1), const Passed_t& iCallee)
	{ return new ZCallable_T_V2<Callee_t, Passed_t, P0, P1>(iMethod, iCallee); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_T_V3

template <class Callee_t, class Passed_t, class P0, class P1, class P2>
class ZCallable_T_V3
:	public ZCallable_V3<P0, P1, P2>
	{
public:
	typedef typename ZCallableTraits_T<Passed_t>::Stored_t Stored_t;
	typedef typename ZCallableTraits_T<Stored_t>::Temp_t Temp_t;

	typedef void (Callee_t::*Method_t)(P0, P1);

	ZCallable_T_V3(Method_t iMethod, const Passed_t& iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_V3
	virtual void Invoke(P0 i0, P1 i1, P2 i2)
		{
		if (Temp_t temp = ZCallableTraits_T<Stored_t>::sGetTemp(fCallee))
			(ZCallableTraits_T<Temp_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2);
		}

private:
	Method_t fMethod;
	Stored_t fCallee;
	};

template <class Callee_t, class Passed_t, class P0, class P1, class P2>
ZRef<ZCallable_V3<P0, P1, P2> >
MakeCallable(void (Callee_t::*iMethod)(P0, P1, P2), const Passed_t& iCallee)
	{ return new ZCallable_T_V3<Callee_t, Passed_t, P0, P1, P2>(iMethod, iCallee); }

} // namespace ZooLib

#endif // __ZCallable_T__
