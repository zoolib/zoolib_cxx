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

#ifndef __ZooLib_Tween_h__
#define __ZooLib_Tween_h__ 1
#include "zconfig.h"

#include "zoolib/CountedWithoutFinalize.h"
#include "zoolib/Compat_algorithm.h" // For std::binary_function
#include "zoolib/Compat_cmath.h" // For fmod

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZRef.h"

namespace ZooLib {

template <class Val0, class Val1>
Val0 sCombineTweenVals(const Val0& iVal0, const Val1& iVal1);

template <class Val0, class Val1>
struct TweenCombiner : public std::binary_function<ZQ<Val0>, ZQ<Val1>, ZQ<Val0> >
	{
	ZQ<Val0> operator()(const ZQ<Val0>& iVal0Q, const ZQ<Val1>& iVal1Q) const
		{
		if (not iVal0Q || not iVal1Q)
			return null;
		return sCombineTweenVals<Val0,Val1>(*iVal0Q, *iVal1Q);
		}
	};

// =================================================================================================
#pragma mark - Tween

/**
A Tween has a weight and returns a value given a place within that weight.
If given a place outside that weight it returns null.
*/

template <class Val_p>
class Tween
:	public CountedWithoutFinalize
	{
protected:
	Tween() {}

public:
	typedef Val_p Val;

	virtual ~Tween()
		{}

// Our protocol
	Val ValAtWrapped(double lPlace)
		{
		const double weight = this->Weight();
		if (weight <= 0)
			{
			lPlace = 0;
			}
		else
			{
			lPlace = fmod(lPlace, weight);
			if (lPlace < 0)
				lPlace += weight;
			}
		return this->QValAt(lPlace).Get();
		}

	virtual ZQ<Val> QValAt(double iPlace) = 0;

	virtual double Weight()
		{ return 1; }
	};

// =================================================================================================
#pragma mark - spWeight

template <class Val>
double spWeight(const ZRef<Tween<Val> >& iTween, ZQ<double>& ioCache)
	{
	if (not ioCache)
		ioCache = iTween->Weight();
	return *ioCache;
	}

// =================================================================================================
#pragma mark - sTween_Null

template <class Val>
class Tween_Null
	{
public:
	Tween_Null()
		{}
	
	virtual ZQ<Val> QValAt(double iPlace)
		{ return null; }
	
	virtual double Weight()
		{ return 0; }
	};

template <class Val>
ZRef<Tween<Val> > sTween_Null()
	{
	static ZRef<Tween<Val> > spTween = new Tween_Null<Val>;
	return spTween;
	}

// =================================================================================================
#pragma mark - sTween_Multiply

template <class Val0, class Val1, class Combiner = TweenCombiner<Val0,Val1> >
class Tween_Multiply
:	public Tween<Val0>
	{
public:
	Tween_Multiply(const ZRef<Tween<Val0> >& i0, const ZRef<Tween<Val1> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From Tween
	virtual ZQ<Val0> QValAt(double iPlace)
		{
		const double proportion = iPlace / spWeight(f0, fD0Q);
		return fCombiner(f0->QValAt(iPlace), f1->QValAt(proportion * spWeight(f1, fD1Q)));
		}

	virtual double Weight()
		{ return spWeight(f0, fD0Q); }

private:
	Combiner fCombiner;

	const ZRef<Tween<Val0> > f0;
	const ZRef<Tween<Val1> > f1;

	ZQ<double> fD0Q, fD1Q;
	};

template <class Val0, class Val1>
ZRef<Tween<Val0> > sTween_Multiply(const ZRef<Tween<Val0> >& i0, const ZRef<Tween<Val1> >& i1)
	{
	if (i0)
		{
		if (i1)
			return new Tween_Multiply<Val0,Val1>(i0, i1);
		return i0;
		}
	return null;
	}

template <class Val0, class Val1>
ZRef<Tween<Val0> > operator*(const ZRef<Tween<Val0> >& i0, const ZRef<Tween<Val1> >& i1)
	{ return sTween_Multiply(i0, i1); }

template <class Val0, class Val1>
ZRef<Tween<Val0> >& operator*=(ZRef<Tween<Val0> >& io0, const ZRef<Tween<Val1> >& i1)
	{ return io0 = io0 * i1; }

template <class Val_p>
struct TweenAccumulatorCombiner_Multiply
	{
	typedef Val_p Val;
	void operator()(ZRef<Tween<Val> >& io0, const ZRef<Tween<Val> >& i1) const
		{
		if (io0)
			io0 = sTween_Multiply(io0, i1);
		else
			io0 = i1;
		}
	};

// =================================================================================================
#pragma mark - sTween_Or

template <class Val0, class Val1, class Combiner = TweenCombiner<Val0,Val1> >
class Tween_Or
:	public Tween<Val0>
	{
public:
	Tween_Or(const ZRef<Tween<Val0> >& i0, const ZRef<Tween<Val1> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From Tween
	virtual ZQ<Val0> QValAt(double iPlace)
		{
		ZQ<Val0> theQ0 = f0->QValAt(iPlace);
		ZQ<Val1> theQ1 = f1->QValAt(iPlace);
		if (theQ0)
			{
		 	if (theQ1)
				return fCombiner(*theQ0, *theQ1);
			return *theQ0;
			}
		return null;
		}

	virtual double Weight()
		{ return sMax(spWeight(f0, fD0Q), spWeight(f1, fD1Q)); }

private:
	Combiner fCombiner;

	const ZRef<Tween<Val0> > f0;
	const ZRef<Tween<Val1> > f1;
	ZQ<double> fD0Q, fD1Q;
	};

template <class Val0, class Val1>
ZRef<Tween<Val0> > sTween_Or(const ZRef<Tween<Val0> >& i0, const ZRef<Tween<Val1> >& i1)
	{
	if (i0)
		{
		if (i1)
			return new Tween_Or<Val0,Val1>(i0, i1);
		return i0;
		}
	return null;
	}

template <class Val0, class Val1>
ZRef<Tween<Val0> > operator|(const ZRef<Tween<Val0> >& i0, const ZRef<Tween<Val1> >& i1)
	{ return sTween_Or(i0, i1); }

template <class Val0, class Val1>
ZRef<Tween<Val0> >& operator|=(ZRef<Tween<Val0> >& io0, const ZRef<Tween<Val1> >& i1)
	{ return io0 = io0 | i1; }

// =================================================================================================
#pragma mark - sTween_Or (for homogenous pairs)

template <class Val, class Combiner = TweenCombiner<Val,Val> >
class Tween_Or_Homogoneous
:	public Tween<Val>
	{
public:
	Tween_Or_Homogoneous(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		ZQ<Val> theQ0 = f0->QValAt(iPlace);
		ZQ<Val> theQ1 = f1->QValAt(iPlace);
		if (theQ0)
			{
		 	if (theQ1)
				return fCombiner(*theQ0, *theQ1);
			return *theQ0;
			}
		else if (theQ1)
			{
			return *theQ1;
			}
		return null;
		}

	virtual double Weight()
		{ return sMax(spWeight(f0, fD0Q), spWeight(f1, fD1Q)); }

private:
	Combiner fCombiner;

	const ZRef<Tween<Val> > f0;
	const ZRef<Tween<Val> > f1;
	ZQ<double> fD0Q, fD1Q;
	};

template <class Val>
ZRef<Tween<Val> > sTween_Or_Homogoneous(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{
	if (i0)
		{
		if (i1)
			return new Tween_Or_Homogoneous<Val>(i0, i1);
		return i0;
		}
	return i1;
	}

template <class Val>
ZRef<Tween<Val> > operator|(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{ return sTween_Or_Homogoneous(i0, i1); }

template <class Val>
ZRef<Tween<Val> >& operator|=(ZRef<Tween<Val> >& io0, const ZRef<Tween<Val> >& i1)
	{ return io0 = io0 | i1; }

template <class Val_p>
struct TweenAccumulatorCombiner_Or
	{
	typedef Val_p Val;
	void operator()(ZRef<Tween<Val> >& io0, const ZRef<Tween<Val> >& i1) const
		{ io0 = sTween_Or_Homogoneous<Val>(io0, i1); }
	};

// =================================================================================================
#pragma mark - sTween_And

template <class Val0, class Val1, class Combiner = TweenCombiner<Val0,Val1> >
class Tween_And
:	public Tween<Val0>
	{
public:
	Tween_And(const ZRef<Tween<Val0> >& i0, const ZRef<Tween<Val1> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From Tween
	virtual ZQ<Val0> QValAt(double iPlace)
		{ return fCombiner(f0->QValAt(iPlace), f1->QValAt(iPlace)); }

	virtual double Weight()
		{ return sMin(spWeight(f0, fD0Q), spWeight(f1, fD1Q)); }

private:
	Combiner fCombiner;

	const ZRef<Tween<Val0> > f0;
	const ZRef<Tween<Val1> > f1;
	ZQ<double> fD0Q, fD1Q;
	};

template <class Val0, class Val1>
ZRef<Tween<Val0> > sTween_And(const ZRef<Tween<Val0> >& i0, const ZRef<Tween<Val1> >& i1)
	{
	if (i0 && i1)
		return new Tween_And<Val0,Val1>(i0, i1);
	return null;
	}

template <class Val0, class Val1>
ZRef<Tween<Val0> > operator&(const ZRef<Tween<Val0> >& i0, const ZRef<Tween<Val1> >& i1)
	{ return sTween_And(i0, i1); }

template <class Val0, class Val1>
ZRef<Tween<Val0> >& operator&=(ZRef<Tween<Val0> >& io0, const ZRef<Tween<Val1> >& i1)
	{ return io0 = io0 & i1; }

template <class Val_p>
struct TweenAccumulatorCombiner_And
	{
	typedef Val_p Val;
	void operator()(ZRef<Tween<Val> >& io0, const ZRef<Tween<Val> >& i1) const
		{
		if (io0)
			io0 = sTween_And(io0, i1);
		else
			io0 = i1;
		}
	};

// =================================================================================================
#pragma mark - sTween_Each

template <class Val>
class Tween_Each
:	public Tween<Val>
	{
public:
	Tween_Each(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < spWeight(f0, fD0Q))
			return f0->QValAt(iPlace);
		else
			return f1->QValAt(iPlace - spWeight(f0, fD0Q));
		}

	virtual double Weight()
		{ return spWeight(f0, fD0Q) + spWeight(f1, fD1Q); }

private:
	const ZRef<Tween<Val> > f0, f1;
	ZQ<double> fD0Q, fD1Q;
	};

template <class Val>
ZRef<Tween<Val> > sTween_Each(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{
	if (i0)
		{
		if (i1)
			return new Tween_Each<Val>(i0, i1);
		return i0;
		}
	return i1;
	}

template <class Val>
ZRef<Tween<Val> > operator^(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{ return sTween_Each(i0, i1); }

template <class Val>
ZRef<Tween<Val> >& operator^=(ZRef<Tween<Val> >& io0, const ZRef<Tween<Val> >& i1)
	{ return io0 = io0 ^ i1; }

template <class Val_p>
struct TweenAccumulatorCombiner_Each
	{
	typedef Val_p Val;
	void operator()(ZRef<Tween<Val> >& io0, const ZRef<Tween<Val> >& i1) const
		{ io0 = sTween_Each(io0, i1); }
	};

// =================================================================================================
#pragma mark - sTween_Repeat (count)

template <class Val>
class Tween_Repeat
:	public Tween<Val>
	{
public:
	Tween_Repeat(const ZRef<Tween<Val> >& iTween, size_t iCount)
	:	fTween(iTween)
	,	fCount(iCount)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double lPlace)
		{
		const double childWeight = spWeight(fTween, fTweenWeightQ);
		lPlace = sMin(lPlace, childWeight * fCount);
		return fTween->QValAt(fmod(lPlace, childWeight));
		}

	virtual double Weight()
		{ return fCount * spWeight(fTween, fTweenWeightQ); }

private:
	const ZRef<Tween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const size_t fCount;
	};

template <class Val>
ZRef<Tween<Val> > sTween_Repeat(size_t iCount, const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Repeat<Val>(iTween, iCount);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_RepeatFor

template <class Val>
class Tween_RepeatFor
:	public Tween<Val>
	{
public:
	Tween_RepeatFor(const ZRef<Tween<Val> >& iTween, double iWeight)
	:	fTween(iTween)
	,	fWeight(iWeight)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (0 <= iPlace && iPlace < fWeight)
			return fTween->QValAt(fmod(iPlace, spWeight(fTween, fTweenWeightQ)));
		return null;
		}

	virtual double Weight()
		{ return fWeight; }

private:
	const ZRef<Tween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fWeight;
	};

template <class Val>
ZRef<Tween<Val> > sTween_RepeatFor(double iWeight, const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_RepeatFor<Val>(iTween, iWeight);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_ForExactly

template <class Val>
class Tween_ForExactly
:	public Tween<Val>
	{
public:
	Tween_ForExactly(const ZRef<Tween<Val> >& iTween, double iWeight)
	:	fTween(iTween)
	,	fWeight(iWeight)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < fWeight)
			return fTween->QValAt(sMin(iPlace, spWeight(fTween, fTweenWeightQ) * 0.999999));
		return null;
		}

	virtual double Weight()
		{ return fWeight; }

private:
	const ZRef<Tween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fWeight;
	};

template <class Val>
ZRef<Tween<Val> > sTween_ForExactly(double iWeight, const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_ForExactly<Val>(iTween, iWeight);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_ForAtMost

template <class Val>
class Tween_ForAtMost
:	public Tween<Val>
	{
public:
	Tween_ForAtMost(const ZRef<Tween<Val> >& iTween, double iAtMost)
	:	fTween(iTween)
	,	fAtMost(iAtMost)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < fAtMost)
			return fTween->QValAt(iPlace);
		return null;
		}

	virtual double Weight()
		{ return sMin(fAtMost, spWeight(fTween, fTweenWeightQ)); }

private:
	const ZRef<Tween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fAtMost;
	};

template <class Val>
ZRef<Tween<Val> > sTween_ForAtMost(double iAtMost, const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_ForAtMost<Val>(iTween, iAtMost);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_ForAtLeast

template <class Val>
class Tween_ForAtLeast
:	public Tween<Val>
	{
public:
	Tween_ForAtLeast(const ZRef<Tween<Val> >& iTween, double iAtLeast)
	:	fTween(iTween)
	,	fAtLeast(iAtLeast)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{ return fTween->QValAt(sMin(iPlace, spWeight(fTween, fTweenWeightQ) * 0.999999)); }

	virtual double Weight()
		{ return sMax(fAtLeast, spWeight(fTween, fTweenWeightQ)); }

private:
	const ZRef<Tween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fAtLeast;
	};

template <class Val>
ZRef<Tween<Val> > sTween_ForAtLeast(double iAtLeast, const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_ForAtLeast<Val>(iTween, iAtLeast);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_WeightShift

template <class Val>
class Tween_WeightShift
:	public Tween<Val>
	{
public:
	Tween_WeightShift(const ZRef<Tween<Val> >& iTween, double iWeightShift)
	:	fTween(iTween)
	,	fWeightShift(iWeightShift)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{ return fTween->QValAt(iPlace - fWeightShift); }

	virtual double Weight()
		{ return sMax(0.0, spWeight(fTween, fTweenWeightQ) + fWeightShift); }

private:
	const ZRef<Tween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fWeightShift;
	};

template <class Val>
ZRef<Tween<Val> > sTween_WeightShift(double iWeightShift, const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_WeightShift<Val>(iTween, iWeightShift);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_WeightScale

template <class Val>
class Tween_WeightScale
:	public Tween<Val>
	{
public:
	Tween_WeightScale(const ZRef<Tween<Val> >& iTween, double iWeightScale)
	:	fTween(iTween)
	,	fWeightScale(iWeightScale)
		{ ZAssert(fWeightScale); }

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (fWeightScale > 0)
			{
			return fTween->QValAt(iPlace / fWeightScale);
			}
		else
			{
			const double theWeight = spWeight(fTween, fTweenWeightQ);
			return fTween->QValAt(sMin(theWeight*0.999999, theWeight + iPlace/fWeightScale));
			}
		}

	virtual double Weight()
		{
		if (fWeightScale > 0)
			return fWeightScale * spWeight(fTween, fTweenWeightQ);
		else 
			return -fWeightScale * spWeight(fTween, fTweenWeightQ);
		}

private:
	const ZRef<Tween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fWeightScale;
	};

template <class Val>
ZRef<Tween<Val> > sTween_WeightScale(double iWeightScale, const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_WeightScale<Val>(iTween, iWeightScale);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_WeightPhase

template <class Val>
class Tween_WeightPhase
:	public Tween<Val>
	{
public:
	Tween_WeightPhase(const ZRef<Tween<Val> >& iTween, double iPhase)
	:	fTween(iTween)
	,	fPhase(iPhase)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		const double theWeight = spWeight(fTween, fTweenWeightQ);
		return fTween->QValAt(fmod(iPlace + fPhase * theWeight, theWeight));
		}

	virtual double Weight()
		{ return spWeight(fTween, fTweenWeightQ); }

private:
	const ZRef<Tween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fPhase;
	};

template <class Val>
ZRef<Tween<Val> > sTween_WeightPhase(double iPhase, const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_WeightPhase<Val>(iTween, iPhase);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_WeightNormalize

template <class Val>
class Tween_WeightNormalize
:	public Tween<Val>
	{
public:
	Tween_WeightNormalize(const ZRef<Tween<Val> >& iTween)
	:	fTween(iTween)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{ return fTween->QValAt(iPlace / spWeight(fTween, fTweenWeightQ)); }

private:
	const ZRef<Tween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	};

template <class Val>
ZRef<Tween<Val> > sTween_WeightNormalize(const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_WeightNormalize<Val>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_Const

template <class Val>
class Tween_Const
:	public Tween<Val>
	{
public:
	Tween_Const(const Val& iVal, double iWeight)
	:	fVal(iVal)
	,	fWeight(iWeight)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (0 <= iPlace && iPlace < fWeight)
			return fVal;
		return null;
		}

	virtual double Weight()
		{ return fWeight; }

private:
	const Val fVal;
	const double fWeight;
	};

template <class Val>
ZRef<Tween<Val> > sTween_Const(const Val& iVal)
	{ return new Tween_Const<Val>(iVal, 1.0); }

template <class Val>
ZRef<Tween<Val> > sTween_Const(const Val& iVal, double iWeight)
	{ return new Tween_Const<Val>(iVal, iWeight); }

// =================================================================================================
#pragma mark - sTween_ValScale

template <class Val>
ZRef<Tween<Val> > sTween_ValScale(Val iValScale, const ZRef<Tween<Val> >& iTween)
	{ return sTween_Multiply(iTween, sTween_Const<Val>(iValScale)); }

// =================================================================================================
#pragma mark - sTween_ValOffset

template <class Val>
class Tween_ValOffset
:	public Tween<Val>
	{
public:
	Tween_ValOffset(const ZRef<Tween<Val> >& iTween, Val iValOffset)
	:	fTween(iTween)
	,	fValOffset(iValOffset)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (ZQ<Val> theQ = fTween->QValAt(iPlace))
			return *theQ + fValOffset;
		return null;
		}

	virtual double Weight()
		{ return fTween->Weight(); }

private:
	const ZRef<Tween<Val> > fTween;
	const Val fValOffset;
	};

template <class Val>
ZRef<Tween<Val> > sTween_ValOffset(Val iValOffset, const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_ValOffset<Val>(iTween, iValOffset);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_Range

template <class Val>
ZRef<Tween<Val> > sTween_ValRange(Val iZeroVal, Val iOneVal, const ZRef<Tween<Val> >& iTween)
	{ return sTween_ValOffset(iZeroVal, sTween_ValScale<Val>(iOneVal - iZeroVal, iTween)); }

// =================================================================================================
#pragma mark - sTween_First

template <class Val>
class Tween_First
:	public Tween<Val>
	{
public:
	Tween_First(const ZRef<Tween<Val> >& iTween)
	:	fTween(iTween)
		{}

// From Tween
	virtual ZQ<Val> QValAt(double iPlace)
		{ return fTween->QValAt(0); }

private:
	const ZRef<Tween<Val> > fTween;
	};

template <class Val>
ZRef<Tween<Val> > sTween_First(const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_First<Val>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_Last

template <class Val>
ZRef<Tween<Val> > sTween_Last(const ZRef<Tween<Val> >& iTween)
	{ return sTween_First(sTween_WeightScale(-1, iTween)); }

} // namespace ZooLib

#endif // __ZooLib_Tween_h__
