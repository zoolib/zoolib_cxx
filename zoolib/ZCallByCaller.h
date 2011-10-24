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

#ifndef __ZCallByCaller__
#define __ZCallByCaller__ 1
#include "zconfig.h"

#include "zoolib/ZCaller.h"
#include "zoolib/ZCallable.h"
#include "zoolib/ZCallable_Bind.h"
#include "zoolib/ZCallable_Function.h"
#include "zoolib/ZPromise.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * sCallByCaller

template <class T>
void spCallWithPromise_T
	(const ZRef<ZPromise<ZQ<T> > >& iPromise, const ZRef<ZCallable<T(void)> >& iCallable)
	{ iPromise->Deliver(sQCall(iCallable)); }

template <class T>
ZRef<ZDelivery<ZQ<T> > > sCallByCaller
	(const ZRef<ZCaller>& iCaller, const ZRef<ZCallable<T(void)> >& iCallable)
	{
	ZRef<ZPromise<ZQ<T> > > thePromise = sPromise<ZQ<T> >();
	if (iCaller)
		iCaller->Queue(sBindR(sCallable(spCallWithPromise_T<T>), thePromise, iCallable));
	return thePromise->GetDelivery();
	}

} // namespace ZooLib

#endif // __ZCallByCaller__
