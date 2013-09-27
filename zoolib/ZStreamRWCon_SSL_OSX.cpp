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

#include "zoolib/ZStreamRWCon_SSL_OSX.h"

#if ZCONFIG_SPI_Enabled(MacOSX)

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZStreamerRWCon_SSL.h"

#include ZMACINCLUDE3(CoreServices,CarbonCore,MacErrors.h) // For ioErr

namespace ZooLib {

// =================================================================================================
// MARK: - Factory functions

namespace { // anonymous

class Make_SSL
:	public ZFunctionChain_T<ZRef<ZStreamerRWCon>, MakeSSLParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZStreamerRWCon_SSL_OSX(iParam.fStreamerR, iParam.fStreamerW, iParam.fIsServer);
		return true;
		}
	} sMaker0;

} // anonymous namespace

// =================================================================================================
// MARK: - ZStreamRWCon_SSL_OSX

ZStreamRWCon_SSL_OSX::ZStreamRWCon_SSL_OSX(
	const ZStreamR& iStreamR, const ZStreamW& iStreamW, bool iIsServer)
:	fStreamR(iStreamR)
,	fStreamW(iStreamW)
,	fSSLCR(nullptr)
,	fLastWasWrite(false)
	{
	::SSLNewContext(iIsServer, &fSSLCR);
	try
		{
		::SSLSetConnection(fSSLCR, this);
		::SSLSetIOFuncs(fSSLCR, spRead, spWrite);

		if (iIsServer)
			{
			// *Must* have a server certificate.
			::SSLSetClientSideAuthenticate(fSSLCR, kNeverAuthenticate);
			::SSLSetProtocolVersionEnabled(fSSLCR, kSSLProtocolAll, true);
//##			::SSLSetCertificate(fSSLCR)
			}
		else
			{
			::SSLSetProtocolVersion(fSSLCR, kSSLProtocolAll);
			}
		::SSLSetAllowsExpiredCerts(fSSLCR, true);
		::SSLSetEnableCertVerify(fSSLCR, false);
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

void ZStreamRWCon_SSL_OSX::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{ ::SSLRead(fSSLCR, oDest, iCount, oCountRead); }

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

void ZStreamRWCon_SSL_OSX::Imp_Flush()
	{ fStreamW.Flush(); }

void ZStreamRWCon_SSL_OSX::Imp_SendDisconnect()
	{
	::SSLClose(fSSLCR);
	}

void ZStreamRWCon_SSL_OSX::Imp_Abort()
	{
	::SSLClose(fSSLCR); // ???
	}

OSStatus ZStreamRWCon_SSL_OSX::pRead(void* oDest, size_t* ioCount)
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
		fStreamR.ReadAll(oDest, countToRead, &countRead);
		*ioCount = countRead;
		if (countRead)
			return noErr;
		}
	catch (...)
		{}

	return ioErr;
	}

OSStatus ZStreamRWCon_SSL_OSX::spRead(SSLConnectionRef iRefcon, void* oDest, size_t* ioCount)
	{
	ZStreamRWCon_SSL_OSX* theS =
		const_cast<ZStreamRWCon_SSL_OSX*>(static_cast<const ZStreamRWCon_SSL_OSX*>(iRefcon));

	return theS->pRead(oDest, ioCount);
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
		if (countWritten)
			return noErr;
		}
	catch (...)
		{}

	return ioErr;
	}

OSStatus ZStreamRWCon_SSL_OSX::spWrite(
	SSLConnectionRef iRefcon, const void* iSource, size_t* ioCount)
	{
	ZStreamRWCon_SSL_OSX* theS =
		const_cast<ZStreamRWCon_SSL_OSX*>(static_cast<const ZStreamRWCon_SSL_OSX*>(iRefcon));

	return theS->pWrite(iSource, ioCount);
	}

// =================================================================================================
// MARK: - ZStreamerRWCon_SSL_OSX

ZStreamerRWCon_SSL_OSX::ZStreamerRWCon_SSL_OSX(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW, bool iIsServer)
:	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fStream(fStreamerR->GetStreamR(), fStreamerW->GetStreamW(), iIsServer)
	{}

ZStreamerRWCon_SSL_OSX::~ZStreamerRWCon_SSL_OSX()
	{}

const ZStreamRCon& ZStreamerRWCon_SSL_OSX::GetStreamRCon()
	{ return fStream; }

const ZStreamWCon& ZStreamerRWCon_SSL_OSX::GetStreamWCon()
	{ return fStream; }

ZStreamRWCon_SSL_OSX& ZStreamerRWCon_SSL_OSX::GetStream()
	{ return fStream; }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(MacOSX)
