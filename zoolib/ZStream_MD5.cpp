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

#include "zoolib/ZStream_MD5.h"

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"

NAMESPACE_ZOOLIB_USING

using std::min;

// =================================================================================================

#if ZCONFIG_StreamMD5_UseOpenSSL

void ZStream_MD5::sInit(Context& oContext)
	{
	::MD5_Init(&oContext);
	}

void ZStream_MD5::sUpdate(Context& ioContext, const void* iData, size_t iCount)
	{
	::MD5_Update(&ioContext, iData, iCount);
	}

void ZStream_MD5::sFinal(Context& ioContext, uint8 oDigest[16])
	{
	::MD5_Final(&oDigest[0], &ioContext);
	}

#endif // ZCONFIG_StreamMD5_UseOpenSSL

// =================================================================================================

#if !ZCONFIG_StreamMD5_UseOpenSSL

#include <cstring> // This is needed for memmove, memcpy and memset

// The four core functions - F1 is optimized somewhat

// #define F1(x, y, z) (x & y | ~x & z)
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))


// This is the central step in the MD5 algorithm.
#if ZCONFIG(Endian, Big)
#	if ZCONFIG(Processor, PPC) && ZCONFIG(Compiler, CodeWarrior)
		// This variant is by Conrad Weyns. It generates fewer instructions and
		// doesn't require more registers than CW is able to manage.
#		define MD5STEP1(f, w, x, y, z, i, c, s) \
			valueAddress = iData + i; \
			asm { lwbrx swapResult, r0, valueAddress }; \
			localData[i] = swapResult; \
			w += f(x, y, z) + c + swapResult; \
			w = w<<s | w>>(32-s); \
			w += x
#	else
#		define MD5STEP1(f, w, x, y, z, i, c, s) \
			( w += f(x, y, z) + c + (localData[i] = ZByteSwap_Read32(iData + i)),\
			w = w<<s | w>>(32-s),  w += x )
#	endif
#	define MD5STEP2(f, w, x, y, z, i, c, s) \
		( w += f(x, y, z) + c + localData[i],  w = w<<s | w>>(32-s),  w += x )

#else
#	define MD5STEP1(f, w, x, y, z, i, c, s) \
		( w += f(x, y, z) + c + iData[i],  w = w<<s | w>>(32-s),  w += x )

#	define MD5STEP2 MD5STEP1

#endif


static void  sMD5_Transform(uint32 ioState[4], const uint32 iData[16])
	{
	uint32 a = ioState[0];
	uint32 b = ioState[1];
	uint32 c = ioState[2];
	uint32 d = ioState[3];

	#if ZCONFIG(Endian, Big)
	uint32 	localData[16];
	#endif

	#if ZCONFIG(Endian, Big) && ZCONFIG(Processor, PPC) && ZCONFIG(Compiler, CodeWarrior)
	register const void* valueAddress;
	register int32 swapResult;
	#endif


	MD5STEP1(F1, a, b, c, d, 0, 0xd76aa478, 7);
	MD5STEP1(F1, d, a, b, c, 1, 0xe8c7b756, 12);
	MD5STEP1(F1, c, d, a, b, 2, 0x242070db, 17);
	MD5STEP1(F1, b, c, d, a, 3, 0xc1bdceee, 22);
	MD5STEP1(F1, a, b, c, d, 4, 0xf57c0faf, 7);
	MD5STEP1(F1, d, a, b, c, 5, 0x4787c62a, 12);
	MD5STEP1(F1, c, d, a, b, 6, 0xa8304613, 17);
	MD5STEP1(F1, b, c, d, a, 7, 0xfd469501, 22);
	MD5STEP1(F1, a, b, c, d, 8, 0x698098d8, 7);
	MD5STEP1(F1, d, a, b, c, 9, 0x8b44f7af, 12);
	MD5STEP1(F1, c, d, a, b, 10, 0xffff5bb1, 17);
	MD5STEP1(F1, b, c, d, a, 11, 0x895cd7be, 22);
	MD5STEP1(F1, a, b, c, d, 12, 0x6b901122, 7);
	MD5STEP1(F1, d, a, b, c, 13, 0xfd987193, 12);
	MD5STEP1(F1, c, d, a, b, 14, 0xa679438e, 17);
	MD5STEP1(F1, b, c, d, a, 15, 0x49b40821, 22);

	MD5STEP2(F2, a, b, c, d, 1, 0xf61e2562, 5);
	MD5STEP2(F2, d, a, b, c, 6, 0xc040b340, 9);
	MD5STEP2(F2, c, d, a, b, 11, 0x265e5a51, 14);
	MD5STEP2(F2, b, c, d, a, 0, 0xe9b6c7aa, 20);
	MD5STEP2(F2, a, b, c, d, 5, 0xd62f105d, 5);
	MD5STEP2(F2, d, a, b, c, 10, 0x02441453, 9);
	MD5STEP2(F2, c, d, a, b, 15, 0xd8a1e681, 14);
	MD5STEP2(F2, b, c, d, a, 4, 0xe7d3fbc8, 20);
	MD5STEP2(F2, a, b, c, d, 9, 0x21e1cde6, 5);
	MD5STEP2(F2, d, a, b, c, 14, 0xc33707d6, 9);
	MD5STEP2(F2, c, d, a, b, 3, 0xf4d50d87, 14);
	MD5STEP2(F2, b, c, d, a, 8, 0x455a14ed, 20);
	MD5STEP2(F2, a, b, c, d, 13, 0xa9e3e905, 5);
	MD5STEP2(F2, d, a, b, c, 2, 0xfcefa3f8, 9);
	MD5STEP2(F2, c, d, a, b, 7, 0x676f02d9, 14);
	MD5STEP2(F2, b, c, d, a, 12, 0x8d2a4c8a, 20);

	MD5STEP2(F3, a, b, c, d, 5, 0xfffa3942, 4);
	MD5STEP2(F3, d, a, b, c, 8, 0x8771f681, 11);
	MD5STEP2(F3, c, d, a, b, 11, 0x6d9d6122, 16);
	MD5STEP2(F3, b, c, d, a, 14, 0xfde5380c, 23);
	MD5STEP2(F3, a, b, c, d, 1, 0xa4beea44, 4);
	MD5STEP2(F3, d, a, b, c, 4, 0x4bdecfa9, 11);
	MD5STEP2(F3, c, d, a, b, 7, 0xf6bb4b60, 16);
	MD5STEP2(F3, b, c, d, a, 10, 0xbebfbc70, 23);
	MD5STEP2(F3, a, b, c, d, 13, 0x289b7ec6, 4);
	MD5STEP2(F3, d, a, b, c, 0, 0xeaa127fa, 11);
	MD5STEP2(F3, c, d, a, b, 3, 0xd4ef3085, 16);
	MD5STEP2(F3, b, c, d, a, 6, 0x04881d05, 23);
	MD5STEP2(F3, a, b, c, d, 9, 0xd9d4d039, 4);
	MD5STEP2(F3, d, a, b, c, 12, 0xe6db99e5, 11);
	MD5STEP2(F3, c, d, a, b, 15, 0x1fa27cf8, 16);
	MD5STEP2(F3, b, c, d, a, 2, 0xc4ac5665, 23);

	MD5STEP2(F4, a, b, c, d, 0, 0xf4292244, 6);
	MD5STEP2(F4, d, a, b, c, 7, 0x432aff97, 10);
	MD5STEP2(F4, c, d, a, b, 14, 0xab9423a7, 15);
	MD5STEP2(F4, b, c, d, a, 5, 0xfc93a039, 21);
	MD5STEP2(F4, a, b, c, d, 12, 0x655b59c3, 6);
	MD5STEP2(F4, d, a, b, c, 3, 0x8f0ccc92, 10);
	MD5STEP2(F4, c, d, a, b, 10, 0xffeff47d, 15);
	MD5STEP2(F4, b, c, d, a, 1, 0x85845dd1, 21);
	MD5STEP2(F4, a, b, c, d, 8, 0x6fa87e4f, 6);
	MD5STEP2(F4, d, a, b, c, 15, 0xfe2ce6e0, 10);
	MD5STEP2(F4, c, d, a, b, 6, 0xa3014314, 15);
	MD5STEP2(F4, b, c, d, a, 13, 0x4e0811a1, 21);
	MD5STEP2(F4, a, b, c, d, 4, 0xf7537e82, 6);
	MD5STEP2(F4, d, a, b, c, 11, 0xbd3af235, 10);
	MD5STEP2(F4, c, d, a, b, 2, 0x2ad7d2bb, 15);
	MD5STEP2(F4, b, c, d, a, 9, 0xeb86d391, 21);

	ioState[0] += a;
	ioState[1] += b;
	ioState[2] += c;
	ioState[3] += d;
	}

void ZStream_MD5::sInit(ZStream_MD5::Context& oContext)
	{
	oContext.fState[0] = 0x67452301;
	oContext.fState[1] = 0xefcdab89;
	oContext.fState[2] = 0x98badcfe;
	oContext.fState[3] = 0x10325476;

	oContext.fSpaceUsed = 0;
	oContext.fBuffersSent = 0;
	}

void ZStream_MD5::sUpdate(ZStream_MD5::Context& ioContext, const void* iData, size_t iCount)
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
				sMD5_Transform(ioContext.fState, (const uint32*)(localData));
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
				sMD5_Transform(ioContext.fState, ioContext.fBuffer32);
				ioContext.fBuffersSent += 1;
				ioContext.fSpaceUsed = 0;
				}
			}
		}
	}

void ZStream_MD5::sFinal(ZStream_MD5::Context& ioContext, uint8 oDigest[16])
	{
	const uint64 finalBitCountBE =
		ZByteSwap_HostToBig64(512 * ioContext.fBuffersSent + ioContext.fSpaceUsed * 8);

	// Pad the stream with 0x80
	const uint8 pad80 = 0x80;
	sUpdate(ioContext, &pad80, 1);

	// Followed by zeroes until we've used 56 bytes, leaving 8 at the
	// end for the 64 bit count of bits.
	const uint8 pad0 = 0;
	while (ioContext.fSpaceUsed != 56)
		sUpdate(ioContext, &pad0, 1);

	// This next call will make it an even 64 bytes and cause sMD5_Transform
	// to be called one final time.
	sUpdate(ioContext, &finalBitCountBE, 8);
	uint32* dest = reinterpret_cast<uint32*>(&oDigest[0]);
	if ZCONFIG(Endian, Big)
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

#endif // !ZCONFIG_StreamMD5_UseOpenSSL

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_MD5

ZStreamR_MD5::ZStreamR_MD5(const ZStreamR& iStreamSource)
:	ZStreamR_Filter(iStreamSource),
	fStreamSource(iStreamSource),
	fDigest(nil)
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

void ZStreamR_MD5::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRead;
	fStreamSource.Read(iDest, iCount, &countRead);
	if (countRead)
		ZStream_MD5::sUpdate(fContext, iDest, countRead);
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
#pragma mark * ZStreamW_MD5

ZStreamW_MD5::ZStreamW_MD5(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fDigest(nil)
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
