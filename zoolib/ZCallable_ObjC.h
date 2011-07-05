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

template <class R, class FunctionPtr_t, int selector = Selector<R>::value>
struct MsgSend;

template <class R, class FunctionPtr_t>
struct MsgSend<R, FunctionPtr_t, eUse_normal>
	{ static FunctionPtr_t sMsgSend() { return (FunctionPtr_t)objc_msgSend; } };

template <class R, class FunctionPtr_t>
struct MsgSend<R, FunctionPtr_t, eUse_stret>
	{ static FunctionPtr_t sMsgSend() { return (FunctionPtr_t)objc_msgSend_stret; } };

#if defined(__i386__) || defined(__x86_64__)
template <class R, class FunctionPtr_t>
struct MsgSend<R, FunctionPtr_t, eUse_fpret>
	{ static FunctionPtr_t sMsgSend() { return (FunctionPtr_t)objc_msgSend_fpret; } };
#endif // defined(__i386__) || defined(__x86_64__)

// =================================================================================================
#pragma mark -
#pragma mark * Base

// =================================================================================================
#pragma mark -
#pragma mark * Callable

template <class Obj, class Signature> class Callable;

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params)

template <class Obj,
	class R>
class Callable<Obj,R(void)>
:	public ZCallable<R(void)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call()
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 1 param)

template <class Obj,
	class R,
	class P0>
class Callable<Obj,R(P0)>
:	public ZCallable<R(P0)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call(P0 i0)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 2 params)

template <class Obj,
	class R,
	class P0, class P1>
class Callable<Obj,R(P0,P1)>
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 3 params)

template <class Obj,
	class R,
	class P0, class P1, class P2>
class Callable<Obj,R(P0,P1,P2)>
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 4 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3>
class Callable<Obj,R(P0,P1,P2,P3)>
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4>
class Callable<Obj,R(P0,P1,P2,P3,P4)>
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 6 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 7 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 8 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6,P7);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6, i7);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 9 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6, i7, i8);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 10 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 11 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 12 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 13 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 14 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 15 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 16 params)

template <class Obj,
	class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable<Obj,R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef R (*FunctionPtr_t)(id,SEL,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable(Obj iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		return MsgSend<R, FunctionPtr_t>::sMsgSend()
			(fObj, fSEL, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
		}

private:
	Obj fObj;
	SEL fSEL;
	};

} // namespace ZCallable_ObjC

// =================================================================================================
#pragma mark -
#pragma mark * sCallable

template <class Signature>
ZRef<ZCallable<Signature> >
sCallable(id iObj, SEL iSEL)
	{ return new ZCallable_ObjC::Callable<id,Signature>(iObj, iSEL); }

template <class Signature,class T>
ZRef<ZCallable<Signature> >
sCallable(const ZRef<T>& iObj, SEL iSEL)
	{ return new ZCallable_ObjC::Callable<ZRef<NSObject>,Signature>(iObj, iSEL); }

} // namespace ZooLib

#endif // __OBJC__
#endif // __ZCallable_ObjC__
