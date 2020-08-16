// Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/Chan_XX_RateCapped.h"

#include "zoolib/Time.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - RateLimiter

RateLimiter::RateLimiter(double iRate, size_t iQuantum)
:	fRate(iRate),
	fQuantum(iQuantum),
	fLastTime(0)
	{}

size_t RateLimiter::GetCount(size_t iLastCount, size_t iCount)
	{
	double now = Time::sSystem();
	if (now <= fLastTime)
		return 0;

	const double lastConsumed = fRate * (now - fLastTime);
	if (iLastCount > lastConsumed)
		{
		const double remaining = double(iLastCount) - lastConsumed;
		ZThread::sSleep(remaining / fRate);
		}

	fLastTime = Time::sSystem();
	return std::min(iCount, fQuantum);
	}

} // namespace ZooLib
