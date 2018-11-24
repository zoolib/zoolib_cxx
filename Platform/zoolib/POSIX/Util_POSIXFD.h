/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZooLib_Util_POSIXFD_h__
#define __ZooLib_Util_POSIXFD_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include <sys/select.h> // For fd_set

#include "zoolib/ZStdInt.h" // For uint64 and size_t

namespace ZooLib {
namespace Util_POSIXFD {

// =================================================================================================
#pragma mark - Util_POSIXFD

void sSetup(fd_set& oSet, int iFD);

bool sWaitReadable(int iFD, double iTimeout);
bool sWaitWriteable(int iFD, double iTimeout);

void sAbort(int iFD);
uint64 sPos(int iFD);
void sPosSet(int iFD, uint64 iPos);
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
