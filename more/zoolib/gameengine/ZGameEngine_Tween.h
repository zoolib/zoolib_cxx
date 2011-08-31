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

#ifndef __ZGameEngine_Tween__
#define __ZGameEngine_Tween__ 1
#include "zconfig.h"

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZCompat_cmath.h" // for fmod
#include "zoolib/ZRef.h"

namespace ZooLib {
namespace ZGameEngine {

// =================================================================================================
#pragma mark -
#pragma mark * ZGameEngine::Tween

template <class Val_t>
class Tween
:	public ZCountedWithoutFinalize
	{
protected:
	Tween() {}

public:
	typedef Val_t Val;

	virtual ~Tween() {}

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
#pragma mark * Tween_ValFilter

template <class Val>
class Tween_ValFilter
:	public Tween<Val>
	{
public:
	Tween_ValFilter(const ZRef<Tween<Val> >& iTween)
	:	fTween(iTween)
		{}

	virtual double Duration()
		{ return fTween->Duration(); }

protected:
	const ZRef<Tween<Val> > fTween;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sEither

template <class Val>
class Tween_Either : public Tween<Val>
	{
public:
	Tween_Either(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	:	f0(i0), f1(i1) {}

	virtual Val ValAt(double iTime)
		{ return f0->ValAt(iTime) * f1->ValAt(iTime); }

	virtual double Duration()
		{ return std::max(f0->Duration(), f1->Duration()); }

private:
	const ZRef<Tween<Val> > f0, f1;
	};

template <class Val>
ZRef<Tween<Val> > sEither(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{ return new Tween_Either<Val>(i0, i1); }

template <class Val>
ZRef<Tween<Val> > operator+(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{ return sEither(i0, i1); }

template <class Val>
ZRef<Tween<Val> >& operator+=(ZRef<Tween<Val> >& io0, const ZRef<Tween<Val> >& i1)
	{ return io0 = sEither(io0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark * sBoth

template <class Val>
class Tween_Both : public Tween<Val>
	{
public:
	Tween_Both(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	:	f0(i0), f1(i1) {}

	virtual Val ValAt(double iTime)
		{ return f0->ValAt(iTime) * f1->ValAt(iTime); }

	virtual double Duration()
		{ return std::min(f0->Duration(), f1->Duration()); }

private:
	const ZRef<Tween<Val> > f0, f1;
	};

template <class Val>
ZRef<Tween<Val> > sBoth(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{ return new Tween_Both<Val>(i0, i1); }

template <class Val>
ZRef<Tween<Val> > operator*(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{ return sBoth(i0, i1); }

template <class Val>
ZRef<Tween<Val> >& operator*=(ZRef<Tween<Val> >& io0, const ZRef<Tween<Val> >& i1)
	{ return io0 = io0 * i1; }

// =================================================================================================
#pragma mark -
#pragma mark * sEach

template <class Val>
class Tween_Each : public Tween<Val>
	{
public:
	Tween_Each(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	:	f0(i0), f1(i1) {}

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
	const ZRef<Tween<Val> > f0, f1;
	};

template <class Val>
ZRef<Tween<Val> > sEach(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{ return new Tween_Each<Val>(i0, i1); }

template <class Val>
ZRef<Tween<Val> > operator|(const ZRef<Tween<Val> >& i0, const ZRef<Tween<Val> >& i1)
	{ return sEach(i0, i1); }

template <class Val>
ZRef<Tween<Val> >& operator|=(ZRef<Tween<Val> >& io0, const ZRef<Tween<Val> >& i1)
	{ return io0 = sEach(io0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeat (count)

template <class Val>
class Tween_Repeat : public Tween<Val>
	{
public:
	Tween_Repeat(const ZRef<Tween<Val> >& iTween, size_t iCount)
	:	fTween(iTween), fCount(iCount) {}

	virtual Val ValAt(double iTime)
		{
		const double childDuration = fTween->Duration();
		iTime = std::min(std::max(0.0, iTime), childDuration * fCount);
		return fTween->ValAt(fmod(iTime, childDuration));
		}

	virtual double Duration()
		{ return fCount * fTween->Duration(); }

private:
	const ZRef<Tween<Val> > fTween;
	const size_t fCount;
	};

template <class Val>
ZRef<Tween<Val> > sRepeat(size_t iCount, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_Repeat<Val>(iTween, iCount); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeatFor

template <class Val>
class Tween_RepeatFor : public Tween<Val>
	{
public:
	Tween_RepeatFor(const ZRef<Tween<Val> >& iTween, double iDuration)
	:	fTween(iTween), fDuration(iDuration)
		{}

	virtual Val ValAt(double iTime)
		{
		iTime = std::min(std::max(0.0, iTime), fDuration);
		return fTween->ValAt(fmod(iTime, fTween->Duration()));
		}

	virtual double Duration()
		{ return fDuration; }

private:
	const ZRef<Tween<Val> > fTween;
	const double fDuration;
	};

template <class Val>
ZRef<Tween<Val> > sRepeatFor(double iDuration, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_RepeatFor<Val>(iTween, iDuration); }

// =================================================================================================
#pragma mark -
#pragma mark * sDurationAtMost

template <class Val>
class Tween_DurationAtMost : public Tween<Val>
	{
public:
	Tween_DurationAtMost(const ZRef<Tween<Val> >& iTween, double iAtMost)
	:	fTween(iTween), fAtMost(iAtMost) {}

	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime); }

	virtual double Duration()
		{ return std::min(fAtMost, fTween->Duration()); }

private:
	const ZRef<Tween<Val> > fTween;
	const double fAtMost;
	};

template <class Val>
ZRef<Tween<Val> > sDurationAtMost(double iDuration, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_DurationAtMost<Val>(iTween, iDuration); }

// =================================================================================================
#pragma mark -
#pragma mark * sDurationAtLeast

template <class Val>
class Tween_DurationAtLeast : public Tween<Val>
	{
public:
	Tween_DurationAtLeast(const ZRef<Tween<Val> >& iTween, double iAtLeast)
	:	fTween(iTween), fAtLeast(iAtLeast) {}

	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime); }

	virtual double Duration()
		{ return std::max(fAtLeast, fTween->Duration()); }

private:
	const ZRef<Tween<Val> > fTween;
	const double fAtLeast;
	};

template <class Val>
ZRef<Tween<Val> > sDurationAtLeast(double iAtLeast, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_DurationAtLeast<Val>(iTween, iAtLeast); }

// =================================================================================================
#pragma mark -
#pragma mark * sTimeOffset

template <class Val>
class Tween_TimeOffset : public Tween<Val>
	{
public:
	Tween_TimeOffset(const ZRef<Tween<Val> >& iTween, double iOffset)
	:	fTween(iTween), fOffset(iOffset) {}

	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime - fOffset); }

	virtual double Duration()
		{ return fTween->Duration() - fOffset; }

private:
	const ZRef<Tween<Val> > fTween;
	const double fOffset;
	};

template <class Val>
ZRef<Tween<Val> > sTimeOffset(double iOffset, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_TimeOffset<Val>(iTween, iOffset); }

// =================================================================================================
#pragma mark -
#pragma mark * sDurationScale

template <class Val>
class Tween_DurationScale : public Tween<Val>
	{
public:
	Tween_DurationScale(const ZRef<Tween<Val> >& iTween, double iScale)
	:	fTween(iTween), fScale(iScale) {}

	virtual Val ValAt(double iTime)
		{ return fTween->ValAt(iTime / fScale); }

	virtual double Duration()
		{ return fScale * fTween->Duration(); }

private:
	const ZRef<Tween<Val> > fTween;
	const double fScale;
	};

template <class Val>
ZRef<Tween<Val> > sDurationScale(double iScale, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_DurationScale<Val>(iTween, iScale); }

// =================================================================================================
#pragma mark -
#pragma mark * sValAtMost

template <class Val>
class Tween_ValAtMost : public Tween_ValFilter<Val>
	{
public:
	Tween_ValAtMost(const ZRef<Tween<Val> >& iTween, Val iAtMost)
	:	Tween_ValFilter<Val>(iTween), fAtMost(iAtMost) {}

	virtual Val ValAt(double iTime)
		{ return std::min(fAtMost, this->fTween->ValAt(iTime)); }

private:
	const Val fAtMost;
	};

template <class Val>
ZRef<Tween<Val> > sValAtMost(Val iAtMost, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_ValAtMost<Val>(iTween, iAtMost); }

// =================================================================================================
#pragma mark -
#pragma mark * sValAtLeast

template <class Val>
class Tween_ValAtLeast : public Tween_ValFilter<Val>
	{
public:
	Tween_ValAtLeast(const ZRef<Tween<Val> >& iTween, Val iAtLeast)
	:	Tween_ValFilter<Val>(iTween), fAtLeast(iAtLeast) {}

	virtual Val ValAt(double iTime)
		{ return std::max(fAtLeast, this->fTween->ValAt(iTime)); }

private:
	const Val fAtLeast;
	};

template <class Val>
ZRef<Tween<Val> > sValAtLeast(Val iAtLeast, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_ValAtLeast<Val>(iTween, iAtLeast); }

// =================================================================================================
#pragma mark -
#pragma mark * sValOffset

template <class Val>
class Tween_ValOffset : public Tween_ValFilter<Val>
	{
public:
	Tween_ValOffset(const ZRef<Tween<Val> >& iTween, Val iOffset)
	:	Tween_ValFilter<Val>(iTween), fOffset(iOffset) {}

	virtual Val ValAt(double iTime)
		{ return fOffset + this->fTween->ValAt(iTime); }

private:
	const Val fOffset;
	};

template <class Val>
ZRef<Tween<Val> > sValOffset(Val iOffset, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_ValOffset<Val>(iTween, iOffset); }

// =================================================================================================
#pragma mark -
#pragma mark * sValScale

template <class Val>
class Tween_ValScale : public Tween_ValFilter<Val>
	{
public:
	Tween_ValScale(const ZRef<Tween<Val> >& iTween, Val iScale)
	:	Tween_ValFilter<Val>(iTween), fScale(iScale) {}

	virtual Val ValAt(double iTime)
		{ return fScale * this->fTween->ValAt(iTime); }

private:
	const Val fScale;
	};

template <class Val>
ZRef<Tween<Val> > sValScale(Val iScale, const ZRef<Tween<Val> >& iTween)
	{ return new Tween_ValScale<Val>(iTween, iScale); }

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Tween__
