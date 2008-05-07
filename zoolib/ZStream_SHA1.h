/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_SHA1__
#define __ZStream_SHA1__ 1
#include "zconfig.h"
#include "ZCONFIG_SPI.h"

#ifndef ZCONFIG_StreamSHA1_UseOpenSSL
#	define ZCONFIG_StreamSHA1_UseOpenSSL ZCONFIG_SPI_Enabled(openssl)
#endif

#include "ZStream.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStream_SHA1

#if ZCONFIG_StreamSHA1_UseOpenSSL
#	include <openssl/sha.h>
#endif

namespace ZStream_SHA1 {

#if ZCONFIG_StreamSHA1_UseOpenSSL

typedef SHA_CTX Context;

#else // ZCONFIG_StreamSHA1_UseOpenSSL

struct Context
	{
	uint32 fState[5];
	size_t fSpaceUsed;
	size_t fBuffersSent;
	union
		{
		uint32 fBuffer32[16];
		uint8 fBuffer8[64];
		};
	};

#endif // ZCONFIG_StreamSHA1_UseOpenSSL

void sInit(Context& oContext);
void sUpdate(Context& ioContext, const void* iData, size_t iCount);
void sFinal(Context& ioContext, uint8 oDigest[20]);

void sTest();

} // namespace ZStream_SHA1

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_SHA1

/// A read filter stream that SHA1 hashes data read through it.

class ZStreamR_SHA1 : public ZStreamR
	{
public:
	ZStreamR_SHA1(const ZStreamR& iStreamSource);
	ZStreamR_SHA1(uint8 oDigest[20], const ZStreamR& iStreamSource);
	~ZStreamR_SHA1();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// Our protocol
	void GetDigest(uint8 oDigest[20]);

protected:
	const ZStreamR& fStreamSource;
	ZStream_SHA1::Context fContext;
	uint8* fDigest;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_SHA1

/// A write filter stream that SHA1 hashes data written through it.

class ZStreamW_SHA1 : public ZStreamW
	{
public:
	ZStreamW_SHA1(const ZStreamW& iStreamSink);
	ZStreamW_SHA1(uint8 oDigest[20], const ZStreamW& iStreamSink);
	~ZStreamW_SHA1();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// Our protocol
	void GetDigest(uint8 oDigest[20]);

protected:
	const ZStreamW& fStreamSink;
	ZStream_SHA1::Context fContext;
	uint8* fDigest;
	};

// =================================================================================================

#endif // __ZStream_SHA1__
