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

#ifndef __ZCallable_Compound_h__
#define __ZCallable_Compound_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

// sCallable_Apply could be expanded so that with with X+Y params we could pass
// X+1 params to iApply and Y params to iCallable. We'd likely need a revised type
// parameter naming scheme to make it intelligible.

// =================================================================================================
// MARK: - sCallable_Apply (specialization for 0 params)

template <class R0, class P0, class R1>
class ZCallable_Apply_0
:	public ZCallable<R0(void)>
	{
public:
	ZCallable_Apply_0(
		const ZRef<ZCallable<R0(P0)> >& iApply, const ZRef<ZCallable<R1(void)> >& iCallable)
	:	fApply(iApply)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R0> QCall()
		{
		if (const ZQ<R1> theR1 = sQCall(fCallable))
			return sQCall(fApply, *theR1);
		return null;
		}

private:
	const ZRef<ZCallable<R0(P0)> > fApply;
	const ZRef<ZCallable<R1(void)> > fCallable;
	};

template <class R0, class P0, class R1>
ZRef<ZCallable<R0()> >
sCallable_Apply(const ZRef<ZCallable<R0(P0)> >& iApply, const ZRef<ZCallable<R1(void)> >& iCallable)
	{ return new ZCallable_Apply_0<R0,P0,R1>(iApply, iCallable); }

// =================================================================================================
// MARK: - sCallable_Apply (specialization for 1 param)

template <class R0, class P0, class R1, class P1>
class ZCallable_Apply_1
:	public ZCallable<R0(P1)>
	{
public:
	ZCallable_Apply_1(
		const ZRef<ZCallable<R0(P0)> >& iApply, const ZRef<ZCallable<R1(P1)> >& iCallable)
	:	fApply(iApply)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<R0> QCall(P1 iP1)
		{
		if (const ZQ<R1> theR1 = sQCall(fCallable, iP1))
			return sQCall(fApply, *theR1);
		return null;
		}

private:
	const ZRef<ZCallable<R0(P0)> > fApply;
	const ZRef<ZCallable<R1(P1)> > fCallable;
	};

template <class R0, class P0, class R1, class P1>
ZRef<ZCallable<R0(P1)> >
sCallable_Apply(const ZRef<ZCallable<R0(P0)> >& iApply, const ZRef<ZCallable<R1(P1)> >& iCallable)
	{ return new ZCallable_Apply_1<R0,P0,R1,P1>(iApply, iCallable); }

// =================================================================================================
// MARK: - sCallable_Seq

template <class R0, class R1>
class ZCallable_Seq
:	public ZCallable<R1(void)>
	{
public:
	ZCallable_Seq(const ZRef<ZCallable<R0(void)> >& i0, const ZRef<ZCallable<R1(void)> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZCallable
	virtual ZQ<R1> QCall()
		{
		sCall(f0);
		return sQCall(f1);
		}

private:
	const ZRef<ZCallable<R0(void)> > f0;
	const ZRef<ZCallable<R1(void)> > f1;
	};

template <class R0, class R1>
ZRef<ZCallable<R1(void)> >
sCallable_Seq(const ZRef<ZCallable<R0(void)> >& i0, const ZRef<ZCallable<R1(void)> >& i1)
	{ return new ZCallable_Seq<R0,R1>(i0, i1); }

// =================================================================================================
// MARK: - sCallable_For

template <class R_Init, class R_Inc>
class ZCallable_For
:	public ZCallable_Void
	{
public:
	ZCallable_For(const ZRef<ZCallable<R_Init(void)> >& iInit,
		const ZRef<ZCallable_Bool>& iCondition,
		const ZRef<ZCallable<R_Inc(void)> >& iInc)
	:	fInit(iInit)
	,	fCondition(iCondition)
	,	fInc(iInc)
		{}

// From ZCallable
	virtual ZQ<void> QCall()
		{
		for (sCall(fInit); sCall(fCondition); sCall(fInc))
			{}
		return notnull;
		}

private:
	const ZRef<ZCallable<R_Init(void)> > fInit;
	const ZRef<ZCallable_Bool>& fCondition;
	const ZRef<ZCallable<R_Inc(void)> >& fInc;
	};

template <class R_Init, class R_Inc>
ZRef<ZCallable_Void> sCallable_For(const ZRef<ZCallable<R_Init(void)> >& iInit,
	const ZRef<ZCallable_Bool>& iCondition,
	const ZRef<ZCallable<R_Inc(void)> >& iInc)
	{ return new ZCallable_For<R_Init,R_Inc>(iInit, iCondition, iInc); }

// =================================================================================================
// MARK: - sCallable_If

template <class R>
class ZCallable_If
:	public ZCallable<R(void)>
	{
public:
	ZCallable_If(const ZRef<ZCallable_Bool>& iCondition,
		const ZRef<ZCallable<R(void)> >& i0, const ZRef<ZCallable<R(void)> >& i1)
	:	fCondition(iCondition)
	,	f0(i0)
	,	f1(i1)
		{}

// From ZCallable
	virtual ZQ<R> QCall()
		{
		if (const ZQ<bool> theQ = sQCall(fCondition))
			{
			if (*theQ)
				return sQCall(f0);
			else
				return sQCall(f1);
			}
		return null;
		}

private:
	const ZRef<ZCallable_Bool> fCondition;
	const ZRef<ZCallable<R(void)> > f0;
	const ZRef<ZCallable<R(void)> > f1;
	};

template <class R>
ZRef<ZCallable<R(void)> > sCallable_If(const ZRef<ZCallable_Bool>& iCondition,
	const ZRef<ZCallable<R(void)> >& i0, const ZRef<ZCallable<R(void)> >& i1)
	{ return new ZCallable_If<R>(iCondition, i0, i1); }

// =================================================================================================
// MARK: - sCallable_Repeat

template <class R>
class ZCallable_Repeat
:	public ZCallable_Void
	{
public:
	ZCallable_Repeat(size_t iCount, const ZRef<ZCallable<R(void)> >& iCallable)
	:	fCount(iCount)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<void> QCall()
		{
		if (fCallable)
			{
			for (size_t theCount = fCount; theCount--; /*no inc*/)
				fCallable->Call();
			return notnull;
			}
		return null;
		}

private:
	const size_t fCount;
	const ZRef<ZCallable<R(void)> > fCallable;
	};

template <class R>
ZRef<ZCallable_Void> sCallable_Repeat(size_t iCount, const ZRef<ZCallable<R(void)> >& iCallable)
	{ return new ZCallable_Repeat<R>(iCount, iCallable); }

// =================================================================================================
// MARK: - sCallable_While

template <class R>
class ZCallable_While
:	public ZCallable_Void
	{
public:
	ZCallable_While(
		const ZRef<ZCallable_Bool>& iCondition, const ZRef<ZCallable<R(void)> >& iCallable)
	:	fCondition(iCondition)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<void> QCall()
		{
		while (sCall(fCondition))
			sCall(fCallable);
		return notnull;
		}

private:
	const ZRef<ZCallable_Bool> fCondition;
	const ZRef<ZCallable<R(void)> > fCallable;
	};

template <class R>
ZRef<ZCallable_Void> sCallable_While(
	const ZRef<ZCallable_Bool>& iCondition, const ZRef<ZCallable<R(void)> >& iCallable)
	{ return new ZCallable_While<R>(iCondition, iCallable); }

inline
ZRef<ZCallable_Void> sCallable_While(const ZRef<ZCallable_Bool>& iCallable)
	{ return new ZCallable_While<void>(iCallable, null); }

} // namespace ZooLib

#endif // __ZCallable_Compound_h__
