// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Indirect_h__
#define __ZooLib_Callable_Indirect_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Safe.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable_Indirect

template <class Signature> class Callable_Indirect;

// =================================================================================================
#pragma mark - Callable_Indirect (specialization for 0 params)

template <class R>
class Callable_Indirect<R()>
:	public Callable<R()>
	{
public:
	typedef Callable<R()> Callable_t;

	Callable_Indirect(const ZP<Callable_t>& iCallable)
	:	fSafeCallable(iCallable)
		{}

// From Callable
	virtual QRet<R> QCall()
		{ return sQCall(fSafeCallable.Get()); }

// Our protocol
	ZP<Callable_t> Get()
		{ return fSafeCallable.Get(); }

	void Set(const ZP<Callable_t>& iCallable)
		{ fSafeCallable.Set(iCallable); }

	bool CAS(const ZP<Callable_t>& iPrior, const ZP<Callable_t>& iNew)
		{ return fSafeCallable.CAS(iPrior, iNew); }

private:
	Safe<ZP<Callable_t>> fSafeCallable;
	};

// =================================================================================================
#pragma mark - Callable_Indirect variants

#define ZMACRO_Callable_Callable(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
class Callable_Indirect<R(ZMACRO_Callable_P##X)> \
:	public Callable<R(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef Callable<R(ZMACRO_Callable_P##X)> Callable_t; \
\
	Callable_Indirect(const ZP<Callable_t>& iCallable) \
	:	fSafeCallable(iCallable) \
		{} \
\
	virtual QRet<R> QCall(ZMACRO_Callable_Pi##X) \
		{ return sQCall(fSafeCallable.Get(), ZMACRO_Callable_i##X); } \
\
	ZP<Callable_t> Get() \
		{ return fSafeCallable.Get(); } \
\
	void Set(const ZP<Callable_t>& iCallable) \
		{ fSafeCallable.Set(iCallable); } \
\
	bool CAS(const ZP<Callable_t>& iPrior, const ZP<Callable_t>& iNew) \
		{ return fSafeCallable.CAS(iPrior, iNew); } \
\
private: \
	Safe<ZP<Callable_t>> fSafeCallable; \
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

// =================================================================================================
#pragma mark - sCallable_Indirect

template <class Sig>
ZP<Callable_Indirect<Sig>> sCallable_Indirect(const ZP<Callable<Sig>>& iCallable)
	{ return new Callable_Indirect<Sig>(iCallable); }

template <class Sig>
ZP<Callable_Indirect<Sig>> sCallable_Indirect()
	{ return new Callable_Indirect<Sig>(null); }

} // namespace ZooLib

#endif // __ZooLib_Callable_Indirect_h__
