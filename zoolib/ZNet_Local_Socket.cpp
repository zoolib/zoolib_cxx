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

#include "zoolib/ZNet_Local_Socket.h"

#if ZCONFIG_API_Enabled(Net_Local_Socket)

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZMemory.h"

#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h> // For chmod
#include <sys/un.h>

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

namespace ZANONYMOUS {

class Make_NameLookup
:	public ZFunctionChain_T<ZRef<ZNetNameLookup>, ZNetName_Local::LookupParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetNameLookup_Local_Socket(iParam);
		return true;
		}	
	} sMaker0;


class Make_Listener
:	public ZFunctionChain_T<ZRef<ZNetListener_Local>, ZNetListener_Local::MakeParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetListener_Local_Socket(iParam.f0, iParam.f1);
		return true;
		}	
	} sMaker1;


class Make_Endpoint
:	public ZFunctionChain_T<ZRef<ZNetEndpoint_Local>, ZNetEndpoint_Local::MakeParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZNetEndpoint_Local_Socket(iParam);
		return true;
		}	
	} sMaker2;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Local_Socket

ZNetNameLookup_Local_Socket::ZNetNameLookup_Local_Socket(const std::string& iPath)
:	fPath(iPath),
	fFinished(false)
	{
	if (fPath.empty())
		fFinished = true;
	}

ZNetNameLookup_Local_Socket::~ZNetNameLookup_Local_Socket()
	{}

void ZNetNameLookup_Local_Socket::Start()
	{}

bool ZNetNameLookup_Local_Socket::Finished()
	{ return fFinished; }

void ZNetNameLookup_Local_Socket::Advance()
	{
	ZAssertStop(2, !fFinished);
	fFinished = true;
	}

ZRef<ZNetAddress> ZNetNameLookup_Local_Socket::CurrentAddress()
	{
	if (!fFinished)
		return new ZNetAddress_Local(fPath);

	return ZRef<ZNetAddress>();
	}

ZRef<ZNetName> ZNetNameLookup_Local_Socket::CurrentName()
	{ return new ZNetName_Local(fPath); }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_Local_Socket

ZRef<ZNetListener_Local_Socket> ZNetListener_Local_Socket::sCreateWithFD(
	int iFD, size_t iListenQueueSize)
	{
	try
		{
		return new ZNetListener_Local_Socket(iFD, iListenQueueSize, true);
		}
	catch (...)
		{}
	return ZRef<ZNetListener_Local_Socket>();
	}

static int spEnsureLocal(int iSocketFD)
	{
	sockaddr_un localSockAddr;
	socklen_t length = sizeof(localSockAddr);
	if (::getsockname(iSocketFD, (sockaddr*)&localSockAddr, &length) >= 0)
		{
		if (localSockAddr.sun_family == AF_LOCAL)
			return iSocketFD;
		}
	throw std::runtime_error("ZNetListener_Local_Socket, not a local socket");
	}

ZNetListener_Local_Socket::ZNetListener_Local_Socket(
	int iSocketFD, size_t iListenQueueSize, bool iKnowWhatImDoing)
:	ZNetListener_Socket(spEnsureLocal(iSocketFD), iListenQueueSize)
	{
	ZAssert(iKnowWhatImDoing);
	}

static int spListen(const string& iPath)
	{
	sockaddr_un localSockAddr;
	ZBlockZero(&localSockAddr, sizeof(localSockAddr));

	if (iPath.empty() || iPath.length() >= sizeof(localSockAddr.sun_path))
		throw ZNetEx(ZNet::errorGeneric);

	int theSocketFD = ::socket(PF_LOCAL, SOCK_STREAM, 0);
	if (theSocketFD < 0)
		throw ZNetEx(ZNet_Socket::sTranslateError(errno));

	localSockAddr.sun_family = AF_LOCAL;
	strcpy(localSockAddr.sun_path, iPath.c_str());

	::unlink(localSockAddr.sun_path);

	if (::bind(theSocketFD, (sockaddr*)&localSockAddr, sizeof(localSockAddr)) < 0)
		{
		int err = errno;
		::close(theSocketFD);
		throw ZNetEx(ZNet_Socket::sTranslateError(err));
		}

	::chmod(localSockAddr.sun_path, 0666);

	return theSocketFD;
	}

ZNetListener_Local_Socket::ZNetListener_Local_Socket(
	const std::string& iPath, size_t iListenQueueSize)
:	ZNetListener_Socket(spListen(iPath), iListenQueueSize),
	fPath(iPath)
	{}

ZNetListener_Local_Socket::~ZNetListener_Local_Socket()
	{
	if (!fPath.empty())
		::unlink(fPath.c_str());
	}

ZRef<ZNetEndpoint> ZNetListener_Local_Socket::Imp_MakeEndpoint(int iSocketFD)
	{
	return new ZNetEndpoint_Local_Socket(iSocketFD);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_Local_Socket

static void spSetSocketOptions(int iSocketFD)
	{}

ZNetEndpoint_Local_Socket::ZNetEndpoint_Local_Socket(int iSocketFD)
:	ZNetEndpoint_Socket(iSocketFD)
	{
	spSetSocketOptions(this->GetSocketFD());
	}

static int spConnect(const string& iPath)
	{
	int socketFD = ::socket(PF_LOCAL, SOCK_STREAM, 0);
	if (socketFD < 0)
		throw ZNetEx(ZNet_Socket::sTranslateError(errno));

	sockaddr_un remoteSockAddr;
	ZBlockZero(&remoteSockAddr, sizeof(remoteSockAddr));
	remoteSockAddr.sun_family = AF_LOCAL;
	strcpy(remoteSockAddr.sun_path, iPath.c_str());

	if (::connect(socketFD, (sockaddr*)&remoteSockAddr, sizeof(remoteSockAddr)) < 0)
		{
		::close(socketFD);
		throw ZNetEx(ZNet_Socket::sTranslateError(errno));
		}
	return socketFD;
	}

ZNetEndpoint_Local_Socket::ZNetEndpoint_Local_Socket(const std::string& iRemotePath)
:	ZNetEndpoint_Socket(spConnect(iRemotePath))
	{
	spSetSocketOptions(this->GetSocketFD());
	}

ZNetEndpoint_Local_Socket::~ZNetEndpoint_Local_Socket()
	{}

ZRef<ZNetAddress> ZNetEndpoint_Local_Socket::GetRemoteAddress()
	{
	sockaddr_un remoteSockAddr;
	socklen_t length = sizeof(remoteSockAddr);
	if (::getpeername(this->GetSocketFD(), (sockaddr*)&remoteSockAddr, &length) >= 0)
		{
		if (remoteSockAddr.sun_family == AF_LOCAL)
			return new ZNetAddress_Local(remoteSockAddr.sun_path);
		}

	return ZRef<ZNetAddress>();
	}

// =================================================================================================

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Net_Local_Socket)
