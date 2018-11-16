/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/POSIX/Net_Socket.h"

#if ZCONFIG_API_Enabled(Net_Socket)

#include "zoolib/POSIX/Util_POSIXFD.h"

#include "zoolib/Memory.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <sys/socket.h>

using std::string;

#if not defined(SOCK_MAXADDRLEN)
	#define SOCK_MAXADDRLEN (255)
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - Net_Socket

// On MacOS X (and FreeBSD) and Linux a send or receive on a socket where the other end
// has closed can cause delivery of a sigpipe. These helper functions, and the conditional
// code in the NetEndpoint_Socket constructor, work around that issue.

#ifdef __APPLE__
// For MacOS X we set an option on the socket to indicate that sigpipe should not
// be delivered when the far end closes on us. That option was not defined in headers
// prior to 10.2, so we define it ourselves if necessary.

#ifndef SO_NOSIGPIPE
	#define SO_NOSIGPIPE 0x1022
#endif

static void spSetSocketOptions(int iSocket)
	{
	// Set the socket to be non blocking
	::fcntl(iSocket, F_SETFL, ::fcntl(iSocket, F_GETFL,0) | O_NONBLOCK);

	// Enable keep alive
	::setsockopt(iSocket, SOL_SOCKET, SO_KEEPALIVE,
		(const char*)(const int*)sConstPtr(int(1)), sizeof(int));

	// Disable sigpipe when writing/reading a far-closed socket.
	::setsockopt(iSocket, SOL_SOCKET, SO_NOSIGPIPE,
		(const char*)(const int*)sConstPtr(int(1)), sizeof(int));
	}

ssize_t Net_Socket::sSend(int iSocket, const char* iSource, size_t iCount)
	{ return ::send(iSocket, iSource, iCount, 0); }

ssize_t Net_Socket::sReceive(int iSocket, char* oDest, size_t iCount)
	{ return ::recv(iSocket, oDest, iCount, 0); }

#elif defined(linux) || defined(__linux__) || defined(__sun__)

// RedHat Linux 6.1 adds the sigpipe-on-closed behavior to recv and send, and also adds
// a new flag that can be passed to supress that behavior. Older kernels return EINVAL
// if they don't recognize the flag, and thus don't have the behavior. So we use a static
// bool to remember if MSG_NOSIGNAL is available in the kernel.

static void spSetSocketOptions(int iSocket)
	{
	// Set the socket to be non blocking
	::fcntl(iSocket, F_SETFL, ::fcntl(iSocket, F_GETFL, 0) | O_NONBLOCK);

	// Enable keep alive
	::setsockopt(iSocket, SOL_SOCKET, SO_KEEPALIVE,
		(const char*)(const int*)sConstPtr(int(1)), sizeof(int));
	}

#ifndef MSG_NOSIGNAL
	#define MSG_NOSIGNAL 0x2000
#endif

static bool spCanUse_MSG_NOSIGNAL = false;
static bool spChecked_MSG_NOSIGNAL = false;

int Net_Socket::sSend(int iSocket, const char* iSource, size_t iCount)
	{
	if (spCanUse_MSG_NOSIGNAL)
		{
		return ::send(iSocket, iSource, iCount, MSG_NOSIGNAL);
		}
	else if (spChecked_MSG_NOSIGNAL)
		{
		return ::send(iSocket, iSource, iCount, 0);
		}
	else
		{
		int result = ::send(iSocket, iSource, iCount, MSG_NOSIGNAL);
		if (result >= 0)
			{
			spCanUse_MSG_NOSIGNAL = true;
			spChecked_MSG_NOSIGNAL = true;
			}
		else if (errno == EINVAL)
			{
			spChecked_MSG_NOSIGNAL = true;
			return ::send(iSocket, iSource, iCount, 0);
			}
		return result;
		}
	}

int Net_Socket::sReceive(int iSocket, char* oDest, size_t iCount)
	{
	if (spCanUse_MSG_NOSIGNAL)
		{
		return ::recv(iSocket, oDest, iCount, MSG_NOSIGNAL);
		}
	else if (spChecked_MSG_NOSIGNAL)
		{
		return ::recv(iSocket, oDest, iCount, 0);
		}
	else
		{
		int result = ::recv(iSocket, oDest, iCount, MSG_NOSIGNAL);

		if (result >= 0)
			{
			spCanUse_MSG_NOSIGNAL = true;
			spChecked_MSG_NOSIGNAL = true;
			}
		else if (errno == EINVAL)
			{
			spChecked_MSG_NOSIGNAL = true;
			return ::recv(iSocket, oDest, iCount, 0);
			}
		return result;
		}
	}

#endif

Net::Error Net_Socket::sTranslateError(int iNativeError)
	{
	Net::Error theError = Net::errorGeneric;
	switch (iNativeError)
		{
		case EADDRINUSE: theError = Net::errorLocalPortInUse; break;
		case EINVAL: theError = Net::errorBadState; break;
		case EBADF: theError = Net::errorBadState; break;
		case ECONNREFUSED: theError = Net::errorCouldntConnect; break;
		case 0: theError = Net::errorNone; break;
		default: break;
		}
	return theError;
	}

// =================================================================================================
#pragma mark - NetListener_Socket

static bool spFastCancellationEnabled;

NetListener_Socket::NetListener_Socket(int iSocketFD)
:	fSocketFD(iSocketFD)
	{
	if (::listen(fSocketFD, 5) < 0)
		{
		int err = errno;
		::close(fSocketFD);
		throw NetEx(Net_Socket::sTranslateError(err));
		}

	::fcntl(fSocketFD, F_SETFL, ::fcntl(fSocketFD, F_GETFL,0) | O_NONBLOCK);

	fThreadID_Listening = 0;
	}

NetListener_Socket::~NetListener_Socket()
	{ ::close(fSocketFD); }

ZRef<ChannerRWClose_Bin> NetListener_Socket::Listen()
	{
	fMtx.Acquire();
	while (fThreadID_Listening)
		fCnd.Wait(fMtx);
	fThreadID_Listening = ZThread::sID();

	int sleepTime;
	if (spFastCancellationEnabled)
		sleepTime = 10;
	else
		sleepTime = 1;

	fMtx.Release();

	Util_POSIXFD::sWaitReadable(fSocketFD, sleepTime);

	char remoteSockAddr[SOCK_MAXADDRLEN];
	const int result = ::accept(fSocketFD,
		(sockaddr*)&remoteSockAddr, sMutablePtr(socklen_t(sizeof(remoteSockAddr))));

	fMtx.Acquire();

	fThreadID_Listening = 0;
	fCnd.Broadcast();
	fMtx.Release();

	if (result > 0)
		return new NetEndpoint_Socket(result);

	return null;
	}

void NetListener_Socket::Cancel()
	{
	fMtx.Acquire();

	// It's not essential that we do anything here other than wait for a call to Listen
	// to drop out. However, if spFastCancellationEnabled, then we issue a SIGALRM to
	// the blocked thread, which will drop out of its call to poll or select with
	// an inncoccuous EINTR.

	#if ZCONFIG_SPI_Enabled(pthread)
		if (spFastCancellationEnabled && fThreadID_Listening)
			{
			::pthread_kill(fThreadID_Listening, SIGALRM);
			}
	#endif

	while (fThreadID_Listening)
		fCnd.Wait(fMtx);

	fMtx.Release();
	}

int NetListener_Socket::GetSocketFD()
	{ return fSocketFD; }

static void spDummyAction(int iSignal)
	{}

void NetListener_Socket::sEnableFastCancellation()
	{
	#if ZCONFIG_SPI_Enabled(pthread)
		struct sigaction sigaction_new;
		sigaction_new.sa_handler = spDummyAction;
		sigaction_new.sa_flags = 0;
		sigemptyset(&sigaction_new.sa_mask);
		::sigaction(SIGALRM, &sigaction_new, nullptr);
		spFastCancellationEnabled = true;
	#endif
	}

// =================================================================================================
#pragma mark - NetEndpoint_Socket

NetEndpoint_Socket::NetEndpoint_Socket(int iSocketFD)
	{
	fSocketFD = iSocketFD;
	ZAssertStop(2, fSocketFD != -1);

	spSetSocketOptions(fSocketFD);
	}

NetEndpoint_Socket::~NetEndpoint_Socket()
	{ ::close(fSocketFD); }

size_t NetEndpoint_Socket::Read(byte* oDest, size_t iCount)
	{
	char* localDest = (char*)oDest;
	if (iCount)
		{
		for (;;)
			{
			const ssize_t result = Net_Socket::sReceive(fSocketFD, localDest, iCount);

			if (result < 0)
				{
				const int err = errno;
				if (err == EAGAIN)
					Util_POSIXFD::sWaitReadable(fSocketFD, 60);
				else if (err != EINTR)
					break;
				}
			else if (result == 0)
				{
				// result is zero, indicating that the other end has sent FIN.
				break;
				}
			else
				{
				localDest += result;
				break;
				}
			}
		}
	return localDest - (char*)oDest;
	}

size_t NetEndpoint_Socket::Readable()
	{ return Util_POSIXFD::sReadable(fSocketFD); }

size_t NetEndpoint_Socket::Write(const byte* iSource, size_t iCount)
	{
	const char* localSource = (const char*)iSource;
	while (iCount)
		{
		const ssize_t result = Net_Socket::sSend(fSocketFD, localSource, iCount);

		if (result < 0)
			{
			const int err = errno;
			if (err == EAGAIN)
				Util_POSIXFD::sWaitWriteable(fSocketFD, 10);
			else if (err != EINTR)
				break;
			}
		else if (result == 0)
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
	return localSource - (const char*)iSource;
	}

void NetEndpoint_Socket::Abort()
	{
	// Cause a RST to be sent when we close().
	// See UNIX Network Programming, 2nd Ed, Stevens, page 423

	struct linger theLinger;
	theLinger.l_onoff = 1;
	theLinger.l_linger = 0;

	::setsockopt(fSocketFD, SOL_SOCKET, SO_LINGER, (char*)&theLinger, sizeof(theLinger));
	::shutdown(fSocketFD, SHUT_RDWR);
	}

bool NetEndpoint_Socket::DisconnectRead(double iTimeout)
	{
	for (;;)
		{
		const ssize_t result = Net_Socket::sReceive(fSocketFD, sGarbageBuffer, sizeof(sGarbageBuffer));
		if (result == 0)
			{
			// result is zero, indicating that the other end has sent FIN.
			::shutdown(fSocketFD, SHUT_RD);
			return true;
			}
		else if (result < 0)
			{
			const int err = errno;
			if (err == EAGAIN)
				{
				Util_POSIXFD::sWaitReadable(fSocketFD, 60);
				}
			else
				{
				return false;
				}
			}
		}
	}

void NetEndpoint_Socket::DisconnectWrite()
	{
	::shutdown(fSocketFD, SHUT_WR);
	}

bool NetEndpoint_Socket::WaitReadable(double iTimeout)
	{ return Util_POSIXFD::sWaitReadable(fSocketFD, iTimeout); }

int NetEndpoint_Socket::GetSocketFD()
	{ return fSocketFD; }

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Socket)
