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

#ifndef __ZCallable_PMF_h__
#define __ZCallable_PMF_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {
namespace ZCallable_PMF {

// =================================================================================================
// MARK: - Traits

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
// MARK: - Callable

template <class Class_p, class Object_p, class Signature> class Callable;

// =================================================================================================
// MARK: - Callable (specialization for 0 params)

template <class Class_p, class Object_p, class R>
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
	virtual ZQ<R> QCall()
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)();
		return null;
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
// MARK: - Callable (specialization for 0 params, void return)

template <class Class_p, class Object_p>
class Callable<Class_p,Object_p,void(void)>
:	public ZCallable<void(void)>
	{
public:
	typedef void (Class_p::*Method_t)();

	Callable(const Object_p& iObject, Method_t iMethod)
	:	fObject(iObject)
	,	fMethod(iMethod)
		{}

// From ZCallable
	virtual ZQ<void> QCall()
		{
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject))
			{
			(Traits<Object_p>::sGetPtr(temp)->*fMethod)();
			return notnull;
			}
		return null;
		}

private:
	Object_p fObject;
	Method_t fMethod;
	};

// =================================================================================================
// MARK: - Callable variants

#define ZMACRO_Callable_Callable(X) \
\
template <class Class_p, class Object_p, class R, ZMACRO_Callable_Class_P##X> \
class Callable<Class_p,Object_p,R(ZMACRO_Callable_P##X)> \
:	public ZCallable<R(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef R (Class_p::*Method_t)(ZMACRO_Callable_P##X); \
\
	Callable(const Object_p& iObject, Method_t iMethod) \
	:	fObject(iObject) \
	,	fMethod(iMethod) \
		{} \
\
	virtual ZQ<R> QCall(ZMACRO_Callable_Pi##X) \
		{ \
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject)) \
			return (Traits<Object_p>::sGetPtr(temp)->*fMethod)(ZMACRO_Callable_i##X); \
		return null; \
		} \
\
private: \
	Object_p fObject; \
	Method_t fMethod; \
	}; \
\
template <class Class_p, class Object_p, ZMACRO_Callable_Class_P##X> \
class Callable<Class_p,Object_p,void(ZMACRO_Callable_P##X)> \
:	public ZCallable<void(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef void (Class_p::*Method_t)(ZMACRO_Callable_P##X); \
\
	Callable(const Object_p& iObject, Method_t iMethod) \
	:	fObject(iObject) \
	,	fMethod(iMethod) \
		{} \
\
	virtual ZQ<void> QCall(ZMACRO_Callable_Pi##X) \
		{ \
		if (typename Traits<Object_p>::Temp_t temp = Traits<Object_p>::sGetTemp(fObject)) \
			{ \
			(Traits<Object_p>::sGetPtr(temp)->*fMethod)(ZMACRO_Callable_i##X); \
			return notnull; \
			} \
		return null; \
		} \
\
private: \
	Object_p fObject; \
	Method_t fMethod; \
	};

ZMACRO_Callable_Callable(0)
ZMACRO_Callable_Callable(1)
ZMACRO_Callable_Callable(2)
ZMACRO_Callable_Callable(3)
ZMACRO_Callable_Callable(4)
ZMACRO_Callable_Callable(5)
ZMACRO_Callable_Callable(6)
ZMACRO_Callable_Callable(7)
ZMACRO_Callable_Callable(8)
ZMACRO_Callable_Callable(9)
ZMACRO_Callable_Callable(A)
ZMACRO_Callable_Callable(B)
ZMACRO_Callable_Callable(C)
ZMACRO_Callable_Callable(D)
ZMACRO_Callable_Callable(E)
ZMACRO_Callable_Callable(F)

#undef ZMACRO_Callable_Callable

// =================================================================================================
// MARK: - sCallable

template <class Class_p, class Object_p, class R>
ZRef<ZCallable<R(void)> >
sCallable(
	const Object_p& iObject,
	R (Class_p::*iMethod)())
	{ return new Callable<Class_p,Object_p,R(void)>(iObject, iMethod); }

#define ZMACRO_Callable_sCallable(X) \
\
template <class Class_p, class Object_p, class R, ZMACRO_Callable_Class_P##X> \
ZRef<ZCallable<R(ZMACRO_Callable_P##X)> > \
sCallable( \
	const Object_p& iObject, \
	R (Class_p::*iMethod)(ZMACRO_Callable_P##X)) \
	{ return new Callable<Class_p,Object_p,R(ZMACRO_Callable_P##X)>(iObject, iMethod); }

ZMACRO_Callable_sCallable(0)
ZMACRO_Callable_sCallable(1)
ZMACRO_Callable_sCallable(2)
ZMACRO_Callable_sCallable(3)
ZMACRO_Callable_sCallable(4)
ZMACRO_Callable_sCallable(5)
ZMACRO_Callable_sCallable(6)
ZMACRO_Callable_sCallable(7)
ZMACRO_Callable_sCallable(8)
ZMACRO_Callable_sCallable(9)
ZMACRO_Callable_sCallable(A)
ZMACRO_Callable_sCallable(B)
ZMACRO_Callable_sCallable(C)
ZMACRO_Callable_sCallable(D)
ZMACRO_Callable_sCallable(E)
ZMACRO_Callable_sCallable(F)

#undef ZMACRO_Callable_sCallable

} // namespace ZCallable_PMF

// =================================================================================================
// MARK: - sCallable

using ZCallable_PMF::sCallable;

} // namespace ZooLib

#endif // __ZCallable_PMF_h__
