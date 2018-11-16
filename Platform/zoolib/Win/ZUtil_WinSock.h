/* -------------------------------------------------------------------------------------------------
Copyright (c) 2013 Andrew Green
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

#ifndef __ZUtil_WinSock_h__
#define __ZUtil_WinSock_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/size_t.h" // For size_t

#include "zoolib/ZCompat_WinSock.h"

namespace ZooLib {
namespace ZUtil_WinSock {

// =================================================================================================
#pragma mark - ZUtil_WinSock

bool sWaitReadable(SOCKET iSOCKET, double iTimeout);
void sWaitWriteable(SOCKET iSOCKET);
size_t sCountReadable(SOCKET iSOCKET);

} // namespace ZUtil_WinSock
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZUtil_WinSock_h__
