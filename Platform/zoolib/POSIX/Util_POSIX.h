// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_POSIX_h__
#define __ZooLib_Util_POSIX_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include <string>

#include "zoolib/ZQ.h"

namespace ZooLib {
namespace Util_POSIX {

// =================================================================================================
#pragma mark - Util_POSIX

ZQ<std::string> sQGetEnv(const std::string& iName);

} // namespace Util_POSIX
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)

#endif // __ZooLib_Util_POSIX_h__
