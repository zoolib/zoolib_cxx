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

template <class T> struct Traits
	{};

template <class T> struct Traits<T*>
	{
	typedef T* Temp_t;
	static T* sGetTemp(T* iObject) { return iObject; }
	static T* sGetPtr(T* iTemp) { return iTemp; }
	};

template <class T> struct Traits<ZRef<T> >
	{
	typedef const ZRef<T>& Temp_t;
	static const ZRef<T>& sGetTemp(const ZRef<T>& iObject) { return iObject; }
	static T* sGetPtr(const ZRef<T>& iTemp) { return iTemp.Get(); }
	};

template <class T> struct Traits<ZWeakRef<T> >
	{
	typedef ZRef<T> Temp_t;
	static ZRef<T> sGetTemp(const ZWeakRef<T>& iObject) { return iObject; }
	static T* sGetPtr(const ZRef<T>& iTemp) { return iTemp.Get(); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable0

template <class Class_t, class Object_t, class R>
class Callable0
:	public ZCallable<R(void)>
	{
public:
	typedef R (Class_t::*Method_t)();

	Callable0(const Object_t& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

	// From ZCallable
	virtual R Call()
		{
		if (typename Traits<Object_t>::Temp_t temp = Traits<Object_t>::sGetTemp(fObject))
			return (Traits<Object_t>::sGetPtr(temp)->*fMethod)();
		return R();
		}

private:
	Object_t fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable1

template <class Class_t, class Object_t, class R, class P0>
class Callable1
:	public ZCallable<R(P0)>
	{
public:
	typedef R (Class_t::*Method_t)(P0);

	Callable1(const Object_t& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

	// From ZCallable
	virtual R Call(P0 i0)
		{
		if (typename Traits<Object_t>::Temp_t temp = Traits<Object_t>::sGetTemp(fObject))
			return (Traits<Object_t>::sGetPtr(temp)->*fMethod)(i0);
		return R();
		}

private:
	Object_t fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable2

template <class Class_t, class Object_t, class R, class P0, class P1>
class Callable2
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef R (Class_t::*Method_t)(P0, P1);

	Callable2(const Object_t& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{
		if (typename Traits<Object_t>::Temp_t temp = Traits<Object_t>::sGetTemp(fObject))
			return (Traits<Object_t>::sGetPtr(temp)->*fMethod)(i0, i1);
		return R();
		}

private:
	Object_t fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable3

template <class Class_t, class Object_t, class R, class P0, class P1, class P2>
class Callable3
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef R (Class_t::*Method_t)(P0, P1, P2);

	Callable3(const Object_t& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{
		if (typename Traits<Object_t>::Temp_t temp = Traits<Object_t>::sGetTemp(fObject))
			return (Traits<Object_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2);
		return R();
		}

private:
	Object_t fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable4

template <class Class_t, class Object_t, class R, class P0, class P1, class P2, class P3>
class Callable4
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef R (Class_t::*Method_t)(P0, P1, P2, P3);

	Callable4(const Object_t& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (typename Traits<Object_t>::Temp_t temp = Traits<Object_t>::sGetTemp(fObject))
			return (Traits<Object_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2, i3);
		return R();
		}

private:
	Object_t fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable5

template <class Class_t, class Object_t, class R, class P0, class P1, class P2, class P3, class P4>
class Callable5
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef R (Class_t::*Method_t)(P0, P1, P2, P3, P4);

	Callable5(const Object_t& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (typename Traits<Object_t>::Temp_t temp = Traits<Object_t>::sGetTemp(fObject))
			return (Traits<Object_t>::sGetPtr(temp)->*fMethod)(i0, i1, i2, i3, i4);
		return R();
		}

private:
	Object_t fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

template <class Class_t, class Object_t, class R>
ZRef<ZCallable<R(void)> >
MakeCallable(const Object_t& iObject, R (Class_t::*iMethod)())
	{ return new Callable0<Class_t, Object_t, R>(iObject, iMethod); }

template <class Class_t, class Object_t, class R, class P0>
ZRef<ZCallable<R(P0)> >
MakeCallable(const Object_t& iObject, R (Class_t::*iMethod)(P0))
	{ return new Callable1<Class_t, Object_t, R, P0>(iObject, iMethod); }

template <class Class_t, class Object_t, class R, class P0, class P1>
ZRef<ZCallable<R(P0,P1)> >
MakeCallable(const Object_t& iObject, R (Class_t::*iMethod)(P0, P1))
	{ return new Callable2<Class_t, Object_t, R, P0, P1>(iObject, iMethod); }

template <class Class_t, class Object_t, class R, class P0, class P1, class P2>
ZRef<ZCallable<R(P0,P1,P2)> >
MakeCallable(const Object_t& iObject, R (Class_t::*iMethod)(P0, P1, P2))
	{ return new Callable3<Class_t, Object_t, R, P0, P1, P2>(iObject, iMethod); }

template <class Class_t, class Object_t, class R, class P0, class P1, class P2, class P3>
ZRef<ZCallable<R(P0,P1,P2,P3)> >
MakeCallable(const Object_t& iObject, R (Class_t::*iMethod)(P0, P1, P2, P3))
	{ return new Callable4<Class_t, Object_t, R, P0, P1, P2, P3>(iObject, iMethod); }

template <class Class_t, class Object_t, class R, class P0, class P1, class P2, class P3, class P4>
ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >
MakeCallable(const Object_t& iObject, R (Class_t::*iMethod)(P0, P1, P2, P3, P4))
	{ return new Callable5<Class_t, Object_t, R, P0, P1, P2, P3, P4>(iObject, iMethod); }

} // namespace ZCallable_PMF

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

using ZCallable_PMF::MakeCallable;

} // namespace ZooLib

#endif // __ZCallable_PMF__
