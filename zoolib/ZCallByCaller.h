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
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(void)> >& iCallable)
	{
	iPromise->Deliver(iCallable->QCall());
	}

template <class R>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(void)> >& iCallable)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R>),
			thePromise,
			iCallable));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 1 param)

template <class R,
	class P0>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0)> >& iCallable,
	P0 i0)
	{
	iPromise->Deliver(iCallable->QCall
		(i0));
	}

template <class R,
	class P0>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0)> >& iCallable,
	P0 i0)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0>),
			thePromise,
			iCallable,
			i0));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 2 params)

template <class R,
	class P0, class P1>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1)> >& iCallable,
	P0 i0, P1 i1)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1));
	}

template <class R,
	class P0, class P1>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1)> >& iCallable,
	P0 i0, P1 i1)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1>),
			thePromise,
			iCallable,
			i0, i1));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 3 params)

template <class R,
	class P0, class P1, class P2>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2)> >& iCallable,
	P0 i0, P1 i1, P2 i2)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2));
	}

template <class R,
	class P0, class P1, class P2>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2)> >& iCallable,
	P0 i0, P1 i1, P2 i2)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2>),
			thePromise,
			iCallable,
			i0, i1, i2));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 4 params)

template <class R,
	class P0, class P1, class P2, class P3>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3));
	}

template <class R,
	class P0, class P1, class P2, class P3>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3>),
			thePromise,
			iCallable,
			i0, i1, i2, i3));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 5 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 6 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 7 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 8 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6, i7));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6,P7>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6, i7));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 9 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6, i7, i8));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6,P7,P8>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 10 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 11 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 12 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 13 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB, PC iC)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB, PC iC)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 14 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 15 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE));
		}
	return thePromise->GetDelivery();
	}

// =================================================================================================
// MARK: - sQCallByCaller (specialization for 16 params)

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
void spQCallWithPromise_T
	(const ZRef<ZPromise<ZQ<R> > >& iPromise,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
	{
	iPromise->Deliver(iCallable->QCall
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF));
	}

template <class R,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
ZRef<ZDelivery<ZQ<R> > > sQCallByCaller
	(const ZRef<ZCaller>& iCaller,
	const ZRef<ZCallable<R(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)> >& iCallable,
	P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
	P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
	{
	ZRef<ZPromise<ZQ<R> > > thePromise = sPromise<ZQ<R> >();
	if (iCaller && iCallable)
		{
		iCaller->Queue(sBindR
			(sCallable(spQCallWithPromise_T<R,P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF>),
			thePromise,
			iCallable,
			i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF));
		}
	return thePromise->GetDelivery();
	}

} // namespace ZooLib

#endif // __ZCallByCaller_h__
