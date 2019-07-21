// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_POSIX_Compat_fcntl_h__
#define __ZooLib_POSIX_Compat_fcntl_h__ 1
#include "zconfig.h"

#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include <fcntl.h>
#if defined(__linux__)

__asm__(".symver fcntl,fcntl@GLIBC_2.4");

#endif // defined(__linux__)

#endif // ZCONFIG_SPI_Enabled(POSIX)
#endif // __ZooLib_POSIX_Compat_fcntl_h__
