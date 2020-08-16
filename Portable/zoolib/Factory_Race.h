// Copyright (c) 2016 Andrew Green. MIT License. http://www.zoolib.org

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
#pragma mark - Factory_Race

template <class T>
class Factory_Race
:	public Factory<T>
	{
public:
	typedef T Result_t;

	Factory_Race()
		{}

	Factory_Race(double iTimeout)
	:	fTimeoutQ(iTimeout)
		{}

// From Callable
	virtual QRet<T> QCall()
		{
		if (not fTimeoutQ && fFactories.size() == 1)
			{
			return sQCall(fFactories.back());
			}
		else if (fFactories.size())
			{
			ZP<Promise<T >> thePromise = sPromise<T>();
			foreacha (factory, fFactories)
				{
				sStartOnNewThread(
					sBindR(sCallable(&Factory_Race::spCallAndQDeliver),
						factory, thePromise));
				if (thePromise->IsDelivered())
					break;
				}

			ZP<Delivery<T>> theDelivery = sGetDeliveryClearPromise(thePromise);

			if (not fTimeoutQ || theDelivery->WaitFor(*fTimeoutQ))
				{
				if (ZQ<T> theQ = theDelivery->QGet())
					return *theQ;
				}
			}
		return QRet<T>();
		}

// Our protocol
	void Add(const ZP<Factory<T>>& iFactory)
		{
		fFactories.push_back(iFactory);
		}

private:
	static void spCallAndQDeliver(const ZP<Factory<T>>& iFactory, const ZP<Promise<T>>& iPromise)
		{
		if (ZQ<T> theT = sQCall(iFactory))
			{
			// QDeliver it, so the value that's seen in the promise is the *first*.
			iPromise->QDeliver(*theT);
			}
		}

	std::vector<ZP<Factory<T>>> fFactories;
	ZQ<double> fTimeoutQ;
	};

} // namespace ZooLib

#endif // __ZooLib_Factory_Race_h__
