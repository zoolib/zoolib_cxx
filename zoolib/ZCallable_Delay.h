/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZCallable_Delay__
#define __ZCallable_Delay__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_DelayFor

template <class R>
class ZCallable_DelayFor
:	public ZCallable<R(void)>
	{
public:
	ZCallable_DelayFor(double iInterval, ZRef<ZCallable<R(void)> > iCallable)
	:	fInterval(iInterval)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call()
		{
		const ZTime systemTime = ZTime::sSystem() + fInterval;
		for (;;)
			{
			const double delta = systemTime - ZTime::sSystem();
			if (delta <= 0)
				return sCall(fCallable);
			ZThread::sSleep(delta);
			}
		}

private:
	const double fInterval;
	const ZRef<ZCallable<R(void)> > fCallable;
	};

template <class R>
ZRef<ZCallable<R(void)> >
sCallable_DelayFor(double iInterval, ZRef<ZCallable<R(void)> > iCallable)
	{ return new ZCallable_DelayFor<R>(iInterval, iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_DelayUntil

template <class R>
class ZCallable_DelayUntil
:	public ZCallable<R(void)>
	{
public:
	ZCallable_DelayUntil(ZTime iSystemTime, ZRef<ZCallable<R(void)> > iCallable)
	:	fSystemTime(iSystemTime)
	,	fCallable(iCallable)
		{}

// From ZCallable
	virtual R Call()
		{
		for (;;)
			{
			const double delta = fSystemTime - ZTime::sSystem();
			if (delta <= 0)
				return sCall(fCallable);
			ZThread::sSleep(delta);
			}
		}

private:
	const ZTime fSystemTime;
	const ZRef<ZCallable<R(void)> > fCallable;
	};

template <class R>
ZRef<ZCallable<R(void)> >
sCallable_DelayUntil(ZTime iSystemTime, ZRef<ZCallable<R(void)> > iCallable)
	{ return new ZCallable_DelayUntil<R>(iSystemTime, iCallable); }

} // namespace ZooLib

#endif // __ZCallable_Delay__
