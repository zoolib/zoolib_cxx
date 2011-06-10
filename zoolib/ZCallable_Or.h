/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZCallable_Or__
#define __ZCallable_Or__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {
namespace ZCallable_Or {

// =================================================================================================
#pragma mark -
#pragma mark * Callable

template <class Signature> class Callable;

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params)

template <>
class Callable<bool(void)>
:	public ZCallable<bool(void)>
	{
public:
	typedef ZCallable<bool(void)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call()
		{
		if (fCallable0->Call())
			return true;
		return fCallable1->Call();
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 1 param)

template <
	class P0>
class Callable<bool(P0)>
:	public ZCallable<bool(P0)>
	{
public:
	typedef ZCallable<bool(P0)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call(P0 i0)
		{
		if (fCallable0->Call(i0))
			return true;
		return fCallable1->Call(i0);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 2 params)

template <
	class P0, class P1>
class Callable<bool(P0,P1)>
:	public ZCallable<bool(P0,P1)>
	{
public:
	typedef ZCallable<bool(P0,P1)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call(P0 i0, P1 i1)
		{
		if (fCallable0->Call(i0, i1))
			return true;
		return fCallable1->Call(i0, i1);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 3 params)

template <
	class P0, class P1, class P2>
class Callable<bool(P0,P1,P2)>
:	public ZCallable<bool(P0,P1,P2)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call(P0 i0, P1 i1, P2 i2)
		{
		if (fCallable0->Call(i0, i1, i2))
			return true;
		return fCallable1->Call(i0, i1, i2);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 4 params)

template <
	class P0, class P1, class P2, class P3>
class Callable<bool(P0,P1,P2,P3)>
:	public ZCallable<bool(P0,P1,P2,P3)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (fCallable0->Call(i0, i1, i2, i3))
			return true;
		return fCallable1->Call(i0, i1, i2, i3);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params)

template <
	class P0, class P1, class P2, class P3,
	class P4>
class Callable<bool(P0,P1,P2,P3,P4)>
:	public ZCallable<bool(P0,P1,P2,P3,P4)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 6 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable<bool(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 7 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 8 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 9 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 10 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 11 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 12 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 13 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 14 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 15 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 16 params) params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable0, const ZRef<Callable_t>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

	// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		if (fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF))
			return true;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Or

inline
ZRef<ZCallable<bool(void)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(void)> >& iCallable0,
	const ZRef<ZCallable<bool(void)> >& iCallable1)
	{ return new Callable<bool(void)>(iCallable0, iCallable1); }

template <
	class P0>
ZRef<ZCallable<bool(P0)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0)> >& iCallable0,
	const ZRef<ZCallable<bool(P0)> >& iCallable1)
	{ return new Callable<bool(P0)>(iCallable0, iCallable1); }

template <
	class P0, class P1>
ZRef<ZCallable<bool(P0,P1)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1)> >& iCallable1)
	{ return new Callable<bool(P0,P1)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2>
ZRef<ZCallable<bool(P0,P1,P2)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3>
ZRef<ZCallable<bool(P0,P1,P2,P3)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3,P4)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3,P4,P5)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3,P4,P5,P6)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >& iCallable1)
	{ return new Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>(iCallable0, iCallable1); }

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >& iCallable1)
	{
	return new Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>(iCallable0, iCallable1);
	}

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >& iCallable1)
	{
	return new Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>(iCallable0, iCallable1);
	}

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >
MakeCallable_Or(
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >& iCallable0,
	const ZRef<ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >& iCallable1)
	{
	return new Callable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
		(iCallable0, iCallable1);
	}

} // namespace ZCallable_Or

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

using ZCallable_Or::MakeCallable_Or;

} // namespace ZooLib

#endif // __ZCallable_Or__
