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

/*
<ftp://ftp.funet.fi/pub/crypt/hash/sha/sha1.c>

SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
	A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
	84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
	34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

#include "zoolib/ZStream_SHA1.h"

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"

NAMESPACE_ZOOLIB_USING

using std::min;

static void sCheckIt(ZStream_SHA1::Context& ioContext, const uint8 iDigest[20])
	{
	uint8 theDigest[20];
	ZStream_SHA1::sFinal(ioContext, theDigest);
	ZAssertLog(0, 0 == memcmp(theDigest, iDigest, 20));
	}

void ZStream_SHA1::sTest()
	{
	Context theContext;

	sInit(theContext);
	sUpdate(theContext, "abc", 3);
	const uint8 check1[] = {
		0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
		0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D };
	sCheckIt(theContext, check1);

	sInit(theContext);
	static const char string2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	sUpdate(theContext, string2, strlen(string2));
	const uint8 check2[] = {
		0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
		0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 };
	sCheckIt(theContext, check2);

	sInit(theContext);
	const char string3 = 'a';
	for (size_t x = 0; x < 1000000; ++x)
		sUpdate(theContext, &string3, 1);
	const uint8 check3[] = {
		0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
		0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F };
	sCheckIt(theContext, check3);
	}


// =================================================================================================

#if ZCONFIG_StreamSHA1_UseOpenSSL

void ZStream_SHA1::sInit(Context& oContext)
	{
	::SHA1_Init(&oContext);
	}

void ZStream_SHA1::sUpdate(Context& ioContext, const void* iData, size_t iCount)
	{
	::SHA1_Update(&ioContext, iData, iCount);
	}

void ZStream_SHA1::sFinal(Context& ioContext, uint8 oDigest[20])
	{
	::SHA1_Final(&oDigest[0], &ioContext);
	}

#endif // ZCONFIG_StreamSHA1_UseOpenSSL

// =================================================================================================

#if !ZCONFIG_StreamSHA1_UseOpenSSL

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */

#if ZCONFIG(Endian, Big)
#	define blk0(i) (localData[i] = iData[i])
#else
#	define blk0(i) (localData[i] = ZByteSwap_ReadBig32(iData + i))
#endif

#define blk(i) (localData[i&15] = \
	rol(localData[(i+13)&15]^localData[(i+8)&15]^localData[(i+2)&15]^localData[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


// Hash a single 512-bit block. This is the core of the algorithm.
static void sSHA1_Transform(uint32 ioState[5], const uint32 iData[16])
	{
	// Copy ioState to working vars
	uint32 a = ioState[0];
	uint32 b = ioState[1];
	uint32 c = ioState[2];
	uint32 d = ioState[3];
	uint32 e = ioState[4];

	uint32 localData[16];

	// 4 rounds of 20 operations each. Note that R0 reads from iData and
	// puts the value into localData, byteswapping from big to host if necessary.
	// The other rounds work with localData.

	R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
	R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
	R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
	R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);

	R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);

	R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
	R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
	R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
	R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
	R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);

	R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
	R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
	R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
	R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
	R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);

	R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
	R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
	R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
	R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
	R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);

	// Add the working vars back into ioState
	ioState[0] += a;
	ioState[1] += b;
	ioState[2] += c;
	ioState[3] += d;
	ioState[4] += e;
	}

void ZStream_SHA1::sInit(ZStream_SHA1::Context& oContext)
	{
	// SHA1 initialization constants.
	oContext.fState[0] = 0x67452301;
	oContext.fState[1] = 0xEFCDAB89;
	oContext.fState[2] = 0x98BADCFE;
	oContext.fState[3] = 0x10325476;
	oContext.fState[4] = 0xC3D2E1F0;
	oContext.fSpaceUsed = 0;
	oContext.fBuffersSent = 0;
	}

void ZStream_SHA1::sUpdate(ZStream_SHA1::Context& ioContext, const void* iData, size_t iCount)
	{
	const uint8* localData = static_cast<const uint8*>(iData);
	size_t countRemaining = iCount;

	while (countRemaining)
		{
		if (ioContext.fSpaceUsed == 0)
			{
			// The buffer is empty.
			if (countRemaining < 64)
				{
				// And we have less than 64 bytes to use -- just transcribe it into our buffer.
				ZBlockCopy(localData, ioContext.fBuffer8 + ioContext.fSpaceUsed, countRemaining);
				ioContext.fSpaceUsed = countRemaining;
				countRemaining = 0;
				}
			else
				{
				// We've got 64 or more bytes to use, work directly with the source material.
				sSHA1_Transform(ioContext.fState, (const uint32*)(localData));
				ioContext.fBuffersSent += 1;

				countRemaining -= 64;
				localData += 64;
				}
			}
		else
			{
			// Top up our existing buffer.
			size_t countToCopy = min(countRemaining, 64 - ioContext.fSpaceUsed);
			ZBlockCopy(localData, ioContext.fBuffer8 + ioContext.fSpaceUsed, countToCopy);
			countRemaining -= countToCopy;
			ioContext.fSpaceUsed += countToCopy;
			localData += countToCopy;
			if (ioContext.fSpaceUsed == 64)
				{
				sSHA1_Transform(ioContext.fState, ioContext.fBuffer32);
				ioContext.fBuffersSent += 1;
				ioContext.fSpaceUsed = 0;
				}
			}
		}
	}

void ZStream_SHA1::sFinal(ZStream_SHA1::Context& ioContext, uint8 oDigest[20])
	{
	// Remember the bit count.
	const uint64 finalBitCountBE
		= ZByteSwap_HostToBig64(ioContext.fBuffersSent * 512 + ioContext.fSpaceUsed * 8);

	// Pad the stream with a single 0x80.
	const uint8 pad80 = 0x80;
	sUpdate(ioContext, &pad80, 1);

	// Followed by zeroes until we've used 56 bytes, leaving 8 at the end
	// for the 64 bit count of bits.
	const uint8 pad0 = 0;
	while (ioContext.fSpaceUsed != 56)
		sUpdate(ioContext, &pad0, 1);

	// This next call will make it an even 64 bytes and cause sSHA1_Transform
	// to be called one final time.
	sUpdate(ioContext, &finalBitCountBE, 8);
	uint32* dest = reinterpret_cast<uint32*>(&oDigest[0]);
	if (ZCONFIG(Endian, Big))
		{
		for (size_t x = 0; x < 5; ++x)
			dest[x] = ioContext.fState[x];
		}
	else
		{
		for (size_t x = 0; x < 5; ++x)
			ZByteSwap_WriteBig32(dest++, ioContext.fState[x]);
		}
	}

#endif // !ZCONFIG_StreamSHA1_UseOpenSSL

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_SHA1

/**
\class ZStreamR_SHA1
*/

ZStreamR_SHA1::ZStreamR_SHA1(const ZStreamR& iStreamSource)
:	ZStreamR_Filter(iStreamSource),
	fStreamSource(iStreamSource),
	fDigest(nil)
	{
	ZStream_SHA1::sInit(fContext);
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
uint8 theDigest[20];
ZStreamR_SHA1(theDigest, sourceStream).Read(dest, destSize);
\endcode
*/
ZStreamR_SHA1::ZStreamR_SHA1(uint8 oDigest[20], const ZStreamR& iStreamSource)
:	ZStreamR_Filter(iStreamSource),
	fStreamSource(iStreamSource),
	fDigest(oDigest)
	{ ZStream_SHA1::sInit(fContext); }

ZStreamR_SHA1::~ZStreamR_SHA1()
	{
	if (fDigest)
		ZStream_SHA1::sFinal(fContext, fDigest);
	}

void ZStreamR_SHA1::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRead;
	fStreamSource.Read(iDest, iCount, &countRead);
	if (countRead)
		ZStream_SHA1::sUpdate(fContext, iDest, countRead);
	if (oCountRead)
		*oCountRead = countRead;
	}

void ZStreamR_SHA1::GetDigest(uint8 oDigest[20])
	{
	ZStream_SHA1::Context tempContext = fContext;
	ZStream_SHA1::sFinal(tempContext, oDigest);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_SHA1

/**
\class ZStreamW_SHA1
*/

ZStreamW_SHA1::ZStreamW_SHA1(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fDigest(nil)
	{ ZStream_SHA1::sInit(fContext); }

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
uint8 theDigest[20];
ZStreamW_SHA1(theDigest, destStream).Write(source, sourceSize);
\endcode

It can also be used to simply digest a block of data in memory, by passing a temporary
ZStreamW_Null as the destination stream:
\code
uint8 theDigest[20];
ZStreamW_SHA1(theDigest, ZStreamW_Null()).Write(source, sourceSize);
\endcode
*/
ZStreamW_SHA1::ZStreamW_SHA1(uint8 oDigest[20], const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fDigest(oDigest)
	{
	ZStream_SHA1::sInit(fContext);
	}

ZStreamW_SHA1::~ZStreamW_SHA1()
	{
	if (fDigest)
		ZStream_SHA1::sFinal(fContext, fDigest);
	}

void ZStreamW_SHA1::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t countWritten;
	fStreamSink.Write(iSource, iCount, &countWritten);
	if (countWritten)
		ZStream_SHA1::sUpdate(fContext, iSource, countWritten);
	if (oCountWritten)
		*oCountWritten = countWritten;
	}

void ZStreamW_SHA1::Imp_Flush()
	{ fStreamSink.Flush(); }

void ZStreamW_SHA1::GetDigest(uint8 oDigest[20])
	{
	ZStream_SHA1::Context tempContext = fContext;
	ZStream_SHA1::sFinal(tempContext, oDigest);
	}
