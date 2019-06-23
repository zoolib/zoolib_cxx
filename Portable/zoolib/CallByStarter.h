/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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
	const ZP<Promise<ZQ<R> > >& iPromise,
	const ZP<Callable<R()> >& iCallable)
	{ iPromise->Deliver(iCallable->QCall()); }

template <class R>
ZP<Delivery<ZQ<R> > > sQCallByStarter(
	const ZP<Starter>& iStarter,
	const ZP<Callable<R()> >& iCallable)
	{
	ZP<Promise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
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
	const ZP<Promise<ZQ<R> > >& iPromise, \
	const ZP<Callable<R(ZMACRO_Callable_P##X)> >& iCallable, \
	ZMACRO_Callable_Pi##X) \
	{ iPromise->Deliver(iCallable->QCall(ZMACRO_Callable_i##X)); } \
\
template <class R, ZMACRO_Callable_Class_P##X> \
ZP<Delivery<ZQ<R> > > sQCallByStarter( \
	const ZP<Starter>& iStarter, \
	const ZP<Callable<R(ZMACRO_Callable_P##X)> >& iCallable, \
	ZMACRO_Callable_Pi##X) \
	{ \
	ZP<Promise<ZQ<R> > > thePromise = sPromise<ZQ<R> >(); \
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
