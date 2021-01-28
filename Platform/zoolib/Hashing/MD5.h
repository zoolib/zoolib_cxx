// Copyright (c) 2002-2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Hashing_MD5_h__
#define __ZooLib_Hashing_MD5_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/StdInt.h"

#ifndef ZCONFIG_Hashing_MD5_UseOpenSSL
	#if ZCONFIG_SPI_Enabled(openssl)
		#define ZCONFIG_Hashing_MD5_UseOpenSSL 1
	#else
		#define ZCONFIG_Hashing_MD5_UseOpenSSL 0
	#endif
#endif

#if ZCONFIG_Hashing_MD5_UseOpenSSL
	#include <openssl/md5.h>
#endif

namespace ZooLib {
namespace Hashing {
namespace MD5 {

// =================================================================================================
#pragma mark -

#if ZCONFIG_Hashing_MD5_UseOpenSSL

typedef MD5_CTX Context;

#else // ZCONFIG_Hashing_MD5_UseOpenSSL

struct Context
	{
	uint32 buf[4];
	uint32 bits[2];
	unsigned char in[64];
	};

#endif // ZCONFIG_Hashing_MD5_UseOpenSSL

void sInit(Context& oContext);
void sUpdate(Context& ioContext, const void* iData, size_t iCount);
void sFinal(Context& ioContext, uint8 oDigest[16]);

// =================================================================================================

} // namespace MD5
} // namespace Hashing
} // namespace ZooLib

#endif // __ZStream_MD5_h__
