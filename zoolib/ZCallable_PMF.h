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
#pragma mark * Callable

template <class Class_p, class Object_p, class Signature> class Callable;

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params)

template <class Class_p, class Object_p,
	class R>
class Callable<Class_p,Object_p,R(void)>
:	public ZCallable<R(void)>
	{
public:
	typedef R (Class_p::*Method_t)();

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call()
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				();
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 1 param)

template <class Class_p, class Object_p,
	class R,
	class P0>
class Callable<Class_p,Object_p,R(P0)>
:	public ZCallable<R(P0)>
	{
public:
	typedef R (Class_p::*Method_t)(P0);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call(P0 i0)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 2 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1>
class Callable<Class_p,Object_p,R(P0,P1)>
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 3 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2>
class Callable<Class_p,Object_p,R(P0,P1,P2)>
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 4 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3)>
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4)>
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 6 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 7 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 8 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6,P7);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6, i7);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 9 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6, i7, i8);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 10 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 11 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 12 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 13 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 14 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 15 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 16 params)

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef R (Class_p::*Method_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
			}
		return R();
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCallable

template <class Class_p, class Object_p,
	class R>
ZRef<ZCallable<R(void)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)())
	{ return new Callable<Class_p,Object_p,R(void)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0>
ZRef<ZCallable<R(P0)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0))
	{ return new Callable<Class_p,Object_p,R(P0)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1>
ZRef<ZCallable<R(P0,P1)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1))
	{ return new Callable<Class_p,Object_p,R(P0,P1)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2>
ZRef<ZCallable<R(P0,P1,P2)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2))
	{ return new Callable<Class_p,Object_p,R(P0,P1,P2)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3>
ZRef<ZCallable<R(P0,P1,P2,P3)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3))
	{ return new Callable<Class_p,Object_p,R(P0,P1,P2,P3)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4>
ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4))
	{ return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5))
	{ return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6))
	{ return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6,P7))
	{ return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6,P7,P8))
	{ return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9))
	{ return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA))
	{ return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>(iObject, iMethod); }

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB))
	{
	return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
		(iObject, iMethod);
	}

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC))
	{
	return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
		(iObject, iMethod);
	}

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >
sCallable
	(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD))
	{
	return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
		(iObject, iMethod);
	}

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >
sCallable(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE))
	{
	return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
		(iObject, iMethod);
	}

template <class Class_p, class Object_p,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >
sCallable(const Object_p& iObject,
	R (Class_p::*iMethod)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF))
	{
	return new Callable<Class_p,Object_p,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
		(iObject, iMethod);
	}

} // namespace ZCallable_PMF

// =================================================================================================
#pragma mark -
#pragma mark * sCallable

using ZCallable_PMF::sCallable;

} // namespace ZooLib

#endif // __ZCallable_PMF__
