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

/*
The algorithm is due to Ron Rivest, the actual implementation of the
MD5 message-digest algorithm used here was written by Colin Plumb
in 1993, for which no copyright is claimed.
*/

#include "zoolib/Memory.h"

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZStream_MD5.h"

#include <string.h> // Needed for memcpy and memset

using std::min;

namespace ZooLib {

// =================================================================================================

#if !ZCONFIG_StreamMD5_UseOpenSSL

/* little-endian word access macros */
#define GET_32BIT_LSB_FIRST(cp) \
    (((uint32)(unsigned char)(cp)[0]) | \
     ((uint32)(unsigned char)(cp)[1] << 8 ) | \
     ((uint32)(unsigned char)(cp)[2] << 16) | \
     ((uint32)(unsigned char)(cp)[3] << 24))

#define PUT_32BIT_LSB_FIRST(cp, value) \
    do { \
	(cp)[0] = (value) & 0xFF; \
	(cp)[1] = ((value) >> 8)  & 0xFF; \
	(cp)[2] = ((value) >> 16) & 0xFF; \
	(cp)[3] = ((value) >> 24) & 0xFF; \
    } while(0)

static void MD5Transform(uint32 buf[4], const unsigned char inext[64], ZStream_MD5::Context *ctx);

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
static void MD5_Init(ZStream_MD5::Context *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;

	memset(ctx->in, 0, 64);
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static void MD5_Update(ZStream_MD5::Context *ctx, unsigned char const *buf, uint32 len)
{
    uint32 t;

    /* Update bitcount */

    t = ctx->bits[0];
    if ((ctx->bits[0] = (t + ((uint32)len << 3)) & 0xffffffff) < t)
	ctx->bits[1]++;		/* Carry from low to high */
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;	/* Bytes already in shsInfo->data */

    /* Handle any leading odd-sized chunks */

    if (t) {
	unsigned char *p = ctx->in + t;

	t = 64 - t;
	if (len < t) {
	    memcpy(p, buf, len);
	    return;
	}
	memcpy(p, buf, t);
	MD5Transform(ctx->buf, ctx->in, ctx);
	buf += t;
	len -= t;
    }
    /* Process data in 64-byte chunks */

    while (len >= 64) {
	memcpy(ctx->in, buf, 64);
	MD5Transform(ctx->buf, ctx->in, ctx);
	buf += 64;
	len -= 64;
    }

    /* Handle any remaining bytes of data. */

    memcpy(ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static void MD5_Final(unsigned char digest[16], ZStream_MD5::Context *ctx)
{
    unsigned count;
    unsigned char *p;

    /* Compute number of bytes mod 64 */
    count = (ctx->bits[0] >> 3) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */
    p = ctx->in + count;
    *p++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if (count < 8) {
	/* Two lots of padding:  Pad the first block to 64 bytes */
	memset(p, 0, count);
	MD5Transform(ctx->buf, ctx->in, ctx);

	/* Now fill the next block with 56 bytes */
	memset(ctx->in, 0, 56);
    } else {
	/* Pad block to 56 bytes */
	memset(p, 0, count - 8);
    }

    /* Append length in bits and transform */
    PUT_32BIT_LSB_FIRST(ctx->in + 56, ctx->bits[0]);
    PUT_32BIT_LSB_FIRST(ctx->in + 60, ctx->bits[1]);

    MD5Transform(ctx->buf, ctx->in, ctx);
    PUT_32BIT_LSB_FIRST(digest, ctx->buf[0]);
    PUT_32BIT_LSB_FIRST(digest + 4, ctx->buf[1]);
    PUT_32BIT_LSB_FIRST(digest + 8, ctx->buf[2]);
    PUT_32BIT_LSB_FIRST(digest + 12, ctx->buf[3]);
    memset(ctx, 0, sizeof(*ctx));	/* In case it's sensitive */
}

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
/* debugging version: */
/*
#define MD5STEP(f, w, x, y, z, data, s) \
	printf("MD5STEP:  w: %x x: %x y: %x z: %x data: %x s: %x\n", 	\
		w, x, y, z, data, s); 	\
	printf("f(x,y,z) = %x\n", f(x,y,z)+data); \
	( w += f(x, y, z) + data,  printf(" - w: %x ", w), \
	w = w<<s | w>>(32-s),  printf(" - w: %x\n", w), w += x )
*/
#define MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data, w = w<<s | w>>(32-s), w += x )

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void MD5Transform(uint32 buf[4], const unsigned char inext[64], ZStream_MD5::Context *ctx)
{
    register uint32 a, b, c, d, i;
    uint32 in[16];

    for (i = 0; i < 16; i++)
      in[i] = GET_32BIT_LSB_FIRST(inext + 4 * i);

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);

    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

#endif // !ZCONFIG_StreamMD5_UseOpenSSL

// =================================================================================================
#pragma mark -
#pragma mark ZStream_MD5

void ZStream_MD5::sInit(Context& oContext)
	{ MD5_Init(&oContext); }

void ZStream_MD5::sUpdate(Context& ioContext, const void* iData, size_t iCount)
	{ MD5_Update(&ioContext, static_cast<const unsigned char*>(iData), iCount); }

void ZStream_MD5::sFinal(Context& ioContext, uint8 oDigest[16])
	{ MD5_Final(&oDigest[0], &ioContext); }

// =================================================================================================
#pragma mark -
#pragma mark ZStreamR_MD5

ZStreamR_MD5::ZStreamR_MD5(const ZStreamR& iStreamSource)
:	ZStreamR_Filter(iStreamSource),
	fStreamSource(iStreamSource),
	fDigest(nullptr)
	{
	ZStream_MD5::sInit(fContext);
	}

/// Constructor for use inline.
/**
This constructor remembers the address of \a oDigest, and when destroyed will place
the digest in that array. This constructor is useful when a ZStreamR is
already being used to read some data, but for which we would like to gather
a digest. e.g. going from this:
\code
sourceStream.Read(dest, destSize);
\endcode
to this:
\code
uint8 theDigest[16];
ZStreamR_MD5(theDigest, sourceStream).Read(dest, destSize);
\endcode
*/
ZStreamR_MD5::ZStreamR_MD5(uint8 oDigest[16], const ZStreamR& iStreamSource)
:	ZStreamR_Filter(iStreamSource),
	fStreamSource(iStreamSource),
	fDigest(oDigest)
	{ ZStream_MD5::sInit(fContext); }

ZStreamR_MD5::~ZStreamR_MD5()
	{
	if (fDigest)
		ZStream_MD5::sFinal(fContext, fDigest);
	}

void ZStreamR_MD5::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRead;
	fStreamSource.Read(oDest, iCount, &countRead);
	if (countRead)
		ZStream_MD5::sUpdate(fContext, oDest, countRead);
	if (oCountRead)
		*oCountRead = countRead;
	}

void ZStreamR_MD5::GetDigest(uint8 oDigest[16])
	{
	ZStream_MD5::Context tempContext = fContext;
	ZStream_MD5::sFinal(tempContext, oDigest);
	}

// =================================================================================================
#pragma mark -
#pragma mark ZStreamW_MD5

ZStreamW_MD5::ZStreamW_MD5(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fDigest(nullptr)
	{ ZStream_MD5::sInit(fContext); }

/// Constructor for use inline.
/**
This constructor remembers the address of \a oDigest, and when destroyed will place
the digest in that array. This constructor is useful when a ZStreamW is
already being used to write some data, but for which we would like to gather
a digest. e.g. going from this:
\code
destStream.Write(source, sourceSize);
\endcode
to this:
\code
uint8 theDigest[16];
ZStreamW_MD5(theDigest, destStream).Write(source, sourceSize);
\endcode

It can also be used to simply digest a block of data in memory, by passing a temporary
ZStreamW_Null as the destination stream:
\code
uint8 theDigest[16];
ZStreamW_MD5(theDigest, ZStreamW_Null()).Write(source, sourceSize);
\endcode
*/
ZStreamW_MD5::ZStreamW_MD5(uint8 oDigest[16], const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fDigest(oDigest)
	{ ZStream_MD5::sInit(fContext); }

ZStreamW_MD5::~ZStreamW_MD5()
	{
	if (fDigest)
		ZStream_MD5::sFinal(fContext, fDigest);
	}

void ZStreamW_MD5::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t countWritten;
	fStreamSink.Write(iSource, iCount, &countWritten);
	if (countWritten)
		ZStream_MD5::sUpdate(fContext, iSource, countWritten);
	if (oCountWritten)
		*oCountWritten = countWritten;
	}

void ZStreamW_MD5::Imp_Flush()
	{ fStreamSink.Flush(); }

void ZStreamW_MD5::GetDigest(uint8 oDigest[16])
	{
	ZStream_MD5::Context tempContext = fContext;
	ZStream_MD5::sFinal(tempContext, oDigest);
	}

} // namespace ZooLib
