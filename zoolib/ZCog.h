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
#pragma mark * sCog_Term

template <class Param>
ZCog<Param> spCogFun_Term(const ZCog<Param>& iSelf, Param iParam)
	{
	// Shouldn't ever call this.
	ZUnimplemented();
	return null;
	}

template <class Param>
const ZCog<Param>& sCog_Term()
	{
	static const ZCog<Param> spCog = sCallable(spCogFun_Term<Param>);
	return spCog;
	}

template <class Param>
const ZCog<Param>& sCog_Term
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Term<Param>(); }

template <class Param>
bool sIsTerm(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Term<Param>() == iCallable; }

// =================================================================================================
#pragma mark -
#pragma mark * sCallCog variants

template <class Cog>
Cog sCallCog(const Cog& iCog, const typename Cog::Param iParam)
	{
	if (not iCog || sIsTerm(iCog))
		return iCog;
	return sCall(iCog, iCog, iParam);
	}

template <class Param>
ZCog<Param> sCallCog
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable, Param iParam)
	{
	if (not iCallable || sIsTerm(iCallable))
		return iCallable;
	return sCall(iCallable, iCallable, iParam);
	}

template <class Param>
ZCog<const Param&> sCallCog
	(const ZRef<ZCallable<ZCog<const Param&>(const ZCog<const Param&>&,const Param&)> >& iCallable,
	const Param& iParam)
	{
	if (not iCallable || sIsTerm(iCallable))
		return iCallable;
	return sCall(iCallable, iCallable, iParam);
	}

template <class Cog>
bool sCallValidCogChanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallCogChanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return ioCog && not sIsTerm(ioCog) && sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallValidCogUnchanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return not sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallCogUnchanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return not ioCog || sIsTerm(ioCog) || not sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_Either

template <class Param>
ZCog<Param> spCogFun_Either(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1);

template <class Param>
ZCog<Param> spCog_Either
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(iCallable0 && iCallable1);
	static ZMACRO_auto(spCallable, sCallable(spCogFun_Either<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_Either
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (iCallable0)
		{
		if (iCallable1)
			return spCog_Either(iCallable0, iCallable1);
		return iCallable0;
		}
	return iCallable1;
	}

template <class Param>
ZCog<Param> spCogFun_Either(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1)
	{
	ZAssert(lCog0 && lCog1);
	
	if (sIsTerm(lCog0))
		{
		if (sIsTerm(lCog1))
			return lCog0;
		if ((lCog1 = lCog1->Call(lCog1, iParam)))
			return lCog1;
		return lCog0;
		}
	else if (sCallValidCogUnchanged(lCog0, iParam))
		{
		if (sIsTerm(lCog1))
			return lCog0;
		if (sCallValidCogUnchanged(lCog1, iParam))
			return iSelf;
		if (not lCog1 || sIsTerm(lCog1))
			return lCog0;
		return spCog_Either(lCog0, lCog1);
		}
	else if (not lCog0)
		{
		if (sIsTerm(lCog1))
			return lCog1;

		return lCog1->Call(lCog1, iParam);
		}
	else if (sIsTerm(lCog1) || (lCog1 = lCog1->Call(lCog1, iParam)))
		{
		return spCog_Either(lCog0, lCog1);
		}
	else
		{
		return lCog0;
		}
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

template <class Param>
struct ZCogAccumulatorCombiner_Either
	{
	void operator()(ZCog<Param>& io0, const ZCog<Param>& i1) const
		{ sCog_Either(io0, i1); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCog_Both

template <class Param>
ZCog<Param> spCogFun_Both(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1);

template <class Param>
ZCog<Param> spCog_Both
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(iCallable0 && iCallable1);
	static ZMACRO_auto(spCallable, sCallable(spCogFun_Both<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_Both
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (iCallable0 && iCallable1)
		return spCog_Both(iCallable0, iCallable1);
	return null;
	}

template <class Param>
ZCog<Param> spCogFun_Both(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1)
	{
	ZAssert(lCog0 && lCog1);

	if (sIsTerm(lCog0))
		{
		if (not sIsTerm(lCog1) && not lCog1->Call(lCog1, iParam))
			return null;
		return lCog0;
		}
	else if (sCallValidCogUnchanged(lCog0, iParam))
		{
		if (sIsTerm(lCog1))
			return lCog1;
		if (sCallValidCogUnchanged(lCog1, iParam))
			return iSelf;
		if (not lCog1)
			return null;
		if (sIsTerm(lCog1))
			return lCog1;
		return spCog_Both(lCog0, lCog1);
		}
	else if (not lCog0)
		{
		return null;
		}
	else if (sIsTerm(lCog0))
		{
		if (sIsTerm(lCog1))
			return lCog1;
		if ((lCog1 = lCog1->Call(lCog1, iParam)) && sIsTerm(lCog1))
			return lCog1;
		return null;
		}
	else if (sIsTerm(lCog1))
		{
		return lCog1;
		}
	else if ((lCog1 = lCog1->Call(lCog1, iParam)))
		{
		if (sIsTerm(lCog1))
			return lCog1;
		return spCog_Both(lCog0, lCog1);
		}
	else
		{
		return null;
		}
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

template <class Param>
struct ZCogAccumulatorCombiner_Both
	{
	void operator()(ZCog<Param>& io0, const ZCog<Param>& i1) const
		{
		if (io0)
			io0 = sCog_Both(io0, i1);
		else
			io0 = i1;
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCog_Each

template <class Param>
ZCog<Param> spCogFun_Each(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1);

template <class Param>
ZCog<Param> spCog_Each
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(iCallable0 && iCallable1);
	static ZMACRO_auto(spCallable, sCallable(spCogFun_Each<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_Each
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (iCallable0)
		{
		if (iCallable1)
			return spCog_Each(iCallable0, iCallable1);
		return iCallable0;
		}
	return iCallable1;
	}

template <class Param>
ZCog<Param> spCogFun_Each(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1)
	{
	ZAssert(lCog0 && lCog1);

	if (sIsTerm(lCog0))
		{
		if (sIsTerm(lCog1))
			return lCog1;
		return lCog1->Call(lCog1, iParam);
		}
	else if (sCallValidCogUnchanged(lCog0, iParam))
		{
		return iSelf;
		}
	else if (not lCog0)
		{
		if (sIsTerm(lCog1))
			return lCog1;
		return lCog1->Call(lCog1, iParam);
		}
	else if (sIsTerm(lCog0))
		{
		if (sIsTerm(lCog1))
			return lCog1;
		if ((lCog1 = lCog1->Call(lCog1, iParam)))
			return spCog_Each(lCog0, lCog1);
		return lCog0;
		}
	else
		{
		return spCog_Each(lCog0, lCog1);
		}
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

template <class Param>
struct ZCogAccumulatorCombiner_Each
	{
	void operator()(ZCog<Param>& io0, const ZCog<Param>& i1) const
		{ io0 = sCog_Each(io0, i1); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCog_FollowedBy

template <class Param>
ZCog<Param> spCogFun_FollowedBy_First(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_FollowedBy_First
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(iCallable0 && iCallable1);
	static ZMACRO_auto(spCallable, sCallable(spCogFun_FollowedBy_First<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_FollowedBy_First
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (iCallable0)
		{
		if (iCallable1)
			return spCog_FollowedBy_First(iCallable0, iCallable1);
		return iCallable0;
		}
	return null;
	}

template <class Param>
ZCog<Param> spCogFun_FollowedBy_Subsequent(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_FollowedBy_Subsequent
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(iCallable0 && iCallable1);
	static ZMACRO_auto(spCallable, sCallable(spCogFun_FollowedBy_Subsequent<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> spCogFun_FollowedBy_Subsequent(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, const ZCog<Param>& iCog1)
	{
	ZAssert(lCog0 && iCog1);

	if (sCallValidCogUnchanged(lCog0, iParam))
		return iSelf;

	if (lCog0 && not sIsTerm(lCog0))
		return spCog_FollowedBy_Subsequent(lCog0, iCog1);
	
	if (not sIsTerm(iCog1))
		return iCog1->Call(iCog1, iParam);

	return iCog1;
	}

template <class Param>
ZCog<Param> spCogFun_FollowedBy_First(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, const ZCog<Param>& iCog1)
	{
	ZAssert(lCog0 && iCog1);

	if (sIsTerm(lCog0))
		{
		if (sIsTerm(iCog1))
			return iCog1;
		return iCog1->Call(iCog1, iParam);
		}
	else if ((lCog0 = lCog0->Call(lCog0, iParam)))
		{
		if (sIsTerm(lCog0))
			{
			if (sIsTerm(iCog1))
				return iCog1;
			return iCog1->Call(iCog1, iParam);
			}
		return spCog_FollowedBy_Subsequent(lCog0, iCog1);
		}
	else
		{
		return null;
		}
	}

template <class Param>
ZCog<Param> operator/
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_FollowedBy_First<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator/=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_FollowedBy_First<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_WhileUnchanged

// Call second cog so long as the calling the first cog is unchanged.

template <class Param>
ZCog<Param> spCogFun_WhileUnchanged(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1);

template <class Param>
ZCog<Param> spCog_WhileUnchanged
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(iCallable0 && iCallable1);
	static ZMACRO_auto(spCallable, sCallable(spCogFun_WhileUnchanged<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_WhileUnchanged
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (iCallable0 && iCallable1)
		return spCog_WhileUnchanged(iCallable0, iCallable1);
	return null;
	}

template <class Param>
ZCog<Param> spCogFun_WhileUnchanged(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, ZCog<Param> lCog1)
	{
	ZAssert(lCog0 && lCog1);

	if (sIsTerm(lCog0) || sCallValidCogChanged(lCog0, iParam))
		return lCog0;

	if (sCallValidCogChanged(lCog1, iParam))
		{
		if (lCog1 && not sIsTerm(lCog1))
			return spCog_WhileUnchanged(lCog0, lCog1);
		return lCog0;
		}

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
ZCog<Param> sCog_Once(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return iCallable * sCog_Term<Param>(); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_While

template <class Param>
ZCog<Param> sCog_While(const ZCog<Param>& iCog_Init, const ZCog<Param>& iCog);

template <class Param>
ZCog<Param> spCogFun_While(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog_Init, ZCog<Param> lCog)
	{
	ZAssert(iCog_Init && lCog);

	if (sIsTerm(lCog) || sCallValidCogUnchanged(lCog, iParam))
		return iSelf;

	if (not lCog)
		return null;

	if (sIsTerm(lCog))
		return sCallCog(sCog_While(iCog_Init, iCog_Init), iParam); //## Dangerous
	else
		return sCog_While(iCog_Init, lCog);
	}

template <class Param>
ZCog<Param> sCog_While(const ZCog<Param>& iCog_Init, const ZCog<Param>& iCog)
	{
	static ZMACRO_auto(spCallable, sCallable(spCogFun_While<Param>));
	return sBindR(spCallable, iCog_Init, iCog);
	}

template <class Param>
ZCog<Param> sCog_While(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (iCallable)
		{
		if (sIsTerm(iCallable))
			return iCallable;
		return sCog_While<Param>(iCallable, iCallable);
		}
	return null;
	}

template <class Param>
ZCog<Param> operator*(const ZCog<Param>& iCog)
	{ return sCog_While<Param>(iCog); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_Repeat (count)

template <class Param>
ZCog<Param> spCog_Repeat(size_t iCount, const ZCog<Param>& iCog_Init, const ZCog<Param>& iCog);

template <class Param>
ZCog<Param> spCogFun_Repeat(const ZCog<Param>& iSelf, Param iParam,
	size_t iCount, const ZCog<Param>& iCog_Init, ZCog<Param> lCog)
	{
	if (sCallCogUnchanged(lCog, iParam))
		return iSelf;

	if (not lCog)
		return null;

	if (sIsTerm(lCog))
		return sCallCog(spCog_Repeat(iCount - 1, iCog_Init, iCog_Init), iParam);
	else
		return spCog_Repeat(iCount, iCog_Init, lCog);
	}

template <class Param>
ZCog<Param> spCog_Repeat(size_t iCount, const ZCog<Param>& iCog_Init, const ZCog<Param>& iCog)
	{
	static ZMACRO_auto(spCallable, sCallable(spCogFun_Repeat<Param>));
	if (iCount && iCog_Init)
		{
		if (sIsTerm(iCog_Init))
			return iCog_Init;
		return sBindR(spCallable, iCount, iCog_Init, iCog);
		}
	return null;
	}

template <class Param>
ZCog<Param> sCog_Repeat(size_t iCount,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return spCog_Repeat<Param>(iCount, iCallable, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_DelayUntil

template <class Param>
ZCog<Param> spCogFun_DelayUntil(const ZCog<Param>& iSelf, Param iParam,
	ZTime iSystemTime)
	{
	if (ZTime::sSystem() > iSystemTime)
		return sCog_Term<Param>();
	return iSelf;
	}

template <class Param>
ZCog<Param> sCog_DelayUntil(ZTime iSystemTime)
	{
	static ZMACRO_auto(spCallable, sCallable(spCogFun_DelayUntil<Param>));
	return sBindR(spCallable, iSystemTime);
	}

// =================================================================================================
#pragma mark -
#pragma mark * sCog_DelayFor

template <class Param>
ZCog<Param> spCogFun_DelayFor(const ZCog<Param>& iSelf, Param iParam,
	double iDelay)
	{ return sCallCog(sCog_DelayUntil<Param>(ZTime::sSystem() + iDelay), iParam); }

template <class Param>
ZCog<Param> sCog_DelayFor(double iDelay)
	{
	static ZMACRO_auto(spCallable, sCallable(spCogFun_DelayFor<Param>));
	return sBindR(spCallable, iDelay);
	}

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
