// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Tween_h__
#define __ZooLib_GameEngine_Tween_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Log.h"
#include "zoolib/PairwiseCombiner_T.h"
#include "zoolib/Tween_Std.h"

#include "zoolib/GameEngine/Types.h"

#include <list>

namespace ZooLib {
namespace GameEngine {

// You need to provide an implementation of this template function for each Tween specialization.

template <class T>
ZP<Tween<T> > sTween(const ZQ<Val>& iValQ);

// =================================================================================================

template <class TweenAccumulatorCombiner>
ZP<Tween<typename TweenAccumulatorCombiner::Val> >
spCombineTweens(size_t iStart, const Seq& iSeq)
	{
	typedef typename TweenAccumulatorCombiner::Val Val;

	PairwiseCombiner_T<ZP<Tween<Val> >,
		TweenAccumulatorCombiner,
		std::list<ZP<Tween<Val> > > > theAcc;

	for (size_t index = iStart, count = iSeq.Size(); index < count; ++index)
		theAcc.Include(sTween<Val>(iSeq.QGet(index)));

	return theAcc.Get();
	}

template <class Val>
ZP<Tween<Val> > spTweens(size_t iStart, const Seq& iSeq)
	{ return spCombineTweens<TweenAccumulatorCombiner_Each<Val> >(iStart, iSeq); }

template <class Val>
ZP<Tween<Val> > sTweens(const Seq& iSeq)
	{
	if (ZQ<string8> theQ = iSeq.QGet<string8>(0))
		{
		if (*theQ == "|")
			return spCombineTweens<TweenAccumulatorCombiner_Or<Val> >(size_t(1), iSeq);

		if (*theQ == "&")
			return spCombineTweens<TweenAccumulatorCombiner_And<Val> >(size_t(1), iSeq);

		if (*theQ == "*")
			return spCombineTweens<TweenAccumulatorCombiner_Multiply<Val> >(size_t(1), iSeq);

		if (*theQ == "/")
			{
			return spCombineTweens<TweenAccumulatorCombiner_Multiply<Val> >(size_t(1), iSeq);
			}

		if (*theQ == "^")
			return spCombineTweens<TweenAccumulatorCombiner_Each<Val> >(size_t(1), iSeq);

		if (*theQ == "RC" || *theQ == "RepeatCount")
			{
			if (ZQ<Rat> theQ = sQRat(iSeq.QGet(1)))
				return sTween_Repeat(*theQ, spTweens<Val>(size_t(2), iSeq));
			}

		if (*theQ == "RF" || *theQ == "RepeatFor")
			{
			if (ZQ<Rat> theQ = sQRat(iSeq.QGet(1)))
				return sTween_RepeatFor(*theQ, spTweens<Val>(size_t(2), iSeq));
			}

		if (*theQ == "Rate" || *theQ == "WeightScale")
			{
			if (ZQ<Rat> theQ = sQRat(iSeq.QGet(1)))
				return sTween_WeightScale(*theQ, spTweens<Val>(size_t(2), iSeq));
			}

		if (*theQ == "Delay" || *theQ == "WeightShift")
			{
			if (ZQ<Rat> theQ = sQRat(iSeq.QGet(1)))
				return sTween_WeightShift(*theQ, spTweens<Val>(size_t(2), iSeq));
			}

		if (*theQ == "ForAtMost")
			{
			if (ZQ<Rat> theQ = sQRat(iSeq.QGet(1)))
				return sTween_ForAtMost(*theQ, spTweens<Val>(size_t(2), iSeq));
			}

		if (*theQ == "RandomPhase")
			{ return sTween_WeightPhase(sRandomInRange(0, 1), spTweens<Val>(size_t(1), iSeq)); }
		}

	return spTweens<Val>(size_t(0), iSeq);
	}

// =================================================================================================
#pragma mark - Tween_Callable

template <class Val>
class Tween_Callable
:	public Tween<Val>
	{
public:
	typedef Val(Signature)(double);

	Tween_Callable(const ZP<Callable<Signature> >& iCallable)
	:	fCallable(iCallable)
		{}

	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace >= 0 && iPlace < 1.0)
			return fCallable->Call(iPlace);
		return null;
		}

private:
	const ZP<Callable<Signature> > fCallable;
	};

template <class Val>
Tween<Val> sTween_Callable(const ZP<Callable<Val(double)> >& iCallable)
	{
	if (iCallable)
		return new Tween_Callable<Val>(iCallable);
	return null;
	}

// =================================================================================================
#pragma mark - Tween_Filter_Callable

template <class Val, class Param>
class Tween_Filter_Callable
:	public Tween<Val>
	{
public:
	typedef Val(Signature)(Param);

	Tween_Filter_Callable(
		const ZP<Tween<Param> >& iTween, const ZP<Callable<Signature> >& iCallable)
	:	fTween(iTween)
	,	fCallable(iCallable)
		{}

	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (ZQ<Param> theQ = fTween->QValAt(iPlace))
			return fCallable->Call(*theQ);
		return null;
		}

	virtual double Weight()
		{ return fTween->Weight(); }

private:
	const ZP<Tween<Param> > fTween;
	const ZP<Callable<Signature> > fCallable;
	};

template <class Val, class Param>
Tween<Val> sTween_Filter_Callable(
	const ZP<Tween<Param> >& iTween, const ZP<Callable<Val(Param)> >& iCallable)
	{
	if (iTween && iCallable)
		return new Tween_Filter_Callable<Val, Param>(iTween, iCallable);
	return null;
	}

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Tween_h__
