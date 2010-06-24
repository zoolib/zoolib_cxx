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

#ifndef __ZWorker_Callable__
#define __ZWorker_Callable__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZWorker.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Callable_T0

class ZWorker_Callable_T0
:	public virtual ZWorker
	{
public:
	typedef ZCallable_R1<bool, ZRef<ZWorker> > Callable_t;

	ZWorker_Callable_T0(ZRef<Callable_t> iCallable)
	:	fCallable(iCallable)
		{}

	virtual bool Work()
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			return fCallable->Invoke(this);
		return false;
		}

private:
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Callable_T1

template <class P0>
class ZWorker_Callable_T1
:	public virtual ZWorker
	{
public:
	typedef ZCallable_R2<bool, ZRef<ZWorker>, const P0&> Callable_t;

	ZWorker_Callable_T1(ZRef<Callable_t> iCallable, const P0& iP0)
	:	fCallable(iCallable)
	,	fP0(iP0)
		{}

	virtual bool Work()
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			return fCallable->Invoke(this, fP0);
		return false;
		}

private:
	ZRef<Callable_t> fCallable;
	const P0 fP0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Callable_T2

template <class P0, class P1>
class ZWorker_Callable_T2
:	public virtual ZWorker
	{
public:
	typedef ZCallable_R3<bool, ZRef<ZWorker>, const P0&, const P1&> Callable_t;

	ZWorker_Callable_T2(ZRef<Callable_t> iCallable, const P0& iP0, const P1& iP1)
	:	fCallable(iCallable)
	,	fP0(iP0)
	,	fP1(iP1)
		{}

	virtual bool Work()
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			return fCallable->Invoke(fP0, fP1);
		return false;
		}

private:
	ZRef<Callable_t> fCallable;
	const P0 fP0;
	const P1 fP1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Callable_T3

template <class P0, class P1, class P2>
class ZWorker_Callable_T3
:	public virtual ZWorker
	{
public:
	typedef ZCallable_R4<bool, ZRef<ZWorker>, const P0&, const P1&, const P2&> Callable_t;

	ZWorker_Callable_T3(ZRef<Callable_t> iCallable, const P0& iP0, const P1& iP1, const P2& iP2)
	:	fCallable(iCallable)
	,	fP0(iP0)
	,	fP1(iP1)
	,	fP2(iP2)
		{}

	virtual bool Work()
		{
		if (ZRef<Callable_t> theCallable = fCallable)
			return fCallable->Invoke(this, fP0, fP1, fP2);
		return false;
		}

private:
	ZRef<Callable_t> fCallable;
	const P0 fP0;
	const P1 fP1;
	const P2 fP2;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Callable_Once_T0

class ZWorker_Callable_Once_T0
:	public ZWorker
	{
public:
	typedef ZCallable_V0 Callable_t;

	ZWorker_Callable_Once_T0(ZRef<Callable_t> iCallable)
	:	fCallable(iCallable)
		{}

	virtual bool Work()
		{ return false; }

	virtual void RunnerDetached()
		{ fCallable->Invoke(); }

private:
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Callable_Once_T1

template <class P0>
class ZWorker_Callable_Once_T1
:	public ZWorker
	{
public:
	typedef ZCallable_V1<const P0&> Callable_t;

	ZWorker_Callable_Once_T1(ZRef<Callable_t> iCallable, const P0& iP0)
	:	fCallable(iCallable)
	,	fP0(iP0)
		{}

	virtual bool Work()
		{ return false; }

	virtual void RunnerDetached()
		{ fCallable->Invoke(fP0); }

private:
	ZRef<Callable_t> fCallable;
	const P0 fP0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Callable_Once_T2

template <class P0, class P1>
class ZWorker_Callable_Once_T2
:	public ZWorker
	{
public:
	typedef ZCallable_V2<const P0&, const P1&> Callable_t;

	ZWorker_Callable_Once_T2(ZRef<Callable_t> iCallable, const P0& iP0, const P1& iP1)
	:	fCallable(iCallable)
	,	fP0(iP0)
	,	fP1(iP1)
		{}

	virtual bool Work()
		{ return false; }

	virtual void RunnerDetached()
		{ fCallable->Invoke(fP0, fP1); }

private:
	ZRef<Callable_t> fCallable;
	const P0 fP0;
	const P1 fP1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Callable_Once_T3

template <class P0, class P1, class P2>
class ZWorker_Callable_Once_T3
:	public ZWorker
	{
public:
	typedef ZCallable_V3<const P0&, const P1&, const P2&> Callable_t;

	ZWorker_Callable_Once_T3(
		ZRef<Callable_t> iCallable, const P0& iP0, const P1& iP1, const P2& iP2)
	:	fCallable(iCallable)
	,	fP0(iP0)
	,	fP1(iP1)
	,	fP2(iP2)
		{}

	virtual bool Work()
		{ return false; }

	virtual void RunnerDetached()
		{ fCallable->Invoke(fP0, fP1, fP2); }

private:
	ZRef<Callable_t> fCallable;
	const P0 fP0;
	const P1 fP1;
	const P2 fP2;
	};

} // namespace ZooLib

#endif // __ZWorker_Callable__
