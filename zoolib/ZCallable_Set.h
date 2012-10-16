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
,	public ZSafeSet<ZRef<ZCallable<void(void)> > >
	{
public:
	typedef ZCallable<void(void)> Callable;

// From ZCallable
	ZQ<void> QCall()
		{
		for (ZSafeSetIterConst<ZRef<Callable> > iter = *this; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc())
				sCall(*theQ);
			else
				return notnull;
			}
		}

// Our protocol
	ZSafeSet<ZRef<Callable> >& GetCallables()
		{ return *this; }
	};

// =================================================================================================
// MARK: - ZCallable_Set variants

#define ZMACRO_Callable_Callable(X) \
\
template <ZMACRO_Callable_Class_P##X> \
class ZCallable_Set<void(ZMACRO_Callable_P##X)> \
:	public ZCallable<void(ZMACRO_Callable_P##X)> \
,	public ZSafeSet<ZRef<ZCallable<void(ZMACRO_Callable_P##X)> > > \
	{ \
public: \
	typedef ZCallable<void(ZMACRO_Callable_P##X)> Callable; \
\
	ZQ<void> QCall(ZMACRO_Callable_Pi##X) \
		{ \
		for (ZSafeSetIterConst<ZRef<Callable> > iter = *this; /*no test*/; /*no inc*/) \
			{ \
			if (ZQ<ZRef<Callable> > theQ = iter.QReadInc()) \
				sCall(*theQ, ZMACRO_Callable_i##X); \
			else \
				return notnull; \
			} \
		} \
\
	ZSafeSet<ZRef<Callable> >& GetCallables() \
		{ return *this; } \
	};

ZMACRO_Callable_Callable(0)
ZMACRO_Callable_Callable(1)
ZMACRO_Callable_Callable(2)
ZMACRO_Callable_Callable(3)
ZMACRO_Callable_Callable(4)
ZMACRO_Callable_Callable(5)
ZMACRO_Callable_Callable(6)
ZMACRO_Callable_Callable(7)
ZMACRO_Callable_Callable(8)
ZMACRO_Callable_Callable(9)
ZMACRO_Callable_Callable(A)
ZMACRO_Callable_Callable(B)
ZMACRO_Callable_Callable(C)
ZMACRO_Callable_Callable(D)
ZMACRO_Callable_Callable(E)
ZMACRO_Callable_Callable(F)

#undef ZMACRO_Callable_Callable

} // namespace ZooLib

#endif // __ZCallable_Set_h__
