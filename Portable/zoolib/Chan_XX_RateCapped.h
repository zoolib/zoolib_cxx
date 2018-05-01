/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_Chan_XX_RateCapped_h__
#define __ZooLib_Chan_XX_RateCapped_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark RateLimiter

class RateLimiter
	{
public:
	RateLimiter(double iRate, size_t iQuantum);

	size_t GetCount(size_t iLastCount, size_t iCount);

private:
	double fRate;
	size_t fQuantum;

	double fLastTime;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanR_XX_RateCapped

template <class Chan_p>
class ChanR_XX_RateCapped
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanR_XX_RateCapped(const Chan_p& iChan, double iRate, size_t iQuantum)
	:	inherited(iChan)
	,	fLimiter(iRate, iQuantum)
	,	fLastCount(0)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		size_t realCount = fLimiter.GetCount(fLastCount, iCount);
		fLastCount = sRead(inherited::pGetChan(), oDest, realCount);
		return fLastCount;
		}

protected:
	RateLimiter fLimiter;
	size_t fLastCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_XX_RateCapped

template <class Chan_p>
class ChanW_XX_RateCapped
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanW_XX_RateCapped(const Chan_p& iChan, double iRate, size_t iQuantum)
	:	inherited(iChan)
	,	fLimiter(iRate, iQuantum)
	,	fLastCount(0)
		{}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		size_t realCount = fLimiter.GetCount(fLastCount, iCount);
		fLastCount = sWrite(inherited::pGetChan(), iSource, realCount);
		return fLastCount;
		}

protected:
	RateLimiter fLimiter;
	size_t fLastCount;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_RateCapped_h__
