// Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_XX_RateCapped_h__
#define __ZooLib_Chan_XX_RateCapped_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - RateLimiter

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
#pragma mark - ChanR_XX_RateCapped

template <class Chan_p>
class ChanR_XX_RateCapped
:	public virtual ChanFilter<Chan_p>
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
#pragma mark - ChanW_XX_RateCapped

template <class Chan_p>
class ChanW_XX_RateCapped
:	public virtual ChanFilter<Chan_p>
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
