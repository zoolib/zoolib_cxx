// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Const_h__
#define __ZooLib_Callable_Const_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable_Const

template <class R>
class Callable_Const
:	public Callable<R()>
	{
public:
	Callable_Const(typename CallableUtil::VT<R>::Param iR)
	:	fR(iR)
		{}

// From Callable
	virtual QRet<R> QCall()
		{ return fR; }

private:
	typename CallableUtil::VT<R>::Field fR;
	};

// =================================================================================================
#pragma mark - sCallable_Const

template <class R>
ZP<Callable<R()>> sCallable_Const(R iR)
	{ return new Callable_Const<R>(iR); }

} // namespace ZooLib

#endif // __ZooLib_Callable_Const_h__
