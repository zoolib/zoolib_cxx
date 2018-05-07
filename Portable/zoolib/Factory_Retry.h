/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_Factory_Retry_h__
#define __ZooLib_Factory_Retry_h__ 1
#include "zconfig.h"

#include "zoolib/Factory.h"
#include "zoolib/Log.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Factory_Retry

template <class T>
class Factory_Retry
:	public Factory<T>
	{
public:
	typedef T Result_t;

	Factory_Retry(const ZRef<Factory<T>>& iFactory, size_t iCount, double iInterval)
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
			ZRef<Factory<T>> theFactory = fFactory;
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
	ZRef<Factory<T>> fFactory;
	size_t fCount;
	double fInterval;
	};

} // namespace ZooLib

#endif // __ZooLib_Factory_Retry_h__
