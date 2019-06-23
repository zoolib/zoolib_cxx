/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/POSIX/Net_Internet_Socket.h"

#if ZCONFIG_API_Enabled(Net_Internet_Socket)

ZMACRO_MSVCStaticLib_cpp(Net_Internet_Socket)

#include "zoolib/Compat_cmath.h" // For fmod
#include "zoolib/Memory.h"

#include "zoolib/POSIX/Util_POSIXFD.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

using std::string;

#if not defined(SOCK_MAXADDRLEN)
	#define SOCK_MAXADDRLEN (255)
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - NetEndpoint_TCP_Socket

static void spSetSocketOptions(int iSocketFD)
	{
	// There's no simple way to flush a socket under MacOS X. We could hold on to the last byte
	// written, then set NODELAY, write the byte, and clear NODELAY. I don't want to munge
	// this code to do that right now. So for the moment on MacOS X we simply set NODELAY and
	// expect higher level code to do buffering if it doesn't want to flood the connection with
	// lots of small packets. This is a reasonable expectation because it's what has to be done
	// anyway under Linux to prevent Nagle algorithm from kicking in.
	::setsockopt(iSocketFD, IPPROTO_TCP, TCP_NODELAY, sConstPtr(int(1)), sizeof(int));
	}

static bool spWaitConnected(int iFD, double iTimeout)
	{
	fd_set writeSet;
	Util_POSIXFD::sSetup(writeSet, iFD);

	struct timeval timeout;
	timeout.tv_sec = int(iTimeout);
	timeout.tv_usec = int(fmod(iTimeout, 1.0) * 1e6);

	int selectResult = ::select(iFD + 1, nullptr, &writeSet, nullptr, &timeout);
	if (selectResult < 0)
		{
		// select failed, presume that FD is bad too.
		return false;
		}

	if (selectResult == 0)
		{
		// There were zero writeable FDs, so also not yet usable.
		return false;
		}

	int selectErr = 0;
	if (0 != getsockopt(iFD, SOL_SOCKET, SO_ERROR,
		(char*)&selectErr, sMutablePtr(socklen_t(sizeof(selectErr)))))
		{
		// We failed to get an error value, so presumably there is no error, and the FD is connected.
		return true;
		}

	// The FD is connected if the reported error was None.
	return selectErr == 0;
	}

static int spConnect4(ip4_addr iLocalHost, ip_port iLocalPort, ip4_addr iRemoteHost, ip_port iRemotePort)
	{
	int socketFD = ::socket(PF_INET, SOCK_STREAM, 0);
	if (socketFD < 0)
		throw NetEx(Net_Socket::sTranslateError(errno));

	if (iLocalHost || iLocalPort)
		{
		sockaddr_in localSockAddr = {};
		#if ZCONFIG_SPI_Enabled(BSD)
			localSockAddr.sin_len = sizeof(localSockAddr);
		#endif
		localSockAddr.sin_family = AF_INET;
		localSockAddr.sin_port = htons(iLocalPort);
		localSockAddr.sin_addr.s_addr = htonl(iLocalHost);
		if (::bind(socketFD, (sockaddr*)&localSockAddr, sizeof(localSockAddr)) < 0)
			{
			int err = errno;
			::close(socketFD);
			throw NetEx(Net_Socket::sTranslateError(err));
			}
		}

	sockaddr_in remoteSockAddr = {};
	#if ZCONFIG_SPI_Enabled(BSD)
		remoteSockAddr.sin_len = sizeof(remoteSockAddr);
	#endif
	remoteSockAddr.sin_family = AF_INET;
	remoteSockAddr.sin_port = htons(iRemotePort);
	remoteSockAddr.sin_addr.s_addr = htonl(iRemoteHost);

	::fcntl(socketFD, F_SETFL, ::fcntl(socketFD, F_GETFL, 0) | O_NONBLOCK);

	int result = ::connect(socketFD, (sockaddr*)&remoteSockAddr, sizeof(remoteSockAddr));
	if (result == 0)
		return socketFD;

	int err = errno;
	if (err != EINPROGRESS)
		{
		::close(socketFD);
		throw NetEx(Net_Socket::sTranslateError(err));
		}

	if (not spWaitConnected(socketFD, 10))
		{
		::close(socketFD);
		throw NetEx(Net::errorCouldntConnect);
		}

	return socketFD;
	}

static int spConnect6(ip6_addr iRemoteHost, ip_port iRemotePort)
	{
	int socketFD = ::socket(PF_INET6, SOCK_STREAM, 0);
	if (socketFD < 0)
		throw NetEx(Net_Socket::sTranslateError(errno));

	sockaddr_in6 remoteSockAddr = {};
	#if ZCONFIG_SPI_Enabled(BSD)
		remoteSockAddr.sin6_len = sizeof(remoteSockAddr);
	#endif
	remoteSockAddr.sin6_family = AF_INET6;
	remoteSockAddr.sin6_port = htons(iRemotePort);
	remoteSockAddr.sin6_addr = *(const struct in6_addr*)(&iRemoteHost);
	if (::connect(socketFD, (sockaddr*)&remoteSockAddr, sizeof(remoteSockAddr)) < 0)
		{
		int err = errno;
		::close(socketFD);
		throw NetEx(Net_Socket::sTranslateError(err));
		}
	return socketFD;
	}

// =================================================================================================
#pragma mark - Factory functions

namespace { // anonymous

class Make_NameLookup
:	public FunctionChain<ZP<NetNameLookup>, NetName_Internet::LookupParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new NetNameLookup_Internet_Socket(iParam.f0, iParam.f1, iParam.f2);
		return true;
		}
	} sMaker0;

class Make_Listener4
:	public FunctionChain<ZP<NetListener_TCP>, MakeParam4_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new NetListener_TCP_Socket(iParam.f0, iParam.f1);
		return true;
		}
	} sMaker1;

class Make_Listener6
:	public FunctionChain<ZP<NetListener_TCP>, MakeParam6_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new NetListener_TCP_Socket(iParam.f0, iParam.f1);
		return true;
		}
	} sMaker2;

class Make_Endpoint4
:	public FunctionChain<ZP<ChannerRWClose_Bin>, MakeParam4_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		const int theSocketFD = spConnect4(0, 0, iParam.f0, iParam.f1);
		spSetSocketOptions(theSocketFD);
		ZP<NetEndpoint_Socket> theEP = new NetEndpoint_Socket(theSocketFD);
		oResult = theEP;
		return true;
		}
	} sMaker3;

class Make_Endpoint6
:	public FunctionChain<ZP<ChannerRWClose_Bin>, MakeParam6_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		const int theSocketFD = spConnect6(iParam.f0, iParam.f1);
		spSetSocketOptions(theSocketFD);
		ZP<NetEndpoint_Socket> theEP = new NetEndpoint_Socket(theSocketFD);
		oResult = theEP;
		return true;
		}
	} sMaker4;

} // anonymous namespace

// =================================================================================================
#pragma mark - Helpers

static ZP<NetAddress_Internet> spAsNetAddress(const sockaddr* iSockAddr, ip_port iPort)
	{
	const sa_family_t theFamily = ((sockaddr*)iSockAddr)->sa_family;

	if (theFamily == AF_INET)
		{
		const sockaddr_in* in = (const sockaddr_in*)iSockAddr;
		if (not iPort)
			iPort = ntohs(in->sin_port);
		return new NetAddress_IP4(
			ntohl(in->sin_addr.s_addr),
			iPort);
		}

	if (theFamily == AF_INET6)
		{
		const sockaddr_in6* in = (const sockaddr_in6*)iSockAddr;
		if (not iPort)
			iPort = ntohs(in->sin6_port);
		return new NetAddress_IP6(
			iPort,
			*((const struct ip6_addr*)(&in->sin6_addr)));
		}

	return null;
	}

// =================================================================================================
#pragma mark - Net_TCP_Socket

int Net_TCP_Socket::sListen(ip4_addr iLocalAddress, ip_port iLocalPort)
	{
	int theSocketFD = ::socket(PF_INET, SOCK_STREAM, 0);
	if (theSocketFD < 0)
		{
		int err = errno;
		throw NetEx(Net_Socket::sTranslateError(err));
		}

	::setsockopt(theSocketFD, SOL_SOCKET, SO_REUSEADDR,
		(const char*)(const int*)sConstPtr(int(1)), sizeof(int));

	sockaddr_in localSockAddr = {};
	#if ZCONFIG_SPI_Enabled(BSD)
		localSockAddr.sin_len = sizeof(localSockAddr);
	#endif
	localSockAddr.sin_family = AF_INET;
	localSockAddr.sin_port = htons(iLocalPort);
	localSockAddr.sin_addr.s_addr = htonl(iLocalAddress);

	if (::bind(theSocketFD, (sockaddr*)&localSockAddr, sizeof(localSockAddr)) < 0)
		{
		int err = errno;
		::close(theSocketFD);
		throw NetEx(Net_Socket::sTranslateError(err));
		}
	return theSocketFD;
	}

int Net_TCP_Socket::sListen(ip6_addr iLocalAddress, ip_port iLocalPort)
	{
	int theSocketFD = ::socket(PF_INET6, SOCK_STREAM, 0);
	if (theSocketFD < 0)
		{
		int err = errno;
		throw NetEx(Net_Socket::sTranslateError(err));
		}

	::setsockopt(theSocketFD, SOL_SOCKET, SO_REUSEADDR,
		(const char*)(const int*)sConstPtr(int(1)), sizeof(int));

	sockaddr_in6 localSockAddr = {};
	#if ZCONFIG_SPI_Enabled(BSD)
		localSockAddr.sin6_len = sizeof(localSockAddr);
	#endif
	localSockAddr.sin6_family = AF_INET6;
	localSockAddr.sin6_port = htons(iLocalPort);
	localSockAddr.sin6_addr = *(const struct in6_addr*)(&iLocalAddress);

	if (::bind(theSocketFD, (sockaddr*)&localSockAddr, sizeof(localSockAddr)) < 0)
		{
		int err = errno;
		::close(theSocketFD);
		throw NetEx(Net_Socket::sTranslateError(err));
		}
	return theSocketFD;
	}

// =================================================================================================
#pragma mark - NetNameLookup_Internet_Socket

NetNameLookup_Internet_Socket::NetNameLookup_Internet_Socket(
	const string& iName, ip_port iPort, size_t iMaxAddresses)
:	fName(iName)
,	fPort(iPort)
,	fCountAddressesToReturn(iMaxAddresses ? iMaxAddresses : 1000)
,	fStarted(true)
,	fCurrentIndex(0)
	{
	if (fName.empty())
		return;

	ip6_addr theAddr6;
	if (1 == ::inet_pton(AF_INET6, fName.c_str(), &theAddr6))
		{
		fAddresses.push_back(new NetAddress_IP6(fPort, theAddr6));
		return;
		}

	ip4_addr theAddr4;
	if (1 == ::inet_pton(AF_INET, fName.c_str(), &theAddr4))
		{
		fAddresses.push_back(new NetAddress_IP4(ntohl(theAddr4), fPort));
		return;
		}

	fStarted = false;
	}

NetNameLookup_Internet_Socket::~NetNameLookup_Internet_Socket()
	{}

void NetNameLookup_Internet_Socket::Start()
	{
	if (fStarted)
		return;

	fStarted = true;

	struct addrinfo* theAI;
	if (0 != ::getaddrinfo(fName.c_str(), nullptr, nullptr, &theAI))
		return;

	for (struct addrinfo* iterAI = theAI;
		iterAI && fAddresses.size() < fCountAddressesToReturn;
		iterAI = iterAI->ai_next)
		{
		if (ZP<NetAddress_Internet> theNA = spAsNetAddress((sockaddr*)iterAI->ai_addr, fPort))
			fAddresses.push_back(theNA);
		}

	::freeaddrinfo(theAI);
	}

bool NetNameLookup_Internet_Socket::Finished()
	{
	if (not fStarted)
		return true;

	return fCurrentIndex >= fAddresses.size();
	}

void NetNameLookup_Internet_Socket::Advance()
	{
	ZAssertStop(2, fStarted);
	if (fCurrentIndex < fAddresses.size())
		++fCurrentIndex;
	}

ZP<NetAddress> NetNameLookup_Internet_Socket::CurrentAddress()
	{
	if (fCurrentIndex < fAddresses.size())
		return fAddresses[fCurrentIndex];

	return null;
	}

ZP<NetName> NetNameLookup_Internet_Socket::CurrentName()
	{ return new NetName_Internet(fName, fPort); }

// =================================================================================================
#pragma mark - NetListener_TCP_Socket

static int spEnsureInet(int iSocketFD)
	{
	uint8 buffer[SOCK_MAXADDRLEN];
	if (0 <= ::getsockname(iSocketFD, (sockaddr*)buffer, sMutablePtr(socklen_t(sizeof(buffer)))))
		{
		const sa_family_t theFamily = ((sockaddr*)buffer)->sa_family;

		if (theFamily == AF_INET)
			return iSocketFD;

		if (theFamily == AF_INET6)
			return iSocketFD;
		}
	throw std::runtime_error("NetListener_TCP_Socket, not an inet socket");
	}

NetListener_TCP_Socket::NetListener_TCP_Socket(int iSocketFD, const IKnowWhatIAmDoing_t&)
:	NetListener_Socket(spEnsureInet(iSocketFD))
	{}

NetListener_TCP_Socket::NetListener_TCP_Socket(ip_port iLocalPort)
:	NetListener_Socket(Net_TCP_Socket::sListen(0, iLocalPort))
	{}

NetListener_TCP_Socket::NetListener_TCP_Socket(ip4_addr iLocalAddress, ip_port iLocalPort)
:	NetListener_Socket(Net_TCP_Socket::sListen(iLocalAddress, iLocalPort))
	{}

NetListener_TCP_Socket::NetListener_TCP_Socket(ip6_addr iLocalAddress, ip_port iLocalPort)
:	NetListener_Socket(Net_TCP_Socket::sListen(iLocalAddress, iLocalPort))
	{}

NetListener_TCP_Socket::~NetListener_TCP_Socket()
	{}

ip_port NetListener_TCP_Socket::GetPort()
	{
	uint8 buffer[SOCK_MAXADDRLEN];
	if (0 <= ::getsockname(fSocketFD, (sockaddr*)&buffer[0], sMutablePtr(socklen_t(sizeof(buffer)))))
		{
		const sa_family_t theFamily = ((sockaddr*)buffer)->sa_family;

		if (theFamily == AF_INET)
			return ntohs(((sockaddr_in*)buffer)->sin_port);

		if (theFamily == AF_INET6)
			return ntohs(((sockaddr_in6*)buffer)->sin6_port);
		}
	return 0;
	}

ZP<NetAddress> NetListener_TCP_Socket::GetAddress()
	{
	uint8 buffer[SOCK_MAXADDRLEN];
	if (0 <= ::getsockname(this->GetSocketFD(), (sockaddr*)buffer, sMutablePtr(socklen_t(sizeof(buffer)))))
		return spAsNetAddress((sockaddr*)buffer, 0);

	return null;
	}

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Internet_Socket)
