/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZCallable_Fallback_h__
#define __ZCallable_Fallback_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCallable_Fallback

template <class Signature> class ZCallable_Fallback;

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 0 params)

template <class R>
class ZCallable_Fallback<R(void)>
:	public ZCallable<R(void)>
	{
public:
	typedef ZCallable<R(void)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall()
		{
		if (ZQ<R> theQ = fCallable0->QCall())
			return theQ;
		return fCallable1->QCall();
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 1 param)

template <class R,
	class P0>
class ZCallable_Fallback<R(P0)>
:	public ZCallable<R(P0)>
	{
public:
	typedef ZCallable<R(P0)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0))
			return theQ;
		return fCallable1->QCall(i0);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 2 params)

template <class R,
	class P0, class P1>
class ZCallable_Fallback<R(P0,P1)>
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef ZCallable<R(P0,P1)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1))
			return theQ;
		return fCallable1->QCall(i0, i1);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
class ZCallable_Fallback<R(P0,P1,P2)>
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef ZCallable<R(P0,P1,P2)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2))
			return theQ;
		return fCallable1->QCall(i0, i1, i2);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class ZCallable_Fallback<R(P0,P1,P2,P3)>
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4)>
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4, i5))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4, i5, i6))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4, i5, i6, i7))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6, i7);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		if (ZQ<R> theQ = fCallable0->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD))
			return theQ;
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		if (ZQ<R> theQ = fCallable0->QCall
			(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE))
			{ return theQ; }
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - ZCallable_Fallback (specialization for 16 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class ZCallable_Fallback<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> Callable;

	ZCallable_Fallback(const ZRef<Callable>& iCallable0, const ZRef<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		if (ZQ<R> theQ = fCallable0->QCall
			(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF))
			{ return theQ; }
		return fCallable1->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
		}

private:
	const ZRef<Callable> fCallable0;
	const ZRef<Callable> fCallable1;
	};

// =================================================================================================
// MARK: - sCallable_Fallback

template <class Sig>
ZRef<ZCallable<Sig> > sCallable_Fallback
	(const ZRef<ZCallable<Sig> >& iCallable0, const ZRef<ZCallable<Sig> >& iCallable1)
	{
	if (iCallable0)
		{
		if (iCallable1)
			return new ZCallable_Fallback<Sig>(iCallable0, iCallable1);
		return iCallable0;
		}
	return iCallable1;
	}

} // namespace ZooLib

#endif // __ZCallable_Fallback_h__
