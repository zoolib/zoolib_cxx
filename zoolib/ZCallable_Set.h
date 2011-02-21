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

#ifndef __ZCallable_Set__
#define __ZCallable_Set__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZSafeSet.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Set

template <class Signature> class ZCallable_Set;

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Set (specialization for 0 params)

template <>
class ZCallable_Set<void(void)>
:	public ZCallable<void(void)>
	{
public:
	typedef ZCallable<void(void)> Callable_t;

	void Insert(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Erase(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Call()
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t>,false> theQ = iter.QReadInc())
				break;
			else if (ZRef<Callable_t> theCB = theQ.Get())
				theCB->Call();
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Set (specialization for 1 param)

template <class P0>
class ZCallable_Set<void(P0)>
:	public ZCallable<void(P0)>
	{
public:
	typedef ZCallable<void(P0)> Callable_t;

	void Insert(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Erase(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Call(P0 i0)
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t>,false> theQ = iter.QReadInc())
				break;
			else if (ZRef<Callable_t> theCB = theQ.Get())
				theCB->Call(i0);
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Set (specialization for 2 params)

template <class P0, class P1>
class ZCallable_Set<void(P0,P1)>
:	public ZCallable<void(P0,P1)>
	{
public:
	typedef ZCallable<void(P0,P1)> Callable_t;

	void Insert(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Erase(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Call(P0 i0, P1 i1)
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t>,false> theQ = iter.QReadInc())
				break;
			else if (ZRef<Callable_t> theCB = theQ.Get())
				theCB->Call(i0, i1);
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Set (specialization for 3 params)

template <class P0, class P1, class P2>
class ZCallable_Set<void(P0,P1,P2)>
:	public ZCallable<void(P0,P1,P2)>
	{
public:
	typedef ZCallable<void(P0,P1,P2)> Callable_t;

	void Insert(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Erase(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Call(P0 i0, P1 i1, P2 i2)
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t>,false> theQ = iter.QReadInc())
				break;
			else if (ZRef<Callable_t> theCB = theQ.Get())
				theCB->Call(i0, i1, i2);
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Set (specialization for 4 params)

template <class P0, class P1, class P2, class P3>
class ZCallable_Set<void(P0,P1,P2,P3)>
:	public ZCallable<void(P0,P1,P2,P3)>
	{
public:
	typedef ZCallable<void(P0,P1,P2,P3)> Callable_t;

	void Insert(ZRef<Callable_t> iCallable)
		{ fCallables.Insert(iCallable); }

	void Erase(ZRef<Callable_t> iCallable)
		{ fCallables.Erase(iCallable); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		for (ZSafeSetIterConst<ZRef<Callable_t> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t>,false> theQ = iter.QReadInc())
				break;
			else if (ZRef<Callable_t> theCB = theQ.Get())
				theCB->Call(i0, i1, i2, i3);
			}
		}

private:
	ZSafeSet<ZRef<Callable_t> > fCallables;
	};

} // namespace ZooLib

#endif // __ZCallable_Set__
