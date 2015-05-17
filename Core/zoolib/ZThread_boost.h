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

#ifndef __ZThread_boost_h__
#define __ZThread_boost_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Thread_boost
	#if ZCONFIG_SPI_Enabled(boost)
		#include <boost/version.hpp>
		#if !defined(__MWERKS__)
			#if defined(BOOST_VERSION) && BOOST_VERSION >= 103500
				#define ZCONFIG_API_Avail__Thread_boost 1
			#endif
		#endif
	#endif
#endif

#ifndef ZCONFIG_API_Avail__Thread_boost
	#define ZCONFIG_API_Avail__Thread_boost 0
#endif

#ifndef ZCONFIG_API_Desired__Thread_boost
	#define ZCONFIG_API_Desired__Thread_boost 1
#endif

#if ZCONFIG_API_Enabled(Thread_boost)

#include "zoolib/ZThread_T.h"
#include "zoolib/ZTypes.h"

#include <boost/thread.hpp>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZCnd_boost

class ZCnd_boost : public boost::condition_variable
	{
public:
	ZCnd_boost();
	~ZCnd_boost();

	void Wait(boost::mutex& iMtx);
	bool WaitFor(boost::mutex& iMtx, double iTimeout);
	bool WaitUntil(boost::mutex& iMtx, double iDeadline);

	void Signal();
	void Broadcast();
	};

// =================================================================================================
#pragma mark -
#pragma mark ZMtx_boost

class ZMtx_boost : public boost::mutex
	{
public:
	ZMtx_boost();
	~ZMtx_boost();

	void Acquire();
	void Release();
	};

// =================================================================================================
#pragma mark -
#pragma mark ZSem_boost

typedef ZSem_T<ZMtx_boost, ZCnd_boost> ZSem_boost;

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_boost)

#endif // __ZThread_boost_h__
