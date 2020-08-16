// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZStreamRWCon_SSL_Apple_h__
#define __ZStreamRWCon_SSL_Apple_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZStreamer.h"

#if ZCONFIG_SPI_Enabled(iPhone)\
	or (defined(MAC_OS_X_VERSION_10_8) and MAC_OS_X_VERSION_10_8 <= MAC_OS_X_VERSION_MIN_REQUIRED)

#include "zoolib/FunctionChain.h"

ZMACRO_MSVCStaticLib_Reference(StreamRWCon_SSL_Apple)

#include <Security/SecureTransport.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStreamRWCon_SSL_Apple

class ZStreamRWCon_SSL_Apple
:	public ZStreamRCon
,	public ZStreamWCon
	{
public:
	ZStreamRWCon_SSL_Apple(const ZStreamR& iStreamR, const ZStreamW& iStreamW, bool iIsServer);
	~ZStreamRWCon_SSL_Apple();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(double iTimeout);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	OSStatus pRead(void* oDest, size_t* ioCount);
	static OSStatus spRead(SSLConnectionRef iRefcon, void* oDest, size_t* ioCount);

	OSStatus pWrite(const void* iSource, size_t* ioCount);
	static OSStatus spWrite(SSLConnectionRef iRefcon, const void* iSource, size_t* ioCount);

	const ZStreamR& fStreamR;
	const ZStreamW& fStreamW;
	SSLContextRef fSSLCR;
	bool fLastWasWrite;
	};

// =================================================================================================
#pragma mark - ZStreamerRWCon_SSL_Apple

class ZStreamerRWCon_SSL_Apple
:	public ZStreamerRWCon
	{
public:
	ZStreamerRWCon_SSL_Apple(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW, bool iIsServer);
	virtual ~ZStreamerRWCon_SSL_Apple();

// From ZStreamerRCon via ZStreamerRWCon_SSL
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZStreamerRWCon_SSL
	virtual const ZStreamWCon& GetStreamWCon();

// Our protocol
	ZStreamRWCon_SSL_Apple& GetStream();

private:
	ZRef<ZStreamerR> fStreamerR;
	ZRef<ZStreamerW> fStreamerW;
	ZStreamRWCon_SSL_Apple fStream;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(iPhone)\
	or (defined(MAC_OS_X_VERSION_10_8) and MAC_OS_X_VERSION_10_8 <= MAC_OS_X_VERSION_MIN_REQUIRED)

#endif // __ZStreamRWCon_SSL_Apple_h__
