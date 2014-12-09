/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_MD5_h__
#define __ZStream_MD5_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_StreamMD5_UseOpenSSL
	#define ZCONFIG_StreamMD5_UseOpenSSL ZCONFIG_SPI_Enabled(openssl)
#endif

#include "zoolib/ZStream_Filter.h"

#if ZCONFIG_StreamMD5_UseOpenSSL
	#include <openssl/md5.h>
#endif

namespace ZooLib {

// =================================================================================================
// MARK: - ZStream_MD5

namespace ZStream_MD5 {

#if ZCONFIG_StreamMD5_UseOpenSSL

typedef MD5_CTX Context;

#else // ZCONFIG_StreamMD5_UseOpenSSL

struct Context
	{
	uint32 buf[4];
	uint32 bits[2];
	unsigned char in[64];
	};

#endif // ZCONFIG_StreamMD5_UseOpenSSL

void sInit(Context& oContext);
void sUpdate(Context& ioContext, const void* iData, size_t iCount);
void sFinal(Context& ioContext, uint8 oDigest[16]);

} // namespace ZStream_MD5

// =================================================================================================
// MARK: - ZStreamR_MD5

/// A read filter stream that MD5 hashes data read through it.

class ZStreamR_MD5 : public ZStreamR_Filter
	{
public:
	ZStreamR_MD5(const ZStreamR& iStreamSource);
	ZStreamR_MD5(uint8 oDigest[16], const ZStreamR& iStreamSource);
	~ZStreamR_MD5();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

// Our protocol
	void GetDigest(uint8 oDigest[16]);

protected:
	const ZStreamR& fStreamSource;
	ZStream_MD5::Context fContext;
	uint8* fDigest;
	};

// =================================================================================================
// MARK: - ZStreamW_MD5

/// A write filter stream that MD5 hashes data written through it.

class ZStreamW_MD5 : public ZStreamW
	{
public:
	ZStreamW_MD5(const ZStreamW& iStreamSink);
	ZStreamW_MD5(uint8 oDigest[16], const ZStreamW& iStreamSink);
	~ZStreamW_MD5();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// Our protocol
	void GetDigest(uint8 oDigest[16]);

protected:
	const ZStreamW& fStreamSink;
	ZStream_MD5::Context fContext;
	uint8* fDigest;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZStream_MD5_h__
