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

#ifndef __ZooLib_Cog_h__
#define __ZooLib_Cog_h__ 1

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"

namespace ZooLib {

// What we really want is a type which is a callable that returns a callable of that same type,
// and takes a callable of that type plus an additional templated param. So you can call it, passing
// itself as the first param and get back exactly the same object, or more usefully a different one.
// Syntactically it would look something like:
//   template <class Param> using Cog = ZRef<Callable<Cog<Param>(const Cog<Param>&,Param)>>;
// but this is explicitly disallowed in the standard, at least as of C++17.

// The closest approximation is to have Cog derived from a ZRef<Callable<...>>, and to use CRTP to
// get Cog itself into the callable's signature.

// It actually works quite well in practice, but it does require that some functions take the base
// class as the type, which to make it a bit more readable we define RefCallableCog as an alias.

// =================================================================================================
#pragma mark - Cog

template <class Param_p>
class Cog
:	public ZRef<Callable<Cog<Param_p>(const Cog<Param_p>&,Param_p)> >
	{
public:
	typedef Param_p Param;
	typedef Cog(Signature)(const Cog& iSelf, Param iParam);
	typedef Callable<Signature> Callable;
	typedef ZRef<Callable> inherited;

//--

	Cog()
		{}

	Cog(const Cog& iOther)
	:	inherited(iOther)
		{}

	virtual ~Cog()
		{}

	Cog& operator=(const Cog& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

//--

#if ZCONFIG_CPP >= 2011

	Cog(Cog&& iOther)
	:	inherited(std::move(iOther))
		{}

	Cog& operator=(Cog&& iOther)
		{
		inherited::operator=(std::move(iOther));
		return *this;
		}

#endif // ZCONFIG_CPP >= 2011

//--

	Cog(Callable* iCallable)
	:	inherited(iCallable)
		{}

	Cog& operator=(Callable* iCallable)
		{
		inherited::operator=(iCallable);
		return *this;
		}

//--

	Cog(const ZRef<Callable>& iOther)
	:	inherited(iOther)
		{}
	
	Cog& operator=(const ZRef<Callable>& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

#if ZCONFIG_CPP >= 2011

	Cog(ZRef<Callable>&& iOther)
	:	inherited(std::move(iOther))
		{}

	Cog& operator=(ZRef<Callable>&& iOther)
		{
		inherited::operator=(std::move(iOther));
		return *this;
		}

#endif // ZCONFIG_CPP >= 2011

//--

	Cog(const null_t&)
		{}

	Cog& operator=(const null_t& iNull)
		{
		inherited::operator=(iNull);
		return *this;
		}

//--

	Cog(const bool iBool);

	Cog& operator=(const bool iBool);
	};

template <class Param_p>
const Cog<Param_p>& sCog_False()
	{
	static const Cog<Param_p> spCog;
	return spCog;
	}

template <class Param_p>
Cog<Param_p> spCogFun_True(const Cog<Param_p>& iSelf, Param_p iParam)
	{ ZUnimplemented(); }

template <class Param_p>
Cog<Param_p> sCog_True()
	{
	static const Cog<Param_p> spCog_True = sCallable(spCogFun_True<Param_p>);
	return spCog_True;
	}

template <class Param_p>
Cog<Param_p>::Cog(const bool iBool)
:	inherited(iBool ? sCog_True<Param_p>() : null)
	{}

template <class Param_p>
Cog<Param_p>& Cog<Param_p>::operator=(const bool iBool)
	{
	inherited::operator=(iBool ? sCog_True<Param_p>() : null);
	return *this;
	}

// This requires too much inference:
//   template <class Param> using RefCallableCog = typename Cog<Param>::inherited;

template <class Param> using RefCallableCog = ZRef<Callable<Cog<Param>(const Cog<Param>&,Param)>>;

// =================================================================================================
#pragma mark - Nullary, sCog_Pending
// A no-op that is always pending, never finished.

template <class Param>
Cog<Param> spCogFun_Pending(const Cog<Param>& iSelf, Param iParam)
	{ return iSelf; }

template <class Param>
const Cog<Param>& sCog_Pending()
	{
	static const Cog<Param> spCog = sCallable(spCogFun_Pending<Param>);
	return spCog;
	}

// =================================================================================================
#pragma mark - Cog classification

template <class Param>
inline
bool sIsTrue(const RefCallableCog<Param>& iCallable)
	{ return sCog_True<Param>() == iCallable; }

template <class Param>
inline
bool sIsFalse(const RefCallableCog<Param>& iCallable)
	{ return not iCallable; }

template <class Param>
inline
bool sIsFinished(const RefCallableCog<Param>& iCallable)
	{ return sIsFalse(iCallable) || sIsTrue(iCallable); }

template <class Param>
inline
bool sIsPending(const RefCallableCog<Param>& iCallable)
	{ return not sIsFinished(iCallable); }

// =================================================================================================
#pragma mark -

// Ctor from callable to Cog. For use in cases where there would be ambiguity.

template <class Param>
Cog<Param> sCog(Callable<Cog<Param>(const Cog<Param>&,Param)>* iCallable)
	{ return Cog<Param>(iCallable); }

// =================================================================================================
#pragma mark - sCallCog variants

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
Cog<Param> sCallCog(const RefCallableCog<Param>& iCallable, Param iParam)
	{
	if (sIsFinished(iCallable))
		return iCallable;

	if (ZQ<Cog<Param> > theQ = iCallable->QCall(iCallable, iParam))
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
bool sCallUpdate_Cog_Changed(Cog& ioCog, typename Cog::Param iParam)
	{
	if (sIsFinished(ioCog))
		return false;

	if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
		return sQSet(ioCog, *theQ);
	ioCog.Clear();
	return true;
	}

template <class Cog>
bool sCallUpdate_Cog_Unchanged(Cog& ioCog, typename Cog::Param iParam)
	{
	if (sIsFinished(ioCog))
		return true;

	if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
		return not sQSet(ioCog, *theQ);
	ioCog.Clear();
	return false;
	}

template <class Cog>
bool sCallUpdate_PendingCog_StillPending(Cog& ioCog, typename Cog::Param iParam)
	{
	ZAssert(sIsPending(ioCog));

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
	ZAssert(sIsPending(ioCog));

	if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
		return sQSet(ioCog, *theQ);
	ioCog.Clear();
	return true;
	}

template <class Cog>
bool sCallUpdate_PendingCog_Unchanged(Cog& ioCog, typename Cog::Param iParam)
	{
	ZAssert(sIsPending(ioCog));

	if (ZQ<Cog> theQ = ioCog->QCall(ioCog, iParam))
		return not sQSet(ioCog, *theQ);
	ioCog.Clear();
	return false;
	}

// =================================================================================================
#pragma mark - Unary, sCog_Not

template <class Param>
class Callable_Cog_Not
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_Not(const RefCallableCog<Param>& iCallable)
	:	fCog(iCallable)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		if (ZQ<const Cog<Param> > newCogQ = fCog->QCall(fCog, iParam))
			{
			const Cog<Param>& newCog = *newCogQ;
			if (newCog == fCog)
				return iSelf;

			if (sIsTrue(newCog))
				return false;

			if (not sIsFalse(newCog))
				return new Callable_Cog_Not(newCog);
			}
		return true;
		}

	const Cog<Param> fCog;
	};

template <class Param>
Cog<Param> sCog_Not(
	const RefCallableCog<Param>& iCallable)
	{
	if (sIsFalse(iCallable))
		return true;

	if (sIsTrue(iCallable))
		return false;

	return new Callable_Cog_Not<Param>(iCallable);
	}

template <class Param>
Cog<Param> operator~(
	const RefCallableCog<Param>& iCallable)
	{ return sCog_Not<Param>(iCallable); }

// =================================================================================================
#pragma mark - Unary, sCog_Tautology (TrueIfFinished/TrueWhenChildIsFinished)

template <class Param>
class Callable_Cog_Tautology
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_Tautology(const RefCallableCog<Param>& iCallable)
	:	fCog(iCallable)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		if (ZQ<const Cog<Param> > newCogQ = fCog->QCall(fCog, iParam))
			{
			const Cog<Param>& newCog = *newCogQ;
			if (newCog == fCog)
				return iSelf;

			if (sIsPending(newCog))
				return new Callable_Cog_Tautology(newCog);
			}
		return true;
		}

	const Cog<Param> fCog;
	};

template <class Param>
Cog<Param> sCog_Tautology(
	const RefCallableCog<Param>& iCallable)
	{
	if (sIsFinished(iCallable))
		return true;

	return new Callable_Cog_Tautology<Param>(iCallable);
	}

template <class Param>
Cog<Param> operator+(const RefCallableCog<Param>& iCallable)
	{ return sCog_Tautology<Param>(iCallable); }

// =================================================================================================
#pragma mark - Unary, sCog_Contradiction (FalseIfFinished/FalseWhenChildIsFinished)

template <class Param>
class Callable_Cog_Contradiction
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_Contradiction(const RefCallableCog<Param>& iCallable)
	:	fCog(iCallable)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		if (ZQ<const Cog<Param> > newCogQ = fCog->QCall(fCog, iParam))
			{
			const Cog<Param>& newCog = *newCogQ;
			if (newCog == fCog)
				return iSelf;

			if (sIsPending(newCog))
				return new Callable_Cog_Contradiction(newCog);
			}
		return false;
		}

	const Cog<Param> fCog;
	};

template <class Param>
Cog<Param> sCog_Contradiction(
	const RefCallableCog<Param>& iCallable)
	{
	if (sIsFinished(iCallable))
		return false;

	return new Callable_Cog_Contradiction<Param>(iCallable);
	}

template <class Param>
Cog<Param> operator-(const RefCallableCog<Param>& iCallable)
	{ return sCog_Contradiction<Param>(iCallable); }

// =================================================================================================
#pragma mark - Unary, sCog_Repeat

template <class Param>
class Callable_Cog_Repeat
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_Repeat(const RefCallableCog<Param>& iCallable_Init, const RefCallableCog<Param>& iCallable)
	:	fCog_Init(iCallable_Init)
	,	fCog(iCallable)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		if (ZQ<const Cog<Param> > newCogQ = fCog->QCall(fCog, iParam))
			{
			const Cog<Param>& newCog = *newCogQ;
			if (newCog == fCog)
				return iSelf;

			if (sIsFalse(newCog))
				return false;
			
			if (not sIsTrue(newCog))
				return new Callable_Cog_Repeat(fCog_Init, newCog);
		
			// To get unbroken repetition on hitting a term we need to call the new cog, but must
			// not make the call if it's an iSelf-equivalent (infinite recursion would ensue).
			if (fCog_Init == fCog)
				return iSelf;
			else
				return sCallCog<Param>(new Callable_Cog_Repeat(fCog_Init, fCog_Init), iParam);
			}
		return false;
		}

	const Cog<Param> fCog_Init;
	const Cog<Param> fCog;
	};

template <class Param>
Cog<Param> sCog_Repeat(const RefCallableCog<Param>& iCallable)
	{
	if (sIsFalse(iCallable))
		return false;

	if (sIsTrue(iCallable))
		return sCog_Pending<Param>();

	return new Callable_Cog_Repeat<Param>(iCallable, iCallable);
	}

//template <class Param>
//Cog<Param> operator*(const RefCallableCog<Param>& iCallable)
//	{ return sCog_Repeat<Param>(iCallable); }

// =================================================================================================
#pragma mark - Binary sequential, sCog_Each

// Call cog0 till it finishes, then call/return cog1

template <class Param>
class Callable_Cog_Each
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_Each(
		const RefCallableCog<Param>& iCallable0,
		const RefCallableCog<Param>& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && sIsPending(fCog1));

		if (ZQ<Cog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const Cog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				return iSelf;

			if (sIsPending(newCog0))
				return new Callable_Cog_Each(newCog0, fCog1);
			}
		return fCog1->QCall(fCog1, iParam);
		}

	const Cog<Param> fCog0;
	const Cog<Param> fCog1;
	};

template <class Param>
Cog<Param> sCog_Each(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{
	if (sIsFinished(iCallable0))
		return iCallable1;

	if (sIsFinished(iCallable1))
		return iCallable0;

	return new Callable_Cog_Each<Param>(iCallable0, iCallable1);
	}

template <class Param>
Cog<Param> operator^(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{ return sCog_Each<Param>(iCallable0, iCallable1); }

template <class Param>
Cog<Param>& operator^=(
	Cog<Param>& ioCog0,
	const RefCallableCog<Param>& iCallable1)
	{ return ioCog0 = ioCog0 ^ iCallable1; }

// =================================================================================================
#pragma mark - Binary sequential, sCog_EachIf

// Call condition till it finishes, if true call/return callable

template <class Param>
class Callable_Cog_EachIf
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_EachIf(
		const RefCallableCog<Param>& iCondition,
		const RefCallableCog<Param>& iCallable)
	:	fCondition(iCondition)
	,	fCog(iCallable)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		if (ZQ<Cog<Param> > newConditionQ = fCondition->QCall(fCondition, iParam))
			{
			const Cog<Param>& newCondition = *newConditionQ;
			if (newCondition == fCondition)
				return iSelf;

			if (sIsFalse(newCondition))
				return newCondition;

			if (sIsTrue(newCondition))
				{
				if (sIsFinished(fCog))
					return fCog;
				return fCog->QCall(fCog, iParam);
				}

			return new Callable_Cog_EachIf(newCondition, fCog);
			}
		else
			{
			return false;
			}
		}

	const Cog<Param> fCondition;
	const Cog<Param> fCog;
	};

template <class Param>
Cog<Param> sCog_EachIf(
	const RefCallableCog<Param>& iCondition,
	const RefCallableCog<Param>& iCallable)
	{
	if (sIsFalse(iCondition))
		return iCondition;

	if (sIsTrue(iCondition))
		return iCallable;

	return new Callable_Cog_EachIf<Param>(iCondition, iCallable);
	}

template <class Param>
Cog<Param> operator>>(
	const RefCallableCog<Param>& iCondition,
	const RefCallableCog<Param>& iCallable)
	{ return sCog_EachIf<Param>(iCondition, iCallable); }

template <class Param>
Cog<Param>& operator>>=(
	Cog<Param>& ioCogCondition,
	const RefCallableCog<Param>& iCallable)
	{ return ioCogCondition = sCog_EachIf<Param>(ioCogCondition, iCallable); }

// =================================================================================================
#pragma mark - Binary parallel, sCog_And

// Call cog0 and cog1 till one of them finishes.
// If the finished cog was true call/return the other cog, otherwise return false.

template <class Param>
class Callable_Cog_And
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_And(const RefCallableCog<Param>& iCallable0,
		const RefCallableCog<Param>& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && not sIsTrue(fCog1));
	
		if (ZQ<Cog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const Cog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (sIsFalse(fCog1))
					return false;
		
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFalse(newCog1))
						return false;
					
					if (sIsTrue(newCog1))
						return newCog0;
			
					return new Callable_Cog_And(newCog0, newCog1);
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
		
				return fCog1->QCall(fCog1, iParam);
				}
			else
				{
				if (sIsFalse(fCog1))
					return false;
		
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;

					if (sIsFalse(newCog1))
						return false;

					if (sIsTrue(newCog1))
						return newCog0;

					return new Callable_Cog_And(newCog0, newCog1);
					}
		
				return false;
				}
			}
		else
			{
			return false;
			}
		}

	const Cog<Param> fCog0;
	const Cog<Param> fCog1;
	};

template <class Param>
Cog<Param> sCog_And(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{
	if (sIsFalse(iCallable0))
		return false;

	if (sIsTrue(iCallable0))
		return iCallable1;

	if (sIsTrue(iCallable1))
		return iCallable0;

	return new Callable_Cog_And<Param>(iCallable0, iCallable1);
	}

template <class Param>
Cog<Param> operator&(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{ return sCog_And<Param>(iCallable0, iCallable1); }

template <class Param>
Cog<Param>& operator&=(
	Cog<Param>& ioCog0,
	const RefCallableCog<Param>& iCallable1)
	{ return ioCog0 = sCog_And<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark - Binary parallel, sCog_Or

// Call cog0 and cog1 till one of them finishes.
// If the finished cog was false return the other cog, otherwise return true.

template <class Param>
class Callable_Cog_Or
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_Or(const RefCallableCog<Param>& iCallable0,
		const RefCallableCog<Param>& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && not sIsFalse(fCog1));
	
		if (ZQ<Cog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const Cog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (sIsTrue(fCog1))
					return true;
		
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFalse(newCog1))
						return newCog0;
					
					if (sIsTrue(newCog1))
						return true;
			
					return new Callable_Cog_Or(newCog0, newCog1);
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
		
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
					if (sIsTrue(newCog1))
						return true;
					return new Callable_Cog_Or(newCog0, newCog1);
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

	const Cog<Param> fCog0;
	const Cog<Param> fCog1;
	};

template <class Param>
Cog<Param> sCog_Or(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{
	if (sIsFalse(iCallable0))
		return iCallable1;

	if (sIsTrue(iCallable0))
		return true;

	if (sIsFalse(iCallable1))
		return iCallable0;

	return new Callable_Cog_Or<Param>(iCallable0, iCallable1);
	}

template <class Param>
Cog<Param> operator|(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{ return sCog_Or<Param>(iCallable0, iCallable1); }

template <class Param>
Cog<Param>& operator|=(
	Cog<Param>& ioCog0,
	const RefCallableCog<Param>& iCallable1)
	{ return ioCog0 = sCog_Or<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark - Binary parallel, sCog_With

// Call cog0 and cog1 while cog0 is pending, result from cog0.

template <class Param>
class Callable_Cog_With
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_With(const RefCallableCog<Param>& iCallable0,
		const RefCallableCog<Param>& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && sIsPending(fCog1));
	
		if (ZQ<Cog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const Cog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Callable_Cog_With(newCog0, newCog1);
					}
				return newCog0;
				}
			else if (sIsPending(newCog0))
				{
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
			
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Callable_Cog_With(newCog0, newCog1);
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

	const Cog<Param> fCog0;
	const Cog<Param> fCog1;
	};

template <class Param>
Cog<Param> sCog_With(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{
	if (sIsPending(iCallable0) && sIsPending(iCallable1))
		return new Callable_Cog_With<Param>(iCallable0, iCallable1);
	return iCallable0;
	}

template <class Param>
Cog<Param> operator/(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{ return sCog_With<Param>(iCallable0, iCallable1); }

template <class Param>
Cog<Param>& operator/=(
	Cog<Param>& ioCog0,
	const RefCallableCog<Param>& iCallable1)
	{ return ioCog0 = sCog_With<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark - Binary parallel, sCog_WithUnchanged

// Call cog0 and cog1 so long as cog0 is pending and unchanged, result from cog0

template <class Param>
class Callable_Cog_WithUnchanged
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_WithUnchanged(const RefCallableCog<Param>& iCallable0,
		const RefCallableCog<Param>& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && sIsPending(fCog1));
	
		if (ZQ<Cog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const Cog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Callable_Cog_WithUnchanged(newCog0, newCog1);
					}
				}		
			return newCog0;
			}
		else
			{
			return false;
			}
		}

	const Cog<Param> fCog0;
	const Cog<Param> fCog1;
	};

template <class Param>
Cog<Param> sCog_WithUnchanged(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{
	if (sIsPending(iCallable0) && sIsPending(iCallable1))
		return new Callable_Cog_WithUnchanged<Param>(iCallable0, iCallable1);
	return iCallable0;
	}

template <class Param>
Cog<Param> operator%(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{ return sCog_WithUnchanged<Param>(iCallable0, iCallable1); }

template <class Param>
Cog<Param>& operator%=(
	Cog<Param>& ioCog0,
	const RefCallableCog<Param>& iCallable1)
	{ return ioCog0 = sCog_WithUnchanged<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark - Binary parallel, sCog_Plus

// Call cog0 and cog1, till both have finished, result is that of the last to return.

template <class Param>
class Callable_Cog_Plus
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_Plus(const RefCallableCog<Param>& iCallable0,
		const RefCallableCog<Param>& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && sIsPending(fCog1));
	
		if (ZQ<Cog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const Cog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Callable_Cog_Plus(newCog0, newCog1);
					}
				return newCog0;
				}
			else if (sIsPending(newCog0))
				{
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
					if (sIsFinished(newCog1))
						return newCog0;
			
					return new Callable_Cog_Plus(newCog0, newCog1);
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

	const Cog<Param> fCog0;
	const Cog<Param> fCog1;
	};

template <class Param>
Cog<Param> sCog_Plus(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{
	if (sIsFinished(iCallable0))
		return iCallable1;

	if (sIsFinished(iCallable1))
		return iCallable0;

	return new Callable_Cog_Plus<Param>(iCallable0, iCallable1);
	}

template <class Param>
Cog<Param> operator+(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{ return sCog_Plus<Param>(iCallable0, iCallable1); }

template <class Param>
Cog<Param>& operator+=(
	Cog<Param>& ioCog0,
	const RefCallableCog<Param>& iCallable1)
	{ return ioCog0 = sCog_Plus<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark - Binary parallel, sCog_Minus

// Call cog0 and cog1, till either finishes, result is that of the first to finish

template <class Param>
class Callable_Cog_Minus
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_Minus(const RefCallableCog<Param>& iCallable0,
		const RefCallableCog<Param>& iCallable1)
	:	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		ZAssert(sIsPending(fCog0) && sIsPending(fCog1));
	
		if (ZQ<Cog<Param> > newCog0Q = fCog0->QCall(fCog0, iParam))
			{
			const Cog<Param>& newCog0 = *newCog0Q;
			if (newCog0 == fCog0)
				{
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
					if (newCog1 == fCog1)
						return iSelf;
			
					if (sIsFinished(newCog1))
						return newCog1;
			
					return new Callable_Cog_Minus(newCog0, newCog1);
					}
				return newCog0;
				}
			else if (sIsPending(newCog0))
				{
				if (ZQ<Cog<Param> > newCog1Q = fCog1->QCall(fCog1, iParam))
					{
					const Cog<Param>& newCog1 = *newCog1Q;
					if (sIsFinished(newCog1))
						return newCog1;
			
					return new Callable_Cog_Minus(newCog0, newCog1);
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

	const Cog<Param> fCog0;
	const Cog<Param> fCog1;
	};

template <class Param>
Cog<Param> sCog_Minus(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{
	if (sIsFinished(iCallable0))
		return iCallable0;

	if (sIsFinished(iCallable1))
		return iCallable1;

	return new Callable_Cog_Minus<Param>(iCallable0, iCallable1);
	}

template <class Param>
Cog<Param> operator-(
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{ return sCog_Minus<Param>(iCallable0, iCallable1); }

template <class Param>
Cog<Param>& operator-=(
	Cog<Param>& ioCog0,
	const RefCallableCog<Param>& iCallable1)
	{ return ioCog0 = sCog_Minus<Param>(ioCog0, iCallable1); }

// =================================================================================================
#pragma mark - sCog_Fallback

template <class Param>
Cog<Param> sCog_Fallback(
	const RefCallableCog<Param>& iPreferred,
	const RefCallableCog<Param>& iFallback)
	{ return iPreferred ? iPreferred : iFallback; }

// =================================================================================================
#pragma mark - Ternary sequential, sCog_If

// Call condition till it finishes, if true then call/return cog0, otherwise call/return cog1

template <class Param>
class Callable_Cog_If
:	public Cog<Param>::Callable
	{
public:
	Callable_Cog_If(const RefCallableCog<Param>& iCondition,
		const RefCallableCog<Param>& iCallable0,
		const RefCallableCog<Param>& iCallable1)
	:	fCondition(iCondition)
	,	fCog0(iCallable0)
	,	fCog1(iCallable1)
		{}

	virtual ZQ<Cog<Param> > QCall(const Cog<Param>& iSelf, Param iParam)
		{
		if (ZQ<Cog<Param> > newConditionQ = fCondition->QCall(fCondition, iParam))
			{
			const Cog<Param>& newCondition = *newConditionQ;
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
		
			return new Callable_Cog_If(newCondition, fCog0, fCog1);
			}

		if (sIsFinished(fCog1))
			return fCog1;
		return fCog1->QCall(fCog1, iParam);
		}

	const Cog<Param> fCondition;
	const Cog<Param> fCog0;
	const Cog<Param> fCog1;
	};

template <class Param>
Cog<Param> sCog_If(
	const RefCallableCog<Param>& iCondition,
	const RefCallableCog<Param>& iCallable0,
	const RefCallableCog<Param>& iCallable1)
	{
	if (sIsFalse(iCondition))
		return iCallable1;

	if (sIsTrue(iCondition))
		return iCallable0;

	return new Callable_Cog_If<Param>(iCondition, iCallable0, iCallable1);
	}

} // namespace ZooLib

#endif // __ZooLib_Cog_h__
