/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZThread_boost.h"

#if ZCONFIG_API_Enabled(Thread_boost)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZCnd_boost

ZCnd_boost::ZCnd_boost()
	{}

ZCnd_boost::~ZCnd_boost()
	{}

void ZCnd_boost::Wait(boost::mutex& iMtx)
	{
	boost::mutex::scoped_lock theLock(iMtx, boost::adopt_lock);
	condition_variable::wait(theLock);
	}

bool ZCnd_boost::Wait(boost::mutex& iMtx, double iTimeout)
	{
	boost::mutex::scoped_lock theLock(iMtx, boost::adopt_lock);

	return condition_variable::timed_wait(
		theLock, boost::posix_time::microseconds(iTimeout * 1e6));
	}

void ZCnd_boost::Signal()
	{ condition_variable::notify_one(); }

void ZCnd_boost::Broadcast()
	{ condition_variable::notify_all(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZMtx_boost

ZMtx_boost::ZMtx_boost(const char* iName)
	{}

ZMtx_boost::~ZMtx_boost()
	{}

void ZMtx_boost::Acquire()
	{ mutex::lock(); }

void ZMtx_boost::Release()
	{ mutex::unlock(); }

//bool IsLocked(); // ??

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Thread_boost)
