// Copyright (c) 2002-2020 Andrew Green. MIT License. http://www.zoolib.org

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

#include "zoolib/Hashing/SHA1.h"

#include "zoolib/ByteSwap.h"
#include "zoolib/Compat_algorithm.h" // for min
#include "zoolib/Memory.h" // for sMemCopy

namespace ZooLib {
namespace Hashing {

// =================================================================================================
#pragma mark - OpenSSL

#if ZCONFIG_Hashing_SHA1_UseOpenSSL

void SHA1::sInit(SHA1::Context& oContext)
	{ ::SHA1_Init(&oContext); }

void SHA1::sUpdate(SHA1::Context& ioContext, const void* iData, size_t iCount)
	{ ::SHA1_Update(&ioContext, iData, iCount); }

void SHA1::sFinal(SHA1::Context& ioContext, uint8 oDigest[20])
	{ ::SHA1_Final(&oDigest[0], &ioContext); }

#endif // ZCONFIG_Hashing_SHA1_UseOpenSSL

// =================================================================================================
#pragma mark - Steve Reid

#if not ZCONFIG_Hashing_SHA1_UseOpenSSL

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */

#if ZCONFIG(Endian, Big)
	#define blk0(i) (localData[i] = iData[i])
#else
	#define blk0(i) (localData[i] = sByteSwapped(iData[i]))
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
static void spSHA1_Transform(uint32 ioState[5], const uint32 iData[16])
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

void SHA1::sInit(SHA1::Context& oContext)
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

void SHA1::sUpdate(SHA1::Context& ioContext, const void* iData, size_t iCount)
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
				sMemCopy(ioContext.fBuffer8 + ioContext.fSpaceUsed, localData, countRemaining);
				ioContext.fSpaceUsed = countRemaining;
				countRemaining = 0;
				}
			else
				{
				// We've got 64 or more bytes to use, work directly with the source material.
				spSHA1_Transform(ioContext.fState, (const uint32*)(localData));
				ioContext.fBuffersSent += 1;

				countRemaining -= 64;
				localData += 64;
				}
			}
		else
			{
			// Top up our existing buffer.
			size_t countToCopy = std::min(countRemaining, 64 - ioContext.fSpaceUsed);
			sMemCopy(ioContext.fBuffer8 + ioContext.fSpaceUsed, localData, countToCopy);
			countRemaining -= countToCopy;
			ioContext.fSpaceUsed += countToCopy;
			localData += countToCopy;
			if (ioContext.fSpaceUsed == 64)
				{
				spSHA1_Transform(ioContext.fState, ioContext.fBuffer32);
				ioContext.fBuffersSent += 1;
				ioContext.fSpaceUsed = 0;
				}
			}
		}
	}

void SHA1::sFinal(SHA1::Context& ioContext, uint8 oDigest[20])
	{
	// Remember the bit count.
	const uint64 finalBitCountBE =

	#if ZCONFIG_Endian == ZCONFIG_Endian_Big
		ioContext.fBuffersSent * 512 + ioContext.fSpaceUsed * 8;
	#else
		sByteSwapped(ioContext.fBuffersSent * 512 + ioContext.fSpaceUsed * 8);
	#endif

	// Pad the stream with a single 0x80.
	const uint8 pad80 = 0x80;
	sUpdate(ioContext, &pad80, 1);

	// Followed by zeroes until we've used 56 bytes, leaving 8 at the end
	// for the 64 bit count of bits.
	const uint8 pad0 = 0;
	while (ioContext.fSpaceUsed != 56)
		sUpdate(ioContext, &pad0, 1);

	// This next call will make it an even 64 bytes and cause spSHA1_Transform
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
			{
			#if ZCONFIG_Endian == ZCONFIG_Endian_Big
				*dest++ = ioContext.fState[x];
			#else
				*dest++ = sByteSwapped(ioContext.fState[x]);
			#endif
			}
		}
	}

#endif // not ZCONFIG_Hashing_SHA1_UseOpenSSL

} // namespace Hashing
} // namespace ZooLib
