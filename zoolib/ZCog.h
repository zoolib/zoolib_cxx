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

#ifndef __ZCog_h__
#define __ZCog_h__ 1

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

//--

	ZCog(const bool iBool)
	:	inherited(iBool ? sTrue() : null)
		{}

	ZCog& operator=(const bool iBool)
		{
		inherited::operator=(iBool ? sTrue() : null);
		return *this;
		}

//--

	static ZCog sFalse()
		{ return null; }

	static const ZCog& sTrue();
	};

template <class Param>
ZCog<Param> spCogFun_True(const ZCog<Param>& iSelf, Param iParam)
	{
	// Shouldn't ever call this.
	ZUnimplemented();
	return null;
	}

template <class Param>
const ZCog<Param>& ZCog<Param>::sTrue()
	{
	static const ZCog<Param> spCog = sCallable(spCogFun_True<Param>);
	return spCog;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Nullary, sCog_Unknown

template <class Param>
ZCog<Param> spCogFun_Unknown(const ZCog<Param>& iSelf, Param iParam)
	{ return iSelf; }

template <class Param>
const ZCog<Param>& sCog_Unknown()
	{
	static const ZCog<Param> spCog = sCallable(spCogFun_Unknown<Param>);
	return spCog;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Cog classification

template <class Param>
bool sIsTrue(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return ZCog<Param>::sTrue() == iCallable; }

template <class Param>
bool sIsFalse(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return not iCallable; }

template <class Param>
bool sIsFinished(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return not iCallable || sIsTrue(iCallable); }

template <class Param>
bool sIsPending(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return iCallable && not sIsTrue(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * sCallCog variants

template <class Cog>
Cog sCallCog(const Cog& iCog, const typename Cog::Param iParam)
	{
	if (sIsFinished(iCog))
		return iCog;
	return iCog->Call(iCog, iParam);
	}

template <class Param>
ZCog<Param> sCallCog
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable, Param iParam)
	{
	if (sIsFinished(iCallable))
		return iCallable;
	return iCallable->Call(iCallable, iParam);
	}

template <class Param>
ZCog<const Param&> sCallCog
	(const ZRef<ZCallable<ZCog<const Param&>(const ZCog<const Param&>&,const Param&)> >& iCallable,
	const Param& iParam)
	{
	if (sIsFinished(iCallable))
		return iCallable;
	return iCallable->Call(iCallable, iParam);
	}

template <class Cog>
bool sCallPendingCog_Changed(Cog& ioCog, const typename Cog::Param iParam)
	{ return sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallCog_Changed(Cog& ioCog, const typename Cog::Param iParam)
	{ return sIsPending(ioCog) && sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallPendingCog_Unchanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return not sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallCog_Unchanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return sIsFinished(ioCog) || not sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

// =================================================================================================
#pragma mark -
#pragma mark * Unary, sCog_Not

template <class Param>
ZCog<Param> spCog_Not
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable);

template <class Param>
ZCog<Param> spCogFun_Not(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog)
	{
	if (sCallPendingCog_Unchanged(lCog, iParam))
		return iSelf;

	if (not lCog)
		return true;

	if (sIsTrue(lCog))
		return false;

	return spCog_Not(lCog);
	}

template <class Param>
ZCog<Param> spCog_Not
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	ZAssert(iCallable && not sIsTrue(iCallable));
	static ZMACRO_auto(spCallable, sCallable(spCogFun_Not<Param>));
	return sBindR(spCallable, iCallable);	
	}

template <class Param>
ZCog<Param> sCog_Not
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (not iCallable)
		return true;

	if (sIsTrue(iCallable))
		return false;

	return spCog_Not(iCallable);
	}

template <class Param>
ZCog<Param> operator~
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Not<Param>(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * Unary, sCog_Tautology (TrueIfFinished)

template <class Param>
ZCog<Param> spCog_Tautology
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable);

template <class Param>
ZCog<Param> spCogFun_Tautology(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog)
	{
	if (sCallPendingCog_Unchanged(lCog, iParam))
		return iSelf;

	if (not lCog || sIsTrue(lCog))
		return true;

	return spCog_Tautology(lCog);
	}

template <class Param>
ZCog<Param> spCog_Tautology
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	ZAssert(iCallable && not sIsTrue(iCallable));
	static ZMACRO_auto(spCallable, sCallable(spCogFun_Tautology<Param>));
	return sBindR(spCallable, iCallable);	
	}

template <class Param>
ZCog<Param> sCog_Tautology
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (not iCallable || sIsTrue(iCallable))
		return true;

	return spCog_Tautology(iCallable);
	}

template <class Param>
ZCog<Param> operator+(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Tautology<Param>(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * Unary, sCog_Contradiction (FalseIfFinished)

template <class Param>
ZCog<Param> spCog_Contradiction
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable);

template <class Param>
ZCog<Param> spCogFun_Contradiction(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog)
	{
	if (sCallPendingCog_Unchanged(lCog, iParam))
		return iSelf;

	if (not lCog || sIsTrue(lCog))
		return false;

	return spCog_Contradiction(lCog);
	}

template <class Param>
ZCog<Param> spCog_Contradiction
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	ZAssert(iCallable && not sIsTrue(iCallable));
	static ZMACRO_auto(spCallable, sCallable(spCogFun_Contradiction<Param>));
	return sBindR(spCallable, iCallable);	
	}

template <class Param>
ZCog<Param> sCog_Contradiction
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (not iCallable || sIsTrue(iCallable))
		return false;

	return spCog_Contradiction(iCallable);
	}

template <class Param>
ZCog<Param> operator-(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Contradiction<Param>(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * Unary, sCog_Repeat

template <class Param>
ZCog<Param> spCog_Repeat
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable_Init,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable);

template <class Param>
ZCog<Param> spCogFun_Repeat(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog_Init, const ZCog<Param>& iCog)
	{
	ZCog<Param> lCog = iCog;

	if (sCallPendingCog_Unchanged(lCog, iParam))
		return iSelf;

	if (not lCog)
		return false;
	
	if (not sIsTrue(lCog))
		return spCog_Repeat(iCog_Init, lCog);

	// To get unbroken repetition on hitting a term we need to call the new cog,
	// don't make the call if we'd just be calling an iSelf-equivalent.
	if (iCog_Init == iCog)
		return iSelf;
	else
		return sCallCog(spCog_Repeat(iCog_Init, iCog_Init), iParam);
	}

template <class Param>
ZCog<Param> spCog_Repeat
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable_Init,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	ZAssert(iCallable_Init && iCallable);
	static ZMACRO_auto(spCallable, sCallable(spCogFun_Repeat<Param>));
	return sBindR(spCallable, iCallable_Init, iCallable);
	}

template <class Param>
ZCog<Param> sCog_Repeat
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (not iCallable)
		return false;

	if (sIsTrue(iCallable))
		return sCog_Unknown<Param>();

	return spCog_Repeat(iCallable, iCallable);
	}

template <class Param>
ZCog<Param> operator*(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Repeat<Param>(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * Binary sequential, sCog_Then

// Call cog0 till it finishes, if true then call cog1

template <class Param>
ZCog<Param> spCog_Then
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1);

template <class Param>
ZCog<Param> spCogFun_Then(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog, const ZCog<Param>& iCog1)
	{
	if (sCallPendingCog_Unchanged(lCog, iParam))
		return iSelf;

	if (sIsFalse(lCog))
		return false;

	if (sIsTrue(lCog))
		{
		if (iCog1 && not sIsTrue(iCog1))
			return iCog1->Call(iCog1, iParam);
		return iCog1;
		}

	return spCog_Then(lCog, iCog1);
	}

template <class Param>
ZCog<Param> sCog_Then
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsFalse(iCallable0))
		return false;

	if (sIsTrue(iCallable0))
		return iCallable1;

	return spCog_Then(iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> spCog_Then
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsFalse(iCallable0))
		return false;

	if (sIsTrue(iCallable0))
		return iCallable1;

	static ZMACRO_auto(spCallable, sCallable(spCogFun_Then<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> operator>>
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_Then<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator>>=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_Then<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * Binary parallel, sCog_And

template <class Param>
ZCog<Param> spCogFun_And(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_And
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(sIsPending(iCallable0) && not sIsTrue(iCallable1));
	static ZMACRO_auto(spCallable, sCallable(spCogFun_And<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_And
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsFalse(iCallable0))
		return false;

	if (sIsTrue(iCallable0))
		return iCallable1;

	if (sIsTrue(iCallable1))
		return iCallable0;

	return spCog_And(iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> spCogFun_And(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, const ZCog<Param>& iCog1)
	{
	ZAssert(sIsPending(lCog0) && not sIsTrue(iCog1));

	if (sCallPendingCog_Unchanged(lCog0, iParam))
		{
		if (not iCog1)
			return false;

		ZCog<Param> lCog1 = iCog1;
		if (sCallPendingCog_Unchanged(lCog1, iParam))
			return iSelf;

		if (not lCog1)
			return false;
		
		if (sIsTrue(lCog1))
			return lCog0;

		return spCog_And(lCog0, lCog1);
		}
	else if (not lCog0)
		{
		return false;
		}
	else if (sIsTrue(lCog0))
		{
		if (not iCog1)
			return false;

		if (sIsTrue(iCog1))
			return true;

		return iCog1->Call(iCog1, iParam);
		}
	else
		{
		if (not iCog1)
			return false;

		if (sIsTrue(iCog1))
			return lCog0;

		if (ZCog<Param> lCog1 = iCog1->Call(iCog1, iParam))
			{
			if (sIsTrue(lCog1))
				return lCog0;
			return spCog_And(lCog0, lCog1);
			}

		return false;
		}
	}

template <class Param>
ZCog<Param> operator&
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_And<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator&=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_And<Param>(ioCog0, iCallable1); }

template <class Param>
struct ZCogAccumulatorCombiner_And
	{
	void operator()(ZCog<Param>& io0, const ZCog<Param>& i1) const
		{
		if (io0)
			io0 = sCog_And(io0, i1);
		else
			io0 = i1;
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * Binary parallel, sCog_Or

template <class Param>
ZCog<Param> spCogFun_Or(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_Or
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(sIsPending(iCallable0) && sIsPending(iCallable1));
	static ZMACRO_auto(spCallable, sCallable(spCogFun_Or<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_Or
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsFalse(iCallable0))
		return iCallable1;

	if (sIsTrue(iCallable0))
		return true;

	if (sIsFalse(iCallable1))
		return iCallable0;

	return spCog_Or(iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> spCogFun_Or(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> lCog0, const ZCog<Param>& iCog1)
	{
	ZAssert(sIsPending(lCog0) && iCog1);

	if (sCallPendingCog_Unchanged(lCog0, iParam))
		{
		if (sIsTrue(iCog1))
			return true;

		ZCog<Param> lCog1 = iCog1;
		if (sCallPendingCog_Unchanged(lCog1, iParam))
			return iSelf;

		if (not lCog1)
			return lCog0;
		
		if (sIsTrue(lCog1))
			return true;

		return spCog_Or(lCog0, lCog1);
		}
	else if (not lCog0)
		{
		if (sIsTrue(iCog1))
			return true;

		return iCog1->Call(iCog1, iParam);
		}
	else if (sIsTrue(lCog0))
		{
		return true;
		}
	else
		{
		if (sIsTrue(iCog1))
			return true;

		if (ZCog<Param> lCog1 = iCog1->Call(iCog1, iParam))
			{
			if (sIsTrue(lCog1))
				return true;
			return spCog_Or(lCog0, lCog1);
			}

		return lCog0;
		}
	}

template <class Param>
ZCog<Param> operator|
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_Or<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator|=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_Or<Param>(ioCog0, iCallable1); }

template <class Param>
struct ZCogAccumulatorCombiner_Or
	{
	void operator()(ZCog<Param>& io0, const ZCog<Param>& i1) const
		{
		if (io0)
			{
			if (i1)
				io0 = sCog_Or(io0, i1);
			}
		else
			{
			io0 = i1;
			}
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCog_With

// Call cog1 so long as cog0 is pending.

template <class Param>
ZCog<Param> spCogFun_With(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_With
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(sIsPending(iCallable0) && sIsPending(iCallable1));

	static ZMACRO_auto(spCallable, sCallable(spCogFun_With<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_With
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsPending(iCallable0) && sIsPending(iCallable1))
		return spCog_With(iCallable0, iCallable1);
	return iCallable0;
	}

template <class Param>
ZCog<Param> spCogFun_With(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	ZAssert(sIsPending(iCog0) && sIsPending(iCog1));

	ZCog<Param> newCog0 = iCog0;
	if (sCallPendingCog_Unchanged(newCog0, iParam))
		{
		ZCog<Param> newCog1 = iCog1;
		if (sCallPendingCog_Unchanged(newCog1, iParam))
			return iSelf;

		if (sIsFinished(newCog1))
			return newCog0;

		return spCog_With(newCog0, newCog1);
		}
	else if (sIsPending(newCog0))
		{
		ZCog<Param> newCog1 = iCog1->Call(iCog1, iParam);
		if (sIsPending(newCog1))
			return spCog_With(newCog0, newCog1);		

		return newCog0;
		}
	else
		{
		return newCog0;
		}
	}

template <class Param>
ZCog<Param> operator/
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_With<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator/=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_With<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * sCog_WithUnchanged

// Call cog1 so long as cog0 is pending and unchanged.

template <class Param>
ZCog<Param> spCogFun_WithUnchanged(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_WithUnchanged
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(iCallable0 && iCallable1);
	static ZMACRO_auto(spCallable, sCallable(spCogFun_WithUnchanged<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_WithUnchanged
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsPending(iCallable0) && sIsPending(iCallable1))
		return spCog_WithUnchanged(iCallable0, iCallable1);
	return iCallable0;
	}

template <class Param>
ZCog<Param> spCogFun_WithUnchanged(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	ZAssert(sIsPending(iCog0) && sIsPending(iCog1));

	ZCog<Param> newCog0 = iCog0;
	if (sCallPendingCog_Changed(newCog0, iParam))
		return newCog0;

	ZCog<Param> newCog1 = iCog1;
	if (sCallPendingCog_Changed(newCog1, iParam))
		{
		if (sIsPending(newCog1))
			return spCog_WithUnchanged(newCog0, newCog1);
		return newCog0;
		}

	return iSelf;
	}

template <class Param>
ZCog<Param> operator%
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_WithUnchanged<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator%=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_WithUnchanged<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark -
#pragma mark * Binary, sCog_Each

// Call cog0 till it finishes, then call cog1

template <class Param>
ZCog<Param> sCog_Each
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsPending(iCallable0))
		{
		if (sIsPending(iCallable1))
			return +iCallable0 >> iCallable1;
		return iCallable0;
		}
	return iCallable1;
	}

template <class Param>
ZCog<Param> operator^
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_Each<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator^=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = ioCog0 ^ iCallable1; }

template <class Param>
struct ZCogAccumulatorCombiner_Each
	{
	void operator()(ZCog<Param>& io0, const ZCog<Param>& i1) const
		{ io0 ^= i1; }
	};

} // namespace ZooLib

#endif // __ZCog_h__
