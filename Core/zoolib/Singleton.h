/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZooLib_Singleton_h__
#define __ZooLib_Singleton_h__ 1
#include "zconfig.h"

#include "zoolib/Atomic.h"
#include "zoolib/Deleter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sSingleton

template <class Type_p, class Tag_p>
Type_p& sSingleton()
	{
	static std::atomic<Type_p*> spType_p;
	if (not spType_p)
		{
		Type_p* newValue = new Type_p();
		if (not sAtomic_CAS<Type_p*>(&spType_p, nullptr, newValue))
			{
			delete newValue;
			}
		else
			{
			static Deleter<Type_p> deleter(*(Type_p**)&spType_p);
			}
		}
	return *spType_p;
	}

template <class Type_p>
Type_p& sSingleton()
	{ return sSingleton<Type_p,Type_p>(); }

} // namespace ZooLib

#endif // __ZooLib_Singleton_h___
