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

#ifndef __ZUtil_WinFile__
#define __ZUtil_WinFile__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZStdInt.h" // For size_t

namespace ZooLib {
namespace ZUtil_WinFile {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_WinFile

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

#endif // __ZUtil_WinFile__
