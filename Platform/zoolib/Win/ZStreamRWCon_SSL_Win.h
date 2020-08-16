// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZStreamRWCon_SSL_Win_h__
#define __ZStreamRWCon_SSL_Win_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZStreamer.h"

#if ZCONFIG_SPI_Enabled(Win)

ZMACRO_MSVCStaticLib_Reference(StreamRWCon_SSL_Win)

#include "zoolib/ZCompat_Win.h"

#include <vector>
#include <deque>

#define SECURITY_WIN32 1
#include <Security.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStreamRWCon_SSL_Win

class ZStreamRWCon_SSL_Win
:	public ZStreamRCon
,	public ZStreamWCon
	{
public:
	ZStreamRWCon_SSL_Win(const ZStreamR& iStreamR, const ZStreamW& iStreamW);
	~ZStreamRWCon_SSL_Win();

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
	bool pConnect();
	bool pHandshake();

	ZMtx fMtx_R;
	ZMtx fMtx_W;

	const ZStreamR& fStreamR;
	const ZStreamW& fStreamW;
	CredHandle fCredHandle;
	CtxtHandle fCtxtHandle;
	bool fSendOpen;
	bool fReceiveOpen;
	std::vector<char> fBufferEnc;
	std::deque<char> fBufferPlain;
	};

// =================================================================================================
#pragma mark - ZStreamRWCon_SSL_Win

class ZStreamerRWCon_SSL_Win
:	public ZStreamerRWCon
	{
public:
	ZStreamerRWCon_SSL_Win(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~ZStreamerRWCon_SSL_Win();

// From ZStreamerRCon via ZStreamerRWCon
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZStreamerRWCon
	virtual const ZStreamWCon& GetStreamWCon();

// Our protocol
	ZStreamRWCon_SSL_Win& GetStream();

private:
	ZRef<ZStreamerR> fStreamerR;
	ZRef<ZStreamerW> fStreamerW;
	ZStreamRWCon_SSL_Win fStream;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZStreamRWCon_SSL_Win_h__
