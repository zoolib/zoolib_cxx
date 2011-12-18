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

#ifndef __ZUtil_Win__
#define __ZUtil_Win__
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZStdInt.h" // For uint64

#if ZCONFIG_SPI_Enabled(Win)

namespace ZooLib {
namespace ZUtil_Win {

bool sIsWinNT();
bool sIsWin95OSR2();
bool sUseWAPI();

void sDisallowWAPI();

HINSTANCE sGetModuleHandle();

ZQ<uint64> sQGetVersion_File(const WCHAR* iPath);

} // namespace ZUtil_Win
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
#endif // __ZUtil_Win__
