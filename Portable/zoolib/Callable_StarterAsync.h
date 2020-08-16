// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_StarterAsync_h__
#define __ZooLib_Callable_StarterAsync_h__ 1
#include "zconfig.h"

#include "zoolib/Starter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable_StarterAsync

template <class Signature> class Callable_StarterAsync;

// =================================================================================================
#pragma mark - Callable_StarterAsync (specialization for 0 params)

template <>
class Callable_StarterAsync<void()>
:	public Callable<void()>
	{
public:
	typedef void (Signature)();

	Callable_StarterAsync(const ZP<Starter>& iStarter, const ZP<Callable<Signature>>& iCallable)
	:	fStarter(iStarter)
	,	fCallable(iCallable)
		{}

// From Callable
	virtual bool QCall()
		{ return sQStart(fStarter, fCallable); }

private:
	const ZP<Starter> fStarter;
	const ZP<Callable<Signature>> fCallable;
	};

// =================================================================================================
#pragma mark - Callable_StarterAsync variants

#define ZMACRO_Callable_Callable(X) \
\
template <ZMACRO_Callable_Class_P##X> \
class Callable_StarterAsync<void(ZMACRO_Callable_P##X)> \
:	public Callable<void(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef void (Signature)(ZMACRO_Callable_P##X); \
\
	Callable_StarterAsync(const ZP<Starter>& iStarter, const ZP<Callable<Signature>>& iCallable) \
	:	fStarter(iStarter) \
	,	fCallable(iCallable) \
		{} \
\
	virtual bool QCall(ZMACRO_Callable_Pi##X) \
		{ return sQStart(fStarter, sBindR(fCallable, ZMACRO_Callable_i##X)); } \
\
private:\
	const ZP<Starter> fStarter; \
	const ZP<Callable<Signature>> fCallable;\
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
#pragma mark - sCallable_StarterAsync

#if ZCONFIG_CPP >= 2011

template <typename... A_p>
ZP<Callable<void(A_p...)>>
sCallable_StarterAsync(
	const ZP<Starter>& iStarter, const ZP<Callable<void(A_p...)>>& iCallable)
	{ return new Callable_StarterAsync<void(A_p...)>(iStarter, iCallable); }

#else

template <class Signature_p>
ZP<Callable<Signature_p>>
sCallable_StarterAsync(
	const ZP<Starter>& iStarter, const ZP<Callable<Signature_p>>& iCallable)
	{ return new Callable_StarterAsync<Signature_p>(iStarter, iCallable); }

#endif // ZCONFIG_CPP >= 2011

} // namespace ZooLib

#endif // __ZooLib_Callable_StarterAsync_h__
