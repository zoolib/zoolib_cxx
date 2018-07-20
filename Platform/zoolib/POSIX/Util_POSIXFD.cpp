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

#include "zoolib/POSIX/Util_POSIXFD.h"

#include "zoolib/ZDebug.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include "zoolib/Compat_cmath.h" // For fmod
#include "zoolib/Compat_algorithm.h" // For sMin

// See comment in Solaris' /usr/include/sys/ioctl.h
#if __sun__
	#define BSD_COMP
#endif

#include <sys/ioctl.h>
#include <sys/select.h>

// AG 2005-01-04. It looks like poll.h is not always present on MacOS X.
// We don't use poll on MacOS, so we can just skip the include for now.
#ifndef __APPLE__
	#define POLL_NO_WARN // Switch off Apple warning re poll().
	#include <poll.h>
#endif

namespace ZooLib {
namespace Util_POSIXFD {

void sSetup(fd_set& oSet, int iFD)
	{
	ZAssert(iFD > 0);
	// For (marginally) improved efficiency we could zero only those bits <= iFD.
	FD_ZERO(&oSet);
	FD_SET(iFD, &oSet);
	}

// =================================================================================================
#pragma mark -
#pragma mark Util_POSIXFD

#ifdef __APPLE__

bool sWaitReadable(int iFD, double iTimeout)
	{
	fd_set readSet, exceptSet;
	sSetup(readSet, iFD);
	sSetup(exceptSet, iFD);

	struct timeval timeout;
	timeout.tv_sec = int(iTimeout);
	timeout.tv_usec = int(fmod(iTimeout, 1.0) * 1e6);

	return 0 < ::select(iFD + 1, &readSet, nullptr, &exceptSet, &timeout);
	}

bool sWaitWriteable(int iFD, double iTimeout)
	{
	fd_set writeSet;
	sSetup(writeSet, iFD);

	struct timeval timeout;
	timeout.tv_sec = int(iTimeout);
	timeout.tv_usec = int(fmod(iTimeout, 1.0) * 1e6);

	return 0 < ::select(iFD + 1, nullptr, &writeSet, nullptr, &timeout);
	}

#elif defined(linux) || defined(__linux__) || defined(__sun__)

bool sWaitReadable(int iFD, double iTimeout)
	{
	pollfd thePollFD;
	thePollFD.fd = iFD;
	thePollFD.events = POLLIN | POLLPRI;
	thePollFD.revents = 0;
	return 0 < ::poll(&thePollFD, 1, 1000 * sMin<int>(3600, iTimeout));
	}

bool sWaitWriteable(int iFD, double iTimeout)
	{
	pollfd thePollFD;
	thePollFD.fd = iFD;
	thePollFD.events = POLLOUT;
	return 0 < ::poll(&thePollFD, 1, 1000 * sMin<int>(3600, iTimeout));
	}

#endif

size_t sCountReadable(int iFD)
	{
	int localResult;
	if (0 <= ::ioctl(iFD, FIONREAD, &localResult))
		return localResult;
	return 0;
	}

} // namespace Util_POSIXFD
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)
