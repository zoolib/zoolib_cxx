// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZCompat_GdiPlus_h__
#define __ZCompat_GdiPlus_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_Win.h"

// =================================================================================================

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_algorithm.h"

namespace Gdiplus {

using std::min;
using std::max;

} // namespace Gdiplus

#include <GdiPlus.h>

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZCompat_GdiPlus_h__
