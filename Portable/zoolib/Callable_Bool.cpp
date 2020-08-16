// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Callable_Bool.h"

#include "zoolib/Callable_Const.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Ctors

ZP<Callable_Bool> sCallable_True()
	{ return sCallable_Const(true); }

ZP<Callable_Bool> sCallable_False()
	{ return sCallable_Const(false); }

ZP<Callable_Bool> sCallable_Not(const ZP<Callable_Bool>& iCallable)
	{
	struct Callable : public Callable_Bool
		{
		Callable(const ZP<Callable_Bool>& iCallable) : fCallable(iCallable) {}
		virtual ZQ<bool> QCall()
			{
			if (const ZQ<bool> theQ = sQCall(fCallable))
				return not *theQ;
			return null;
			}
		const ZP<Callable_Bool> fCallable;
		};
	return new Callable(iCallable);
	}

ZP<Callable_Bool> sCallable_And(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1)
	{
	struct Callable : public Callable_Bool
		{
		Callable(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1) : f0(i0), f1(i1) {}
		virtual ZQ<bool> QCall()
			{
			if (const ZQ<bool> theQ0 = sQCall(f0))
				{
				if (*theQ0)
					return sQCall(f1);
				return false;
				}
			return null;
			}
		const ZP<Callable_Bool> f0, f1;
		};
	return new Callable(i0, i1);
	}

ZP<Callable_Bool> sCallable_Or(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1)
	{
	struct Callable : public Callable_Bool
		{
		Callable(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1) : f0(i0), f1(i1) {}
		virtual ZQ<bool> QCall()
			{
			if (const ZQ<bool> theQ0 = sQCall(f0))
				{
				if (*theQ0)
					return true;
				}
			return sQCall(f1);
			}
		const ZP<Callable_Bool> f0, f1;
		};
	return new Callable(i0, i1);
	}

ZP<Callable_Bool> sCallable_Xor(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1)
	{
	struct Callable : public Callable_Bool
		{
		Callable(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1) : f0(i0), f1(i1) {}
		virtual ZQ<bool> QCall()
			{
			if (const ZQ<bool> theQ0 = sQCall(f0))
				{
				if (const ZQ<bool> theQ1 = sQCall(f1))
					return *theQ0 ^ *theQ1;
				}
			return null;
			}
		const ZP<Callable_Bool> f0, f1;
		};
	return new Callable(i0, i1);
	}

} // namespace ZooLib
