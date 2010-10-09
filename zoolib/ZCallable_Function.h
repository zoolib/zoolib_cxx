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

#ifndef __ZCallable_Function__
#define __ZCallable_Function__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

namespace ZCallable_Function {

// =================================================================================================
#pragma mark -
#pragma mark * Callable0

template <class R>
class Callable0
:	public ZCallable<R(void)>
	{
public:
	typedef R (*FunctionPtr_t)();

	Callable0(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call()
		{ return fFunctionPtr(); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable1

template <class R,
	class P0>
class Callable1
:	public ZCallable<R(P0)>
	{
public:
	typedef R (*FunctionPtr_t)(P0);

	Callable1(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call(P0 i0)
		{ return fFunctionPtr(i0); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable2

template <class R,
	class P0, class P1>
class Callable2
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1);

	Callable2(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{ return fFunctionPtr(i0, i1); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable3

template <class R,
	class P0, class P1, class P2>
class Callable3
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2);

	Callable3(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{ return fFunctionPtr(i0, i1, i2); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable4

template <class R,
	class P0, class P1, class P2, class P3>
class Callable4
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3);

	Callable4(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fFunctionPtr(i0, i1, i2, i3); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable5

template <class R,
	class P0, class P1, class P2, class P3, class P4>
class Callable5
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4);

	Callable5(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fFunctionPtr(i0, i1, i2, i3, i4); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable6

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable6
:	public ZCallable<R(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5);

	Callable6(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable7

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable7
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6);

	Callable7(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable8

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable8
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7);

	Callable8(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable9

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable9
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable9(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable10

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable10
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable10(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable11

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable11
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable11(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable12

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable12
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable12(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable13

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable13
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable13(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable14

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable14
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable14(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable15

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable15
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable15(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable16

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable16
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef R (*FunctionPtr_t)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable16(FunctionPtr_t iFunctionPtr)
	:	fFunctionPtr(iFunctionPtr)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{ return fFunctionPtr(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF); }

private:
	FunctionPtr_t fFunctionPtr;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallablle

template <class R>
ZRef<ZCallable<R(void)> >
MakeCallable(R (*iFunctionPtr)())
	{ return new Callable0<R>(iFunctionPtr); }

template <class R,
	class P0>
ZRef<ZCallable<R(P0)> >
MakeCallable(R (*iFunctionPtr)(P0))
	{ return new Callable1<R,P0>(iFunctionPtr); }

template <class R,
	class P0, class P1>
ZRef<ZCallable<R(P0,P1)> >
MakeCallable(R (*iFunctionPtr)(P0,P1))
	{ return new Callable2<R,P0,P1>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2>
ZRef<ZCallable<R(P0,P1,P2)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2))
	{ return new Callable3<R,P0,P1,P2>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3>
ZRef<ZCallable<R(P0,P1,P2,P3)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3))
	{ return new Callable4<R,P0,P1,P2,P3>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4))
	{ return new Callable5<R,P0,P1,P2,P3,P4>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5))
	{ return new Callable6<R,P0,P1,P2,P3,P4,P5>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6))
	{ return new Callable7<R,P0,P1,P2,P3,P4,P5,P6>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7))
	{ return new Callable8<R,P0,P1,P2,P3,P4,P5,P6,P7>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8))
	{ return new Callable9<R,P0,P1,P2,P3,P4,P5,P6,P7,P8>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9))
	{ return new Callable10<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA))
	{ return new Callable11<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB))
	{ return new Callable12<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC))
	{ return new Callable13<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD))
	{ return new Callable14<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE))
	{ return new Callable15<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE>(iFunctionPtr); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >
MakeCallable(R (*iFunctionPtr)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF))
	{ return new Callable16<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF>(iFunctionPtr); }

} // namespace ZCallable_Function

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

using ZCallable_Function::MakeCallable;

} // namespace ZooLib

#endif // __ZCallable_Function__
