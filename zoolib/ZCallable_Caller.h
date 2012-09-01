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

#ifndef __ZCallable_Caller_h__
#define __ZCallable_Caller_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallByCaller.h"

namespace ZooLib {
namespace ZCallable_Caller {

// =================================================================================================
// MARK: - Callable

template <class Signature> class Callable;

// =================================================================================================
// MARK: - Callable (specialization for 0 params)

template <class R>
class Callable<R(void)>
:	public ZCallable<R(void)>
	{
public:
	typedef R (Signature)();

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall()
		{
		return sQCallByCaller(fCaller, fCallable)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 1 param)

template <class R,
	class P0>
class Callable<R(P0)>
:	public ZCallable<R(P0)>
	{
public:
	typedef R (Signature)(P0);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 2 params)

template <class R,
	class P0, class P1>
class Callable<R(P0,P1)>
:	public ZCallable<R(P0,P1)>
	{
public:
	typedef R (Signature)(P0,P1);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
class Callable<R(P0,P1,P2)>
:	public ZCallable<R(P0,P1,P2)>
	{
public:
	typedef R (Signature)(P0,P1,P2);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
class Callable<R(P0,P1,P2,P3)>
:	public ZCallable<R(P0,P1,P2,P3)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
class Callable<R(P0,P1,P2,P3,P4)>
:	public ZCallable<R(P0,P1,P2,P3,P4)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
class Callable<R(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
class Callable<R(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6, i7)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

// =================================================================================================
// MARK: - Callable (specialization for 16 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
class Callable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
:	public ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
public:
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	Callable(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	:	fCaller(iCaller)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R> QCall
		(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		return sQCallByCaller(fCaller, fCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF)->QGet().Get();
		}

private:
	const ZRef<ZCaller> fCaller;
	const ZRef<ZCallable<Signature> > fCallable;
	};

} // namespace ZCallable_Caller

// =================================================================================================
// MARK: - sCallable_Caller

template <class Signature>
ZRef<ZCallable<Signature> >
sCallable_Caller
	(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<Signature> >& iCallable)
	{ return new ZCallable_Caller::Callable<Signature>(iCaller, iCallable); }

} // namespace ZooLib

#endif // __ZCallable_Caller_h__
