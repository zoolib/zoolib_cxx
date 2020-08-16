// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Promise_h__
#define __ZooLib_Promise_h__ 1
#include "zconfig.h"

#include "zoolib/CountedWithoutFinalize.h"

#include "zoolib/ZP.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

template <class T> class Promise;

// =================================================================================================
#pragma mark - Delivery

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
#pragma mark - Promise

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

	ZP<Delivery<T>> GetDelivery()
		{ return fDelivery; }

private:
	ZP<Delivery<T>> fDelivery;
	};

// =================================================================================================
#pragma mark - Promise<void>

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

	ZP<Delivery<void>> GetDelivery()
		{ return fDelivery; }

private:
	ZP<Delivery<void>> fDelivery;
	};

// =================================================================================================
#pragma mark - sPromise

inline
ZP<Promise<void>> sPromise()
	{ return new Promise<void>; }

template <class T>
ZP<Promise<T>> sPromise()
	{ return new Promise<T>; }

// =================================================================================================
#pragma mark - sGetDeliveryClearPromise

template <class T>
ZP<Delivery<T>> sGetDeliveryClearPromise(ZP<Promise<T>>& ioPromise)
	{
	ZP<Delivery<T>> theDelivery = ioPromise->GetDelivery();
	ioPromise.Clear();
	return theDelivery;
	}

} // namespace ZooLib

#endif // __ZooLib_Promise_h__
