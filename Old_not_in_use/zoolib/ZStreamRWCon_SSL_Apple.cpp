// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZStreamRWCon_SSL_Apple.h"

#if ZCONFIG_SPI_Enabled(iPhone)\
	or (defined(MAC_OS_X_VERSION_10_8) and MAC_OS_X_VERSION_10_8 <= MAC_OS_X_VERSION_MIN_REQUIRED)

ZMACRO_MSVCStaticLib_cpp(StreamRWCon_SSL_Apple)

#include "zoolib/ZStreamerRWCon_SSL.h"

//#include <CoreServices/CoreServices.h> // For ioErr

namespace ZooLib {

// =================================================================================================
#pragma mark - Factory functions

namespace { // anonymous

class Make_SSL
:	public FunctionChain<ZRef<ZStreamerRWCon>, MakeSSLParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZStreamerRWCon_SSL_Apple(
			iParam.fStreamerR, iParam.fStreamerW, iParam.fIsServer);
		return true;
		}
	} sMaker0;

} // anonymous namespace

// =================================================================================================
#pragma mark - ZStreamRWCon_SSL_Apple

ZStreamRWCon_SSL_Apple::ZStreamRWCon_SSL_Apple(
	const ZStreamR& iStreamR, const ZStreamW& iStreamW, bool iIsServer)
:	fStreamR(iStreamR)
,	fStreamW(iStreamW)
,	fSSLCR(nullptr)
,	fLastWasWrite(false)
	{
	fSSLCR = SSLCreateContext(0, iIsServer ? kSSLServerSide : kSSLClientSide, kSSLStreamType);
	try
		{
		::SSLSetConnection(fSSLCR, this);
		::SSLSetIOFuncs(fSSLCR, spRead, spWrite);

		if (iIsServer)
			{
			// *Must* have a server certificate.
			::SSLSetClientSideAuthenticate(fSSLCR, kNeverAuthenticate);
			}

		::SSLSetProtocolVersionMin(fSSLCR, kSSLProtocol3);
		::SSLSetProtocolVersionMax(fSSLCR, kDTLSProtocol1);

//##		::SSLSetAllowsExpiredCerts(fSSLCR, true);
//##		::SSLSetEnableCertVerify(fSSLCR, false);
//##		::SSLSetAllowsExpiredRoots(fSSLCR, true);
//##		::SSLSetAllowsAnyRoot(fSSLCR, true);

		OSStatus result = ::SSLHandshake(fSSLCR);

		if (noErr != result)
			throw std::runtime_error("SSL connect failed");
		}
	catch (...)
		{
		if (fSSLCR)
			CFRelease(fSSLCR);
		throw;
		}
	}

ZStreamRWCon_SSL_Apple::~ZStreamRWCon_SSL_Apple()
	{
	if (fSSLCR)
		CFRelease(fSSLCR);
	}

void ZStreamRWCon_SSL_Apple::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{ ::SSLRead(fSSLCR, oDest, iCount, oCountRead); }

size_t ZStreamRWCon_SSL_Apple::Imp_CountReadable()
	{
	size_t result;
	::SSLGetBufferedReadSize(fSSLCR, &result);
	return result;
	}

bool ZStreamRWCon_SSL_Apple::Imp_ReceiveDisconnect(double iTimeout)
	{
	for (;;)
		{
		uint64 countSkipped;
		this->SkipAll(&countSkipped);
		if (0 == countSkipped)
			return true;
		}
	}

void ZStreamRWCon_SSL_Apple::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{ ::SSLWrite(fSSLCR, iSource, iCount, oCountWritten); }

void ZStreamRWCon_SSL_Apple::Imp_Flush()
	{ fStreamW.Flush(); }

void ZStreamRWCon_SSL_Apple::Imp_SendDisconnect()
	{
	::SSLClose(fSSLCR);
	}

void ZStreamRWCon_SSL_Apple::Imp_Abort()
	{
	::SSLClose(fSSLCR); // ???
	}

OSStatus ZStreamRWCon_SSL_Apple::pRead(void* oDest, size_t* ioCount)
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
		fStreamR.ReadFully(oDest, countToRead, &countRead);
		*ioCount = countRead;
		if (countRead)
			return noErr;
		}
	catch (...)
		{}

	return -1;
	}

OSStatus ZStreamRWCon_SSL_Apple::spRead(SSLConnectionRef iRefcon, void* oDest, size_t* ioCount)
	{
	ZStreamRWCon_SSL_Apple* theS =
		const_cast<ZStreamRWCon_SSL_Apple*>(static_cast<const ZStreamRWCon_SSL_Apple*>(iRefcon));

	return theS->pRead(oDest, ioCount);
	}

OSStatus ZStreamRWCon_SSL_Apple::pWrite(const void* iSource, size_t* ioCount)
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

	return -1;
	}

OSStatus ZStreamRWCon_SSL_Apple::spWrite(
	SSLConnectionRef iRefcon, const void* iSource, size_t* ioCount)
	{
	ZStreamRWCon_SSL_Apple* theS =
		const_cast<ZStreamRWCon_SSL_Apple*>(static_cast<const ZStreamRWCon_SSL_Apple*>(iRefcon));

	return theS->pWrite(iSource, ioCount);
	}

// =================================================================================================
#pragma mark - ZStreamerRWCon_SSL_Apple

ZStreamerRWCon_SSL_Apple::ZStreamerRWCon_SSL_Apple(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW, bool iIsServer)
:	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fStream(fStreamerR->GetStreamR(), fStreamerW->GetStreamW(), iIsServer)
	{}

ZStreamerRWCon_SSL_Apple::~ZStreamerRWCon_SSL_Apple()
	{}

const ZStreamRCon& ZStreamerRWCon_SSL_Apple::GetStreamRCon()
	{ return fStream; }

const ZStreamWCon& ZStreamerRWCon_SSL_Apple::GetStreamWCon()
	{ return fStream; }

ZStreamRWCon_SSL_Apple& ZStreamerRWCon_SSL_Apple::GetStream()
	{ return fStream; }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(iPhone)\
	or (defined(MAC_OS_X_VERSION_10_8) and MAC_OS_X_VERSION_10_8 <= MAC_OS_X_VERSION_MIN_REQUIRED)
