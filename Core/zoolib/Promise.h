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

#ifndef __ZooLib_Promise_h__
#define __ZooLib_Promise_h__ 1
#include "zconfig.h"

#include "zoolib/CountedWithoutFinalize.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

template <class T> class Promise;

// =================================================================================================
#pragma mark -
#pragma mark Delivery

template <class T>
class Delivery
:	public CountedWithoutFinalize
	{
	Delivery()
	:	fPromiseExists(true)
		{}

public:
	virtual ~Delivery()
		{}

	void Wait()
		{
		ZAcqMtx acq(fMtx);
		while (fPromiseExists && not fValQ)
			fCnd.Wait(fMtx);
		}

	bool WaitFor(double iTimeout)
		{
		ZAcqMtx acq(fMtx);

		if (fValQ || not fPromiseExists)
			return true;

		fCnd.WaitFor(fMtx, iTimeout);

		return fValQ || not fPromiseExists;
		}

	bool WaitUntil(double iDeadline)
		{
		ZAcqMtx acq(fMtx);

		if (fValQ || not fPromiseExists)
			return true;

		fCnd.WaitUntil(fMtx, iDeadline);

		return fValQ || not fPromiseExists;
		}

	ZQ<T> QGet()
		{
		ZAcqMtx acq(fMtx);
		while (fPromiseExists && not fValQ)
			fCnd.Wait(fMtx);
		return fValQ;
		}

private:
	friend class Promise<T>;
	ZMtx fMtx;
	ZCnd fCnd;
	bool fPromiseExists;
	ZQ<T> fValQ;
	};

// =================================================================================================
#pragma mark -
#pragma mark Promise

template <class T = void>
class Promise
:	public CountedWithoutFinalize
	{
public:
	Promise()
	:	fDelivery(new Delivery<T>)
		{}

	virtual ~Promise()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		fDelivery->fPromiseExists = false;
		fDelivery->fCnd.Broadcast();
		}

	bool IsDelivered()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		return fDelivery->fValQ;
		}

	void Deliver(const T& iVal)
		{
		ZAcqMtx acq(fDelivery->fMtx);
		fDelivery->fValQ.Set(iVal);
		fDelivery->fCnd.Broadcast();
		}

	bool QDeliver(const T& iVal)
		{
		ZAcqMtx acq(fDelivery->fMtx);
		if (not fDelivery->fValQ.QSet(iVal))
			return false;
		fDelivery->fCnd.Broadcast();
		return true;
		}

	ZRef<Delivery<T> > GetDelivery()
		{ return fDelivery; }

private:
	ZRef<Delivery<T> > fDelivery;
	};

// =================================================================================================
#pragma mark -
#pragma mark Promise<void>

template <>
class Promise<void>
:	public CountedWithoutFinalize
	{
public:
	Promise()
	:	fDelivery(new Delivery<void>)
		{}

	virtual ~Promise()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		fDelivery->fPromiseExists = false;
		fDelivery->fCnd.Broadcast();
		}

	bool IsDelivered()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		return fDelivery->fValQ;
		}

	void Deliver()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		fDelivery->fValQ.Set();
		fDelivery->fCnd.Broadcast();
		}

	bool QDeliver()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		if (not fDelivery->fValQ.QSet())
			return false;
		fDelivery->fCnd.Broadcast();
		return true;
		}

	ZRef<Delivery<void> > GetDelivery()
		{ return fDelivery; }

private:
	ZRef<Delivery<void> > fDelivery;
	};

// =================================================================================================
#pragma mark -
#pragma mark sPromise

inline
ZRef<Promise<void> > sPromise()
	{ return new Promise<void>; }

template <class T>
ZRef<Promise<T> > sPromise()
	{ return new Promise<T>; }

// =================================================================================================
#pragma mark -
#pragma mark sGetDeliveryClearPromise

template <class T>
ZRef<Delivery<T> > sGetDeliveryClearPromise(ZRef<Promise<T> >& ioPromise)
	{
	ZRef<Delivery<T> > theDelivery = ioPromise->GetDelivery();
	ioPromise.Clear();
	return theDelivery;
	}

} // namespace ZooLib

#endif // __ZooLib_Promise_h__
