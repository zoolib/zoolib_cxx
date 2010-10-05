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

#ifndef __ZCallable_ObjC__
#define __ZCallable_ObjC__ 1
#include "zconfig.h"
#include "zoolib/ZCallable.h"

#ifdef __OBJC__

#include <objc/message.h>
#if defined(__arm__)
	#include <tr1/type_traits>
#endif

namespace ZooLib {
namespace ZCallable_ObjC {

#if defined(__arm__)
	using namespace std::tr1;
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

enum
	{
	eUse_normal,
	eUse_stret
	#if defined(__i386__) || defined(__x86_64__)
		,eUse_fpret
	#endif
	};

// -----

template <bool> struct IfSmall { enum { value = eUse_normal }; };
template <> struct IfSmall<false> { enum { value = eUse_stret }; };

#if defined(__arm__)

	// On Arm void, integer, float, and enum (natural types) use objc_msgSend.
	// By inspection, objc_msgSend is also used for structures <= 4 bytes in size.
	template <class T,
		bool = is_void<T>::value || is_integral<T>::value
		|| is_floating_point<T>::value || is_enum<T>::value>
	struct Selector { enum { value = eUse_normal }; };

	template <class T> struct Selector<T, false>
		{ enum { value = IfSmall<(sizeof(T)<=4)>::value }; };

#else

	// On PPC and x86 all types <= 8 bytes use objc_msgSend, others use objc_msgSend_stret.
	template <class T> struct Selector { enum { value = IfSmall<(sizeof(T) <= 8)>::value }; };

	// On 32 bit x86 floats and doubles use objc_msgSend_fpret.
	#if defined(__i386__)
		template <> struct Selector<float> { enum { value = eUse_fpret }; };
		template <> struct Selector<double> { enum { value = eUse_fpret }; };
	#endif

	// On 32 and 64 bit x86, long doubles also use objc_msgSend_fpret.
	#if defined(__i386__) || defined(__x86_64__)
		template <> struct Selector<long double> { enum { value = eUse_fpret }; };
	#endif

#endif

// Explicit specialization for void, so we don't end up taking sizeof(void).
template <> struct Selector<void> { enum { value = eUse_normal }; };

// -----

template <class R, class Func_t, int selector = Selector<R>::value>
struct MsgSend;

template <class R, class Func_t>
struct MsgSend<R, Func_t, eUse_normal>
	{ static Func_t sMsgSend() { return (Func_t)objc_msgSend; } };

template <class R, class Func_t>
struct MsgSend<R, Func_t, eUse_stret>
	{ static Func_t sMsgSend() { return (Func_t)objc_msgSend_stret; } };

#if defined(__i386__) || defined(__x86_64__)
	template <class R, class Func_t>
	struct MsgSend<R, Func_t, eUse_fpret>
		{ static Func_t sMsgSend() { return (Func_t)objc_msgSend_fpret; } };
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Base

class Base
	{
public:
	Base(id iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

protected:
	id fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable0

template <class Func> class Callable;

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params)

template <class R>
class Callable<R(void)>
:	Base
,	public ZCallable<R(void)>
	{
public:
	Callable(id iObj, SEL iSEL) : Base(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL);

	// From ZCallable
	virtual R Call()
		{
		return MsgSend<R, Function_t>::sMsgSend()
			(fObj, fSEL);
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 1 param)

template <class R, class P0>
class Callable<R(P0)>
:	Base
,	public ZCallable<R(P0)>
	{
public:
	Callable(id iObj, SEL iSEL) : Base(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL, P0);

	// From ZCallable
	virtual R Call(P0 i0)
		{
		return MsgSend<R, Function_t>::sMsgSend()
			(fObj, fSEL, i0);
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 2 params)

template <class R, class P0, class P1>
class Callable<R(P0,P1)>
:	Base
,	public ZCallable<R(P0,P1)>
	{
public:
	Callable(id iObj, SEL iSEL) : Base(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL, P0, P1);

	// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{
		return MsgSend<R, Function_t>::sMsgSend()
			(fObj, fSEL, i0, i1);
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 3 params)

template <class R, class P0, class P1, class P2>
class Callable<R(P0,P1,P2)>
:	Base
,	public ZCallable<R(P0,P1,P2)>
	{
public:
	Callable(id iObj, SEL iSEL) : Base(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL, P0, P1, P2);

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{
		return MsgSend<R, Function_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2);
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params)

template <class R, class P0, class P1, class P2, class P3>
class Callable<R(P0,P1,P2,P3)>
:	Base
,	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	Callable(id iObj, SEL iSEL) : Base(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL, P0, P1, P2, P3);

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		return MsgSend<R, Function_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3);
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable5

template <class R, class P0, class P1, class P2, class P3, class P4>
class Callable<R(P0,P1,P2,P3,P4)>
:	Base
,	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	Callable(id iObj, SEL iSEL) : Base(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL, P0, P1, P2, P3, P4);

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		return MsgSend<R, Function_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4);
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

template <class Func>
ZRef<ZCallable<Func> >
MakeCallable(id iObj, SEL iSEL)
	{ return new Callable<Func>(iObj, iSEL); }

} // namespace ZCallable_ObjC

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

using ZCallable_ObjC::MakeCallable;

} // namespace ZooLib

#endif // __OBJC__
#endif // __ZCallable_ObjC__
