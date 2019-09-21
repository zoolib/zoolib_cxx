/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/POSIX/pthread_more.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include "zoolib/Memory.h" // For sMemZero

namespace ZooLib {
namespace POSIX {

// =================================================================================================
#pragma mark - sQGetThreadName

ZQ<std::string> sQGetThreadName()
	{
	#if not defined(__ANDROID_API__)
		char buffer[256];
		if (0 == ::pthread_getname_np(pthread_self(), buffer, countof(buffer)))
			return std::string(buffer);
	#endif
	return null;
	}

// =================================================================================================
#pragma mark - SaveSetRestoreThreadName

SaveSetRestoreThreadName::SaveSetRestoreThreadName(const std::string& iName)
:	fQPrior(sQGetThreadName())
	{
	if (fQPrior)
		ZThread_pthread::sSetName(iName.c_str());
	}

SaveSetRestoreThreadName::SaveSetRestoreThreadName(const char* iName)
:	fQPrior(sQGetThreadName())
	{
	if (fQPrior)
		ZThread_pthread::sSetName(iName);
	}

SaveSetRestoreThreadName::~SaveSetRestoreThreadName()
	{
	if (fQPrior)
		ZThread_pthread::sSetName(fQPrior->c_str());
	}

// =================================================================================================
#pragma mark - SaveSetRestoreThreadPriority

SaveSetRestoreThreadPriority::SaveSetRestoreThreadPriority(int iPriority)
	{
	pthread_getschedparam(pthread_self(), &fPolicyPrior, &fSchedPrior);

	// Can't use = {0} here
	struct sched_param schedNew;
	sMemZero_T(schedNew);

	schedNew.sched_priority = iPriority;
	pthread_setschedparam(pthread_self(), SCHED_RR, &schedNew);
	}

SaveSetRestoreThreadPriority::~SaveSetRestoreThreadPriority()
	{
	pthread_setschedparam(pthread_self(), fPolicyPrior, &fSchedPrior);
	}

// =================================================================================================
#pragma mark - SaveSetRestoreThreadPriority_Max

SaveSetRestoreThreadPriority_Max::SaveSetRestoreThreadPriority_Max()
:	SaveSetRestoreThreadPriority(sched_get_priority_max(SCHED_RR))
	{}

} // namespace POSIX
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)
