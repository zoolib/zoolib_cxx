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

// Enable 64 bit variants of system calls on Linux. We don't go the whole hog and
// use _FILE_OFFSET_BITS to do renaming of entry points and data types because I'd
// rather be explicit about which functions we're calling.
#ifndef _LARGEFILE64_SOURCE
	#define _LARGEFILE64_SOURCE
#endif

#include "zoolib/POSIX/Util_POSIXFD.h"
#include "zoolib/POSIX/Compat_fcntl.h"

#include "zoolib/ZDebug.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include "zoolib/ChanAspect.h" // For sClamped
#include "zoolib/Compat_cmath.h" // For fmod
#include "zoolib/Compat_algorithm.h" // For sMin

// See comment in Solaris' /usr/include/sys/ioctl.h
#if __sun__
	#define BSD_COMP
#endif

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// AG 2005-01-04. It looks like poll.h is not always present on MacOS X.
// We don't use poll on MacOS, so we can just skip the include for now.
#ifndef __APPLE__
	#define POLL_NO_WARN // Switch off Apple warning re poll().
	#include <poll.h>
#endif

namespace ZooLib {
namespace Util_POSIXFD {

// =================================================================================================
#pragma mark - Util_POSIXFD

void sSetNonBlocking(int iFD)
	{
	::fcntl(iFD, F_SETFL, ::fcntl(iFD, F_GETFL, 0) | O_NONBLOCK);
	}

void sSetup(fd_set& oSet, int iFD)
	{
	ZAssert(iFD > 0);
	// For (marginally) improved efficiency we could zero only those bits <= iFD.
	FD_ZERO(&oSet);
	FD_SET(iFD, &oSet);
	}

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

void sAbort(int iFD)
	{
	if (iFD >= 0)
		{
		// Open /dev/null
		int nullFD = ::open("/dev/null", O_RDONLY);

		// Atomically move the /dev/null control block so that iFD refers to
		// it, and closes the control block iFD had referred to. Anything
		// using iFD from now on will get EOF behavior, and will just unwind
		// and ultimately close() it.
		::dup2(nullFD, iFD);

		// Close our FD referencing the control block.
		::close(nullFD);
		}
	}

bool spDisconnectRead(int iFD, double iTimeout);

void spDisconnectWrite(int iFD);

uint64 sPos(int iFD)
	{
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		return ::lseek64(iFD, 0, SEEK_CUR);
	#else
		return ::lseek(iFD, 0, SEEK_CUR);
	#endif
	}

void sPosSet(int iFD, uint64 iPos)
	{
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		::lseek64(iFD, iPos, SEEK_SET);
	#else
		::lseek(iFD, iPos, SEEK_SET);
	#endif
	}

size_t sRead(int iFD, byte* oDest, size_t iCount)
	{
	byte* localDest = oDest;
	while (iCount)
		{
		ssize_t countRead = ::read(iFD, localDest, iCount);

		if (countRead == 0)
			break;

		if (countRead < 0)
			{
			int err = errno;
			if (err == EINTR)
				continue;
			break;
			}
		iCount -= countRead;
		localDest += countRead;
		}
	return localDest - oDest;
	}

size_t sReadCon(int iFD, byte* oDest, size_t iCount)
	{
	byte* localDest = oDest;
	while (iCount)
		{
		ssize_t result = ::read(iFD, localDest, iCount);

		if (result < 0)
			{
			int err = errno;
			if (err == EAGAIN)
				{
				sWaitReadable(iFD, 10);
				}
			else if (err != EINTR)
				{
				break;
				}
			}
		else
			{
			if (result == 0)
				{
				// result is zero, indicating that the other end has sent FIN.
				break;
				}
			else
				{
				localDest += result;
				iCount -= result;
				break;
				}
			}
		}
	return localDest - oDest;
	}

size_t sReadable(int iFD)
	{
	int localResult;
	if (0 <= ::ioctl(iFD, FIONREAD, &localResult))
		return localResult;
	return 0;
	}

uint64 sSize(int iFD)
	{
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		struct stat64 theStatStruct;
		if (::fstat64(iFD, &theStatStruct))
			return 0;
	#else
		struct stat theStatStruct;
		if (::fstat(iFD, &theStatStruct))
			return 0;
	#endif

	return theStatStruct.st_size;
	}

void sSizeSet(int iFD, uint64 iSize)
	{
	// NB ftruncate is not supported on all systems
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		::ftruncate64(iFD, iSize);
	#else
		::ftruncate(iFD, iSize);
	#endif
	}

size_t sUnread(int iFD, const byte* iSource, size_t iCount)
	{
	const uint64 pos = sPos(iFD);
	if (pos <= iCount)
		{
		sPosSet(iFD, 0);
		return sClamped(pos);
		}
	sPosSet(iFD, pos - iCount);
	return iCount;
	}

size_t sUnreadableLimit(int iFD)
	{
	// Unreadable limit is whatever our position is.
	return sClamped(sPos(iFD));
	}

size_t sWrite(int iFD, const byte* iSource, size_t iCount)
	{
	const byte* localSource = iSource;
	while (iCount)
		{
		ssize_t countWritten = ::write(iFD, localSource, iCount);
		if (countWritten < 0)
			{
			int err = errno;
			if (err == EINTR)
				continue;
			break;
			}
		iCount -= countWritten;
		localSource += countWritten;
		}
	return localSource - iSource;
	}

size_t sWriteCon(int iFD, const byte* iSource, size_t iCount)
	{
	const byte* localSource = iSource;
	while (iCount)
		{
		ssize_t result = ::write(iFD, localSource, iCount);

		if (result < 0)
			{
			int err = errno;
			if (err == EAGAIN)
				{
				sWaitWriteable(iFD, 10);
				}
			else if (err != EINTR)
				{
				break;
				}
			}
		else
			{
			if (result == 0)
				{
				// result is zero, indicating that the other end has closed its receive side.
				break;
				}
			else
				{
				localSource += result;
				iCount -= result;
				}
			}
		}
	return localSource - iSource;
	}

} // namespace Util_POSIXFD
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)
