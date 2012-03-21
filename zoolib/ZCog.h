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
// MARK: - ZCog

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

	static const ZCog& sFalse()
		{
		static const ZCog spCog;
		return spCog;
		}

	static ZCog spCogFun_True(const ZCog& iSelf, Param iParam)
		{
		// Must not ever call this.
		ZUnimplemented();
		}

	static const ZCog& sTrue()
		{
		static const ZCog spCog = sCallable(spCogFun_True);
		return spCog;
		}
	};

// =================================================================================================
// MARK: - Nullary, sCog_Pending

template <class Param>
ZCog<Param> spCogFun_Pending(const ZCog<Param>& iSelf, Param iParam)
	{ return iSelf; }

template <class Param>
const ZCog<Param>& sCog_Pending()
	{
	static const ZCog<Param> spCog = sCallable(spCogFun_Pending<Param>);
	return spCog;
	}

// =================================================================================================
// MARK: - Cog classification

template <class Param>
bool sIsTrue(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return ZCog<Param>::sTrue() == iCallable; }

template <class Param>
bool sIsFalse(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return not iCallable; }

template <class Param>
bool sIsFinished(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sIsFalse(iCallable) || sIsTrue(iCallable); }

template <class Param>
bool sIsPending(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return not sIsFinished(iCallable); }

// =================================================================================================
// MARK:-

template <class Param>
ZCog<Param>& sPreserveIfPending(ZCog<Param>& ioCog)
	{
	if (sIsFinished(ioCog))
		ioCog = null;
	return ioCog;
	}

// =================================================================================================
// MARK:- sCallCog variants

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
void sCallAndUpdateCog(Cog& ioCog, const typename Cog::Param iParam)
	{
	if (sIsPending(ioCog))
		ioCog = ioCog->Call(ioCog, iParam);
	}

template <class Cog>
bool sCallAndUpdatePendingCog_StillPending(Cog& ioCog, const typename Cog::Param iParam)
	{ return sIsPending(ioCog = ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallAndUpdatePendingCog_Changed(Cog& ioCog, const typename Cog::Param iParam)
	{ return sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallAndUpdatePendingCog_Unchanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return not sCompareAndSet(ioCog, ioCog->Call(ioCog, iParam)); }

template <class Cog>
bool sCallAndUpdateCog_Changed(Cog& ioCog, const typename Cog::Param iParam)
	{ return sIsPending(ioCog) && sCallAndUpdatePendingCog_Changed(ioCog, iParam); }

template <class Cog>
bool sCallAndUpdateCog_Unchanged(Cog& ioCog, const typename Cog::Param iParam)
	{ return sIsFinished(ioCog) || sCallAndUpdatePendingCog_Unchanged(ioCog, iParam); }

// =================================================================================================
// MARK: - Unary, sCog_Not

template <class Param>
ZCog<Param> spCog_Not
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable);

template <class Param>
ZCog<Param> spCogFun_Not(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog)
	{
	ZCog<Param> newCog = iCog;
	if (sCallAndUpdatePendingCog_Unchanged(newCog, iParam))
		return iSelf;

	if (sIsFalse(newCog))
		return true;

	if (sIsTrue(newCog))
		return false;

	return spCog_Not(newCog);
	}

template <class Param>
ZCog<Param> spCog_Not
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	ZAssert(sIsPending(iCallable));

	static const ZMACRO_auto(spCallable, sCallable(spCogFun_Not<Param>));
	return sBindR(spCallable, iCallable);	
	}

template <class Param>
ZCog<Param> sCog_Not
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (sIsFalse(iCallable))
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
// MARK: - Unary, sCog_Tautology (TrueIfFinished)

template <class Param>
ZCog<Param> spCog_Tautology
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable);

template <class Param>
ZCog<Param> spCogFun_Tautology(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog)
	{
	ZCog<Param> newCog = iCog;
	if (sCallAndUpdatePendingCog_Unchanged(newCog, iParam))
		return iSelf;

	if (sIsFinished(newCog))
		return true;

	return spCog_Tautology(newCog);
	}

template <class Param>
ZCog<Param> spCog_Tautology
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	ZAssert(sIsPending(iCallable));

	static const ZMACRO_auto(spCallable, sCallable(spCogFun_Tautology<Param>));
	return sBindR(spCallable, iCallable);	
	}

template <class Param>
ZCog<Param> sCog_Tautology
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (sIsFinished(iCallable))
		return true;

	return spCog_Tautology(iCallable);
	}

template <class Param>
ZCog<Param> operator+(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Tautology<Param>(iCallable); }

// =================================================================================================
// MARK: - Unary, sCog_Contradiction (FalseIfFinished)

template <class Param>
ZCog<Param> spCog_Contradiction
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable);

template <class Param>
ZCog<Param> spCogFun_Contradiction(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog)
	{
	ZCog<Param> newCog = iCog;
	if (sCallAndUpdatePendingCog_Unchanged(newCog, iParam))
		return iSelf;

	if (sIsFinished(newCog))
		return false;

	return spCog_Contradiction(newCog);
	}

template <class Param>
ZCog<Param> spCog_Contradiction
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	ZAssert(sIsPending(iCallable));

	static const ZMACRO_auto(spCallable, sCallable(spCogFun_Contradiction<Param>));
	return sBindR(spCallable, iCallable);	
	}

template <class Param>
ZCog<Param> sCog_Contradiction
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (sIsFinished(iCallable))
		return false;

	return spCog_Contradiction(iCallable);
	}

template <class Param>
ZCog<Param> operator-(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Contradiction<Param>(iCallable); }

// =================================================================================================
// MARK: - Unary, sCog_Repeat

template <class Param>
ZCog<Param> spCog_Repeat
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable_Init,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable);

template <class Param>
ZCog<Param> spCogFun_Repeat(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog_Init, const ZCog<Param>& iCog)
	{
	ZCog<Param> newCog = iCog;
	if (sCallAndUpdatePendingCog_Unchanged(newCog, iParam))
		return iSelf;

	if (sIsFalse(newCog))
		return false;
	
	if (not sIsTrue(newCog))
		return spCog_Repeat(iCog_Init, newCog);

	// To get unbroken repetition on hitting a term we need to call the new cog, but must
	// not make the call if it's an iSelf-equivalent (infinite recursion would ensue).
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

	static const ZMACRO_auto(spCallable, sCallable(spCogFun_Repeat<Param>));
	return sBindR(spCallable, iCallable_Init, iCallable);
	}

template <class Param>
ZCog<Param> sCog_Repeat
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (sIsFalse(iCallable))
		return false;

	if (sIsTrue(iCallable))
		return sCog_Pending<Param>();

	return spCog_Repeat(iCallable, iCallable);
	}

template <class Param>
ZCog<Param> operator*(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Repeat<Param>(iCallable); }

// =================================================================================================
// MARK: - Ternary sequential, sCog_If

// Call cog0 till it finishes, if true then call cog1

template <class Param>
ZCog<Param> spCog_If
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCondition,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1);

template <class Param>
ZCog<Param> spCogFun_If(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCondition,
	const ZCog<Param>& iCog0,
	const ZCog<Param>& iCog1)
	{
	ZCog<Param> newCondition = iCondition;
	if (sCallAndUpdatePendingCog_Unchanged(newCondition, iParam))
		return iSelf;

	if (sIsTrue(newCondition))
		{
		if (sIsFinished(iCog0))
			return iCog0;
		return iCog0->Call(iCog0, iParam);
		}

	if (sIsFalse(newCondition))
		{
		if (sIsFinished(iCog1))
			return iCog1;
		return iCog1->Call(iCog1, iParam);
		}

	return spCog_If(newCondition, iCog0, iCog1);
	}

template <class Param>
ZCog<Param> sCog_If
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCondition,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsTrue(iCondition))
		return iCallable0;

	if (sIsFalse(iCondition))
		return iCallable1;

	return spCog_If(iCondition, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> spCog_If
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCondition,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	static const ZMACRO_auto(spCallable, sCallable(spCogFun_If<Param>));
	return sBindR(spCallable, iCondition, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> operator>>
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_If<Param>(iCallable0, iCallable1, null); }

template <class Param>
ZCog<Param>& operator>>=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_If<Param>(ioCog0, iCallable1, null); }

// =================================================================================================
// MARK: - Binary sequential, sCog_Each

// Call cog0 till it finishes, then call cog1

template <class Param>
ZCog<Param> sCog_Each
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_If(iCallable0, iCallable1, iCallable1); }

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

// =================================================================================================
// MARK: - Binary parallel, sCog_And

template <class Param>
ZCog<Param> spCogFun_And(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_And
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(sIsPending(iCallable0) && not sIsTrue(iCallable1));

	static const ZMACRO_auto(spCallable, sCallable(spCogFun_And<Param>));
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
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	ZAssert(sIsPending(iCog0) && not sIsTrue(iCog1));

	ZCog<Param> newCog0 = iCog0;
	if (sCallAndUpdatePendingCog_Unchanged(newCog0, iParam))
		{
		if (sIsFalse(iCog1))
			return false;

		ZCog<Param> newCog1 = iCog1;
		if (sCallAndUpdatePendingCog_Unchanged(newCog1, iParam))
			return iSelf;

		if (sIsFalse(newCog1))
			return false;
		
		if (sIsTrue(newCog1))
			return newCog0;

		return spCog_And(newCog0, newCog1);
		}
	else if (sIsFalse(newCog0))
		{
		return false;
		}
	else if (sIsTrue(newCog0))
		{
		if (sIsFalse(iCog1))
			return false;

		if (sIsTrue(iCog1))
			return true;

		return iCog1->Call(iCog1, iParam);
		}
	else
		{
		if (sIsFalse(iCog1))
			return false;

		if (sIsTrue(iCog1))
			return newCog0;

		if (ZCog<Param> newCog1 = iCog1->Call(iCog1, iParam))
			{
			if (sIsTrue(newCog1))
				return newCog0;
			return spCog_And(newCog0, newCog1);
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

// =================================================================================================
// MARK: - Binary parallel, sCog_Or

template <class Param>
ZCog<Param> spCogFun_Or(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_Or
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(sIsPending(iCallable0) && not sIsFalse(iCallable1));

	static const ZMACRO_auto(spCallable, sCallable(spCogFun_Or<Param>));
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
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	ZAssert(sIsPending(iCog0) && iCog1);

	ZCog<Param> newCog0 = iCog0;
	if (sCallAndUpdatePendingCog_Unchanged(newCog0, iParam))
		{
		if (sIsTrue(iCog1))
			return true;

		ZCog<Param> newCog1 = iCog1;
		if (sCallAndUpdatePendingCog_Unchanged(newCog1, iParam))
			return iSelf;

		if (sIsFalse(newCog1))
			return newCog0;
		
		if (sIsTrue(newCog1))
			return true;

		return spCog_Or(newCog0, newCog1);
		}
	else if (sIsFalse(newCog0))
		{
		if (sIsTrue(iCog1))
			return true;

		return iCog1->Call(iCog1, iParam);
		}
	else if (sIsTrue(newCog0))
		{
		return true;
		}
	else
		{
		if (sIsTrue(iCog1))
			return true;

		if (ZCog<Param> newCog1 = iCog1->Call(iCog1, iParam))
			{
			if (sIsTrue(newCog1))
				return true;
			return spCog_Or(newCog0, newCog1);
			}

		return newCog0;
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

// =================================================================================================
// MARK: - Binary parallel, sCog_While

// Call cog0 and cog1 while cog1 is pending, result from cog1.

template <class Param>
ZCog<Param> spCogFun_While(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_While
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(sIsPending(iCallable0) && sIsPending(iCallable1));

	static const ZMACRO_auto(spCallable, sCallable(spCogFun_While<Param>));
	return sBindR(spCallable, iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> sCog_While
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsPending(iCallable0) && sIsPending(iCallable1))
		return spCog_While(iCallable0, iCallable1);
	return iCallable1;
	}

template <class Param>
ZCog<Param> spCogFun_While(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	ZAssert(sIsPending(iCog0) && sIsPending(iCog1));

	ZCog<Param> newCog0 = iCog0;
	if (sCallAndUpdatePendingCog_Unchanged(newCog0, iParam))
		{
		ZCog<Param> newCog1 = iCog1;
		if (sCallAndUpdatePendingCog_Unchanged(newCog1, iParam))
			return iSelf;

		if (sIsFinished(newCog1))
			return newCog1;

		return spCog_While(newCog0, newCog1);
		}
	else if (sIsFinished(newCog0))
		{
		return iCog1->Call(iCog1, iParam);
		}
	else
		{
		ZCog<Param> newCog1 = iCog1->Call(iCog1, iParam);

		if (sIsFinished(newCog1))
			return newCog1;

		return spCog_While(newCog0, newCog1);		
		}
	}

// =================================================================================================
// MARK: - Binary parallel, sCog_With

// Call cog0 and cog1 while cog0 is pending, result from cog0.

template <class Param>
ZCog<Param> spCogFun_With(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_With
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(sIsPending(iCallable0) && sIsPending(iCallable1));

	static const ZMACRO_auto(spCallable, sCallable(spCogFun_With<Param>));
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
	if (sCallAndUpdatePendingCog_Unchanged(newCog0, iParam))
		{
		ZCog<Param> newCog1 = iCog1;
		if (sCallAndUpdatePendingCog_Unchanged(newCog1, iParam))
			return iSelf;

		if (sIsFinished(newCog1))
			return newCog0;

		return spCog_With(newCog0, newCog1);
		}
	else if (sIsPending(newCog0))
		{
		ZCog<Param> newCog1 = iCog1->Call(iCog1, iParam);

		if (sIsFinished(newCog1))
			return newCog0;

		return spCog_With(newCog0, newCog1);		
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
// MARK: - Binary parallel, sCog_WithUnchanged

// Call cog0 and cog1 so long as cog0 is pending and unchanged, result from cog0

template <class Param>
ZCog<Param> spCogFun_WithUnchanged(const ZCog<Param>& iSelf, Param iParam,
	const ZCog<Param>& iCog0, const ZCog<Param>& iCog1);

template <class Param>
ZCog<Param> spCog_WithUnchanged
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	ZAssert(sIsPending(iCallable0) && sIsPending(iCallable1));

	static const ZMACRO_auto(spCallable, sCallable(spCogFun_WithUnchanged<Param>));
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
	if (sCallAndUpdatePendingCog_Unchanged(newCog0, iParam))
		{
		ZCog<Param> newCog1 = iCog1;
		if (sCallAndUpdatePendingCog_Unchanged(newCog1, iParam))
			return iSelf;

		if (sIsFinished(newCog1))
			return newCog0;

		return spCog_WithUnchanged(newCog0, newCog1);		
		}

	return newCog0;
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

} // namespace ZooLib

#endif // __ZCog_h__
