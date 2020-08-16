// Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Factory_Retry_h__
#define __ZooLib_Factory_Retry_h__ 1
#include "zconfig.h"

#include "zoolib/Factory.h"
#include "zoolib/Log.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Factory_Retry

template <class T>
class Factory_Retry
:	public Factory<T>
	{
public:
	typedef T Result_t;

	Factory_Retry(const ZP<Factory<T>>& iFactory, size_t iCount, double iInterval)
	:	fFactory(iFactory)
	,	fCount(iCount)
	,	fInterval(iInterval)
		{}

	virtual ~Factory_Retry()
		{}

// From Callable
	virtual ZQ<T> QCall()
		{
		for (size_t attempt = 1; /*no test*/; ++attempt)
			{
			ZP<Factory<T>> theFactory = fFactory;
			if (not theFactory)
				{
				if (ZLOG(w, eInfo, "Factory_Retry"))
					sEWritef(w, "QCall, no factory");
				break;
				}

			if (ZLOG(s, eDebug, "Factory_Retry"))
				sWritef(s, "QCall, attempt %zu of %zu", attempt, fCount);

			if (ZQ<T> theQ = theFactory->Call())
				{
				if (ZLOG(w, eInfo, "Factory_Retry"))
					w << "QCall succeeded";

				return theQ;
				}

			if (not fCount || attempt < fCount)
				{
				if (ZLOG(w, eDebug, "Factory_Retry"))
					sEWritef(w, "QCall failed, sleeping for %g seconds", fInterval);

				ZThread::sSleep(fInterval);
				}
			else
				{
				if (ZLOG(w, eInfo, "Factory_Retry"))
					sEWritef(w, "QCall failed after %zu attempts", fCount);
				break;
				}
			}

		return null;
		}

protected:
	ZP<Factory<T>> fFactory;
	size_t fCount;
	double fInterval;
	};

} // namespace ZooLib

#endif // __ZooLib_Factory_Retry_h__
