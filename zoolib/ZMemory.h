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

#ifndef __ZMemory__
#define __ZMemory__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#include <cstring> // Needed for memmove, memcpy and memset

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * memmove, memcpy, memset, bzero wrappers.

inline void ZMemMove(void* iDest, const void* iSource, size_t iCount)
	{ std::memmove(iDest, iSource, iCount); }

inline void ZMemCopy(void* iDest, const void* iSource, size_t iCount)
	{ std::memcpy(iDest, iSource, iCount); }

inline void ZMemSet(void* iDest, unsigned char iValue, size_t iCount)
	{ std::memset(iDest, iValue, iCount); }

inline void ZMemZero(void* iDest, size_t iCount)
	{
	#if ZCONFIG_SPI_Enabled(POSIX)
		::bzero(iDest, iCount);
	#else
		ZMemSet(iDest, 0, iCount);
	#endif
	}

template <class T>
void ZMemZero_T(T& iT)
	{ ZMemZero(&iT, sizeof(iT)); }

NAMESPACE_ZOOLIB_END

// =================================================================================================
#pragma mark -
#pragma mark * BlockMove, BlockCopy, BlockSet wrappers

// ZBlockMove, ZBlockCopy and ZBlockSet -- standardizes the API and takes advantage of any native
// performance enhacnements that may be available. Note that the parameter order is reversed from
// the C standard API. This is because these methods started off as simple wrappers around MacOS
// procedures which also had what might to you seem to be awkward ordering. Sorry.

#if 0

#define ZBlockMove(srcPtr, destPtr, byteCount) ZMemMove(destPtr, srcPtr, byteCount)
#define ZBlockCopy(srcPtr, destPtr, byteCount) ZMemCopy(destPtr, srcPtr, byteCount)

#define ZBlockSet(destPtr, byteCount, value) ZMemSet(destPtr, value, byteCount)
#define ZBlockZero(destPtr, byteCount) ZMemZero(destPtr, byteCount)

#endif

// =================================================================================================

#endif // __ZMemory__

