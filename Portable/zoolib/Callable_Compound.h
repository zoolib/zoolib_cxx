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

#ifndef __ZooLib_Callable_Compound_h__
#define __ZooLib_Callable_Compound_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

// sCallable_Apply could be expanded so that with with X+Y params we could pass
// X+1 params to iApply and Y params to iCallable. We'd likely need a revised type
// parameter naming scheme to make it intelligible.

// =================================================================================================
#pragma mark - sCallable_Apply (specialization for 0 params)

template <class R0, class P0, class R1>
class Callable_Apply_0
:	public Callable<R0()>
	{
public:
	Callable_Apply_0(
		const ZP<Callable<R0(P0)>>& iApply, const ZP<Callable<R1()>>& iCallable)
	:	fApply(iApply)
	,	fCallable(iCallable)
		{}

// From Callable
	virtual QRet<R0> QCall()
		{
		if (const QRet<R1> theR1 = sQCall(fCallable))
			return sQCall(fApply, *theR1);
		return QRet<R0>();
		}

private:
	const ZP<Callable<R0(P0)>> fApply;
	const ZP<Callable<R1()>> fCallable;
	};

template <class R0, class P0, class R1>
ZP<Callable<R0()>>
sCallable_Apply(const ZP<Callable<R0(P0)>>& iApply, const ZP<Callable<R1()>>& iCallable)
	{ return new Callable_Apply_0<R0,P0,R1>(iApply, iCallable); }

// =================================================================================================
#pragma mark - sCallable_Apply (specialization for 1 param)

template <class R0, class P0, class R1, class P1>
class Callable_Apply_1
:	public Callable<R0(P1)>
	{
public:
	Callable_Apply_1(
		const ZP<Callable<R0(P0)>>& iApply, const ZP<Callable<R1(P1)>>& iCallable)
	:	fApply(iApply)
	,	fCallable(iCallable)
		{}

// From Callable
	virtual QRet<R0> QCall(P1 iP1)
		{
		if (const QRet<R1> theR1 = sQCall(fCallable, iP1))
			return sQCall(fApply, *theR1);
		return QRet<R0>();
		}

private:
	const ZP<Callable<R0(P0)>> fApply;
	const ZP<Callable<R1(P1)>> fCallable;
	};

template <class R0, class P0, class R1, class P1>
ZP<Callable<R0(P1)>>
sCallable_Apply(const ZP<Callable<R0(P0)>>& iApply, const ZP<Callable<R1(P1)>>& iCallable)
	{ return new Callable_Apply_1<R0,P0,R1,P1>(iApply, iCallable); }

// =================================================================================================
#pragma mark - sCallable_Seq

template <class R0, class R1>
class Callable_Seq
:	public Callable<R1()>
	{
public:
	Callable_Seq(const ZP<Callable<R0()>>& i0, const ZP<Callable<R1()>>& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From Callable
	virtual QRet<R1> QCall()
		{
		sCall(f0);
		return sQCall(f1);
		}

private:
	const ZP<Callable<R0()>> f0;
	const ZP<Callable<R1()>> f1;
	};

template <class R0, class R1>
ZP<Callable<R1()>>
sCallable_Seq(const ZP<Callable<R0()>>& i0, const ZP<Callable<R1()>>& i1)
	{ return new Callable_Seq<R0,R1>(i0, i1); }

// =================================================================================================
#pragma mark - sCallable_For

template <class R_Init, class R_Inc>
class Callable_For
:	public Callable_Void
	{
public:
	Callable_For(const ZP<Callable<R_Init()>>& iInit,
		const ZP<Callable_Bool>& iCondition,
		const ZP<Callable<R_Inc()>>& iInc)
	:	fInit(iInit)
	,	fCondition(iCondition)
	,	fInc(iInc)
		{}

// From Callable
	virtual bool QCall()
		{
		for (sCall(fInit); sCall(fCondition); sCall(fInc))
			{}
		return true;
		}

private:
	const ZP<Callable<R_Init()>> fInit;
	const ZP<Callable_Bool>& fCondition;
	const ZP<Callable<R_Inc()>>& fInc;
	};

template <class R_Init, class R_Inc>
ZP<Callable_Void> sCallable_For(const ZP<Callable<R_Init()>>& iInit,
	const ZP<Callable_Bool>& iCondition,
	const ZP<Callable<R_Inc()>>& iInc)
	{ return new Callable_For<R_Init,R_Inc>(iInit, iCondition, iInc); }

// =================================================================================================
#pragma mark - sCallable_If

template <class R>
class Callable_If
:	public Callable<R()>
	{
public:
	Callable_If(const ZP<Callable_Bool>& iCondition,
		const ZP<Callable<R()>>& i0, const ZP<Callable<R()>>& i1)
	:	fCondition(iCondition)
	,	f0(i0)
	,	f1(i1)
		{}

// From Callable
	virtual QRet<R> QCall()
		{
		if (const QRet<bool> theQ = sQCall(fCondition))
			{
			if (*theQ)
				return sQCall(f0);
			else
				return sQCall(f1);
			}
		return QRet<R>();
		}

private:
	const ZP<Callable_Bool> fCondition;
	const ZP<Callable<R()>> f0;
	const ZP<Callable<R()>> f1;
	};

template <class R>
ZP<Callable<R()>> sCallable_If(const ZP<Callable_Bool>& iCondition,
	const ZP<Callable<R()>>& i0, const ZP<Callable<R()>>& i1)
	{ return new Callable_If<R>(iCondition, i0, i1); }

// =================================================================================================
#pragma mark - sCallable_Repeat

template <class R>
class Callable_Repeat
:	public Callable_Void
	{
public:
	Callable_Repeat(size_t iCount, const ZP<Callable<R()>>& iCallable)
	:	fCount(iCount)
	,	fCallable(iCallable)
		{}

// From Callable
	virtual bool QCall()
		{
		if (fCallable)
			{
			for (size_t theCount = fCount; theCount--; /*no inc*/)
				fCallable->Call();
			return true;
			}
		return false;
		}

private:
	const size_t fCount;
	const ZP<Callable<R()>> fCallable;
	};

template <class R>
ZP<Callable_Void> sCallable_Repeat(size_t iCount, const ZP<Callable<R()>>& iCallable)
	{ return new Callable_Repeat<R>(iCount, iCallable); }

// =================================================================================================
#pragma mark - sCallable_While

template <class R>
class Callable_While
:	public Callable_Void
	{
public:
	Callable_While(
		const ZP<Callable_Bool>& iCondition, const ZP<Callable<R()>>& iCallable)
	:	fCondition(iCondition)
	,	fCallable(iCallable)
		{}

// From Callable
	virtual bool QCall()
		{
		while (sCall(fCondition))
			sCall(fCallable);
		return true;
		}

private:
	const ZP<Callable_Bool> fCondition;
	const ZP<Callable<R()>> fCallable;
	};

template <class R>
ZP<Callable_Void> sCallable_While(
	const ZP<Callable_Bool>& iCondition, const ZP<Callable<R()>>& iCallable)
	{ return new Callable_While<R>(iCondition, iCallable); }

inline
ZP<Callable_Void> sCallable_While(const ZP<Callable_Bool>& iCallable)
	{ return new Callable_While<void>(iCallable, null); }

} // namespace ZooLib

#endif // __ZooLib_Callable_Compound_h__
