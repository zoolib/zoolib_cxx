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

#include "zoolib/ZStreamRWCon_SSL_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

ZMACRO_MSVCStaticLib_cpp(StreamRWCon_SSL_Win)

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZStreamerRWCon_SSL.h"
#include "zoolib/ZUtil_STL.h"

#include <vector>

#include <schannel.h>

// Some #include fixups
#ifndef SecInvalidateHandle
	#define SecInvalidateHandle( x ) \
		((PSecHandle) x)->dwLower = (ULONG_PTR) -1 ; \
		((PSecHandle) x)->dwUpper = (ULONG_PTR) -1 ;
#endif
	
#ifndef SP_PROT_SSL3TLS1_CLIENTS
	#define SP_PROT_SSL3TLS1_CLIENTS (SP_PROT_TLS1_CLIENT | SP_PROT_SSL3_CLIENT)
#endif

#ifndef SEC_I_CONTEXT_EXPIRED
	#define SEC_I_CONTEXT_EXPIRED _HRESULT_TYPEDEF_(0x00090317L)
#endif

using std::deque;
using std::min;
using std::runtime_error;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

namespace ZANONYMOUS {

class Make_SSL
:	public ZFunctionChain_T<ZRef<ZStreamerRWCon>, MakeSSLParam_t>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZStreamerRWCon_SSL_Win(iParam.fStreamerR, iParam.fStreamerW);
		return true;
		}	
	} sMaker0;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static PSecurityFunctionTableA sPSFT = ::InitSecurityInterfaceA();

static void spWriteFlushFree(SecBuffer& sb, const ZStreamW& w)
	{
	if (!sb.pvBuffer)
		return;

	if (!sb.cbBuffer)
		{
		sPSFT->FreeContextBuffer(sb.pvBuffer);
		sb.pvBuffer = nullptr;
		}
	else
		{
		size_t countWritten = 0;
		w.Write(sb.pvBuffer, sb.cbBuffer, &countWritten);

		const bool shortWrite = (countWritten != sb.cbBuffer);
		sb.cbBuffer = 0;

		sPSFT->FreeContextBuffer(sb.pvBuffer);
		sb.pvBuffer = nullptr;

		if (shortWrite)
			ZStreamW::sThrowEndOfStream();
		w.Flush();
		}
	}

static bool spReadMore(vector<char>& ioBuf, const ZStreamR& r)
	{
	const size_t priorSize = ioBuf.size();
	const size_t newSize = priorSize + 4096;
	ioBuf.resize(newSize);
	size_t countRead;
	r.Read(&ioBuf[priorSize], newSize - priorSize, &countRead);
	ioBuf.resize(priorSize + countRead);
	return countRead != 0;
	}

static bool spAcquireCredentials(bool iVerify, bool iCheckName, CredHandle& oCredHandle)
	{
	SCHANNEL_CRED theSCC = {0};
	theSCC.dwVersion = SCHANNEL_CRED_VERSION;
	theSCC.grbitEnabledProtocols = SP_PROT_SSL3TLS1_CLIENTS | 0xA00;

	theSCC.dwFlags = SCH_CRED_NO_DEFAULT_CREDS;

	if (!iVerify)
		theSCC.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;
	else if (!iCheckName)		
		theSCC.dwFlags |= SCH_CRED_NO_SERVERNAME_CHECK;

	return SEC_E_OK == sPSFT->AcquireCredentialsHandleA(
		nullptr,
		const_cast<SEC_CHAR*>(UNISP_NAME_A),
		SECPKG_CRED_OUTBOUND,
		nullptr,
		&theSCC,
		nullptr,
		nullptr,
		&oCredHandle,
		nullptr);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWCon_SSL_Win

static const DWORD spRequirements =
	ISC_REQ_SEQUENCE_DETECT
	| ISC_REQ_REPLAY_DETECT
	| ISC_REQ_CONFIDENTIALITY
	| ISC_REQ_EXTENDED_ERROR
	| ISC_REQ_ALLOCATE_MEMORY
	| ISC_REQ_STREAM;

ZStreamRWCon_SSL_Win::ZStreamRWCon_SSL_Win(const ZStreamR& iStreamR, const ZStreamW& iStreamW)
:	fStreamR(iStreamR)
,	fStreamW(iStreamW)
,	fSendOpen(true)
	{
	bool iVerify = false;
	bool iCheckName = false;

	SecInvalidateHandle(&fCredHandle);
	SecInvalidateHandle(&fCtxtHandle);

	if (!spAcquireCredentials(iVerify, iCheckName, fCredHandle))
		throw runtime_error("ZStreamRWCon_SSL_Win, couldn't acquire credentials");
		
	if (!this->pConnect())
		{
		sPSFT->FreeCredentialsHandle(&fCredHandle);
		throw runtime_error("ZStreamRWCon_SSL_Win, couldn't handshake");
		}
	}

ZStreamRWCon_SSL_Win::~ZStreamRWCon_SSL_Win()
	{
	sPSFT->DeleteSecurityContext(&fCtxtHandle);
	sPSFT->FreeCredentialsHandle(&fCredHandle);
	}

void ZStreamRWCon_SSL_Win::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = static_cast<char*>(iDest);

	while (iCount)
		{
		if (fBufferPlain.size())
			{
			// We've got some data to return.
			size_t countToRead = min(iCount, fBufferPlain.size());
			deque<char>::iterator begin = fBufferPlain.begin();
			deque<char>::iterator end = begin + countToRead;
			std::copy(begin, end, static_cast<char*>(localDest));
			fBufferPlain.erase(begin, end);
			localDest += countToRead;
			iCount -= countToRead;
			}

		if (localDest != iDest)
			{
			// We've somehow managed to read some data, just above or down
			// below, and so can bail from the loop.
			break;
			}

		if (!fReceiveOpen)
			break;

		// We pass four buffers. inSB[0] references the available encrypted
		// data. inSB[1] through inSB[3] are marked as being empty, and may
		// be modified by DecryptMessage.
		SecBuffer inSB[4];
		inSB[0].cbBuffer = fBufferEnc.size();
		inSB[0].pvBuffer = ZUtil_STL::sFirstOrNil(fBufferEnc);
		inSB[0].BufferType = SECBUFFER_DATA;

		inSB[1].BufferType = SECBUFFER_EMPTY;
		inSB[2].BufferType = SECBUFFER_EMPTY;
		inSB[3].BufferType = SECBUFFER_EMPTY;

		SecBufferDesc inSBD;
		inSBD.cBuffers = 4;
		inSBD.pBuffers = inSB;
		inSBD.ulVersion = SECBUFFER_VERSION;

		SECURITY_STATUS scRet = sPSFT->DecryptMessage(&fCtxtHandle, &inSBD, 0, nullptr);

		if (scRet == SEC_E_INCOMPLETE_MESSAGE || (FAILED(scRet) && fBufferEnc.empty()))
			{
			// fBufferEnc holds an incomplete chunk, DecryptMessage needs
			// more data before it can do the decrypt.
			if (!spReadMore(fBufferEnc, fStreamR))
				{
				// We couldn't read any more encrypted data. Mark
				// our receive as being closed.
				fReceiveOpen = false;
				break;
				}
			continue;
			}

		if (FAILED(scRet))
			{
			// We failed for some other reason.
			fReceiveOpen = false;
			break;
			}

		// If DecryptMessage did any work, inSB[0] through inSB[2] will now reference
		// the header, decrypted data and trailer. inSB[3] will indicate how many
		// bytes at the end of fBufferEnc were unused by this decrypt, and so
		// must be preserved for subsequent use.
		// This assignment of information to buffers is only something I've determined
		// by inspection, so for safety we walk through them to find the two we care
		// about -- the decrypted data and any unused encrypted data.

		SecBuffer* decrypted = nullptr;
		SecBuffer* encrypted = nullptr;

		// Pickup any decrypted data
		for (size_t x = 1; x < 4; ++x)
			{
			if (inSB[x].BufferType == SECBUFFER_DATA && ! decrypted)
				decrypted = &inSB[x];
			if (inSB[x].BufferType == SECBUFFER_EXTRA && ! encrypted)
				encrypted = &inSB[x];
			}

		// The decryption happens in-place, ie in fBufferEnc. Therefore
		// we must copy out the decrypted data before munging fBufferEnc to
		// reference only the unused data.

		if (decrypted)
			{
			// Copy some decrypted data to our destination.
			const size_t countToCopy = std::min(iCount, size_t(decrypted->cbBuffer));
			ZMemCopy(localDest, decrypted->pvBuffer, countToCopy);
			localDest += countToCopy;
			iCount -= countToCopy;

			// Anything remaining we put in fBufferPlain, which must be
			// empty otherwise we wouldn't have got to this point.
			const char* data = static_cast<const char*>(decrypted->pvBuffer);
			fBufferPlain.insert(fBufferPlain.begin(), data + countToCopy, data + decrypted->cbBuffer);
			}

		if (encrypted)
			{
			// There is some unused data, move it to the front of fBufferEnc,
			// and resize fBufferEnc to reference only that data.
			ZMemMove(&fBufferEnc[0], encrypted->pvBuffer, encrypted->cbBuffer);
			fBufferEnc.resize(encrypted->cbBuffer);
			}
		else
			{
			// There was no unused data.
			fBufferEnc.clear();
			}

		if (scRet == SEC_I_CONTEXT_EXPIRED)
			{
			// We've received a close indication.
			fReceiveOpen = false;
			}
		else if (scRet == SEC_I_RENEGOTIATE)
			{
			// We need to re-handshake.
			if (!this->pHandshake())
				{
				fReceiveOpen = false;
				fSendOpen = false;
				}
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<char*>(iDest);
	}

size_t ZStreamRWCon_SSL_Win::Imp_CountReadable()
	{ return fBufferPlain.size(); }

bool ZStreamRWCon_SSL_Win::Imp_ReceiveDisconnect(double iTimeout)
	{
	for (;;)
		{
		uint64 countSkipped;
		this->SkipAll(&countSkipped);
		if (0 == countSkipped)
			return true;
		}
	}

void ZStreamRWCon_SSL_Win::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	SecPkgContext_StreamSizes theSizes;
	if (FAILED(sPSFT->QueryContextAttributesA(
		&fCtxtHandle, SECPKG_ATTR_STREAM_SIZES, &theSizes)))
		{
		// QueryContextAttributesA really shouldn't ever fail.
		ZAssert(false);
		return;
		}

	// Local buffer that will hold the message header, cyphertext and message trailer
	vector<char> buffer(theSizes.cbHeader
		+ min(iCount, size_t(theSizes.cbMaximumMessage))
		+ theSizes.cbTrailer,
		'\0');

	// Encryption happens in-place, copy the plaintext to the appropriate offset of the buffer.
	const size_t countToEncrypt = min(iCount, size_t(theSizes.cbMaximumMessage));
	ZMemCopy(&buffer[theSizes.cbHeader], iSource, countToEncrypt);

	SecBuffer outSB[4];
	outSB[0].cbBuffer = theSizes.cbHeader;
	outSB[0].pvBuffer = &buffer[0];
	outSB[0].BufferType = SECBUFFER_STREAM_HEADER;

	outSB[1].cbBuffer = countToEncrypt;
	outSB[1].pvBuffer = &buffer[theSizes.cbHeader];
	outSB[1].BufferType = SECBUFFER_DATA;

	outSB[2].cbBuffer = theSizes.cbTrailer;
	outSB[2].pvBuffer = &buffer[theSizes.cbHeader + countToEncrypt];
	outSB[2].BufferType = SECBUFFER_STREAM_TRAILER;

	outSB[3].BufferType = SECBUFFER_EMPTY;

	SecBufferDesc outSBD;
	outSBD.pBuffers = outSB;
	outSBD.cBuffers = 4;
	outSBD.ulVersion = SECBUFFER_VERSION;

	SECURITY_STATUS result = sPSFT->EncryptMessage(&fCtxtHandle, 0, &outSBD, 0);
	if (SUCCEEDED(result))
		{
		const size_t countToSend = outSB[0].cbBuffer + outSB[1].cbBuffer + outSB[2].cbBuffer;
		size_t countSent;
		fStreamW.Write(outSB[0].pvBuffer, countToSend, &countSent);
		if (countSent == countToSend)
			{
			if (oCountWritten)
				*oCountWritten = countToEncrypt;
			return;
			}
		}

	fSendOpen = false;
	}

void ZStreamRWCon_SSL_Win::Imp_SendDisconnect()
	{
	if (!fSendOpen)
		return;

	fSendOpen = false;

	SecBufferDesc outSBD;
	SecBuffer outSB[1];
	outSB[0].cbBuffer = 0;
	outSB[0].pvBuffer = nullptr;
	outSB[0].BufferType= SECBUFFER_TOKEN;

	outSBD.cBuffers = 1;
	outSBD.pBuffers = outSB;
	outSBD.ulVersion = SECBUFFER_VERSION;

	DWORD attributes;
	SECURITY_STATUS scRet = sPSFT->InitializeSecurityContextA(
		&fCredHandle,
		&fCtxtHandle,
		nullptr,
		spRequirements,
		0, // Reserved1
		SECURITY_NATIVE_DREP,
		nullptr,
		0, // Reserved2
		nullptr,
		&outSBD,
		&attributes,
		nullptr);

	if (FAILED(scRet))
		return;

	spWriteFlushFree(outSB[0], fStreamW);
	}

void ZStreamRWCon_SSL_Win::Imp_Abort()
	{
	// Hmm. We should really abort our source stream.
	this->Imp_SendDisconnect();
	}

bool ZStreamRWCon_SSL_Win::pConnect()
	{
	SecBuffer outSB[1];
	outSB[0].cbBuffer = 0;
	outSB[0].pvBuffer = nullptr;
	outSB[0].BufferType = SECBUFFER_TOKEN;

	SecBufferDesc outSBD;
	outSBD.cBuffers = 1;
	outSBD.pBuffers = outSB;
	outSBD.ulVersion = SECBUFFER_VERSION;

	DWORD attributes;
	if (SEC_I_CONTINUE_NEEDED != sPSFT->InitializeSecurityContextA(
		&fCredHandle,
		nullptr,
		nullptr, // other party, needed if we're verifying
		spRequirements,
		0, // Reserved1
		SECURITY_NATIVE_DREP,
		nullptr,
		0, // Reserved2
		&fCtxtHandle,
		&outSBD,
		&attributes,
		nullptr))
		{
		return false;
		}

	spWriteFlushFree(outSB[0], fStreamW);

	return this->pHandshake();
	}

bool ZStreamRWCon_SSL_Win::pHandshake()
	{
	for (;;)
		{
		SecBuffer inSB[2];

		// Data from peer is in fBufferEnc, and passed to schannel via inSB[0].
		inSB[0].cbBuffer = fBufferEnc.size();
		inSB[0].pvBuffer = ZUtil_STL::sFirstOrNil(fBufferEnc);
		inSB[0].BufferType = SECBUFFER_TOKEN;

		// After calling schannel, any unused data in fBufferEnc is described by inSB[1].
		inSB[1].cbBuffer = 0;
		inSB[1].pvBuffer = nullptr;
		inSB[1].BufferType = SECBUFFER_EMPTY;

		SecBufferDesc inSBD;
		inSBD.cBuffers = 2;
		inSBD.pBuffers = inSB;
		inSBD.ulVersion = SECBUFFER_VERSION;

		// Will hold data to be sent to peer
		SecBufferDesc outSBD;
		SecBuffer outSB[1];
		outSB[0].cbBuffer = 0;
		outSB[0].pvBuffer = nullptr;
		outSB[0].BufferType= SECBUFFER_TOKEN;

		outSBD.cBuffers = 1;
		outSBD.pBuffers = outSB;
		outSBD.ulVersion = SECBUFFER_VERSION;

		DWORD attributes;
		const SECURITY_STATUS result = sPSFT->InitializeSecurityContextA(
			&fCredHandle,
			&fCtxtHandle,
			nullptr,
			spRequirements,
			0, // Reserved1
			SECURITY_NATIVE_DREP,
			&inSBD,
			0, // Reserved2
			nullptr,
			&outSBD,
			&attributes,
			nullptr);

		if (result == SEC_E_INCOMPLETE_MESSAGE)
			{
			if (!spReadMore(fBufferEnc, fStreamR))
				return false;
			}
		else
			{
			// Send anything that was generated.
			spWriteFlushFree(outSB[0], fStreamW);

			if (FAILED(result))
				return false;

			// Remove consumed data.
			if (inSB[1].BufferType == SECBUFFER_EXTRA) 
				fBufferEnc.erase(fBufferEnc.begin(), fBufferEnc.end() - inSB[1].cbBuffer);
			else
				fBufferEnc.clear();


			if (result == SEC_E_OK) 
				{
				// Handshake completed successfully.
				return true;
				}
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWCon_SSL_Win

ZStreamerRWCon_SSL_Win::ZStreamerRWCon_SSL_Win(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fStream(fStreamerR->GetStreamR(), fStreamerW->GetStreamW())
	{}

ZStreamerRWCon_SSL_Win::~ZStreamerRWCon_SSL_Win()
	{}

const ZStreamRCon& ZStreamerRWCon_SSL_Win::GetStreamRCon()
	{ return fStream; }

const ZStreamWCon& ZStreamerRWCon_SSL_Win::GetStreamWCon()
	{ return fStream; }

ZStreamRWCon_SSL_Win& ZStreamerRWCon_SSL_Win::GetStream()
	{ return fStream; }

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Win)
