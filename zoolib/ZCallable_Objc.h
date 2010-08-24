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

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableBase_Objc

class ZCallableBase_Objc
	{
public:
	ZCallableBase_Objc(id iObj, SEL iSEL) : fObj(iObj), fSEL(iSEL) {}

protected:
	template <class T> struct RefAsPtr
		{ static T sGet(const T& iT) { return iT; } };

	template <class T> struct RefAsPtr<const T&>
		{ static const T* sGet(const T& iT) { return &iT; } };

	template <class T> struct RefAsPtr<T&>
		{ static T* sGet(T& iT) { return &iT; } };

	id fObj;
	SEL fSEL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_O0

class ZCallable_Objc_O0
:	ZCallableBase_Objc
,	public ZCallable0<id>
	{
public:
	ZCallable_Objc_O0(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable0
	virtual id Invoke()
		{ return objc_msgSend(fObj, fSEL); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_O1

template <class P0>
class ZCallable_Objc_O1
:	ZCallableBase_Objc
,	public ZCallable1<id, P0>
	{
public:
	ZCallable_Objc_O1(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable1
	virtual id Invoke(P0 i0)
		{
		return objc_msgSend(fObj, fSEL,
			RefAsPtr<P0>::sGet(i0));
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_O2

template <class P0, class P1>
class ZCallable_Objc_O2
:	ZCallableBase_Objc
,	public ZCallable2<id, P0, P1>
	{
public:
	ZCallable_Objc_O2(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable2
	virtual id Invoke(P0 i0, P1 i1)
		{
		return objc_msgSend(fObj, fSEL,
			RefAsPtr<P0>::sGet(i0),
			RefAsPtr<P1>::sGet(i1));
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_O3

template <class P0, class P1, class P2>
class ZCallable_Objc_O3
:	ZCallableBase_Objc
,	public ZCallable3<id, P0, P1, P2>
	{
public:
	ZCallable_Objc_O3(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable3
	virtual id Invoke(P0 i0, P1 i1, P2 i2)
		{
		return objc_msgSend(fObj, fSEL,
			RefAsPtr<P0>::sGet(i0),
			RefAsPtr<P1>::sGet(i1),
			RefAsPtr<P2>::sGet(i2));
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_O4

template <class P0, class P1, class P2, class P3>
class ZCallable_Objc_O4
:	ZCallableBase_Objc
,	public ZCallable4<id, P0, P1, P2, P3>
	{
public:
	ZCallable_Objc_O4(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable4
	virtual id Invoke(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		return objc_msgSend(fObj, fSEL,
			RefAsPtr<P0>::sGet(i0),
			RefAsPtr<P1>::sGet(i1),
			RefAsPtr<P2>::sGet(i2),
			RefAsPtr<P3>::sGet(i3));
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_V0

class ZCallable_Objc_V0
:	ZCallableBase_Objc
,	public ZCallable0<void>
	{
public:
	ZCallable_Objc_V0(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable0
	virtual void Invoke()
		{ objc_msgSend(fObj, fSEL); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_V1

template <class P0>
class ZCallable_Objc_V1
:	ZCallableBase_Objc
,	public ZCallable1<void,P0>
	{
public:
	ZCallable_Objc_V1(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable1
	virtual void Invoke(P0 i0)
		{
		objc_msgSend(fObj, fSEL,
			RefAsPtr<P0>::sGet(i0));
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_V2

template <class P0, class P1>
class ZCallable_Objc_V2
:	ZCallableBase_Objc
,	public ZCallable2<void,P0, P1>
	{
public:
	ZCallable_Objc_V2(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable2
	virtual void Invoke(P0 i0, P1 i1)
		{
		objc_msgSend(fObj, fSEL,
			RefAsPtr<P0>::sGet(i0),
			RefAsPtr<P1>::sGet(i1));
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_V3

template <class P0, class P1, class P2>
class ZCallable_Objc_V3
:	ZCallableBase_Objc
,	public ZCallable3<void,P0, P1, P2>
	{
public:
	ZCallable_Objc_V3(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable3
	virtual void Invoke(P0 i0, P1 i1, P2 i2)
		{
		objc_msgSend(fObj, fSEL,
			RefAsPtr<P0>::sGet(i0),
			RefAsPtr<P1>::sGet(i1),
			RefAsPtr<P2>::sGet(i2));
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Objc_V4

template <class P0, class P1, class P2, class P3>
class ZCallable_Objc_V4
:	ZCallableBase_Objc
,	public ZCallable4<void,P0, P1, P2, P3>
	{
public:
	ZCallable_Objc_V4(id iObj, SEL iSEL) : ZCallableBase_Objc(iObj, iSEL) {}

	// From ZCallable4
	virtual void Invoke(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		objc_msgSend(fObj, fSEL,
			RefAsPtr<P0>::sGet(i0),
			RefAsPtr<P1>::sGet(i1),
			RefAsPtr<P2>::sGet(i2),
			RefAsPtr<P3>::sGet(i3));
		}
	};

} // namespace ZooLib

#endif // __OBJC__
#endif // __ZCallable_Objc__
