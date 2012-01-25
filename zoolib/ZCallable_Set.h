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

#ifndef __ZCallable_Set_h__
#define __ZCallable_Set_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZSafeSet.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCallable_Set

template <class Signature> class ZCallable_Set;

// =================================================================================================
// MARK: - ZCallable_Set (specialization for 0 params)

template <>
class ZCallable_Set<void(void)>
:	public ZCallable<void(void)>
	{
public:
	typedef ZCallable<void(void)> Callable;

// From ZCallable
	ZQ<void> QCall()
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(theQ.Get());
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return fCallables; }

private:
	ZSafeSet<ZRef<Callable> > fCallables;
	};

// =================================================================================================
// MARK: - ZCallable_Set (specialization for 1 param)

template <class P0>
class ZCallable_Set<void(P0)>
:	public ZCallable<void(P0)>
	{
public:
	typedef ZCallable<void(P0)> Callable;

// From ZCallable
	ZQ<void> QCall(P0 i0)
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(theQ.Get(), i0);
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return fCallables; }

private:
	ZSafeSet<ZRef<Callable> > fCallables;
	};

// =================================================================================================
// MARK: - ZCallable_Set (specialization for 2 params)

template <class P0, class P1>
class ZCallable_Set<void(P0,P1)>
:	public ZCallable<void(P0,P1)>
	{
public:
	typedef ZCallable<void(P0,P1)> Callable;

// From ZCallable
	ZQ<void> QCall(P0 i0, P1 i1)
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(theQ.Get(), i0, i1);
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return fCallables; }

private:
	ZSafeSet<ZRef<Callable> > fCallables;
	};

// =================================================================================================
// MARK: - ZCallable_Set (specialization for 3 params)

template <class P0, class P1, class P2>
class ZCallable_Set<void(P0,P1,P2)>
:	public ZCallable<void(P0,P1,P2)>
	{
public:
	typedef ZCallable<void(P0,P1,P2)> Callable;

// From ZCallable
	ZQ<void> QCall(P0 i0, P1 i1, P2 i2)
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(theQ.Get(), i0, i1, i2);
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return fCallables; }

private:
	ZSafeSet<ZRef<Callable> > fCallables;
	};

// =================================================================================================
// MARK: - ZCallable_Set (specialization for 4 params)

template <class P0, class P1, class P2, class P3>
class ZCallable_Set<void(P0,P1,P2,P3)>
:	public ZCallable<void(P0,P1,P2,P3)>
	{
public:
	typedef ZCallable<void(P0,P1,P2,P3)> Callable;

// From ZCallable
	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(theQ.Get(), i0, i1, i2, i3);
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return fCallables; }

private:
	ZSafeSet<ZRef<Callable> > fCallables;
	};

// =================================================================================================
// MARK: - ZCallable_Set (specialization for 5 params)

template <class P0, class P1, class P2, class P3, class P4>
class ZCallable_Set<void(P0,P1,P2,P3,P4)>
:	public ZCallable<void(P0,P1,P2,P3,P4)>
	{
public:
	typedef ZCallable<void(P0,P1,P2,P3,P4)> Callable;

// From ZCallable
	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(theQ.Get(), i0, i1, i2, i3, i4);
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return fCallables; }

private:
	ZSafeSet<ZRef<Callable> > fCallables;
	};

// =================================================================================================
// MARK: - ZCallable_Set (specialization for 6 params)

template <class P0, class P1, class P2, class P3, class P4, class P5>
class ZCallable_Set<void(P0,P1,P2,P3,P4,P5)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5)>
	{
public:
	typedef ZCallable<void(P0,P1,P2,P3,P4,P5)> Callable;

// From ZCallable
	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(theQ.Get(), i0, i1, i2, i3, i4, i5);
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return fCallables; }

private:
	ZSafeSet<ZRef<Callable> > fCallables;
	};

// =================================================================================================
// MARK: - ZCallable_Set (specialization for 7 params)

template <class P0, class P1, class P2, class P3, class P4, class P5, class P6>
class ZCallable_Set<void(P0,P1,P2,P3,P4,P5,P6)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6)>
	{
public:
	typedef ZCallable<void(P0,P1,P2,P3,P4,P5,P6)> Callable;

// From ZCallable
	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(theQ.Get(), i0, i1, i2, i3, i4, i5, i6);
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return fCallables; }

private:
	ZSafeSet<ZRef<Callable> > fCallables;
	};

// =================================================================================================
// MARK: - ZCallable_Set (specialization for 8 params)

template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class ZCallable_Set<void(P0,P1,P2,P3,P4,P5,P6,P7)>
:	public ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
public:
	typedef ZCallable<void(P0,P1,P2,P3,P4,P5,P6,P7)> Callable;

// From ZCallable
	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = fCallables; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(theQ.Get(), i0, i1, i2, i3, i4, i5, i6, i7);
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return fCallables; }

private:
	ZSafeSet<ZRef<Callable> > fCallables;
	};

} // namespace ZooLib

#endif // __ZCallable_Set_h__
