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

#ifndef __ZCallable_Objc__
#define __ZCallable_Objc__ 1
#include "zconfig.h"

#ifdef __OBJC__
#include "zoolib/ZCallable.h"
#include "zoolib/ZOBJC.h"

#include <objc/message.h>

namespace ZooLib {

// Warning, does not handle large-sized return types. We'll need to call objc_msgSend_stret,
// but I need to examine the ABI docs first.

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableBase_Objc

class ZCallableBase_Objc
	{
public:
	ZCallableBase_Objc(id iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

protected:
	id fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc0

template <class R>
class ZCallable_Objc0
:	ZCallableBase_Objc
,	public ZCallable0<R>
	{
public:
	ZCallable_Objc0(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL);

	// From ZCallable0
	virtual R Invoke()
		{ return ((Function_t)objc_msgSend)(fObj, fSEL); }
	};

template <class R>
ZRef<ZCallable0<R> >
MakeCallable(id iObj, SEL iSEL)
	{ return new ZCallable_Objc0<R>(iObj, iSEL); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc1

template <class R, class P0>
class ZCallable_Objc1
:	ZCallableBase_Objc
,	public ZCallable1<R,P0>
	{
public:
	ZCallable_Objc1(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL, P0);

	// From ZCallable1
	virtual R Invoke(P0 i0)
		{ return ((Function_t)objc_msgSend)(fObj, fSEL, i0); }
	};

template <class R, class P0>
ZRef<ZCallable1<R,P0> >
MakeCallable(id iObj, SEL iSEL)
	{ return new ZCallable_Objc1<R,P0>(iObj, iSEL); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc2

template <class R, class P0, class P1>
class ZCallable_Objc2
:	ZCallableBase_Objc
,	public ZCallable2<R,P0, P1>
	{
public:
	ZCallable_Objc2(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL, P0, P1);

	// From ZCallable2
	virtual R Invoke(P0 i0, P1 i1)
		{ return ((Function_t)objc_msgSend)(fObj, fSEL, i0, i1); }
	};

template <class R, class P0, class P1>
ZRef<ZCallable2<R,P0,P1> >
MakeCallable(id iObj, SEL iSEL)
	{ return new ZCallable_Objc2<R,P0,P1>(iObj, iSEL); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc3

template <class R, class P0, class P1, class P2>
class ZCallable_Objc3
:	ZCallableBase_Objc
,	public ZCallable3<R,P0, P1, P2>
	{
public:
	ZCallable_Objc3(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL, P0, P1, P2);

	// From ZCallable3
	virtual R Invoke(P0 i0, P1 i1, P2 i2)
		{ return ((Function_t)objc_msgSend)(fObj, fSEL, i0, i1, i2); }
	};

template <class R, class P0, class P1, class P2>
ZRef<ZCallable3<R,P0,P1,P2> >
MakeCallable(id iObj, SEL iSEL)
	{ return new ZCallable_Objc3<R,P0,P1,P2>(iObj, iSEL); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc4

template <class R, class P0, class P1, class P2, class P3>
class ZCallable_Objc4
:	ZCallableBase_Objc
,	public ZCallable4<R,P0,P1,P2,P3>
	{
public:
	ZCallable_Objc4(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}
	typedef R (*Function_t)(id, SEL, P0, P1, P2, P3);

	// From ZCallable4
	virtual R Invoke(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return ((Function_t)objc_msgSend)(fObj, fSEL, i0, i1, i2, i3); }
	};

template <class R, class P0, class P1, class P2, class P3>
ZRef<ZCallable4<R,P0,P1,P2,P3> >
MakeCallable(id iObj, SEL iSEL)
	{ return new ZCallable_Objc4<R,P0,P1,P2,P3>(iObj, iSEL); }

} // namespace ZooLib

#endif // __OBJC__
#endif // __ZCallable_Objc__
