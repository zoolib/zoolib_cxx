// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_StarterSync_h__
#define __ZooLib_Callable_StarterSync_h__ 1
#include "zconfig.h"

#include "zoolib/CallByStarter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable_StarterSync

template <class Signature> class Callable_StarterSync;

// =================================================================================================
#pragma mark - Callable_StarterSync (specialization for 0 params)

template <class R>
class Callable_StarterSync<R()>
:	public Callable<R()>
	{
public:
	typedef R (Signature)();

	Callable_StarterSync(const ZP<Starter>& iStarter, const ZP<Callable<Signature>>& iCallable)
	:	fStarter(iStarter)
	,	fCallable(iCallable)
		{}

// From Callable
	virtual QRet<R> QCall()
		{ return sQCallByStarter(fStarter, fCallable)->QGet().Get(); }

private:
	const ZP<Starter> fStarter;
	const ZP<Callable<Signature>> fCallable;
	};

// =================================================================================================
#pragma mark - Callable_StarterSync variants

#define ZMACRO_Callable_Callable(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
class Callable_StarterSync<R(ZMACRO_Callable_P##X)> \
:	public Callable<R(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef R (Signature)(ZMACRO_Callable_P##X); \
\
	Callable_StarterSync(const ZP<Starter>& iStarter, const ZP<Callable<Signature>>& iCallable) \
	:	fStarter(iStarter) \
	,	fCallable(iCallable) \
		{} \
\
	virtual QRet<R> QCall(ZMACRO_Callable_Pi##X) \
		{ \
		return sQCallByStarter<R,ZMACRO_Callable_P##X>(fStarter, fCallable, \
			ZMACRO_Callable_i##X)->QGet().Get(); \
		} \
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
#pragma mark - sCallable_StarterSync

template <class Signature_p>
ZP<Callable<Signature_p>>
sCallable_StarterSync(
	const ZP<Starter>& iStarter, const ZP<Callable<Signature_p>>& iCallable)
	{ return new Callable_StarterSync<Signature_p>(iStarter, iCallable); }

} // namespace ZooLib

#endif // __ZooLib_Callable_Starter_h__
