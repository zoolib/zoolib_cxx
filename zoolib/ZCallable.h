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

#ifndef __ZCallable__
#define __ZCallable__ 1
#include "zconfig.h"

#include "zoolib/ZSafeSet.h"
#include "zoolib/ZWeakRef.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable

class ZCallable
:	public ZCounted
,	public ZWeakReferee
	{};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_R0

template <class R>
class ZCallable_R0
:	public ZCallable
	{
public:
	virtual R Invoke() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_R1

template <class R, class P0>
class ZCallable_R1
:	public ZCallable
	{
public:
	virtual R Invoke(P0) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_R2

template <class R, class P0, class P1>
class ZCallable_R2
:	public ZCallable
	{
public:
	virtual R Invoke(P0, P1) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_R3

template <class R, class P0, class P1, class P2>
class ZCallable_R3
:	public ZCallable
	{
public:
	virtual R Invoke(P0, P1, P2) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_R3

template <class R, class P0, class P1, class P2, class P3>
class ZCallable_R4
:	public ZCallable
	{
public:
	virtual R Invoke(P0, P1, P2, P3 iP3) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_V0

class ZCallable_V0
:	public ZCallable
	{
public:
	virtual void Invoke() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_V1

template <class P0>
class ZCallable_V1
:	public ZCallable
	{
public:
	virtual void Invoke(P0) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_V2

template <class P0, class P1>
class ZCallable_V2
:	public ZCallable
	{
public:
	virtual void Invoke(P0, P1) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_V3

template <class P0, class P1, class P2>
class ZCallable_V3
:	public ZCallable
	{
public:
	virtual void Invoke(P0, P1, P2) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_V4

template <class P0, class P1, class P2, class P3>
class ZCallable_V4
:	public ZCallable
	{
public:
	virtual void Invoke(P0, P1, P2, P3) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet_T0

class ZCallableSet_T0
	{
public:
	typedef ZCallable_V0 Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke()
		{
		for (ZSafeSetIterConst<ZWeakRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ_T<ZWeakRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB.Get()->Invoke();
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZWeakRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet_T1

template <class P0>
class ZCallableSet_T1
	{
public:
	typedef ZCallable_V1<P0> Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke(P0 iP0)
		{
		for (ZSafeSetIterConst<ZWeakRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ_T<ZWeakRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB.Get()->Invoke(iP0);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZWeakRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet_T2

template <class P0, class P1>
class ZCallableSet_T2
	{
public:
	typedef ZCallable_V2<P0, P1> Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke(P0 iP0, P1 iP1)
		{
		for (ZSafeSetIterConst<ZWeakRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ_T<ZWeakRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB.Get()->Invoke(iP0, iP1);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZWeakRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet_T3

template <class P0, class P1, class P2>
class ZCallableSet_T3
	{
public:
	typedef ZCallable_V3<P0, P1, P2> Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke(P0 iP0, P1 iP1, P2 iP2)
		{
		for (ZSafeSetIterConst<ZWeakRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ_T<ZWeakRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB.Get()->Invoke(iP0, iP1, iP2);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZWeakRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet_T4

template <class P0, class P1, class P2, class P3>
class ZCallableSet_T4
	{
public:
	typedef ZCallable_V4<P0, P1, P2, P3> Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke(P0 iP0, P1 iP1, P2 iP2, P3 iP3)
		{
		for (ZSafeSetIterConst<ZWeakRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ_T<ZWeakRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB.Get()->Invoke(iP0, iP1, iP2, iP3);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZWeakRef<Callable_t> > fCallables;
	};

} // namespace ZooLib

#endif // __ZCallable__
