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

#ifndef __ZPromise_h__
#define __ZPromise_h__ 1
#include "zconfig.h"

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

template <class T> class ZPromise;

// =================================================================================================
#pragma mark -
#pragma mark * ZDelivery

template <class T>
class ZDelivery
:	public ZCountedWithoutFinalize
	{
	ZDelivery()
	:	fPromiseExists(true)
		{}

public:
	virtual ~ZDelivery()
		{}

	void Wait()
		{
		ZAcqMtx acq(fMtx);
		for (;;)
			{
			if (fVal || not fPromiseExists)
				return;
			fCnd.Wait(fMtx);
			}
		}

	bool WaitFor(double iTimeout)
		{
		ZAcqMtx acq(fMtx);

		if (fVal || not fPromiseExists)
			return true;

		fCnd.WaitFor(fMtx, iTimeout);

		if (fVal || not fPromiseExists)
			return true;

		return false;
		}

	bool WaitUntil(ZTime iDeadline)
		{
		ZAcqMtx acq(fMtx);

		if (fVal || not fPromiseExists)
			return true;

		fCnd.WaitUntil(fMtx, iDeadline);

		if (fVal || not fPromiseExists)
			return true;

		return false;
		}

	ZQ<T> QGet()
		{
		ZAcqMtx acq(fMtx);
		while (fPromiseExists && not fVal)
			fCnd.Wait(fMtx);
		return fVal;
		}

private:
	friend class ZPromise<T>;
	ZMtx fMtx;
	ZCnd fCnd;
	bool fPromiseExists;
	ZQ<T> fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZPromise

template <class T = void>
class ZPromise
:	public ZCountedWithoutFinalize
	{
public:
	ZPromise()
	:	fDelivery(new ZDelivery<T>)
		{}

	virtual ~ZPromise()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		fDelivery->fPromiseExists = false;
		fDelivery->fCnd.Broadcast();
		}

	bool IsDelivered()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		return fDelivery->fVal;
		}

	void Deliver(const T& iVal)
		{
		ZAcqMtx acq(fDelivery->fMtx);
		fDelivery->fVal.Set(iVal);
		fDelivery->fCnd.Broadcast();
		}

	bool QDeliver(const T& iVal)
		{
		ZAcqMtx acq(fDelivery->fMtx);
		if (not fDelivery->fVal.QSet(iVal))
			return false;
		fDelivery->fCnd.Broadcast();
		return true;
		}

	ZRef<ZDelivery<T> > GetDelivery()
		{ return fDelivery; }

private:
	ZRef<ZDelivery<T> > fDelivery;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZPromise<void>

template <>
class ZPromise<void>
:	public ZCountedWithoutFinalize
	{
public:
	ZPromise()
	:	fDelivery(new ZDelivery<void>)
		{}

	virtual ~ZPromise()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		fDelivery->fPromiseExists = false;
		fDelivery->fCnd.Broadcast();
		}

	bool IsDelivered()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		return fDelivery->fVal;
		}

	void Deliver()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		fDelivery->fVal.Set();
		fDelivery->fCnd.Broadcast();
		}

	bool QDeliver()
		{
		ZAcqMtx acq(fDelivery->fMtx);
		if (not fDelivery->fVal.QSet())
			return false;
		fDelivery->fCnd.Broadcast();
		return true;
		}

	ZRef<ZDelivery<void> > GetDelivery()
		{ return fDelivery; }

private:
	ZRef<ZDelivery<void> > fDelivery;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sPromise

inline
ZRef<ZPromise<void> > sPromise()
	{ return new ZPromise<void>; }

template <class T>
ZRef<ZPromise<T> > sPromise()
	{ return new ZPromise<T>; }

// =================================================================================================
#pragma mark -
#pragma mark * sGetClear

template <class T>
ZRef<ZDelivery<T> > sGetDeliveryClearPromise(ZRef<ZPromise<T> >& ioPromise)
	{
	ZRef<ZDelivery<T> > theDelivery = ioPromise->GetDelivery();
	ioPromise.Clear();
	return theDelivery;
	}

} // namespace ZooLib

#endif // __ZPromise_h__
