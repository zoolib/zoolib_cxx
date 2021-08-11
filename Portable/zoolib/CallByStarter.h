// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_CallByStarter_h__
#define __ZooLib_CallByStarter_h__ 1
#include "zconfig.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Starter.h"
#include "zoolib/Promise.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sQCallByStarter (specialization for 0 params)

template <class R>
void spQCallWithPromise_T(
	const ZP<Promise<R>>& iPromise,
	const ZP<Callable<R()>>& iCallable)
	{ iPromise->DeliverQRet(iCallable->QCall()); }

template <class R>
ZP<Delivery<R>> sQCallByStarter(
	const ZP<Starter>& iStarter,
	const ZP<Callable<R()>>& iCallable)
	{
	ZP<Promise<R>> thePromise = sPromise<R>();
	if (iStarter && iCallable)
		{
		iStarter->QStart(sBindR(
			sCallable(spQCallWithPromise_T<R>),
			thePromise,
			iCallable));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
#pragma mark - sQCallByStarter variants

#define ZMACRO_Callable_CallByStarter(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
void spQCallWithPromise_T( \
	const ZP<Promise<R>>& iPromise, \
	const ZP<Callable<R(ZMACRO_Callable_P##X)>>& iCallable, \
	ZMACRO_Callable_Pi##X) \
	{ iPromise->DeliverQRet(iCallable->QCall(ZMACRO_Callable_i##X));} \
\
template <class R, ZMACRO_Callable_Class_P##X> \
ZP<Delivery<R>> sQCallByStarter( \
	const ZP<Starter>& iStarter, \
	const ZP<Callable<R(ZMACRO_Callable_P##X)>>& iCallable, \
	ZMACRO_Callable_Pi##X) \
	{ \
	ZP<Promise<R>> thePromise = sPromise<R>(); \
	if (iStarter && iCallable) \
		{ \
		iStarter->QStart(sBindR( \
			sCallable(spQCallWithPromise_T<R,ZMACRO_Callable_P##X>), \
			thePromise, \
			iCallable, \
			ZMACRO_Callable_i##X)); \
		} \
	return thePromise->GetDelivery(); \
	}

ZMACRO_Callable_CallByStarter(0)
ZMACRO_Callable_CallByStarter(1)
ZMACRO_Callable_CallByStarter(2)
ZMACRO_Callable_CallByStarter(3)
ZMACRO_Callable_CallByStarter(4)
ZMACRO_Callable_CallByStarter(5)
ZMACRO_Callable_CallByStarter(6)
ZMACRO_Callable_CallByStarter(7)
ZMACRO_Callable_CallByStarter(8)
ZMACRO_Callable_CallByStarter(9)
ZMACRO_Callable_CallByStarter(A)
ZMACRO_Callable_CallByStarter(B)
ZMACRO_Callable_CallByStarter(C)
ZMACRO_Callable_CallByStarter(D)
ZMACRO_Callable_CallByStarter(E)
ZMACRO_Callable_CallByStarter(F)

#undef ZMACRO_Callable_CallByStarter

} // namespace ZooLib

#endif // __ZooLib_CallByStarter_h__
