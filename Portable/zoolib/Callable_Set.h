// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Set_h__
#define __ZooLib_Callable_Set_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/SafeSet.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable_Set

template <class Signature> class Callable_Set;

// =================================================================================================
#pragma mark - Callable_Set (specialization for 0 params)

template <>
class Callable_Set<void()>
:	public Callable<void()>
,	public SafeSet<ZP<Callable<void()>>>
	{
public:
	typedef Callable<void()> Callable_t;

// From Callable
	bool QCall()
		{
		for (SafeSetIterConst<ZP<Callable_t>> iter = *this; /*no test*/; /*no inc*/)
			{
			if (ZQ<ZP<Callable_t>> theQ = iter.QReadInc())
				sCall(*theQ);
			else
				return true;
			}
		}

// Our protocol
	SafeSet<ZP<Callable_t>>& GetCallables()
		{ return *this; }
	};

// =================================================================================================
#pragma mark - Callable_Set variants

#define ZMACRO_Callable_Callable(X) \
\
template <ZMACRO_Callable_Class_P##X> \
class Callable_Set<void(ZMACRO_Callable_P##X)> \
:	public Callable<void(ZMACRO_Callable_P##X)> \
,	public SafeSet<ZP<Callable<void(ZMACRO_Callable_P##X)>>> \
	{ \
public: \
	typedef Callable<void(ZMACRO_Callable_P##X)> Callable_t; \
\
	bool QCall(ZMACRO_Callable_Pi##X) \
		{ \
		for (SafeSetIterConst<ZP<Callable_t>> iter = *this; /*no test*/; /*no inc*/) \
			{ \
			if (ZQ<ZP<Callable_t>> theQ = iter.QReadInc()) \
				sCall(*theQ, ZMACRO_Callable_i##X); \
			else \
				return true; \
			} \
		} \
\
	SafeSet<ZP<Callable_t>>& GetCallables() \
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
