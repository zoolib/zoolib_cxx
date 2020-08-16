// Copyright (c) 2013 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZUtil_WinSock.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_cmath.h" // For fmod

namespace ZooLib {
namespace ZUtil_WinSock {

// =================================================================================================
#pragma mark - ZUtil_WinSock

bool sWaitReadable(SOCKET iSOCKET, double iTimeout)
	{
	fd_set readSet, exceptSet;
	FD_ZERO(&readSet);
	FD_ZERO(&exceptSet);
	FD_SET(iSOCKET, &readSet);
	FD_SET(iSOCKET, &exceptSet);

	struct timeval timeOut;
	timeOut.tv_sec = int(iTimeout);
	timeOut.tv_usec = int(fmod(iTimeout, 1.0) * 1e6);
	return 0 < ::select(0, &readSet, nullptr, &exceptSet, &timeOut);
	}

//void sWaitWriteable(SOCKET iSOCKET);

size_t sCountReadable(SOCKET iSOCKET)
	{
	// Use non-blocking select to see if a read would succeed
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(iSOCKET, &readSet);
	struct timeval timeOut;
	timeOut.tv_sec = 0;
	timeOut.tv_usec = 0;
	int result = ::select(0, &readSet, nullptr, nullptr, &timeOut);
	if (result <= 0)
		return 0;

	// FIONREAD will give us a result without hanging.
	unsigned long localResult;
	if (::ioctlsocket(iSOCKET, FIONREAD, &localResult) == SOCKET_ERROR)
		return 0;
	return localResult;
	}

} // namespace ZUtil_WinSock
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
