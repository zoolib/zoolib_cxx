/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZCallable__
#define __ZCallable__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableUtil

namespace ZCallableUtil {

template <class T> struct VT
	{
	typedef const T F; // Field
	typedef const T& P; // Parameter
	};

template <class T> struct VT<const T&>
	{
	typedef const T F;
	typedef const T& P;
	};

template <class T> struct VT<T&>
	{
	typedef T& F;
	typedef T& P;
	};

} // namespace ZCallableUtil

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable

template <class Signature> class ZCallable;

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 0 params)

template <class R>
class ZCallable<R(void)>
:	public ZCounted
	{
public:
	typedef R(Signature)(void);

	virtual R Call() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 1 param)

template <class R,
	class P0>
class ZCallable<R(P0)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0);

	virtual R Call(P0) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 2 params)

template <class R,
	class P0, class P1>
class ZCallable<R(P0,P1)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1);

	virtual R Call(P0,P1) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
class ZCallable<R(P0,P1,P2)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2);

	virtual R Call(P0,P1,P2) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class ZCallable<R(P0,P1,P2,P3)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3);

	virtual R Call(P0,P1,P2,P3) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
class ZCallable<R(P0,P1,P2,P3,P4)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4);

	virtual R Call(P0,P1,P2,P3,P4) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class ZCallable<R(P0,P1,P2,P3,P4,P5)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5);

	virtual R Call(P0,P1,P2,P3,P4,P5) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6,P7) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6,P7,P8) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable (specialization for 16 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCounted
	{
public:
	typedef R(Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	virtual R Call(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCall

template <class T>
T sCall(ZRef<ZCallable<T(void)> > iCallable)
	{ return iCallable->Call(); }

// =================================================================================================
#pragma mark -
#pragma mark * sCallReturnVoid

template <class T>
void sCallReturnVoid(ZRef<ZCallable<T(void)> > iCallable)
	{ iCallable->Call(); }

// =================================================================================================
#pragma mark -
#pragma mark * Useful typedefs

typedef ZCallable<void(void)> ZCallable_Void;

typedef ZCallable<bool(void)> ZCallable_Bool;

// =================================================================================================
#pragma mark -
#pragma mark * ZCaller

typedef ZCallable<void(ZRef<ZCallable_Void>)> ZCaller;

} // namespace ZooLib

#endif // __ZCallable__
