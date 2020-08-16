// Copyright (c) 2017 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Cast_h__
#define __ZooLib_Callable_Cast_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#if ZCONFIG_CPP >= 2011

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable_Cast

template <class R0, class R1, typename... A_p>
class Callable_Cast
:	public Callable<R0(A_p...)>
	{
public:
	Callable_Cast(const ZP<Callable<R1(A_p...)>>& iCallable)
	:	fCallable(iCallable)
		{}

// From Callable
	virtual QRet<R0> QCall(A_p... iParams)
		{
		if (QRet<R1> theQ = sQCall(fCallable, iParams...))
			return static_cast<QRet<R0>>(theQ);
		return QRet<R0>();
		}

private:
	const ZP<Callable<R1(A_p...)>> fCallable;
	};

template <class R0, class R1, typename... A_p>
ZP<Callable<R0(A_p...)>> sCallable_Cast(const ZP<Callable<R1(A_p...)>>& iCallable)
	{
	if (iCallable)
		return new Callable_Cast<R0,R1,A_p...>(iCallable);
	return null;
	}

} // namespace ZooLib

#endif // ZCONFIG_CPP >= 2011

#endif // __ZooLib_Callable_Cast_h__
