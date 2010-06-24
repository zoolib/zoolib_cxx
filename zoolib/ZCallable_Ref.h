/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZCallable_Ref__
#define __ZCallable_Ref__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Ref_R0

template <class Callee, class R>
class ZCallable_Ref_R0
:	public ZCallable_R0<R>
	{
public:
	typedef R (Callee::*Method_t)();

	ZCallable_Ref_R0(Method_t iMethod, ZRef<Callee> iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_R0
	virtual R Invoke()
		{ return (fCallee.Get()->*fMethod)(); }

private:
	Method_t fMethod;
	ZRef<Callee> fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Ref_R1

template <class Callee, class R, class P0>
class ZCallable_Ref_R1
:	public ZCallable_R1<R, P0>
	{
public:
	typedef R (Callee::*Method_t)(P0);

	ZCallable_Ref_R1(Method_t iMethod, ZRef<Callee> iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_R1
	virtual R Invoke(P0 iP0)
		{ return (fCallee.Get()->*fMethod)(iP0); }

private:
	Method_t fMethod;
	ZRef<Callee> fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Ref_R2

template <class Callee, class R, class P0, class P1>
class ZCallable_Ref_R2
:	public ZCallable_R2<R, P0, P1>
	{
public:
	typedef R (Callee::*Method_t)(P0, P1);

	ZCallable_Ref_R2(Method_t iMethod, ZRef<Callee> iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_R2
	virtual R Invoke(P0 iP0, P1 iP1)
		{ return (fCallee.Get()->*fMethod)(iP0, iP1); }

private:
	Method_t fMethod;
	ZRef<Callee> fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Ref_R3

template <class Callee, class R, class P0, class P1, class P2>
class ZCallable_Ref_R3
:	public ZCallable_R3<R, P0, P1, P2>
	{
public:
	typedef R (Callee::*Method_t)(P0, P1, P2);

	ZCallable_Ref_R3(Method_t iMethod, ZRef<Callee> iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_R3
	virtual R Invoke(P0 iP0, P1 iP1, P2 iP2)
		{ return (fCallee.Get()->*fMethod)(iP0, iP1, iP2); }

private:
	Method_t fMethod;
	ZRef<Callee> fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Ref_V0

template <class Callee>
class ZCallable_Ref_V0
:	public ZCallable_V0
	{
public:
	typedef void (Callee::*Method_t)();

	ZCallable_Ref_V0(Method_t iMethod, ZRef<Callee> iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_V0
	virtual void Invoke()
		{ (fCallee.Get()->*fMethod)(); }

private:
	Method_t fMethod;
	ZRef<Callee> fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Ref_V1

template <class Callee, class P0>
class ZCallable_Ref_V1
:	public ZCallable_V1<P0>
	{
public:
	typedef void (Callee::*Method_t)(P0);

	ZCallable_Ref_V1(Method_t iMethod, ZRef<Callee> iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_V1
	virtual void Invoke(P0 iP0)
		{ (fCallee.Get()->*fMethod)(iP0); }

private:
	Method_t fMethod;
	ZRef<Callee> fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Ref_V2

template <class Callee, class P0, class P1>
class ZCallable_Ref_V2
:	public ZCallable_V2<P0, P1>
	{
public:
	typedef void (Callee::*Method_t)(P0, P1);

	ZCallable_Ref_V2(Method_t iMethod, ZRef<Callee> iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_V2
	virtual void Invoke(P0 iP0, P1 iP1)
		{ (fCallee.Get()->*fMethod)(iP0, iP1); }

private:
	Method_t fMethod;
	ZRef<Callee> fCallee;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Ref_V3

template <class Callee, class P0, class P1, class P2>
class ZCallable_Ref_V3
:	public ZCallable_V3<P0, P1, P2>
	{
public:
	typedef void (Callee::*Method_t)(P0, P1, P2);

	ZCallable_Ref_V3(Method_t iMethod, ZRef<Callee> iCallee)
	:	fMethod(iMethod)
	,	fCallee(iCallee)
		{}

	// From ZCallable_V3
	virtual void Invoke(P0 iP0, P1 iP1, P2 iP2)
		{ (fCallee.Get()->*fMethod)(iP0, iP1, iP2); }

private:
	Method_t fMethod;
	ZRef<Callee> fCallee;
	};

} // namespace ZooLib

#endif // __ZCallable_Ref__
