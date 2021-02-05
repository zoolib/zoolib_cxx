// Copyright (c) 2002-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Hashing_SHA1_h__
#define __ZooLib_Hashing_SHA1_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/StdInt.h"

#ifndef ZCONFIG_Hashing_SHA1_UseOpenSSL
	#if ZCONFIG_SPI_Enabled(openssl)
		#define ZCONFIG_Hashing_SHA1_UseOpenSSL 1
	#else
		#define ZCONFIG_Hashing_SHA1_UseOpenSSL 0
	#endif
#endif

#if ZCONFIG_Hashing_SHA1_UseOpenSSL
	#include <openssl/sha.h>
#endif

namespace ZooLib {
namespace Hashing {
namespace SHA1 {

// =================================================================================================
#pragma mark -

constexpr size_t kHashSize = 20;

#if ZCONFIG_Hashing_SHA1_UseOpenSSL

typedef SHA_CTX Context;

#else // ZCONFIG_Hashing_SHA1_UseOpenSSL

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

#endif // ZCONFIG_Hashing_SHA1_UseOpenSSL

void sInit(Context& oContext);
void sUpdate(Context& ioContext, const void* iData, size_t iCount);
void sFinal(Context& ioContext, uint8 oDigest[kHashSize]);

// =================================================================================================

} // namespace SHA1
} // namespace Hashing
} // namespace ZooLib

#endif // __ZooLib_Hashing_SHA1_h__
