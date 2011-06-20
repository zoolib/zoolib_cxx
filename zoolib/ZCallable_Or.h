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

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or

template <class Signature> class ZCallable_Or;

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 0 params)

template <>
class ZCallable_Or<bool(void)>
:	public ZCallable<bool(void)>
	{
public:
	typedef ZCallable<bool(void)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call()
		{
		return fCallable0->Call()
			|| fCallable1->Call();
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 1 param)

template <
	class P0>
class ZCallable_Or<bool(P0)>
:	public ZCallable<bool(P0)>
	{
public:
	typedef ZCallable<bool(P0)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call(P0 i0)
		{
		return fCallable0->Call(i0)
			|| fCallable1->Call(i0);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 2 params)

template <
	class P0, class P1>
class ZCallable_Or<bool(P0,P1)>
:	public ZCallable<bool(P0,P1)>
	{
public:
	typedef ZCallable<bool(P0,P1)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call(P0 i0, P1 i1)
		{
		return fCallable0->Call(i0, i1)
			|| fCallable1->Call(i0, i1);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 3 params)

template <
	class P0, class P1, class P2>
class ZCallable_Or<bool(P0,P1,P2)>
:	public ZCallable<bool(P0,P1,P2)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call(P0 i0, P1 i1, P2 i2)
		{
		return fCallable0->Call(i0, i1, i2)
			|| fCallable1->Call(i0, i1, i2);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 4 params)

template <
	class P0, class P1, class P2, class P3>
class ZCallable_Or<bool(P0,P1,P2,P3)>
:	public ZCallable<bool(P0,P1,P2,P3)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		return fCallable0->Call(i0, i1, i2, i3)
			|| fCallable1->Call(i0, i1, i2, i3);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 5 params)

template <
	class P0, class P1, class P2, class P3,
	class P4>
class ZCallable_Or<bool(P0,P1,P2,P3,P4)>
:	public ZCallable<bool(P0,P1,P2,P3,P4)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4)
			|| fCallable1->Call(i0, i1, i2, i3, i4);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 6 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 7 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 8 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 9 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 10 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 11 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 12 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 13 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 14 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 15 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Or (specialization for 16 params)

template <
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class ZCallable_Or<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef ZCallable<bool(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> Callable;

	ZCallable_Or(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual bool Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		return fCallable0->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF)
			|| fCallable1->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
		}

private:
	ZRef<Callable> fCallable0;
	ZRef<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Or

template <class Sig>
ZRef<ZCallable<Sig> > MakeCallable_Or(
	const ZRef<ZCallable<Sig> >& iCallable0, const ZRef<ZCallable<Sig> >& iCallable1)
	{ return new ZCallable_Or<Sig>(iCallable0, iCallable1); }

} // namespace ZooLib

#endif // __ZCallable_Or__
