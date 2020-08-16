// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZWinRegistry_Yad_h__
#define __ZWinRegistry_Yad_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZWinRegistry_Val.h"
#include "zoolib/ZYad.h"

namespace ZooLib {
namespace ZWinRegistry {

// =================================================================================================
#pragma mark - ZWinRegistry

ZRef<ZYadR> sYadR(const Val& iVal);

ZRef<ZYadMapRPos> sYadR(const KeyRef& iKeyRef);

} // namespace ZWinRegistry
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinRegistry_Yad_h__
