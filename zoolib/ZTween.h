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

#ifndef __ZTween_h__
#define __ZTween_h__ 1
#include "zconfig.h"

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZCompat_algorithm.h" // For std::binary_function
#include "zoolib/ZCompat_cmath.h" // For fmod
#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZRef.h"

namespace ZooLib {

template <class Val0, class Val1>
ZQ<Val0> sCombineTweenVals(const ZQ<Val0>& iVal0Q, const ZQ<Val1>& iVal1Q);

template <class Val0, class Val1>
struct TweenCombiner : public std::binary_function<ZQ<Val0>, ZQ<Val1>, ZQ<Val0> >
	{
	ZQ<Val0> operator()(const ZQ<Val0>& iVal0Q, const ZQ<Val1>& iVal1Q) const
		{ return sCombineTweenVals<Val0,Val1>(iVal0Q, iVal1Q); }
	};

// =================================================================================================
// MARK: - ZTween

/**
A ZTween has a weight and returns a value given a place within that weight.
If given a place outside that weight it returns null.
*/

template <class Val_p>
class ZTween
:	public ZCountedWithoutFinalize
	{
protected:
	ZTween() {}

public:
	typedef Val_p Val;

	virtual ~ZTween()
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
// MARK: - spWeight

template <class Val>
double spWeight(const ZRef<ZTween<Val> >& iTween, ZQ<double>& ioCache)
	{
	if (not ioCache)
		ioCache = iTween->Weight();
	return *ioCache;
	}

// =================================================================================================
// MARK: - sTween_Null

template <class Val>
class ZTween_Null
	{
public:
	ZTween_Null()
		{}
	
	virtual ZQ<Val> QValAt(double iPlace)
		{ return null; }
	
	virtual double Weight()
		{ return 0; }
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Null()
	{
	static ZRef<ZTween<Val> > spTween = new ZTween_Null<Val>;
	return spTween;
	}

// =================================================================================================
// MARK: - sTween_Or

template <class Val0, class Val1, class Combiner = TweenCombiner<Val0,Val1> >
class ZTween_Or
:	public ZTween<Val0>
	{
public:
	ZTween_Or(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZTween
	virtual ZQ<Val0> QValAt(double iPlace)
		{ return fCombiner(f0->QValAt(iPlace), f1->QValAt(iPlace)); }

	virtual double Weight()
		{ return std::max(spWeight(f0, fD0Q), spWeight(f1, fD1Q)); }

private:
	Combiner fCombiner;

	const ZRef<ZTween<Val0> > f0;
	const ZRef<ZTween<Val1> > f1;
	ZQ<double> fD0Q, fD1Q;
	};

template <class Val0, class Val1>
ZRef<ZTween<Val0> > sTween_Or(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{
	if (i0)
		{
		if (i1)
			return new ZTween_Or<Val0,Val1>(i0, i1);
		return i0;
		}
	return null;
	}

template <class Val0, class Val1>
ZRef<ZTween<Val0> > operator|(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{ return sTween_Or(i0, i1); }

template <class Val0, class Val1>
ZRef<ZTween<Val0> >& operator|=(ZRef<ZTween<Val0> >& io0, const ZRef<ZTween<Val1> >& i1)
	{ return io0 = io0 | i1; }

// =================================================================================================
// MARK: - sTween_Or (for homogenous pairs)

template <class Val>
ZRef<ZTween<Val> > sTween_Or(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{
	if (i0)
		{
		if (i1)
			return new ZTween_Or<Val,Val>(i0, i1);
		return i0;
		}
	return i1;
	}

template <class Val>
ZRef<ZTween<Val> > operator|(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{ return sTween_Or(i0, i1); }

template <class Val>
ZRef<ZTween<Val> >& operator|=(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1)
	{ return io0 = io0 | i1; }

template <class Val_p>
struct ZTweenAccumulatorCombiner_Or
	{
	typedef Val_p Val;
	void operator()(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1) const
		{ io0 = sTween_Or<Val>(io0, i1); }
	};

// =================================================================================================
// MARK: - sTween_And

template <class Val0, class Val1, class Combiner = TweenCombiner<Val0,Val1> >
class ZTween_And
:	public ZTween<Val0>
	{
public:
	ZTween_And(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZTween
	virtual ZQ<Val0> QValAt(double iPlace)
		{ return fCombiner(f0->QValAt(iPlace), f1->QValAt(iPlace)); }

	virtual double Weight()
		{ return std::min(spWeight(f0, fD0Q), spWeight(f1, fD1Q)); }

private:
	Combiner fCombiner;

	const ZRef<ZTween<Val0> > f0;
	const ZRef<ZTween<Val1> > f1;
	ZQ<double> fD0Q, fD1Q;
	};

template <class Val0, class Val1>
ZRef<ZTween<Val0> > sTween_And(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{
	if (i0 && i1)
		return new ZTween_And<Val0,Val1>(i0, i1);
	return null;
	}

template <class Val0, class Val1>
ZRef<ZTween<Val0> > operator&(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{ return sTween_And(i0, i1); }

template <class Val0, class Val1>
ZRef<ZTween<Val0> >& operator&=(ZRef<ZTween<Val0> >& io0, const ZRef<ZTween<Val1> >& i1)
	{ return io0 = io0 & i1; }

template <class Val_p>
struct ZTweenAccumulatorCombiner_And
	{
	typedef Val_p Val;
	void operator()(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1) const
		{
		if (io0)
			io0 = sTween_And(io0, i1);
		else
			io0 = i1;
		}
	};

// =================================================================================================
// MARK: - sTween_Each

template <class Val>
class ZTween_Each
:	public ZTween<Val>
	{
public:
	ZTween_Each(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZTween
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
	const ZRef<ZTween<Val> > f0, f1;
	ZQ<double> fD0Q, fD1Q;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Each(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{
	if (i0)
		{
		if (i1)
			return new ZTween_Each<Val>(i0, i1);
		return i0;
		}
	return i1;
	}

template <class Val>
ZRef<ZTween<Val> > operator^(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{ return sTween_Each(i0, i1); }

template <class Val>
ZRef<ZTween<Val> >& operator^=(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1)
	{ return io0 = io0 ^ i1; }

template <class Val_p>
struct ZTweenAccumulatorCombiner_Each
	{
	typedef Val_p Val;
	void operator()(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1) const
		{ io0 = sTween_Each(io0, i1); }
	};

// =================================================================================================
// MARK: - sTween_With

template <class Val0, class Val1, class Combiner = TweenCombiner<Val0,Val1> >
class ZTween_With
:	public ZTween<Val0>
	{
public:
	ZTween_With(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZTween
	virtual ZQ<Val0> QValAt(double iPlace)
		{
		const double proportion = iPlace / spWeight(f0, fD0Q);
		return fCombiner(f0->QValAt(iPlace), f1->QValAt(proportion * spWeight(f1, fD1Q)));
		}

	virtual double Weight()
		{ return spWeight(f0, fD0Q); }

private:
	Combiner fCombiner;

	const ZRef<ZTween<Val0> > f0;
	const ZRef<ZTween<Val1> > f1;

	ZQ<double> fD0Q, fD1Q;
	};

template <class Val0, class Val1>
ZRef<ZTween<Val0> > sTween_With(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{
	if (i0)
		{
		if (i1)
			return new ZTween_With<Val0,Val1>(i0, i1);
		return i0;
		}
	return null;
	}

template <class Val0, class Val1>
ZRef<ZTween<Val0> > operator/(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{ return sTween_With(i0, i1); }

template <class Val0, class Val1>
ZRef<ZTween<Val0> >& operator/=(ZRef<ZTween<Val0> >& io0, const ZRef<ZTween<Val1> >& i1)
	{ return io0 = io0 / i1; }

template <class Val_p>
struct ZTweenAccumulatorCombiner_With
	{
	typedef Val_p Val;
	void operator()(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1) const
		{
		if (io0)
			io0 = sTween_With(io0, i1);
		else
			io0 = i1;
		}
	};

// =================================================================================================
// MARK: - sTween_Repeat (count)

template <class Val>
class ZTween_Repeat
:	public ZTween<Val>
	{
public:
	ZTween_Repeat(const ZRef<ZTween<Val> >& iTween, size_t iCount)
	:	fTween(iTween)
	,	fCount(iCount)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double lPlace)
		{
		const double childWeight = spWeight(fTween, fTweenWeightQ);
		lPlace = sMinMax(0.0, lPlace, childWeight * fCount);
		return fTween->QValAt(fmod(lPlace, childWeight));
		}

	virtual double Weight()
		{ return fCount * spWeight(fTween, fTweenWeightQ); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const size_t fCount;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Repeat(size_t iCount, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Repeat<Val>(iTween, iCount);
	return null;
	}

// =================================================================================================
// MARK: - sTween_RepeatFor

template <class Val>
class ZTween_RepeatFor
:	public ZTween<Val>
	{
public:
	ZTween_RepeatFor(const ZRef<ZTween<Val> >& iTween, double iWeight)
	:	fTween(iTween)
	,	fWeight(iWeight)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < 0 || iPlace >= fWeight)
			return null;
		return fTween->QValAt(fmod(iPlace, spWeight(fTween, fTweenWeightQ)));
		}

	virtual double Weight()
		{ return fWeight; }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fWeight;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_RepeatFor(double iWeight, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_RepeatFor<Val>(iTween, iWeight);
	return null;
	}

// =================================================================================================
// MARK: - sTween_ForExactly

template <class Val>
class ZTween_ForExactly
:	public ZTween<Val>
	{
public:
	ZTween_ForExactly(const ZRef<ZTween<Val> >& iTween, double iWeight)
	:	fTween(iTween)
	,	fWeight(iWeight)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace >= fWeight)
			return null;
		return fTween->QValAt(iPlace);
		}

	virtual double Weight()
		{ return fWeight; }

private:
	const ZRef<ZTween<Val> > fTween;
	const double fWeight;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_ForExactly(double iWeight, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_ForExactly<Val>(iTween, iWeight);
	return null;
	}

// =================================================================================================
// MARK: - sTween_ForAtMost

template <class Val>
class ZTween_ForAtMost
:	public ZTween<Val>
	{
public:
	ZTween_ForAtMost(const ZRef<ZTween<Val> >& iTween, double iAtMost)
	:	fTween(iTween)
	,	fAtMost(iAtMost)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace >= fAtMost)
			return null;
		return fTween->QValAt(iPlace);
		}

	virtual double Weight()
		{ return std::min(fAtMost, spWeight(fTween, fTweenWeightQ)); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fAtMost;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_ForAtMost(double iAtMost, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_ForAtMost<Val>(iTween, iAtMost);
	return null;
	}

// =================================================================================================
// MARK: - sTween_ForAtLeast

template <class Val>
class ZTween_ForAtLeast
:	public ZTween<Val>
	{
public:
	ZTween_ForAtLeast(const ZRef<ZTween<Val> >& iTween, double iAtLeast)
	:	fTween(iTween)
	,	fAtLeast(iAtLeast)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace >= fAtLeast)
			return null;
		return fTween->QValAt(std::min(iPlace, spWeight(fTween, fTweenWeightQ) - 1e-3));
		}

	virtual double Weight()
		{ return std::max(fAtLeast, spWeight(fTween, fTweenWeightQ)); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fAtLeast;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_ForAtLeast(double iAtLeast, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_ForAtLeast<Val>(iTween, iAtLeast);
	return null;
	}

// =================================================================================================
// MARK: - sTween_WeightShift

template <class Val>
class ZTween_WeightShift
:	public ZTween<Val>
	{
public:
	ZTween_WeightShift(const ZRef<ZTween<Val> >& iTween, double iWeightShift)
	:	fTween(iTween)
	,	fWeightShift(iWeightShift)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{ return fTween->QValAt(iPlace - fWeightShift); }

	virtual double Weight()
		{ return std::max(0.0, spWeight(fTween, fTweenWeightQ) + fWeightShift); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fWeightShift;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_WeightShift(double iWeightShift, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_WeightShift<Val>(iTween, iWeightShift);
	return null;
	}

// =================================================================================================
// MARK: - sTween_WeightScale

template <class Val>
class ZTween_WeightScale
:	public ZTween<Val>
	{
public:
	ZTween_WeightScale(const ZRef<ZTween<Val> >& iTween, double iWeightScale)
	:	fTween(iTween)
	,	fWeightScale(iWeightScale)
		{ ZAssert(fWeightScale); }

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (fWeightScale > 0)
			{
			return fTween->QValAt(iPlace / fWeightScale);
			}
		else
			{
			const double theWeight = spWeight(fTween, fTweenWeightQ);
			if (0 <= iPlace && iPlace < theWeight)
				return fTween->QValAt((theWeight*0.999999) + iPlace/fWeightScale);
			return null;
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
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fWeightScale;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_WeightScale(double iWeightScale, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_WeightScale<Val>(iTween, iWeightScale);
	return null;
	}

// =================================================================================================
// MARK: - sTween_WeightPhase

template <class Val>
class ZTween_WeightPhase
:	public ZTween<Val>
	{
public:
	ZTween_WeightPhase(const ZRef<ZTween<Val> >& iTween, double iPhase)
	:	fTween(iTween)
	,	fPhase(iPhase)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		const double theWeight = spWeight(fTween, fTweenWeightQ);
		return fTween->QValAt(fmod(iPlace + fPhase * theWeight, theWeight));
		}

	virtual double Weight()
		{ return spWeight(fTween, fTweenWeightQ); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	const double fPhase;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_WeightPhase(double iPhase, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_WeightPhase<Val>(iTween, iPhase);
	return null;
	}

// =================================================================================================
// MARK: - sTween_WeightNormalize

template <class Val>
class ZTween_WeightNormalize
:	public ZTween<Val>
	{
public:
	ZTween_WeightNormalize(const ZRef<ZTween<Val> >& iTween)
	:	fTween(iTween)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{ return fTween->QValAt(iPlace / spWeight(fTween, fTweenWeightQ)); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_WeightNormalize(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_WeightNormalize<Val>(iTween);
	return null;
	}

// =================================================================================================
// MARK: - sTween_Const

template <class Val>
class ZTween_Const
:	public ZTween<Val>
	{
public:
	ZTween_Const(const Val& iVal, double iWeight)
	:	fVal(iVal)
	,	fWeight(iWeight)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < 0 || iPlace >= fWeight)
			return null;
		return fVal;
		}

	virtual double Weight()
		{ return fWeight; }

private:
	const Val fVal;
	const double fWeight;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Const(const Val& iVal)
	{ return new ZTween_Const<Val>(iVal, 1.0); }

template <class Val>
ZRef<ZTween<Val> > sTween_Const(const Val& iVal, double iWeight)
	{ return new ZTween_Const<Val>(iVal, iWeight); }

// =================================================================================================
// MARK: - sTween_ValScale

template <class Val>
ZRef<ZTween<Val> > sTween_ValScale(Val iValScale, const ZRef<ZTween<Val> >& iTween)
	{ return sTween_With(iTween, sTween_Const<Val>(iValScale)); }

// =================================================================================================
// MARK: - sTween_ValOffset

template <class Val>
class ZTween_ValOffset
:	public ZTween<Val>
	{
public:
	ZTween_ValOffset(const ZRef<ZTween<Val> >& iTween, Val iValOffset)
	:	fTween(iTween)
	,	fValOffset(iValOffset)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (ZQ<Val> theQ = fTween->QValAt(iPlace))
			return *theQ + fValOffset;
		return null;
		}

	virtual double Weight()
		{ return fTween->Weight(); }

private:
	const ZRef<ZTween<Val> > fTween;
	const Val fValOffset;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_ValOffset(Val iValOffset, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_ValOffset<Val>(iTween, iValOffset);
	return null;
	}

// =================================================================================================
// MARK: - sTween_Range

template <class Val>
ZRef<ZTween<Val> > sTween_ValRange(Val iZeroVal, Val iOneVal, const ZRef<ZTween<Val> >& iTween)
	{ return sTween_ValOffset(iZeroVal, sTween_ValScale<Val>(iOneVal - iZeroVal, iTween)); }

// =================================================================================================
// MARK: - sTween_First

template <class Val>
class ZTween_First
:	public ZTween<Val>
	{
public:
	ZTween_First(const ZRef<ZTween<Val> >& iTween)
	:	fTween(iTween)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{ return fTween->QValAt(0); }

private:
	const ZRef<ZTween<Val> > fTween;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_First(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_First<Val>(iTween);
	return null;
	}

// =================================================================================================
// MARK: - sTween_Last

template <class Val>
class ZTween_Last
:	public ZTween<Val>
	{
public:
	ZTween_Last(const ZRef<ZTween<Val> >& iTween)
	:	fTween(iTween)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < 0 || iPlace >= 1)
			return null;

		return fTween->QValAt(spWeight(fTween, fTweenWeightQ) * 0.999);
		}

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenWeightQ;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Last(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Last<Val>(iTween);
	return null;
	}

} // namespace ZooLib

#endif // __ZTween_h__
