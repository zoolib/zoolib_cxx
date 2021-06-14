// Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_POSIX_Net_Local_Socket_h__
#define __ZooLib_POSIX_Net_Local_Socket_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/Net_Local.h"
#include "zoolib/POSIX/Net_Socket.h"

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
#pragma mark - NetNameLookup_Local_Socket

class NetNameLookup_Local_Socket : public NetNameLookup
	{
public:
	NetNameLookup_Local_Socket(const std::string& iPath);
	virtual ~NetNameLookup_Local_Socket();

// From NetNameLookup
	virtual void Start();
	virtual bool Finished();
	virtual void Advance();

	virtual ZP<NetAddress> CurrentAddress();
	virtual ZP<NetName> CurrentName();

protected:
	std::string fPath;
	bool fFinished;
	};

// =================================================================================================
#pragma mark - NetListener_Local_Socket

class NetListener_Local_Socket
:	public virtual NetListener_Local,
	public virtual NetListener_Socket
	{
	NetListener_Local_Socket(int iFD, const IKnowWhatIAmDoing_t&);
public:
	static ZP<NetListener_Local_Socket> sCreateWithFD(int iFD);

	NetListener_Local_Socket(const std::string& iPath);
	virtual ~NetListener_Local_Socket();

// From NetListener_Socket
	virtual ZP<NetAddress> GetAddress();

private:
	std::string fPath;
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Local_Socket)

#endif // __ZooLib_POSIX_Net_Local_Socket_h__
