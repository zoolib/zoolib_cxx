// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

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
