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

#ifndef __ZCallByCaller_h__
#define __ZCallByCaller_h__ 1
#include "zconfig.h"

#include "zoolib/ZCaller.h"
#include "zoolib/ZCallable_Bind.h"
#include "zoolib/ZCallable_Function.h"
#include "zoolib/ZPromise.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 0 params)

template <class R>
void spQCallWithPromise_T(
	const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(void)> >& iCallable)
	{ iPromise->Deliver(iCallable->QCall()); }

template <class R>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller(
	const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(void)> >& iCallable)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR(
			sCallable(spQCallWithPromise_T<R>),
			thePromise,
			iCallable));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller variants

#define ZMACRO_Callable_CallByCaller(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
void spQCallWithPromise_T( \
	const ZRef<ZPromise<ZQ<R> > >& iPromise, \
	const ZRef<ZCallable<R(ZMACRO_Callable_P##X)> >& iCallable, \
	ZMACRO_Callable_Pi##X) \
	{ iPromise->Deliver(iCallable->QCall(ZMACRO_Callable_i##X)); } \
\
template <class R, ZMACRO_Callable_Class_P##X> \
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller( \
	const ZRef<ZCaller>& iCaller, \
	const ZRef<ZCallable<R(ZMACRO_Callable_P##X)> >& iCallable, \
	ZMACRO_Callable_Pi##X) \
	{ \
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >(); \
	if (iCaller && iCallable) \
		{ \
		iCaller->Queue(sBindR( \
			sCallable(spQCallWithPromise_T<R,ZMACRO_Callable_P##X>), \
			thePromise, \
			iCallable, \
			ZMACRO_Callable_i##X)); \
		} \
	return thePromise->GetDelivery(); \
	}

ZMACRO_Callable_CallByCaller(0)
ZMACRO_Callable_CallByCaller(1)
ZMACRO_Callable_CallByCaller(2)
ZMACRO_Callable_CallByCaller(3)
ZMACRO_Callable_CallByCaller(4)
ZMACRO_Callable_CallByCaller(5)
ZMACRO_Callable_CallByCaller(6)
ZMACRO_Callable_CallByCaller(7)
ZMACRO_Callable_CallByCaller(8)
ZMACRO_Callable_CallByCaller(9)
ZMACRO_Callable_CallByCaller(A)
ZMACRO_Callable_CallByCaller(B)
ZMACRO_Callable_CallByCaller(C)
ZMACRO_Callable_CallByCaller(D)
ZMACRO_Callable_CallByCaller(E)
ZMACRO_Callable_CallByCaller(F)

#undef ZMACRO_Callable_CallByCaller

} // namespace ZooLib

#endif // __ZCallByCaller_h__
