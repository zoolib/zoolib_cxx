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

#include "zoolib/ZStreamRWCon_OpenSSL.h"

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZStreamerRWCon_SSL.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

namespace { // anonymous

class Make_SSL
:	public ZFunctionChain_T<ZRef<ZStreamerRWCon>, MakeSSLParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZStreamerRWCon_OpenSSL(iParam.fStreamerR, iParam.fStreamerW);
		return true;
		}
	} sMaker0;

} // anonymous namespace

static SSL_CTX* spCTX;

static int spDummy = ::SSL_library_init();

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWCon_OpenSSL

ZStreamRWCon_OpenSSL::ZStreamRWCon_OpenSSL(const ZStreamR& iStreamR, const ZStreamW& iStreamW)
:	fStreamR(iStreamR)
,	fStreamW(iStreamW)
,	fSSL(nullptr)
,	fLastWasWrite(false)
	{
	fSSL_CTX = ::SSL_CTX_new(::SSLv23_client_method());
	fSSL = ::SSL_new(fSSL_CTX);
	fBIOR = ::BIO_new(::BIO_s_mem());
	fBIOW = ::BIO_new(::BIO_s_mem());
	::SSL_set_bio(fSSL, fBIOR, fBIOW);
	::SSL_set_connect_state(fSSL);

	}

ZStreamRWCon_OpenSSL::~ZStreamRWCon_OpenSSL()
	{
	if (fSSL)
		::SSL_free(fSSL);
	}

void ZStreamRWCon_OpenSSL::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	int result = ::SSL_read(fSSL, oDest, iCount);
	if (result < 0)
		{
		int error = ::SSL_get_error(fSSL, result);

		switch(error)
			{
			case SSL_ERROR_ZERO_RETURN:
			case SSL_ERROR_NONE: 
			case SSL_ERROR_WANT_READ:
				{
				break;
				}
			}
		result = 0;
		}

	if (oCountRead)
		*oCountRead = result;
	}

size_t ZStreamRWCon_OpenSSL::Imp_CountReadable()
	{
	return 0;
	}

bool ZStreamRWCon_OpenSSL::Imp_ReceiveDisconnect(double iTimeout)
	{
	for (;;)
		{
		uint64 countSkipped;
		this->SkipAll(&countSkipped);
		if (0 == countSkipped)
			return true;
		}
	}

void ZStreamRWCon_OpenSSL::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	int result = ::SSL_write(fSSL, iSource, iCount);
	if (result < 0)
		{
		int error = ::SSL_get_error(fSSL, result);
		switch(error)
			{
			case SSL_ERROR_ZERO_RETURN:
				break;

			case SSL_ERROR_NONE: 
				break;

			case SSL_ERROR_WANT_READ:
				break;
			}

		if (oCountWritten)
			*oCountWritten = 0;
		}
	else
		{
		if (oCountWritten)
			*oCountWritten = result;
		}
	}

void ZStreamRWCon_OpenSSL::Imp_Flush()
	{ fStreamW.Flush(); }

void ZStreamRWCon_OpenSSL::Imp_SendDisconnect()
	{
	::SSL_shutdown(fSSL);
	}

void ZStreamRWCon_OpenSSL::Imp_Abort()
	{
	::SSL_shutdown(fSSL);
	}

void ZStreamRWCon_OpenSSL::pRead(void* oDest, size_t* ioCount)
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
//		if (countRead == 0)
//			return ioErr;
//		return noErr;
		}
	catch (...)
		{
//		return errSSLClosedAbort;
		}
	}

#if 0
OSStatus ZStreamRWCon_OpenSSL::spRead(SSLConnectionRef iRefcon, void* oDest, size_t* ioCount)
	{
	ZStreamRWCon_OpenSSL* theS =
		const_cast<ZStreamRWCon_OpenSSL*>(static_cast<const ZStreamRWCon_OpenSSL*>(iRefcon));

	return theS->pRead(oDest, ioCount);
	}
#endif

OSStatus ZStreamRWCon_OpenSSL::pWrite(const void* iSource, size_t* ioCount)
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

OSStatus ZStreamRWCon_OpenSSL::spWrite
	(SSLConnectionRef iRefcon, const void* iSource, size_t* ioCount)
	{
	ZStreamRWCon_OpenSSL* theS =
		const_cast<ZStreamRWCon_OpenSSL*>(static_cast<const ZStreamRWCon_OpenSSL*>(iRefcon));

	return theS->pWrite(iSource, ioCount);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWCon_OpenSSL

ZStreamerRWCon_OpenSSL::ZStreamerRWCon_OpenSSL
	(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fStream(fStreamerR->GetStreamR(), fStreamerW->GetStreamW())
	{}

ZStreamerRWCon_OpenSSL::~ZStreamerRWCon_OpenSSL()
	{}

const ZStreamRCon& ZStreamerRWCon_OpenSSL::GetStreamRCon()
	{ return fStream; }

const ZStreamWCon& ZStreamerRWCon_OpenSSL::GetStreamWCon()
	{ return fStream; }

ZStreamRWCon_OpenSSL& ZStreamerRWCon_OpenSSL::GetStream()
	{ return fStream; }

} // namespace ZooLib
