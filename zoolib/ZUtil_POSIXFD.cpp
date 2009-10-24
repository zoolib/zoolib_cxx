/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZUtil_POSIXFD.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include "zoolib/ZCompat_cmath.h" // For fmod

// See comment in Solaris' /usr/include/sys/ioctl.h
#if __sun__
#	define BSD_COMP
#endif

#include <sys/ioctl.h>
#include <sys/select.h>

// AG 2005-01-04. It looks like poll.h is not always present on MacOS X.
// We don't use poll on MacOS, so we can just skip the include for now.
#ifndef __APPLE__
#	define POLL_NO_WARN // Switch off Apple warning re poll().
#	include <poll.h>
#endif

NAMESPACE_ZOOLIB_BEGIN
namespace ZUtil_POSIXFD {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_POSIXFD

#ifdef __APPLE__

bool sWaitReadable(int iFD, double iTimeout)
	{
	fd_set readSet, exceptSet;
	FD_ZERO(&readSet);
	FD_ZERO(&exceptSet);
	FD_SET(iFD, &readSet);
	FD_SET(iFD, &exceptSet);

	struct timeval timeOut;
	timeOut.tv_sec = int(iTimeout);
	timeOut.tv_usec = int(fmod(iTimeout, 1.0) * 1e6);
	return 0 < ::select(iFD + 1, &readSet, nullptr, &exceptSet, &timeOut);
	}

void sWaitWriteable(int iFD)
	{
	fd_set writeSet;
	FD_ZERO(&writeSet);
	FD_SET(iFD, &writeSet);

	struct timeval timeOut;
	timeOut.tv_sec = 1;
	timeOut.tv_usec = 0;

	::select(iFD + 1, nullptr, &writeSet, nullptr, &timeOut);
	}

#elif defined(linux) || defined(__sun__)

bool sWaitReadable(int iFD, double iTimeout)
	{
	pollfd thePollFD;
	thePollFD.fd = iFD;
	thePollFD.events = POLLIN | POLLPRI;
	thePollFD.revents = 0;
	return 0 < ::poll(&thePollFD, 1, 1000 * int(min(3600.0, iTimeout));
	}

void sWaitWriteable(int iFD)
	{
	pollfd thePollFD;
	thePollFD.fd = iFD;
	thePollFD.events = POLLOUT;
	::poll(&thePollFD, 1, 1000);
	}

#endif

size_t sCountReadable(int iFD)
	{
	int localResult;
	if (0 <= ::ioctl(iFD, FIONREAD, &localResult))
		return localResult;
	return 0;
	}

} // namespace ZUtil_POSIXFD
NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(POSIX)
