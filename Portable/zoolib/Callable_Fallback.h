// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Fallback_h__
#define __ZooLib_Callable_Fallback_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable_Fallback

template <class Signature> class Callable_Fallback;

// =================================================================================================
#pragma mark - Callable_Fallback (specialization for 0 params)

template <class R>
class Callable_Fallback<R()>
:	public Callable<R()>
	{
public:
	typedef Callable<R()> Callable;

	Callable_Fallback(const ZP<Callable>& iCallable0, const ZP<Callable>& iCallable1)
	:	fCallable0(iCallable0)
	,	fCallable1(iCallable1)
		{}

// From Callable
	virtual QRet<R> QCall()
		{
		if (const ZQ<R>& theQ = fCallable0->QCall())
			return theQ;
		return fCallable1->QCall();
		}

private:
	const ZP<Callable> fCallable0;
	const ZP<Callable> fCallable1;
	};

// =================================================================================================
#pragma mark - Callable_Fallback variants

#define ZMACRO_Callable_Callable(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
class Callable_Fallback<R(ZMACRO_Callable_P##X)> \
:	public Callable<R(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef Callable<R(ZMACRO_Callable_P##X)> Callable; \
\
	Callable_Fallback(const ZP<Callable>& iCallable0, const ZP<Callable>& iCallable1) \
	:	fCallable0(iCallable0) \
	,	fCallable1(iCallable1) \
		{} \
\
	virtual QRet<R> QCall(ZMACRO_Callable_Pi##X) \
		{ \
		if (const QRet<R>& theQ = fCallable0->QCall(ZMACRO_Callable_i##X)) \
			return theQ; \
		return fCallable1->QCall(ZMACRO_Callable_i##X); \
		} \
\
private: \
	const ZP<Callable> fCallable0; \
	const ZP<Callable> fCallable1; \
	};

ZMACRO_Callable_Callable(0)
ZMACRO_Callable_Callable(1)
ZMACRO_Callable_Callable(2)
ZMACRO_Callable_Callable(3)
ZMACRO_Callable_Callable(4)
ZMACRO_Callable_Callable(5)
ZMACRO_Callable_Callable(6)
ZMACRO_Callable_Callable(7)
ZMACRO_Callable_Callable(8)
ZMACRO_Callable_Callable(9)
ZMACRO_Callable_Callable(A)
ZMACRO_Callable_Callable(B)
ZMACRO_Callable_Callable(C)
ZMACRO_Callable_Callable(D)
ZMACRO_Callable_Callable(E)
ZMACRO_Callable_Callable(F)

#undef ZMACRO_Callable_Callable

// =================================================================================================
#pragma mark - sCallable_Fallback

template <class Sig>
ZP<Callable<Sig>> sCallable_Fallback(
	const ZP<Callable<Sig>>& iCallable0, const ZP<Callable<Sig>>& iCallable1)
	{
	if (iCallable0)
		{
		if (iCallable1)
			return new Callable_Fallback<Sig>(iCallable0, iCallable1);
		return iCallable0;
		}
	return iCallable1;
	}

} // namespace ZooLib

#endif // __ZooLib_Callable_Fallback_h__
