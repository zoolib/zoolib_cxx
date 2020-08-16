// Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZNet_Local_Socket_h__
#define __ZNet_Local_Socket_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZNet_Local.h"
#include "zoolib/ZNet_Socket.h"

#ifndef ZCONFIG_API_Avail__Net_Local_Socket
	#if ZCONFIG_API_Enabled(Net_Socket)
		#define ZCONFIG_API_Avail__Net_Local_Socket 1
	#endif
#endif

#ifndef ZCONFIG_API_Avail__Net_Local_Socket
	#define ZCONFIG_API_Avail__Net_Local_Socket 0
#endif

#ifndef ZCONFIG_API_Desired__Net_Local_Socket
	#define ZCONFIG_API_Desired__Net_Local_Socket 1
#endif

#if ZCONFIG_API_Enabled(Net_Local_Socket)

ZMACRO_MSVCStaticLib_Reference(Net_Local_Socket)

namespace ZooLib {

// =================================================================================================
#pragma mark - ZNetNameLookup_Local_Socket

class ZNetNameLookup_Local_Socket : public ZNetNameLookup
	{
public:
	ZNetNameLookup_Local_Socket(const std::string& iPath);
	virtual ~ZNetNameLookup_Local_Socket();

// From ZNetNameLookup
	virtual void Start();
	virtual bool Finished();
	virtual void Advance();

	virtual ZRef<ZNetAddress> CurrentAddress();
	virtual ZRef<ZNetName> CurrentName();

protected:
	std::string fPath;
	bool fFinished;
	};

// =================================================================================================
#pragma mark - ZNetListener_Local_Socket

class ZNetListener_Local_Socket
:	public ZNetListener_Local,
	public ZNetListener_Socket
	{
	ZNetListener_Local_Socket(int iFD, const IKnowWhatIAmDoing_t&);
public:
	static ZRef<ZNetListener_Local_Socket> sCreateWithFD(int iFD);

	ZNetListener_Local_Socket(const std::string& iPath);
	virtual ~ZNetListener_Local_Socket();

// From ZNetListener_Socket
	virtual ZRef<ZNetAddress> GetAddress();
	virtual ZRef<ZNetEndpoint> Imp_MakeEndpoint(int iSocketFD);

private:
	std::string fPath;
	};

// =================================================================================================
#pragma mark - ZNetEndpoint_Local_Socket

class ZNetEndpoint_Local_Socket
:	public ZNetEndpoint_Local,
	public ZNetEndpoint_Socket
	{
public:
	ZNetEndpoint_Local_Socket(int iSocketFD);
	ZNetEndpoint_Local_Socket(const std::string& iRemotePath);

	virtual ~ZNetEndpoint_Local_Socket();

// From ZNetEndpoint via ZNetEndpoint_Socket
	virtual ZRef<ZNetAddress> GetRemoteAddress();
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Local_Socket)

#endif // __ZNet_Local_Socket_h__
