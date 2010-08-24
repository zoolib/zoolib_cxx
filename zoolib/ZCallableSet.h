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

#ifndef __ZCallableSet__
#define __ZCallableSet__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZSafeSet.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet0

class ZCallableSet0
:	public ZCallable0<void>
	{
public:
	typedef ZCallable0<void> Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke()
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB->Invoke();
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet1

template <class P0>
class ZCallableSet1
:	public ZCallable1<void,P0>
	{
public:
	typedef ZCallable1<void,P0> Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke(P0 i0)
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB->Invoke(i0);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet2

template <class P0, class P1>
class ZCallableSet2
:	public ZCallable2<void,P0,P1>
	{
public:
	typedef ZCallable2<void,P0,P1> Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke(P0 i0, P1 i1)
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB->Invoke(i0, i1);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet3

template <class P0, class P1, class P2>
class ZCallableSet3
:	public ZCallable3<void,P0,P1,P2>
	{
public:
	typedef ZCallable3<void,P0,P1,P2> Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke(P0 i0, P1 i1, P2 i2)
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB->Invoke(i0, i1, i2);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallableSet4

template <class P0, class P1, class P2, class P3>
class ZCallableSet4
:	public ZCallable4<void,P0,P1,P2,P3>
	{
public:
	typedef ZCallable4<void,P0,P1,P2,P3> Callable_t;

	void Register(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Unregister(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Invoke(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables;
			/*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t> > theQ = iter.QReadInc())
				{
				if (ZRef<Callable_t> theCB = theQ.Get())
					theCB->Invoke(i0, i1, i2, i3);
				continue;
				}
			break;
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

} // namespace ZooLib

#endif // __ZCallableSet__
