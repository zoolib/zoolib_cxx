// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_POSIX_Net_Socket_h__
#define __ZooLib_POSIX_Net_Socket_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Net_Socket
	#define ZCONFIG_API_Avail__Net_Socket ZCONFIG_SPI_Enabled(POSIX)
#endif

#ifndef ZCONFIG_API_Desired__Net_Socket
	#define ZCONFIG_API_Desired__Net_Socket 1
#endif

#include "zoolib/Net.h"

#if ZCONFIG_API_Enabled(Net_Socket)

#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Net_Socket

class Net_Socket
	{
public:
	static ssize_t sSend(int iSocket, const char* iSource, size_t iCount);
	static ssize_t sReceive(int iSocket, char* oDest, size_t iCount);
//	static bool sWaitReadable(int iSocket, double iTimeout);
//	static void sWaitWriteable(int iSocket);
	static Net::Error sTranslateError(int iNativeError);
	};

// =================================================================================================
#pragma mark - NetListener_Socket

class NetListener_Socket
:	public virtual NetListener
	{
protected:
	NetListener_Socket(int iSocketFD);
	virtual ~NetListener_Socket();

// From NetListener
	virtual ZP<ChannerRWCon_Bin> Listen();

// From Cancellable via NetListener
	virtual void Cancel();

// Our protocol
	int GetSocketFD();

	static void sEnableFastCancellation();

protected:
	ZMtx fMtx;
	ZCnd fCnd;
	int fSocketFD;
	ZThread::ID fThreadID_Listening;
	};

ZP<NetListener_Socket> sNetListener_Socket(int iFD);

// =================================================================================================
#pragma mark - NetEndpoint_Socket

class NetEndpoint_Socket
:	public ChannerRWCon_Bin
	{
public:
	NetEndpoint_Socket(int iSocketFD);
	virtual ~NetEndpoint_Socket();

// From ChanAspect_Abort
	virtual void Abort();

// From ChanAspect_DisconnectRead
	virtual bool DisconnectRead(double iTimeout);

// From ChanAspect_DisconnectWrite
	virtual void DisconnectWrite();

// From ChanR_Bin
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);

// From ChanAspect_WaitReadable
	virtual bool WaitReadable(double iTimeout);

// Our protocol
	int GetSocketFD();

private:
	int fSocketFD;
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Socket)

#endif // __ZooLib_POSIX_Net_Socket_h__
