// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_POSIX_pthread_more_h__
#define __ZooLib_POSIX_pthread_more_h__
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include "zoolib/ZThread_pthread.h"
#include "zoolib/ZQ.h"

#include <string>

namespace ZooLib {
namespace POSIX {

// =================================================================================================
#pragma mark - sQGetName

ZQ<std::string> sQGetThreadName();

// =================================================================================================
#pragma mark - SaveSetRestoreThreadName

class SaveSetRestoreThreadName
	{
public:
	SaveSetRestoreThreadName(const std::string& iName);
	SaveSetRestoreThreadName(const char* iName);
	~SaveSetRestoreThreadName();

private:
	const ZQ<std::string> fQPrior;
	};

// =================================================================================================
#pragma mark - SaveSetRestoreThreadPriority

class SaveSetRestoreThreadPriority
	{
public:
	SaveSetRestoreThreadPriority(int iPriority);
	~SaveSetRestoreThreadPriority();

private:
	struct sched_param fSchedPrior;
	int fPolicyPrior;
	};

// =================================================================================================
#pragma mark - SaveSetRestoreThreadPriority_Max

class SaveSetRestoreThreadPriority_Max
:	public SaveSetRestoreThreadPriority
	{
public:
	SaveSetRestoreThreadPriority_Max();
	};

} // namespace POSIX
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)

#endif // __ZooLib_POSIX_pthread_more_h__
