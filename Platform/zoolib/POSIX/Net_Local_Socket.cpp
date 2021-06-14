// Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/POSIX/Net_Local_Socket.h"

#if ZCONFIG_API_Enabled(Net_Local_Socket)

#include "zoolib/Memory.h"

#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h> // For chmod
#include <sys/un.h>
#include <unistd.h>

#if not defined(SOCK_MAXADDRLEN)
	#define SOCK_MAXADDRLEN (255)
#endif

ZMACRO_MSVCStaticLib_cpp(Net_Local_Socket)

namespace ZooLib {

using std::string;

static void spSetSocketOptions(int iSocketFD)
	{}

static int spConnect(const string& iPath)
	{
	int socketFD = ::socket(PF_LOCAL, SOCK_STREAM, 0);
	if (socketFD < 0)
		throw NetEx(Net_Socket::sTranslateError(errno));

	sockaddr_un remoteSockAddr = {};
	#if ZCONFIG_SPI_Enabled(BSD)
		remoteSockAddr.sun_len = sizeof(remoteSockAddr);
	#endif
	remoteSockAddr.sun_family = AF_LOCAL;
	strcpy(remoteSockAddr.sun_path, iPath.c_str());

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
:	public FunctionChain<ZP<NetNameLookup>, NetName_Local::LookupParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new NetNameLookup_Local_Socket(iParam);
		return true;
		}
	} sMaker0;

class Make_Listener
:	public FunctionChain<ZP<NetListener_Local>, std::string>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new NetListener_Local_Socket(iParam);
		return true;
		}
	} sMaker1;

class Make_Endpoint
:	public FunctionChain<ZP<ChannerRWClose_Bin>, std::string>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		const int theSocketFD = spConnect(iParam);
		spSetSocketOptions(theSocketFD);
		ZP<NetEndpoint_Socket> theEP = new NetEndpoint_Socket(theSocketFD);
		oResult = theEP;
		return true;
		}
	} sMaker2;

} // anonymous namespace

// =================================================================================================
#pragma mark - Helpers

static ZP<NetAddress_Local> spAsNetAddress(const sockaddr* iSockAddr)
	{
	const sa_family_t theFamily = ((sockaddr*)iSockAddr)->sa_family;

	if (theFamily == AF_LOCAL)
		{
		const sockaddr_un* local = (const sockaddr_un*)iSockAddr;
		if (local->sun_path)
			return new NetAddress_Local(local->sun_path);
		}

	return null;
	}

// =================================================================================================
#pragma mark - NetNameLookup_Local_Socket

NetNameLookup_Local_Socket::NetNameLookup_Local_Socket(const std::string& iPath)
:	fPath(iPath)
,	fFinished(false)
	{
	if (fPath.empty())
		fFinished = true;
	}

NetNameLookup_Local_Socket::~NetNameLookup_Local_Socket()
	{}

void NetNameLookup_Local_Socket::Start()
	{}

bool NetNameLookup_Local_Socket::Finished()
	{ return fFinished; }

void NetNameLookup_Local_Socket::Advance()
	{
	ZAssertStop(2, not fFinished);
	fFinished = true;
	}

ZP<NetAddress> NetNameLookup_Local_Socket::CurrentAddress()
	{
	if (not fFinished)
		return new NetAddress_Local(fPath);

	return null;
	}

ZP<NetName> NetNameLookup_Local_Socket::CurrentName()
	{ return new NetName_Local(fPath); }

// =================================================================================================
#pragma mark - NetListener_Local_Socket

ZP<NetListener_Local_Socket> NetListener_Local_Socket::sCreateWithFD(int iFD)
	{
	try
		{
		return new NetListener_Local_Socket(iFD, IKnowWhatIAmDoing);
		}
	catch (...)
		{}
	return null;
	}

static int spEnsureLocal(int iSocketFD)
	{
	sockaddr_un localSockAddr;
	if (0 <= ::getsockname(
		iSocketFD, (sockaddr*)&localSockAddr, sMutablePtr(socklen_t(sizeof(localSockAddr)))))
		{
		if (localSockAddr.sun_family == AF_LOCAL)
			return iSocketFD;
		}
	throw std::runtime_error("NetListener_Local_Socket, not a local socket");
	}

NetListener_Local_Socket::NetListener_Local_Socket(int iSocketFD, const IKnowWhatIAmDoing_t&)
:	NetListener_Socket(spEnsureLocal(iSocketFD))
	{}

static int spListen(const string& iPath)
	{
	sockaddr_un localSockAddr = {};
	#if ZCONFIG_SPI_Enabled(BSD)
		localSockAddr.sun_len = sizeof(sockaddr_un);
	#endif

	if (iPath.empty() || iPath.length() >= sizeof(localSockAddr.sun_path))
		throw NetEx(Net::errorGeneric);

	int theSocketFD = ::socket(PF_LOCAL, SOCK_STREAM, 0);
	if (theSocketFD < 0)
		throw NetEx(Net_Socket::sTranslateError(errno));

	localSockAddr.sun_family = AF_LOCAL;
	strcpy(localSockAddr.sun_path, iPath.c_str());

	::unlink(localSockAddr.sun_path);

	if (::bind(theSocketFD, (sockaddr*)&localSockAddr, sizeof(localSockAddr)) < 0)
		{
		int err = errno;
		::close(theSocketFD);
		throw NetEx(Net_Socket::sTranslateError(err));
		}

	::chmod(localSockAddr.sun_path, 0666);

	return theSocketFD;
	}

NetListener_Local_Socket::NetListener_Local_Socket(const std::string& iPath)
:	NetListener_Socket(spListen(iPath)),
	fPath(iPath)
	{}

NetListener_Local_Socket::~NetListener_Local_Socket()
	{
	if (not fPath.empty())
		::unlink(fPath.c_str());
	}

ZP<NetAddress> NetListener_Local_Socket::GetAddress()
	{
	uint8 buffer[SOCK_MAXADDRLEN];
	if (0 <= ::getsockname(
		this->GetSocketFD(), (sockaddr*)buffer, sMutablePtr(socklen_t(sizeof(buffer)))))
		{ return spAsNetAddress((sockaddr*)buffer); }

	return null;
	}

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Local_Socket)
