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

#include "zoolib/ZNet_Socket.h"

#if ZCONFIG_API_Enabled(Net_Socket)

#include "zoolib/ZMemory.h"
#include "zoolib/ZTime.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>

// See comment in Solaris' /usr/include/sys/ioctl.h
#if __sun__
#	define BSD_COMP
#endif
#include <sys/ioctl.h>
#include <sys/socket.h>

// AG 2005-01-04. It looks like poll.h is not always present on MacOS X.
// We don't use poll on MacOS, so we can just skip the include for now.
#ifndef __APPLE__
#	define POLL_NO_WARN // Switch off Apple warning re poll().
#	include <poll.h>
#endif

using namespace ZooLib;

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZNet_Socket

// On MacOS X (and FreeBSD) and Linux a send or receive on a socket where the other end
// has closed can cause delivery of a sigpipe. These helper functions, and the conditional
// code in the ZNetEndpoint_Socket constructor, work around that issue.

#ifdef __APPLE__
// For MacOS X we set an option on the socket to indicate that sigpipe should not
// be delivered when the far end closes on us. That option was not defined in headers
// prior to 10.2, so we define it ourselves if necessary.

#ifndef SO_NOSIGPIPE
#	define SO_NOSIGPIPE 0x1022
#endif

#ifndef TCP_NODELAY
#	include <netinet/tcp.h>
#endif

static void sSetSocketOptions(int iSocket)
	{
	// Set the socket to be non blocking
	::fcntl(iSocket, F_SETFL, ::fcntl(iSocket, F_GETFL,0) | O_NONBLOCK);

	// Enable keep alive
	int keepAliveFlag = 1;
	::setsockopt(iSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAliveFlag, sizeof(keepAliveFlag));

	// Disable sigpipe when writing/reading a far-closed socket.
	int noSigPipeFlag = 1;
	::setsockopt(iSocket, SOL_SOCKET, SO_NOSIGPIPE, (char*)&noSigPipeFlag, sizeof(noSigPipeFlag));
	}

int ZNet_Socket::sSend(int iSocket, const char* iSource, size_t iCount)
	{ return ::send(iSocket, iSource, iCount, 0); }

int ZNet_Socket::sReceive(int iSocket, char* iDest, size_t iCount)
	{ return ::recv(iSocket, iDest, iCount, 0); }

bool ZNet_Socket::sWaitReadable(int iSocket, int iMilliseconds)
	{
	fd_set readSet, exceptSet;
	FD_ZERO(&readSet);
	FD_ZERO(&exceptSet);
	FD_SET(iSocket, &readSet);
	FD_SET(iSocket, &exceptSet);

	struct timeval timeOut;
	timeOut.tv_sec = iMilliseconds / 1000;
	timeOut.tv_usec = (iMilliseconds % 1000) * 1000;
	return 0 < ::select(iSocket + 1, &readSet, nil, &exceptSet, &timeOut);
	}

void ZNet_Socket::sWaitWriteable(int iSocket)
	{
	fd_set writeSet;
	FD_ZERO(&writeSet);
	FD_SET(iSocket, &writeSet);

	struct timeval timeOut;
	timeOut.tv_sec = 1;
	timeOut.tv_usec = 0;

	::select(iSocket + 1, nil, &writeSet, nil, &timeOut);
	}

#elif defined(linux) || defined(__sun__)

// RedHat Linux 6.1 adds the sigpipe on closed behavior to recv and send, and also adds
// a new flag that can be passed to supress that behavior. Older kernels return EINVAL
// if they don't recognize the flag, and thus don't have the behavior. So we use a static
// bool to remember if MSG_NOSIGNAL is available in the kernel.

static void sSetSocketOptions(int iSocket)
	{
	// Set the socket to be non blocking
	::fcntl(iSocket, F_SETFL, ::fcntl(iSocket, F_GETFL,0) | O_NONBLOCK);

	// Enable keep alive
	int keepAliveFlag = 1;
	::setsockopt(iSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAliveFlag, sizeof(keepAliveFlag));
	}

#ifndef MSG_NOSIGNAL
#	define MSG_NOSIGNAL 0x2000
#endif

static bool sCanUse_MSG_NOSIGNAL = false;
static bool sChecked_MSG_NOSIGNAL = false;

int ZNet_Socket::sSend(int iSocket, const char* iSource, size_t iCount)
	{
	if (sCanUse_MSG_NOSIGNAL)
		{
		return ::send(iSocket, iSource, iCount, MSG_NOSIGNAL);
		}
	else
		{
		if (sChecked_MSG_NOSIGNAL)
			{
			return ::send(iSocket, iSource, iCount, 0);
			}
		else
			{
			int result = ::send(iSocket, iSource, iCount, MSG_NOSIGNAL);
			if (result >= 0)
				{
				sCanUse_MSG_NOSIGNAL = true;
				sChecked_MSG_NOSIGNAL = true;
				}
			else
				{
				if (errno == EINVAL)
					{
					sChecked_MSG_NOSIGNAL = true;
					return ::send(iSocket, iSource, iCount, 0);
					}				
				}
			return result;
			}
		}
	}

int ZNet_Socket::sReceive(int iSocket, char* iDest, size_t iCount)
	{
	if (sCanUse_MSG_NOSIGNAL)
		{
		return ::recv(iSocket, iDest, iCount, MSG_NOSIGNAL);
		}
	else
		{
		if (sChecked_MSG_NOSIGNAL)
			{
			return ::recv(iSocket, iDest, iCount, 0);
			}
		else
			{
			int result = ::recv(iSocket, iDest, iCount, MSG_NOSIGNAL);

			if (result >= 0)
				{
				sCanUse_MSG_NOSIGNAL = true;
				sChecked_MSG_NOSIGNAL = true;
				}
			else
				{
				if (errno == EINVAL)
					{
					sChecked_MSG_NOSIGNAL = true;
					return ::recv(iSocket, iDest, iCount, 0);
					}				
				}
			return result;
			}
		}
	}

bool ZNet_Socket::sWaitReadable(int iSocket, int iMilliseconds)
	{
	pollfd thePollFD;
	thePollFD.fd = iSocket;
	thePollFD.events = POLLIN | POLLPRI;
	thePollFD.revents = 0;
	return 0 < ::poll(&thePollFD, 1, iMilliseconds);
	}

void ZNet_Socket::sWaitWriteable(int iSocket)
	{
	pollfd thePollFD;
	thePollFD.fd = iSocket;
	thePollFD.events = POLLOUT;
	::poll(&thePollFD, 1, 1000);
	}

#endif

ZNet::Error ZNet_Socket::sTranslateError(int iNativeError)
	{
	ZNet::Error theError = ZNet::errorGeneric;
	switch (iNativeError)
		{
		case EADDRINUSE: theError = ZNet::errorLocalPortInUse; break;
		case EINVAL: theError = ZNet::errorBadState; break;
		case EBADF: theError = ZNet::errorBadState; break;
		case ECONNREFUSED: theError = ZNet::errorCouldntConnect; break;
		case 0: theError = ZNet::errorNone; break;
		default: break;
		}
	return theError;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_Socket

static bool sFastCancellationEnabled;

ZNetListener_Socket::ZNetListener_Socket(int iSocketFD, size_t iListenQueueSize)
:	fSocketFD(iSocketFD)
	{
	if (::listen(fSocketFD, iListenQueueSize) < 0)
		{
		int err = errno;
		::close(fSocketFD);
		throw ZNetEx(sTranslateError(err));
		}

	::fcntl(fSocketFD, F_SETFL, ::fcntl(fSocketFD, F_GETFL,0) | O_NONBLOCK);

	fThreadID_Listening = 0;
	}

ZNetListener_Socket::~ZNetListener_Socket()
	{
	::close(fSocketFD);
	}

ZRef<ZNetEndpoint> ZNetListener_Socket::Listen()
	{
	fMutexNR.Acquire();
	while (fThreadID_Listening)
		fCondition.Wait(fMutexNR);
	fThreadID_Listening = ZThread::sCurrentID();

	int sleepTime;
	if (sFastCancellationEnabled)
		sleepTime = 10000;
	else
		sleepTime = 1000;

	fMutexNR.Release();

	sWaitReadable(fSocketFD, sleepTime);

	char remoteSockAddr[1024];
	ZBlockSet(&remoteSockAddr, sizeof(remoteSockAddr), 0);
	socklen_t addrSize = sizeof(remoteSockAddr);
	int result = ::accept(fSocketFD, (sockaddr*)&remoteSockAddr, &addrSize);

	fMutexNR.Acquire();

	fThreadID_Listening = 0;
	fCondition.Broadcast();
	fMutexNR.Release();

	if (result > 0)
		{
		try
			{
			return this->Imp_MakeEndpoint(result);
			}
		catch (...)
			{}
		::close(result);
		}

	return ZRef<ZNetEndpoint>();
	}

void ZNetListener_Socket::CancelListen()
	{
	fMutexNR.Acquire();
	
	// It's not essential that we do anything here other than wait for a call to Listen
	// to drop out. However, if sFastCancellationEnabled, then we issue a SIGALRM to
	// the blocked thread, which will drop out of its call to poll or select with
	// an inncoccuous EINTR.
	
	#if ZCONFIG_SPI_Enabled(pthread)
		if (sFastCancellationEnabled && fThreadID_Listening)
			{
			::pthread_kill(fThreadID_Listening, SIGALRM);
			}
	#endif

	while (fThreadID_Listening)
		fCondition.Wait(fMutexNR);

	fMutexNR.Release();
	}

int ZNetListener_Socket::GetSocketFD()
	{ return fSocketFD; }

static void sDummyAction(int iSignal)
	{}

void ZNetListener_Socket::sEnableFastCancellation()
	{
	#if ZCONFIG_SPI_Enabled(pthread)
		struct sigaction sigaction_new;
		sigaction_new.sa_handler = sDummyAction;
		sigaction_new.sa_flags = 0;
		sigemptyset(&sigaction_new.sa_mask);
		::sigaction(SIGALRM, &sigaction_new, nil);
		sFastCancellationEnabled = true;
	#endif
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_Socket

ZNetEndpoint_Socket::ZNetEndpoint_Socket(int iSocketFD)
	{
	fSocketFD = iSocketFD;
	ZAssertStop(2, fSocketFD != -1);

	sSetSocketOptions(fSocketFD);
	}

ZNetEndpoint_Socket::~ZNetEndpoint_Socket()
	{
	::close(fSocketFD);
	}

const ZStreamRCon& ZNetEndpoint_Socket::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& ZNetEndpoint_Socket::GetStreamWCon()
	{ return *this; }

int ZNetEndpoint_Socket::GetSocketFD()
	{ return fSocketFD; }

void ZNetEndpoint_Socket::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = static_cast<char*>(iDest);
	while (iCount)
		{
		int result = sReceive(fSocketFD, localDest, iCount);

		if (result < 0)
			{
			int err = errno;
			if (err == EAGAIN)
				{
				sWaitReadable(fSocketFD, 1000);
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
	if (oCountRead)
		*oCountRead = localDest - static_cast<char*>(iDest);
	}

size_t ZNetEndpoint_Socket::Imp_CountReadable()
	{
	int localResult;
	if (0 <= ::ioctl(fSocketFD, FIONREAD, &localResult))
		return localResult;
	return 0;
	}

bool ZNetEndpoint_Socket::Imp_WaitReadable(int iMilliseconds)
	{
	return sWaitReadable(fSocketFD, iMilliseconds);
	}

void ZNetEndpoint_Socket::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const char* localSource = static_cast<const char*>(iSource);
	while (iCount)
		{
		int result = sSend(fSocketFD, localSource, iCount);

		if (result < 0)
			{
			int err = errno;
			if (err == EAGAIN)
				{
				sWaitWriteable(fSocketFD);
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
	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const char*>(iSource);
	}

bool ZNetEndpoint_Socket::Imp_ReceiveDisconnect(int iMilliseconds)
	{
	ZTime endTime = ZTime::sSystem() + iMilliseconds / 1000.0;

	bool gotIt = false;
	for (;;)
		{
		int result = sReceive(fSocketFD, ZooLib::sGarbageBuffer, sizeof(ZooLib::sGarbageBuffer));
		if (result == 0)
			{
			// result is zero, indicating that the other end has sent FIN.
			gotIt = true;
			break;
			}
		else if (result < 0)
			{
			int err = errno;
			if (err == EAGAIN)
				{
				if (iMilliseconds < 0)
					{
					sWaitReadable(fSocketFD, 1000);
					}
				else
					{
					ZTime now = ZTime::sSystem();
					if (endTime < now)
						break;
					sWaitReadable(fSocketFD, int(1000 * (endTime - now)));
					}
				}
			else if (err != EINTR)
				{
				break;
				}
			}
		}
	return gotIt;
	}

void ZNetEndpoint_Socket::Imp_SendDisconnect()
	{
	::shutdown(fSocketFD, SHUT_WR);
	}

void ZNetEndpoint_Socket::Imp_Abort()
	{
	// Cause a RST to be sent when we close(). See UNIX Network
	// Programming, 2nd Ed, Stevens, page 423

	struct linger theLinger;
	theLinger.l_onoff = 1;

	// AG 2001-10-04. [Stevens] says that l_linger should be 0. The code previously set l_linger
	// to be 1, I'm not sure if we found that to be correct in practice so I'm correcting it --
	// if we experience problems, let me know.
	theLinger.l_linger = 0;

	::setsockopt(fSocketFD, SOL_SOCKET, SO_LINGER, (char*)&theLinger, sizeof(theLinger));
	::shutdown(fSocketFD, SHUT_RDWR);
	}

// =================================================================================================

#endif // ZCONFIG_API_Enabled(Net_Socket)
