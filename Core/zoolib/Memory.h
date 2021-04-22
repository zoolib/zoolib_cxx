// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Memory_h__
#define __ZooLib_Memory_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#include <cstring> // For std::memcmp, std::memcpy, std::memmove and std::memset

#if defined(__ANDROID__)
	// On Android we get bzero from strings.h.
	#include <strings.h>
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - memcmp, memcpy, memmove, memset, bzero wrappers.

inline int sMemCompare(const void* iLHS, const void* iRHS, size_t iCount)
	{ return std::memcmp(iLHS, iRHS, iCount); }

inline int sMemCompare(const void* iL, size_t iCountL, const void* iR, size_t iCountR)
	{
	if (iCountL < iCountR)
		{
		if (int compare = std::memcmp(iL, iR, iCountL))
			return compare;
		return -1;
		}
	else if (iCountL > iCountR)
		{
		if (int compare = std::memcmp(iL, iR, iCountR))
			return compare;
		return 1;
		}
	else
		{
		return std::memcmp(iL, iR, iCountL);
		}
	}

inline void sMemCopy(void* oDest, const void* iSource, size_t iCount)
	{ std::memcpy(oDest, iSource, iCount); }

inline void sMemMove(void* oDest, const void* iSource, size_t iCount)
	{ std::memmove(oDest, iSource, iCount); }

inline void sMemSet(void* oDest, unsigned char iValue, size_t iCount)
	{ std::memset(oDest, iValue, iCount); }

inline void sMemZero(void* oDest, size_t iCount)
	{
	#if ZCONFIG_SPI_Enabled(POSIX)
		// bzero is not always a function, it may be a macro, so we must not namespace-qualify it.
		bzero(oDest, iCount);
	#else
		sMemSet(oDest, 0, iCount);
	#endif
	}

template <class T>
void sMemZero_T(T& oT)
	{ sMemZero(&oT, sizeof(oT)); }

} // namespace ZooLib

#endif // __ZooLib_Memory_h__
