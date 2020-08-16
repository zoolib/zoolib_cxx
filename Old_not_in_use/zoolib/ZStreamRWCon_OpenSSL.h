// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZStreamRWCon_OpenSSL_h__
#define __ZStreamRWCon_OpenSSL_h__ 1
#include "zconfig.h"
#include "ZConfig_SPI.h"

#include "zoolib/ZStreamer.h"

#include <openssl/bio.h>
#include <openssl/ssl.h>

ZMACRO_MSVCStaticLib_Reference(StreamRWCon_OpenSSL)

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStreamRWCon_OpenSSL

class ZStreamRWCon_OpenSSL
:	public ZStreamRCon
,	public ZStreamWCon
	{
public:
	ZStreamRWCon_OpenSSL(const ZStreamR& iStreamR, const ZStreamW& iStreamW, bool iIsServer);
	~ZStreamRWCon_OpenSSL();

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
	bool pHandleError(int iResult);

	int pRead(void* oDest, int iCount);
	static int spRead(BIO* iBIO, char* oDest, int iCount);

	int pWrite(const void* iSource, int iCount);
	static int spWrite(BIO* iBIO, const char* iSource, int iCount);

	SSL_CTX* fSSL_CTX;
	SSL* fSSL;
	BIO* fBIOR;
	BIO* fBIOW;
	const ZStreamR& fStreamR;
	const ZStreamW& fStreamW;
	bool fLastWasWrite;

	};

// =================================================================================================
#pragma mark - ZStreamRWCon_OpenSSL

class ZStreamerRWCon_OpenSSL
:	public ZStreamerRWCon
	{
public:
	ZStreamerRWCon_OpenSSL(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW, bool iIsServer);
	virtual ~ZStreamerRWCon_OpenSSL();

// From ZStreamerRCon via ZStreamerRWCon_SSL
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZStreamerRWCon_SSL
	virtual const ZStreamWCon& GetStreamWCon();

// Our protocol
	ZStreamRWCon_OpenSSL& GetStream();

private:
	ZRef<ZStreamerR> fStreamerR;
	ZRef<ZStreamerW> fStreamerW;
	ZStreamRWCon_OpenSSL fStream;
	};

} // namespace ZooLib

#endif // __ZStreamRWCon_OpenSSL_h__
