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

// FD_SETSIZE is defined to be 64 if no definition is
// already in place, so we provide a more sensible size.
#define FD_SETSIZE 1024

#include "zoolib/ZNet_Internet_WinSock.h"

#if ZCONFIG_API_Enabled(Net_Internet_WinSock)

ZMACRO_MSVCStaticLib_cpp(Net_Internet_WinSock)

#include "zoolib/ZCompat_cmath.h"
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZTime.h"

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
		oResult = new ZNetNameLookup_Internet_WinSock(iParam.f0, iParam.f1, iParam.f2);
		return true;
		}
	} sMaker0;


class Make_Listener
:	public ZFunctionChain_T<ZRef<ZNetListener_TCP>, ZNetListener_TCP::MakeParam4_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetListener_TCP_WinSock(iParam.f0, iParam.f1);
		return true;
		}
	} sMaker1;


class Make_Endpoint
:	public ZFunctionChain_T<ZRef<ZNetEndpoint_TCP>, ZNetEndpoint_TCP::MakeParam4_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetEndpoint_TCP_WinSock(iParam.f0, iParam.f1);
		return true;
		}
	} sMaker2;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * InitializeWinSock

namespace { // anonymous

class InitializeWinSock
	{
public:
	InitializeWinSock()
		{
		// Low order byte is major version number, high order byte is minor version number.
		int result = ::WSAStartup(0x0002, &fWSADATA);
		if (result)
			ZDebugLogf(2, ("WSAStartup got error %d", GetLastError()));
		}

	~InitializeWinSock()
		{ ::WSACleanup(); }

private:
	WSADATA fWSADATA;
	} sInitializeWinSock;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZNet_Internet_WinSock

ZNet::Error ZNet_Internet_WinSock::sTranslateError(int inNativeError)
	{
	// Because WSAGetLastError is not thread safe, we're not guaranteed
	// to get passed the appropriate value. In particular, we may get
	// given a zero error. Hence, we just bail for now and return
	// ZNet::errorGeneric in all cases (we won't get called unless
	// an error was signalled in some other fashion).
	ZNet::Error theError = ZNet::errorGeneric;
/*	switch (nativeError)
		{
		case 0: theError = ZNet::errorNone; break;
		default: break;
		}*/
	return theError;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Internet_WinSock

ZNetNameLookup_Internet_WinSock::ZNetNameLookup_Internet_WinSock
	(const string& inName, ip_port inPort, size_t inMaxAddresses)
:	fName(inName),
	fPort(inPort),
	fStarted(false),
	fCurrentIndex(0),
	fCountAddressesToReturn(inMaxAddresses)
	{
	if (inName.empty())
		{
		fStarted = true;
		fCountAddressesToReturn = 0;
		}
	else
		{
		ip4_addr theIPAddr = ::inet_addr(inName.c_str());
		if (theIPAddr != INADDR_NONE)
			{
			fStarted = true;
			fAddresses.push_back(ntohl(theIPAddr));
			}
		}
	}

ZNetNameLookup_Internet_WinSock::~ZNetNameLookup_Internet_WinSock()
	{}

void ZNetNameLookup_Internet_WinSock::Start()
	{
	if (fStarted)
		return;

	fStarted = true;

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
	else
		{
		int err = ::WSAGetLastError();
		ZDebugLogf(2, ("%d", err));
		}
	}

bool ZNetNameLookup_Internet_WinSock::Finished()
	{
	if (!fStarted)
		return true;

	return fCurrentIndex >= fAddresses.size();
	}

void ZNetNameLookup_Internet_WinSock::Advance()
	{
	ZAssertStop(2, fStarted);
	if (fCurrentIndex < fAddresses.size())
		++fCurrentIndex;
	}

ZRef<ZNetAddress> ZNetNameLookup_Internet_WinSock::CurrentAddress()
	{
	if (fCurrentIndex < fAddresses.size())
		return new ZNetAddress_IP4(fAddresses[fCurrentIndex], fPort);

	return null;
	}

ZRef<ZNetName> ZNetNameLookup_Internet_WinSock::CurrentName()
	{ return new ZNetName_Internet(fName, fPort); }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_TCP_WinSock

ZNetListener_TCP_WinSock::ZNetListener_TCP_WinSock(ip_port iLocalPort)
	{
	this->pInit(INADDR_ANY, iLocalPort);
	}

ZNetListener_TCP_WinSock::ZNetListener_TCP_WinSock
	(ip4_addr iLocalAddress, ip_port iLocalPort)
	{
	this->pInit(iLocalAddress, iLocalPort);
	}

ZNetListener_TCP_WinSock::~ZNetListener_TCP_WinSock()
	{
	::closesocket(fSOCKET);
	}

ZRef<ZNetAddress> ZNetListener_TCP_WinSock::GetAddress()
	{
	sockaddr_in localSockAddr;
	int length = sizeof(localSockAddr);
	if (::getsockname(fSOCKET, (sockaddr*)&localSockAddr, &length) >= 0)
		{
		if (localSockAddr.sin_family == AF_INET)
			{
			return new ZNetAddress_IP4
				(ntohl(localSockAddr.sin_addr.s_addr), ntohs(localSockAddr.sin_port));
			}
		}

	return null;
	}

ZRef<ZNetEndpoint> ZNetListener_TCP_WinSock::Listen()
	{
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(fSOCKET, &readSet);
	struct timeval timeOut;
	timeOut.tv_sec = 1;
	timeOut.tv_usec = 0;
	int result = ::select(0, &readSet, nullptr, nullptr, &timeOut);
	if (result > 0)
		{
		// We got one.
		sockaddr_in remoteSockAddr;
		ZMemZero_T(remoteSockAddr);
		int addrSize = sizeof(remoteSockAddr);
		SOCKET newSOCKET = ::accept(fSOCKET, (sockaddr*)&remoteSockAddr, &addrSize);
		if (newSOCKET != INVALID_SOCKET)
			{
			// Ensure new socket is blocking.
			unsigned long param = 0;
			if (::ioctlsocket(newSOCKET, FIONBIO, &param) == SOCKET_ERROR)
				::closesocket(newSOCKET);
			else
				return new ZNetEndpoint_TCP_WinSock(newSOCKET);
			}
		}
	return null;
	}

void ZNetListener_TCP_WinSock::CancelListen()
	{
	// It's hard to wake up a blocked call to accept, which is why we use a non-blocking
	// socket and a one second timeout on select. Rather than doing a full-blown
	// implementation here I'm just going to take advantage of the fact that callers are
	// supposed to retry their call to Listen if they get a nil endpoint back and have
	// CancelListen do nothing at all -- any pending call to Listen will return in at
	// most one second anyway.
	}

ip_port ZNetListener_TCP_WinSock::GetPort()
	{
	sockaddr_in localSockAddr;
	int length = sizeof(localSockAddr);
	if (::getsockname(fSOCKET, (sockaddr*)&localSockAddr, &length) >= 0)
		return ntohs(localSockAddr.sin_port);
	return 0;
	}

void ZNetListener_TCP_WinSock::pInit
	(ip4_addr iLocalAddress, ip_port iLocalPort)
	{
	fSOCKET = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fSOCKET == INVALID_SOCKET)
		{
		int err = ::WSAGetLastError();
		throw ZNetEx(sTranslateError(err));
		}

	int reuseAddrFlag = 1;
	::setsockopt(fSOCKET, IPPROTO_TCP, SO_REUSEADDR, (char*)&reuseAddrFlag, sizeof(reuseAddrFlag));

	sockaddr_in localSockAddr;
	ZMemZero_T(localSockAddr);
	localSockAddr.sin_family = AF_INET;
	localSockAddr.sin_port = htons(iLocalPort);
	localSockAddr.sin_addr.s_addr = htonl(iLocalAddress);

	if (::bind(fSOCKET, (struct sockaddr*)&localSockAddr, sizeof(localSockAddr)))
		{
		int err = ::WSAGetLastError();
		::closesocket(fSOCKET);
		throw ZNetEx(sTranslateError(err));
		}

	if (::listen(fSOCKET, 5))
		{
		int err = ::WSAGetLastError();
		::closesocket(fSOCKET);
		throw ZNetEx(sTranslateError(err));
		}

	// Set the socket to be non-blocking
	unsigned long param = 1;
	if (::ioctlsocket(fSOCKET, FIONBIO, &param))
		{
		int err = ::WSAGetLastError();
		::closesocket(fSOCKET);
		throw ZNetEx(sTranslateError(err));
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_TCP_WinSock

ZNetEndpoint_TCP_WinSock::ZNetEndpoint_TCP_WinSock(SOCKET iSOCKET)
	{
	// Assumes that the socket is already connected
	fSOCKET = iSOCKET;
	// Make sure that Nagle algorithm is enabled
	int noDelayFlag = 0;
	::setsockopt(fSOCKET, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelayFlag, sizeof(noDelayFlag));
	}

ZNetEndpoint_TCP_WinSock::ZNetEndpoint_TCP_WinSock(ip4_addr iRemoteHost, ip_port iRemotePort)
	{
	fSOCKET = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fSOCKET == INVALID_SOCKET)
		{
		int err = ::WSAGetLastError();
		throw ZNetEx(sTranslateError(err));
		}

	sockaddr_in remoteSockAddr;
	ZMemZero_T(remoteSockAddr);
	remoteSockAddr.sin_family = AF_INET;
	remoteSockAddr.sin_port = htons(iRemotePort);
	remoteSockAddr.sin_addr.s_addr = htonl(iRemoteHost);
	if (::connect(fSOCKET, (sockaddr*)&remoteSockAddr, sizeof(remoteSockAddr)) < 0)
		{
		int err = ::WSAGetLastError();
		::closesocket(fSOCKET);
		throw ZNetEx(sTranslateError(err));
		}
	}

ZNetEndpoint_TCP_WinSock::~ZNetEndpoint_TCP_WinSock()
	{ ::closesocket(fSOCKET); }

const ZStreamRCon& ZNetEndpoint_TCP_WinSock::GetStreamRCon()
	{ return *this; }

const ZStreamWCon& ZNetEndpoint_TCP_WinSock::GetStreamWCon()
	{ return *this; }

ZRef<ZNetAddress> ZNetEndpoint_TCP_WinSock::GetLocalAddress()
	{
	sockaddr_in localSockAddr;
	int length = sizeof(localSockAddr);
	if (::getsockname(fSOCKET, (sockaddr*)&localSockAddr, &length) >= 0)
		{
		if (localSockAddr.sin_family == AF_INET)
			{
			return new ZNetAddress_IP4
				(ntohl(localSockAddr.sin_addr.s_addr), ntohs(localSockAddr.sin_port));
			}
		}

	return null;
	}

ZRef<ZNetAddress> ZNetEndpoint_TCP_WinSock::GetRemoteAddress()
	{
	sockaddr_in remoteSockAddr;
	int length = sizeof(remoteSockAddr);
	if (::getpeername(fSOCKET, (sockaddr*)&remoteSockAddr, &length) >= 0)
		{
		return new ZNetAddress_IP4
			(ntohl(remoteSockAddr.sin_addr.s_addr), ntohs(remoteSockAddr.sin_port));
		}

	return null;
	}

void ZNetEndpoint_TCP_WinSock::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = static_cast<char*>(oDest);
	if (iCount)
		{
		int result = ::recv(fSOCKET, localDest, iCount, 0);
		if (result > 0)
			{
			localDest += result;
			iCount -= result;
			}
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<char*>(oDest);
	}

size_t ZNetEndpoint_TCP_WinSock::Imp_CountReadable()
	{
	// Use non-blocking select to see if a read would succeed
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(fSOCKET, &readSet);
	struct timeval timeOut;
	timeOut.tv_sec = 0;
	timeOut.tv_usec = 0;
	int result = ::select(0, &readSet, nullptr, nullptr, &timeOut);
	if (result <= 0)
		return 0;

	// FIONREAD will give us a result without hanging.
	unsigned long localResult;
	if (::ioctlsocket(fSOCKET, FIONREAD, &localResult) == SOCKET_ERROR)
		return 0;
	return localResult;
	}

static bool spWaitReadable(SOCKET iSOCKET, double iTimeout)
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

bool ZNetEndpoint_TCP_WinSock::Imp_WaitReadable(double iTimeout)
	{ return spWaitReadable(fSOCKET, iTimeout); }

void ZNetEndpoint_TCP_WinSock::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const char* localSource = static_cast<const char*>(iSource);
	while (iCount)
		{
		int result = ::send(fSOCKET, localSource, iCount, 0);
		if (result > 0)
			{
			localSource += result;
			iCount -= result;
			}
		else
			{
			break;
			}
		}
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const char*>(iSource);
	}

bool ZNetEndpoint_TCP_WinSock::Imp_ReceiveDisconnect(double iTimeout)
	{
	ZTime endTime = ZTime::sSystem() + iTimeout;

	bool gotIt = false;
	for (;;)
		{
		int result = ::recv(fSOCKET, sGarbageBuffer, sizeof(sGarbageBuffer), 0);
		if (result == 0)
			{
			// result is zero, indicating that the other end has sent FIN.
			gotIt = true;
			break;
			}
		else if (result < 0)
			{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
				{
				if (iTimeout < 0)
					{
					spWaitReadable(fSOCKET, 60);
					}
				else
					{
					ZTime now = ZTime::sSystem();
					if (endTime < now)
						break;
					spWaitReadable(fSOCKET, endTime - now);
					}
				}
			else
				{
				break;
				}
			}
		}
	return gotIt;
	}

void ZNetEndpoint_TCP_WinSock::Imp_SendDisconnect()
	{
	// Graceful close. See "Windows Sockets Network Programming"
	// pp 231-232, Quinn, Shute. Addison Wesley. ISBN 0-201-63372-8
	::shutdown(fSOCKET, 1);
	}

void ZNetEndpoint_TCP_WinSock::Imp_Abort()
	{
	struct linger theLinger;
	theLinger.l_onoff = 1;
	theLinger.l_linger = 1;
	::setsockopt(fSOCKET, SOL_SOCKET, SO_LINGER, (char*)&theLinger, sizeof(theLinger));
	::shutdown(fSOCKET, 2);
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Internet_WinSock)
