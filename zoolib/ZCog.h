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

#ifndef __ZCog__
#define __ZCog__ 1

#include "zoolib/ZCallable.h"
#include "zoolib/ZCallable_Bind.h"
#include "zoolib/ZCallable_Function.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCog

template <class Param_p>
class ZCog
:	public ZRef<ZCallable<ZCog<Param_p>(const ZCog<Param_p>&,Param_p)> >
	{
public:
	typedef Param_p Param;
	typedef ZCog(Signature)(const ZCog& iSelf, Param iParam);
	typedef ZCallable<Signature> Callable;
	typedef ZRef<Callable> inherited;

//--

	ZCog()
		{}

	ZCog(const ZCog& iOther)
	:	inherited(iOther)
		{}

	~ZCog()
		{}

	ZCog& operator=(const ZCog& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

//--

	ZCog(Callable* iCallable)
	:	inherited(iCallable)
		{}

	ZCog& operator=(Callable* iCallable)
		{
		inherited::operator=(iCallable);
		return *this;
		}

//--

	ZCog(const ZRef<Callable>& iOther)
	:	inherited(iOther)
		{}
	
	ZCog& operator=(const ZRef<Callable>& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

//--

	ZCog(const null_t&)
		{}

	ZCog& operator=(const null_t& iNull)
		{
		inherited::operator=(iNull);
		return *this;
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCallCog

template <class Cog>
Cog sCallCog(const Cog& iCog, const typename Cog::Param iParam)
	{ return sCall(iCog, iCog, iParam); }

template <class Param>
ZCog<Param> sCallCog
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable, Param iParam)
	{ return sCall(iCallable, iCallable, iParam); }

template <class Param>
ZCog<const Param&> sCallCog
	(const ZRef<ZCallable<ZCog<const Param&>(const ZCog<const Param&>&,const Param&)> >& iCallable,
	const Param& iParam)
	{ return sCall(iCallable, iCallable, iParam); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog

const struct
	{
	template <class Param>
	ZCog<Param> operator()
		(ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>* iCallable) const
		{ return iCallable; }

	template <class Param>
	ZCog<Param> operator()
		(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable) const
		{ return iCallable; }

	template <class Param>
	const ZCog<Param>& operator()
		(const ZCog<Param>& iCog) const
		{ return iCog; }

	template <class Param>
	ZCog<Param> operator&
		(ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>* iCallable) const
		{ return iCallable; }

	template <class Param>
	ZCog<Param> operator&
		(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable) const
		{ return iCallable; }

	template <class Param>
	const ZCog<Param>& operator&
		(const ZCog<Param>& iCog) const
		{ return iCog; }

	} sCog = {};

// =================================================================================================
#pragma mark -
#pragma mark * sEither

template <class Param>
ZCog<Param> sEither_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1);

template <class Param>
ZCog<Param> sEither(const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	if (iCog0)
		{
		if (iCog1)
			return sBindR(sCallable(sEither_Fun<Param>), iCog0, iCog1);
		return iCog0;
		}
	return iCog1;
	}

template <class Param>
ZCog<Param> sEither_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1)
	{
	if (iCog0 && not sCompareAndSet(iCog0, iCog0->Call(iCog0, iParam)))
		{
		if (iCog1 && not sCompareAndSet(iCog1, iCog1->Call(iCog1, iParam)))
			return iSelf;
		}
	else if (iCog1)
		{
		iCog1 = iCog1->Call(iCog1, iParam);
		}

	return sEither(iCog0, iCog1);
	}

template <class Param>
ZCog<Param> operator+
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sEither<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator+=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sEither<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * sBoth

template <class Param>
ZCog<Param> sBoth_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1);

template <class Param>
ZCog<Param> sBoth(const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	if (iCog0 && iCog1)
		return sBindR(sCallable(sBoth_Fun<Param>), iCog0, iCog1);
	return null;
	}

template <class Param>
ZCog<Param> sBoth_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1)
	{
	if (iCog0 && not sCompareAndSet(iCog0, iCog0->Call(iCog0, iParam)))
		{
		if (iCog1 && not sCompareAndSet(iCog1, iCog1->Call(iCog1, iParam)))
			return iSelf;
		}
	else if (iCog0 && iCog1)
		{
		iCog1 = iCog1->Call(iCog1, iParam);
		}

	return sBoth(iCog0, iCog1);
	}

template <class Param>
ZCog<Param> operator*
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sBoth<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator*=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sBoth<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * sEach

template <class Param>
ZCog<Param> sEach_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1);

template <class Param>
ZCog<Param> sEach(const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	if (iCog0)
		{
		if (iCog1)
			return sBindR(sCallable(sEach_Fun<Param>), iCog0, iCog1);
		return iCog0;
		}
	return iCog1;
	}

template <class Param>
ZCog<Param> sEach_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1)
	{
	if (iCog0 && not sCompareAndSet(iCog0, iCog0->Call(iCog0, iParam)))
		return iSelf;

	if (!iCog0 && iCog1)
		iCog1 = iCog1->Call(iCog1, iParam);

	return sEach(iCog0, iCog1);
	}

template <class Param>
ZCog<Param> operator|
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sEach<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator|=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sEach<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * sOnce

template <class Param>
ZCog<Param> sOnce_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog)
	{
	sCallCog(iCog, iParam);
	return null;
	}

template <class Param>
ZCog<Param> sOnce(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sBindR(sCallable(sOnce_Fun<Param>), iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeat

template <class Param>
ZCog<Param> sRepeat_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog_Init, ZCog<Param> iCog)
	{
	if (not iCog || not sCompareAndSet(iCog, iCog->Call(iCog, iParam)))
		return iSelf;

	if (iCog)
		return sBindR(sCallable(sRepeat_Fun<Param>), iCog_Init, iCog);

	return sBindR(sCallable(sRepeat_Fun<Param>), iCog_Init, iCog_Init);
	}

template <class Param>
ZCog<Param> sRepeat(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sBindR(sCallable(sRepeat_Fun<Param>), iCallable, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeat (count)

template <class Param>
ZCog<Param> sRepeat_Count_Fun(const ZCog<Param>& iSelf, Param iParam,
	size_t iCount, ZCog<Param> iCog_Init, ZCog<Param> iCog)
	{
	if (not iCount)
		return null;

	if (iCog && not sCompareAndSet(iCog, iCog->Call(iCog, iParam)))
		return iSelf;

	if (iCog)
		return sBindR(sCallable(sRepeat_Count_Fun<Param>), iCount, iCog_Init, iCog);

	if (--iCount)
		return sBindR(sCallable(sRepeat_Count_Fun<Param>), iCount, iCog_Init, iCog_Init);

	return null;
	}

template <class Param>
ZCog<Param> sRepeat(size_t iCount,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sBindR(sCallable(sRepeat_Count_Fun<Param>), iCount, iCallable, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * sDelayUntil

template <class Param>
ZCog<Param> sDelayUntil_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZTime iSystemTime)
	{
	if (ZTime::sSystem() > iSystemTime)
		return null;
	return iSelf;
	}

template <class Param>
ZCog<Param> sDelayUntil(ZTime iSystemTime)
	{ return sBindR(sCallable(sDelayUntil_Fun<Param>), iSystemTime); }

// =================================================================================================
#pragma mark -
#pragma mark * sDelayFor

template <class Param>
ZCog<Param> sDelayFor_Fun(const ZCog<Param>& iSelf, Param iParam,
	double iDelay)
	{ return sCallCog(sDelayUntil<Param>(ZTime::sSystem() + iDelay), iParam); }

template <class Param>
ZCog<Param> sDelayFor(double iDelay)
	{ return sBindR(sCallable(sDelayFor_Fun<Param>), iDelay); }

// =================================================================================================
#pragma mark -
#pragma mark * sStartAt, sStopAt, sStartAfter, sStopAfter

template <class Param>
ZCog<Param> sStartAt(ZTime iSystemTime,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCog)
	{ return sDelayUntil<Param>(iSystemTime) | iCog; }

template <class Param>
ZCog<Param> sStopAt(ZTime iSystemTime,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCog)
	{ return sDelayUntil<Param>(iSystemTime) * iCog; }

template <class Param>
ZCog<Param> sStartAfter(double iDelay,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCog)
	{ return sDelayFor<Param>(iDelay) | iCog; }

template <class Param>
ZCog<Param> sStopAfter(double iDelay,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCog)
	{ return sDelayFor<Param>(iDelay) * iCog; }

} // namespace ZooLib

#endif // __ZCog__
