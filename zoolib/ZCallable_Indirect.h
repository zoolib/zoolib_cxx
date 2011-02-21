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

#ifndef __ZCallable_Indirect__
#define __ZCallable_Indirect__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZSafe.h"

namespace ZooLib {
namespace ZCallable_Indirect {

// =================================================================================================
#pragma mark -
#pragma mark * Callable

template <class Signature> class Callable;

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 0 params)

template <class R>
class Callable<R(void)>
:	public ZCallable<R(void)>
	{
public:
	typedef ZCallable<R(void)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call()
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				();
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 1 param)

template <class R,
	class P0>
class Callable<R(P0)>
:	public ZCallable<R(P0)>
	{
public:
	typedef ZCallable<R(P0)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call(P0 i0)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 2 params)

template <class R,
	class P0, class P1>
class Callable<R(P0,P1)>
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef ZCallable<R(P0,P1)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
class Callable<R(P0,P1,P2)>
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef ZCallable<R(P0,P1,P2)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class Callable<R(P0,P1,P2,P3)>
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
class Callable<R(P0,P1,P2,P3,P4)>
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable<R(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable<R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable (specialization for 16 params) params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> Callable_t;

	Callable(const ZRef<Callable_t>& iCallable)
	:	fCallable(iCallable)
		{}

	// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
			}
		return R();
		}

	// Our protocol
	ZRef<Callable_t> GetSet(const ZRef<Callable_t>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable_t> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable_Indirect

template <class R>
ZRef<ZCallable<R(void)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(void)> >& iCallable)
	{ return new Callable<R(void)>(iCallable); }

template <class R,
	class P0>
ZRef<ZCallable<R(P0)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0)> >& iCallable)
	{ return new Callable<R(P0)>(iCallable); }

template <class R,
	class P0, class P1>
ZRef<ZCallable<R(P0,P1)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1)> >& iCallable)
	{ return new Callable<R(P0,P1)>(iCallable); }

template <class R,
	class P0, class P1, class P2>
ZRef<ZCallable<R(P0,P1,P2)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2)> >& iCallable)
	{ return new Callable<R(P0,P1,P2)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3>
ZRef<ZCallable<R(P0,P1,P2,P3)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>(iCallable); }

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >
MakeCallable_Indirect(
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >& iCallable)
	{ return new Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>(iCallable); }

} // namespace ZCallable_Indirect

// =================================================================================================
#pragma mark -
#pragma mark * MakeCallable

using ZCallable_Indirect::MakeCallable_Indirect;

} // namespace ZooLib

#endif // __ZCallable_Indirect__
