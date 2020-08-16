// Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZUtil_Win_h__
#define __ZUtil_Win_h__
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZQ.h"

#if ZCONFIG_SPI_Enabled(Win)

namespace ZooLib {
namespace ZUtil_Win {

bool sIsWinNT();
bool sIsWin95OSR2();
bool sIsVistaOrLater();

bool sIsUserAdmin();

bool sUseWAPI();

void sDisallowWAPI();

HINSTANCE sGetModuleHandle();

ZQ<__uint64> sQGetVersion_File(const WCHAR* iPath);

} // namespace ZUtil_Win
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
#endif // __ZUtil_Win_h__
