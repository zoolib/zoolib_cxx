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

#include "zoolib/ZCallable_Bind.h"
#include "zoolib/ZCallable_Function.h"
#include "zoolib/ZCallOnNewThread.h"

namespace ZooLib {
namespace ZGenerator {

// =================================================================================================
// MARK: - ShelfBase

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
// MARK: - Shelf

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

private:
	const T* fPtr;
	};

// =================================================================================================
// MARK: - Shelf (specialization for void)

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

private:
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
// MARK: - AsSig

template <class Sig_p>
struct AsSigBase
	{
	typedef Sig_p Sig;
	typedef ZCallable<Sig> Callable;
	};

template <class T0, class T1>
struct AsSig : public AsSigBase<T0(T1)>
	{};

template <class T>
struct AsSig<T,void> : public AsSigBase<T()>
	{};

template <class T>
struct AsSig<void,T> : public AsSigBase<void(T)>
	{};

template <>
struct AsSig<void,void> : public AsSigBase<void()>
	{};

// =================================================================================================
// MARK: - Callable_Gen

template <class T0, class T1>
class Callable_Gen
:	public ZCallable<T0(T1)>
	{
public:
	Callable_Gen(const ZRef<ShelfPair<T0,T1> >& iShelfPair)
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
// MARK: - Callable_Gen (partial specialization for <T,void>)

template <class T>
class Callable_Gen<T,void>
:	public ZCallable<T()>
	{
public:
	Callable_Gen(const ZRef<ShelfPair<T,void> >& iShelfPair)
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
// MARK: - Callable_Yield

template <class T0, class T1>
class Callable_Yield
:	public ZCallable<T1(T0)>
	{
public:
	Callable_Yield(const ZRef<ShelfPair<T0,T1> >& iShelfPair)
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
// MARK: - Callable_Yield (specialization for <void,T>)

template <class T>
class Callable_Yield<void,T>
:	public ZCallable<T()>
	{
public:
	Callable_Yield(const ZRef<ShelfPair<void,T> >& iShelfPair)
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
void
sCallablePair(
	ZRef<typename AsSig<T0,T1>::Callable>& oCallable_Gen,
	ZRef<typename AsSig<T1,T0>::Callable>& oCallable_Yield)
	{
	ZRef<ShelfPair<T0,T1> > theShelfPair = new ShelfPair<T0,T1>;
	oCallable_Gen = new Callable_Gen<T0,T1>(theShelfPair);
	oCallable_Yield = new Callable_Yield<T0,T1>(theShelfPair);
	}

// =================================================================================================
// MARK: - sGenerator

template <class T0, class T1>
ZRef<typename AsSig<T0,T1>::Callable>
sGenerator(const ZRef<ZCallable<void(const ZRef<typename AsSig<T1,T0>::Callable>&)> >& iCallable)
	{
	ZRef<typename AsSig<T0,T1>::Callable> theCallable_Gen;
	ZRef<typename AsSig<T1,T0>::Callable> theCallable_Yield;
	sCallablePair<T0,T1>(theCallable_Gen, theCallable_Yield);

	if (iCallable)
		sCallOnNewThread(sBindR(iCallable, theCallable_Yield));

	return theCallable_Gen;
	}

template <class T>
ZRef<ZCallable<T()> >
sGenerator(const ZRef<ZCallable<void(const ZRef<ZCallable<void(T)> >&)> >& iCallable)
	{ return sGenerator<T,void>(iCallable); }

template <class T>
ZRef<ZCallable<void(T)> >
sGenerator(const ZRef<ZCallable<void(const ZRef<ZCallable<T(void)> >&)> >& iCallable)
	{ return sGenerator<void,T>(iCallable); }

// =================================================================================================
// MARK: - sGenerator

typedef ZThreadVal<ZRef<ZCounted>, struct Tag_Callable_Yield> ZThreadVal_Callable_Yield;

template <class R, class P>
ZQ<R>
sQYield(P iP)
	{
	typedef typename AsSig<R,P>::Callable Callable;
	if (ZRef<Callable> theCallable = ZThreadVal_Callable_Yield::sGet().DynamicCast<Callable>())
		return theCallable->QCall(iP);
	return null;
	}

template <class P>
void
sYield(P iP)
	{ return sQYield<void,P>(iP).Get(); }

template <class T0, class T1>
void
sInstallYieldCall(const ZRef<ZCallable_Void>& iCallable_Void,
	const ZRef<typename AsSig<T1,T0>::Callable>& iCallable_Yield)
	{
	ZThreadVal_Callable_Yield theTV(iCallable_Yield);
	iCallable_Void->Call();
	}

template <class T0, class T1>
ZRef<typename AsSig<T0,T1>::Callable>
sGenerator(const ZRef<ZCallable_Void>& iCallable_Void)
	{
	ZRef<typename AsSig<T0,T1>::Callable> theCallable_Gen;
	ZRef<typename AsSig<T1,T0>::Callable> theCallable_Yield;
	sCallablePair<T0,T1>(theCallable_Gen, theCallable_Yield);

	if (iCallable_Void)
		{
		sCallOnNewThread(
			sBindR(sCallable(sInstallYieldCall<T0,T1>), iCallable_Void, theCallable_Yield));
		}

	return theCallable_Gen;
	}

template <class T>
ZRef<typename AsSig<T,void>::Callable>
sGenerator(const ZRef<ZCallable_Void>& iCallable_Void)
	{ return sGenerator<T,void>(iCallable_Void); }

} // namespace ZGenerator

using ZGenerator::sYield;
using ZGenerator::sQYield;
using ZGenerator::sGenerator;

} // namespace ZooLib

#endif // __ZGenerator_h__
