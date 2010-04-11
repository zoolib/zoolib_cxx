/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include <cstddef> // For std::size_t
#include <cstring> // For std::memcmp, std::memcpy, std::memmove and std::memset

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * memcmp, memcpy, memmove, memset, bzero wrappers.

inline int ZMemCompare(const void* iLHS, const void* iRHS, std::size_t iCount)
	{ return std::memcmp(iLHS, iRHS, iCount); }

int ZMemCompare(const void* iL, std::size_t iCountL, const void* iR, std::size_t iCountR);

inline void ZMemCopy(void* oDest, const void* iSource, std::size_t iCount)
	{ std::memcpy(oDest, iSource, iCount); }

inline void ZMemMove(void* oDest, const void* iSource, std::size_t iCount)
	{ std::memmove(oDest, iSource, iCount); }

inline void ZMemSet(void* oDest, unsigned char iValue, std::size_t iCount)
	{ std::memset(oDest, iValue, iCount); }

inline void ZMemZero(void* oDest, std::size_t iCount)
	{
	#if ZCONFIG_SPI_Enabled(POSIX)
		::bzero(oDest, iCount);
	#else
		ZMemSet(oDest, 0, iCount);
	#endif
	}

template <class T>
void ZMemZero_T(T& oT)
	{ ZMemZero(&oT, sizeof(oT)); }

NAMESPACE_ZOOLIB_END

#endif // __ZMemory__
