// Copyright (c) 2009-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_POSIXFD_h__
#define __ZooLib_Util_POSIXFD_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include <sys/select.h> // For fd_set

#include "zoolib/StdInt.h" // For uint64 and size_t

namespace ZooLib {
namespace Util_POSIXFD {

// =================================================================================================
#pragma mark - Util_POSIXFD

void sSetNonBlocking(int iFD);

void sSetup(fd_set& oSet, int iFD);

bool sWaitReadable(int iFD, double iTimeout);
bool sWaitWriteable(int iFD, double iTimeout);

void sClose(int iFD);

void sAbort(int iFD);
uint64 sPos(int iFD);
void sPosSet(int iFD, uint64 iPos);
uint64 sSkip(int iFD, uint64 iDelta);
size_t sRead(int iFD, byte* oDest, size_t iCount);
size_t sReadCon(int iFD, byte* oDest, size_t iCount);
size_t sReadable(int iFD);
uint64 sSize(int iFD);
void sSizeSet(int iFD, uint64 iSize);
size_t sUnread(int iFD, const byte* iSource, size_t iCount);
size_t sUnreadableLimit(int iFD);
size_t sWrite(int iFD, const byte* iSource, size_t iCount);
size_t sWriteCon(int iFD, const byte* iSource, size_t iCount);

} // namespace Util_POSIXFD
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)

#endif // __ZooLib_Util_POSIXFD_h__
