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
#pragma mark * sCallCog variants

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

template <class Cog>
bool sCallCogChanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return ioCog && sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallCogUnchanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return not ioCog || not sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog function and pseudo operator

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
ZCog<Param> sCogFun_Either(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1);

template <class Param>
ZCog<Param> sCog_Either
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (iCallable0)
		{
		if (iCallable1)
			return sBindR(sCallable(sCogFun_Either<Param>), iCallable0, iCallable1);
		return iCallable0;
		}
	return iCallable1;
	}

template <class Param>
ZCog<Param> sCogFun_Either(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1)
	{
	if (sCallCogUnchanged(lCog0, iParam))
		{
		if (sCallCogUnchanged(lCog1, iParam))
			return iSelf;
		}
	else if (lCog1)
		{
		lCog1 = lCog1->Call(lCog1, iParam);
		}

	return sCog_Either(lCog0, lCog1);
	}

template <class Param>
ZCog<Param> operator+
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_Either<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator+=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_Either<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_Both

template <class Param>
ZCog<Param> sCogFun_Both(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1);

template <class Param>
ZCog<Param> sCog_Both
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (iCallable0 && iCallable1)
		return sBindR(sCallable(sCogFun_Both<Param>), iCallable0, iCallable1);
	return null;
	}

template <class Param>
ZCog<Param> sCogFun_Both(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1)
	{
	if (sCallCogUnchanged(lCog0, iParam))
		{
		if (sCallCogUnchanged(lCog1, iParam))
			return iSelf;
		}
	else if (lCog0 && lCog1)
		{
		lCog1 = lCog1->Call(lCog1, iParam);
		}

	return sCog_Both(lCog0, lCog1);
	}

template <class Param>
ZCog<Param> operator*
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_Both<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator*=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_Both<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_Each

template <class Param>
ZCog<Param> sCogFun_Each(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1);

template <class Param>
ZCog<Param> sCog_Each
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (iCallable0)
		{
		if (iCallable1)
			return sBindR(sCallable(sCogFun_Each<Param>), iCallable0, iCallable1);
		return iCallable0;
		}
	return iCallable1;
	}

template <class Param>
ZCog<Param> sCogFun_Each(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1)
	{
	if (lCog0 && not sCompareAndSet(lCog0, lCog0->Call(lCog0, iParam)))
		return iSelf;

	if (not lCog0 && lCog1)
		lCog1 = lCog1->Call(lCog1, iParam);

	return sCog_Each(lCog0, lCog1);
	}

template <class Param>
ZCog<Param> operator|
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_Each<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator|=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_Each<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_WhileUnchanged

// Call second cog so long as the calling the first cog is unchanged.

template <class Param>
ZCog<Param> sCogFun_WhileUnchanged(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1);

template <class Param>
ZCog<Param> sCog_WhileUnchanged
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (iCallable0 && iCallable1)
		return sBindR(sCallable(sCogFun_WhileUnchanged<Param>), iCallable0, iCallable1);
	return null;
	}

template <class Param>
ZCog<Param> sCogFun_WhileUnchanged(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1)
	{
	if (sCallCogChanged(lCog0, iParam))
		return lCog0;

	if (sCallCogChanged(lCog1, iParam))
		return sCog_WhileUnchanged(lCog0, lCog1);

	return iSelf;
	}

template <class Param>
ZCog<Param> operator%
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_WhileUnchanged<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator%=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_WhileUnchanged<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_Once

template <class Param>
ZCog<Param> sCogFun_Once(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog)
	{
	sCallCog(iCog, iParam);
	return null;
	}

template <class Param>
ZCog<Param> sCog_Once(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sBindR(sCallable(sCogFun_Once<Param>), iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_Repeat

template <class Param>
ZCog<Param> sCogFun_Repeat(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog_Init, ZCog<Param> lCog)
	{
	if (sCallCogUnchanged(lCog, iParam))
		return iSelf;

	if (lCog)
		return sBindR(sCallable(sCogFun_Repeat<Param>), iCog_Init, lCog);

	return sBindR(sCallable(sCogFun_Repeat<Param>), iCog_Init, iCog_Init);
	}

template <class Param>
ZCog<Param> sCog_Repeat(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sBindR(sCallable(sCogFun_Repeat<Param>), iCallable, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_Repeat (count)

template <class Param>
ZCog<Param> sCogFun_Repeat_Count(const ZCog<Param>& iSelf, Param iParam,
	size_t iCount, ZCog<Param> iCog_Init, ZCog<Param> lCog)
	{
	if (not iCount)
		return null;

	if (sCallCogUnchanged(lCog, iParam))
		return iSelf;

	if (lCog)
		return sBindR(sCallable(sCogFun_Repeat_Count<Param>), iCount, iCog_Init, lCog);

	if (--iCount)
		return sBindR(sCallable(sCogFun_Repeat_Count<Param>), iCount, iCog_Init, iCog_Init);

	return null;
	}

template <class Param>
ZCog<Param> sCog_Repeat(size_t iCount,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sBindR(sCallable(sCogFun_Repeat_Count<Param>), iCount, iCallable, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_DelayUntil

template <class Param>
ZCog<Param> sCogFun_DelayUntil(const ZCog<Param>& iSelf, Param iParam,
	ZTime iSystemTime)
	{
	if (ZTime::sSystem() > iSystemTime)
		return null;
	return iSelf;
	}

template <class Param>
ZCog<Param> sCog_DelayUntil(ZTime iSystemTime)
	{ return sBindR(sCallable(sCogFun_DelayUntil<Param>), iSystemTime); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_DelayFor

template <class Param>
ZCog<Param> sCogFun_DelayFor(const ZCog<Param>& iSelf, Param iParam,
	double iDelay)
	{ return sCallCog(sCog_DelayUntil<Param>(ZTime::sSystem() + iDelay), iParam); }

template <class Param>
ZCog<Param> sCog_DelayFor(double iDelay)
	{ return sBindR(sCallable(sCogFun_DelayFor<Param>), iDelay); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_StartAt, sCog_StopAt, sCog_StartAfter, sCog_StopAfter

template <class Param>
ZCog<Param> sCog_StartAt(ZTime iSystemTime,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_DelayUntil<Param>(iSystemTime) | iCallable; }

template <class Param>
ZCog<Param> sCog_StopAt(ZTime iSystemTime,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_DelayUntil<Param>(iSystemTime) * iCallable; }

template <class Param>
ZCog<Param> sCog_StartAfter(double iDelay,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >iCallable)
	{ return sCog_DelayFor<Param>(iDelay) | iCallable; }

template <class Param>
ZCog<Param> sCog_StopAfter(double iDelay,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >iCallable)
	{ return sCog_DelayFor<Param>(iDelay) * iCallable; }

} // namespace ZooLib

#endif // __ZCog__
