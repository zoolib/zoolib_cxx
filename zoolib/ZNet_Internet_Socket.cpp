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
#pragma mark -
#pragma mark * Factory functions

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


class Make_Listener
:	public ZFunctionChain_T<ZRef<ZNetListener_TCP>, ZNetListener_TCP::MakeParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetListener_TCP_Socket(iParam.f0, iParam.f1, iParam.f2);
		return true;
		}	
	} sMaker1;


class Make_Endpoint
:	public ZFunctionChain_T<ZRef<ZNetEndpoint_TCP>, ZNetEndpoint_TCP::MakeParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetEndpoint_TCP_Socket(iParam.f0, iParam.f1);
		return true;
		}	
	} sMaker2;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZNet_TCP_Socket

int ZNet_TCP_Socket::sListen(ip_addr iLocalAddress, ip_port iLocalPort)
	{
	int theSocketFD = ::socket(PF_INET, SOCK_STREAM, 0);
	if (theSocketFD < 0)
		{
		int err = errno;
		throw ZNetEx(ZNet_Socket::sTranslateError(err));
		}

	// Enable SO_REUSEADDR, cause it's a real pain waiting for TIME_WAIT to expire
	int reuseAddrFlag = 1;
	::setsockopt(
		theSocketFD, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddrFlag, sizeof(reuseAddrFlag));

	sockaddr_in localSockAddr;
	ZMemZero_T(localSockAddr);
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

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Internet_Socket

ZNetNameLookup_Internet_Socket::ZNetNameLookup_Internet_Socket(
	const string& iName, ip_port iPort, size_t iMaxAddresses)
:	fName(iName),
	fPort(iPort),
	fStarted(false),
	fCurrentIndex(0),
	fCountAddressesToReturn(iMaxAddresses)
	{
	if (iName.empty())
		{
		fStarted = true;
		fCountAddressesToReturn = 0;
		}
	else
		{
		in_addr theInAddr;
		if (0 != ::inet_aton(fName.c_str(), &theInAddr))
			{
			fStarted = true;
			fAddresses.push_back(ntohl(theInAddr.s_addr));
			}
		}
	}

ZNetNameLookup_Internet_Socket::~ZNetNameLookup_Internet_Socket()
	{}

void ZNetNameLookup_Internet_Socket::Start()
	{
	if (fStarted)
		return;

	fStarted = true;

#if __MACH__
	// FreeBSD uses thread-specific data to make gethostbyname reentrant.
	if (hostent* theHostEnt = ::gethostbyname(fName.c_str()))
		{
		char** listPtr = theHostEnt->h_addr_list;
		while (*listPtr && fCountAddressesToReturn)
			{
			in_addr* in_addrPtr = reinterpret_cast<in_addr*>(*listPtr);
			fAddresses.push_back(ntohl(in_addrPtr->s_addr));
			++listPtr;
			--fCountAddressesToReturn;
			}
		}
#else
	// This is the thread-safe version
	char buffer[4096];
	hostent resultBuf;
	hostent* result;
	int herrno;

#if __sun__
	result = ::gethostbyname_r(fName.c_str(), &resultBuf, buffer, 4096, &herrno);
#else
	if (0 != ::gethostbyname_r(fName.c_str(), &resultBuf, buffer, 4096, &result, &herrno))
		result = nullptr;
#endif

	if (result)
		{
		char** listPtr = result->h_addr_list;
		while (*listPtr && fCountAddressesToReturn)
			{
			in_addr* in_addrPtr = reinterpret_cast<in_addr*>(*listPtr);
			fAddresses.push_back(ntohl(in_addrPtr->s_addr));
			++listPtr;
			--fCountAddressesToReturn;
			}
		}
#endif
	}

bool ZNetNameLookup_Internet_Socket::Finished()
	{
	if (!fStarted)
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
		return new ZNetAddress_Internet(fAddresses[fCurrentIndex], fPort);

	return ZRef<ZNetAddress>();
	}

ZRef<ZNetName> ZNetNameLookup_Internet_Socket::CurrentName()
	{ return new ZNetName_Internet(fName, fPort); }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP_Socket

ZRef<ZNetListener_TCP_Socket> ZNetListener_TCP_Socket::sCreateWithFD(
	int iFD, size_t iListenQueueSize)
	{
	try
		{
		return new ZNetListener_TCP_Socket(iFD, iListenQueueSize, true);
		}
	catch (...)
		{}
	return ZRef<ZNetListener_TCP_Socket>();
	}

static int spEnsureInet(int iSocketFD)
	{
	sockaddr_in localSockAddr;
	socklen_t length = sizeof(localSockAddr);
	if (::getsockname(iSocketFD, (sockaddr*)&localSockAddr, &length) >= 0)
		{
		if (localSockAddr.sin_family == AF_INET)
			return iSocketFD;
		}
	throw std::runtime_error("ZNetListener_TCP_Socket, not an inet socket");
	}

ZNetListener_TCP_Socket::ZNetListener_TCP_Socket(
	int iSocketFD, size_t iListenQueueSize, bool iKnowWhatImDoing)
:	ZNetListener_Socket(spEnsureInet(iSocketFD), iListenQueueSize)
	{
	// We could ensure that iSocketFD
	ZAssert(iKnowWhatImDoing);
	}

ZNetListener_TCP_Socket::ZNetListener_TCP_Socket(ip_port iLocalPort, size_t iListenQueueSize)
:	ZNetListener_Socket(ZNet_TCP_Socket::sListen(0, iLocalPort), iListenQueueSize)
	{}

ZNetListener_TCP_Socket::ZNetListener_TCP_Socket(
	ip_addr iLocalAddress, ip_port iLocalPort, size_t iListenQueueSize)
:	ZNetListener_Socket(ZNet_TCP_Socket::sListen(iLocalAddress, iLocalPort), iListenQueueSize)
	{}

ZNetListener_TCP_Socket::~ZNetListener_TCP_Socket()
	{}

ip_port ZNetListener_TCP_Socket::GetPort()
	{
	sockaddr_in localSockAddr;
	socklen_t length = sizeof(localSockAddr);
	if (::getsockname(this->GetSocketFD(), (sockaddr*)&localSockAddr, &length) >= 0)
		return ntohs(localSockAddr.sin_port);
	return 0;
	}

ZRef<ZNetEndpoint> ZNetListener_TCP_Socket::Imp_MakeEndpoint(int iSocketFD)
	{
	return new ZNetEndpoint_TCP_Socket(iSocketFD);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP_Socket

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

ZNetEndpoint_TCP_Socket::ZNetEndpoint_TCP_Socket(int iSocketFD)
:	ZNetEndpoint_Socket(iSocketFD)
	{
	spSetSocketOptions(this->GetSocketFD());
	}

static int spConnect(ip_addr iRemoteHost, ip_port iRemotePort)
	{
	int socketFD = ::socket(PF_INET, SOCK_STREAM, 0);
	if (socketFD < 0)
		throw ZNetEx(ZNet_Socket::sTranslateError(errno));

	sockaddr_in remoteSockAddr;
	ZMemZero_T(remoteSockAddr);
	remoteSockAddr.sin_family = AF_INET;
	remoteSockAddr.sin_port = htons(iRemotePort);
	remoteSockAddr.sin_addr.s_addr = htonl(iRemoteHost);
	if (::connect(socketFD, (sockaddr*)&remoteSockAddr, sizeof(remoteSockAddr)) < 0)
		{
		::close(socketFD);
		throw ZNetEx(ZNet_Socket::sTranslateError(errno));
		}
	return socketFD;
	}

ZNetEndpoint_TCP_Socket::ZNetEndpoint_TCP_Socket(ip_addr iRemoteHost, ip_port iRemotePort)
:	ZNetEndpoint_Socket(spConnect(iRemoteHost, iRemotePort))
	{
	spSetSocketOptions(this->GetSocketFD());
	}

ZNetEndpoint_TCP_Socket::~ZNetEndpoint_TCP_Socket()
	{}

ZRef<ZNetAddress> ZNetEndpoint_TCP_Socket::GetLocalAddress()
	{
	sockaddr_in localSockAddr;
	socklen_t length = sizeof(localSockAddr);
	if (::getsockname(this->GetSocketFD(), (sockaddr*)&localSockAddr, &length) >= 0)
		{
		if (localSockAddr.sin_family == AF_INET)
			{
			return new ZNetAddress_Internet(
				ntohl(localSockAddr.sin_addr.s_addr), ntohs(localSockAddr.sin_port));
			}
		}

	return ZRef<ZNetAddress>();
	}

ZRef<ZNetAddress> ZNetEndpoint_TCP_Socket::GetRemoteAddress()
	{
	sockaddr_in remoteSockAddr;
	socklen_t length = sizeof(remoteSockAddr);
	if (::getpeername(this->GetSocketFD(), (sockaddr*)&remoteSockAddr, &length) >= 0)
		{
		if (remoteSockAddr.sin_family == AF_INET)
			{
			return new ZNetAddress_Internet(
				ntohl(remoteSockAddr.sin_addr.s_addr), ntohs(remoteSockAddr.sin_port));
			}
		}

	return ZRef<ZNetAddress>();
	}

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Internet_Socket)
