// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Delay_h__
#define __ZooLib_Callable_Delay_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Callable_DelayFor

template <class R>
class Callable_DelayFor
:	public Callable<R()>
	{
public:
	Callable_DelayFor(double iInterval, const ZP<Callable<R()>>& iCallable)
	:	fInterval(iInterval)
	,	fCallable(iCallable)
		{}

// From Callable
	virtual QRet<R> QCall()
		{
		const double systemTime = Time::sSystem() + fInterval;
		for (;;)
			{
			const double delta = systemTime - Time::sSystem();
			if (delta <= 0)
				return sQCall(fCallable);
			ZThread::sSleep(delta);
			}
		}

private:
	const double fInterval;
	const ZP<Callable<R()>> fCallable;
	};

template <class R>
ZP<Callable<R()>>
sCallable_DelayFor(double iInterval, const ZP<Callable<R()>>& iCallable)
	{ return new Callable_DelayFor<R>(iInterval, iCallable); }

// =================================================================================================
#pragma mark - Callable_DelayUntil

template <class R>
class Callable_DelayUntil
:	public Callable<R()>
	{
public:
	Callable_DelayUntil(double iSystemTime, const ZP<Callable<R()>>& iCallable)
	:	fSystemTime(iSystemTime)
	,	fCallable(iCallable)
		{}

// From Callable
	virtual QRet<R> QCall()
		{
		for (;;)
			{
			const double delta = fSystemTime - Time::sSystem();
			if (delta <= 0)
				return sQCall(fCallable);
			ZThread::sSleep(delta);
			}
		}

private:
	const double fSystemTime;
	const ZP<Callable<R()>> fCallable;
	};

template <class R>
ZP<Callable<R()>>
sCallable_DelayUntil(double iSystemTime, const ZP<Callable<R()>>& iCallable)
	{ return new Callable_DelayUntil<R>(iSystemTime, iCallable); }

} // namespace ZooLib

#endif // __ZooLib_Callable_Delay_h__
