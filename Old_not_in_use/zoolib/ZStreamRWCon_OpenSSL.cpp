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

#include "zoolib/OpenSSL/ZStreamRWCon_OpenSSL.h"

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZStreamerRWCon_SSL.h"

ZMACRO_MSVCStaticLib_cpp(StreamRWCon_OpenSSL)

namespace ZooLib {

// =================================================================================================
#pragma mark - Factory functions

namespace { // anonymous

class Make_SSL
:	public ZFunctionChain_T<ZRef<ZStreamerRWCon>, MakeSSLParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZStreamerRWCon_OpenSSL(iParam.fStreamerR, iParam.fStreamerW, iParam.fIsServer);
		return true;
		}
	} sMaker0;

} // anonymous namespace

static int spDummy = ::SSL_library_init();

// =================================================================================================
#pragma mark - ZStreamRWCon_OpenSSL

ZStreamRWCon_OpenSSL::ZStreamRWCon_OpenSSL(
	const ZStreamR& iStreamR, const ZStreamW& iStreamW, bool iIsServer)
:	fStreamR(iStreamR)
,	fStreamW(iStreamW)
,	fSSL(nullptr)
,	fLastWasWrite(false)
	{
	if (iIsServer)
		fSSL_CTX = ::SSL_CTX_new(::DTLSv1_server_method());
	else
		fSSL_CTX = ::SSL_CTX_new(::TLSv1_method());

	fSSL = ::SSL_new(fSSL_CTX);

	::SSL_set_verify(fSSL, SSL_VERIFY_NONE, nullptr);

	fBIOR = ::BIO_new(::BIO_s_mem());
	::BIO_set_nbio(fBIOR, 1);

	fBIOW = ::BIO_new(::BIO_s_mem());
	::BIO_set_nbio(fBIOW, 1);

	::SSL_set_bio(fSSL, fBIOR, fBIOW);

	if (iIsServer)
		::SSL_set_accept_state(fSSL);
	else
		::SSL_set_connect_state(fSSL);

	for (;;)
		{
		int result = ::SSL_do_handshake(fSSL);
		if (result == 0)
			break;
		if (this->pHandleError(result))
			break;
		}
	}

bool ZStreamRWCon_OpenSSL::pHandleError(int iResult)
	{
	int error = ::SSL_get_error(fSSL, iResult);

	switch (error)
		{
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			{
			break;
			}
		case SSL_ERROR_ZERO_RETURN:
		case SSL_ERROR_NONE:
			{
			return true;
			}
		default:
			{
			return false;
			}
		}

	char buf[1024];
	int countRead = ::BIO_read(fBIOW, buf, sizeof(buf));
	if (countRead > 0)
		{
		fStreamW.Write(buf, countRead);
		return false;
		}

	if (SSL_want_read(fSSL))
		{
		fStreamW.Flush();
		size_t countRead;
		fStreamR.Read(buf, sizeof(buf), &countRead);
		if (countRead)
			{
			::BIO_write(fBIOR, buf, countRead);
			return false;
			}
		return true;
		}
	return true;
	}

ZStreamRWCon_OpenSSL::~ZStreamRWCon_OpenSSL()
	{
	if (fSSL)
		::SSL_free(fSSL);
	}

void ZStreamRWCon_OpenSSL::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	for (;;)
		{
		int result = ::SSL_read(fSSL, oDest, iCount);
		if (result >= 0)
			{
			if (oCountRead)
				*oCountRead = result;
			break;
			}
		if (this->pHandleError(result))
			{
			if (oCountRead)
				*oCountRead = 0;
			break;
			}
		}
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
	for (;;)
		{
		int result = ::SSL_write(fSSL, iSource, iCount);
		if (result >= 0)
			{
			if (oCountWritten)
				*oCountWritten = result;
			break;
			}
		if (this->pHandleError(result))
			{
			if (oCountWritten)
				*oCountWritten = 0;
			break;
			}
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

// =================================================================================================
#pragma mark - ZStreamRWCon_OpenSSL

ZStreamerRWCon_OpenSSL::ZStreamerRWCon_OpenSSL(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW, bool iIsServer)
:	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fStream(fStreamerR->GetStreamR(), fStreamerW->GetStreamW(), iIsServer)
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
