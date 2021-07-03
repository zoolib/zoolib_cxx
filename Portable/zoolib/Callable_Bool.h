// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Bool_h__
#define __ZooLib_Callable_Bool_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

typedef Callable<bool()> Callable_Bool;

// =================================================================================================
#pragma mark - Ctors

ZP<Callable_Bool> sCallable_True();
ZP<Callable_Bool> sCallable_False();
ZP<Callable_Bool> sCallable_Not(const ZP<Callable_Bool>& iCallable);
ZP<Callable_Bool> sCallable_And(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1);
ZP<Callable_Bool> sCallable_Or(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1);
ZP<Callable_Bool> sCallable_Xor(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1);

// =================================================================================================
#pragma mark - Concise Ctors

inline ZP<Callable_Bool> sNot(const ZP<Callable_Bool>& iCallable)
	{ return sCallable_Not(iCallable); }

inline ZP<Callable_Bool> sAnd(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1)
	{ return sCallable_And(i0, i1); }

inline ZP<Callable_Bool> sOr(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1)
	{ return sCallable_Or(i0, i1); }

inline ZP<Callable_Bool> sXor(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1)
	{ return sCallable_Xor(i0, i1); }

// =================================================================================================
#pragma mark - Infix notation

inline
ZP<Callable_Bool> operator~(const ZP<Callable_Bool>& iCallable)
	{ return sCallable_Not(iCallable); }

inline
ZP<Callable_Bool> operator&(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1)
	{ return sCallable_And(i0, i1); }

inline
ZP<Callable_Bool>& operator&=(ZP<Callable_Bool>& io0, const ZP<Callable_Bool>& i1)
	{ return io0 = sCallable_And(io0, i1); }

inline
ZP<Callable_Bool> operator|(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1)
	{ return sCallable_Or(i0, i1); }

inline
ZP<Callable_Bool>& operator|=(ZP<Callable_Bool>& io0, const ZP<Callable_Bool>& i1)
	{ return io0 = sCallable_Or(io0, i1); }

inline
ZP<Callable_Bool> operator^(const ZP<Callable_Bool>& i0, const ZP<Callable_Bool>& i1)
	{ return sCallable_Xor(i0, i1); }

inline
ZP<Callable_Bool>& operator^=(ZP<Callable_Bool>& io0, const ZP<Callable_Bool>& i1)
	{ return io0 = sCallable_Xor(io0, i1); }

} // namespace ZooLib

#endif // __ZooLib_Callable_Bool_h__
