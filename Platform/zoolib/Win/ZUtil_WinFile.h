// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZUtil_WinFile_h__
#define __ZUtil_WinFile_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/size_t.h" // For size_t

#include "zoolib/ZCompat_Win.h"

namespace ZooLib {
namespace ZUtil_WinFile {

// =================================================================================================
#pragma mark - ZUtil_WinFile

#if 0
bool sWaitReadable(HANDLE iHANDLE, double iTimeout);
void sWaitWriteable(HANDLE iHANDLE);
size_t sCountReadable(HANDLE iHANDLE);
#endif

DWORD sFlush(HANDLE iHANDLE);

DWORD sRead(HANDLE iHANDLE, void* oDest, size_t iCount, size_t* oCountRead);
DWORD sWrite(HANDLE iHANDLE, const void* iSource, size_t iCount, size_t* oCountWritten);

DWORD sRead(HANDLE iHANDLE, const uint64* iOffset,
	void* oDest, size_t iCount, size_t* oCountRead);

DWORD sWrite(HANDLE iHANDLE, const uint64* iOffset,
	const void* iSource, size_t iCount, size_t* oCountWritten);

DWORD sRead(HANDLE iHANDLE, const uint64* iOffset, HANDLE iEvent,
	void* oDest, size_t iCount, size_t* oCountRead);

DWORD sWrite(HANDLE iHANDLE, const uint64* iOffset, HANDLE iEvent,
	const void* iSource, size_t iCount, size_t* oCountWritten);

} // namespace ZUtil_WinFile
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZUtil_WinFile_h__
