// Copyright (c) 2013-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Generator_h__
#define __ZooLib_Generator_h__ 1
#include "zconfig.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/ThreadVal.h"

/*
In the most common scenario, calls to the Generator take no parameter. So the yielding code
could yield an item and immediately move on to produce the next, thus its execution could
actually be parallel with the consumer's.
This code doesn't do that because part of the design is that the producer can know that what
it produced was consumed. This is not necessary in most scenarios, but any enhancements to
this code should respect the contract and use an alternate interface to make clear what's happening.
In reality, I've used PullPush everywhere a Generator might otherwise make sense, and there
the protocol follows stream semantics.
*/

namespace ZooLib {
namespace Generator {

// =================================================================================================
#pragma mark - ShelfBase

class ShelfBase
	{
public:
	ShelfBase()
	:	fPutLive(true)
	,	fTakeLive(true)
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

protected:
	ZMtx fMtx;
	ZCnd fCnd;
	bool fPutLive;
	bool fTakeLive;
	};

// =================================================================================================
#pragma mark - Shelf

template <class T>
class Shelf
:	public ShelfBase
	{
public:
	Shelf()
	:	fPtr(nullptr)
		{}

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
		// We're not doing std::move here because fPtr references the actual storage of
		// the item passed to QPut. That could be something that we don't want to clear.
		// With some rework of ZQ, and a change to QPut to distinguish rvalue refs, we
		// could std::move from there to the result.
		ZAcqMtx acq(fMtx);
		for (;;)
			{
			if (fPtr)
				{
				//
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

private:
	const T* fPtr;
	};

// =================================================================================================
#pragma mark - Shelf (specialization for void)

template <>
class Shelf<void>
:	public ShelfBase
	{
public:
	Shelf()
	:	fGotVal(false)
		{}

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

	bool QTake()
		{
		ZAcqMtx acq(fMtx);
		for (;;)
			{
			if (fGotVal)
				{
				fGotVal = false;
				fCnd.Broadcast();
				return true;
				}

			if (not fPutLive)
				return false;

			fCnd.Wait(fMtx);
			}
		}

private:
	bool fGotVal;
	};

// =================================================================================================
#pragma mark - ShelfPair

template <class T0, class T1>
class ShelfPair
:	public CountedWithoutFinalize
	{
public:
	Shelf<T0> fShelf0;
	Shelf<T1> fShelf1;
	};

// =================================================================================================
#pragma mark - Callable_Gen

template <class T0, class T1>
class Callable_Gen
:	public Callable<T0(T1)>
	{
public:
	Callable_Gen(const ZP<ShelfPair<T0,T1>>& iShelfPair)
	:	fShelfPair(iShelfPair)
		{}

// From Counted via Callable
	virtual void Finalize()
		{
		if (this->FinishFinalize())
			{
			fShelfPair->fShelf1.FinishedPuts();
			fShelfPair->fShelf0.FinishedTakes();
			delete this;
			}
		}

// From Callable
	virtual ZQ<T0> QCall(T1 iT1)
		{
		if (fShelfPair->fShelf1.QPut(iT1))
			return fShelfPair->fShelf0.QTake();
		return null;
		}

private:
	ZP<ShelfPair<T0,T1>> fShelfPair;
	};

// =================================================================================================
#pragma mark - Callable_Gen (partial specialization for <T,void>)

template <class T>
class Callable_Gen<T,void>
:	public Callable<T()>
	{
public:
	Callable_Gen(const ZP<ShelfPair<T,void>>& iShelfPair)
	:	fShelfPair(iShelfPair)
		{}

// From Counted via Callable
	virtual void Finalize()
		{
		if (this->FinishFinalize())
			{
			fShelfPair->fShelf1.FinishedPuts();
			fShelfPair->fShelf0.FinishedTakes();
			delete this;
			}
		}

// From Callable
	virtual ZQ<T> QCall()
		{
		if (fShelfPair->fShelf1.QPut())
			return fShelfPair->fShelf0.QTake();
		return null;
		}

private:
	ZP<ShelfPair<T,void>> fShelfPair;
	};

// =================================================================================================
#pragma mark - Callable_Yield

template <class T0, class T1>
class Callable_Yield
:	public Callable<T1(T0)>
	{
public:
	Callable_Yield(const ZP<ShelfPair<T0,T1>>& iShelfPair)
	:	fShelfPair(iShelfPair)
		{}

// From Counted via Callable
	virtual void Finalize()
		{
		if (this->FinishFinalize())
			{
			fShelfPair->fShelf0.FinishedPuts();
			fShelfPair->fShelf1.FinishedTakes();
			delete this;
			}
		}

// From Callable
	virtual ZQ<T1> QCall(T0 iT0)
		{
		if (ZQ<T1> theQ = fShelfPair->fShelf1.QTake())
			{
			if (fShelfPair->fShelf0.QPut(iT0))
				return theQ;
			}
		return null;
		}

private:
	ZP<ShelfPair<T0,T1>> fShelfPair;
	};

// =================================================================================================
#pragma mark - Callable_Yield (partial specialization for <void,T>)

template <class T>
class Callable_Yield<void,T>
:	public Callable<T()>
	{
public:
	Callable_Yield(const ZP<ShelfPair<void,T>>& iShelfPair)
	:	fShelfPair(iShelfPair)
		{}

// From Counted via Callable
	virtual void Finalize()
		{
		if (this->FinishFinalize())
			{
			fShelfPair->fShelf0.FinishedPuts();
			fShelfPair->fShelf1.FinishedTakes();
			delete this;
			}
		}

// From Callable
	virtual ZQ<T> QCall()
		{
		if (ZQ<T> theQ = fShelfPair->fShelf1.QTake())
			{
			if (fShelfPair->fShelf0.QPut())
				return theQ;
			}
		return null;
		}

private:
	ZP<ShelfPair<void,T>> fShelfPair;
	};

// =================================================================================================
#pragma mark - AsSig

// AsSig is needed so a void T0 or T1 does not end up in the param list of a signature.

template <class Sig_p>
struct AsSigBase { typedef Callable<Sig_p> Callable_t; };

template <class T0, class T1>
struct AsSig : public AsSigBase<T0(T1)> {};

template <class T>
struct AsSig<T,void> : public AsSigBase<T()> {};

template <class T>
struct AsSig<void,T> : public AsSigBase<void(T)> {};

template <>
struct AsSig<void,void> : public AsSigBase<void()> {};

// =================================================================================================
#pragma mark - sCallablePair

template <class T0, class T1>
void
sCallablePair(
	ZP<typename AsSig<T0,T1>::Callable_t>& oCallable_Gen,
	ZP<typename AsSig<T1,T0>::Callable_t>& oCallable_Yield)
	{
	ZP<ShelfPair<T0,T1>> theShelfPair = new ShelfPair<T0,T1>;
	oCallable_Gen = new Callable_Gen<T0,T1>(theShelfPair);
	oCallable_Yield = new Callable_Yield<T0,T1>(theShelfPair);
	}

// =================================================================================================
#pragma mark - sGenerator, function is passed a yield callable

template <class T0, class T1>
ZP<typename AsSig<T0,T1>::Callable_t>
sGenerator(const ZP<Callable<void(const ZP<typename AsSig<T1,T0>::Callable_t>&)>>& iCallable)
	{
	ZP<typename AsSig<T0,T1>::Callable_t> theCallable_Gen;
	ZP<typename AsSig<T1,T0>::Callable_t> theCallable_Yield;
	sCallablePair<T0,T1>(theCallable_Gen, theCallable_Yield);

	if (iCallable)
		sStartOnNewThread(sBindR(iCallable, theCallable_Yield));

	return theCallable_Gen;
	}

template <class T>
ZP<Callable<T()>>
sGenerator(const ZP<Callable<void(const ZP<Callable<void(T)>>&)>>& iCallable)
	{ return sGenerator<T,void>(iCallable); }

template <class T>
ZP<Callable<void(T)>>
sGenerator(const ZP<Callable<void(const ZP<Callable<T()>>&)>>& iCallable)
	{ return sGenerator<void,T>(iCallable); }

// =================================================================================================
#pragma mark - sGenerator, yield is installed as a thread value.

// The signature of the Generator's root function, the thing that's called,
// is void(T0*, T1*) -- two null pointers are passed, they're just there to
// distinguish it from the generator that is passed a yield callable.

typedef ThreadVal<ZP<Counted>, struct Tag_Callable_Yield> ThreadVal_Callable_Yield;

template <class R, class P>
ZQ<R>
sQYield(P iP)
	{
	typedef typename AsSig<R,P>::Callable_t Callable;
	if (ZP<Callable> theCallable = ThreadVal_Callable_Yield::sGet().DynamicCast<Callable>())
		return theCallable->QCall(iP);
	return null;
	}

template <class P>
void
sYield(P iP)
	{ return *sQYield<void,P>(iP); }

template <class T0, class T1>
void
sInstallYieldCall(const ZP<Callable<void(T0*,T1*)>>& iCallable,
	const ZP<typename AsSig<T1,T0>::Callable_t>& iCallable_Yield)
	{
	ThreadVal_Callable_Yield theTV(iCallable_Yield);
	iCallable->Call(nullptr, nullptr);
	}

template <class T0, class T1>
ZP<typename AsSig<T0,T1>::Callable_t>
sGenerator(const ZP<Callable<void(T0*,T1*)>>& iCallable)
	{
	ZP<typename AsSig<T0,T1>::Callable_t> theCallable_Gen;
	ZP<typename AsSig<T1,T0>::Callable_t> theCallable_Yield;
	sCallablePair<T0,T1>(theCallable_Gen, theCallable_Yield);

	if (iCallable)
		{
		sStartOnNewThread(
			sBindR(sCallable(sInstallYieldCall<T0,T1>), iCallable, theCallable_Yield));
		}

	return theCallable_Gen;
	}

} // namespace Generator

using Generator::sYield;
using Generator::sQYield;
using Generator::sGenerator;

} // namespace ZooLib

#endif // __ZooLib_Generator_h__
