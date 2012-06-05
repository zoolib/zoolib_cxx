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
#include "zoolib/ZMACRO_auto.h"

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
		{ ZUnimplemented(); }

	static const ZCog spCog_True;

	static const ZCog& sTrue()
		{ return spCog_True; }
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
inline
bool sIsTrue(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return ZCog<Param>::sTrue() == iCallable; }

template <class Param>
inline
bool sIsFalse(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return not iCallable; }

template <class Param>
inline
bool sIsFinished(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sIsFalse(iCallable) || sIsTrue(iCallable); }

template <class Param>
inline
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
Cog sCallCog(const Cog& iCog, typename Cog::Param iParam)
	{
	if (sIsFinished(iCog))
		return iCog;

	if (ZQ<Cog> theQ = iCog->QCall(iCog, iParam))
		return *theQ;

	return null;
	}

template <class Param>
ZCog<Param> sCallCog
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable, Param iParam)
	{
	if (sIsFinished(iCallable))
		return iCallable;

	if (ZQ<ZCog<Param> > theQ = iCallable->QCall(iCallable, iParam))
		return *theQ;

	return null;
	}

template <class Cog>
void sCallUpdate_Cog(Cog& ioCog, typename Cog::Param iParam)
	{
	if (sIsPending(ioCog))
		{
		if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
			ioCog = *theQ;
		else
			ioCog.Clear();
		}
	}

template <class Cog>
bool sCallUpdate_PendingCog_StillPending(Cog& ioCog, typename Cog::Param iParam)
	{
	if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
		{
		ioCog = *theQ;
		return sIsPending(ioCog);
		}
	ioCog.Clear();
	return false;
	}

template <class Cog>
bool sCallUpdate_PendingCog_Changed(Cog& ioCog, typename Cog::Param iParam)
	{
	if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
		return sCompareSet(ioCog, *theQ);
	ioCog.Clear();
	return true;
	}

template <class Cog>
bool sCallUpdate_PendingCog_Unchanged(Cog& ioCog, typename Cog::Param iParam)
	{
	if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
		return not sCompareSet(ioCog, *theQ);
	ioCog.Clear();
	return false;
	}

template <class Cog>
bool sCallUpdate_Cog_Changed(Cog& ioCog, typename Cog::Param iParam)
	{
	if (sIsFinished(ioCog))
		return false;

	if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
		return sCompareSet(ioCog, *theQ);
	ioCog.Clear();
	return true;
	}

template <class Cog>
bool sCallUpdate_Cog_Unchanged(Cog& ioCog, typename Cog::Param iParam)
	{
	if (sIsFinished(ioCog))
		return true;

	if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
		return not sCompareSet(ioCog, *theQ);
	ioCog.Clear();
	return false;
	}

// =================================================================================================
// MARK: - Unary, sCog_Not

template <class Param>
class Cog_Not
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_Not(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	:	fCog(iCallable)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		if (ZQ<const ZCog<Param> > newCogQ = fCog->QCall(fCog, iParam))
			{
			const ZCog<Param>& newCog = *newCogQ;
			if (newCog == fCog)
				return iSelf;

			if (sIsFalse(newCog))
				return true;

			if (sIsTrue(newCog))
				return false;

			return new Cog_Not(newCog);
			}
		return true;
		}

	const ZCog<Param> fCog;
	};

template <class Param>
ZCog<Param> sCog_Not
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (sIsFalse(iCallable))
		return true;

	if (sIsTrue(iCallable))
		return false;

	return new Cog_Not<Param>(iCallable);
	}

template <class Param>
ZCog<Param> operator~
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Not<Param>(iCallable); }

// =================================================================================================
// MARK: - Unary, sCog_Tautology (TrueIfFinished)

template <class Param>
class Cog_Tautology
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_Tautology(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	:	fCog(iCallable)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		if (ZQ<const ZCog<Param> > newCogQ = fCog->QCall(fCog, iParam))
			{
			const ZCog<Param>& newCog = *newCogQ;
			if (newCog == fCog)
				return iSelf;

			if (sIsFinished(newCog))
				return true;

			return new Cog_Tautology(newCog);
			}
		return true;
		}

	const ZCog<Param> fCog;
	};

template <class Param>
ZCog<Param> sCog_Tautology
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (sIsFinished(iCallable))
		return true;

	return new Cog_Tautology<Param>(iCallable);
	}

template <class Param>
ZCog<Param> operator+(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Tautology<Param>(iCallable); }

// =================================================================================================
// MARK: - Unary, sCog_Contradiction (FalseIfFinished)

template <class Param>
class Cog_Contradiction
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_Contradiction(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	:	fCog(iCallable)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		if (ZQ<const ZCog<Param> > newCogQ = fCog->QCall(fCog, iParam))
			{
			const ZCog<Param>& newCog = *newCogQ;
			if (newCog == fCog)
				return iSelf;

			if (sIsFinished(newCog))
				return false;

			return new Cog_Contradiction(newCog);
			}
		return false;
		}

	const ZCog<Param> fCog;
	};

template <class Param>
ZCog<Param> sCog_Contradiction
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (sIsFinished(iCallable))
		return false;

	return new Cog_Contradiction<Param>(iCallable);
	}

template <class Param>
ZCog<Param> operator-(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Contradiction<Param>(iCallable); }

// =================================================================================================
// MARK: - Unary, sCog_Repeat

template <class Param>
class Cog_Repeat
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_Repeat(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable_Init,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	:	fCog_Init(iCallable_Init)
	,	fCog(iCallable)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		if (ZQ<const ZCog<Param> > newCogQ = fCog->QCall(fCog, iParam))
			{
			const ZCog<Param>& newCog = *newCogQ;
			if (newCog == fCog)
				return iSelf;

			if (sIsFalse(newCog))
				return false;
			
			if (not sIsTrue(newCog))
				return new Cog_Repeat(fCog_Init, newCog);
		
			// To get unbroken repetition on hitting a term we need to call the new cog, but must
			// not make the call if it's an iSelf-equivalent (infinite recursion would ensue).
			if (fCog_Init == fCog)
				return iSelf;
			else
				return sCallCog<Param>(new Cog_Repeat(fCog_Init, fCog_Init), iParam);
			}
		return false;
		}

	const ZCog<Param> fCog_Init;
	const ZCog<Param> fCog;
	};

template <class Param>
ZCog<Param> sCog_Repeat(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{
	if (sIsFalse(iCallable))
		return false;

	if (sIsTrue(iCallable))
		return sCog_Pending<Param>();

	return new Cog_Repeat<Param>(iCallable, iCallable);
	}

template <class Param>
ZCog<Param> operator*(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_Repeat<Param>(iCallable); }

// =================================================================================================
// MARK: - Ternary sequential, sCog_If

// Call cog0 till it finishes, if true then call cog1

template <class Param>
class Cog_If
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_If(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCondition,
		const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
		const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	:	fCondition(iCondition)
	,	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		if (ZQ<ZCog<Param> > newConditionQ = fCondition->QCall(fCondition, iParam))
			{
			const ZCog<Param>& newCondition = *newConditionQ;
			if (newCondition == fCondition)
				return iSelf;

			if (sIsFalse(newCondition))
				{
				if (sIsFinished(fCog1))
					return fCog1;
				return fCog1->QCall(fCog1, iParam);
				}
		
			if (sIsTrue(newCondition))
				{
				if (sIsFinished(fCog0))
					return fCog0;
				return fCog0->QCall(fCog0, iParam);
				}
		
			return new Cog_If(newCondition, fCog0, fCog1);
			}

		if (sIsFinished(fCog1))
			return fCog1;
		return fCog1->QCall(fCog1, iParam);
		}

	const ZCog<Param> fCondition;
	const ZCog<Param> fCog0;
	const ZCog<Param> fCog1;
	};

template <class Param>
ZCog<Param> sCog_If
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCondition,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsFalse(iCondition))
		return iCallable1;

	if (sIsTrue(iCondition))
		return iCallable0;

	return new Cog_If<Param>(iCondition, iCallable0, iCallable1);
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
class Cog_And
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_And(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
		const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && not sIsTrue(fCog1));
	
		if (ZQ<ZCog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const ZCog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (sIsFalse(fCog1))
					return false;
		
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFalse(newCog1))
						return false;
					
					if (sIsTrue(newCog1))
						return newCog0;
			
					return new Cog_And(newCog0, newCog1);
					}
				return false;
				}
			else if (sIsFalse(newCog0))
				{
				return false;
				}
			else if (sIsTrue(newCog0))
				{
				if (sIsFalse(fCog1))
					return false;
		
				if (sIsTrue(fCog1))
					return true;
		
				return fCog1->QCall(fCog1, iParam);
				}
			else
				{
				if (sIsFalse(fCog1))
					return false;
		
				if (sIsTrue(fCog1))
					return newCog0;
		
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (sIsTrue(newCog1))
						return newCog0;
					return new Cog_And(newCog0, newCog1);
					}
		
				return false;
				}
			}
		else
			{
			return false;
			}
		}

	const ZCog<Param> fCog0;
	const ZCog<Param> fCog1;
	};

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

	return new Cog_And<Param>(iCallable0, iCallable1);
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
class Cog_Or
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_Or(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
		const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && fCog1);
	
		if (ZQ<ZCog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const ZCog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (sIsTrue(fCog1))
					return true;
		
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFalse(newCog1))
						return newCog0;
					
					if (sIsTrue(newCog1))
						return true;
			
					return new Cog_Or(newCog0, newCog1);
					}
				return newCog0;
				}
			else if (sIsFalse(newCog0))
				{
				if (sIsTrue(fCog1))
					return true;
		
				return fCog1->QCall(fCog1, iParam);
				}
			else if (sIsTrue(newCog0))
				{
				return true;
				}
			else
				{
				if (sIsTrue(fCog1))
					return true;
		
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (sIsTrue(newCog1))
						return true;
					return new Cog_Or(newCog0, newCog1);
					}		
				return newCog0;
				}
			}
		else
			{
			if (sIsTrue(fCog1))
				return true;
	
			return fCog1->QCall(fCog1, iParam);
			}
		}

	const ZCog<Param> fCog0;
	const ZCog<Param> fCog1;
	};

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

	return new Cog_Or<Param>(iCallable0, iCallable1);
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
class Cog_While
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_While(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
		const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && sIsPending(fCog1));
	
		if (ZQ<ZCog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const ZCog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFinished(newCog1))
						return newCog1;
			
					return new Cog_While(newCog0, newCog1);
					}
				else
					{
					return false;
					}
				}
			else if (sIsFinished(newCog0))
				{
				return fCog1->QCall(fCog1, iParam);
				}
			else
				{
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (sIsFinished(newCog1))
						return newCog1;
			
					return new Cog_While(newCog0, newCog1);
					}
				return false;
				}
			}
		else
			{
			return fCog1->QCall(fCog1, iParam);
			}
		}

	const ZCog<Param> fCog0;
	const ZCog<Param> fCog1;
	};

template <class Param>
ZCog<Param> sCog_While
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsPending(iCallable0) && sIsPending(iCallable1))
		return new Cog_While<Param>(iCallable0, iCallable1);
	return iCallable1;
	}

// =================================================================================================
// MARK: - Binary parallel, sCog_With

// Call cog0 and cog1 while cog0 is pending, result from cog0.

template <class Param>
class Cog_With
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_With(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
		const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && sIsPending(fCog1));
	
		if (ZQ<ZCog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const ZCog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Cog_With(newCog0, newCog1);
					}
				return newCog0;
				}
			else if (sIsPending(newCog0))
				{
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
			
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Cog_With(newCog0, newCog1);
					}
				return newCog0;
				}
			else
				{
				return newCog0;
				}
			}
		else
			{
			return false;
			}
		}

	const ZCog<Param> fCog0;
	const ZCog<Param> fCog1;
	};

template <class Param>
ZCog<Param> sCog_With
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsPending(iCallable0) && sIsPending(iCallable1))
		return new Cog_With<Param>(iCallable0, iCallable1);
	return iCallable1;
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
// MARK: - Binary parallel, sCog_Plus

// Call cog0 and cog1

template <class Param>
class Cog_Plus
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_Plus(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
		const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && sIsPending(fCog1));
	
		if (ZQ<ZCog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const ZCog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Cog_Plus(newCog0, newCog1);
					}
				return newCog0;
				}
			else if (sIsPending(newCog0))
				{
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Cog_Plus(newCog0, newCog1);
					}
				return newCog0;
				}
			else
				{
				return fCog1->QCall(fCog1, iParam);
				}
			}
		else
			{
			return fCog1->QCall(fCog1, iParam);
			}
		}

	const ZCog<Param> fCog0;
	const ZCog<Param> fCog1;
	};

template <class Param>
ZCog<Param> sCog_Plus
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsFinished(iCallable0))
		return iCallable1;

	if (sIsFinished(iCallable1))
		return iCallable0;

	return new Cog_Plus<Param>(iCallable0, iCallable1);
	}

template <class Param>
ZCog<Param> operator+
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return sCog_Plus<Param>(iCallable0, iCallable1); }

template <class Param>
ZCog<Param>& operator+=
	(ZCog<Param>& ioCog0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{ return ioCog0 = sCog_Plus<Param>(ioCog0, iCallable1); }

// =================================================================================================
// MARK: - Binary parallel, sCog_WithUnchanged

// Call cog0 and cog1 so long as cog0 is pending and unchanged, result from cog0

template <class Param>
class Cog_WithUnchanged
:	public ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>
	{
public:
	Cog_WithUnchanged(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
		const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<ZCog<Param> > QCall(const ZCog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && sIsPending(fCog1));
	
		if (ZQ<ZCog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const ZCog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (ZQ<ZCog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const ZCog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Cog_WithUnchanged(newCog0, newCog1);		
					}
				}		
			return newCog0;
			}
		else
			{
			return false;
			}
		}

	const ZCog<Param> fCog0;
	const ZCog<Param> fCog1;
	};

template <class Param>
ZCog<Param> sCog_WithUnchanged
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable0,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable1)
	{
	if (sIsPending(iCallable0) && sIsPending(iCallable1))
		return new Cog_WithUnchanged<Param>(iCallable0, iCallable1);
	return iCallable1;
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
// MARK: - sCog_Fallback

template <class Param>
ZCog<Param> sCog_Fallback
	(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iPreferred,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iFallback)
	{ return iPreferred ? iPreferred : iFallback; }

} // namespace ZooLib

#endif // __ZCog_h__
