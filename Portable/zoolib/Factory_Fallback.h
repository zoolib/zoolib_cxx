/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZooLib_Factory_Fallback_h__
#define __ZooLib_Factory_Fallback_h__ 1
#include "zconfig.h"

#include "zoolib/Factory.h"
#include "zoolib/ZMACRO_foreach.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - Factory_Fallback

template <class T>
class Factory_Fallback
:	public Factory<T>
	{
public:
	typedef T Result_t;

	Factory_Fallback()
		{}

	Factory_Fallback(const ZRef<Factory_Fallback<T>>* iFactories, size_t iCount)
	:	fFactories(iFactories, iFactories + iCount)
		{}

	virtual ~Factory_Fallback()
		{}

// From Callable
	virtual ZQ<T> QCall()
		{
		foreachi (ii, fFactories)
			{
			if (ZQ<T> theQ = sQCall(*ii))
				return theQ;
			}
		return null;
		}

// Our protocol
	void Add(ZRef<Factory_Fallback<T>> iFactory)
		{ fFactories.push_back(iFactory); }

	void Add(const ZRef<Factory_Fallback<T>>* iFactories, size_t iCount)
		{
		if (iFactories && iCount)
			fFactories.insert(fFactories.end(), iFactories, iFactories + iCount);
		}

protected:
	std::vector<ZRef<Factory_Fallback<T>> > fFactories;
	};

} // namespace ZooLib

#endif // __ZooLib_Factory_Fallback_h__
