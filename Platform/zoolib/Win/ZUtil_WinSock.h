// Copyright (c) 2013 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZUtil_WinSock_h__
#define __ZUtil_WinSock_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/size_t.h" // For size_t

#include "zoolib/ZCompat_WinSock.h"

namespace ZooLib {
namespace ZUtil_WinSock {

// =================================================================================================
#pragma mark - ZUtil_WinSock

bool sWaitReadable(SOCKET iSOCKET, double iTimeout);
void sWaitWriteable(SOCKET iSOCKET);
size_t sCountReadable(SOCKET iSOCKET);

} // namespace ZUtil_WinSock
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZUtil_WinSock_h__
