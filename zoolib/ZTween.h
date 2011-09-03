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
#include "zoolib/ZCompat_cmath.h" // for fmod
#include "zoolib/ZRef.h"

namespace ZooLib {

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
			iTime = fmod(iTime, duration);
			if (iTime < 0)
				iTime += duration;
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
		{
		if (fTween != iPrior)
			return false;
		fTween = iNew;
		return true;
		}

private:
	ZRef<ZTween<Val> > fTween;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sEither

template <class Val>
class ZTween_Either
:	public ZTween<Val>
	{
public:
	ZTween_Either(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return f0->ValAt(iTime) * f1->ValAt(iTime); }

	virtual double Duration()
		{ return std::max(f0->Duration(), f1->Duration()); }

private:
	const ZRef<ZTween<Val> > f0, f1;
	};

template <class Val>
ZRef<ZTween<Val> > sEither(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{ return new ZTween_Either<Val>(i0, i1); }

template <class Val>
ZRef<ZTween<Val> > operator+(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{ return sEither(i0, i1); }

template <class Val>
ZRef<ZTween<Val> >& operator+=(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1)
	{ return io0 = sEither(io0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark * sBoth

template <class Val>
class ZTween_Both
:	public ZTween<Val>
	{
public:
	ZTween_Both(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return f0->ValAt(iTime) * f1->ValAt(iTime); }

	virtual double Duration()
		{ return std::min(f0->Duration(), f1->Duration()); }

private:
	const ZRef<ZTween<Val> > f0, f1;
	};

template <class Val>
ZRef<ZTween<Val> > sBoth(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{ return new ZTween_Both<Val>(i0, i1); }

template <class Val>
ZRef<ZTween<Val> > operator*(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{ return sBoth(i0, i1); }

template <class Val>
ZRef<ZTween<Val> >& operator*=(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1)
	{ return io0 = io0 * i1; }

// =================================================================================================
#pragma mark -
#pragma mark * sEach

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
		if (iTime < 0)
			{
			return f0->ValAt(0);
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
ZRef<ZTween<Val> > sEach(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{ return new ZTween_Each<Val>(i0, i1); }

template <class Val>
ZRef<ZTween<Val> > operator|(const ZRef<ZTween<Val> >& i0, const ZRef<ZTween<Val> >& i1)
	{ return sEach(i0, i1); }

template <class Val>
ZRef<ZTween<Val> >& operator|=(ZRef<ZTween<Val> >& io0, const ZRef<ZTween<Val> >& i1)
	{ return io0 = sEach(io0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeat (count)

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
		iTime = std::min(std::max(0.0, iTime), childDuration * fCount);
		return fTween->ValAt(fmod(iTime, childDuration));
		}

	virtual double Duration()
		{ return fCount * fTween->Duration(); }

private:
	const ZRef<ZTween<Val> > fTween;
	const size_t fCount;
	};

template <class Val>
ZRef<ZTween<Val> > sRepeat(size_t iCount, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Repeat<Val>(iTween, iCount); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeatFor

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
		iTime = std::min(std::max(0.0, iTime), fDuration);
		return fTween->ValAt(fmod(iTime, fTween->Duration()));
		}

	virtual double Duration()
		{ return fDuration; }

private:
	const ZRef<ZTween<Val> > fTween;
	const double fDuration;
	};

template <class Val>
ZRef<ZTween<Val> > sRepeatFor(double iDuration, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_RepeatFor<Val>(iTween, iDuration); }

// =================================================================================================
#pragma mark -
#pragma mark * sDurationAtMost

template <class Val>
class ZTween_DurationAtMost
:	public ZTween<Val>
	{
public:
	ZTween_DurationAtMost(const ZRef<ZTween<Val> >& iTween, double iAtMost)
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
ZRef<ZTween<Val> > sDurationAtMost(double iDuration, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_DurationAtMost<Val>(iTween, iDuration); }

// =================================================================================================
#pragma mark -
#pragma mark * sDurationAtLeast

template <class Val>
class ZTween_DurationAtLeast
:	public ZTween<Val>
	{
public:
	ZTween_DurationAtLeast(const ZRef<ZTween<Val> >& iTween, double iAtLeast)
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
ZRef<ZTween<Val> > sDurationAtLeast(double iAtLeast, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_DurationAtLeast<Val>(iTween, iAtLeast); }

// =================================================================================================
#pragma mark -
#pragma mark * sTimeOffset

template <class Val>
class ZTween_TimeOffset
:	public ZTween<Val>
	{
public:
	ZTween_TimeOffset(const ZRef<ZTween<Val> >& iTween, double iOffset)
	:	fTween(iTween)
	,	fOffset(iOffset)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime - fOffset); }

	virtual double Duration()
		{ return fTween->Duration() + fOffset; }

private:
	const ZRef<ZTween<Val> > fTween;
	const double fOffset;
	};

template <class Val>
ZRef<ZTween<Val> > sTimeOffset(double iOffset, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_TimeOffset<Val>(iTween, iOffset); }

// =================================================================================================
#pragma mark -
#pragma mark * sDurationScale

template <class Val>
class ZTween_DurationScale
:	public ZTween<Val>
	{
public:
	ZTween_DurationScale(const ZRef<ZTween<Val> >& iTween, double iScale)
	:	fTween(iTween)
	,	fScale(iScale)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime / fScale); }

	virtual double Duration()
		{ return fScale * fTween->Duration(); }

private:
	const ZRef<ZTween<Val> > fTween;
	const double fScale;
	};

template <class Val>
ZRef<ZTween<Val> > sDurationScale(double iScale, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_DurationScale<Val>(iTween, iScale); }

// =================================================================================================
#pragma mark -
#pragma mark * sValAtMost

template <class Val>
class ZTween_ValAtMost
:	public ZTween<Val>
	{
public:
	ZTween_ValAtMost(const ZRef<ZTween<Val> >& iTween, Val iAtMost)
	:	fTween(iTween)
	,	fAtMost(iAtMost)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return std::min(fAtMost, fTween->ValAt(iTime)); }

	virtual double Duration()
		{ return fTween->Duration(); }

private:
	const ZRef<ZTween<Val> > fTween;
	const Val fAtMost;
	};

template <class Val>
ZRef<ZTween<Val> > sValAtMost(Val iAtMost, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_ValAtMost<Val>(iTween, iAtMost); }

// =================================================================================================
#pragma mark -
#pragma mark * sValAtLeast

template <class Val>
class ZTween_ValAtLeast
:	public ZTween<Val>
	{
public:
	ZTween_ValAtLeast(const ZRef<ZTween<Val> >& iTween, Val iAtLeast)
	:	fTween(iTween)
	,	fAtLeast(iAtLeast)
		{}

// From ZTween
	virtual Val ValAt(double iTime)
		{ return std::max(fAtLeast, fTween->ValAt(iTime)); }

	virtual double Duration()
		{ return fTween->Duration(); }

private:
	const ZRef<ZTween<Val> > fTween;
	const Val fAtLeast;
	};

template <class Val>
ZRef<ZTween<Val> > sValAtLeast(Val iAtLeast, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_ValAtLeast<Val>(iTween, iAtLeast); }

// =================================================================================================
#pragma mark -
#pragma mark * sValOffset

template <class Val>
class ZTween_ValOffset
:	public ZTween<Val>
	{
public:
	ZTween_ValOffset(const ZRef<ZTween<Val> >& iTween, Val iOffset)
	:	fTween(iTween)
	,	fOffset(iOffset)
		{}

// From ZTween via ZTween_ValFilter
	virtual Val ValAt(double iTime)
		{ return fOffset + fTween->ValAt(iTime); }

	virtual double Duration()
		{ return fTween->Duration(); }

private:
	const ZRef<ZTween<Val> > fTween;
	const Val fOffset;
	};

template <class Val>
ZRef<ZTween<Val> > sValOffset(Val iOffset, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_ValOffset<Val>(iTween, iOffset); }

// =================================================================================================
#pragma mark -
#pragma mark * sValScale

template <class Val>
class ZTween_ValScale
:	public ZTween<Val>
	{
public:
	ZTween_ValScale(const ZRef<ZTween<Val> >& iTween, Val iScale)
	:	fTween(iTween)
	,	fScale(iScale)
		{}

// From ZTween via ZTween
	virtual Val ValAt(double iTime)
		{ return fScale * fTween->ValAt(iTime); }

	virtual double Duration()
		{ return fTween->Duration(); }

private:
	const ZRef<ZTween<Val> > fTween;
	const Val fScale;
	};

template <class Val>
ZRef<ZTween<Val> > sValScale(Val iScale, const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_ValScale<Val>(iTween, iScale); }

} // namespace ZooLib

#endif // __ZTween__
