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

#ifndef __ZCallable_And__
#define __ZCallable_And__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {
namespace ZCallable_And {

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
		if (!fCallable0->Call())
			return false;
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
		if (!fCallable0->Call(i0))
			return false;
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
		if (!fCallable0->Call(i0, i1))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE))
			return false;
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
		if (!fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF))
			return false;
		return fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
		}

private:
	ZRef<Callable_t> fCallable0;
	ZRef<Callable_t> fCallable1;
	};

} // namespace ZCallable_And

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_And

template <class Sig>
ZRef<ZCallable<Sig> > MakeCallable_And(
	const ZRef<ZCallable<Sig> >& iCallable0, const ZRef<ZCallable<Sig> >& iCallable1)
	{ return new ZCallable_And::Callable<Sig>(iCallable0, iCallable1); }

} // namespace ZooLib

#endif // __ZCallable_And__
