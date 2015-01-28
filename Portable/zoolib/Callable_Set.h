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

#ifndef __ZooLib_Callable_Set_h__
#define __ZooLib_Callable_Set_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/SafeSet.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Callable_Set

template <class Signature> class Callable_Set;

// =================================================================================================
#pragma mark -
#pragma mark Callable_Set (specialization for 0 params)

template <>
class Callable_Set<void(void)>
:	public Callable<void(void)>
,	public SafeSet<ZRef<Callable<void(void)> > >
	{
public:
	typedef Callable<void(void)> Callable_t;

// From Callable
	ZQ<void> QCall()
		{
		for (SafeSetIterConst<ZRef<Callable_t> > iter = *this; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZRef<Callable_t> > theQ = iter.QReadInc())
				sCall(*theQ);
			else
				return notnull;
			}
		}

// Our protocol
	SafeSet<ZRef<Callable_t> >& GetCallables()
		{ return *this; }
	};

// =================================================================================================
#pragma mark -
#pragma mark Callable_Set variants

#define ZMACRO_Callable_Callable(X) \
\
template <ZMACRO_Callable_Class_P##X> \
class Callable_Set<void(ZMACRO_Callable_P##X)> \
:	public Callable<void(ZMACRO_Callable_P##X)> \
,	public SafeSet<ZRef<Callable<void(ZMACRO_Callable_P##X)> > > \
	{ \
public: \
	typedef Callable<void(ZMACRO_Callable_P##X)> Callable_t; \
\
	ZQ<void> QCall(ZMACRO_Callable_Pi##X) \
		{ \
		for (SafeSetIterConst<ZRef<Callable_t> > iter = *this; /*no test*/; /*no inc*/) \
			{ \
			if (ZQ<ZRef<Callable_t> > theQ = iter.QReadInc()) \
				sCall(*theQ, ZMACRO_Callable_i##X); \
			else \
				return notnull; \
			} \
		} \
\
	SafeSet<ZRef<Callable_t> >& GetCallables() \
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

#endif // __ZooLib_Callable_Set_h__
