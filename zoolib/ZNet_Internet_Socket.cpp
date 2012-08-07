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

#include "zoolib/ZNet_Internet_Socket.h"

#if ZCONFIG_API_Enabled(Net_Internet_Socket)

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZMemory.h"

#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

using std::string;

namespace ZooLib {

// =================================================================================================
// MARK: - Factory functions

namespace { // anonymous

class Make_NameLookup
:	public ZFunctionChain_T<ZRef<ZNetNameLookup>, ZNetName_Internet::LookupParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetNameLookup_Internet_Socket(iParam.f0, iParam.f1, iParam.f2);
		return true;
		}
	} sMaker0;

class Make_Listener4
:	public ZFunctionChain_T<ZRef<ZNetListener_TCP>, ZNetListener_TCP::MakeParam4_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetListener_TCP_Socket(iParam.f0, iParam.f1);
		return true;
		}
	} sMaker1;

class Make_Listener6
:	public ZFunctionChain_T<ZRef<ZNetListener_TCP>, ZNetListener_TCP::MakeParam6_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetListener_TCP_Socket(iParam.f0, iParam.f1);
		return true;
		}
	} sMaker2;

class Make_Endpoint4
:	public ZFunctionChain_T<ZRef<ZNetEndpoint_TCP>, ZNetEndpoint_TCP::MakeParam4_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetEndpoint_TCP_Socket(iParam.f0, iParam.f1);
		return true;
		}
	} sMaker3;

class Make_Endpoint6
:	public ZFunctionChain_T<ZRef<ZNetEndpoint_TCP>, ZNetEndpoint_TCP::MakeParam6_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetEndpoint_TCP_Socket(iParam.f0, iParam.f1);
		return true;
		}
	} sMaker4;

} // anonymous namespace

// =================================================================================================
// MARK: - Helpers

static ZRef<ZNetAddress_Internet> spAsNetAddress(const sockaddr* iSockAddr, ip_port iPort)
	{
	const sa_family_t theFamily = ((sockaddr*)iSockAddr)->sa_family;

	if (theFamily == AF_INET)
		{
		const sockaddr_in* in = (const sockaddr_in*)iSockAddr;
		if (not iPort)
			iPort = ntohs(in->sin_port);
		return new ZNetAddress_IP4
			(ntohl(in->sin_addr.s_addr),
			iPort);
		}

	if (theFamily == AF_INET6)
		{
		const sockaddr_in6* in = (const sockaddr_in6*)iSockAddr;
		if (not iPort)
			iPort = ntohs(in->sin6_port);
		return new ZNetAddress_IP6
			(iPort,
			*((const struct ip6_addr*)(&in->sin6_addr)));
		}

	return null;
	}

// =================================================================================================
// MARK: - ZNet_TCP_Socket

int ZNet_TCP_Socket::sListen(ip4_addr iLocalAddress, ip_port iLocalPort)
	{
	int theSocketFD = ::socket(PF_INET, SOCK_STREAM, 0);
	if (theSocketFD < 0)
		{
		int err = errno;
		throw ZNetEx(ZNet_Socket::sTranslateError(err));
		}

	int reuseAddrFlag = 1;
	::setsockopt(theSocketFD,
		SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddrFlag, sizeof(reuseAddrFlag));

	sockaddr_in localSockAddr = {0};
	localSockAddr.sin_len = sizeof(localSockAddr);
	localSockAddr.sin_family = AF_INET;
	localSockAddr.sin_port = htons(iLocalPort);
	localSockAddr.sin_addr.s_addr = htonl(iLocalAddress);

	if (::bind(theSocketFD, (sockaddr*)&localSockAddr, sizeof(localSockAddr)) < 0)
		{
		int err = errno;
		::close(theSocketFD);
		throw ZNetEx(ZNet_Socket::sTranslateError(err));
		}
	return theSocketFD;
	}

int ZNet_TCP_Socket::sListen(ip6_addr iLocalAddress, ip_port iLocalPort)
	{
	int theSocketFD = ::socket(PF_INET6, SOCK_STREAM, 0);
	if (theSocketFD < 0)
		{
		int err = errno;
		throw ZNetEx(ZNet_Socket::sTranslateError(err));
		}

	int reuseAddrFlag = 1;
	::setsockopt(theSocketFD,
		SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddrFlag, sizeof(reuseAddrFlag));

	sockaddr_in6 localSockAddr = {0};
	localSockAddr.sin6_len = sizeof(localSockAddr);
	localSockAddr.sin6_family = AF_INET6;
	localSockAddr.sin6_port = htons(iLocalPort);
	localSockAddr.sin6_addr = *(const struct in6_addr*)(&iLocalAddress);

	if (::bind(theSocketFD, (sockaddr*)&localSockAddr, sizeof(localSockAddr)) < 0)
		{
		int err = errno;
		::close(theSocketFD);
		throw ZNetEx(ZNet_Socket::sTranslateError(err));
		}
	return theSocketFD;
	}

// =================================================================================================
// MARK: - ZNetNameLookup_Internet_Socket

ZNetNameLookup_Internet_Socket::ZNetNameLookup_Internet_Socket
	(const string& iName, ip_port iPort, size_t iMaxAddresses)
:	fName(iName)
,	fPort(iPort)
,	fCountAddressesToReturn(iMaxAddresses)
,	fStarted(true)
,	fCurrentIndex(0)
	{
	if (fName.empty())
		return;

	ip6_addr theAddr6;
	if (1 == ::inet_pton(AF_INET6, fName.c_str(), &theAddr6))
		{
		fAddresses.push_back(new ZNetAddress_IP6(fPort, theAddr6));
		return;
		}

	ip4_addr theAddr4;
	if (1 == ::inet_pton(AF_INET, fName.c_str(), &theAddr4))
		{
		fAddresses.push_back(new ZNetAddress_IP4(ntohl(theAddr4), fPort));
		return;
		}

	fStarted = false;
	}

ZNetNameLookup_Internet_Socket::~ZNetNameLookup_Internet_Socket()
	{}

void ZNetNameLookup_Internet_Socket::Start()
	{
	if (fStarted)
		return;

	fStarted = true;

	struct addrinfo hints = {0};
	hints.ai_family = PF_UNSPEC;
	struct addrinfo* theAI;
	if (0 != ::getaddrinfo(fName.c_str(), nullptr, nullptr, &theAI))
		return;

	for (struct addrinfo* iterAI = theAI;
		iterAI && fAddresses.size() < fCountAddressesToReturn;
		iterAI = iterAI->ai_next)
		{
		if (ZRef<ZNetAddress_Internet> theNA = spAsNetAddress((sockaddr*)iterAI->ai_addr, fPort))
			fAddresses.push_back(theNA);
		}

	::freeaddrinfo(theAI);
	}

bool ZNetNameLookup_Internet_Socket::Finished()
	{
	if (not fStarted)
		return true;

	return fCurrentIndex >= fAddresses.size();
	}

void ZNetNameLookup_Internet_Socket::Advance()
	{
	ZAssertStop(2, fStarted);
	if (fCurrentIndex < fAddresses.size())
		++fCurrentIndex;
	}

ZRef<ZNetAddress> ZNetNameLookup_Internet_Socket::CurrentAddress()
	{
	if (fCurrentIndex < fAddresses.size())
		return fAddresses[fCurrentIndex];

	return null;
	}

ZRef<ZNetName> ZNetNameLookup_Internet_Socket::CurrentName()
	{ return new ZNetName_Internet(fName, fPort); }

// =================================================================================================
// MARK: - ZNetListener_TCP_Socket

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
	throw std::runtime_error("ZNetListener_TCP_Socket, not an inet socket");
	}

ZRef<ZNetListener_TCP_Socket> ZNetListener_TCP_Socket::sCreateWithFD(int iFD)
	{
	try
		{
		return new ZNetListener_TCP_Socket(iFD, IKnowWhatIAmDoing);
		}
	catch (...)
		{}
	return null;
	}

ZNetListener_TCP_Socket::ZNetListener_TCP_Socket(int iSocketFD, const IKnowWhatIAmDoing_t&)
:	ZNetListener_Socket(spEnsureInet(iSocketFD))
	{}

ZNetListener_TCP_Socket::ZNetListener_TCP_Socket(ip_port iLocalPort)
:	ZNetListener_Socket(ZNet_TCP_Socket::sListen(0, iLocalPort))
	{}

ZNetListener_TCP_Socket::ZNetListener_TCP_Socket(ip4_addr iLocalAddress, ip_port iLocalPort)
:	ZNetListener_Socket(ZNet_TCP_Socket::sListen(iLocalAddress, iLocalPort))
	{}

ZNetListener_TCP_Socket::ZNetListener_TCP_Socket(ip6_addr iLocalAddress, ip_port iLocalPort)
:	ZNetListener_Socket(ZNet_TCP_Socket::sListen(iLocalAddress, iLocalPort))
	{}

ZNetListener_TCP_Socket::~ZNetListener_TCP_Socket()
	{}

ip_port ZNetListener_TCP_Socket::GetPort()
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

ZRef<ZNetAddress> ZNetListener_TCP_Socket::GetAddress()
	{
	uint8 buffer[SOCK_MAXADDRLEN];
	if (0 <= ::getsockname(this->GetSocketFD(), (sockaddr*)buffer, sMutablePtr(socklen_t(sizeof(buffer)))))
		return spAsNetAddress((sockaddr*)buffer, 0);

	return null;
	}

ZRef<ZNetEndpoint> ZNetListener_TCP_Socket::Imp_MakeEndpoint(int iSocketFD)
	{ return new ZNetEndpoint_TCP_Socket(iSocketFD); }

// =================================================================================================
// MARK: - ZNetEndpoint_TCP_Socket

static void spSetSocketOptions(int iSocketFD)
	{
	// There's no simple way to flush a socket under MacOS X. We could hold on to the last byte
	// written, then set NODELAY, write the byte, and clear NODELAY. I don't want to munge
	// this code to do that right now. So for the moment on MacOS X we simply set NODELAY and
	// expect higher level code to do buffering if it doesn't want to flood the connection with
	// lots of small packets. This is a reasonable expectation because it's what has to be done
	// anyway under Linux to prevent Nagle algorithm from kicking in.
	int noDelayFlag = 1;
	::setsockopt(iSocketFD, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelayFlag, sizeof(noDelayFlag));
	}

static int spConnect4(ip4_addr iLocalHost, ip_port iLocalPort, ip4_addr iRemoteHost, ip_port iRemotePort)
	{
	int socketFD = ::socket(PF_INET, SOCK_STREAM, 0);
	if (socketFD < 0)
		throw ZNetEx(ZNet_Socket::sTranslateError(errno));

	if (iLocalHost || iLocalPort)
		{
		sockaddr_in localSockAddr = {0};
		localSockAddr.sin_len = sizeof(localSockAddr);
		localSockAddr.sin_family = AF_INET;
		localSockAddr.sin_port = htons(iLocalPort);
		localSockAddr.sin_addr.s_addr = htonl(iLocalHost);
		if (::bind(socketFD, (sockaddr*)&localSockAddr, sizeof(localSockAddr)) < 0)
			{
			int err = errno;
			::close(socketFD);
			throw ZNetEx(ZNet_Socket::sTranslateError(err));
			}
		}

	sockaddr_in remoteSockAddr = {0};
	remoteSockAddr.sin_len = sizeof(remoteSockAddr);
	remoteSockAddr.sin_family = AF_INET;
	remoteSockAddr.sin_port = htons(iRemotePort);
	remoteSockAddr.sin_addr.s_addr = htonl(iRemoteHost);
	if (::connect(socketFD, (sockaddr*)&remoteSockAddr, sizeof(remoteSockAddr)) < 0)
		{
		int err = errno;
		::close(socketFD);
		throw ZNetEx(ZNet_Socket::sTranslateError(err));
		}
	return socketFD;
	}

static int spConnect6(ip6_addr iRemoteHost, ip_port iRemotePort)
	{
	int socketFD = ::socket(PF_INET6, SOCK_STREAM, 0);
	if (socketFD < 0)
		throw ZNetEx(ZNet_Socket::sTranslateError(errno));

	sockaddr_in6 remoteSockAddr = {0};
	remoteSockAddr.sin6_len = sizeof(remoteSockAddr);
	remoteSockAddr.sin6_family = AF_INET6;
	remoteSockAddr.sin6_port = htons(iRemotePort);
	remoteSockAddr.sin6_addr = *(const struct in6_addr*)(&iRemoteHost);
	if (::connect(socketFD, (sockaddr*)&remoteSockAddr, sizeof(remoteSockAddr)) < 0)
		{
		int err = errno;
		::close(socketFD);
		throw ZNetEx(ZNet_Socket::sTranslateError(err));
		}
	return socketFD;
	}

ZNetEndpoint_TCP_Socket::ZNetEndpoint_TCP_Socket(int iSocketFD)
:	ZNetEndpoint_Socket(iSocketFD)
	{
	spSetSocketOptions(this->GetSocketFD());
	}

ZNetEndpoint_TCP_Socket::ZNetEndpoint_TCP_Socket
	(ip4_addr iLocalHost, ip_port iLocalPort, ip4_addr iRemoteHost, ip_port iRemotePort)
:	ZNetEndpoint_Socket(spConnect4(iLocalHost, iLocalPort, iRemoteHost, iRemotePort))
	{
	spSetSocketOptions(this->GetSocketFD());
	}

ZNetEndpoint_TCP_Socket::ZNetEndpoint_TCP_Socket(ip4_addr iRemoteHost, ip_port iRemotePort)
:	ZNetEndpoint_Socket(spConnect4(ZNetAddress_IP4::sAny, 0, iRemoteHost, iRemotePort))
	{
	spSetSocketOptions(this->GetSocketFD());
	}

ZNetEndpoint_TCP_Socket::ZNetEndpoint_TCP_Socket(ip6_addr iRemoteHost, ip_port iRemotePort)
:	ZNetEndpoint_Socket(spConnect6(iRemoteHost, iRemotePort))
	{
	spSetSocketOptions(this->GetSocketFD());
	}

ZNetEndpoint_TCP_Socket::~ZNetEndpoint_TCP_Socket()
	{}

ZRef<ZNetAddress> ZNetEndpoint_TCP_Socket::GetLocalAddress()
	{
	uint8 buffer[SOCK_MAXADDRLEN];
	if (0 <= ::getsockname(this->GetSocketFD(), (sockaddr*)buffer, sMutablePtr(socklen_t(sizeof(buffer)))))
		return spAsNetAddress((sockaddr*)buffer, 0);

	return null;
	}

ZRef<ZNetAddress> ZNetEndpoint_TCP_Socket::GetRemoteAddress()
	{
	uint8 buffer[SOCK_MAXADDRLEN];
	if (0 <= ::getpeername(this->GetSocketFD(), (sockaddr*)buffer, sMutablePtr(socklen_t(sizeof(buffer)))))
		return spAsNetAddress((sockaddr*)buffer, 0);

	return null;
	}

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Internet_Socket)
