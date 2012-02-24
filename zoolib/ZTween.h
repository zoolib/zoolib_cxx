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
	ZQ<Val0> operator()(const ZQ<Val0>& i0, const ZQ<Val1>& i1) const
		{ return sCombineTweenVals<Val0,Val1>(i0, i1); }
	};

// =================================================================================================
// MARK: - ZTween

/**
A ZTween has an extent and returns a value given a place within that extent.
If given a time outside that range, returns null.
*/

template <class Val_p>
class ZTween
:	public ZCountedWithoutFinalize
	{
protected:
	ZTween() {}

public:
	typedef Val_p Val;

	virtual ~ZTween() {}

// Our protocol
	Val ValAtWrapped(double lPlace)
		{
		const double extent = this->Extent();
		if (extent <= 0)
			{
			lPlace = 0;
			}
		else
			{
			lPlace = fmod(lPlace, extent);
			if (lPlace < 0)
				lPlace += extent;
			}
		return this->QValAt(lPlace).Get();
		}

	virtual ZQ<Val> QValAt(double iPlace) = 0;

	virtual double Extent()
		{ return 1; }
	};

// =================================================================================================
// MARK: - spExtent

template <class Val>
double spExtent(const ZRef<ZTween<Val> >& iTween, ZQ<double>& ioCache)
	{
	if (not ioCache)
		ioCache = iTween->Extent();
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
	
	virtual double Extent()
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

	virtual double Extent()
		{ return std::max(spExtent(f0, fD0Q), spExtent(f1, fD1Q)); }

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

	virtual double Extent()
		{ return std::min(spExtent(f0, fD0Q), spExtent(f1, fD1Q)); }

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
		if (iPlace < spExtent(f0, fD0Q))
			return f0->QValAt(iPlace);
		else
			return f1->QValAt(iPlace - spExtent(f0, fD0Q));
		}

	virtual double Extent()
		{ return spExtent(f0, fD0Q) + spExtent(f1, fD1Q); }

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
		const double proportion = iPlace / spExtent(f0, fD0Q);
		return fCombiner(f0->QValAt(iPlace), f1->QValAt(proportion * spExtent(f1, fD1Q)));
		}

	virtual double Extent()
		{ return spExtent(f0, fD0Q); }

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
		const double childExtent = spExtent(fTween, fTweenExtentQ);
		lPlace = sMinMax(0.0, lPlace, childExtent * fCount);
		return fTween->QValAt(fmod(lPlace, childExtent));
		}

	virtual double Extent()
		{ return fCount * spExtent(fTween, fTweenExtentQ); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenExtentQ;
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
	ZTween_RepeatFor(const ZRef<ZTween<Val> >& iTween, double iExtent)
	:	fTween(iTween)
	,	fExtent(iExtent)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < 0 || iPlace >= fExtent)
			return null;
		return fTween->QValAt(fmod(iPlace, spExtent(fTween, fTweenExtentQ)));
		}

	virtual double Extent()
		{ return fExtent; }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenExtentQ;
	const double fExtent;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_RepeatFor(double iExtent, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_RepeatFor<Val>(iTween, iExtent);
	return null;
	}

// =================================================================================================
// MARK: - sTween_ForExactly

template <class Val>
class ZTween_ForExactly
:	public ZTween<Val>
	{
public:
	ZTween_ForExactly(const ZRef<ZTween<Val> >& iTween, double iExtent)
	:	fTween(iTween)
	,	fExtent(iExtent)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace >= fExtent)
			return null;
		return fTween->QValAt(iPlace);
		}

	virtual double Extent()
		{ return fExtent; }

private:
	const ZRef<ZTween<Val> > fTween;
	const double fExtent;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_ForExactly(double iExtent, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_ForExactly<Val>(iTween, iExtent);
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

	virtual double Extent()
		{ return std::min(fAtMost, spExtent(fTween, fTweenExtentQ)); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenExtentQ;
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
		return fTween->QValAt(std::min(iPlace, spExtent(fTween, fTweenExtentQ) - 1e-3));
		}

	virtual double Extent()
		{ return std::max(fAtLeast, spExtent(fTween, fTweenExtentQ)); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenExtentQ;
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
// MARK: - sTween_Delay

template <class Val>
class ZTween_Delay
:	public ZTween<Val>
	{
public:
	ZTween_Delay(const ZRef<ZTween<Val> >& iTween, double iDelay)
	:	fTween(iTween)
	,	fDelay(iDelay)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{ return fTween->QValAt(iPlace - fDelay); }

	virtual double Extent()
		{ return std::max(0.0, spExtent(fTween, fTweenExtentQ) + fDelay); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenExtentQ;
	const double fDelay;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Delay(double iDelay, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Delay<Val>(iTween, iDelay);
	return null;
	}

// =================================================================================================
// MARK: - sTween_Rate

template <class Val>
class ZTween_Rate
:	public ZTween<Val>
	{
public:
	ZTween_Rate(const ZRef<ZTween<Val> >& iTween, double iRate)
	:	fTween(iTween)
	,	fRate(iRate)
		{ ZAssert(fRate); }

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (fRate > 0)
			return fTween->QValAt(iPlace / fRate);
		else
			return fTween->QValAt(spExtent(fTween, fTweenExtentQ) + iPlace/fRate);
		}

	virtual double Extent()
		{
		if (fRate > 0)
			return fRate * spExtent(fTween, fTweenExtentQ);
		else 
			return -fRate * spExtent(fTween, fTweenExtentQ);
		}

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenExtentQ;
	const double fRate;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Rate(double iRate, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Rate<Val>(iTween, iRate);
	return null;
	}

// =================================================================================================
// MARK: - sTween_Phase

template <class Val>
class ZTween_Phase
:	public ZTween<Val>
	{
public:
	ZTween_Phase(const ZRef<ZTween<Val> >& iTween, double iPhase)
	:	fTween(iTween)
	,	fPhase(iPhase)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		const double theExtent = spExtent(fTween, fTweenExtentQ);
		return fTween->QValAt(fmod(iPlace + fPhase * theExtent, theExtent));
		}

	virtual double Extent()
		{ return spExtent(fTween, fTweenExtentQ); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenExtentQ;
	const double fPhase;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Phase(double iPhase, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Phase<Val>(iTween, iPhase);
	return null;
	}

// =================================================================================================
// MARK: - sTween_Normalize

template <class Val>
class ZTween_Normalize
:	public ZTween<Val>
	{
public:
	ZTween_Normalize(const ZRef<ZTween<Val> >& iTween)
	:	fTween(iTween)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{ return fTween->QValAt(iPlace / spExtent(fTween, fTweenExtentQ)); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenExtentQ;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Normalize(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Normalize<Val>(iTween);
	return null;
	}

// =================================================================================================
// MARK: - sTween_Const

template <class Val>
class ZTween_Const
:	public ZTween<Val>
	{
public:
	ZTween_Const(const Val& iVal, double iExtent)
	:	fVal(iVal)
	,	fExtent(iExtent)
		{}

// From ZTween
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < 0 || iPlace >= fExtent)
			return null;
		return fVal;
		}

	virtual double Extent()
		{ return fExtent; }

private:
	const Val fVal;
	const double fExtent;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Const(const Val& iVal)
	{ return new ZTween_Const<Val>(iVal, 1.0); }

template <class Val>
ZRef<ZTween<Val> > sTween_Const(const Val& iVal, double iExtent)
	{ return new ZTween_Const<Val>(iVal, iExtent); }

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

	virtual double Extent()
		{ return spExtent(fTween, fTweenExtentQ); }

private:
	const ZRef<ZTween<Val> > fTween;
	ZQ<double> fTweenExtentQ;
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

} // namespace ZooLib

#endif // __ZTween_h__
