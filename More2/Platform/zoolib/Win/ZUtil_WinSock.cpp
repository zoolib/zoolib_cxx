/* -------------------------------------------------------------------------------------------------
Copyright (c) 2013 Andrew Green
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

#include "zoolib/ZUtil_WinSock.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_cmath.h" // For fmod

namespace ZooLib {
namespace ZUtil_WinSock {

// =================================================================================================
// MARK: - ZUtil_WinSock

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
