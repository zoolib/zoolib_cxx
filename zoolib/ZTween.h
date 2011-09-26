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

#ifndef __ZTween__
#define __ZTween__ 1
#include "zconfig.h"

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZCompat_algorithm.h" // for std::binary_function
#include "zoolib/ZCompat_cmath.h" // for fmod
#include "zoolib/ZRef.h"

namespace ZooLib {

template <class Val0, class Val1>
Val0 sCombineTweenVals(const Val0& iVal0, const Val1& iVal1);

template <class Val0, class Val1>
struct TweenCombiner : public std::binary_function<Val0, Val1, Val0>
	{
	Val0 operator()(const Val0& i0, const Val1& i1) const
		{ return sCombineTweenVals<Val0,Val1>(i0, i1); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTween

/**
A ZTween has a duration and returns a value given a time within that duration.
If given a negative time the value at zero is returned, if given a time after
the duration the value at the duration is returned.
*/

template <class Val_t>
class ZTween
:	public ZCountedWithoutFinalize
	{
protected:
	ZTween() {}

public:
	typedef Val_t Val;

	virtual ~ZTween() {}

// Our protocol
	Val ValAt(bool iWrap, double iTime)
		{
		if (iWrap)
			{
			const double duration = this->Duration();
			if (duration <= 0)
				{
				iTime = 0;
				}
			else
				{
				iTime = fmod(iTime, duration);
				if (iTime < 0)
					iTime += duration;
				}
			}
		return this->ValAt(iTime);
		}

	virtual Val ValAt(double iTime) = 0;
	virtual double Duration() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTween_Indirect

template <class Val>
class ZTween_Indirect
:	public ZTween<Val>
	{
public:
	ZTween_Indirect()
		{}

	ZTween_Indirect(const ZRef<ZTween<Val> >& iTween)
	:	fTween(iTween) {}

// From ZTween
	virtual Val ValAt(double iTime)
		{
		if (ZRef<ZTween<Val> > theTween = fTween)
			return theTween->ValAt(iTime);
		return Val();
		}

	virtual double Duration()
		{
		if (ZRef<ZTween<Val> > theTween = fTween)
			return theTween->Duration();
		return 0;
		}

	ZRef<ZTween<Val> > Get()
		{ return fTween; }

	void Set(const ZRef<ZTween<Val> >& iTween)
		{ fTween = iTween; }

	bool CAS(const ZRef<ZTween<Val> >& iPrior, const ZRef<ZTween<Val> >& iNew)
		{ return fTween.CAS(iPrior, iNew); }

private:
	ZRef<ZTween<Val> > fTween;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Either

template <class Val0, class Val1, class Combiner = TweenCombiner<Val0,Val1> >
class ZTween_Either
:	public ZTween<Val0>
	{
public:
	ZTween_Either(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZTween
	virtual Val0 ValAt(double iTime)
		{ return fCombiner(f0->ValAt(iTime), f1->ValAt(iTime)); }

	virtual double Duration()
		{ return std::max(f0->Duration(), f1->Duration()); }

private:
	Combiner fCombiner;
	const ZRef<ZTween<Val0> > f0;
	const ZRef<ZTween<Val1> > f1;
	};

template <class Val0, class Val1>
ZRef<ZTween<Val0> > sTween_Either(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{
	ZAssert(i0 && i1);
	return new ZTween_Either<Val0,Val1>(i0, i1);
	}

template <class Val0, class Val1>
ZRef<ZTween<Val0> > operator+(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{ return sTween_Either(i0, i1); }

template <class Val0, class Val1>
ZRef<ZTween<Val0> >& operator+=(ZRef<ZTween<Val0> >& io0, const ZRef<ZTween<Val1> >& i1)
	{ return io0 = io0 + i1; }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Both

template <class Val0, class Val1, class Combiner = TweenCombiner<Val0,Val1> >
class ZTween_Both
:	public ZTween<Val0>
	{
public:
	ZTween_Both(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZTween
	virtual Val0 ValAt(double iTime)
		{ return fCombiner(f0->ValAt(iTime), f1->ValAt(iTime)); }

	virtual double Duration()
		{ return std::min(f0->Duration(), f1->Duration()); }

private:
	Combiner fCombiner;
	const ZRef<ZTween<Val0> > f0;
	const ZRef<ZTween<Val1> > f1;
	};

template <class Val0, class Val1>
ZRef<ZTween<Val0> > sTween_Both(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{
	ZAssert(i0 && i1);
	return new ZTween_Both<Val0,Val1>(i0, i1);
	}

template <class Val0, class Val1>
ZRef<ZTween<Val0> > operator*(const ZRef<ZTween<Val0> >& i0, const ZRef<ZTween<Val1> >& i1)
	{ return sTween_Both(i0, i1); }

template <class Val0, class Val1>
ZRef<ZTween<Val0> >& operator*=(ZRef<ZTween<Val0> >& io0, const ZRef<ZTween<Val1> >& i1)
	{ return io0 = io0 * i1; }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Each

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
	virtual Val ValAt(double iTime)
		{
		if (iTime <= 0)
			{
			return f0->ValAt(iTime);
			}
		else
			{
			const double duration0 = f0->Duration();
			if (iTime < duration0)
				return f0->ValAt(iTime);
			else
				return f1->ValAt(iTime - duration0);
			}
		}

	virtual double Duration()
		{ return f0->Duration() + f1->Duration(); }

private:
	const ZRef<ZTween<Val> > f0, f1;
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
ZRef<ZTween<Val> > operator|(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{ return sTween_Each(i0, i1); }

template <class Val>
ZRef<ZTween<Val> >& operator|=(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1)
	{ return io0 = io0 | i1; }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Repeat (count)

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
	virtual Val ValAt(double iTime)
		{
		const double childDuration = fTween->Duration();
		iTime = sMinMax(0.0, iTime, childDuration * fCount);
		return fTween->ValAt(fmod(iTime, childDuration));
		}

	virtual double Duration()
		{ return fCount * fTween->Duration(); }

private:
	const ZRef<ZTween<Val> > fTween;
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
#pragma mark -
#pragma mark * sTween_RepeatFor

template <class Val>
class ZTween_RepeatFor
:	public ZTween<Val>
	{
public:
	ZTween_RepeatFor(const ZRef<ZTween<Val> >& iTween, double iDuration)
	:	fTween(iTween)
	,	fDuration(iDuration)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{
		iTime = sMinMax(0.0, iTime, fDuration);
		return fTween->ValAt(fmod(iTime, fTween->Duration()));
		}

	virtual double Duration()
		{ return fDuration; }

private:
	const ZRef<ZTween<Val> > fTween;
	const double fDuration;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_RepeatFor(double iDuration, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_RepeatFor<Val>(iTween, iDuration);
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sTween_ForExactly

template <class Val>
class ZTween_ForExactly
:	public ZTween<Val>
	{
public:
	ZTween_ForExactly(const ZRef<ZTween<Val> >& iTween, double iDuration)
	:	fTween(iTween)
	,	fDuration(iDuration)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime); }

	virtual double Duration()
		{ return fDuration; }

private:
	const ZRef<ZTween<Val> > fTween;
	const double fDuration;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_ForExactly(double iDuration, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_ForExactly<Val>(iTween, iDuration);
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sTween_ForAtMost

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
	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime); }

	virtual double Duration()
		{ return std::min(fAtMost, fTween->Duration()); }

private:
	const ZRef<ZTween<Val> > fTween;
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
#pragma mark -
#pragma mark * sTween_ForAtLeast

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
	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime); }

	virtual double Duration()
		{ return std::max(fAtLeast, fTween->Duration()); }

private:
	const ZRef<ZTween<Val> > fTween;
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
#pragma mark -
#pragma mark * sTween_Delay

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
	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime - fDelay); }

	virtual double Duration()
		{ return std::max(0.0, fTween->Duration() + fDelay); }

private:
	const ZRef<ZTween<Val> > fTween;
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
#pragma mark -
#pragma mark * sTween_Rate

template <class Val>
class ZTween_Rate
:	public ZTween<Val>
	{
public:
	ZTween_Rate(const ZRef<ZTween<Val> >& iTween, double iRate)
	:	fTween(iTween)
	,	fRate(iRate)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{
		if (fRate > 0)
			return fTween->ValAt(iTime / fRate);
		else
			return fTween->ValAt((fTween->Duration() - iTime) / -fRate);
		}

	virtual double Duration()
		{
		if (fRate > 0)
			return fRate * fTween->Duration();
		else 
			return -fRate * fTween->Duration();
		}

private:
	const ZRef<ZTween<Val> > fTween;
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
#pragma mark -
#pragma mark * sTween_Const

template <class Val>
class ZTween_Const
:	public ZTween<Val>
	{
public:
	ZTween_Const(const Val& iVal, double iDuration)
	:	fVal(iVal)
	,	fDuration(iDuration)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return fVal; }

	virtual double Duration()
		{ return fDuration; }

private:
	const Val fVal;
	const double fDuration;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Const(const Val& iVal)
	{ return new ZTween_Const<Val>(iVal, 1.0); }

template <class Val>
ZRef<ZTween<Val> > sTween_Const(const Val& iVal, double iDuration)
	{ return new ZTween_Const<Val>(iVal, iDuration); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Scale

template <class Val>
class ZTween_Scale
:	public ZTween<Val>
	{
public:
	ZTween_Scale(const ZRef<ZTween<Val> >& iTween, Val iScale)
	:	fTween(iTween)
	,	fScale(iScale)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return fScale * fTween->ValAt(iTime); }

	virtual double Duration()
		{ return fTween->Duration(); }

private:
	const ZRef<ZTween<Val> > fTween;
	const Val fScale;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Scale(Val iScale, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Scale<Val>(iTween, iScale);
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Offset

template <class Val>
class ZTween_Offset
:	public ZTween<Val>
	{
public:
	ZTween_Offset(const ZRef<ZTween<Val> >& iTween, Val iOffset)
	:	fTween(iTween)
	,	fOffset(iOffset)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return fOffset + fTween->ValAt(iTime); }

	virtual double Duration()
		{ return fTween->Duration(); }

private:
	const ZRef<ZTween<Val> > fTween;
	const Val fOffset;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_Offset(Val iOffset, const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Offset<Val>(iTween, iOffset);
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Range

template <class Val>
ZRef<ZTween<Val> > sTween_Range(Val iZeroVal, Val iOneVal, const ZRef<ZTween<Val> >& iTween)
	{ return sTween_Offset(iZeroVal, sTween_Scale(iOneVal - iZeroVal, iTween)); }

} // namespace ZooLib

#endif // __ZTween__
