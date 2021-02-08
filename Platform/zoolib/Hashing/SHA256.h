// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Hashing_SHA256_h__
#define __ZooLib_Hashing_SHA256_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/StdInt.h"
#include "zoolib/ZDebug.h"

#ifndef ZCONFIG_Hashing_SHA256_UseOpenSSL
	#if ZCONFIG_SPI_Enabled(openssl)
		#define ZCONFIG_Hashing_SHA256_UseOpenSSL 1
	#else
		#define ZCONFIG_Hashing_SHA256_UseOpenSSL 0
	#endif
#endif

#if ZCONFIG_Hashing_SHA256_UseOpenSSL
	#include <openssl/sha.h>
#endif

namespace ZooLib {
namespace Hashing {

// =================================================================================================
#pragma mark -

struct SHA256 {

static constexpr size_t kHashSize = 32;
static constexpr char kName[] = "SHA256";

#if ZCONFIG_Hashing_SHA256_UseOpenSSL

typedef SHA256_CTX Context;

#else // ZCONFIG_Hashing_SHA256_UseOpenSSL

// Our current implementation is little-endian only.

ZAssertCompile(ZCONFIG(Endian, Little));

typedef uint8 BYTE;
typedef uint32 WORD;

struct Context
	{
	BYTE data[64];
	WORD datalen;
	uint64 bitlen;
	WORD state[8];
	};

#endif // ZCONFIG_Hashing_SHA256_UseOpenSSL

static void sInit(Context& oContext);
static void sUpdate(Context& ioContext, const void* iData, size_t iCount);
static void sFinal(Context& ioContext, uint8 oDigest[kHashSize]);

}; // struct SHA256

// =================================================================================================

} // namespace Hashing
} // namespace ZooLib

#endif // __ZooLib_Hashing_SHA256_h__
