/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZStreamRWCon_SSL_OSX_h__
#define __ZStreamRWCon_SSL_OSX_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZStreamer.h"

#if ZCONFIG_SPI_Enabled(MacOSX)

#include "zoolib/ZFunctionChain.h"

ZMACRO_MSVCStaticLib_Reference(StreamRWCon_SSL_OSX);

#include <Security/SecureTransport.h>

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamRWCon_SSL_OSX

class ZStreamRWCon_SSL_OSX
:	public ZStreamRCon
,	public ZStreamWCon
	{
public:
	ZStreamRWCon_SSL_OSX(const ZStreamR& iStreamR, const ZStreamW& iStreamW, bool iIsServer);
	~ZStreamRWCon_SSL_OSX();

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
// MARK: - ZStreamRWCon_SSL_OSX

class ZStreamerRWCon_SSL_OSX
:	public ZStreamerRWCon
	{
public:
	ZStreamerRWCon_SSL_OSX(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW, bool iIsServer);
	virtual ~ZStreamerRWCon_SSL_OSX();

// From ZStreamerRCon via ZStreamerRWCon_SSL
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZStreamerRWCon_SSL
	virtual const ZStreamWCon& GetStreamWCon();

// Our protocol
	ZStreamRWCon_SSL_OSX& GetStream();

private:
	ZRef<ZStreamerR> fStreamerR;
	ZRef<ZStreamerW> fStreamerW;
	ZStreamRWCon_SSL_OSX fStream;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(MacOSX)

#endif // __ZStreamRWCon_SSL_OSX_h__
