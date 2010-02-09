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

#include "zoolib/ZStream_SSL_OSX.h"

#if ZCONFIG_SPI_Enabled(MacOSX)

#include ZMACINCLUDE3(CoreServices,CarbonCore,MacErrors.h) // For ioErr

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWCon_SSL_OSX

ZStreamRWCon_SSL_OSX::ZStreamRWCon_SSL_OSX(const ZStreamR& iStreamR, const ZStreamW& iStreamW)
:	fStreamR(iStreamR)
,	fStreamW(iStreamW)
,	fSSLCR(nullptr)
,	fLastWasWrite(false)
	{
	::SSLNewContext(false, &fSSLCR);
	try
		{
		::SSLSetConnection(fSSLCR, this);
		::SSLSetIOFuncs(fSSLCR, spRead, spWrite);

		::SSLSetProtocolVersion(fSSLCR, kSSLProtocolAll);
		::SSLSetAllowsExpiredCerts(fSSLCR, true);
	//	::SSLSetEnableCertVerify(fSSLCR, false);
		::SSLSetAllowsExpiredRoots(fSSLCR, true);
		::SSLSetAllowsAnyRoot(fSSLCR, true);

		OSStatus result = ::SSLHandshake(fSSLCR);

		if (noErr != result)
			throw std::runtime_error("SSL connect failed");
		}
	catch (...)
		{
		if (fSSLCR)
			::SSLDisposeContext(fSSLCR);
		throw;
		}
	}

ZStreamRWCon_SSL_OSX::~ZStreamRWCon_SSL_OSX()
	{
	if (fSSLCR)
		::SSLDisposeContext(fSSLCR);
	}

void ZStreamRWCon_SSL_OSX::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{ ::SSLRead(fSSLCR, iDest, iCount, oCountRead); }

size_t ZStreamRWCon_SSL_OSX::Imp_CountReadable()
	{
	size_t result;
	::SSLGetBufferedReadSize(fSSLCR, &result);
	return result;
	}

bool ZStreamRWCon_SSL_OSX::Imp_ReceiveDisconnect(double iTimeout)
	{
	for (;;)
		{
		uint64 countSkipped;
		this->SkipAll(&countSkipped);
		if (0 == countSkipped)
			return true;
		}
	}

void ZStreamRWCon_SSL_OSX::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{ ::SSLWrite(fSSLCR, iSource, iCount, oCountWritten); }

void ZStreamRWCon_SSL_OSX::Imp_SendDisconnect()
	{
	::SSLClose(fSSLCR);
	}

void ZStreamRWCon_SSL_OSX::Imp_Abort()
	{
	::SSLClose(fSSLCR); //???
	}

OSStatus ZStreamRWCon_SSL_OSX::pRead(void* iDest, size_t* ioCount)
	{
	try
		{
		size_t countToRead = *ioCount;
		if (fLastWasWrite)
			{
			fLastWasWrite = false;
			fStreamW.Flush();
			}
		size_t countRead;
		fStreamR.ReadAll(iDest, countToRead, &countRead);
		*ioCount = countRead;
		if (countRead == 0)
			return ioErr;
		return noErr;
		}
	catch (...)
		{
		return errSSLClosedAbort;
		}		
	}

OSStatus ZStreamRWCon_SSL_OSX::spRead(SSLConnectionRef iRefcon, void* iDest, size_t* ioCount)
	{
	ZStreamRWCon_SSL_OSX* theS =
		const_cast<ZStreamRWCon_SSL_OSX*>(static_cast<const ZStreamRWCon_SSL_OSX*>(iRefcon));

	return theS->pRead(iDest, ioCount);
	}

OSStatus ZStreamRWCon_SSL_OSX::pWrite(const void* iSource, size_t* ioCount)
	{
	try
		{
		size_t countToWrite = *ioCount;
		size_t countWritten;
		fStreamW.Write(iSource, countToWrite, &countWritten);
		fLastWasWrite = true;
		*ioCount = countWritten;
		if (countWritten == 0)
			return errSSLClosedAbort;
		return noErr;
		}
	catch (...)
		{
		return errSSLClosedAbort;
		}		
	}

OSStatus ZStreamRWCon_SSL_OSX::spWrite(SSLConnectionRef iRefcon, const void* iSource, size_t* ioCount)
	{
	ZStreamRWCon_SSL_OSX* theS =
		const_cast<ZStreamRWCon_SSL_OSX*>(static_cast<const ZStreamRWCon_SSL_OSX*>(iRefcon));

	return theS->pWrite(iSource, ioCount);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWCon_SSL_OSX

ZStreamerRWCon_SSL_OSX::ZStreamerRWCon_SSL_OSX(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fStream(fStreamerR->GetStreamR(), fStreamerW->GetStreamW())
	{}

ZStreamerRWCon_SSL_OSX::~ZStreamerRWCon_SSL_OSX()
	{}

const ZStreamRCon& ZStreamerRWCon_SSL_OSX::GetStreamRCon()
	{ return fStream; }

const ZStreamWCon& ZStreamerRWCon_SSL_OSX::GetStreamWCon()
	{ return fStream; }

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(MacOSX)
