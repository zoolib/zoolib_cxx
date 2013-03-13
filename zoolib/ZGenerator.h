/* -------------------------------------------------------------------------------------------------
Copyright (c) 2013 Andrew Green
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

#ifndef __ZGenerator_h__
#define __ZGenerator_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallOnNewThread.h"
#include "zoolib/ZCallable_Function.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Shelf

template <class T>
class Shelf
	{
public:
	Shelf()
	:	fPutLive(true)
	,	fTakeLive(true)
	,	fPtr(nullptr)
		{}

	void FinishedPuts()
		{
		ZAcqMtx acq(fMtx);
		fPutLive = false;
		fCnd.Broadcast();
		}

	void FinishedTakes()
		{
		ZAcqMtx acq(fMtx);
		fTakeLive = false;
		fCnd.Broadcast();
		}

	bool QPut(const T& iT)
		{
		ZAcqMtx acq(fMtx);
		ZAssert(not fPtr);
		fPtr = &iT;
		fCnd.Broadcast();

		for (;;)
			{
			if (not fPtr)
				return true;

			if (not fTakeLive)
				return false;

			fCnd.Wait(fMtx);
			}		
		}

	ZQ<T> QTake()
		{
		ZAcqMtx acq(fMtx);
		for (;;)
			{
			if (fPtr)
				{
				const T result = *fPtr;
				fPtr = nullptr;
				fCnd.Broadcast();
				return result;
				}

			if (not fPutLive)
				return null;

			fCnd.Wait(fMtx);
			}
		}

	ZMtx fMtx;
	ZCnd fCnd;
	bool fPutLive;
	bool fTakeLive;
	const T* fPtr;
	};

// =================================================================================================
// MARK: - Shelf (specialization for void)

template <>
class Shelf<void>
	{
public:
	Shelf()
	:	fPutLive(true)
	,	fTakeLive(true)
	,	fGotVal(false)
		{}

	void FinishedPuts()
		{
		ZAcqMtx acq(fMtx);
		fPutLive = false;
		fCnd.Broadcast();
		}

	void FinishedTakes()
		{
		ZAcqMtx acq(fMtx);
		fTakeLive = false;
		fCnd.Broadcast();
		}

	bool QPut()
		{
		ZAcqMtx acq(fMtx);
		ZAssert(not fGotVal);
		fGotVal = true;
		fCnd.Broadcast();

		for (;;)
			{
			if (not fGotVal)
				return true;

			if (not fTakeLive)
				return false;

			fCnd.Wait(fMtx);
			}		
		}

	ZQ<void> QTake()
		{
		ZAcqMtx acq(fMtx);
		for (;;)
			{
			if (fGotVal)
				{
				fGotVal = false;
				fCnd.Broadcast();
				return notnull;
				}

			if (not fPutLive)
				return null;

			fCnd.Wait(fMtx);
			}
		}

	ZMtx fMtx;
	ZCnd fCnd;
	bool fPutLive;
	bool fTakeLive;
	bool fGotVal;
	};

// =================================================================================================
// MARK: - ShelfPair

template <class T0, class T1>
class ShelfPair
:	public ZCountedWithoutFinalize
	{
public:
	Shelf<T0> fShelf0;
	Shelf<T1> fShelf1;
	};


// =================================================================================================
// MARK: - SigThing

template <class Sig_p>
struct SigThingBase
	{
	typedef Sig_p Sig;
	typedef ZCallable<Sig> Callable;
	typedef ZRef<Callable> ZRef_Callable;
	};

template <class T0, class T1>
struct SigThing : public SigThingBase<T0(T1)>
	{};

template <class T>
struct SigThing<T,void> : public SigThingBase<T()>
	{};

template <class T>
struct SigThing<void,T> : public SigThingBase<void(T)>
	{};

template <>
struct SigThing<void,void> : public SigThingBase<void()>
	{};

// =================================================================================================
// MARK: - Callable01

template <class T0, class T1>
class Callable01
:	public ZCallable<T0(T1)>
	{
public:
	Callable01(const ZRef<ShelfPair<T0,T1> >& iShelfPair)
	:	fShelfPair(iShelfPair)
		{}

// From ZCounted via ZCallable
	virtual void Finalize()
		{
		if (this->FinishFinalize())
			{
			fShelfPair->fShelf1.FinishedPuts();
			fShelfPair->fShelf0.FinishedTakes();
			delete this;
			}
		}

// From ZCallable
	virtual ZQ<T0> QCall(T1 iT1)
		{
		if (fShelfPair->fShelf1.QPut(iT1))
			return fShelfPair->fShelf0.QTake();
		return null;
		}

private:
	ZRef<ShelfPair<T0,T1> > fShelfPair;
	};

// =================================================================================================
// MARK: - Callable01 (partial specialization for <T,void>)

template <class T>
class Callable01<T,void>
:	public ZCallable<T()>
	{
public:
	Callable01(const ZRef<ShelfPair<T,void> >& iShelfPair)
	:	fShelfPair(iShelfPair)
		{}

// From ZCounted via ZCallable
	virtual void Finalize()
		{
		if (this->FinishFinalize())
			{
			fShelfPair->fShelf1.FinishedPuts();
			fShelfPair->fShelf0.FinishedTakes();
			delete this;
			}
		}

// From ZCallable
	virtual ZQ<T> QCall()
		{
		if (fShelfPair->fShelf1.QPut())
			return fShelfPair->fShelf0.QTake();
		return null;
		}

private:
	ZRef<ShelfPair<T,void> > fShelfPair;
	};

// =================================================================================================
// MARK: - Callable10

template <class T0, class T1>
class Callable10
:	public ZCallable<T1(T0)>
	{
public:
	Callable10(const ZRef<ShelfPair<T0,T1> >& iShelfPair)
	:	fShelfPair(iShelfPair)
		{}

// From ZCounted via ZCallable
	virtual void Finalize()
		{
		if (this->FinishFinalize())
			{
			fShelfPair->fShelf0.FinishedPuts();
			fShelfPair->fShelf1.FinishedTakes();
			delete this;
			}
		}

// From ZCallable
	virtual ZQ<T1> QCall(T0 iT0)
		{
		if (ZQ<T1> theQ = fShelfPair->fShelf1.QTake())
			{
			if (fShelfPair->fShelf0.QPut(iT0))
				return theQ;
			}
		return null;
		}

	ZRef<ShelfPair<T0,T1> > fShelfPair;
	};

// =================================================================================================
// MARK: - Callable10 (specialization for <void,T>)

template <class T>
class Callable10<void,T>
:	public ZCallable<T()>
	{
public:
	Callable10(const ZRef<ShelfPair<void,T> >& iShelfPair)
	:	fShelfPair(iShelfPair)
		{}

// From ZCounted via ZCallable
	virtual void Finalize()
		{
		if (this->FinishFinalize())
			{
			fShelfPair->fShelf0.FinishedPuts();
			fShelfPair->fShelf1.FinishedTakes();
			delete this;
			}
		}

// From ZCallable
	virtual ZQ<T> QCall()
		{
		if (ZQ<T> theQ = fShelfPair->fShelf1.QTake())
			{
			if (fShelfPair->fShelf0.QPut())
				return theQ;
			}
		return null;
		}

	ZRef<ShelfPair<void,T> > fShelfPair;
	};

// =================================================================================================
// MARK: - sCallablePair

template <class T0, class T1>
void sCallablePair
	(typename SigThing<T0,T1>::ZRef_Callable& oCallable_01,
	typename SigThing<T1,T0>::ZRef_Callable& oCallable_10)
	{
	ZRef<ShelfPair<T0,T1> > theShelfPair = new ShelfPair<T0,T1>;
	oCallable_01 = new Callable01<T0,T1>(theShelfPair);
	oCallable_10 = new Callable10<T0,T1>(theShelfPair);
	}

// =================================================================================================
// MARK: - sGenerator

template <class T0, class T1>
typename SigThing<T0,T1>::ZRef_Callable
sGenerator(const ZRef<ZCallable<
	void(const typename SigThing<T1,T0>::ZRef_Callable&)
	> >& iCallable)
	{
	typename SigThing<T0,T1>::ZRef_Callable theCallable_01;
	typename SigThing<T1,T0>::ZRef_Callable theCallable_10;

	sCallablePair<T0,T1>(theCallable_01, theCallable_10);
	sCallOnNewThread(sBindR(iCallable, theCallable_10));
	return theCallable_01;
	}

template <class T>
typename SigThing<T,void>::ZRef_Callable
sGenerator(const ZRef<ZCallable<
	void(const typename SigThing<void,T>::ZRef_Callable&)
	> >& iCallable)
	{ return sGenerator<T,void>(iCallable); }

// =================================================================================================
// MARK: - sGenerator

typedef ZThreadVal<ZRef<ZCounted>, struct Tag_Callable_Yield> ZThreadVal_Callable_Yield;

template <class R, class P>
ZQ<R> sQYield(P iP)
	{
	typedef typename SigThing<R,P>::Callable Callable;
	if (ZRef<Callable> theCallable = ZThreadVal_Callable_Yield::sGet().DynamicCast<Callable>())
		return theCallable->QCall(iP);
	return null;
	}

template <class P>
void sYield(P iP)
	{ return sQYield<void,P>(iP).Get(); }

template <class T0, class T1>
void
sCallWith(const ZRef<ZCallable_Void>& iCallable_Void,
	const typename SigThing<T1,T0>::ZRef_Callable& iCallable_Yield)
	{
	ZThreadVal_Callable_Yield theTV(iCallable_Yield);
	sCall(iCallable_Void);
	}

template <class T0, class T1>
typename SigThing<T0,T1>::ZRef_Callable
sGenerator(const ZRef<ZCallable_Void>& iCallable_Void)
	{
	typename SigThing<T0,T1>::ZRef_Callable theCallable_01;
	typename SigThing<T1,T0>::ZRef_Callable theCallable_10;
	sCallablePair<T0,T1>(theCallable_01, theCallable_10);
	sCallOnNewThread(sBindR(sCallable(sCallWith<T0,T1>), iCallable_Void, theCallable_10));
	return theCallable_01;
	}

template <class T>
typename SigThing<T,void>::ZRef_Callable
sGenerator(const ZRef<ZCallable_Void>& iCallable_Void)
	{ return sGenerator<T,void>(iCallable_Void); }

} // namespace ZooLib

#endif // __ZGenerator_h__
