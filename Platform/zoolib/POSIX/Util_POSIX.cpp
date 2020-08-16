// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/POSIX/Util_POSIX.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include <stdlib.h> // For getenv

namespace ZooLib {
namespace Util_POSIX {

// =================================================================================================
#pragma mark - Util_POSIX

ZQ<std::string> sQGetEnv(const std::string& iName)
	{
	if (const char* theEnv = getenv(iName.c_str()))
		return theEnv;
	return null;
	}

} // namespace Util_POSIX
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)
