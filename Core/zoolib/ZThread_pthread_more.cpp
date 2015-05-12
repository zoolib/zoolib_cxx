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

#include "zoolib/ZThread_pthread_more.h"

#if ZCONFIG_API_Enabled(Thread_pthread)

#include "zoolib/Memory.h" // For sMemZero

namespace ZooLib {
namespace ZThread_pthread {

// =================================================================================================
#pragma mark -
#pragma mark sQGetName

ZQ<std::string> sQGetName()
	{
	#if not defined(__ANDROID_API__)
		char buffer[256];
		if (0 == ::pthread_getname_np(pthread_self(), buffer, countof(buffer)))
			return std::string(buffer);
	#endif
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark SaveSetRestoreName

SaveSetRestoreName::SaveSetRestoreName(const std::string& iName)
:	fQPrior(sQGetName())
	{
	if (fQPrior)
		sSetName(iName.c_str());
	}

SaveSetRestoreName::SaveSetRestoreName(const char* iName)
:	fQPrior(sQGetName())
	{
	if (fQPrior)
		sSetName(iName);
	}

SaveSetRestoreName::~SaveSetRestoreName()
	{
	if (fQPrior)
		sSetName(fQPrior->c_str());
	}

// =================================================================================================
#pragma mark -
#pragma mark SaveSetRestorePriority

SaveSetRestorePriority::SaveSetRestorePriority(int iPriority)
	{
	pthread_getschedparam(pthread_self(), &fPolicyPrior, &fSchedPrior);

	// Can't use = {0} here
	struct sched_param schedNew;
	sMemZero_T(schedNew);

	schedNew.sched_priority = iPriority;
	pthread_setschedparam(pthread_self(), SCHED_RR, &schedNew);
	}

SaveSetRestorePriority::~SaveSetRestorePriority()
	{
	pthread_setschedparam(pthread_self(), fPolicyPrior, &fSchedPrior);
	}

// =================================================================================================
#pragma mark -
#pragma mark SaveSetRestorePriority_Max

SaveSetRestorePriority_Max::SaveSetRestorePriority_Max()
:	SaveSetRestorePriority(sched_get_priority_max(SCHED_RR))
	{}

} // namespace ZThread_pthread
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_pthread)
