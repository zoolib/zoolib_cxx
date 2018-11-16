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

#ifndef __ZooLib_Callable_Bool_h__
#define __ZooLib_Callable_Bool_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Ctors

ZRef<Callable_Bool> sCallable_True();
ZRef<Callable_Bool> sCallable_False();
ZRef<Callable_Bool> sCallable_Not(const ZRef<Callable_Bool>& iCallable);
ZRef<Callable_Bool> sCallable_And(const ZRef<Callable_Bool>& i0, const ZRef<Callable_Bool>& i1);
ZRef<Callable_Bool> sCallable_Or(const ZRef<Callable_Bool>& i0, const ZRef<Callable_Bool>& i1);
ZRef<Callable_Bool> sCallable_Xor(const ZRef<Callable_Bool>& i0, const ZRef<Callable_Bool>& i1);

// =================================================================================================
#pragma mark - Concise Ctors

inline ZRef<Callable_Bool> sNot(const ZRef<Callable_Bool>& iCallable)
	{ return sCallable_Not(iCallable); }

inline ZRef<Callable_Bool> sAnd(const ZRef<Callable_Bool>& i0, const ZRef<Callable_Bool>& i1)
	{ return sCallable_And(i0, i1); }

inline ZRef<Callable_Bool> sOr(const ZRef<Callable_Bool>& i0, const ZRef<Callable_Bool>& i1)
	{ return sCallable_Or(i0, i1); }

inline ZRef<Callable_Bool> sXor(const ZRef<Callable_Bool>& i0, const ZRef<Callable_Bool>& i1)
	{ return sCallable_Xor(i0, i1); }

// =================================================================================================
#pragma mark - Infix notation

inline
ZRef<Callable_Bool> operator~(const ZRef<Callable_Bool>& iCallable)
	{ return sCallable_Not(iCallable); }

inline
ZRef<Callable_Bool> operator&(const ZRef<Callable_Bool>& i0, const ZRef<Callable_Bool>& i1)
	{ return sCallable_And(i0, i1); }

inline
ZRef<Callable_Bool>& operator&=(ZRef<Callable_Bool>& io0, const ZRef<Callable_Bool>& i1)
	{ return io0 = sCallable_And(io0, i1); }

inline
ZRef<Callable_Bool> operator|(const ZRef<Callable_Bool>& i0, const ZRef<Callable_Bool>& i1)
	{ return sCallable_Or(i0, i1); }

inline
ZRef<Callable_Bool>& operator|=(ZRef<Callable_Bool>& io0, const ZRef<Callable_Bool>& i1)
	{ return io0 = sCallable_Or(io0, i1); }

inline
ZRef<Callable_Bool> operator^(const ZRef<Callable_Bool>& i0, const ZRef<Callable_Bool>& i1)
	{ return sCallable_Xor(i0, i1); }

inline
ZRef<Callable_Bool>& operator^=(ZRef<Callable_Bool>& io0, const ZRef<Callable_Bool>& i1)
	{ return io0 = sCallable_Xor(io0, i1); }

} // namespace ZooLib

#endif // __ZooLib_Callable_Bool_h__
