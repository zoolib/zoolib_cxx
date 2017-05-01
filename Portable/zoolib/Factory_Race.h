/* -------------------------------------------------------------------------------------------------
Copyright (c) 2016 Andrew Green
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

#ifndef __ZooLib_Factory_Race_h__
#define __ZooLib_Factory_Race_h__ 1
#include "zconfig.h"

#include "zoolib/Callable_Function.h"
#include "zoolib/Factory.h"
#include "zoolib/Promise.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/Log.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Factory_Race

template <class T>
class Factory_Race
:	public Factory<T>
	{
public:
	typedef T Result_t;

	Factory_Race()
		{}

// From Callable
	virtual ZQ<T> QCall()
		{
		if (fFactories.size() == 1)
			{
			return sCall(fFactories.back());
			}
		else if (fFactories.size() > 1)
			{
			ZRef<Promise<T > > thePromise = sPromise<T>();
			foreacha (factory, fFactories)
				{
				sStartOnNewThread(
					sBindR(sCallable(&Factory_Race::spTryIt),
						factory, thePromise));
				}

			ZRef<Delivery<T> > theDelivery = sGetDeliveryClearPromise(thePromise);

			if (theDelivery->WaitFor(30))
				{
				if (ZQ<T> theQ = theDelivery->QGet())
					return *theQ;
				}
			}

		return null;
		}

// Our protocol
	void Add(const ZRef<Factory<T> >& iFactory)
		{
		fFactories.push_back(iFactory);
		}

private:
	static void spTryIt(const ZRef<Factory<T>>& theFactory, const ZRef<Promise<T> >& iPromise)
		{
		ZLOGTRACE(eDebug);

		if (ZQ<T> theT = sQCall(theFactory))
			{
			if (not iPromise->QDeliver(*theT))
				{
				ZLOGTRACE(eDebug);
				}
			else
				{
				ZLOGTRACE(eDebug);
				}
			}
		}

	std::vector<ZRef<Factory<T> > > fFactories;
	};

} // namespace ZooLib

#endif // __ZooLib_Factory_Race_h__
