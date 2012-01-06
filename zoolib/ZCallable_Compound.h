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

// =================================================================================================
#pragma mark -
#pragma mark * sCallable_Apply

template <class A, class B, class C>
class ZCallable_Apply
:	public ZCallable<A(C)>
	{
public:
	ZCallable_Apply(const ZRef<ZCallable<A(B)> >& iApply, const ZRef<ZCallable<B(C)> >& iCallable)
	:	fApply(iApply)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual ZQ<A> QCall(C iC)
		{
		if (ZQ<B> theB = sQCall(fCallable, iC))
			return sQCall(fApply, theB.Get());
		return null;
		}

private:
	const ZRef<ZCallable<A(B)> > fApply;
	const ZRef<ZCallable<B(C)> > fCallable;
	};

template <class A, class B, class C>
ZRef<ZCallable<A(C)> >
sCallable_Apply(const ZRef<ZCallable<A(B)> >& iApply, const ZRef<ZCallable<B(C)> >& iCallable)
	{ return new ZCallable_Apply<A,B,C>(iApply, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * sCallable_Seq

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
#pragma mark -
#pragma mark * sCallable_For

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
#pragma mark -
#pragma mark * sCallable_If

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
		if (ZQ<bool> theQ = sQCall(fCondition))
			{
			if (theQ.Get())
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
#pragma mark -
#pragma mark * sCallable_Repeat

template<class R>
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
#pragma mark -
#pragma mark * sCallable_While

template <class R>
class ZCallable_While
:	public ZCallable_Void
	{
public:
	ZCallable_While
		(const ZRef<ZCallable_Bool>& iCondition, const ZRef<ZCallable<R(void)> >& iCallable)
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
ZRef<ZCallable_Void> sCallable_While
	(const ZRef<ZCallable_Bool>& iCondition, const ZRef<ZCallable<R(void)> >& iCallable)
	{ return new ZCallable_While<R>(iCondition, iCallable); }

inline
ZRef<ZCallable<void(void)> > sCallable_While(const ZRef<ZCallable_Bool>& iCallable)
	{ return new ZCallable_While<void>(iCallable, null); }

} // namespace ZooLib

#endif // __ZCallable_Compound_h__
