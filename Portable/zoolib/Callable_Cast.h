/* -------------------------------------------------------------------------------------------------
Copyright (c) 2017 Andrew Green
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
	Callable_Cast(const ZRef<Callable<R1(A_p...)> >& iCallable)
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
	const ZRef<Callable<R1(A_p...)> > fCallable;
	};

template <class R0, class R1, typename... A_p>
ZRef<Callable<R0(A_p...)>> sCallable_Cast(const ZRef<Callable<R1(A_p...)>>& iCallable)
	{
	if (iCallable)
		return new Callable_Cast<R0,R1,A_p...>(iCallable);
	return null;
	}

} // namespace ZooLib

#endif // ZCONFIG_CPP >= 2011

#endif // __ZooLib_Callable_Cast_h__
