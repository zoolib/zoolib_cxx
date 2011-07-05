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

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect

template <class Signature> class ZCallable_Indirect;

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 0 params)

template <class R>
class ZCallable_Indirect<R(void)>
:	public ZCallable<R(void)>
	{
public:
	typedef ZCallable<R(void)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call()
		{
		if (ZRef<Callable> theCallable = fCallable)
			return theCallable->Call();
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 1 param)

template <class R,
	class P0>
class ZCallable_Indirect<R(P0)>
:	public ZCallable<R(P0)>
	{
public:
	typedef ZCallable<R(P0)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0)
		{
		if (ZRef<Callable> theCallable = fCallable)
			return theCallable->Call(i0);
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 2 params)

template <class R,
	class P0, class P1>
class ZCallable_Indirect<R(P0,P1)>
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef ZCallable<R(P0,P1)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1)
		{
		if (ZRef<Callable> theCallable = fCallable)
			return theCallable->Call(i0, i1);
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
class ZCallable_Indirect<R(P0,P1,P2)>
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef ZCallable<R(P0,P1,P2)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2)
		{
		if (ZRef<Callable> theCallable = fCallable)
			return theCallable->Call(i0, i1, i2);
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class ZCallable_Indirect<R(P0,P1,P2,P3)>
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (ZRef<Callable> theCallable = fCallable)
			return theCallable->Call(i0, i1, i2, i3);
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4)>
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (ZRef<Callable> theCallable = fCallable)
			return theCallable->Call(i0, i1, i2, i3, i4);
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		if (ZRef<Callable> theCallable = fCallable)
			return theCallable->Call(i0, i1, i2, i3, i4, i5);
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		if (ZRef<Callable> theCallable = fCallable)
			return theCallable->Call(i0, i1, i2, i3, i4, i5, i6);
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		if (ZRef<Callable> theCallable = fCallable)
			return theCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7);
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		if (ZRef<Callable> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8);
			}
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		if (ZRef<Callable> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
			}
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		if (ZRef<Callable> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
			}
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		if (ZRef<Callable> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
			}
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		if (ZRef<Callable> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
			}
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		if (ZRef<Callable> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
			}
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		if (ZRef<Callable> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
			}
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Indirect (specialization for 16 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class ZCallable_Indirect<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> Callable;

	ZCallable_Indirect(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		if (ZRef<Callable> theCallable = fCallable)
			{
			return theCallable->Call
				(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
			}
		return R();
		}

// Our protocol
	ZRef<Callable> GetSet(const ZRef<Callable>& iCallable)
		{ return fCallable.GetSet(iCallable); }

private:
	ZSafe<ZRef<Callable> > fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCallable_Indirect

template <class Sig>
ZRef<ZCallable<Sig> > sCallable_Indirect(const ZRef<ZCallable<Sig> >& iCallable)
	{ return new ZCallable_Indirect<Sig>(iCallable); }

} // namespace ZooLib

#endif // __ZCallable_Indirect__
