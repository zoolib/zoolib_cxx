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

#ifndef __ZooLib_Callable_Indirect_h__
#define __ZooLib_Callable_Indirect_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Safe.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable_Indirect

template <class Signature> class Callable_Indirect;

// =================================================================================================
#pragma mark - Callable_Indirect (specialization for 0 params)

template <class R>
class Callable_Indirect<R(void)>
:	public Callable<R(void)>
	{
public:
	typedef Callable<R(void)> Callable_t;

	Callable_Indirect(const ZRef<Callable_t>& iCallable)
	:	fSafeCallable(iCallable)
		{}

// From Callable
	virtual QRet<R> QCall()
		{ return sQCall(fSafeCallable.Get()); }

// Our protocol
	ZRef<Callable_t> Get()
		{ return fSafeCallable.Get(); }

	void Set(const ZRef<Callable_t>& iCallable)
		{ fSafeCallable.Set(iCallable); }

	bool CAS(const ZRef<Callable_t>& iPrior, const ZRef<Callable_t>& iNew)
		{ return fSafeCallable.CAS(iPrior, iNew); }

private:
	Safe<ZRef<Callable_t> > fSafeCallable;
	};

// =================================================================================================
#pragma mark - Callable_Indirect variants

#define ZMACRO_Callable_Callable(X) \
\
template <class R, ZMACRO_Callable_Class_P##X> \
class Callable_Indirect<R(ZMACRO_Callable_P##X)> \
:	public Callable<R(ZMACRO_Callable_P##X)> \
	{ \
public: \
	typedef Callable<R(ZMACRO_Callable_P##X)> Callable_t; \
\
	Callable_Indirect(const ZRef<Callable_t>& iCallable) \
	:	fSafeCallable(iCallable) \
		{} \
\
	virtual QRet<R> QCall(ZMACRO_Callable_Pi##X) \
		{ return sQCall(fSafeCallable.Get(), ZMACRO_Callable_i##X); } \
\
	ZRef<Callable_t> Get() \
		{ return fSafeCallable.Get(); } \
\
	void Set(const ZRef<Callable_t>& iCallable) \
		{ fSafeCallable.Set(iCallable); } \
\
	bool CAS(const ZRef<Callable_t>& iPrior, const ZRef<Callable_t>& iNew) \
		{ return fSafeCallable.CAS(iPrior, iNew); } \
\
private: \
	Safe<ZRef<Callable_t> > fSafeCallable; \
	};

ZMACRO_Callable_Callable(0)
ZMACRO_Callable_Callable(1)
ZMACRO_Callable_Callable(2)
ZMACRO_Callable_Callable(3)
ZMACRO_Callable_Callable(4)
ZMACRO_Callable_Callable(5)
ZMACRO_Callable_Callable(6)
ZMACRO_Callable_Callable(7)
ZMACRO_Callable_Callable(8)
ZMACRO_Callable_Callable(9)
ZMACRO_Callable_Callable(A)
ZMACRO_Callable_Callable(B)
ZMACRO_Callable_Callable(C)
ZMACRO_Callable_Callable(D)
ZMACRO_Callable_Callable(E)
ZMACRO_Callable_Callable(F)

#undef ZMACRO_Callable_Callable

// =================================================================================================
#pragma mark - sCallable_Indirect

template <class Sig>
ZRef<Callable_Indirect<Sig> > sCallable_Indirect(const ZRef<Callable<Sig> >& iCallable)
	{ return new Callable_Indirect<Sig>(iCallable); }

template <class Sig>
ZRef<Callable_Indirect<Sig> > sCallable_Indirect()
	{ return new Callable_Indirect<Sig>(null); }

} // namespace ZooLib

#endif // __ZooLib_Callable_Indirect_h__
